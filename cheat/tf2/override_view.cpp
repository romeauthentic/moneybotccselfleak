#include "base_cheat.h"
#include "ctx.hpp"
#include "hooks.h"
#include "settings.h"
void __fastcall hooks::override_view( void* ecx_, void* edx_, CViewSetup* setup ) {
	static auto override_view_o = cl.m_clientmode->get_old_function< decltype( &hooks::override_view ) >( 16 );
	if( cl.m_panic ) {
		return override_view_o( ecx_, nullptr, setup );
	}

	g_ctx.m_fov = setup->m_flFov;
	if( g_ctx.run_frame( ) && g_ctx.m_local->is_valid( ) ) {
		setup->m_flFov = g_settings.misc.fov_changer( );
	}
	g_cheat.extra.thirdperson( );
	override_view_o( ecx_, nullptr, setup );
}