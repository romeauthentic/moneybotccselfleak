#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <vector>

#include "util.hpp"
#include "sdk.hpp"

enum e_font_style : size_t {

};

enum FontAlign_t : size_t {
	ALIGN_CENTER,
	ALIGN_LEFT,
	ALIGN_RIGHT
};

NAMESPACE_REGION( drawings )


class c_fonts {
protected:
	struct font_t {
		friend class c_fonts;
		font_t( std::string name, int size,
			int weight, ulong_t flags );

		operator HFont( ) {
			return m_font;
		}

	private:
		void initialize( );

		HFont		m_font;
		std::string m_name;
		int			m_size;
		int			m_weight;
		ulong_t		m_flags;
	};

	std::vector< font_t* > m_container;
	friend struct c_fonts::font_t;
public:
	void initialize( );

	font_t f_esp_small{ xors( "Tahoma" ), 11, 500, FONTFLAG_OUTLINE };
	font_t f_bold{ xors( "Verdana" ), 18, 900, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS };
	font_t f_12{ 
#ifndef COMIC_SANS
		xors( "Tahoma" )
#else
		xors( "Comic Sans" )
#endif
		, 12, 0, FONTFLAG_DROPSHADOW };
	font_t f_14{ xors( "Verdana" ), 14, 700, 128 };
	font_t f_16{ xors( "Verdana" ), 16, 700, 128 };
	font_t f_18{ xors( "Verdana" ), 18, 700, 128 };
};



//im going to fill ur triangles
class c_drawings {
private:
	//dumb hacky fix for template functions
	void draw_string( const wchar_t* msg, HFont font, int x, int y, const clr_t& color );

public:
	void draw_line( const vec2_t& begin, const vec2_t& end, const clr_t& color );
	void draw_line( int x, int y, int x1, int y2, const clr_t& color );
	void draw_rect( int x, int y, int w, int h, const clr_t& color );
	void draw_box( int x, int y, int w, int h, const clr_t& color ); //thicc
	void draw_polygon( int count, vertex_t* vertices, const clr_t& col );
	void draw_circle( int x, int y, int r, const clr_t& col, int res = 48 );
	void draw_filled_circle( int x, int y, int r, const clr_t& col, int res = 48 );

	template < FontAlign_t align = ALIGN_LEFT > //fuck ur sprintf nigga varargs nigga
	void draw_string( HFont font, int x, int y, const clr_t& color, const wchar_t* msg, ... ) {
		wchar_t* buffer = ( wchar_t* )_alloca( 2048 );
		va_list list{ };
		int wide, tall;

		memset( buffer, 0, 2048 );

		__crt_va_start( list, msg );
		vswprintf_s( buffer, 1024, msg, list );
		__crt_va_end( list );

		g_csgo.m_surface( )->GetTextSize(
			font, buffer, wide, tall );

		switch( align ) {
		case ALIGN_CENTER:
			x -= wide / 2;
			break;
		case ALIGN_RIGHT:
			x -= wide;
			break;
		default: break;
		}

		draw_string( buffer, font, x, y, color );
	}

	template < FontAlign_t align = ALIGN_LEFT > //debatable whether it should be center or not
	void draw_string( HFont font, int x, int y, const clr_t& color, const char* msg, ... ) {
		char* buffer = ( char* )_alloca( 1024 );
		va_list list{ };

		memset( buffer, 0, 1024 );

		__crt_va_start( list, msg );
		vsprintf_s( buffer, 1024, msg, list );
		__crt_va_end( list );

		auto wide_str = util::ascii_to_unicode( buffer );
		draw_string< align >( font, x, y, color, wide_str.c_str( ) );
	}
};

END_REGION

extern drawings::c_fonts	g_fonts;
extern drawings::c_drawings g_renderer;