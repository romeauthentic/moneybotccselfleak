#pragma once
#include "ui_base_item.h"

namespace ui 
{
	class c_label : public base_item {
	public:
		c_label( int x, int y, const char* text ) :
			base_item( x, y, 0, 16, text ) { }

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			ui_draw_string( x + 2, y + 2, false, ui_get_text_col( ), m_text );
		}
	};
}