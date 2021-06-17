#include "context.hpp"
#include "hooks.hpp"

void __fastcall hooks::hud_process_input( void* ecx_, void* edx_, bool active ) {
	static auto hud_update_o = g_csgo.m_chl->get_old_function< decltype( &hooks::hud_process_input ) >( 10 );

	//todo: hook hudupdate
	//g_csgo.m_prediction( )->SetLocalViewAngles( g_ctx.m_last_fakeangle );

	return hud_update_o( ecx_, 0, active );
}