#include <intrin.h>

#include "hooks.h"
#include "mem.hpp"
#include "interfaces.h"
#include "ctx.hpp"
#include "input_system.hpp"
#include "math.h"
#include "settings.h"
#include "base_cheat.h"

//YEA FUCK USING THIS EVEN THOUGH I HAVE IT RIGHT HERE

//YEA VIVA LA REVOLUTION

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

void __fastcall hooks::hl_create_move( void* ecx_, void* edx_, int sequence_number, float input_sameple_frametime, bool active, byte& sendpacket ) {
	static auto create_move_o = cl.m_chl->get_old_function< void( __fastcall* )( void*, void*, int, float, bool ) >( 21 );

	user_cmd_t* ucmd = &cl.m_hl_input->m_pCommands[ sequence_number % MULTIPLAYER_BACKUP ];
	if( !ucmd )
		return;

	verified_cmd_t* verified_cmd = &cl.m_hl_input->m_pVerifiedCommands[ sequence_number % MULTIPLAYER_BACKUP ];
	if( !verified_cmd )
		return;

	// when switching to non automatic weapons, it will sometimes not shoot when aimbotting so we reset attack flag
	bool is_switching_weapon = false;
	if( cl.m_hl_input->m_hSelectedWeapon != -1 ) {
		is_switching_weapon = true;
	}

	cl.m_hl_input->CreateMove( sequence_number, input_sameple_frametime, active );
	//hl_create_move_o( ecx_, edx_, sequence_number, input_sample_time, active );

	if( !ucmd->m_tick_count || !ucmd->m_cmd_nr )
		return;

	// FIX ME NAVE
	// ok love

	byte* send_packet = &sendpacket;

	g_ctx.create_snapshot( ucmd );
	// soon   g_cheat.m_identity( );

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		static int last_frame = cl.m_globals->framecount;
		if( last_frame != cl.m_globals->framecount )
			g_ctx.m_has_fired_this_frame = false;

		last_frame = cl.m_globals->framecount;

		g_cheat.aim.sample_angle_data( ucmd->m_viewangles );
		g_cheat.prediction.run_prediction( ucmd );
		g_cheat.movement.update( ucmd );

		g_cheat.aim.m_lagcomp( ucmd );
		g_cheat.aim.triggerbot( ucmd );

	}


	if( is_switching_weapon ) {
		ucmd->m_buttons &= ~IN_ATTACK;
	}

	//m_bIsValveDS
	if( *( bool* )( c_base_player::get_game_rules( ) + 0x75 ) )
		g_settings.menu.anti_untrusted = true;

	ucmd->clamp( g_settings.menu.anti_untrusted );

	// crc stuff works, no double steps coming from this
	verified_cmd->m_cmd = *ucmd;
	verified_cmd->m_crc = ucmd->get_check_sum( );
}