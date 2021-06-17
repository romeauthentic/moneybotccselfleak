#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_text_input : public base_item {
		c_text_input( int x, int y, int w, const char* name, size_t max_chars, char* str ) :
			base_item( x, y, w, 16, name ), m_length( max_chars ), m_str( str ) { }

		virtual bool is_hovered( ) override {

		}

		void input( ) {
			static float last_press[ KEYS_MAX ]{ };

			int key = g_input.is_any_key_pressed( );
			if ( key == KEYS_BACK ) {
				if ( strlen( m_str ) ) {
					m_str[ strlen( m_str ) - 1 ] = 0;
				}
			}
			if ( key != KEYS_NONE ) {
				float cur_time = GetTickCount( ) * 0.001f;
				if ( std::abs( cur_time - last_press[ key ] ) > 0.1f ) {

					auto scan_code = MapVirtualKeyA( key, MAPVK_VK_TO_VSC );
					uword_t ascii;
					ToAscii( key, scan_code, 0, &ascii, 0 );
					char str[ 2 ] = { ( char )ascii, 0 };
					strcat_s( m_str, m_length, str );

					last_press[ key ] = cur_time;
				}
			}
		}

		virtual void render( ) override {

		}

	protected:
		size_t	m_length{ };
		char*	m_str{ };

	};
}