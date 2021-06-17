#include "hooks.h"
#include "base_cheat.h"
#include "ctx.hpp"
//whore?
bool __fastcall hooks::do_post_screen_space_effects( void* ecx_, void* edx_, CViewSetup* setup ) {
	static auto old_fn = cl.m_clientmode->get_old_function< decltype( &do_post_screen_space_effects ) >( 39 );

	g_ctx.m_drawing_postscreenspace = true;
	if( cl.m_engine( )->IsInGame( ) && g_ctx.m_local ) {
	}

	bool ret = old_fn( ecx_, 0, setup );
	g_ctx.m_drawing_postscreenspace = false;

	return ret;
}