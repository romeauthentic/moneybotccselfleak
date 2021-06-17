#pragma once
#include "color.hpp"
#include "interfaces.h"
#include "math.h"
typedef unsigned long HFont;

enum alignment {
	left,
	right,
	center
};

enum alignment_vertical {
	top,
	bottom
};

enum fontflag : long {
	none,
	italic = 0x001,
	underline = 0x002,
	strikeout = 0x004,
	symbol = 0x008,
	antialias = 0x010,
	blur = 0x020,
	rotary = 0x040,
	dropshadow = 0x080,
	additive = 0x100,
	outline = 0x200,
	custom = 0x400,
	bitmap = 0x800,
};

namespace tf2
{
	class c_fonts {
	public:
		c_fonts( ) {
		}

		void init( ) {
			f_12 = cl.m_surface( )->CreateFnt( );
			f_14 = cl.m_surface( )->CreateFnt( );
			f_16 = cl.m_surface( )->CreateFnt( );
			f_18 = cl.m_surface( )->CreateFnt( );
			f_esp = cl.m_surface( )->CreateFnt( );

			cl.m_surface( )->SetFontGlyphSet( f_12, "Tahoma", 12, 300, 0, 0, fontflag::outline );
			cl.m_surface( )->SetFontGlyphSet( f_14, "Tahoma", 14, 300, 0, 0, fontflag::outline );
			cl.m_surface( )->SetFontGlyphSet( f_16, "Tahoma", 16, 300, 0, 0, fontflag::outline );
			cl.m_surface( )->SetFontGlyphSet( f_18, "Tahoma", 18, 300, 0, 0, fontflag::outline );
			cl.m_surface( )->SetFontGlyphSet( f_esp, "Tahoma", 13, 550, 0, 0, fontflag::dropshadow );
		}

		HFont f_12;
		HFont f_14;
		HFont f_16;
		HFont f_18;

		HFont f_esp;
	};

	class c_graphics {
	public:
		void draw_line( int x, int y, int x1, int y1, clr_t clr );
		void draw_string( HFont font, int x, int y, clr_t clr, alignment f_alignment, std::string msg );
		void draw_string( HFont font, int x, int y, clr_t clr, alignment f_alignment, std::wstring msg );
		void draw_rect( int x, int y, int w, int h, clr_t clr );
		void draw_rect_outlined( int x, int y, int w, int h, clr_t clr );
		void draw_circle_outlined( int x, int y, int r, clr_t clr );
		void get_text_size( HFont font, std::string text, int& w, int& h );
		void get_text_size( HFont font, std::wstring text, int& w, int& h );
		void draw_polygon( int count, vertex_t* vertexs, clr_t clr_t );
		void draw_filled_circle( int x, int y, int r, clr_t clr_t );
		vec2_t world_to_screen( vec3_t world_pos );
	};
}

extern tf2::c_graphics graphics;
extern tf2::c_fonts fonts;