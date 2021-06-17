#include "ui_itemlist.h"
#include <windows.h>

namespace ui
{
	template < typename t, size_t size >
	void c_itemlist< t, size >::text_input( ) {
		if( is_textbox_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
			if( !m_was_held ) {
				m_box_active ^= 1;
			}
			m_was_held = true;
		}
		else {
			m_was_held = false;
		}

		if( m_box_active ) {
			if( !is_textbox_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				m_box_active = false;
			}

			float current_time = GetTickCount( ) * 0.001f;
			size_t length = strlen( m_search_text );

			for( size_t i{ }; i < 0xfe; ++i ) {
				if( g_input.is_key_pressed( i ) ) {
					float delta_time = current_time - m_last_key_input[ i ];
					if( fabs( delta_time ) > 0.2f ) {
						if( i == KEYS_BACK ) {
							m_search_text[ length - 1 ] = 0;
							m_last_key_input[ i ] = current_time;
							continue;
						}

						m_key_states[ i ] = 0xf0;
						wchar_t pressed_char;
						const auto scan = MapVirtualKeyA( i, 2 );
						auto ret = ToAscii( i, scan, ( BYTE* )m_key_states, ( LPWORD )&pressed_char, 1 );

						if( ret == 1 ) {
							if( length < size ) {
								m_search_text[ length ] = ( char )( pressed_char );
								m_search_text[ length + 1 ] = 0;
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
				m_box_active = false;
			}
		}
	}
}