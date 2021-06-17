#include "detours.h"
#include "hde32.h"

// very MUCH comment i APPRECIATE cool

memory::c_detours memory::detours;

__forceinline static bool is_code_padding( uintptr_t address, size_t size )
{
	uint8_t* code_ptr = (uint8_t*)address;

	if ( code_ptr[0] != 0x00 && code_ptr[0] != 0x90 && code_ptr[0] != 0xCC )
		return false;

	for ( int i = 1; i < size; ++i )
	{
		if ( code_ptr[i] != code_ptr[0] )
			return false;
	}

	return true;
}

bool memory::c_detours::create_trampoline( detour_hook_t* hook )
{
	if ( !hook->allocate_trampoline( ) )
		return false;

	instructions::long_jump_t call = {
		0xE8,
		0x00000000
	};

	instructions::long_jump_t jmp = {
		0xE9,
		0x00000000
	};

	instructions::JCC_REL jcc = {
		0x0F, 0x80,
		0x00000000
	};

	uint8_t     old_position = 0;
	uint8_t     new_position = 0;
	uintptr_t   jmp_dest = 0;
	bool        finished = false;

	hook->short_patch = false;

	const int max_instruction_boundaries = 8;
	int instruction_boundaries = 0;

	do
	{
		hde32s       hs;
		uint8_t      copy_size;
		uintptr_t    copy_src;
		uintptr_t    old_instruction = hook->m_original + old_position;
		uintptr_t    new_instruction = hook->m_trampoline + new_position;

		copy_size = hde32_disasm( (void*)old_instruction, &hs );
		if ( hs.flags & F_ERROR )
			return false;

		copy_src = old_instruction;
		if ( old_position >= sizeof( instructions::long_jump_t ) )
		{
			// the trampoline function is long enough.
			// complete the function with the jump to the target function.

			jmp.opcode = 0xE9;
			jmp.operand = (uint32_t)( old_instruction - ( new_instruction + sizeof( instructions::long_jump_t ) ) );
			copy_src = uintptr_t( &jmp );
			copy_size = sizeof( jmp );

			finished = true;
		}
		else if ( hs.opcode == 0xE8 )
		{
			// direct relative CALL
			uintptr_t dest = old_instruction + hs.len + (int32_t)hs.imm.imm32;

			call.opcode = 0xE8;
			call.operand = (uint32_t)( dest - ( new_instruction + sizeof( instructions::long_jump_t ) ) );

			copy_src = uintptr_t( &call );
			copy_size = sizeof( call );
		}
		else if ( ( hs.opcode & 0xFD ) == 0xE9 )
		{
			// direct relative JMP (EB or E9)
			uintptr_t dest = old_instruction + hs.len;

			if ( hs.opcode == 0xEB ) // short jmp
				dest += (int8_t)hs.imm.imm8;
			else
				dest += (int32_t)hs.imm.imm32;

			// simply copy an internal jump.
			if ( hook->m_original <= dest && dest < hook->m_original + sizeof( instructions::long_jump_t ) )
			{
				if ( jmp_dest < dest )
					jmp_dest = dest;
			}
			else
			{
				jmp.operand = (uint32_t)( dest - ( new_instruction + sizeof( instructions::long_jump_t ) ) );

				copy_src = uintptr_t( &jmp );
				copy_size = sizeof( instructions::long_jump_t );

				// exit the function if it is not in the branch
				finished = ( old_instruction >= jmp_dest );
			}
		}
		else if ( ( hs.opcode & 0xF0 ) == 0x70 || ( hs.opcode & 0xFC ) == 0xE0 || ( hs.opcode2 & 0xF0 ) == 0x80 )
		{
			// direct relative JCC
			uintptr_t dest = old_instruction + hs.len;

			if ( ( hs.opcode & 0xF0 ) == 0x70      // JCC
				 || ( hs.opcode & 0xFC ) == 0xE0 )  // LOOPNZ/LOOPZ/LOOP/JECXZ
				dest += (int8_t)hs.imm.imm8;
			else
				dest += (int32_t)hs.imm.imm32;

			// simply copy an internal jump.
			if ( (uintptr_t)hook->m_original <= dest && dest < ( (uintptr_t)hook->m_original + sizeof( instructions::long_jump_t ) ) )
			{
				if ( jmp_dest < dest )
					jmp_dest = dest;
			}
			else if ( ( hs.opcode & 0xFC ) == 0xE0 )
			{
				// LOOPNZ/LOOPZ/LOOP/JCXZ/JECXZ to the outside are not supported.
				return false;
			}
			else
			{
				uint8_t cond = ( ( hs.opcode != 0x0F ? hs.opcode : hs.opcode2 ) & 0x0F );

				jcc.opcode1 = 0x80 | cond;
				jcc.operand = (uint32_t)( dest - ( new_instruction + sizeof( jcc ) ) );

				copy_src = uintptr_t( &jcc );
				copy_size = sizeof( jcc );
			}
		}
		else if ( ( hs.opcode & 0xFE ) == 0xC2 )
		{
			// RET (C2 or C3)

			// complete the function if not in a branch.
			finished = ( old_instruction >= jmp_dest );
		}

		// can't alter the instruction length in a branch.
		if ( old_instruction < jmp_dest && copy_size != hs.len )
			return false;

		// trampoline function is too large.
		if ( new_position + copy_size > MEMORY_SLOT_SIZE )
			return false;

		// trampoline function has too many instructions.
		if ( instruction_boundaries >= max_instruction_boundaries )
			return false;

		instruction_boundaries++;

		memcpy( (void*)( hook->m_trampoline + new_position ), (void*)copy_src, copy_size );

		new_position += copy_size;
		old_position += hs.len;
	}
	while ( !finished );

	// can we do a regular 5 byte jmp patch?
	if ( old_position < sizeof( instructions::long_jump_t ) && !is_code_padding( (uintptr_t)hook->m_original + old_position, sizeof( instructions::long_jump_t ) - old_position ) )
	{
		// can we put 2 byte jmp patch?
		if ( old_position < sizeof( instructions::short_jump_t ) && !is_code_padding( (uintptr_t)hook->m_original + old_position, sizeof( instructions::short_jump_t ) - old_position ) )
			return false;

		// can we put a 5 byte jmp patch above it?
		if ( !is_code_padding( (uintptr_t)hook->m_original - sizeof( instructions::long_jump_t ), sizeof( instructions::long_jump_t ) ) )
			return false;

		hook->short_patch = true;
	}

	return true;
}

bool memory::c_detours::set_hook( detour_hook_t* hook, bool enable )
{
	// hook is already in place silly
	if ( hook->enabled == enable )
		return true;

	if ( enable )
	{
		if ( hook->short_patch )
		{
			uint8_t* target = (uint8_t*)( hook->m_original - sizeof( instructions::long_jump_t ) );

			// prepare patch
			instructions::short_patch_t jmp_patch;
			jmp_patch.jmp.opcode = 0xE9;
			jmp_patch.jmp.operand = (uint32_t)( hook->m_custom - hook->m_original - sizeof( instructions::long_jump_t ) );

			jmp_patch.jmp_short.opcode = 0xEB;
			jmp_patch.jmp_short.operand = (uint8_t)( 0 - sizeof( instructions::short_patch_t ) );

			// overwrite protection
			DWORD  original_protection;
			VirtualProtect( target, sizeof( instructions::short_patch_t ), PAGE_EXECUTE_READWRITE, &original_protection );

			// place hook
			memcpy( target, &jmp_patch, sizeof( instructions::short_patch_t ) );

			// restore protection
			VirtualProtect( target, sizeof( instructions::short_patch_t ), original_protection, &original_protection );

			// let cpu know we altered code
			FlushInstructionCache( GetCurrentProcess( ), target, sizeof( instructions::short_patch_t ) );
		}
		else
		{
			// prepare patch
			instructions::long_jump_t jmp_patch;
			jmp_patch.opcode = 0xE9;
			jmp_patch.operand = (uint32_t)( hook->m_custom - hook->m_original - sizeof( instructions::long_jump_t ) );

			// overwrite protection
			DWORD  original_protection;
			VirtualProtect( (void*)hook->m_original, sizeof( instructions::long_jump_t ), PAGE_EXECUTE_READWRITE, &original_protection );

			// place hook
			memcpy( (void*)hook->m_original, &jmp_patch, sizeof( instructions::long_jump_t ) );

			// restore protection
			VirtualProtect( (void*)hook->m_original, sizeof( instructions::long_jump_t ), original_protection, &original_protection );

			// let cpu know we altered code
			FlushInstructionCache( GetCurrentProcess( ), (void*)hook->m_original, sizeof( instructions::long_jump_t ) );
		}


	}
	else
	{
		if ( hook->short_patch )
		{
			uint8_t* target = (uint8_t*)( hook->m_original - sizeof( instructions::long_jump_t ) );

			// overwrite protection
			DWORD  original_protection;
			VirtualProtect( target, sizeof( instructions::short_patch_t ), PAGE_EXECUTE_READWRITE, &original_protection );

			// restore backup
			memcpy( target, hook->m_original_bytes, sizeof( instructions::short_patch_t ) );

			// restore protection
			VirtualProtect( target, sizeof( instructions::short_patch_t ), original_protection, &original_protection );

			// let cpu know we altered code
			FlushInstructionCache( GetCurrentProcess( ), target, sizeof( instructions::short_patch_t ) );
		}
		else
		{
			uint8_t* target = (uint8_t*)hook->m_original;

			// overwrite protection
			DWORD  original_protection;
			VirtualProtect( target, sizeof( instructions::long_jump_t ), PAGE_EXECUTE_READWRITE, &original_protection );

			// restore backup
			memcpy( target, hook->m_original_bytes, sizeof( instructions::long_jump_t ) );

			// restore protection
			VirtualProtect( target, sizeof( instructions::long_jump_t ), original_protection, &original_protection );

			// let cpu know we altered code
			FlushInstructionCache( GetCurrentProcess( ), target, sizeof( instructions::long_jump_t ) );
		}
	}

	hook->enabled = enable;
	return true;
}