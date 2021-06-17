#include "hooks.hpp"

void __fastcall hooks::lock_cursor( void* ecx_, void* edx_ ) {
	if( g_settings.menu.open || g_con->m_open ) {
		g_csgo.m_surface( )->UnlockCursor( );
		SetCursor( NULL );
		return;
	}

	lock_cursor_o( ecx_, 0 );
}