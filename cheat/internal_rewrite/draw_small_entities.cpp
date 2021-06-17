#include <intrin.h>

#include "hooks.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "base_cheat.hpp"

#include "js_mgr.h"

void __fastcall hooks::draw_small_entities( void* ecx, void* edx, bool state ) {

#ifdef HEADER_MODULE
	static auto ret_addr = g_header.patterns.draw_small_entities_retaddr + 6;
#else
	static auto ret_addr = pattern::first_code_match< void* >( g_csgo.m_chl.dll( ), "FF 90 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 28", 6 );
#endif

	// please shoot me
	// i have given up
	// i want to leave
	if( _ReturnAddress( ) == (void*)ret_addr ) {
		g_ctx.m_stage = FRAME_RENDER_END;
		g_cheat.m_visuals.world_modulate( );

		//g_con->game_console_print( "render end\n" );
		// FRAME_RENDER_END
		g_cheat.m_ragebot.m_lagcomp->store_visuals( );
		g_cheat.m_chams.m_materials.update_materials( );
		g_cheat.m_extra.no_recoil( true );
		g_js->run_on_stage( );
	}

	draw_small_entities_o( ecx, edx, state );
}
