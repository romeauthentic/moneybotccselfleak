#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_form : public base_item {
	public:
		c_form( int x, int y, int w, int h, const char* name, int max_h = 0 ) :
			base_item( x, y, w, h, name ), m_dynamic( !h ), m_max_height( max_h ) { };

		virtual int x( ) const override {
			return m_x + 10;
		}

		virtual int y( ) const override {
			return m_y + m_scroll_offset + 9;
		}

		virtual bool is_form( ) const override {
			return true;
		}

		virtual bool is_hovered( ) override {
			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			int x = get_relative_x( );
			int y = get_relative_y( );

			return mouse_x > x && mouse_x < x + w( )
				&& mouse_y > y && mouse_y < y + h( );
		}

		inline void update_size( ) {
			if ( !m_dynamic ) return;

			int total_height{ ITEM_OFFSET * 2 };
			for ( auto& it : m_items ) {
				if ( it->get_visible( ) ) {
					auto item_height = it->get_total_height( ) + ITEM_OFFSET;
					if( m_max_height && total_height + item_height > m_max_height ) {
						total_height = m_max_height;
						break;
					}
					total_height += it->get_total_height( ) + ITEM_OFFSET;
				}
			}

			m_height = total_height;
		}

		virtual int get_total_height( ) const override {
			return m_height + 5;
		}

		int get_total_item_height( ) {
			int total_height{ ITEM_OFFSET * 2 };
			for( auto& it : m_items ) {
				if( it->get_visible( ) ) {
					auto item_height = it->get_total_height( ) + ITEM_OFFSET;
					total_height += it->get_total_height( ) + ITEM_OFFSET;
				}
			}

			return total_height;
		}

		void input( ) {
			if( m_max_height && get_total_item_height( ) > m_max_height ) {
				if( !m_disabled && is_hovered( ) ) {
					auto scroll_state = g_input.get_scroll_state( );
					if( !!scroll_state && m_was_hovered ) {
						scroll_state > 0 ? m_scroll_offset += 13 : m_scroll_offset -= 13;
					}
					m_scroll_offset = std::clamp( m_scroll_offset, -( get_total_item_height( ) - m_height + 3 ), 0 );
				}
			}
			else {
				m_scroll_offset = 0;
			}
		}

		virtual void render( ) override {
			update_size( );
			input( );
			int x = get_relative_x( );
			int y = get_relative_y( );

			int text_w, text_h;
			ui_get_text_size( text_w, text_h, m_text );

			for( int i{ }; i < 8; ++i ) {
				clr_t col = ui_get_bg_col( ) * ( 0.72f + i * 0.04f );
				ui_draw_rect( x, y + i, m_width, m_height - i * 2, col );
			}

			ui_draw_outlined_rect( x, y, m_width, m_height, ui_get_accent_col( ) );

			if( m_max_height && get_total_item_height( ) > m_height ) {
				const size_t height = get_total_height( ) - 20;
				const float delta = ( float )( get_total_item_height( ) - height + 1 );
				const float slider_step = ( ( float )( height ) / delta );
				const float slider_height = slider_step * 13.f;

				size_t slider_pos = static_cast< size_t >( slider_step * m_scroll_offset );
				ui_draw_rect( x + m_width - 7, y + 8, 4, height, ui_get_disabled_col( ) );
				ui_draw_rect( x + m_width - 7, y - slider_pos + 8, 4, ( int )slider_height + 1, ui_get_accent_col( ) );
			}

			if( is_hovered( ) != m_was_hovered ) {
				bool backup = m_disabled;
				if( !backup ) {
					set_disabled( !is_hovered( ) );
				}
				m_disabled = backup;
			}

			ui_draw_line( x + 3, y, x + text_w + 1, y, ui_get_bg_col( ) );
			ui_draw_string( x + 3, y - 7, false, ui_get_text_col( ), m_text );

			m_was_hovered = is_hovered( );
		}

	protected:
		bool m_dynamic{ };
		bool m_was_hovered{ };
		int m_max_height{ };
		int m_scroll_offset{ };
	};
}