#include <intrin.h>

#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

void __fastcall hooks::begin_lock( void* ecx, void* edx ) { // THIS IS PROLLY NOT ENDLOCK, BUT WHATEVER
	static auto begin_lock_o = g_csgo.m_mdl_cache->get_old_function< decltype( &hooks::begin_lock ) >( 33 );


	// signature:
	// search "CL_CallPostDataUpdates: missing ent %d" in engine.dll.
	// where framestagenotify gets called
	static auto ret_addr = pattern::first_code_match< void* >( g_csgo.m_engine.dll( ), "A1 ? ? ? ? B9 ? ? ? ? 6A 00 FF 50 18 8B 15 ? ? ? ?" );
	if( _ReturnAddress( ) == ret_addr ) {
		g_ctx.m_stage = FRAME_NET_UPDATE_POSTDATAUPDATE_START;
		g_cheat.m_visuals.world_modulate( );

		// FRAME_NET_UPDATE_POSTDATAUPDATE_START
		g_cheat.m_extra.float_ragdolls( );

		g_cheat.m_player_mgr.frame_stage_notify( );
		g_cheat.m_prediction.frame_stage_notify( );
		g_cheat.m_ragebot.m_resolver->frame_stage_notify( );
		g_cheat.m_ragebot.m_lagcomp->fsn_render_start( );
		g_cheat.m_skins( );
	}

	begin_lock_o( ecx, edx );
}