#include "hooks.hpp"
#include "pattern.hpp"
#include "mem.hpp"

bool __fastcall hooks::is_hltv( void* ecx, uintptr_t pvs_ent ) {
	static auto is_hltv_o = g_csgo.m_engine->get_old_function< decltype( &hooks::is_hltv ) >( 93 );
	static auto pvs_retaddr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7" ) );

	if ( !g_csgo.m_panic && pvs_retaddr ) {
		stack_t stack( get_baseptr( ) );
		auto return_address = stack.return_address( );

		if ( return_address == pvs_retaddr ) {
			if ( pvs_ent && reinterpret_cast< c_base_player* >( pvs_ent )->is_valid( ) ) {
				*( int* )( pvs_ent + 0xa24 ) = -1;
				*( int* )( pvs_ent + 0xa2c ) = *( int* )( pvs_ent + 0xa28 );
				*( int* )( pvs_ent + 0xa28 ) = 0;
				return true;
			}
		}
	}

	return is_hltv_o( ecx, pvs_ent );
}

bool __declspec( naked ) __fastcall hooks::is_hltv_proxy( void*, void* ) {
	__asm {
		mov   edx, edi;
		jmp   is_hltv;
	}
}