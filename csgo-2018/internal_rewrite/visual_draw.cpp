#include "d3d.hpp"
#include "interface.hpp"
#include "renderer.hpp"
#include "visual.hpp"
#include "settings.hpp"

namespace features
{
	void c_visuals::draw_line( const vec2_t& a, const vec2_t& b, const clr_t& clr ) {
		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_line( clr, a.x, a.y, b.x, b.y );
		else
			g_renderer.draw_line( a, b, clr );
	}

	void c_visuals::draw_line( int x, int y, int x1, int y1, const clr_t& clr ) {
		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_line( clr, x, y, x1, y1 );
		else
			g_renderer.draw_line( x, y, x1, y1, clr );
	}

	void c_visuals::draw_rect( int x, int y, int w, int h, const clr_t& clr ) {
		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_rect( clr, x, y, w, h );
		else
			g_renderer.draw_box( x, y, w, h, clr );
	}

	void c_visuals::draw_filled_rect( int x, int y, int w, int h, const clr_t& col ) {
		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_filled_rect( col, x, y, w, h );
		else
			g_renderer.draw_rect( x, y, w, h, col );
	}

	void c_visuals::draw_circle( int x, int y, int r, const clr_t& col, int res ) {
		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_circle( col, x, y, r, res );
		else
			g_renderer.draw_circle( x, y, r, col, res );
	}

	void c_visuals::draw_string( int x, int y, int align, bool big, const clr_t& col, const char* msg, ... ) {
		char* buffer = ( char* )_alloca( 1024 );
		va_list list{ };

		memset( buffer, 0, 1024 );

		__crt_va_start( list, msg );
		vsprintf_s( buffer, 1024, msg, list );
		__crt_va_end( list );

		switch( align ) {
		case ALIGN_CENTER:
			if( g_settings.misc.hide_from_obs )
				g_d3d.draw_text< ALIGN_CENTER >( big ? ::d3d::fonts.f_12 : ::d3d::fonts.f_esp_small, col, x, y, big ? D3DFONTFLAG_DROPSHADOW : D3DFONTFLAG_OUTLINE, buffer );
			else
				g_renderer.draw_string< ALIGN_CENTER >( big ? g_fonts.f_12 : g_fonts.f_esp_small, x, y, col, buffer );

			break;
		case ALIGN_LEFT:
			if( g_settings.misc.hide_from_obs )
				g_d3d.draw_text< ALIGN_LEFT >( big ? ::d3d::fonts.f_12 : ::d3d::fonts.f_esp_small, col, x, y, big ? D3DFONTFLAG_DROPSHADOW : D3DFONTFLAG_OUTLINE, buffer );
			else
				g_renderer.draw_string< ALIGN_LEFT >( big ? g_fonts.f_12 : g_fonts.f_esp_small, x, y, col, buffer );

			break;
		case ALIGN_RIGHT:
			if( g_settings.misc.hide_from_obs )
				g_d3d.draw_text< ALIGN_RIGHT >( big ? ::d3d::fonts.f_12 : ::d3d::fonts.f_esp_small, col, x, y, big ? D3DFONTFLAG_DROPSHADOW : D3DFONTFLAG_OUTLINE, buffer );
			else
				g_renderer.draw_string< ALIGN_RIGHT >( big ? g_fonts.f_12 : g_fonts.f_esp_small, x, y, col, buffer );

			break;
		}
	}

	void c_visuals::draw_really_big_string( int x, int y, const clr_t& col, const char* msg, ... ) {
		char* buffer = ( char* )_alloca( 1024 );
		va_list list{ };

		memset( buffer, 0, 1024 );

		__crt_va_start( list, msg );
		vsprintf_s( buffer, 1024, msg, list );
		__crt_va_end( list );

		if( g_settings.misc.hide_from_obs )
			g_d3d.draw_text< ALIGN_LEFT >( ::d3d::fonts.f_18, col, x, y, D3DFONTFLAG_OUTLINE, buffer );
		else
			g_renderer.draw_string< ALIGN_LEFT >( g_fonts.f_bold, x, y, col, msg );
	}
}