#include <RemoteCode/RemoteProcess.hpp>

namespace RemoteCode
{
	// RemoteModule implementation
	RemoteModule::RemoteModule(HANDLE Module) :
		m_Module(Module) {}

	// RemoteProcess implementation
	bool RemoteProcess::Start(const char *ProcessName)
	{
		void *Toolhelp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if(!Toolhelp)
			return false;

		PROCESSENTRY32 ProcessEntry{ sizeof PROCESSENTRY32 };

		if(!Process32First(Toolhelp, &ProcessEntry))
			return false;

		while(Process32Next(Toolhelp, &ProcessEntry))
		{
			if(strstr(ProcessName, ProcessEntry.szExeFile))
			{
				CloseHandle(Toolhelp);

				// swoo
				m_ProcessId = ProcessEntry.th32ProcessID;
				m_Process   = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessEntry.th32ProcessID);

				WRAP_IF_DEBUG(
					printf("[DEBUG] Found process \"%s\" -> %p\n", ProcessEntry.szExeFile, m_Process);
				);

				if(!m_Process)
					ERROR_ASSERT(STR("[000G:%08x] There was an error with accessing a process."), GetLastError());

				return true;
			}
		}

		CloseHandle(Toolhelp);
		return false;
	}

	void RemoteProcess::ReadMemoryWrapper_Internal(void *Address, void *Data, size_t SizeOfData)
	{
		static auto ZwReadVirtualMemory = Syscalls->Find<long(__stdcall *)(void *, void *, void *, size_t, void *)>(FNV("ZwReadVirtualMemory"));
		
		NTSTATUS Status = ZwReadVirtualMemory(m_Process, Address, Data, SizeOfData, nullptr);

		if(NT_ERROR(Status))
			ERROR_ASSERT(STR("[00DF:%08x] There was an error with accessing a process."), Status);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Read %zd bytes from process\n", SizeOfData);
		);
	}

	void RemoteProcess::WriteMemoryWrapper_Internal(void *Address, void *Data, size_t SizeOfData)
	{
		static auto ZwWriteVirtualMemory = Syscalls->Find<long(__stdcall *)(void *, void *, void *, size_t, void *)>(FNV("ZwWriteVirtualMemory"));
		
		NTSTATUS Status = ZwWriteVirtualMemory(m_Process, Address, Data, SizeOfData, nullptr);
	
		if(NT_ERROR(Status))
			ERROR_ASSERT(STR("[00DF:%08x] There was an error with accessing a process."), Status);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Wrote %zd bytes to process\n", SizeOfData);
		);
	}

	void *RemoteProcess::Allocate(size_t AllocationSize)
	{
		static auto ZwAllocateVirtualMemory = Syscalls->Find<long(__stdcall *)(void *, void *, uint32_t, size_t *, uint32_t, uint32_t)>(FNV("ZwAllocateVirtualMemory"));
		
		// :b:invoke the :b:unction :b:oi
		void *AllocationAddress = nullptr;
		NTSTATUS Status         = ZwAllocateVirtualMemory(
			m_Process,
			&AllocationAddress,
			0,
			&AllocationSize,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE
		);

		if(NT_ERROR(Status))
			ERROR_ASSERT(STR("[00DF:%08x] There was an error with accessing a process."), Status);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Allocated page at %p (%zd bytes)\n", AllocationAddress, AllocationSize);
		);

		return AllocationAddress;
	}

	RemoteModule RemoteProcess::FindModule(const char *ModuleName)
	{
		void *Toolhelp = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_ProcessId);
		
		if(!Toolhelp)
			return RemoteModule{};
		
		MODULEENTRY32 ModuleEntry{ sizeof MODULEENTRY32 };
		
		if(!Module32First(Toolhelp, &ModuleEntry))
			return RemoteModule{};
		
		while(Module32Next(Toolhelp, &ModuleEntry))
		{
			if(strstr(ModuleEntry.szModule, ModuleName))
			{
				if(!ModuleEntry.hModule)
					ERROR_ASSERT(STR("[00DF:00001C00] An integrity check failed."));

				CloseHandle(Toolhelp);

				WRAP_IF_DEBUG(
					printf("[DEBUG] Found module \"%s\" at %p\n", ModuleEntry.szModule, ModuleEntry.hModule);
				);

				return RemoteModule(ModuleEntry.hModule);
			}
		}
		
		CloseHandle(Toolhelp);
		return RemoteModule{};
	}
}