#include "context.hpp"
#include "hooks.hpp"

void __fastcall hooks::hud_process_input( void* ecx_, void* edx_, bool active ) {
	static auto hud_update_o = g_csgo.m_chl->get_old_function< decltype( &hooks::hud_process_input ) >( 10 );

	static auto g_vecRenderAngle = *pattern::first_code_match< vec3_t** >( g_csgo.m_chl.dll( ), 
		xors( "F3 0F 10 05 ? ? ? ? F3 0F 5C 05 ? ? ? ? F3 0F 11 86" ), 0x4 );

	if( g_ctx.run_frame( ) ) {
		float backup = g_vecRenderAngle->y;
		g_vecRenderAngle->y = g_ctx.m_last_fakeangle.y;
		hud_update_o( ecx_, 0, active );
		g_vecRenderAngle->y = backup;
		return;
	}

	return hud_update_o( ecx_, 0, active );
}