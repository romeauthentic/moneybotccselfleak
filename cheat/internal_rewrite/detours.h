#pragma once
#include <windows.h>
#include <cstdint>
#include <future>

#define MEMORY_SLOT_SIZE 64

namespace memory
{
	namespace instructions
	{
// we're dealing with opcodes, align by 1 byte
#pragma pack(push, 1)
		struct short_jump_t
		{
			uint8_t  opcode{ };
			uint8_t  operand{ };
		};

		struct long_jump_t
		{
			uint8_t  opcode{ };
			uint32_t operand{ };
		};

		struct short_patch_t
		{
			long_jump_t jmp{ };
			short_jump_t jmp_short{ };
		};

		struct JCC_REL
		{
			uint8_t  opcode0{ };
			uint8_t  opcode1{ };
			uint32_t operand{ };
		};
#pragma pack(pop)
	}

	struct detour_hook_t
	{
		bool enabled{ };
		bool short_patch{ };

		uintptr_t m_original{ };
		uintptr_t m_custom{ };
		uintptr_t m_trampoline{ };

		uint8_t m_original_bytes[8]{ };

		bool allocate_trampoline( )
		{
			if ( m_trampoline )
				return false;

			m_trampoline = (uintptr_t)VirtualAlloc( nullptr, MEMORY_SLOT_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
			memset( (void*)m_trampoline, 0, MEMORY_SLOT_SIZE );

			return m_trampoline;
		}

		void restore( )
		{
			if ( m_trampoline )
			{
				VirtualFree( (void*)m_trampoline, MEMORY_SLOT_SIZE, MEM_RELEASE );
				m_trampoline = 0;
			}
		}
	};

	class c_detours
	{
		std::vector<detour_hook_t> m_hooks;

		static bool create_trampoline( detour_hook_t* hook );

		detour_hook_t* find_hook( uintptr_t custom_func )
		{
			detour_hook_t* hook = nullptr;

			for ( auto& m_hook : m_hooks )
			{
				if ( m_hook.m_custom == custom_func )
				{
					return &m_hook;
				}
			}

			return nullptr;
		}

		bool set_hook( detour_hook_t* hook, bool enable );

		bool set_hook( uintptr_t custom_func, bool enable )
		{
			detour_hook_t* hook = find_hook( custom_func );

			if ( hook )
				return set_hook( hook, enable );

			return false;
		}

	public:

		template < typename fn = uintptr_t >
		fn create_hook( fn custom_func, void* original_func )
		{
			detour_hook_t hook;
			hook.m_original = uintptr_t( original_func );
			hook.m_custom   = intptr_t( custom_func );

			if ( create_trampoline( &hook ) )
			{
				if ( hook.short_patch )
					memcpy( hook.m_original_bytes, (void*)( hook.m_original - sizeof( instructions::long_jump_t ) ), sizeof( instructions::short_patch_t ) );
				else
					memcpy( hook.m_original_bytes, (void*)( hook.m_original ), sizeof( instructions::long_jump_t ) );

#ifdef _DEBUG
				printf( " $> prepared hook [ original: 0x%08x ] [ custom: 0x%08x ] [ short jmp: %d ] \n", uintptr_t( original_func ), uintptr_t( custom_func ), hook.short_patch );
#endif

				m_hooks.push_back( hook );
				return (fn)hook.m_trampoline;
			}
#ifdef _DEBUG
			printf( " $> FAILED TO HOOK [ original: 0x%08x ] [ custom: hook 0x%08x ] \n", uintptr_t( original_func ), uintptr_t( custom_func ) );
#endif
			return (fn)0;
		}

		void enable( )
		{
			for ( auto& hook : m_hooks )
				set_hook( &hook, true );
		}

		void restore( )
		{
			for ( auto& hook : m_hooks )
			{
				set_hook( &hook, false );
				hook.restore( );
			}

			m_hooks.clear( );
		}

		template < typename fn = uintptr_t >
		fn original( void* custom_func )
		{
			auto hook = find_hook( (uintptr_t)custom_func );

			if ( hook )
				return (fn)hook->m_trampoline;

			return (fn)0;
		}
	}; extern c_detours detours;
}

