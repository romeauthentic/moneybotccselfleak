#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_text_input : public base_item {
	public:
		c_text_input( int x, int y, int w, const char* name, size_t max_chars, char* str ) :
			base_item( x, y, w, 16, name ), m_text_len( max_chars ), m_text_ptr( str ) {
		}

		virtual bool is_hovered( ) override {
			int cursor_x, cursor_y;
			ui_get_cursor_pos( cursor_x, cursor_y );

			int x = get_relative_x( );
			int y = get_relative_y( ) + 12;

			return cursor_x >= x && cursor_x <= x + m_width
				&& cursor_y >= y && cursor_y <= y + m_height;
		}

		virtual int get_total_height( ) const override {
			return m_height + 12;
		}

		virtual void render( ) override;

	protected:
		bool m_was_held{ };
		char* m_text_ptr{ };
		size_t m_text_len{ };
		bool m_active{ };
		float m_last_key_input[ KEYS_LAST ]{ };
		uint8_t m_key_states[ 256 ]{ };
	};
}