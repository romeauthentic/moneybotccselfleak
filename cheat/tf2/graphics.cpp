#include "graphics.h"

tf2::c_fonts fonts;
tf2::c_graphics graphics;

namespace tf2
{
	void c_graphics::draw_line( int x, int y, int x1, int y1, clr_t clr ) {
		cl.m_surface( )->DrawSetColor( clr );
		cl.m_surface( )->DrawLine( x, y, x1, y1 );
	}

	void c_graphics::draw_string( HFont font, int x, int y, clr_t clr, alignment f_alignment, std::string msg ) {
		std::wstring a( msg.begin( ), msg.end( ) );
		const wchar_t* wbuf = a.c_str( );

		int w, h; get_text_size( font, msg, w, h );

		if ( f_alignment == alignment::right ) x -= w;

		if ( f_alignment == alignment::center ) x -= w / 2;

		cl.m_surface( )->DrawSetTextFont( font );
		cl.m_surface( )->DrawSetTextColor( clr );
		cl.m_surface( )->DrawSetTextPos( x, y - h / 2 );
		cl.m_surface( )->DrawPrintText( ( wchar_t* )wbuf, wcslen( wbuf ) );
	}

	void c_graphics::draw_string( HFont font, int x, int y, clr_t clr, alignment f_alignment, std::wstring msg ) {
		int w, h; get_text_size( font, msg, w, h );

		if ( f_alignment == alignment::right ) x -= w;

		if ( f_alignment == alignment::center ) x -= w / 2;

		cl.m_surface( )->DrawSetTextFont( font );
		cl.m_surface( )->DrawSetTextColor( clr );
		cl.m_surface( )->DrawSetTextPos( x, y - h / 2 );
		cl.m_surface( )->DrawPrintText( ( wchar_t* )msg.c_str( ), wcslen( msg.c_str( ) ) );
	}

	void c_graphics::draw_rect( int x, int y, int w, int h, clr_t clr ) {
		cl.m_surface( )->DrawSetColor( clr );
		cl.m_surface( )->DrawFilledRect( x, y, x + w, y + h );
	}

	void c_graphics::draw_rect_outlined( int x, int y, int w, int h, clr_t clr ) {
		cl.m_surface( )->DrawSetColor( clr );
		cl.m_surface( )->DrawOutlinedRect( x, y, x + w, y + h );
	}

	void c_graphics::draw_circle_outlined( int x, int y, int r, clr_t clr ) {
		cl.m_surface( )->DrawSetColor( clr );
		cl.m_surface( )->DrawOutlinedCircle( x, y, r, 48 );
	}

	void c_graphics::get_text_size( HFont font, std::string text, int& w, int& h ) {
		std::wstring a( text.begin( ), text.end( ) );
		const wchar_t* wstr = a.c_str( );

		cl.m_surface( )->GetTextSize( font, wstr, w, h );
	}

	void c_graphics::get_text_size( HFont font, std::wstring text, int& w, int& h ) {
		const wchar_t* wstr = text.c_str( );

		cl.m_surface( )->GetTextSize( font, wstr, w, h );
	}

	void c_graphics::draw_polygon( int count, vertex_t* vertexs, clr_t clr_t ) {
		static int texture = cl.m_surface( )->CreateNewTextureID( true );
		if ( !texture ) texture = cl.m_surface( )->CreateNewTextureID( true );

		byte buffer[ 4 ] = { 255, 255, 255, 255 };

		cl.m_surface( )->DrawSetTextureRGBA( texture, buffer, 1, 1 );
		cl.m_surface( )->DrawSetColor( clr_t );
		cl.m_surface( )->DrawSetTexture( texture );

		cl.m_surface( )->DrawTexturedPolygon( count, vertexs );
	}

	void c_graphics::draw_filled_circle( int x, int y, int r, clr_t clr_t ) {
		static vertex_t shape[ 24 ];
		static float step = M_PI * 2.0f / 24;

		for ( int i{ }; i < 24; i++ ) {
			float theta = i * step;
			float x1 = x + r * cos( theta );
			float y1 = y + r * sin( theta );
			shape[ i ].init( x1, y1 );
		}
		draw_polygon( 24, shape, clr_t );
	}

	vec2_t c_graphics::world_to_screen( vec3_t world_pos ) {
		vec3_t tmp; cl.m_overlay( )->ScreenPosition( world_pos, tmp );
		const matrix3x4& w2s_matrix = cl.m_engine( )->GetWorldToScreenMatrix( );
		float w = w2s_matrix[ 3 ][ 3 ];
		for ( int i{ }; i < 3; i++ ) {
			w += w2s_matrix[ 3 ][ i ] * world_pos[ i ];
		}
		if ( w < 0.001f ) return{ 10000.f, 10000.f };

		return{ tmp.x, tmp.y };
	}
}