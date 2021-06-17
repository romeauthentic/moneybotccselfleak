#pragma once

// std::unique_ptr
#include <memory>

// Intrinsics (_ReturnAddress)
#include <intrin.h>

// PEB
#include <windows.h>
#include <winternl.h>

// IOCTL
#include <winioctl.h>

// EnumDeviceDrivers
#include <psapi.h>

// WinInet
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

// Required for the SDK from VMP which offers
// virtual machines and string encryption, as 
// well as debug/VM checks.
#include <VMProtectSDK.h>

// Required for MinHook.
#include <MinHook.h>
#pragma comment(lib, "MinHook.lib")

// Used for wrapping Themida's macros along with some other things.
// e.g: WRAP_IF_RELEASE( VM_SHARK_BLACK_START ) will only trigger in Release mode.
// Likewise, WRAP_IF_DEBUG( printf( "Error: %08x", GetLastError() ) ) will only 
// trigger in Debug mode.
// Just a neat little feature that I decided to implement :-)
#ifdef DEBUG
	// Sick macros, retard.
	#define WRAP_IF_RELEASE( s )
	#define WRAP_IF_DEBUG( s ) { s; }

	#define STR( s ) s
#else
	// Sick macros, retard.
	#define WRAP_IF_RELEASE( s ) { s; }
	#define WRAP_IF_DEBUG( s )

	#define STR( s ) VMProtectDecryptStringA( s )
#endif

namespace Security
{
	// Sigh..
	struct CodeIntegrityInformation {
		uint32_t m_Size;
		uint32_t m_Options;
	};

	enum SpecialMode {
		SAFE_MODE,
		TEST_SIGN_MODE,
		TEST_BUILD_MODE,
		DEBUGGING_MODE
	};

	// Hardware ID structure (this is hashed and sent to server, but it's easier to use it 
	// this way internally)
	struct HardwareIdentifier
	{
		// Generic CPU information.
		uint16_t m_CpuArchitecture;
		uint32_t m_CpuCount;
		
		// Hash of the hard disk serial identifier.
		uint64_t m_HardDiskSerialHash;

		// VMP HWID
		char	 m_CustomDetail[256];

		// Safe-mode/Test-signing mode status
		uint8_t  m_SpecialMode[4];
	};

	// This class implements the runtime security system.
	// In short, upon initialization, the system applies detours to numerous API functions
	// which will be checked for integrity every time they are called. 
	// Also, a few threads are dispatched in the process in order to ensure that there are no
	// forbidden programs/conditions being triggered. 
	// The class has an (inlined) security callback which can be used to phone home and infract/ban
	// any potentially malicious actions from users.
	class RuntimeSecurity
	{
		HardwareIdentifier m_Identifier;

	protected:
		// Applies necessary API hooks.
		bool ApplyApiHooks();

		// Patches common debugging functions to crash the program.
		void PatchDebugFunctions();

		// Dispatches security threads.
		void DispatchSecurityThreads();

		// Grabs all hardware data.
		void SetupSystemIdentifier();

		// The following functions are used in security threads to run checks.
		void CheckForVirtualMachine();

		void CheckForDebugger();

		void CheckForDrivers();

		void CheckForTampering();

	public:
		// Initializes the runtime security system.
		bool Start();

		// Retrieves the current Hardware ID for the system.
		HardwareIdentifier GetHardwareId();

		// Syscall wrapper for ZwQueryVirtualMemory.
		// This is so we don't get hooked.
		MEMORY_BASIC_INFORMATION QueryMemory(void *Address);

		// Security callback.
		// This should phone back to the server and log any tampering.
		// TODO: Nave, actually implement this.
		void SecurityCallback(const char *Reason);
	};

	// Readability
	using RuntimeSecurityPtr = std::unique_ptr<RuntimeSecurity>;
}

extern Security::RuntimeSecurityPtr Protection;