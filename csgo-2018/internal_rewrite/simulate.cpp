#include <intrin.h>

#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

void __fastcall hooks::simulate( void* ecx, void* edx ) {
	static auto simulate_o = g_csgo.m_engine_vgui->get_old_function< decltype( &hooks::simulate ) >( 39 );

	static auto ret_addr = pattern::first_code_match< void* >( g_csgo.m_engine.dll( ), "8B CE 6A FF 8B 06", -0x81 );
	if( _ReturnAddress( ) == ret_addr ) {
		g_ctx.m_stage = FRAME_RENDER_START;
		g_cheat.m_visuals.world_modulate( );

		//g_con->game_console_print( "render start\n" );
		g_cheat.m_extra.disable_post_processing( );
		g_cheat.m_extra.no_flash( );
		g_cheat.m_extra.no_recoil( false );
		util::disable_pvs( );
		g_cheat.m_ragebot.m_lagcomp->fsn_render_start( );
		//g_cheat.m_ragebot.m_lagcomp->invalidate_bone_caches( );
		if( g_csgo.m_input( )->m_fCameraInThirdPerson ) {
			bool real_yaw = g_settings.rage.anti_aim( );
			if( g_ctx.get_last_cmd( ) && g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
				//g_ctx.m_local->m_vecThirdpersonAngles( ) = ( real_yaw ? g_ctx.m_thirdperson_angle : g_ctx.get_last_cmd( )->m_viewangles );
			}
		}

		g_cheat.m_extra.no_smoke( );
		if( g_settings.misc.no_smoke ) {
			static auto smoke_count = pattern::first_code_match( g_csgo.m_chl.dll( ),
				xors( "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0" ) );
			**reinterpret_cast< int** >( smoke_count + 0x8 ) = 0;
		}

		simulate_o( ecx, edx );
	}
	else {
		simulate_o( ecx, edx );
	}
}