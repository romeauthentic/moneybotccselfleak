#pragma once
#include "ui_base_item.h"
#include "ui_dropdown_item.h"

#include <algorithm>

namespace ui
{
	template < typename t, size_t size >
	class c_itemlist : public base_item {
	public:
		static constexpr size_t ITEM_SIZE = 18;

		c_itemlist( int x, int y, int width, const char* name, std::vector< dropdowns::dropdown_item_t< t > >* dropdowns, t* setting )
			: base_item( x, y, width, 20, name ), m_items( dropdowns ), m_setting( setting ) { }

		void text_input( );
		bool is_textbox_hovered( ) {
			if( m_disabled )
				return false;
			
			int x = get_relative_x( );
			int y = get_relative_y( ) + 12;

			int mx, my;
			ui_get_cursor_pos( mx, my );
			
			return mx >= x && my >= y
				&& mx <= x + m_width && my <= y + ITEM_SIZE;
		}

		void input( ) {
			text_input( );
		}

		virtual int get_total_height( ) const override {
			size_t total_items = 0;

			if( strlen( m_search_text ) ) {
				for( size_t i{ }; i < m_items->size( ); ++i ) {
					if( !strstr( m_items->at( i ).m_name, m_search_text ) )
						continue;

					++total_items;
				}
			}
			else
				total_items = m_items->size( );

			return 12 + 20 + total_items * 20;
		}

		bool is_item_hovered( int x, int y ) {
			if( m_disabled )
				return false;
			
			int mx, my;
			ui_get_cursor_pos( mx, my );

			return mx >= x && mx <= x + m_width
				&& my >= y && my <= y + ITEM_SIZE;
		}

		void draw_items( int x, int y ) {
			int max_y = INT_MAX;

			auto parent = get_parent( );
			if( parent->is_form( ) ) {
				max_y = parent->get_relative_y( ) + parent->get_total_height( ) + ITEM_SIZE;
			}

			std::string search_str( m_search_text );
			std::transform( search_str.begin( ), search_str.end( ), search_str.begin( ), [ ]( char c ) { return tolower( c ); } );

			int text_len = strlen( m_search_text );
			for( size_t i{ }; i < m_items->size( ); ++i ) {
				if( y > max_y )
					break;

				auto& item = m_items->at( i );

				if( !strstr( item.m_name, m_search_text ) )
					continue;

				ui_draw_rect( x - 1, y - 1, m_width + 2, ITEM_SIZE + 2, 
					is_item_hovered( x, y ) || *m_setting == item.m_value
					? ui_get_text_col( ) : ui_get_accent_col( ) );

				ui_draw_rect( x, y, m_width, ITEM_SIZE, ui_get_disabled_col( ) );

				ui_draw_string( x + 2, y + 3, false, ui_get_text_col( ), item.m_name );

				if( g_input.is_key_pressed( KEYS_MOUSE1 ) && is_item_hovered( x, y ) )
					*m_setting = item.m_value;

				y += ITEM_SIZE + 2;
			}
		}

		virtual void render( ) override {
			input( );

			int x = get_relative_x( );
			int y = get_relative_y( );

			ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );

			ui_draw_rect( x - 1, y + 12, m_width + 2, ITEM_SIZE + 2, is_textbox_hovered( ) || m_box_active ? ui_get_text_col( ) : ui_get_accent_col( ) );
			if( is_textbox_hovered( ) ) {
				ui_draw_rect( x, y + 13, m_width, ITEM_SIZE, ui_get_text_col( ) );
			}
			ui_draw_rect( x + 1, y + 14, m_width - 2, ITEM_SIZE - 2, ui_get_disabled_col( ) );
			ui_draw_string( x + 3, y + 17, false, ui_get_text_col( ), m_box_active ? "%s_" : "%s", m_search_text );

			draw_items( x, y + 14 + ITEM_SIZE );
		}

	private:
		std::vector< dropdowns::dropdown_item_t< t > >* m_items;
		bool m_box_active{ };
		bool m_was_held{ };
		char m_search_text[ size ]{ };
		float m_last_key_input[ KEYS_LAST ]{ };
		uint8_t m_key_states[ 256 ]{ };
		t* m_setting{ };
	};
}