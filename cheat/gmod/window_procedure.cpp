#include "base_cheat.hpp"
#include "hooks.hpp"
#include "input_system.hpp"
#include "settings.hpp"

decltype( &hooks::window_procedure ) hooks::window_procedure_o;
long __stdcall hooks::window_procedure( HWND hwnd, uint32_t msg, uint32_t wparam, long lparam ) {
	if ( g_gmod.m_panic ) {
		return CallWindowProcA( ( WNDPROC )window_procedure_o, hwnd, msg, wparam, lparam );
	}

	if ( msg == WM_ACTIVATE && wparam == WA_INACTIVE ) {
		g_input.clear_keys( );
	}

	if ( msg == WM_MOUSEMOVE ) {
		g_input.capture_mouse_move( lparam );
		g_con->get_input( )->capture_mouse_move( lparam );
	}

	auto enable_screen_clicker = []( bool enable ) -> void {

	};

	//static auto cvar = g_gmod.m_cvar( )->FindVar( xors( "cl_mouseenable" ) );
	static auto con_pressed = false;
	if( g_con->get_input( )->is_key_pressed( KEYS_HOME ) ) {
		if( !con_pressed ) {
			if( !g_con->m_open ) {
				g_con->m_consuming_input = true;
			}
	
			g_con->m_open ^= 1;
		}
	
		//cvar->set_value( !g_con->m_open );
		con_pressed = true;
	}
	else con_pressed = false;

	static auto list_pressed = false;
	if( g_con->get_input( )->is_key_pressed( KEYS_DELETE ) ) {
		if( !list_pressed ) {
			g_cheat.m_playerlist.is_open( ) ^= 1;
			list_pressed = true;
		}
	}
	else list_pressed = false;

	if( g_cheat.m_playerlist.is_open( ) ) {
		if( g_con->get_input( )->is_key_pressed( KEYS_DOWN ) ) {
			g_cheat.m_playerlist.get_index( )++;
		}
		else if( g_con->get_input( )->is_key_pressed( KEYS_UP ) ) {
			g_cheat.m_playerlist.get_index( )--;
		}

		if( g_cheat.m_playerlist.get_index( ) < 0 )
			g_cheat.m_playerlist.get_index( ) = g_cheat.m_playerlist.get_list( ).size( ) - 1;
		else if( g_cheat.m_playerlist.get_index( ) >= ( signed )g_cheat.m_playerlist.get_list( ).size( ) )
			g_cheat.m_playerlist.get_index( ) = 0;

		static bool was_pressed = false;
		if( g_con->get_input( )->is_key_pressed( KEYS_LEFT ) || g_con->get_input( )->is_key_pressed( KEYS_RIGHT ) ) {
			if( !was_pressed ) {
				*g_cheat.m_playerlist.get_list( )[ g_cheat.m_playerlist.get_index( ) ].m_friend ^= 1;
				was_pressed = true;
			}
		}
		else was_pressed = false;
	}

	if( g_con->get_input( )->register_key_press( VirtualKeyEvents_t( msg ), VirtualKeys_t( wparam ) ) ) {
		if( g_con->m_open ) return false;
	}

	if( !g_con->m_open ) {
		static bool was_pressed = false;
		if( g_input.is_key_pressed( KEYS_INSERT ) ) {
			if( !was_pressed ) {
				g_settings.menu.open ^= 1;
				//cvar->set_value( !g_settings.menu.open );
				//g_gmod.m_surface( )->SetCursorVisible( g_settings.menu.open );
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