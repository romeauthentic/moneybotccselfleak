#pragma once

#include <windows.h>
#include <winternl.h>
#include <cstdint>
#include <algorithm>
#include <map>
#include <fstream>
#include <vector>
#include <iterator>

#include <UserExperience/UserInterface.hpp>

#include <Security/RuntimeSecurity.hpp>
#include <Security/FnvHash.hpp>

using ByteArray = std::vector<uint8_t>;

namespace Wrapper
{
	// A stub used for our syscalls.
	class SyscallStub
	{
		// The shellcode which executes a low latency system call.
		uint8_t m_Shellcode[11] = {
			0x4C, 0x8B, 0xD1,				// mov r10, rcx
			0xB8, 0x00, 0x00, 0x00, 0x00,	// mov eax, [syscall index]
			0x0F, 0x05,						// syscall
			0xC3
		};
	public:
		// Constructors.
		SyscallStub() = default;

		// Sets the syscall index.
		void SetIndex(uint32_t Index);

		__forceinline uintptr_t Get()
		{
			return (uintptr_t)m_Shellcode;
		}
	};

	// Manager for system calls. Used to iterate NTDLL for all syscall indices.
	// Read: https://www.evilsocket.net/2014/02/11/on-windows-syscall-mechanism-and-syscall-numbers-extraction-methods/
	class SyscallManager
	{
		// Reading NTDLL from disk because it cannot be modified
		// due to restrictions put in place by PatchGuard.
		ByteArray GetNtdllFromDisk();

		// Container for all syscall stubs.
		std::map<uint64_t, SyscallStub> m_Syscalls;

		// Helper functions.
		uintptr_t GetRawOffsetByRva(IMAGE_SECTION_HEADER *SectionHeader, uintptr_t Sections, uintptr_t FileSize, uintptr_t Rva);
		IMAGE_SECTION_HEADER *GetSectionByRva(IMAGE_SECTION_HEADER *SectionHeader, uintptr_t Sections, uintptr_t Rva);

	public:
		// Initialises the syscall manager, dumping all the
		// syscall indices.
		bool Start();

		// Finds a syscall by hash.
		template < typename T >
		T Find(uint64_t Hash)
		{
			uint64_t Syscall = m_Syscalls[Hash].Get();

			if(!Syscall)
				ERROR_ASSERT(STR("[000F:00001B00] Internal software error. Please contact an administrator."));

			return (T)m_Syscalls[Hash].Get();
		}
	};

	using SyscallManagerPtr = std::unique_ptr<SyscallManager>;
}

extern Wrapper::SyscallManagerPtr Syscalls;