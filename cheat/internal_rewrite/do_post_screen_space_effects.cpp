#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

bool __fastcall hooks::do_post_screen_space_effects( void* ecx_, void* edx_, CViewSetup* setup ) {
	static auto old_fn = do_post_screen_space_effects_o;

	g_ctx.m_drawing_postscreenspace = true;
	if( g_csgo.m_engine( )->IsInGame( ) && g_ctx.m_local ) {
		g_cheat.m_visuals.update_glow( );
	}

	bool ret = old_fn( ecx_, 0, setup );
	g_ctx.m_drawing_postscreenspace = false;

	return ret;
}