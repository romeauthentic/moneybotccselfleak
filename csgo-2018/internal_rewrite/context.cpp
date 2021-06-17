#include "context.hpp"
#include "console.hpp"
#include "hooks.hpp"
#include "base_cheat.hpp"

context::c_context g_ctx;

NAMESPACE_REGION( context )

bool c_context::run_frame( ) {
	m_local = g_csgo.m_entlist( )->GetClientEntity< >(
		g_csgo.m_engine( )->GetLocalPlayer( ) );

	return !!m_local;
}

void c_context::on_cmove_end( user_cmd_t* cmd ) {
	if( g_settings.menu.anti_untrusted )
		cmd->clamp( );

	auto send = g_cheat.m_lagmgr.get_state( );

	if( send ) {
		m_last_fakeangle = cmd->m_viewangles;
		m_last_origin = m_local->m_vecOrigin( );
	}
	else {
		m_last_realangle = cmd->m_viewangles;
		m_last_realangle.x = m_last_fakeangle.x;
	}

	if( !g_cheat.m_lagmgr.get_choked( ) ) {
		m_thirdperson_angle = m_last_realangle;
	}

	m_thirdperson_angle.x = std::clamp( cmd->m_viewangles.x, -89.f, 89.f );
	if( g_cheat.m_lagmgr.get_sent( ) > 1 ) {
		m_thirdperson_angle.y = m_last_fakeangle.y;
	}

	//auto anim_state = g_cheat.m_ragebot.m_antiaim->get_animstate( );


	if( ( cmd->m_buttons & IN_ATTACK ) && m_local->can_attack( ) ) {
		auto weap = m_local->get_weapon( );
		if( !weap->is_knife( ) && weap->m_iItemDefinitionIndex( ) != WEAPON_TASER && !weap->is_grenade( ) ) {
			if( !m_has_incremented_shots && !m_has_fired_this_frame ) {
				shot_data_t new_shot{ };
				new_shot.m_angle = cmd->m_viewangles;
				new_shot.m_local_pos = m_local->get_eye_pos( );

				m_last_shot++;
				m_last_shot %= 128;
				m_shot_data[ m_last_shot ] = new_shot;


				//g_con->log( "incrementing shots( in attack )" );
				m_has_incremented_shots = true;
			}

			m_has_fired_this_frame = true;
		}
	}

	if (!m_local->get_weapon())
		return;

	weapon_info_t* wpn_info = m_local->get_weapon( )->get_wpn_info( );
	if( wpn_info && m_has_fired_this_frame )
		m_local->get_weapon( )->m_flNextPrimaryAttack( ) += wpn_info->cycle_time;

	if( g_settings.misc.log_hits( ) ) {
		for( auto& it : m_shot_data ) {
			if( !it.m_enemy_index )
				continue;

			if( !it.m_handled )
				break;

			if( !it.m_logged ) {
				char print_str[ 100 ]{ 0 };
				memset( print_str, 0, 100 );

				if( it.m_hit ) {
					player_info_t info;
					if( !g_csgo.m_engine( )->GetPlayerInfo( it.m_enemy_index, &info ) )
						continue;

					char name[ 32 ];
					auto ent = g_csgo.m_entlist( )->GetClientEntity( it.m_enemy_index );

					ent->get_name_safe( name );

					strenc::w_sprintf_s( print_str, 100, "[\3moneybot\1] hit %s in %s for %d\0", name, util::hitgroup_to_string( it.m_hit ).c_str( ), it.m_damage );
				}
				else if( it.m_missed )
					strenc::w_sprintf_s( print_str, 100, "[\3moneybot\1] missed shot due to spread\0" );

				g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, print_str );
				g_con->game_console_print( "%s\n", print_str );
			}

			it.m_logged = true;
		}
	}

	static float last_shot = 0.f;
	if( m_has_fired_this_frame )
		last_shot = g_csgo.m_globals->m_curtime;
	else if( auto weapon = g_ctx.m_local->get_weapon( ) ) {
		float fire_rate = weapon->get_wpn_info( )->cycle_time;
		float tolerance = util::get_total_latency( ) * 2.f + fire_rate + TICK_INTERVAL( );
		if( std::abs( last_shot - g_csgo.m_globals->m_curtime ) > tolerance ) {
			reset_shot_queue( );
			last_shot = g_csgo.m_globals->m_curtime;
		}
	}


	m_has_incremented_shots = false;
	if( std::abs( m_last_shot - m_last_shot_ack ) >= 10 )
		reset_shot_queue( );

	*get_last_cmd( ) = *cmd;
}

void c_context::reset_shot_queue( ) {
	auto last_shot = m_shot_data[ m_last_shot ];
	m_last_shot = m_last_shot_ack = 0;
	m_shot_data = { };
	m_shot_data[ 0 ] = last_shot;
}

void c_context::update_local_hook( ) {
	if( !run_frame( ) )
		return;

	auto func = util::get_vfunc< void* >( m_local, 218 );
	if( func != &hooks::update_clientside_animation || g_csgo.m_local( ) != m_local ) {
		g_csgo.m_local( m_local );
		g_csgo.m_local->hook( 218, &hooks::update_clientside_animation );
	}
}

//predicted servertime of player, use this for breaking lby etc
float c_context::pred_time( ) {
	calculate_tickbase( );

	return m_tickbase * g_csgo.m_globals->m_interval_per_tick;
}

//calculate tickbase depending on whether last ucmd was predicted
bool c_context::calculate_tickbase( ) {
	if ( !m_local ) {
		return false;
	}

	//get current tickbase
	auto player_tickbase = m_local->m_nTickBase( );

	//disabled due to our engine pred being shit
	m_tickbase = player_tickbase;
	return true;

	if ( m_snapshot.empty( ) ) {
		m_tickbase = player_tickbase;
		return false;
	}

	//if cmd wasnt predicted increment tickbase
	auto snap_cmd = &m_snapshot.front( );
	if ( !snap_cmd->m_predicted ) {
		if ( !m_tickbase ) {
			m_tickbase = player_tickbase;
		}

		m_tickbase++;
		snap_cmd->m_predicted = true;
	}
	else {
		m_tickbase = player_tickbase;
	}

	return true;
}

bool c_context::precache_model( const char* model ) {
	
	auto cache_table = g_csgo.m_string_table( )->FindTable( "modelprecache" );

	if( !cache_table )
		return true;

	g_csgo.m_model_info( )->FindOrLoadModel( model );

	int string_index = cache_table->AddString( false, model );

	if( string_index == -1 )
		return false;

	return true;
}

//save snapshots of usercommands
bool c_context::create_snapshot( user_cmd_t* ucmd ) {
	user_cmd_t ucmd_copy;

	while ( m_snapshot.size( ) >= 64 ) {
		m_snapshot.pop_back( );
	}

	if ( !ucmd ) {
		return false;
	}

	memcpy( &ucmd_copy,
		ucmd,
		sizeof( ucmd_copy ) );

	m_snapshot.push_front( ucmd_copy );
	return true;
}

END_REGION