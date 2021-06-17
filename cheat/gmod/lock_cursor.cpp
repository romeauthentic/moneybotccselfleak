#include "hooks.hpp"

void __fastcall hooks::lock_cursor( void* ecx, void* edx ) {
	static auto lock_cursor_o = g_gmod.m_surface->get_old_function< decltype( &hooks::lock_cursor ) >( 62 );
	if( g_gmod.m_panic ) return lock_cursor_o( ecx, edx );

	if( g_settings.menu.open ) {
		g_gmod.m_surface( )->UnlockCursor( );
	}
	else {
		lock_cursor_o( ecx, edx );
	}
}