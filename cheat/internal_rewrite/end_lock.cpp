#include <intrin.h>

#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

#include "js_mgr.h"

void __fastcall hooks::end_lock( void* ecx, void* edx ) { // THIS IS PROLLY NOT ENDLOCK, BUT WHATEVER
	// signature:
	// search "CL_CallPostDataUpdates: missing ent %d" in engine.dll.
	// where framestagenotify gets called
	static auto ret_addr = pattern::first_code_match< void* >( g_csgo.m_engine.dll( ), "FF 50 18 FF 75 F0", 0x27 );
	if( _ReturnAddress( ) == ret_addr ) {
		g_ctx.m_stage = FRAME_NET_UPDATE_POSTDATAUPDATE_END;
		g_cheat.m_visuals.world_modulate( );
		g_js->run_on_stage( );
		//g_con->game_console_print( "net postdataupdate end\n" );
	}

	end_lock_o( ecx, edx );
}
