#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_checkbox : public base_item {
	public:
		c_checkbox( int x, int y, const char* txt, bool* setting ) :
			base_item( x, y, 16, 16, txt ), m_setting( setting ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			int rel_x = get_relative_x( );
			int rel_y = get_relative_y( );

			return mouse_x >= rel_x && mouse_x <= rel_x + m_width
				&& mouse_y >= rel_y && mouse_y <= rel_y + m_height;
		}

		inline void render_checkbox( const int& x, const int& y ) {
			clr_t col = ui_get_bg_col( );
			if ( is_hovered( ) ) {
				col = *m_setting ? ui_get_accent_col( ) * 0.8f : ui_get_bg_col( ) * 1.3f;
			}
			else if ( *m_setting ) {
				col = ui_get_accent_col( );
			}

			ui_draw_rect( x, y, m_width, m_height, ui_get_disabled_col( ) );
			ui_draw_rect( x + 1, y + 1, m_width - 2, m_height - 2, col );

			//ui_draw_outlined_rect( x, y, m_width, m_height, ui_get_accent_col( ) );
		}

		inline void input( ) {
			bool mouse_presesed = g_input.is_key_pressed( KEYS_MOUSE1 );

			if ( is_hovered( ) && mouse_presesed ) {
				if ( !m_mouse_held ) {
					*m_setting = !*m_setting;
				}
				m_mouse_held = true;
			}
			else {
				m_mouse_held = false;
			}
		}

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			render_checkbox( x, y );
			input( );


			ui_draw_string( x + m_width + 6, y + 2, false, ui_get_text_col( ), m_text );
		}

	protected:
		bool* m_setting;
		bool m_mouse_held{ };
	};
}