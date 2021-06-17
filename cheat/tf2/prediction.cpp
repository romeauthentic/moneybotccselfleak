#include "prediction.h"
#include "sdk.h"
#include "interfaces.h"

void features::c_prediction::run_prediction( user_cmd_t* cmd ) {
	static uintptr_t addr_of_runcommand = cl.m_prediction->get_function( 17 );
	static uint32_t pseudo_random_offset = pattern::first_code_match( cl.m_chl.dll( ), "55 8B EC 8B 45 08 85 C0 75 0C", 0x1 );
	static char* move_data = reinterpret_cast< char* >( malloc( 0x256 ) );

	if ( !cmd || !g_ctx.m_local ) return;

	int flags = g_ctx.m_local->get_flags( );
	int cmd_buttons = cmd->m_buttons;

	auto set_random_seed = reinterpret_cast< void( __cdecl* )( uint32_t ) >( pseudo_random_offset );

	*reinterpret_cast< user_cmd_t** >( uintptr_t( g_ctx.m_local ) + 0x107c ) = cmd;

	if ( cmd->m_impulse ) {
		*reinterpret_cast< uint8_t* >( uintptr_t( g_ctx.m_local ) + 0x1124 ) = cmd->m_impulse;
	}

	/*void __cdecl SetButtonState(int a1)
	{
	int v1; // ecx@0
	int v2; // edx@1

	 v2 = a1 ^ *(_DWORD *)(v1 + 0x1078);
	*(_DWORD *)(v1 + 0x106C) = *(_DWORD *)(v1 + 0x1078);
	*(_DWORD *)(v1 + 0x1078) = a1;
	 *(_DWORD *)(v1 + 0x1070) = a1 & v2;
	 *(_DWORD *)(v1 + 0x1074) = v2 & ~a1;
	}*/
	auto v2 = cmd->m_buttons ^ *reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x1078 );

	*reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x106c ) = *reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x1078 );
	*reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x1078 ) = cmd->m_buttons;
	*reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x1070 ) = cmd->m_buttons & v2;
	*reinterpret_cast< int* >( uintptr_t( g_ctx.m_local ) + 0x1074 ) = v2 &~ cmd->m_buttons;

	cl.m_globals->curtime = g_ctx.m_local->get_tick_base( ) * cl.m_globals->interval_per_tick;
	cl.m_globals->frametime = cl.m_globals->interval_per_tick;

	set_random_seed( cmd->m_random_seed );
	**reinterpret_cast< uintptr_t** >( addr_of_runcommand + 0x2b + 0x2 ) = uintptr_t( g_ctx.m_local );

	cl.m_movement( )->StartTrackPredictionErrors( reinterpret_cast< uintptr_t* >( g_ctx.m_local ) );

	int tick_base = g_ctx.m_local->get_tick_base( );

	static auto setup_move = cl.m_prediction->get_function< void( __thiscall* )( void*, c_base_player*, user_cmd_t*, IMoveHelper*, CMoveData* ) >( 18 );
	static auto finish_move = cl.m_prediction->get_function< void( __thiscall* )( void*, c_base_player*, user_cmd_t*, CMoveData* ) >( 19 );
	setup_move( cl.m_prediction( ), g_ctx.m_local, cmd, cl.m_movehelper, reinterpret_cast< CMoveData* >( move_data ) );
	cl.m_movement( )->ProcessMovement( reinterpret_cast< uintptr_t* >( g_ctx.m_local ), reinterpret_cast< CMoveData* >( move_data ) );
	finish_move( cl.m_prediction( ), g_ctx.m_local, cmd, reinterpret_cast< CMoveData* >( move_data ) );

	g_ctx.m_local->m_nTickBase( );

	cl.m_movement( )->FinishTrackPredictionErrors( reinterpret_cast< uintptr_t* >( g_ctx.m_local ) );

	set_random_seed( 0xfffffff );
	**reinterpret_cast< uintptr_t*** >( addr_of_runcommand + 0x2b + 0x2 ) = nullptr;
	*reinterpret_cast< user_cmd_t** >( uintptr_t( g_ctx.m_local ) + 0x107c ) = nullptr;

	cl.m_globals->curtime = g_ctx.m_local->get_tick_base( ) * cl.m_globals->interval_per_tick;
	cl.m_globals->frametime = cl.m_globals->interval_per_tick;

	g_ctx.m_local->m_nTickBase( );
	cmd->m_buttons = cmd_buttons;
}