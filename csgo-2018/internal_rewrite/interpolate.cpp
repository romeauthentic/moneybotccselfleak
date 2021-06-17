#include "hooks.hpp"
#include "interface.hpp"
#include "mem.hpp"
#include "base_cheat.hpp"
#include "util.hpp"
#include "context.hpp"

int __fastcall hooks::cl_interpolate_get_int( void* ecx_, void* edx_ ) {
	static auto old_fn = g_csgo.m_interpolate->get_old_function< decltype( &hooks::cl_interpolate_get_int ) >( 13 );
	static auto interpolate_entities = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 1C 8B 0D ? ? ? ? 53" ), 0x72 );
	
	stack_t stack( get_baseptr( ) );

	if( stack.return_address( ) == interpolate_entities ) {
		//if( g_settings.rage.enabled( ) && g_ctx.m_local && g_ctx.m_local->is_valid( ) )
			//return 0;
	}

	return old_fn( ecx_, 0 );
}