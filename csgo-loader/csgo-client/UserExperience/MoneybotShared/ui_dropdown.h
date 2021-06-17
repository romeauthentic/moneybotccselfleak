#pragma once
#include "ui_dropdown_item.h"

namespace ui
{
	template < typename t = int >
	class c_dropdown : public base_item {
	public:
		c_dropdown( int x, int y, int w, const char* name, t* setting,
			std::vector< dropdowns::dropdown_item_t< t > >* items, size_t max_items = 8 ) :
			base_item( x, y, w, 16, name ), m_dropdown_items( items ), m_setting( setting ),
			m_max_items( max_items ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( ) + 12;
			int h = m_height;

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			
			return mouse_x >= x && mouse_x <= x + m_width
				&& mouse_y >= y && mouse_y <= y + h;
		}

		inline bool is_any_item_hovered( ) {
			if ( m_disabled || !m_active ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( ) + m_height + 12;
			int h = m_height * ( std::min< size_t >(
				m_dropdown_items->size( ), m_max_items ) );

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );


			return mouse_x >= x && mouse_x <= x + m_width
				&& mouse_y >= y && mouse_y <= y + h;
		}

		virtual int get_total_height( ) const override {
			return m_height + 13;
		}

		void draw_box( const int& x, const int& y, const char* str, bool hovered = false ) {
			ui_draw_rect( x, y, m_width, m_height, hovered ? ui_get_disabled_col( ) : ui_get_bg_col( ) );
			ui_draw_line( x, y + m_height, x + m_width, y + m_height, ui_get_accent_col( ) );

			ui_draw_string( x + m_width / 2, y + 2, true, ui_get_text_col( ), str );
		}

		void update_value( ) {
			for ( auto& it : *m_dropdown_items ) {
				if ( it.m_value == *m_setting ) {
					m_selected_item = &it;
				}
			}
		}

		void draw_items( const int& x, const int& y ) {
			auto& items = *m_dropdown_items;
			auto it		= &items.front( );
			int offset	= m_height + 1;
			int hovered = 0;
			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			auto is_hovered = [ & ] ( int y_offset ) {
				return mouse_x >= x && mouse_x <= x + m_width
					&& mouse_y >= y + y_offset && mouse_y <= y + y_offset + m_height;
			};


			for ( size_t i = items.size( ) > m_max_items ? m_curr_scroll : 0;
				i < std::min< size_t >( m_dropdown_items->size( ), m_max_items + m_curr_scroll );
				++i, offset += m_height + 1
				) {
				it = &items.at( i );

				draw_box( x, y + offset, it->m_name );

				if ( is_hovered( offset ) ) {
					hovered = offset;
					if ( g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
						m_selected_item = it;
						*m_setting = it->m_value;
						m_active = false;
						m_enable_time = GetTickCount( ) * 0.001f + 0.220f;
						m_enable_next_frame = true;
					}
				}
			}

			if ( hovered ) {
				ui_draw_outlined_rect( x - 1, y - 1 + hovered, 
					m_width + 1, m_height + 1, ui_get_text_col( ) );
			}
		}

		void input( ) {
			bool active_backup	= m_active;
			bool active_changed = false;

			if ( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				if ( !m_mouse_held ) {
					m_active = !m_active;
				}
				m_mouse_held = true;
			}
			else if ( !is_any_item_hovered( ) ) {
				m_mouse_held = false;
			}

			if ( !is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) && !is_any_item_hovered( ) ) {
				m_active = false;
			}
			
			if ( GetTickCount( ) * 0.001f > m_enable_time && m_enable_next_frame ) {
				set_disabled_callbacks( false );
				m_enable_next_frame = false;
			}


			active_changed = m_active != active_backup;
			//disable input on all items
			if ( active_changed ) {
				if ( !m_active ) {
					m_enable_time = GetTickCount( ) * 0.001f + 0.220f;
					m_enable_next_frame = true;
				}
				else {
					set_disabled_callbacks( true );
				}
			}

			if ( m_selected_item ) {
				*m_setting = m_selected_item->m_value;
			}

			if ( m_active && m_dropdown_items->size( ) > m_max_items ) {
				int scroll_input = g_input.get_scroll_state( );

				if ( m_curr_scroll > 0 || scroll_input < 0 ) //we dont want scroll to loop around from 0 to max
					m_curr_scroll -= scroll_input; //because positive is scroll up, we gotta flip it

				if ( m_curr_scroll > m_dropdown_items->size( ) - m_max_items )
					m_curr_scroll = m_dropdown_items->size( ) - m_max_items;
			}
		}

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			bool restore = false;
			RECT prev_rect{ };

			if ( m_active ) {
				restore = true;
				g_d3d.get_device( )->GetScissorRect( &prev_rect );

				RECT new_rect{
					prev_rect.left,
					prev_rect.top,
					g_d3d.m_width,
					g_d3d.m_height,
				};

				g_d3d.get_device( )->SetScissorRect( &new_rect );

				draw_items( x, y + 11 );
				

				//draw scrollbar
				size_t total_items = m_dropdown_items->size( );
				if ( total_items > m_max_items ) {
					const size_t height = ( m_height + 1 ) * m_max_items;
					const float slider_step = ( float )( height ) / float( total_items - m_max_items + 1 );

					size_t slider_pos = static_cast< size_t >( slider_step * m_curr_scroll );
					ui_draw_rect( x + m_width - 1, y + slider_pos + m_height + 13, 2, ( int )slider_step, ui_get_accent_col( ) );
				}
			}

			update_value( );
			input( );

			ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );
			ui_draw_rect( x, y + 13, m_width, m_height, ui_get_disabled_col( ) );
			ui_draw_outlined_rect( x - 1, y + 12, m_width + 1, m_height + 1,
				is_hovered( ) || m_active ? ui_get_text_col( ) : ui_get_accent_col( ) );

			if ( m_selected_item ) {
				ui_draw_string( x + m_width / 2, y + 14, true, ui_get_text_col( ), m_selected_item->m_name );
			}

			if( restore ) {
				g_d3d.get_device( )->SetScissorRect( &prev_rect );
			}
		}

	protected:
		std::vector< dropdowns::dropdown_item_t< t > >* m_dropdown_items{ };
		dropdowns::dropdown_item_t< t >* m_selected_item{ };
		bool m_active = false;
		bool m_mouse_held = false;
		t* m_setting{ };
		size_t m_max_items{ };
		size_t m_curr_scroll{ };
		float m_enable_time{ };
		int m_enable_next_frame{ };
	};
}