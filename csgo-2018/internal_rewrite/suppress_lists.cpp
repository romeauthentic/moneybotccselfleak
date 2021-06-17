#include <intrin.h>

#include "hooks.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::suppress_lists( void* ecx, void* edx, int list_mask, bool suppress ) {
	static auto suppress_lists_o = g_csgo.m_partition->get_old_function< decltype( &hooks::suppress_lists ) >( 16 );

	static auto net_update_ret = pattern::first_code_match< void* >( g_csgo.m_chl.dll( ), "5F 5E 5D C2 04 00 83 3D ? ? ? ? ?" );

	if( _ReturnAddress( ) == net_update_ret ) {
		g_ctx.m_stage = FRAME_NET_UPDATE_START;
		g_cheat.m_visuals.world_modulate( );
		// FRAME_NET_UPDATE_START
		//g_con->game_console_print( "net start\n" );

		static bool rich_presence_active = false;

		g_ctx.run_frame( );
		g_cheat.m_ragebot.m_lagcomp->fsn_net_update_start( );

		if( g_settings.misc.rich_presence( ) ) {
			g_cheat.m_extra.rich_presence_flex( );
			rich_presence_active = true;
		}
		else if( rich_presence_active ) {
			if( !g_settings.misc.rich_presence( ) )
				g_cheat.m_extra.remove_rich_presence_flex( );

			rich_presence_active = false;
		}
		g_cheat.m_visuals.draw_sound( );
		g_cheat.m_visuals.draw_tracers( );

	}

	suppress_lists_o( ecx, edx, list_mask, suppress );

}