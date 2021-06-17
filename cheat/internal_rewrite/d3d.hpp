#ifndef D3D_HEADER //stackoverflow my niggas
#define D3D_HEADER

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma warning(disable : 4838)

#include <xnamath.h>
#include <windows.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "util.hpp"
#include "color.hpp"
#include "renderer.hpp"

enum D3DFontFlags_t {
	D3DFONTFLAG_OUTLINE = 0x10,
	D3DFONTFLAG_DROPSHADOW = 0x100,
};

enum GradientType_t {
	GRADIENT_HORIZONTAL,
	GRADIENT_VERTICAL
};

namespace features { class c_chams; }

//suck my dick
namespace d3d
{
	struct d3d_vertex_t {
		d3d_vertex_t( float x, float y, float z, clr_t color ) :
			m_x( x ), m_y( y ), m_z( z ),
			m_clr( D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ) ) { };

		d3d_vertex_t( ) : m_x( 0.f ), m_y( 0.f ), m_z( 0.f ),
			m_clr( 0 ) { };

		float m_x;
		float m_y;
		float m_z;
		float m_rhw = 1.f;
		D3DCOLOR m_clr;
	};

	constexpr size_t VERTEX_SIZE = sizeof( d3d_vertex_t );

	struct d3d_fonts_t {
		void release( );
		void create( IDirect3DDevice9* device );

		ID3DXFont* f_12;
		ID3DXFont* f_esp_small;
		ID3DXFont* f_16;
		ID3DXFont* f_18;
		ID3DXFont* f_menu;
		ID3DXFont* f_con;
	};

	class c_renderer {
	private:
		friend class features::c_chams;

		IDirect3DDevice9*		m_device;
		IDirect3DStateBlock9*	m_block;
		IDirect3DTexture9*		m_chams;
		IDirect3DTexture9*		m_sil_txt;
		IDirect3DVertexBuffer9* m_buffer;
		IDirect3DSurface9*		m_surface;
	public:
		ulong_t					m_width;
		ulong_t					m_height;

		c_renderer( ) { };
		c_renderer( IDirect3DDevice9* device );
		~c_renderer( );

		void on_device_lost( );
		void on_device_reset( );
		auto get_device( ) {
			return m_device;
		}

		bool run_frame( IDirect3DDevice9* device );
		void begin( );
		void end( );

		void draw_line( clr_t color, int x0, int y0, int x1, int y1 );
		void draw_rect( clr_t color, int x, int y, int w, int h );
		void draw_filled_rect( clr_t color, int x, int y, int w, int h );
		void draw_circle( clr_t color, int x, int y, int r, int steps = 48 );
		void draw_filled_circle( clr_t color, int x, int y, int r, int steps = 48 );
		void draw_gradient( clr_t start, clr_t end, int x, int y, int w, int h, GradientType_t type );

		void draw_text( ID3DXFont* font, clr_t color, int x, int y, FontAlign_t align, long font_flags, const char* msg );

		template < FontAlign_t align = ALIGN_CENTER >
		void draw_text( ID3DXFont* font, clr_t color, int x, int y, long font_flags, const char* msg, ... ) {
			char* buffer = ( char* )_alloca( 2048 );
			va_list list{ };

			memset( buffer, 0, 2048 );

			__crt_va_start( list, msg );
			vsprintf_s( buffer, 2048, msg, list );
			__crt_va_end( list );

			draw_text( font, color, x, y, align, font_flags, buffer );
		}

		int get_text_width( ID3DXFont* font, long font_flags, const char* msg, ... );
		int get_text_height( ID3DXFont* font, long font_flags, const char* msg, ... );

	private:
		void invalidate_objects( );
		void create_objects( );
	};

	extern d3d::d3d_fonts_t fonts;
}

extern d3d::c_renderer g_d3d;

#endif