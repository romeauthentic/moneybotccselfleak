#include "hooks.h"
#include "base_cheat.h"
#include "ctx.hpp"

void __fastcall hooks::override_mouse_input( void* ecx_, void* edx_, float* x, float* y ) {
	static auto omi_o = cl.m_clientmode->get_old_function< decltype( &hooks::override_mouse_input ) >( 20 );
	omi_o( ecx_, edx_, x, y );

	if( cl.m_panic ) return;

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		g_cheat.aim( x, y );
	}
}