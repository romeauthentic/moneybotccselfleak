#include <intrin.h>

#include "hooks.hpp"
#include "mem.hpp"
#include "interface.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"

#include "base_cheat.hpp"

#include "js_mgr.h"

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

	if( ret )
		g_csgo.m_engine( )->SetViewAngles( ucmd->m_viewangles );

	// FIX ME NAVE
	// ok love
	// yes thank u

	g_ctx.create_snapshot( ucmd );
	g_cheat.m_extra.update_netchannel( );
	g_cheat.m_identity( );

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		static int last_frame = g_csgo.m_globals->m_framecount;
		if( last_frame != g_csgo.m_globals->m_framecount )
			g_ctx.m_has_fired_this_frame = false;

		last_frame = g_csgo.m_globals->m_framecount;

		g_cheat.m_legitbot.sample_angle_data( ucmd->m_viewangles );

		// u forgot to put this back in the right place after u removed
		// my epic engine prediction that set seed to the player ptr 
		g_cheat.m_prediction.run_command(ucmd);
		g_cheat.m_movement( ucmd );
		
		//SUPER SECRET EXPLOIT DO NOT LEAK
		if( g_settings.misc.no_crouch_cooldown )
			ucmd->m_buttons |= IN_BULLRUSH;

		g_cheat.m_extra.auto_revolver( ucmd );

		g_cheat.m_legitbot.m_lagcomp( ucmd );
		g_cheat.m_legitbot.triggerbot( ucmd );

		g_cheat.m_lagmgr( ucmd, send_packet );

		g_cheat.m_visuals.update_hit_flags( );
		g_cheat.m_extra.fake_duck( ucmd );
		g_cheat.m_ragebot( ucmd );

		g_cheat.m_extra.no_recoil( ucmd );

		g_cheat.m_move_rec( ucmd );

		g_cheat.m_visuals.radar( );

		g_cheat.m_extra.rank_reveal( ucmd );

		if( g_cheat.m_lagmgr.get_choked( ) > 16 ) {
			g_cheat.m_lagmgr.set_state( true );
		}

		vec3_t real_angle = g_ctx.m_thirdperson_angle;

		g_cheat.m_lagmgr.on_cmove_end( );
		g_ctx.on_cmove_end( ucmd );

		auto cl = g_csgo.m_client_state;
		if( g_cheat.m_lagmgr.get_state( ) ) {
			g_ctx.m_cmd_numbers.push_back( ucmd->m_cmd_nr );
		}
		else {
			int choked = cl->m_netchannel->m_nChokedPackets;
			//printf( "choked: %d\n", choked );
			//
			cl->m_netchannel->m_nChokedPackets = 0;
			//
			int in_seq = cl->m_netchannel->m_nInSequenceNr;
			int reliable = cl->m_netchannel->m_nInReliableState;
			//
			g_cheat.m_extra.add_latency( cl->m_netchannel );
			send_datagram_o( cl->m_netchannel, 0, 0 );
			//
			cl->m_netchannel->m_nInSequenceNr = in_seq;
			//
			cl->m_netchannel->m_nInReliableState = reliable;
			cl->m_netchannel->m_nOutSequenceNr--;
			cl->m_netchannel->m_nChokedPackets = choked;
		}

		g_cheat.m_ragebot.m_antiaim->on_runcommand( );
	}
	else {
		g_ctx.reset_shot_queue( );
	}

	if( is_switching_weapon ) {
		ucmd->m_buttons &= ~IN_ATTACK;
	}

	g_js->run_on_cmove( );

	//m_bIsValveDS
	//if( *( bool* )( c_base_player::get_game_rules( ) + 0x75 ) )
		//g_settings.menu.anti_untrusted = true;

	ucmd->clamp( g_settings.menu.anti_untrusted );

	return false;
}