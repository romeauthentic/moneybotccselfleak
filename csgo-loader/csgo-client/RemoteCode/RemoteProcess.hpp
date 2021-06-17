#pragma once

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <Security/FnvHash.hpp>
#include <Security/SyscallManager.hpp>

namespace RemoteCode
{
	// The module wrapper.
	class RemoteProcess;
	class RemoteModule
	{
		HANDLE  m_Module;

	public:
		RemoteModule() = default;
		RemoteModule(HANDLE Module);
		~RemoteModule() { CloseHandle(m_Module); }
		
		// Fuck This , #Lol
		//uintptr_t FindOccurence(const char *Pattern);

		// Allow us to access the module by just passing the
		// handle as a parameter.
		operator HANDLE() { return m_Module; }
	};

	// The process wrapper.
	class RemoteProcess
	{
		HANDLE	m_Process;
		int32_t	m_ProcessId;

		// Exposing the syscalls in a convenient way to use with templating.
		void ReadMemoryWrapper_Internal(void *Address, void *Data, size_t SizeOfData);
		void WriteMemoryWrapper_Internal(void *Address, void *Data, size_t SizeOfData);

	public:
		RemoteProcess() = default;

		// For portability, will ignore exceptions.
		RemoteProcess(const char *ProcessName)
		{
			Start(ProcessName);
		}

		// Release the handle when the process goes out of scope.
		//~RemoteProcess()
		//{
		//	if(m_Process)
		//		CloseHandle(m_Process);
		//}

		// Find the process ID and initialise the process.
		bool Start(const char *ProcessName);

		// Writes to the process memory.
		template <typename T>
		void Write(void *Address, T Data)
		{
			WriteMemoryWrapper_Internal(Address, (void *)&Data, sizeof T);
		}

		void Write(void *Address, uint8_t *Data, size_t SizeOfData)
		{
			WriteMemoryWrapper_Internal(Address, (void *)Data, SizeOfData);
		}

		// Reads from the process memory.
		template <typename T>
		T Read(void *Address)
		{
			T Buffer{};
			ReadMemoryWrapper_Internal(Address, (void *)&Buffer, sizeof T);

			return Buffer;
		}

		void Read(void *Address, uint8_t *Data, size_t SizeOfData)
		{
			ReadMemoryWrapper_Internal(Address, Data, SizeOfData);
		}

		// Allocates a memory region in the process.
		void *Allocate(size_t AllocationSize);

		// Finds a module in the process.
		RemoteModule FindModule(const char *ModuleName);

		// Allow us to access the process by just passing the
		// handle as a parameter.
		operator HANDLE() { return m_Process; }
	};
}