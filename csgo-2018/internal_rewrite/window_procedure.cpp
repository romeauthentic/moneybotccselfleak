#include "hooks.hpp"
#include "input_system.hpp"
#include "settings.hpp"

decltype( &hooks::window_procedure ) hooks::window_procedure_o;
long __stdcall hooks::window_procedure( HWND hwnd, uint32_t msg, uint32_t wparam, long lparam ) {
	if ( g_csgo.m_panic ) {
		return CallWindowProcA( ( WNDPROC )window_procedure_o, hwnd, msg, wparam, lparam );
	}

	if ( msg == WM_ACTIVATE && wparam == WA_INACTIVE ) {
		g_input.clear_keys( );
	}

	if ( msg == WM_MOUSEMOVE ) {
		g_input.capture_mouse_move( lparam );
		g_con->get_input( )->capture_mouse_move( lparam );
	}

	static auto con_pressed = false;
	if( g_con->get_input( )->is_key_pressed( KEYS_HOME ) ) {
		if( !con_pressed ) {
			if( !g_con->m_open ) {
				g_con->m_consuming_input = true;
			}

			g_con->m_open ^= 1;
		}

		con_pressed = true;
	}
	else con_pressed = false;

	if( g_con->get_input( )->register_key_press( VirtualKeyEvents_t( msg ), VirtualKeys_t( wparam ) ) ) {
		if( g_con->m_open ) return false;
	}

	if( !g_con->m_open ) {
		static bool was_pressed = false;
		if( g_input.is_key_pressed( KEYS_INSERT ) ) {
			if( !was_pressed ) {
				g_settings.menu.open ^= 1;

				was_pressed = true;
			}
		}
		else {
			was_pressed = false;
		}

		if( g_input.register_key_press( VirtualKeyEvents_t( msg ), VirtualKeys_t( wparam ) )
			&& g_settings.menu.open ) {
			return false;
		}
	}

	return CallWindowProcA( ( WNDPROC )window_procedure_o, hwnd, msg, wparam, lparam );
}