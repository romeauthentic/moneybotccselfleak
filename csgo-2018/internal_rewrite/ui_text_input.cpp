#include "ui_text_input.h"
#include <Windows.h>

void ui::c_text_input::render( ) {
	if( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
		if( !m_was_held ) {
			m_active ^= 1;
		}
		m_was_held = true;
	}
	else {
		m_was_held = false;
	}

	if( m_active ) {
		if( !is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
			m_active = false;
		}

		float current_time = GetTickCount( ) * 0.001f;
		size_t length = strlen( m_text_ptr );

		for( size_t i{ }; i < 0xfe; ++i ) {
			if( g_input.is_key_pressed( i ) ) {
				float delta_time = current_time - m_last_key_input[ i ];
				if( fabs( delta_time ) > 0.2f ) {
					if( i == KEYS_BACK ) {
						m_text_ptr[ length - 1 ] = 0;
						m_last_key_input[ i ] = current_time;
						continue;
					}

					m_key_states[ i ] = 0xf0;
					wchar_t pressed_char;
					const auto scan = MapVirtualKeyA( i, 2 );
					auto ret = ToAscii( i, scan, ( BYTE* )m_key_states, ( LPWORD )&pressed_char, 1 );

					if( ret == 1 ) {
						if( length < m_text_len ) {
							m_text_ptr[ length ] = ( char )( pressed_char );
							m_text_ptr[ length + 1 ] = 0;
						}
					}
					m_last_key_input[ i ] = current_time;
				}
			}
			else {
				m_last_key_input[ i ] = 0.f;
				m_key_states[ i ] = 0;
			}
		}

		if( g_input.is_key_pressed( KEYS_RETURN ) ) {
			m_active = false;
		}
	}

	int x = get_relative_x( );
	int y = get_relative_y( );

	ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );
	y += 12;

	ui_draw_rect( x - 1, y - 1, m_width + 2, m_height + 2,
		( is_hovered( ) || m_active ) ? ui_get_text_col( ) : ui_get_accent_col( ) );
	ui_draw_rect( x, y, m_width, m_height, ui_get_disabled_col( ) );

	ui_draw_string( x + 2, y + 3, false, ui_get_text_col( ), m_active ? "%s_" : "%s", m_text_ptr );
}