#include "context.hpp"
#include "console.hpp"
#include "hooks.hpp"
#include "base_cheat.hpp"

context::c_context g_ctx;

NAMESPACE_REGION( context )

bool c_context::run_frame( ) {
	m_local = g_gmod.m_entlist( )->GetClientEntity< >(
		g_gmod.m_engine( )->GetLocalPlayer( ) );

	return !!m_local;
}
//
void c_context::on_cmove_end( user_cmd_t* cmd ) {
	auto send = g_cheat.m_lagmgr.get_state( );

	if( send ) {
		m_thirdperson_angle = m_last_realangle;
		m_last_fakeangle = cmd->m_viewangles;
		m_last_origin = m_local->m_vecOrigin( );
	}
	else {
		m_last_realangle = cmd->m_viewangles;
		m_last_realangle.x = m_last_fakeangle.x;
	}

	m_thirdperson_angle.x = cmd->m_viewangles.x;
	if( g_cheat.m_lagmgr.get_sent( ) > 1 ) {
		m_thirdperson_angle.y = m_last_fakeangle.y;
	}
	*get_last_cmd( ) = *cmd;
}
//
//void c_context::reset_shot_queue( ) {
//	m_last_shot = m_last_shot_ack = 0;
//	m_shot_data = { };
//}
//
////predicted servertime of player, use this for breaking lby etc
//float c_context::pred_time( ) {
//	calculate_tickbase( );
//
//	return m_tickbase * g_gmod.m_globals->m_interval_per_tick;
//}
//
////calculate tickbase depending on whether last ucmd was predicted
//bool c_context::calculate_tickbase( ) {
//	if ( !m_local ) {
//		return false;
//	}
//
//	//get current tickbase
//	auto player_tickbase = m_local->m_nTickBase( );
//
//	//disabled due to our engine pred being shit
//	m_tickbase = player_tickbase;
//	return true;
//
//	if ( m_snapshot.empty( ) ) {
//		m_tickbase = player_tickbase;
//		return false;
//	}
//
//	//if cmd wasnt predicted increment tickbase
//	auto snap_cmd = &m_snapshot.front( );
//	if ( !snap_cmd->m_predicted ) {
//		if ( !m_tickbase ) {
//			m_tickbase = player_tickbase;
//		}
//
//		m_tickbase++;
//		snap_cmd->m_predicted = true;
//	}
//	else {
//		m_tickbase = player_tickbase;
//	}
//
//	return true;
//}
//
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