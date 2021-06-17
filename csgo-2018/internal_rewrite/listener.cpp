#include "listener.hpp"
#include "interface.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

#undef PlaySound

generic_listener_t::generic_listener_t( const char* name, void( *function )( IGameEvent* ) ) :
	m_function( function ), m_name( name ) { }

generic_listener_t::~generic_listener_t( ) {
	if( m_registered )
		g_csgo.m_event_mgr( )->RemoveListener( this );

}

void generic_listener_t::init( ) {
	g_csgo.m_event_mgr( )->AddListener( this, m_name, false );
	m_registered = true;
}

namespace listeners
{	
	void weapon_fire( IGameEvent* e ) {
		if( !g_csgo.m_panic && e ) {
			int userid = e->GetInt( xors( "userid" ) );
			if( g_csgo.m_engine( )->GetPlayerForUserID( userid ) == g_csgo.m_engine( )->GetLocalPlayer( ) ) {
				auto wep = e->GetString( xors( "weapon" ) );

				if( !strstr( wep, xors( "knife" ) ) && !strstr( wep, xors( "taser" ) ) && !strstr( wep, xors( "molotov" ) ) && !strstr( wep, xors( "grenade" ) ) && !strstr( wep, xors( "flashbang" ) ) ) {
					g_ctx.m_last_shot_ack++;
					g_ctx.m_last_shot_ack %= 128;

					auto m_last_shot = g_ctx.m_last_shot_ack;
					auto m_shot_data = g_ctx.m_shot_data;

					auto& shot = g_ctx.m_shot_data[ g_ctx.m_last_shot_ack ];
					if( shot.m_enemy_index && shot.m_resolver_state ) {
						//g_cheat.m_ragebot.m_resolver->increment_shots( shot.m_enemy_index );
					}
				}
			}
		}
	}

	void player_death(IGameEvent* e) {
		g_cheat.m_extra.money_talk(e);
	}

	void bullet_impact( IGameEvent* e ) {
		if( !g_csgo.m_panic && e ) {
			static float last_time = g_csgo.m_globals->m_curtime;
			float x = e->GetFloat( xors( "x" ) );
			float y = e->GetFloat( xors( "y" ) );
			float z = e->GetFloat( xors( "z" ) );
			int user_id = e->GetInt( xors( "userid" ) );
			int player_id = g_csgo.m_engine( )->GetPlayerForUserID( user_id );

			vec3_t impact{ x, y, z };
			if( player_id == g_csgo.m_engine( )->GetLocalPlayer( ) ) {
				g_cheat.m_visuals.store_shot( impact, true );
				
				if( g_ctx.m_last_shot_ack > g_ctx.m_last_shot ) {
					g_ctx.reset_shot_queue( );
				}

				auto& shot = g_ctx.m_shot_data[ g_ctx.m_last_shot_ack ];

				if( shot.m_enemy_index && !shot.m_handled ) {
					vec3_t ang = math::vector_angles( shot.m_local_pos, impact );
					vec3_t dir = math::angle_vectors( ang );
					//csgo is gay
					dir *= 8192.f;

					vec3_t pos = shot.m_local_pos;

					bool intersect = util::intersects_hitbox( pos, dir, shot.m_hitbox.min * 1.05f, shot.m_hitbox.max * 1.05f, shot.m_hitbox.radius * 1.05f );

					if( !intersect ) {
						if( shot.m_enemy_index ) {
							g_cheat.m_ragebot.m_resolver->on_missed_spread( shot.m_enemy_index, g_ctx.m_last_shot_ack );
							shot.m_missed = true;
						}
					}
					else {
						if( shot.m_resolver_state ) {
							//g_cheat.m_ragebot.m_resolver->increment_shots( shot.m_enemy_index );
						}

						shot.m_missed = false;
					}
					shot.m_handled = true;
				}
			}

			if( g_settings.visuals.bullet_tracers( ) && last_time != g_csgo.m_globals->m_curtime ) {
				g_cheat.m_visuals.store_tracer( player_id, vec3_t( x, y, z ) );
			}

			last_time = g_csgo.m_globals->m_curtime;
		}
	}

	void player_hurt( IGameEvent* e ) {
		if( !g_csgo.m_panic && e ) {

			int user_id = e->GetInt( xors( "userid" ) );
			int attacker = e->GetInt( xors( "attacker" ) );
			int hitgroup = e->GetInt( xors( "hitgroup" ) );
			int dmg = e->GetInt( xors( "dmg_health" ) );
			int hp = e->GetInt( xors( "health" ) );
			int attacker_id = g_csgo.m_engine( )->GetPlayerForUserID( attacker );
			int player_id = g_csgo.m_engine( )->GetPlayerForUserID( user_id );

			g_cheat.m_visuals.store_ent_dmg( attacker_id, player_id, dmg );
			if( attacker_id == g_csgo.m_engine( )->GetLocalPlayer( ) && user_id != attacker_id ) {
				g_cheat.m_visuals.store_hit( );

				auto& shot = g_ctx.m_shot_data[ g_ctx.m_last_shot_ack % 128 ];

				if( shot.m_hitgroup == hitgroup && shot.m_resolver_state && !shot.m_missed && hitgroup != HITGROUP_GENERIC ) {
					g_cheat.m_ragebot.m_resolver->listener( player_id, g_ctx.m_last_shot_ack );
				}

				if( shot.m_enemy_index && hitgroup != HITGROUP_GENERIC ) {
					shot.m_hit = hitgroup;
					shot.m_handled = true;
					shot.m_logged = false;
					shot.m_damage = dmg;
				}
				else if( g_settings.misc.log_hits( ) ) {
					char print_str[ 100 ];
					sprintf_s( print_str, "[\3moneybot\1] hit player in %s for %d", util::hitgroup_to_string( hitgroup ).c_str( ), dmg );

					g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, print_str );
					g_con->game_console_print( "%s\n", print_str );
				}

				if( shot.m_enemy_index && !shot.m_missed ) {
					//g_cheat.m_visuals.store_shot( shot.m_enemy_pos, true );
				}
			}
		}
	}

	void round_start( IGameEvent* e ) {
		g_cheat.m_visuals.on_round_start( );

		if( g_ctx.run_frame( ) ) {
			switch( g_settings.misc.autobuy.main_weapon ) {
			case 1:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy g3sg1" ) );
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy scar20" ) );
				break;
			case 2:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy ssg08" ) );
				break;
			case 3:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy awp" ) );
				break;
			case 4:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy ak47" ) );
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy m4a1" ) );
				break;
			case 5:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy ssg556" ) );
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy aug" ) );
				break;
			case 6:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy mac10" ) );
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy mp9" ) );
				break;
			}

			switch( g_settings.misc.autobuy.secondary_weapon ) {
			case 1:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy deagle" ) );
				break;
			case 2:
				g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy elite" ) );
				break;
			}

			if( g_ctx.m_local->m_iAccount( ) > 1000 ) {
				if( g_settings.misc.autobuy.armor( ) ) {
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy vest" ) );
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy vesthelm" ) );
				}

				if( g_settings.misc.autobuy.zeus( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy taser 34" ) );


				if( g_settings.misc.autobuy.defuser( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy defuser" ) );


				if( g_settings.misc.autobuy.molly( ) ) {
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy molotov" ) );
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy incgrenade" ) );
				}

				if( g_settings.misc.autobuy.smoke( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy smokegrenade" ) );


				if( g_settings.misc.autobuy.grenade( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy hegrenade" ) );


				if( g_settings.misc.autobuy.flash( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy flashbang" ) );


				if( g_settings.misc.autobuy.decoy( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy decoy" ) );


				if( g_settings.misc.autobuy.flash( ) )
					g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "buy flashbang" ) );
			}
		}
	}

	void molotov_detonate( IGameEvent* e ) {
		//1 line pro
		float x = e->GetFloat( xors( "x" ) );
		float y = e->GetFloat( xors( "y" ) );
		float z = e->GetFloat( xors( "z" ) );
		if( !( x && y && z ) )
			return;

		vec3_t pos{ x, y, z };
		auto thrower = g_csgo.m_engine( )->GetPlayerForUserID( e->GetInt( xors( "userid" ) ) );
		g_cheat.m_visuals.store_firegrenades( thrower, pos );

	}

}