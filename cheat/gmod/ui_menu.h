#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_menu : public base_item {
	public:
		c_menu( int start_x, int start_y, int width,
			int height, const char* name, const char* right_text = nullptr ) :
			base_item( start_x, start_y, width, height, name ),
			m_right_text( right_text ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			return mouse_x >= m_x && mouse_x <= m_x + w( )
				&& mouse_y >= m_y && mouse_y <= m_y + 19;
		}

		virtual int y( ) const override {
			return m_y + 19;
		}

		inline void input( ) {
			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );
			bool mouse_clicked = g_input.is_key_pressed( KEYS_MOUSE1 );

			if ( is_hovered( ) ) {
				m_mouse_held = true;
			}

			if ( !mouse_clicked ) {
				m_mouse_held = is_hovered( );
			}

			if ( m_mouse_held && !mouse_clicked ) {
				m_drag_offset_y = mouse_y - m_y;
				m_drag_offset_x = mouse_x - m_x;
			}

			if ( m_mouse_held && mouse_clicked ) {
				m_x = mouse_x - m_drag_offset_x;
				m_y = mouse_y - m_drag_offset_y;
			}
		}

		virtual void render( ) override {
			constexpr auto top_height = 19;

			input( );

			//draw a c00l shadow
			ui_draw_outlined_rect( m_x, m_y + 1, m_width + 1, m_height, clr_t( 0, 0, 0, 166 ) );

			ui_draw_rect( m_x + 1, m_y + 1, m_width - 1, top_height - 2, ui_get_bg_col( ) * 1.2f );
			if( is_hovered( ) ) {
				ui_draw_rect( m_x + 1, m_y + 1, m_width - 1, top_height - 2, clr_t( 61, 61, 61 ) );
			}

			for( int i{ }; i < 8; ++i ) {
				clr_t col = ui_get_bg_col( ) * ( 0.72f + i * 0.04f );
				ui_draw_rect( m_x, m_y + i + top_height - 1, m_width, m_height - i * 2 - top_height + 1, col );
			}
			ui_draw_outlined_rect( m_x, m_y, m_width, m_height, ui_get_accent_col( ) );

			if( m_right_text ) {
				ui_draw_string( m_x + 5, m_y + 4, false, ui_get_text_col( ), m_text );
				
				int width, height;
				ui_get_text_size( width, height, m_right_text );

				ui_draw_string( m_x + m_width - 5 - width, m_y + 4, false, ui_get_text_col( ), m_right_text );
			}
			else {
				ui_draw_string( m_x + m_width / 2, m_y + 4, true, ui_get_text_col( ), m_text );
			}
		}

	protected:
		int m_drag_offset_x{ };
		int m_drag_offset_y{ };
		bool m_mouse_held{ };
		const char* m_right_text{ };
	};
}