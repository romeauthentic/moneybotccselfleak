#include <intrin.h>

#include "hooks.hpp"
#include "mem.hpp"
#include "interface.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"

#include "base_cheat.hpp"


bool __fastcall hooks::create_move( void* ecx_, void* edx_, float input_sample_frametime, user_cmd_t* ucmd ) {
	static auto create_move_o = g_gmod.m_clientmode->get_old_function< decltype( &hooks::create_move ) >( 21 );

	// call from CInput::ExtraMouseSample with dummy cmd.
	if( !ucmd->m_cmd_nr || !ucmd->m_tick_count || g_gmod.m_panic )
		return create_move_o( ecx_, 0, input_sample_frametime, ucmd );

	// for interfacing with lua related stuff.
	g_ctx.m_lua = g_gmod.m_lua_shared( )->GetLuaInterface( LUA_CLIENT );
	if( !g_ctx.m_lua )
		return create_move_o( ecx_, 0, input_sample_frametime, ucmd );

	// random_seed isn't generated in ClientMode::CreateMove yet (since CInput::CreateMove handles that), we must generate it ourselves.
	ucmd->m_random_seed = math::md5_pseudorandom( ucmd->m_cmd_nr ) & 0x7fffffff;

	//get sendpacket off the stack
	stack_t stack( get_baseptr( ) );
	byte    *send_packet = stack.next( ).next( ).next( ).local< byte* >( 0x1 );

	g_ctx.create_snapshot( ucmd );

	//return create_move_o( ecx_, 0, input_sample_frametime, ucmd );

	/*
	g_cheat.m_extra.update_netchannel( );
	g_cheat.m_identity( );
	*/

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		static int last_frame = g_gmod.m_globals->m_framecount;
		if( last_frame != g_gmod.m_globals->m_framecount )
			g_ctx.m_has_fired_this_frame = false;

		last_frame = g_gmod.m_globals->m_framecount;

		// hook fire bullets for spread compensation.
		if( !fire_bullets_o ) {
			fire_bullets_o = decltype( &hooks::fire_bullets )( c_vmt::hook_method( g_ctx.m_local, 16, hooks::fire_bullets ) );
		}
		
		g_cheat.m_prediction( ucmd );
		g_cheat.m_movement( ucmd );

		g_cheat.m_lagmgr( ucmd, send_packet );

		g_cheat.m_aimbot( ucmd );	

		//if( auto weapon = g_ctx.m_local->get_weapon( ) ) {
		//	if( auto c_class = weapon->ce( )->GetClientClass( ) ) {
		//		g_con->print( "%s\n", c_class->m_name );
		//	}
		//}

		g_cheat.m_extra.use_spammer( ucmd );

		if( g_cheat.m_lagmgr.get_choked( ) > 15 ) {
			g_cheat.m_lagmgr.set_state( true );
		}
		
		g_cheat.m_lagmgr.on_cmove_end( );
	
		g_ctx.on_cmove_end( ucmd );

		ucmd->clamp( );
		// call original *after* any changes due to anticheats detecting changes between this and any changes after.
		create_move_o( ecx_, 0, input_sample_frametime, ucmd );

		g_cheat.m_aimbot.silent( );
	}
	else {
		create_move_o( ecx_, 0, input_sample_frametime, ucmd );
	}
	
	return false;
}
