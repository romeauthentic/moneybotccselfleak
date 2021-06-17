#include "ctx.hpp"
#include "console.hpp"
#include "hooks.h"
#include "base_cheat.h"

context::c_context g_ctx;

NAMESPACE_REGION( context )

bool c_context::run_frame( ) {
	m_local = cl.m_entlist( )->get_client_entity< c_base_player >(
		cl.m_engine( )->GetLocalPlayer( ) );

	return !!m_local;
}


//predicted servertime of player, use this for breaking lby etc
float c_context::pred_time( ) {
	calculate_tickbase( );

	return m_tickbase * cl.m_globals->interval_per_tick;
}

//calculate tickbase depending on whether last ucmd was predicted
bool c_context::calculate_tickbase( ) {
	if( !m_local ) {
		return false;
	}

	//get current tickbase
	auto player_tickbase = m_local->get_tick_base( );

	//disabled due to our engine pred being shit
	m_tickbase = player_tickbase;
	return true;

	if( m_snapshot.empty( ) ) {
		m_tickbase = player_tickbase;
		return false;
	}

	//if cmd wasnt predicted increment tickbase
	auto snap_cmd = &m_snapshot.front( );
	if( !snap_cmd->m_predicted ) {
		if( !m_tickbase ) {
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

	auto cache_table = cl.m_string_table( )->FindTable( "modelprecache" );

	if( !cache_table )
		return true;

	cl.m_modelinfo( )->FindOrLoadModel( model );

	int string_index = cache_table->AddString( false, model );

	if( string_index == -1 )
		return false;

	return true;
}

//save snapshots of usercommands
bool c_context::create_snapshot( user_cmd_t* ucmd ) {
	user_cmd_t ucmd_copy;

	while( m_snapshot.size( ) >= 64 ) {
		m_snapshot.pop_back( );
	}

	if( !ucmd ) {
		return false;
	}

	memcpy( &ucmd_copy,
		ucmd,
		sizeof( ucmd_copy ) );

	m_snapshot.push_front( ucmd_copy );
	return true;
}

END_REGION