#include "base_cheat.hpp"
#include "context.hpp"
#include "hooks.hpp"
#include "settings.hpp"

void __fastcall hooks::override_view( void* ecx_, void* edx_, CViewSetup* setup ) {
	static auto override_view_o = g_gmod.m_clientmode->get_old_function< decltype( &hooks::override_view ) >( 16 );
	if( g_gmod.m_panic ) {
		return override_view_o( ecx_, edx_, setup );
	}
	
	//g_ctx.m_fov = setup->m_flFov;
	//if ( g_ctx.run_frame( ) && g_ctx.m_local->is_valid( )) {
	//	setup->m_flFov = g_settings.misc.fov_changer( );
	//}
	//g_cheat.m_extra.third_person( );
	override_view_o( ecx_, edx_, setup );

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_valid( ) && g_settings.misc.no_recoil ) {
		setup->angles -= g_ctx.m_local->m_vecPunchAngle( );
	}
}