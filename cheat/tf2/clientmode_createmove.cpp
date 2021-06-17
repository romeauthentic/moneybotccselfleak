#include "hooks.h"
#include "interfaces.h"
#include "search.h"
#include "base_cheat.h"
#include <intrin.h>
#include "mem.hpp"
#define get_baseptr( ) ( ( uintptr_t )( _AddressOfReturnAddress( ) ) - sizeof( uintptr_t ) )
bool __fastcall hooks::create_move( void *ecx_, void *edx_, float input_sample_time, user_cmd_t *ucmd ) {
	static auto create_move_o = cl.m_clientmode->get_old_function< decltype( hooks::create_move )* >( 21 );

	if( cl.m_panic || !ucmd->m_cmd_nr || !ucmd->m_tick_count ) {
		return create_move_o( ecx_, edx_, input_sample_time, ucmd );
	}

	// call from CInput::CreateMove, mimic what the engine does.
	cl.m_engine( )->SetViewAngles( ucmd->m_viewangles );

	// random_seed isn't generated in ClientMode::CreateMove yet (since CInput::CreateMove handles that), we must generate it ourselves.
	ucmd->m_random_seed = math::md5_pseudorandom( ucmd->m_cmd_nr ) & 0x7fffffff;

	//get sendpacket off the stack
	stack_t stack( get_baseptr( ) );
	byte    *send_packet = stack.next( ).local< byte* >( 0x1C );

	g_ctx.create_snapshot( ucmd );

	cl.m_engine( )->SetViewAngles( ucmd->m_viewangles );
	g_ctx.calculate_tickbase( );

	//bool* send_packet = ***get_ebp< bool**** >( ) - 0x1;

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_alive( ) ) {
		static int last_frame = cl.m_globals->framecount;
		if( last_frame != cl.m_globals->framecount )
			g_ctx.m_has_fired_this_frame = false;

		last_frame = cl.m_globals->framecount;

		g_cheat.aim.sample_angle_data( ucmd->m_viewangles );
		//g_cheat.prediction.run_prediction( ucmd );
		g_cheat.movement.update( ucmd );

		g_cheat.aim.m_lagcomp( ucmd );
		g_cheat.aim.triggerbot( ucmd );
	}
	

	ucmd->clamp( );
	return false;
}