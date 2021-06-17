#pragma once

#include <functional>

#include "ui_base_item.h"

namespace ui
{
	class c_button : public base_item {
	public:
		c_button( int x, int y, int w, int h, const char* name, std::function< void( ) > fn ) :
			base_item( x, y, w, h, name ), m_fn( fn ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( );
			int h = m_height;

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );


			return mouse_x >= x && mouse_x <= x + m_width
				&& mouse_y >= y && mouse_y <= y + h;
		}


		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			ui_draw_rect( x, y, m_width, m_height, ui_get_disabled_col( ) );
			ui_draw_outlined_rect( x - 1, y - 1, m_width + 1, m_height + 1,
				is_hovered( ) ? ui_get_text_col( ) : ui_get_accent_col( ) );

			if ( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				ui_draw_rect( x, y, m_width, m_height, ui_get_bg_col( ) );
				if ( !m_mouse_held ) {
					m_fn( );
				}
				m_mouse_held = true;
			}
			else {
				m_mouse_held = false;
			}

			ui_draw_string( x + m_width / 2, y + 2, true, ui_get_text_col( ), m_text );
		}

	protected:
		std::function< void( ) > m_fn;
		bool m_mouse_held{ };
	};
}