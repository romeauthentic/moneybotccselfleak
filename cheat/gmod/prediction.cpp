#include "prediction.hpp"
#include "hooks.hpp"
#include "context.hpp"

NAMESPACE_REGION( features )

void c_prediction::predict_player( c_base_player* player ) {
	static uintptr_t run_command_address = g_gmod.m_prediction->get_old_function< uintptr_t >( 17 );
	static uintptr_t set_prediction_random_seed_address = ( run_command_address + 0x21 + 4 + *( uintptr_t* )( run_command_address + 0x21 ) );


	CMoveData move_data{ };
	auto ce = player->ce( );

	float old_curtime = g_gmod.m_globals->m_curtime;
	float old_frametime = g_gmod.m_globals->m_frametime;

	int backup_tickbase = player->m_nTickBase( );
	int backup_simtime = player->m_flSimulationTime( );


	//random seed is already being calculated and set in createmove
	**( uintptr_t** )( set_prediction_random_seed_address + 0xC ) = g_ctx.get_last_cmd( )->m_random_seed; //prediction seed
	**( uintptr_t** )( run_command_address + 0x2F ) = uintptr_t( g_ctx.m_local ); //prediction player

	//start prediction
	g_gmod.m_game_movement( )->StartTrackPredictionErrors( ce );

	//run prediction
	g_gmod.m_prediction( )->SetupMove( ce, *player->get< user_cmd_t** >( 0x24B0 ), g_gmod.m_move_helper( ), &move_data );
	g_gmod.m_game_movement( )->ProcessMovement( ce, &move_data );
	g_gmod.m_prediction( )->FinishMove( ce, *player->get< user_cmd_t** >( 0x24B0 ), &move_data );

	//finish prediction
	g_gmod.m_game_movement( )->FinishTrackPredictionErrors( ce );

	**( uintptr_t** )( set_prediction_random_seed_address + 0xC ) = 0xffffffff;
	**( uintptr_t*** )( run_command_address + 0x2F ) = nullptr;

	player->m_nTickBase( ) = backup_tickbase;
	player->m_flSimulationTime( ) = backup_simtime;

	g_gmod.m_globals->m_curtime = old_curtime;
	g_gmod.m_globals->m_frametime = old_frametime;
}

void c_prediction::run( user_cmd_t* ucmd ) {	
	if ( !ucmd || !g_ctx.m_local || !g_ctx.m_local->is_alive( ) )
		return;

	static uintptr_t run_command_address = g_gmod.m_prediction->get_old_function< uintptr_t >( 17 );
	static uintptr_t relative_call_addr = run_command_address + 0x21;
	static uintptr_t set_prediction_random_seed_address = ( relative_call_addr + 4 + *( uintptr_t* )( relative_call_addr ) );

	static auto sv_footsteps = g_gmod.m_cvar( )->FindVar( "sv_footsteps" );

	CMoveData move_data{ };
	IClientEntity* local_ent   = g_ctx.m_local->ce( );

	//backup data
	int old_buttons            = ucmd->m_buttons;
	float old_curtime          = g_gmod.m_globals->m_curtime;
	float old_frame_time       = g_gmod.m_globals->m_frametime;
	int old_tickbase           = g_ctx.m_local->m_nTickBase( );
	int old_flags              = g_ctx.m_local->m_fFlags( );
	MoveType_t old_move_type   = g_ctx.m_local->m_nMoveType( );
	vec3_t old_velocity        = g_ctx.m_local->m_vecVelocity( );
	int old_sv_flags		   = sv_footsteps->m_flags;

	sv_footsteps->m_flags = 0;
	sv_footsteps->m_nvalue = 0;

	//set globals
	g_gmod.m_globals->m_curtime = g_gmod.m_globals->m_interval_per_tick * old_tickbase;
	g_gmod.m_globals->m_frametime = g_gmod.m_globals->m_interval_per_tick;

	//random seed is already being calculated and set in createmove
	**( uintptr_t** )( set_prediction_random_seed_address + 0xC ) = ucmd->m_random_seed; //prediction seed
	**( uintptr_t** )( run_command_address + 0x2F ) = uintptr_t( g_ctx.m_local ); //prediction player
		
	//start prediction
	g_gmod.m_game_movement( )->StartTrackPredictionErrors( local_ent );
	
	//run prediction
	g_gmod.m_prediction( )->SetupMove( local_ent, ucmd, nullptr, &move_data );
	g_gmod.m_game_movement( )->ProcessMovement( local_ent, &move_data );
	g_gmod.m_prediction( )->FinishMove( local_ent, ucmd, &move_data );

	//finish prediction
	g_gmod.m_game_movement( )->FinishTrackPredictionErrors( local_ent );

	**( uintptr_t** )( set_prediction_random_seed_address + 0xC ) = 0xffffffff;
	**( uintptr_t*** )( run_command_address + 0x2F ) = nullptr;

	//good to have, can be used for edge jump and such
	m_predicted_flags = g_ctx.m_local->m_fFlags( ); 

	//restore
	ucmd->m_buttons					= old_buttons;
	g_gmod.m_globals->m_curtime     = old_curtime;
	g_gmod.m_globals->m_frametime   = old_frame_time;
	g_ctx.m_local->m_nTickBase( )   = old_tickbase;
	g_ctx.m_local->m_fFlags( )      = old_flags;
	g_ctx.m_local->m_nMoveType( )   = old_move_type;
	g_ctx.m_local->m_vecVelocity( ) = old_velocity;

	sv_footsteps->m_nvalue = 1;
	sv_footsteps->m_flags = old_sv_flags;
}

END_REGION