#include "base_cheat.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"
#include "hooks.hpp"
#include "interface.hpp"
#undef min


namespace features
{
	void c_extra::rank_reveal( user_cmd_t* ucmd ) {
		if( !( ucmd->m_buttons & IN_SCORE ) || !g_settings.misc.rank_reveal( ) ) return;

		vec3_t a{ }; //stop this
		static uintptr_t show_rank_addr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 8B 0D ? ? ? ? 68" ) );
		//reinterpret_cast<bool( __cdecl* )( vec3_t* )>( show_rank_addr )( &a );

	}
	
	void c_extra::thirdperson( ) {
		if( !g_ctx.m_local )
			return;

		static bool enabled = false;
		static bool toggle = true;
		static bool key_held = false;


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
			g_csgo.m_engine( )->GetViewAngles( viewangles );
			g_csgo.m_input( )->m_fCameraInThirdPerson = true;
			g_csgo.m_input( )->m_vecCameraOffset = vec3_t( viewangles.x, viewangles.y, 150.0f );

			constexpr float    cam_hull_offset{ 16.f };

			const     vec3_t   cam_hull_min( -cam_hull_offset, -cam_hull_offset, -cam_hull_offset );
			const     vec3_t   cam_hull_max( cam_hull_offset, cam_hull_offset, cam_hull_offset );
			vec3_t             cam_forward, origin = g_ctx.m_local->get_eye_pos( );

			cam_forward = math::angle_vectors( vec3_t( viewangles.x, viewangles.y, 0.f ) );

			CTraceFilterWorldAndPropsOnly filter;
			Ray_t						  ray;
			CGameTrace					  tr;

			ray.Init( origin, origin - ( cam_forward * 150.f ), cam_hull_min, cam_hull_max );

			g_csgo.m_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr );
			g_csgo.m_input( )->m_vecCameraOffset.z = 150.f * tr.fraction;

			enabled = true;
		}
		else if( g_csgo.m_entlist( )->GetClientEntityFromHandle< >( g_ctx.m_local->m_hObserverTarget( ) ) ) {
			g_csgo.m_input( )->m_fCameraInThirdPerson = false;
			g_ctx.m_local->m_iObserverMode( ) = 5;
		}
	}

	void c_extra::update_netchannel( ) {
		if( !g_csgo.m_engine( )->IsInGame( ) || !g_ctx.m_local || 
			!g_ctx.m_local->is_valid( ) ) {
			return;
		}

		switch( g_settings.misc.net_fakelag ) {
		case 0:
			g_settings.misc.net_fakeping_active = false;
			break;
		case 1:
			g_settings.misc.net_fakeping_active = g_input.is_key_pressed( g_settings.misc.net_fakeping_key );
			break;
		case 2: {
			static bool held = false;
			bool pressed = g_input.is_key_pressed( g_settings.misc.net_fakeping_key );
			if( pressed ) {
				if( !held )
					g_settings.misc.net_fakeping_active ^= 1;
				held = true;
			}
			else held = false;
		}
				break;
		case 3:
			g_settings.misc.net_fakeping_active = true;
			break;
		default:
			g_settings.misc.net_fakeping_active = false;
			break;
		}

		auto clientstate = g_csgo.m_global_state->get_client_state( );
		if( clientstate ) {
			auto netchannel = clientstate->m_netchannel;
			if( netchannel ) {
				auto old_netchannel = g_csgo.m_net_channel( );

				if( old_netchannel != netchannel || 
					( old_netchannel && 
						g_csgo.m_net_channel->get_function< void* >( 46 ) != &hooks::send_datagram ) ) {
					g_csgo.m_net_channel( netchannel );
					g_csgo.m_net_channel->hook( 46, &hooks::send_datagram );
				}
			}
		}
	}

	void c_extra::add_latency( INetChannel* channel ) {
		if (!g_settings.misc.net_fakelag())
			return;
		if (g_ctx.m_local && g_ctx.m_local->is_valid()) {

			auto nci = g_csgo.m_engine()->GetNetChannelInfo();
			float in_latency = nci->GetLatency(0);

			float aaa = g_settings.misc.net_fakeping_amount() * 0.001f;

			// amount of latency we want to achieve
			float latency = 0.15f - (in_latency + g_csgo.m_globals->m_frametime) - TICK_INTERVAL();

			if (g_ctx.m_local && g_ctx.m_local->is_valid()) {
				// god this is autistic
				for (auto& it : m_net_records) {
					float delta = g_csgo.m_globals->m_curtime - it.m_curtime;

					if (delta >= latency) {
						// apply latency
						channel->m_nInReliableState = it.m_reliable;
						channel->m_nInSequenceNr += 2 * 64 - (63 * aaa);

						break;
					}
				}
			}
		}
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

	void c_extra::fake_duck(user_cmd_t* cmd) {

		if (!g_settings.misc.fake_duck)
			return;

		if (!g_input.is_key_pressed(g_settings.misc.fake_duck_key))
			return;

		cmd->m_buttons |= IN_BULLRUSH;

		int choked = g_cheat.m_lagmgr.get_choked();
		if (choked > 6)
			cmd->m_buttons |= IN_DUCK;
		else
			cmd->m_buttons &= ~IN_DUCK;

		g_cheat.m_lagmgr.set_state((choked >= 14));
	}

	void c_extra::money_talk(IGameEvent* evt) {
		if (!g_settings.misc.money_talk())
			return;

		if (evt && !strcmp(evt->GetName(), xors("player_death"))) {
			const int player = g_csgo.m_engine()->GetPlayerForUserID(evt->GetInt(xors("attacker")));

			if (player == g_csgo.m_engine()->GetLocalPlayer()) {
				g_csgo.m_engine()->ClientCmd(xors("say god i wish i had moneybot"));
			}
		}
	}

	void c_extra::rich_presence_flex( ) {
		g_csgo.m_steam_friends( )->set_rich_presence( "game:mode", "competitive" );
		g_csgo.m_steam_friends( )->set_rich_presence( "game:mapgroupname", "mg_active" );
		g_csgo.m_steam_friends( )->set_rich_presence( "game:score", "moneybot.cash" );
	}

	void c_extra::remove_rich_presence_flex( ) {
		g_csgo.m_steam_friends( )->clear_rich_presence( );
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
		if( def_index != WEAPON_R8REVOLVER ) return;

		if( !g_ctx.m_local->can_attack( ) ) {
			cmd->m_buttons |= IN_ATTACK;
		}
		else if( !g_cheat.m_lagmgr.get_sent( ) ) {
			g_cheat.m_lagmgr.set_state( true );
		}
	}

	void c_extra::no_flash( ) {
		static bool once = false;
		if( !g_ctx.m_local || !g_ctx.m_local->is_valid( ) ) {
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
		static bool once = true;;
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

	void c_extra::server_information( ) {
/*		if( !g_settings.misc.server_information( ) )
			return;

		auto lobby = g_csgo.m_mm( )->GetMatchSession( );
		if( !lobby )
			return;
		
		auto info = lobby->GetSessionSettings( );
		if( !info )
			return;
		
		auto q = info->get_string( xors( "game/mmqueue" ), xors( "no queue!" ) );
		if( !q || strcmp( q, xors( "reserved" ) ) ) 
			return;
	
		auto map = info->get_string( xors( "game/map" ), xors( "no map!" ) );
		if( !map )
			return;
	
		auto location = info->get_string( xors( "game/loc" ), xors( "no location!" ) );

		g_csgo.m_cvar( )->ConsoleColorPrintf( clr_t( 231, 105, 105 ), xors( "\n-----------------------\n" ) );
		g_csgo.m_cvar( )->ConsoleColorPrintf( clr_t( 231, 105, 105 ), xors( "map: %s\n" ), map );
		g_csgo.m_cvar( )->ConsoleColorPrintf( clr_t( 231, 105, 105 ), xors( "location: %s" ), location );
		g_csgo.m_cvar( )->ConsoleColorPrintf( clr_t( 231, 105, 105 ), xors( "\n-----------------------\n" ) );
		*/
	}

	void c_extra::unlock_hidden_cvars( ) {
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