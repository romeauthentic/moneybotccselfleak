#include <Security/RuntimeSecurity.hpp>
#include <Security/SyscallManager.hpp>
#include <Security/FnvHash.hpp>

#include <UserExperience/UserInterface.hpp>
#include <Networking/TCPClient.hpp>

// Global accessor to security instance.
Security::RuntimeSecurityPtr Protection = std::make_unique<Security::RuntimeSecurity>();


namespace Security
{
	// Hooked functions.
	///////////////////////////////////////////////////////////

#pragma optimize("", off)

	decltype(&OpenProcess) oOpenProcess;
	HANDLE __stdcall Hooked_OpenProcess(DWORD AccessLevel, bool Inherit, DWORD ProcessId)
	{
		// Determine where the return address of the function actually points.
		void *Address                  = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		if(ReturnModule != LoaderModule)
		{
			Protection->SecurityCallback(STR("Malicious activity [Tampering]."));

			[&](decltype(&OpenProcess) A)
			{
				// Again, let's meme anyone who tries to reverse this.
				uintptr_t NullPointer = *(uintptr_t *)0x00000000;
				A(NullPointer, NullPointer, NullPointer);
			}(OpenProcess);
		}

		// Call original function
		return oOpenProcess(AccessLevel, Inherit, ProcessId);
	}

	
	decltype(&FindWindowA) oFindWindow;
	HWND __stdcall Hooked_FindWindow(const char *Class, const char *Text)
	{
		// Determine where the return address of the function actually points.
		void *Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		if(ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(STR("Malicious activity [Tampering].")); return HWND{}; }();

		return oFindWindow(Class, Text);
	}

	decltype(&ExitProcess) oExitProcess;
	void __stdcall Hooked_ExitProcess(DWORD ExitCode)
	{
		WRAP_IF_DEBUG(oExitProcess(ExitCode));

		WRAP_IF_RELEASE(
			[&](decltype(&ExitProcess) A)
			{
				// Again, let's meme anyone who tries to reverse this.
				uintptr_t NullPointer = *(uintptr_t *)0x00000000;
				A(NullPointer);
			}(oExitProcess);
		);
	}

	decltype(&recv) oWSARecv;
	int __stdcall Hooked_WSARecv(SOCKET Socket, char *Buffer, int Length, int Flags)
	{
		// Determine where the return address of the function actually points.
		void *Address = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);
		
		// Let's meme anyone who tries to reverse this.
		if(ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(STR("Malicious activity [Tampering].")); return -1; }();

		// Call original function
		return oWSARecv(Socket, Buffer, Length, Flags);

	}

	decltype(&send) oWSASend;
	int __stdcall Hooked_WSASend(SOCKET Socket, char *Buffer, int Length, int Flags)
	{
		// Determine where the return address of the function actually points.
		void *Address                  = _ReturnAddress();
		MEMORY_BASIC_INFORMATION Query = Protection->QueryMemory(Address);

		// If the return address points outside of the loader module,
		// fail the function.
		HMODULE ReturnModule = (HMODULE)Query.AllocationBase;
		HMODULE LoaderModule = GetModuleHandleA(NULL);

		// Let's meme anyone who tries to reverse this.
		if(ReturnModule != LoaderModule)
			return []() { Protection->SecurityCallback(STR("Malicious activity [Tampering].")); return -1; }();

		// Call original function
		return oWSASend(Socket, Buffer, Length, Flags);
	}

#pragma optimize("", on)

	// The following functions are only called internally.
	///////////////////////////////////////////////////////////

	// Sick macros, retard.
	#define CreateMinHook()		MH_STATUS Status; Status = MH_Initialize();
	#define CheckStatus()			if(Status != MH_OK) { return false; }
	#define SafeCallTo(Function)	Status = Function; CheckStatus();

#pragma optimize("", off)
	
	bool RuntimeSecurity::ApplyApiHooks()
	{
		VMProtectBeginMutation("ApplyHooks");

		// Make sure that MinHook is initialized properly.
		CreateMinHook();
		CheckStatus();

		// Apply any hooks.
		SafeCallTo(MH_CreateHook(&OpenProcess, Hooked_OpenProcess, (void **)&oOpenProcess));
		SafeCallTo(MH_EnableHook(&OpenProcess));

		SafeCallTo(MH_CreateHook(&FindWindowA, Hooked_FindWindow, (void **)&oFindWindow));
		SafeCallTo(MH_EnableHook(&FindWindowA));
		
		SafeCallTo(MH_CreateHook(&ExitProcess, Hooked_ExitProcess, (void **)&oExitProcess));
		SafeCallTo(MH_EnableHook(&ExitProcess));

		SafeCallTo(MH_CreateHook(&recv, Hooked_WSARecv, (void **)&oWSARecv));
		SafeCallTo(MH_EnableHook(&recv));

		SafeCallTo(MH_CreateHook(&send, Hooked_WSASend, (void **)&oWSASend));
		SafeCallTo(MH_EnableHook(&send));

		return true;

		VMProtectEnd();
	}

#pragma optimize("", on)

	void RuntimeSecurity::PatchDebugFunctions()
	{
		HMODULE Module = GetModuleHandleA(STR("ntdll.dll"));

		if(!Module)
			ERROR_ASSERT(STR("[000F:00001A00] Failed to initialize. Please contact an administrator."));

		// Grab exports from ntdll.dll
		uintptr_t Export_DbgUiRemoteBreakin = (uintptr_t)GetProcAddress(Module, STR("DbgUiRemoteBreakin"));
		uintptr_t Export_DbgBreakPoint      = (uintptr_t)GetProcAddress(Module, STR("DbgBreakPoint"));
		
		// Most plugins for OllyDBG / IDA only fix DbgUiRemoteBreakin/DbgBreakPoint,
		// however, NtContinue is never touched although it is used.
		// This should prevent any such plugins from effectively attaching the debugger.
		// NOTE: This does not work on x64dbg for whatever reason..
		uintptr_t Export_NtContinue         = (uintptr_t)GetProcAddress(Module, STR("NtContinue"));
		
		// Ensure that the program gets closed if a debugger is attached.
		uintptr_t Exports[] = {
			Export_DbgUiRemoteBreakin,
			Export_DbgBreakPoint,
			Export_NtContinue // This causes a lot of crashes ATM while debugging, leave this out till release.
		};

		for(auto &It : Exports)
		{
			DWORD OldProtection;
			if(!VirtualProtect((void *)It, sizeof uintptr_t + 1, PAGE_EXECUTE_READWRITE, &OldProtection))
				ERROR_ASSERT(STR("[000F:00001A00] Failed to initialize. Please contact an administrator."));

			// Patch to __asm { jmp oExitProcess; };
			*(uint8_t *)It         = 0xE9;
			*(uintptr_t *)(It + 1) = (uintptr_t)oExitProcess;

			VirtualProtect((void *)It, sizeof uintptr_t + 1, OldProtection, &OldProtection);
		}
	}

	void RuntimeSecurity::DispatchSecurityThreads()
	{
		VMProtectBeginMutation("DispatchThreads");

		std::thread DebugThread	(&RuntimeSecurity::CheckForDebugger,		this); DebugThread.detach();
		std::thread VMThread	(&RuntimeSecurity::CheckForVirtualMachine,	this); VMThread.detach();
		std::thread DriverThread(&RuntimeSecurity::CheckForDrivers,			this); DriverThread.detach();
		std::thread TamperThread(&RuntimeSecurity::CheckForTampering,		this); TamperThread.detach();
	
		VMProtectEnd();
	}

	// The following functions are only called internally.
	// The reason these are seperate is in order to maintain
	// code readability.
	///////////////////////////////////////////////////////////

#pragma optimize("", off)

	void RuntimeSecurity::CheckForVirtualMachine()
	{
		VMProtectBeginVirtualization("VMThread");

		for(;;)
		{
			if(VMProtectIsVirtualMachinePresent())
				SecurityCallback(STR("Malicious activity [Virtualized environment]."));

			// Don't put too much stress on the CPU.
			Sleep(1);
		}

		VMProtectEnd();
	}

	void RuntimeSecurity::CheckForDebugger()
	{
		VMProtectBeginVirtualization("DebuggerThread");

		for(;;)
		{
			// Read the PEB from the TIB.
			// Offset for x86 is 0x30 ; mov ..., dword ptr fs:[0x30]
			// Offset for x64 is 0x60 ; mov ..., qword ptr gs:[0x60]
			PEB *ProcessEnvBlock = (PEB *)__readgsqword(0x60);

			if(ProcessEnvBlock->BeingDebugged)
				SecurityCallback(STR("Malicious activity [Debugging attempt]."));

			// TODO: Check for x64dbg window?
			using WindowParams = std::pair<const char *, const char *>;
			static std::vector<WindowParams> BlackListedWindows = {
				{STR("ID"),				STR("Immunity")}, // Immunity Debugger
				{STR("Qt5QWindowIcon"),	STR("x64dbg")},	// x64dbg
				{STR("Qt5QWindowIcon"),	STR("x32dbg")},	// x32dbg
				{STR("Qt5QWindowIcon"),	STR("The Wireshark Network Analyzer")}, // x32dbg
				{STR("OLLYDBG"),		STR("OllyDbg")}, // OllyDbg
				{nullptr, STR("Progress Telerik Fiddler Web Debugger")}, // Telerik Fiddler
			};

			for(auto &It : BlackListedWindows)
			{
				if(FindWindowA(It.first, It.second))
					SecurityCallback(STR("Malicious activity [Debugging attempt]."));
			}

			// Don't put too much stress on the CPU.
			Sleep(1);
		}

		VMProtectEnd();
	}

	void RuntimeSecurity::CheckForDrivers()
	{
		VMProtectBeginMutation("DriverThread");

		// TODO: Check for disallowed drivers
		for(;;)
		{
			static const char *BlackListedDrivers[] = {
				STR("Sbie"),	// Sandboxie
				STR("NPF"),		// WireShark / WinPCAP
				STR("acker"),	// Process Hacker
				STR("CEDRI"),	// Cheat Engine
				STR("nHide"),	// TitanHide
				//STR("VBox")		// VirtualBox
			};

			static const char *BlackListReasons[] = {
				STR("Please uninstall Sandboxie."),
				STR("Please uninstall WireShark."),
				STR("Please close Process Hacker."),
				STR("Please close Cheat Engine."),
				STR("Please uninstall TitanHide."),
				//STR("Please uninstall VirtualBox.")
			};

			uint16_t Length = sizeof BlackListedDrivers / sizeof(BlackListedDrivers[0]);

			void *DriverList[1024];
			DWORD Needed;
			
			if(K32EnumDeviceDrivers(DriverList, sizeof DriverList, &Needed))
			{
				if(Needed > sizeof DriverList)
				{
					ERROR_ASSERT(
						STR("[00DF:00001CFF] A security thread has failed. Contact an administrator.")
					);
				}

				char	 DriverName[1024];
				uint32_t DriverCount = Needed / sizeof DriverList[0];

				for(size_t n{}; n < DriverCount; ++n)
				{
					if(K32GetDeviceDriverBaseNameA(DriverList[n], DriverName, sizeof DriverName / sizeof DriverList[0]))
					{
						for(size_t j{}; j < Length; ++j)
						{
							if(strstr(DriverName, BlackListedDrivers[j]))
								SecurityCallback(BlackListReasons[j]);
						}
					}
				}
			}

			// Don't put too much stress on the CPU.
			Sleep(1);
		}

		VMProtectEnd();
	}

	void RuntimeSecurity::CheckForTampering()
	{
		VMProtectBeginMutation("TamperThread");

		for(;;)
		{
			if(m_Identifier.m_SpecialMode[SAFE_MODE])
				ERROR_ASSERT(STR("[000F:00003D00] This program cannot run under Safe Mode.\nPlease reboot your system and select 'Normal Mode'."));

			if(m_Identifier.m_SpecialMode[TEST_SIGN_MODE])
				ERROR_ASSERT(STR("[000F:00003D00] This program cannot run under Test Signing Mode.\nPlease reboot your system and select 'Normal Mode'."));

			if(m_Identifier.m_SpecialMode[DEBUGGING_MODE])
				SecurityCallback(STR("Malicious activity [Plausible]."));

			if(m_Identifier.m_SpecialMode[TEST_BUILD_MODE])
				SecurityCallback(STR("Malicious activity [Plausible]."));

			// Don't put too much stress on the CPU.
			Sleep(1);
		}

		VMProtectEnd();
	}

#pragma optimize("", on)

	// The following functions are exposed publicly.
	///////////////////////////////////////////////////////////


	constexpr uintptr_t KUSER_SHARED_DATA = 0x7FFE0000;

	__forceinline uint64_t get_hdd_hash() {
		STORAGE_PROPERTY_QUERY		query{ };
		STORAGE_DESCRIPTOR_HEADER	desc_header{ };
		STORAGE_DEVICE_DESCRIPTOR*	device_descriptor{ };
		HANDLE						device;
		DWORD						bytes_returned;
		uint8_t*					out_buffer;
		
		device = CreateFileA(STR("\\\\.\\PhysicalDrive0"), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if(!device) return uint64_t{ };

		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		if(!DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY,
			&query, sizeof(STORAGE_PROPERTY_QUERY),
			&desc_header, sizeof(STORAGE_DESCRIPTOR_HEADER),
			&bytes_returned, 0)) {
			return uint64_t{ };
		}

		out_buffer = new uint8_t[desc_header.Size];
		memset(out_buffer, 0, desc_header.Size);

		if(!DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY,
			&query, sizeof(STORAGE_PROPERTY_QUERY),
			out_buffer, desc_header.Size,
			&bytes_returned, 0)) {
			delete[] out_buffer;
			return uint64_t{ };
		}

		device_descriptor = (STORAGE_DEVICE_DESCRIPTOR*)out_buffer;
		if(device_descriptor->SerialNumberOffset) {
			std::string serial_num = reinterpret_cast<const char*>(
				out_buffer + device_descriptor->SerialNumberOffset);

			delete[] out_buffer;
			CloseHandle(device);
			return fnv::hash_runtime(serial_num.c_str());
		}

		return 0;
	}

	void RuntimeSecurity::SetupSystemIdentifier()
	{
		VMProtectBeginUltra("SetupHWID");

		HardwareIdentifier Identifier{};

		// CPU information
		Identifier.m_CpuCount = *(uint32_t *)(KUSER_SHARED_DATA + 0x3C0);
		Identifier.m_CpuArchitecture = *(uint16_t *)(KUSER_SHARED_DATA + 0x26A);

		// HDD serial number
		Identifier.m_HardDiskSerialHash = get_hdd_hash();

		// Safe-mode
		Identifier.m_SpecialMode[0] = *(uint8_t *)(KUSER_SHARED_DATA + 0x2EC);

		// Test-signing mode
		static auto ZwQuerySystemInformation = Syscalls->Find<long(__stdcall *)(uint32_t, void *, uint32_t, uint32_t *)>(FNV("ZwQuerySystemInformation"));

		// 0x02 CODEINTEGRITY_OPTION_TESTSIGN
		// 0x20	CODEINTEGRITY_OPTION_TEST_BUILD
		// 0x80	CODEINTEGRITY_OPTION_DEBUGMODE_ENABLED

		CodeIntegrityInformation Info{ sizeof CodeIntegrityInformation };
		NTSTATUS Status = ZwQuerySystemInformation(0x67, &Info, sizeof Info, nullptr);

		if(!VMProtectGetCurrentHWID(Identifier.m_CustomDetail, 256))
			ERROR_ASSERT(STR("[000F:00001A00] Failed to initialize. Please contact an administrator."));

		if(NT_ERROR(Status))
			ERROR_ASSERT(STR("[000F:00001A00] Failed to initialize. Please contact an administrator."));

		if(Info.m_Options & 0x02)
			Identifier.m_SpecialMode[1] = true;

		if(Info.m_Options & 0x20)
			Identifier.m_SpecialMode[2] = true;

		if(Info.m_Options & 0x40)
			Identifier.m_SpecialMode[3] = true;

		m_Identifier = Identifier;
		
		VMProtectEnd();
	}

	bool RuntimeSecurity::Start()
	{
		WRAP_IF_RELEASE(
			// If hooking API functions fails, exit the program.
			if(!ApplyApiHooks())
				return false;

			// Dispatch threads before patching NtContinue & co.
			DispatchSecurityThreads();

			// Patch DbgUiRemoteBreakin, DbgBreakPoint, NtContinue
			// This also fucks up detours for some reason... only extra protection :-)
			PatchDebugFunctions();
		);

		SetupSystemIdentifier();

		return true;
	}

	HardwareIdentifier RuntimeSecurity::GetHardwareId() { return m_Identifier; }

#pragma optimize("", off)

	__declspec(noinline) MEMORY_BASIC_INFORMATION RuntimeSecurity::QueryMemory(void *Address)
	{
		MEMORY_BASIC_INFORMATION Result{};
	
		// VirtualQuery is also referenced in MinHook lib, will be a pain to find anyway
		// especially if I have VMP encrypt all this shit.
		bool Success = VirtualQuery(Address, &Result, sizeof Result);

		if(!Success)
		{
			char		ReasonParameter[64];
			uint32_t	Status = GetLastError();

			sprintf_s(ReasonParameter, STR("[00DF:%08x] There was an error with accessing a process."), Status);
			ERROR_ASSERT(ReasonParameter);
		}

		return Result;
	}

	void RuntimeSecurity::SecurityCallback(const char *Reason)
	{
		static bool TriggeredCallback = false;

		if(!TriggeredCallback) 
		{
			// You can use the reason parameters to debug the security in case
			// something weird starts going on with it.
			std::ofstream File("loader.err");
			File.write(Reason, strlen(Reason));
			File.close();

			// The process will straight up die on Release mode.
			// Compile with FuckMSVC to debug this.
			WRAP_IF_RELEASE(
				ExitProcess(rand() % RAND_MAX);
			);

			TriggeredCallback = true;
		}
	}

#pragma optimize("", on)
}