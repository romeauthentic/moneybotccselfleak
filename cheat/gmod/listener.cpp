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
	bool dbg_spread( ) {
		static con_var< bool > var{ &data::holder_, fnv( "dbg_spread" ), 0 };
		return var( );
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
			if( std::abs( last_time - g_csgo.m_globals->m_curtime ) > ( 1.f / 64.f ) &&
				player_id == g_csgo.m_engine( )->GetLocalPlayer( ) ) {


				g_ctx.m_last_shot_ack++;
				g_ctx.m_last_shot_ack %= 128;

				if( std::abs( last_time - g_csgo.m_globals->m_curtime ) > 1.0f ) {
					if( std::abs( g_ctx.m_last_shot - g_ctx.m_last_shot_ack ) ) {
						//if( dbg_spread( ) )
							//g_con->log( "shot queue desynced: delta too high ( %d %d )", g_ctx.m_last_shot_ack, g_ctx.m_last_shot );
						g_ctx.reset_shot_queue( );
					}
				}

				if( g_ctx.m_last_shot_ack > g_ctx.m_last_shot ) {
					//if( dbg_spread( ) )
						//g_con->log( "shot queue desynced: ack > last" );
					g_ctx.reset_shot_queue( );
				}

				auto& shot = g_ctx.m_shot_data[ g_ctx.m_last_shot_ack ];
				//if( dbg_spread( ) )
					//g_con->log( "shot: %d ack shot: %d", g_ctx.m_last_shot, g_ctx.m_last_shot_ack );

				if( shot.m_enemy_index ) {
					auto ang = math::vector_angles( shot.m_local_pos, shot.m_enemy_pos );
					auto shot_ang = math::vector_angles( shot.m_local_pos, impact );

					auto dist = shot.m_local_pos.dist_to( shot.m_enemy_pos );
					auto ang_delta = ( ang - shot_ang ).clamp( );

					float pitch_delta = sin( DEG2RAD( std::abs( ang_delta.x ) ) ) * dist;
					float yaw_delta   = sin( DEG2RAD( std::abs( ang_delta.y ) ) ) * dist;

					float delta = sqrtf( pitch_delta * pitch_delta + yaw_delta * yaw_delta );
					//scale it up a bit, hitboxes arent *really* boxes anymore
					float max = shot.m_hitbox_radius;

					if( delta > max ) {
						if( dbg_spread( ) )
							g_con->log( xors( "missed shot due to spread: %f > %f" ), delta, max );
						if( g_settings.misc.log_hits ) {
							g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, xors( "[\3moneybot\1] missed shot due to spread" ) );
						}

						if( shot.m_resolver_state ) {
							g_cheat.m_ragebot.m_resolver->on_missed_spread( shot.m_enemy_index, shot.m_resolver_shots );
						}
					}
					else {
						shot.m_missed = false;
					}
				}

				last_time = g_csgo.m_globals->m_curtime;
			}

			if( g_settings.visuals.bullet_tracers( ) ) {
				g_cheat.m_visuals.store_tracer( player_id, vec3_t( x, y, z ) );
			}
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
			

			if( attacker_id == g_csgo.m_engine( )->GetLocalPlayer( ) && user_id != attacker_id ) {
				g_cheat.m_visuals.store_hit( );

				auto& shot = g_ctx.m_shot_data[ g_ctx.m_last_shot_ack % 128 ];
				if( hitgroup == shot.m_hitgroup && shot.m_resolver_state && !shot.m_missed ) {
					g_cheat.m_ragebot.m_resolver->listener( player_id, shot.m_resolver_shots );
				}

				if( !shot.m_missed || !shot.m_resolver_state ) {
					//if( dbg_spread( ) )
						//g_con->log( xors( "hit enemy in %s" ), util::hitgroup_to_string( hitgroup ).c_str( ) );

					if( g_settings.misc.log_hits ) {
						char hit_str[ 100 ];
						if( hitgroup == HITGROUP_HEAD && hp <= 0 ) {
							strenc::w_sprintf_s( hit_str, 100, xors( "[\3moneybot\1] pHit" ) );
						}
						else {
							strenc::w_sprintf_s( hit_str, 100, xors( "[\3moneybot\1] hit player in %s for %d" ), util::hitgroup_to_string( hitgroup ).c_str( ), dmg );
						}
						g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, hit_str );
					}
				}
			}
		}
	}

	void round_start( IGameEvent * e ) {
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
}