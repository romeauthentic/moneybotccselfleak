#include "hooks.hpp"

void __fastcall hooks::lock_cursor( void* ecx_, void* edx_ ) {
	static auto lock_cursor_o = g_csgo.m_surface->get_old_function< decltype( &lock_cursor ) >( 67 );

	if( g_settings.menu.open || g_con->m_open ) {
		g_csgo.m_surface( )->UnlockCursor( );
		return;
	}

	lock_cursor_o( ecx_, 0 );
}