#include <intrin.h>

#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

#include "js_mgr.h"

void __fastcall hooks::begin_lock( void* ecx, void* edx ) { // THIS IS PROLLY NOT ENDLOCK, BUT WHATEVER
	// signature:
	// search "CL_CallPostDataUpdates: missing ent %d" in engine.dll.
	// where framestagenotify gets called

	static auto ret_addr = pattern::first_code_match< void* >( g_csgo.m_engine.dll( ), "A1 ? ? ? ? B9 ? ? ? ? 6A 00 FF 50 18 8B 15 ? ? ? ?" );

	if( _ReturnAddress( ) == (void*)ret_addr ) {
		g_ctx.m_stage = FRAME_NET_UPDATE_POSTDATAUPDATE_START;
		g_cheat.m_visuals.world_modulate( );

		static float last_time = 0.f;

		if( g_ctx.run_frame( ) && g_ctx.m_local->is_valid( ) ) {
			auto state = g_ctx.m_local->get_animstate( );

			auto& anims = g_ctx.m_local->get_animdata( ).m_animlayers;

			if( state && state->m_flLastClientSideAnimationUpdateTime != last_time ) {
				for( size_t i{ }; i < 13; ++i ) {
					auto& layer = g_ctx.m_local->m_AnimOverlay( ).GetElements( )[ i ];

					if( layer.m_flWeight != anims.at( i ).m_flWeight || layer.m_flCycle != anims.at( i ).m_flCycle ) {
						memcpy( &g_ctx.m_local->get_animdata( ).m_animlayers,
							g_ctx.m_local->m_AnimOverlay( ).GetElements( ),
							sizeof( C_AnimationLayer ) * 13 );

						last_time = state->m_flLastClientSideAnimationUpdateTime;
						break;
					}
				}
			}
		}


		
		/*if( g_settings.rage.anti_aim && g_ctx.run_frame( ) ) {
			static float last_time = 0.f;

			if( last_time != g_ctx.m_local->get_animstate( )->m_flLastClientSideAnimationUpdateTime ) {
				
			}
		}*/
		

		//g_con->game_console_print( "net postdataupdate start\n" );

		// FRAME_NET_UPDATE_POSTDATAUPDATE_START
		g_cheat.m_extra.float_ragdolls( );

		g_cheat.m_player_mgr.frame_stage_notify( );
		g_cheat.m_prediction.frame_stage_notify( );
		g_cheat.m_ragebot.m_resolver->frame_stage_notify( );
		g_cheat.m_ragebot.m_lagcomp->fsn_render_start( );
		g_cheat.m_skins( );

		g_js->run_on_stage( );
	}

	begin_lock_o( ecx, edx );
}
