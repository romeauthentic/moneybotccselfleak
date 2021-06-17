#include "base_cheat.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"
#include "hooks.hpp"
#include "interface.hpp"
#include "detours.h"
#undef min


namespace features
{
	void c_extra::rank_reveal( user_cmd_t* ucmd ) {
		if( !( ucmd->m_buttons & IN_SCORE ) || !g_settings.misc.rank_reveal( ) ) return;
		g_csgo.m_chl( )->DispatchUserMessage( 50, 0, 0, nullptr ); // 50 CS_UM_ServerRankRevealAll
	}
	
	void c_extra::thirdperson( ) {
		if( !g_ctx.m_local )
			return;

		static bool enabled = false;
		static bool toggle = true;
		static bool key_held = false;

		static auto var = g_csgo.m_cvar( )->FindVar( xors( "cam_idealdist" ) );

		if( g_input.is_key_pressed( ( VirtualKeys_t )g_settings.misc.thirdperson_key( ) ) ) {
			if( !key_held ) {
				toggle ^= 1;
			}
			key_held = true;
		}
		else {
			key_held = false;
		}

		if( !g_settings.misc.thirdperson || !toggle ) {
			if( enabled ) {
				g_csgo.m_input( )->m_fCameraInThirdPerson = false;
				enabled = false;
			}

			return;
		}
		
		vec3_t viewangles{ };
		if( g_ctx.m_local->is_alive( ) ) {
            float dist = var->get_float( );

			if( var->get_float( ) != g_settings.misc.thirdperson_dist( ) ) {
				var->set_value( g_settings.misc.thirdperson_dist( ) );
			}

			g_csgo.m_engine( )->GetViewAngles( viewangles );
			g_csgo.m_input( )->m_fCameraInThirdPerson = true;
			g_csgo.m_input( )->m_vecCameraOffset = vec3_t( viewangles.x, viewangles.y, dist );

			constexpr float    cam_hull_offset{ 16.f };

			const     vec3_t   cam_hull_min( -cam_hull_offset, -cam_hull_offset, -cam_hull_offset );
			const     vec3_t   cam_hull_max( cam_hull_offset, cam_hull_offset, cam_hull_offset );
			vec3_t             cam_forward, origin = g_ctx.m_local->get_eye_pos( );

			cam_forward = math::angle_vectors( vec3_t( viewangles.x, viewangles.y, 0.f ) );

			CTraceFilterWorldAndPropsOnly filter;
			Ray_t						  ray;
			CGameTrace					  tr;

			ray.Init( origin, origin - ( cam_forward * dist ), cam_hull_min, cam_hull_max );

			g_csgo.m_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr );
			g_csgo.m_input( )->m_vecCameraOffset.z = dist * tr.fraction;

			enabled = true;
		}
		else if( g_csgo.m_entlist( )->GetClientEntityFromHandle< >( g_ctx.m_local->m_hObserverTarget( ) ) ) {
			g_csgo.m_input( )->m_fCameraInThirdPerson = false;
			g_ctx.m_local->m_iObserverMode( ) = 5;
		}
	}

	void c_extra::update_netchannel( ) {
		// fuck u bithc
		auto *netchan = g_csgo.m_client_state->m_netchannel;

		if( netchan->m_nInSequenceNr != m_last_sequence ) {
			m_last_sequence = netchan->m_nInSequenceNr;

			// emplace new record
			netchan_record_t record{
				g_csgo.m_globals->m_curtime,
				netchan->m_nInReliableState,
				netchan->m_nInSequenceNr
			};
			m_net_records.emplace_back( record );
		}

		// here's the thing.
		// stl is autistic and it crashes.

		// clear outdated records
		for( size_t i{ }; i < m_net_records.size( ); ++i ) {
			const float delta = g_csgo.m_globals->m_curtime - m_net_records.at( i ).m_curtime;

			// 200ms is the max we can fake anyway..
			// hnn this might be why it's so fucked :/
			if( delta > 0.4f )
				m_net_records.erase( m_net_records.begin( ) + i  );
		}
	}

	void c_extra::add_latency( INetChannel* channel ) {
		if( !g_settings.misc.net_fakelag( ) )
			return;

		auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
		float in_latency = nci->GetLatency( 0 );

		// amount of latency we want to achieve
		float latency = 0.15f - ( in_latency + g_csgo.m_globals->m_frametime ) - TICK_INTERVAL( );

		if( g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
			// god this is autistic
			for( auto& it : m_net_records ) {
				float delta = g_csgo.m_globals->m_curtime - it.m_curtime;

				if( delta >= latency ) {
					// apply latency
					channel->m_nInReliableState = it.m_reliable;
					channel->m_nInSequenceNr = it.m_sequence;

					break;
				}
			}
		}
	}

	void c_extra::fake_duck( user_cmd_t* cmd ) {
		if( !g_settings.misc.fake_duck )
			return;

		if( !g_input.is_key_pressed( g_settings.misc.fake_duck_key ) )
			return;

		cmd->m_buttons |= IN_BULLRUSH;

		int choked = g_cheat.m_lagmgr.get_choked( );
		if( choked > 6 )
			cmd->m_buttons |= IN_DUCK;
		else
			cmd->m_buttons &= ~IN_DUCK;

		g_cheat.m_lagmgr.set_state( ( choked >= 14 ) );
	}

	void c_extra::no_recoil( user_cmd_t* cmd ) {
		if( !g_settings.misc.no_recoil( ) )
			return;

		if( g_ctx.m_local && g_ctx.m_local->is_alive( ) ) {
			static auto weapon_recoil_scale = g_csgo.m_cvar( )->FindVar( xors( "weapon_recoil_scale" ) );
			cmd->m_viewangles  -= g_ctx.m_local->m_aimPunchAngle( true ) * weapon_recoil_scale->get_float( );
			cmd->m_viewangles.y = std::remainderf( cmd->m_viewangles.y, 360.f );
		}
	}

	void c_extra::rich_presence_flex( ) {
		g_csgo.m_steam_friends( )->set_rich_presence( "game:mode", "competitive" );
		g_csgo.m_steam_friends( )->set_rich_presence( "game:mapgroupname", "mg_active" );
		g_csgo.m_steam_friends( )->set_rich_presence( "game:score", "moneybot.cc" );
	}

	void c_extra::remove_rich_presence_flex( ) {
		g_csgo.m_steam_friends( )->clear_rich_presence( );
	}

	// I am so, so, so, so sorry.
	void c_extra::money_talk( IGameEvent *evt ) {
		if( !g_settings.misc.money_talk( ) )
			return;

		if( evt && !strcmp( evt->GetName( ), xors( "player_death" ) ) ) {
			const int player = g_csgo.m_engine( )->GetPlayerForUserID( evt->GetInt( xors( "attacker" ) ) );

			if( player == g_csgo.m_engine( )->GetLocalPlayer( ) ) {
				g_csgo.m_engine( )->ClientCmd( xors( "say god i wish i had moneybot" ) );
			}
		}
	}

	void c_extra::no_recoil( bool original ) {
		static bool restore = false;
		static vec3_t punch{ };
		static vec3_t view_punch{ };
		
		if( !g_settings.misc.no_recoil( ) || !g_ctx.m_local || !g_ctx.m_local->is_alive( ) )
			return;

		if( !original ) {
			punch = g_ctx.m_local->m_aimPunchAngle( true );
			view_punch = g_ctx.m_local->m_viewPunchAngle( );

			g_ctx.m_local->m_aimPunchAngle( true ) = vec3_t( );
			g_ctx.m_local->m_viewPunchAngle( ) = vec3_t( );

			restore = true;
		}

		if( original && restore ) {
			g_ctx.m_local->m_aimPunchAngle( true ) = punch;
			g_ctx.m_local->m_viewPunchAngle( ) = view_punch;

			restore = false;
		}
	}

	void c_extra::auto_revolver( user_cmd_t* cmd ) {
		if( !g_settings.misc.auto_revolver ) return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon ) return;

		int def_index = weapon->m_iItemDefinitionIndex( );
		if( def_index != WEAPON_REVOLVER ) return;

		static float next_time = 0.f;

		auto time = g_ctx.pred_time( );
		auto primaryattack = weapon->m_flNextPrimaryAttack( );
		auto nextattack = g_ctx.m_local->m_flNextAttack( );
		
		if( primaryattack < time && nextattack < time ) {
			if( next_time >= time ) {
				g_ctx.m_revolver_shot = false;
				cmd->m_buttons |= IN_ATTACK;
			}
			else {
				next_time = time + 0.234375f;
				g_ctx.m_revolver_shot = true;
			}
		}
		else {
			g_ctx.m_revolver_shot = false;
			next_time = time + 0.234375f;
		}
	}

	void c_extra::no_flash( ) {
		static bool once = false;

		if( !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) ) {
			return;
		}

		if( !g_settings.misc.no_flash ) {
			if( once ) {
				g_ctx.m_local->m_flMaxFlashAlpha( ) = 255.f;
			}
			return;
		}
		
		once = true;

		if( g_ctx.m_local->m_flFlashDuration( ) )
			g_ctx.m_local->m_flMaxFlashAlpha( ) = std::numeric_limits< float >::min( );
	}

	//this is not how it works
	void c_extra::disable_post_processing( ) {
		static auto var = g_csgo.m_cvar( )->FindVar( xors( "mat_postprocess_enable" ) );
		var->m_flags |= 0;
		var->set_value( !g_settings.misc.disable_post_process( ) );
	}

	void c_extra::float_ragdolls( ) {
		static bool once = true;
		static auto var = g_csgo.m_cvar( )->FindVar( xors( "cl_ragdoll_gravity" ) );

		if( !g_settings.visuals.floating_ragdolls ) {
			if( !once ) {
				var->set_value( 600 );
			}
			once = true;
			return;
		}

		once = false;
		var->m_flags |= 0;
		var->set_value( -100 );
	}

	/*void c_extra::print_overwatch_info( ) {
		for( int i{}; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );
			if( !ent->is_player( ) )
				continue;

			g_csgo.m_cvar( )->ConsolePrintf( "name: " );
			g_csgo.m_cvar( )->ConsolePrintf( "userid: " );
			g_csgo.m_cvar( )->ConsolePrintf( "steam3: " );
			g_csgo.m_cvar( )->ConsolePrintf( "steam64: " );
			g_csgo.m_cvar( )->ConsolePrintf( "steam community link: " );
			g_csgo.m_cvar( )->ConsolePrintf( "rank: " );
			g_csgo.m_cvar( )->ConsolePrintf( "wins: " );
		}
	}*/

	void c_extra::no_smoke( ) {
		static bool set = false;
		static bool last_setting = g_settings.misc.no_smoke;
		if( !g_ctx.m_local || !g_csgo.m_engine( )->IsInGame( ) ) {
			last_setting = !g_settings.misc.no_smoke;
			return;
		}

		if( g_settings.misc.no_smoke || last_setting != g_settings.misc.no_smoke ) {
			if( g_ctx.m_local && g_csgo.m_engine( )->IsInGame( ) ) {
				static auto v_smoke_smokegrenade = g_csgo.m_mat_system( )->FindMaterial( xors( "particle\\vistasmokev1\\vistasmokev1_smokegrenade" ) );
				bool draw = g_settings.misc.no_smoke( );

				if( v_smoke_smokegrenade )
					v_smoke_smokegrenade->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, draw );
			}
		}

		last_setting = g_settings.misc.no_smoke;
	}

	void c_extra::unlock_hidden_cvars( ) {
		// Wouldn't this get you untrusted?
		if ( g_settings.menu.anti_untrusted )
			return;

		auto it = g_csgo.m_cvar( )->FactoryInternalIterator( ); // genius code. list of cvars unlocked at https://github.com/saul/csgo-cvar-unhide/blob/master/cvarlist.md
		for( it->SetFirst( ); it->IsValid( ); it->Next( ) ) {
			auto cvar = ( cvar_t* )it->Get( );
			if( cvar ) {
				cvar->m_flags &= ~FCVAR_HIDDEN;		
				cvar->m_flags &= ~FCVAR_DEVELOPMENTONLY;
			}
		}
	}
}