#include <intrin.h>

#include "hooks.hpp"
#include "mem.hpp"
#include "interface.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"

#include "base_cheat.hpp"

/*
void __declspec( naked ) __stdcall hooks::hl_create_move_gate( int sequence_number, float input_sample_time, bool active ) {
	__asm {
		push	ebp
		mov		ebp, esp
		push	ebx //push ebx (sendpacket) to the top of the stack 
		push	esp //push the stack (with sendpacket on top) 
		push	active
		push	input_sample_time
		push	sequence_number
		call	hl_create_move
		pop		ebx
		pop		ebp
		ret		0xc
	}
}
*/

bool __fastcall hooks::create_move( void* ecx_, void* edx_, float input_sample_frametime, user_cmd_t* ucmd ) {
	static auto create_move_o = g_csgo.m_clientmode->get_old_function< decltype( &hooks::create_move ) >( 24 );

	stack_t stack( get_baseptr( ) );
	byte* send_packet = stack.next( ).local< byte* >( 0x1c );

	bool ret = create_move_o( ecx_, edx_, input_sample_frametime, ucmd );

	if( !ucmd->m_tick_count || !ucmd->m_cmd_nr )
		return ret;

	// when switching to non automatic weapons, it will sometimes not shoot when aimbotting so we reset attack flag
	bool is_switching_weapon = false;
	if( g_csgo.m_input( )->m_hSelectedWeapon != -1 ) {
		is_switching_weapon = true;
	}

	//g_csgo.m_input( )->CreateMove( sequence_number, input_sample_time, active );


	if( ret )
		g_csgo.m_engine( )->SetViewAngles( ucmd->m_viewangles );

	// FIX ME NAVE
	// ok love
	// yes thank u

	g_ctx.create_snapshot( ucmd );
	g_cheat.m_extra.update_netchannel( );
	g_cheat.m_identity( );

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		g_ctx.update_local_hook( );
		static int last_frame = g_csgo.m_globals->m_framecount;
		if( last_frame != g_csgo.m_globals->m_framecount )
			g_ctx.m_has_fired_this_frame = false;

		last_frame = g_csgo.m_globals->m_framecount;

		g_cheat.m_legitbot.sample_angle_data( ucmd->m_viewangles );
		g_cheat.m_prediction.local_pred( ucmd );
		g_cheat.m_movement( ucmd );

		if (g_settings.misc.no_crouch_cooldown)
			ucmd->m_buttons |= IN_BULLRUSH;

		g_cheat.m_legitbot.m_lagcomp( ucmd );
		g_cheat.m_legitbot.triggerbot( ucmd );

		g_cheat.m_lagmgr( ucmd, send_packet );

		g_cheat.m_visuals.update_hit_flags( );
		g_cheat.m_extra.fake_duck(ucmd);
		g_cheat.m_ragebot( ucmd );
		g_cheat.m_extra.auto_revolver( ucmd );

		g_cheat.m_extra.no_recoil( ucmd );
		g_cheat.m_move_rec( ucmd );

		g_cheat.m_visuals.radar( );

		g_cheat.m_extra.rank_reveal( ucmd );

		if( g_cheat.m_lagmgr.get_choked( ) > 15 ) {
			g_cheat.m_lagmgr.set_state( true );
		}

		auto animstate = g_ctx.m_local->get_animstate( );

		g_cheat.m_lagmgr.on_cmove_end( );
		g_ctx.on_cmove_end( ucmd );


		

	}
	else {
		g_ctx.reset_shot_queue( );
	}

	if( is_switching_weapon ) {
		ucmd->m_buttons &= ~IN_ATTACK;
	}



	//m_bIsValveDS
	if( *( bool* )( c_base_player::get_game_rules( ) + 0x75 ) )
		g_settings.menu.anti_untrusted = true;

	ucmd->clamp( g_settings.menu.anti_untrusted );

	return false;
}