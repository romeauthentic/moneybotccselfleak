#include "renderer.hpp"
#include "interface.hpp"

#undef CreateFont


drawings::c_fonts	 g_fonts;
drawings::c_drawings g_renderer;

NAMESPACE_REGION( drawings )

c_fonts::font_t::font_t( std::string name, int size,
	int weight, ulong_t flags ) :
	m_name( name ), m_size( size ),
	m_weight( weight ), m_flags( flags ) {
	g_fonts.m_container.push_back( this );
};

void c_fonts::font_t::initialize( ) {
	m_font = g_csgo.m_surface( )->CreateFont( );
	g_csgo.m_surface( )->SetFontGlyphSet(
		m_font, m_name.c_str( ), m_size,
		m_weight, 0, 0,
		m_flags
	);
}

void c_fonts::initialize( ) {
	for( auto& it : m_container ) {
		it->initialize( );
	}
}

void c_drawings::draw_string( const wchar_t* text, HFont font, int x, int y, const clr_t& color ) {
	g_csgo.m_surface( )->DrawSetTextPos( x, y );
	g_csgo.m_surface( )->DrawSetTextFont( font );
	g_csgo.m_surface( )->DrawSetTextColor( color );
	g_csgo.m_surface( )->DrawPrintText( text, wcslen( text ) );
}

void c_drawings::draw_rect( int x, int y, int w, int h, const clr_t& color ) {
	g_csgo.m_surface( )->DrawSetColor( color );
	g_csgo.m_surface( )->DrawFilledRect( x, y, x + w, y + h );
}

void c_drawings::draw_line( int x, int y, int x1, int y1, const clr_t& clr ) {
	g_csgo.m_surface( )->DrawSetColor( clr );
	g_csgo.m_surface( )->DrawLine( x, y, x1, y1 );
}

void c_drawings::draw_box( int x, int y, int w, int h, const clr_t& clr ) {
	draw_line( x, y, x + w, y, clr );
	draw_line( x + w, y, x + w, y + h, clr );
	draw_line( x, y + h, x + w + 1, y + h, clr );
	draw_line( x, y, x, y + h, clr );
}

void c_drawings::draw_circle( int x, int y, int r, const clr_t& col, int res ) {
	g_csgo.m_surface( )->DrawSetColor( col );
	g_csgo.m_surface( )->DrawOutlinedCircle( x, y, r, res );
}

void c_drawings::draw_filled_circle( int x, int y, int r, const clr_t& col, int res ) {
	vertex_t* v = ( vertex_t* )( _alloca( res * sizeof( vertex_t ) ) );
	const float step = M_PI * 2.0f / res;

	for( size_t i{ }; i < res; ++i ) {
		float theta = i * step;
		float x_off = r * cos( theta );
		float y_off = r * sin( theta );
		v[ i ].init( x + x_off, y + y_off );
	}

	draw_polygon( res, v, col );
}

void c_drawings::draw_line( const vec2_t& begin, const vec2_t& end, const clr_t& clr ) {
	draw_line( ( int )begin.x, ( int )begin.y, ( int )end.x, ( int )end.y, clr );
}

void c_drawings::draw_polygon( int count, vertex_t* vertices, const clr_t& col ) {
	static int texture = g_csgo.m_surface( )->CreateNewTextureID( true );

	clr_t buf( 255, 255, 255 );

	g_csgo.m_surface( )->DrawSetTextureRGBA( texture, ( byte* )( &buf ), 1, 1 );
	g_csgo.m_surface( )->DrawSetColor( col );
	g_csgo.m_surface( )->DrawSetTexture( texture );

	g_csgo.m_surface( )->DrawTexturedPolygon( count, vertices );
}

END_REGION