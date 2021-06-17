#include <RemoteCode/RemoteMapper.hpp>

namespace RemoteCode
{
	bool RemoteMapper::Start(RemoteProcess &Process, ByteArray &Code)
	{
		m_Code   = Code;

		// Check if the PE file is valid.
		uint8_t			 *Buffer = m_Code.data();
		IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER *)Buffer;

		if(!DosHeader || DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		IMAGE_NT_HEADERS *NtHeaders = (IMAGE_NT_HEADERS *)(Buffer + DosHeader->e_lfanew);

		if(!NtHeaders || NtHeaders->Signature != IMAGE_NT_SIGNATURE)
			return false;

		WRAP_IF_DEBUG(
			printf("[DEBUG] PE file validated!\n");
		);

		uint32_t SizeOfImage = NtHeaders->OptionalHeader.SizeOfImage;

		m_Process = Process;

		// Allocate map in process.
		m_Mapped.reserve(SizeOfImage);
		m_Map = m_Process.Allocate(SizeOfImage);

		if(!m_Map)
			return false;

		return true;
	}

	bool RemoteMapper::WriteCodeToMap()
	{
		uint8_t			 *Buffer    = m_Code.data();
		IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER *)Buffer;
		IMAGE_NT_HEADERS *NtHeaders = (IMAGE_NT_HEADERS *)(Buffer + DosHeader->e_lfanew);
		
		IMAGE_SECTION_HEADER *Sections = (IMAGE_SECTION_HEADER *)((uintptr_t)&NtHeaders->OptionalHeader + NtHeaders->FileHeader.SizeOfOptionalHeader);

		if(!Sections)
			return false;

		uint16_t SectionCount = NtHeaders->FileHeader.NumberOfSections;
		
		// Write each section to the process.
		for(uint16_t n{}; n < SectionCount; n++)
		{
			uint32_t	VirtualAddress = Sections[n].VirtualAddress;
			uint32_t	PointerToData  = Sections[n].PointerToRawData;
			uint32_t	SizeOfData     = Sections[n].SizeOfRawData;

			WRAP_IF_DEBUG(
				printf("[DEBUG] Writing PE section #%d!\n", n);
				printf("[DEBUG] Virtual address: 0x%x\n", VirtualAddress);
				printf("[DEBUG] Pointer to data: 0x%x\n", PointerToData);
				printf("[DEBUG] Size of data   : %d bytes\n", SizeOfData);
			);

			// Write the data to map.
			uint8_t *Data     = (uint8_t *)((uintptr_t)Buffer + PointerToData);			
			std::memcpy(&m_Mapped[VirtualAddress], Data, SizeOfData);
		}

		// Copy over the PE header (temporarily).
		uint32_t SizeOfHeaders = NtHeaders->OptionalHeader.SizeOfHeaders;
		std::memcpy(&m_Mapped[0], Buffer, SizeOfHeaders);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Successfully copied over PE header!\n");
			printf("[DEBUG] Relocating image...\n");
		);

		// Relocate the image.
		IMAGE_BASE_RELOCATION *Reloc = (IMAGE_BASE_RELOCATION *)(&m_Mapped[0] + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		
		uintptr_t ImageDelta = (uintptr_t)m_Map - NtHeaders->OptionalHeader.ImageBase;
		
		while(Reloc->SizeOfBlock > 0)
		{
			// Address in binary where we need to relocate an address used by code.
			uintptr_t RelocationAddress     = (uintptr_t)(&m_Mapped[Reloc->VirtualAddress]);
			uintptr_t RelocationDataAddress = (uintptr_t)((uintptr_t)Reloc + sizeof IMAGE_BASE_RELOCATION);
			size_t    RelocationCount       = (Reloc->SizeOfBlock - sizeof IMAGE_BASE_RELOCATION) / sizeof uint16_t;
		
			for(size_t i = 0; i < RelocationCount; i++)
			{
				uint16_t RelocationData   = *(uint16_t *)(RelocationDataAddress + sizeof uint16_t * i);
				uint16_t RelocationType   = RelocationData >> 12;
				uint16_t RelocationOffset = RelocationData & 0x0FFF;
		
				WRAP_IF_DEBUG(
					printf("[DEBUG] Processing relocation at %llx!\n", RelocationAddress + RelocationOffset);
				);
		
				switch(RelocationType)
				{
					case IMAGE_REL_BASED_MIPS_JMPADDR:
					case IMAGE_REL_BASED_HIGH:
					case IMAGE_REL_BASED_LOW:
					case IMAGE_REL_BASED_HIGHLOW:
					{
						*(uint32_t*)((uintptr_t)RelocationAddress + RelocationOffset) += ImageDelta;
						break;
					}
		
					case IMAGE_REL_BASED_ABSOLUTE:
					default:
						break;
				};
			}
		
			Reloc = (IMAGE_BASE_RELOCATION*)((uintptr_t)Reloc + Reloc->SizeOfBlock);
		}

		// Cripple the entire PE header.
		//std::memset(&m_Mapped[0], 0, SizeOfHeaders);
		//
		//WRAP_IF_DEBUG(
		//	printf("[DEBUG] Successfully crippled PE header!\n");
		//);

		// Write the mapped image to the process.
		m_Process.Write(m_Map, &m_Mapped[0], m_Code.size());

		return true;
	}

	// This contains any parameters that will be passed to the shellcode function once invoked.
	struct ShellcodeParameters
	{
		// Base of image allocation
		uintptr_t m_ImageBase;

		// Used for fixing imports
		uintptr_t m_GetModuleHandle;
		uintptr_t m_LoadLibrary;
		uintptr_t m_GetProcAddress;

		// Headers
		IMAGE_NT_HEADERS        *m_NtHeaders;
		IMAGE_IMPORT_DESCRIPTOR *m_ImportDir;
	};

	// Lazy.
	uint8_t Shellcode[256] = {
		0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x6C, 0x24, 0x10, 0x48, 0x89,
		0x74, 0x24, 0x18, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57,
		0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0x51, 0x28, 0x4C, 0x8B, 0xF1, 0x4C,
		0x8B, 0x69, 0x08, 0x4C, 0x8B, 0x61, 0x10, 0x48, 0x8B, 0x69, 0x18, 0x8B,
		0x0A, 0x85, 0xC9, 0x0F, 0x84, 0x84, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x80,
		0x00, 0x00, 0x00, 0x00, 0x49, 0x8B, 0x06, 0x44, 0x8B, 0x7A, 0x0C, 0x8B,
		0x5A, 0x10, 0x4C, 0x03, 0xF8, 0x8B, 0xF9, 0x48, 0x03, 0xD8, 0x49, 0x8B,
		0xCF, 0x48, 0x03, 0xF8, 0x41, 0xFF, 0xD5, 0x48, 0x8B, 0xF0, 0x48, 0x85,
		0xC0, 0x75, 0x0E, 0x49, 0x8B, 0xCF, 0x41, 0xFF, 0xD4, 0x48, 0x8B, 0xF0,
		0x48, 0x85, 0xC0, 0x74, 0x70, 0x48, 0x8B, 0x0F, 0x48, 0x85, 0xC9, 0x74,
		0x32, 0x48, 0x2B, 0xFB, 0x48, 0x85, 0xC9, 0x79, 0x05, 0x0F, 0xB7, 0xD1,
		0xEB, 0x0A, 0x49, 0x8B, 0x16, 0x48, 0x83, 0xC2, 0x02, 0x48, 0x03, 0xD1,
		0x48, 0x8B, 0xCE, 0xFF, 0xD5, 0x48, 0x85, 0xC0, 0x74, 0x03, 0x48, 0x89,
		0x03, 0x48, 0x8B, 0x4C, 0x1F, 0x08, 0x48, 0x83, 0xC3, 0x08, 0x48, 0x85,
		0xC9, 0x75, 0xD4, 0x49, 0x8B, 0x56, 0x28, 0x48, 0x83, 0xC2, 0x14, 0x49,
		0x89, 0x56, 0x28, 0x8B, 0x0A, 0x85, 0xC9, 0x75, 0x83, 0x49, 0x8B, 0x46,
		0x20, 0x8B, 0x48, 0x28, 0x48, 0x85, 0xC9, 0x74, 0x18, 0x49, 0x8B, 0x06,
		0x45, 0x33, 0xC0, 0x4C, 0x8D, 0x0C, 0x08, 0x8B, 0xC8, 0x41, 0x8D, 0x50,
		0x01, 0x41, 0xFF, 0xD1, 0x0F, 0xB6, 0xC0, 0xEB, 0x02, 0x33, 0xC0, 0x48,
		0x8B, 0x5C, 0x24, 0x50, 0x48, 0x8B, 0x6C, 0x24, 0x58, 0x48, 0x8B, 0x74,
		0x24, 0x60, 0x48, 0x83, 0xC4, 0x20, 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D,
		0x41, 0x5C, 0x5F, 0xC3
	};

	bool RemoteMapper::ExecuteCodeFromMap()
	{
		uint8_t			 *Buffer = m_Code.data();
		IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER *)Buffer;
		IMAGE_NT_HEADERS *NtHeaders = (IMAGE_NT_HEADERS *)(Buffer + DosHeader->e_lfanew);

		WRAP_IF_DEBUG(
			printf("[DEBUG] Executing payload!\n");
		);

		ShellcodeParameters ParameterPack = {
			(uintptr_t)m_Map,

			(uintptr_t)GetModuleHandleA,
			(uintptr_t)LoadLibraryA,
			(uintptr_t)GetProcAddress,

			(IMAGE_NT_HEADERS *)((uintptr_t)m_Map + DosHeader->e_lfanew),
			(IMAGE_IMPORT_DESCRIPTOR *)((uintptr_t)m_Map + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
		};

		// Write shellcode & parameter pack.
		void *ShellcodeParams = m_Process.Allocate(sizeof ShellcodeParameters);

		if(ShellcodeParams)
			m_Process.Write< ShellcodeParameters >(ShellcodeParams, ParameterPack);

		void *ShellcodeAlloc = m_Process.Allocate(256);

		if(ShellcodeAlloc)
			m_Process.Write(ShellcodeAlloc, &Shellcode[0], sizeof Shellcode);

		system("pause");

		// Create thread.
		HANDLE Thread = CreateRemoteThread(m_Process, nullptr, 0, (LPTHREAD_START_ROUTINE)ShellcodeAlloc, ShellcodeParams, 0, nullptr);

		if(Thread)
			WaitForSingleObject(Thread, INFINITE);

		WRAP_IF_DEBUG(
			DWORD Code;
			GetExitCodeThread(Thread, &Code);

			printf("[DEBUG] Thread execution finished with code %08x!\n", Code);
		)

		system("pause");

		return true;
	}
}