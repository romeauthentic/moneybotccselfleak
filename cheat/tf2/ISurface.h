#pragma once
#include "VFunc.h"
#include "vector.hpp"
#include "color.hpp"

enum FontDrawType_t {
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags_t {
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum ImageFormat {
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
								IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
								IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

								NUM_IMAGE_FORMATS
};

struct vertex_t {
	vertex_t( ) {}

	vertex_t( const vec2_t& pos, const vec2_t& coord = vec2_t( 0, 0 ) ) {
		m_position = pos;
		m_tex_coord = coord;
	}

	void init( float x, float y, const vec2_t& coord = vec2_t( 0, 0 ) ) {
		m_position = vec2_t( x, y );
		m_tex_coord = coord;
	}

	vec2_t m_position;
	vec2_t m_tex_coord;
};

using HFont = unsigned long;
using VPANEL = unsigned long;
class IntRect;
class IHTMLEvents;
class IHTML;
using HCursor = unsigned long;
using HTexture = unsigned long;
class CharRenderInfo;
class IVguiMatInfo;
class IImage;
class DrawTexturedRectParms_t;
class IHTMLChromeController;

enum GradientType_t {
	GRADIENT_HORIZONTAL = 0,
	GRADIENT_VERTICAL = 1
};

class ISurface {
public:
	void DrawSetColor( clr_t clr ) {
		return call_vfunc< void( __thiscall* )( void*, int , int , int, int ) >( this, 11 )( this, clr.r( ), clr.g( ), clr.b( ), clr.a( ) );
	}

	void DrawFilledRect( int x, int y, int w, int h ) {
		return call_vfunc< void( __thiscall* )( void*, int, int, int, int ) >( this, 12 )( this, x, y, w, h );
	}

	void DrawOutlinedRect( int x, int y, int w, int h ) {
		return call_vfunc< void( __thiscall* )( void*, int, int, int, int ) >( this, 14 )( this, x, y, w, h );
	}

	void DrawLine( int x0, int y0, int x1, int y1 ) {
		return call_vfunc< void( __thiscall* )( void*, int, int, int, int ) >( this, 15 )( this, x0, y0, x1, y1 );
	}

	void DrawPolyLine( int* x, int* y, int n ) {
		return call_vfunc< void( __thiscall* )( void*, int*, int*, int ) >( this, 16 )( this, x, y, n );
	}

	void DrawSetTextFont( unsigned long index ) {
		return call_vfunc< void( __thiscall* )( void*, unsigned long ) >( this, 17 )( this, index );
	}

	void DrawSetTextColor( clr_t clr ) {
		return call_vfunc< void( __thiscall* )( void*, int, int, int, int ) >( this, 19 )( this, clr.r( ), clr.g( ), clr.b( ), clr.a( ) );
	}

	void DrawSetTextPos( int x, int y ) {
		return call_vfunc< void( __thiscall* )( void*, int, int ) >( this, 20 )( this, x, y );
	}

	void DrawPrintText( const wchar_t* text, int length, FontDrawType_t drawtype = FontDrawType_t::FONT_DRAW_DEFAULT ) {
		return call_vfunc< void( __thiscall* )( void*, const wchar_t*, int, FontDrawType_t ) >( this, 22 )( this, text, length, drawtype );
	}

	void DrawSetTextureRGBA( int id, const unsigned char* rgba, int w, int h ) {
		return call_vfunc< void( __thiscall* )( void*, int, const unsigned char*, int, int, int, bool ) >( this, 31 )( this, id, rgba, w, h, 0, true );
	}

	void DrawSetTexture( int id ) {
		return call_vfunc< void( __thiscall* )( void*, int ) >( this, 32 )( this, id );
	}

	int CreateNewTextureID( bool procedural ) {
		return call_vfunc< int( __thiscall* )( void*, bool ) >( this, 37 )( this, procedural );
	}

	void SetCursorAlwaysVis( bool vis ) {
		return call_vfunc< void( __thiscall* )( void*, bool ) >( this, 52 )( this, vis );
	}

	unsigned long CreateFnt( ) {
		return call_vfunc< unsigned long( __thiscall* )( void* ) >( this, 66 )( this );
	}

	void UnlockCursor( ) {
		return call_vfunc< void( __thiscall* )( void* ) >( this, 61 )( this );
	}

	void LockCursor( ) {
		return call_vfunc< void( __thiscall* )( void* ) >( this, 62 )( this );
	}

	void SetFontGlyphSet( unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0 ) {
		return call_vfunc< void( __thiscall* )( void*, unsigned long, const char*, int, int, int, int, int, int, int ) >( this, 67 )( this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax );
	}

	void GetTextSize( unsigned long font, const wchar_t* text, int& wide, int& tall ) {
		return call_vfunc< void( __thiscall* )( void*, unsigned long, const wchar_t*, int&, int& ) >( this, 75 )( this, font, text, wide, tall );
	}

	void play_sound( const char* fileName ) {
		return call_vfunc< void( __thiscall* )( void*, const char* ) >( this, 82 )( this, fileName );
	}

	void GetCursorPosition( int &x, int &y ) {
		return call_vfunc< void( __thiscall* )( void*, int&, int& ) >( this, 96 )( this, x, y );
	}

	void DrawOutlinedCircle( int x, int y, int radius, int segments ) {
		return call_vfunc< void( __thiscall* )( void*, int, int, int, int ) >( this, 99 )( this, x, y, radius, segments );
	}

	void DrawTexturedPolygon( int n, vertex_t* pVertice, bool bClipVertices = true ) {
		return call_vfunc< void( __thiscall* )( void*, int, vertex_t*, bool ) >( this, 102 )( this, n, pVertice, bClipVertices );
	}

	void DrawGradient( int x, int y, int w, int h, clr_t first, clr_t second, GradientType_t type )
	{
		clr_t present_draw_clr = { };

		auto draw_filled_rect_gradient = [ & ]( bool reversed ) -> void {
			call_vfunc< void( __thiscall* )( void*, int, int, int, int, unsigned, unsigned, bool ) >( this, 118 )
				( this, x, y, w, h, reversed ? 255 : 0, reversed ? 0 : 255, type == GRADIENT_HORIZONTAL );
		};

		DrawSetColor( clr_t::blend( first, second, 0.5f ) );
		DrawFilledRect( x, y, w, h );

		DrawSetColor( first );
		present_draw_clr = first;
		draw_filled_rect_gradient( true );

		DrawSetColor( second );
		present_draw_clr = second;
		draw_filled_rect_gradient( false );
	}
};