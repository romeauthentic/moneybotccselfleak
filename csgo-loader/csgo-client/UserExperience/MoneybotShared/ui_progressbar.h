#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_progress_bar : public base_item {
	public:
		c_progress_bar( int x, int y, int w, float* progress ) :
			base_item( x, y, w, 4, xors( "PROGRESS_BAR" ) ),
			m_progress( progress ) { }


		virtual void render( ) override {
			static const clr_t col_start = clr_t( 231, 105, 105, 255 );
			static const clr_t col_end = clr_t( 0xf4, 0x7c, 0xa8, 255 );

			int x = get_relative_x( );
			int y = get_relative_y( );

			ui_draw_rect( x, y, m_width, m_height, ui_get_disabled_col( ) );
			
			if( *m_progress > 0.001f ) {
				int fill = *m_progress * m_width;

				bool reverse = false;
				for( int i{ }; i < fill; ++i ) {
					float progress = std::fmod( float( i ) / fill - ( anim_time ), 1.f );
					if( progress == 1.0f ) reverse = true;
					if( reverse ) {
						progress = 1.0f - progress;
					}

					clr_t col = clr_t::blend( col_start, col_end, progress );


					ui_draw_rect( x + i, y, 1, m_height, col );
				}
			}
		}

	private:
		float* m_progress;
	};
}