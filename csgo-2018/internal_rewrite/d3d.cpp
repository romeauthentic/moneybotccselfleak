#include "d3d.hpp"
#include "interface.hpp"
#include "math.hpp"
#include "d3d_sprite.hpp"

d3d::c_renderer g_d3d;
d3d::d3d_fonts_t d3d::fonts;


//theres shit still left to add like drawrect etc but thats really simple
//this is the base and it works so thats ok
//love
// - nave

// note - dex;  probably better idea to batch all calls up into one DrawPrimitive / DrawIndexedPrimitive call each (if you want to have index buffers too)
//              DrawPrimitiveUP for each object will slow stuff down eventually
//              dont know much about DrawIndexedPrimitive myself but msdn suggests to use strips over anything else

namespace d3d
{
	void d3d_fonts_t::release( ) {
		if( f_12 ) f_12->Release( );
		if( f_esp_small ) f_esp_small->Release( );
		if( f_16 ) f_16->Release( );
		if( f_18 ) f_18->Release( );
		if( f_menu ) f_menu->Release( );
		if( f_con ) f_con->Release( );

		f_12 = f_esp_small = f_16 = f_18 = f_menu = f_con = nullptr;
	}

	void d3d_fonts_t::create( IDirect3DDevice9* device ) {
		auto create_font = [ & ]( ID3DXFont** font, const char* font_name, int width, int size, int weight ) {
			//auto wide_str = util::ascii_to_unicode( std::string( font_name ) );

			auto code = D3DXCreateFontA( device, size, width, weight, 0, false, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, font_name, font );

			if( code < 0 ) throw xors( "fuck d3d" );
		};

		create_font( &f_12, 
#ifndef COMIC_SANS
			xors( "Tahoma" )
#else
		xors( "Comic Sans MS" )
#endif
			, 0, 12, 0 ); //change this idc
		create_font( &f_esp_small, xors( "Tahoma" ), 0, 11, 500 );
		create_font( &f_16, xors( "Verdana" ), 0, 16, 0 );
		create_font( &f_18, xors( "Verdana" ), 0, 18, 900 );
		create_font( &f_menu, 
#ifndef COMIC_SANS
			xors( "Tahoma" )
#else
			xors( "Comic Sans MS" )
#endif
			, 0, 12,
#ifndef COMIC_SANS
			300
#else
			600
#endif
		);
		create_font( &f_con, xors( "Consolas" ), 6, 12, 300 );
	}


	c_renderer::c_renderer( IDirect3DDevice9* device ) : m_device( device ) {
		create_objects( );
	}

	bool c_renderer::run_frame( IDirect3DDevice9* device ) {
		if( g_csgo.m_panic ) {
			return false;
		}

		if( !m_device ) {
			m_device = device;
			create_objects( );
			return false;
		}

		return true;
	}

	c_renderer::~c_renderer( ) {
		if( !m_device )
			return;

		invalidate_objects( );
	}

	void c_renderer::on_device_lost( ) {
		if( !m_device )
			return;

		invalidate_objects( );
	}

	void c_renderer::on_device_reset( ) {
		if( !m_device )
			return;

		create_objects( );
	}

	void c_renderer::invalidate_objects( ) {
		if( m_block ) m_block->Release( );
		fonts.release( );
		//m_buffer->Release( );
		//m_sil_txt->Release( );
		//m_surface->Release( );
	}

	void c_renderer::create_objects( ) {
		D3DVIEWPORT9 viewport;

		if( !m_device ) return;

		if( m_device->GetViewport( &viewport ) < 0 ) {
			return;
		}

		if( m_device->CreateStateBlock( D3DSBT_ALL, &m_block ) < 0 ) {
			return;
		}

		if( !m_block ) {
			return;
		}

		// get display size.
		m_width = viewport.Width;
		m_height = viewport.Height;

		fonts.create( m_device );

		/*IDirect3DSurface9* backbuffer;
		D3DSURFACE_DESC desc;

		m_device->GetRenderTarget( 0, &backbuffer );
		backbuffer->GetDesc( &desc );
		backbuffer->Release( );

		m_device->CreateTexture( desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_sil_txt, 0 );
		m_sil_txt->GetSurfaceLevel( 0, &m_surface );

		m_device->CreateVertexBuffer( 4 * 24, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_buffer, nullptr );*/
	}

	void c_renderer::begin( ) {
		if( !m_device ) return;

		D3DVIEWPORT9 vp{ 0, 0, m_width, m_height, 0.f, 1.f };

		m_device->SetViewport( &vp );

		//m_sil_txt->GetSurfaceLevel( 0, &m_surface );

		m_block->Capture( );

		// set vertex stream declaration.
		m_device->SetVertexShader( nullptr );
		m_device->SetPixelShader( nullptr );
		m_device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

		m_device->SetRenderState( D3DRS_LIGHTING, false );
		m_device->SetRenderState( D3DRS_FOGENABLE, false );
		m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

		m_device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		m_device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
		m_device->SetRenderState( D3DRS_ZWRITEENABLE, false );
		m_device->SetRenderState( D3DRS_STENCILENABLE, false );

		m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, false );
		m_device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );

		m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		m_device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
		m_device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, true );

		m_device->SetTexture( 0, nullptr );
		m_device->SetTexture( 1, nullptr );
		m_device->SetTexture( 2, nullptr );
		m_device->SetTexture( 3, nullptr );

		m_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		m_device->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_device->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
		m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_device->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
		m_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

		m_device->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
		m_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xffffffff );
		/* commented out until further notice */
		//m_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		//m_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		// todo - dex; if we use textures, need to set those rendering states too
	}

	void c_renderer::end( ) {
		//m_device->SetTexture( 0, nullptr );
		//m_device->SetTexture( 1, nullptr );
		//m_device->SetTexture( 2, nullptr );
		//m_device->SetTexture( 3, nullptr );
		m_block->Apply( );
		//m_block->Release( );
	}

	void c_renderer::draw_line( clr_t color, int x0, int y0, int x1, int y1 ) {
		d3d_vertex_t v[ 2 ] = {
			d3d_vertex_t( float( x0 ), float( y0 ), 1.0f, color ), //because fuck you thats why
			d3d_vertex_t( float( x1 ), float( y1 ), 1.0f, color )
		}; //edit: do we wanna use z for shit? i mean we could for like menu stuff
		   //so it renders above other stuff

		m_device->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, VERTEX_SIZE );
	}

	void c_renderer::draw_rect( clr_t color, int x, int y, int w, int h ) {
		d3d_vertex_t v[ 5 ] = {
			d3d_vertex_t( float( x ), float( y ), 1.0f, color ),
			d3d_vertex_t( float( x + w ), float( y ), 1.0f, color ),
			d3d_vertex_t( float( x + w ), float( y + h ), 1.0f, color ),
			d3d_vertex_t( float( x ), float( y + h ), 1.0f, color ),
			d3d_vertex_t( float( x ), float( y ), 1.0f, color )
		};

		m_device->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, v, VERTEX_SIZE );
	}

	void c_renderer::draw_filled_rect( clr_t color, int x, int y, int w, int h ) {
		d3d_vertex_t v[ 6 ] = {
			d3d_vertex_t( float( x + w ), float( y ), 1.0f, color ),
			d3d_vertex_t( float( x ), float( y + h ), 1.0f, color ),
			d3d_vertex_t( float( x + w ), float( y + h ), 1.0f, color ),
			d3d_vertex_t( float( x ), float( y ), 1.0f, color ),
			d3d_vertex_t( float( x ), float( y + h ), 1.0f, color ),
			d3d_vertex_t( float( x + w ), float( y ), 1.0f, color )
		};

		m_device->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, v, VERTEX_SIZE );
	}

	void c_renderer::draw_gradient( clr_t start, clr_t end, int x, int y, int w, int h, GradientType_t type ) {
		d3d_vertex_t v[ 4 ];

		switch( type ) {
		case GRADIENT_VERTICAL:
			v[ 0 ] = { float( x ), float( y ), 1.0f, start };
			v[ 1 ] = { float( x + w ), float( y ), 1.0f, start };
			v[ 2 ] = { float( x ), float( y + h ), 1.0f, end };
			v[ 3 ] = { float( x + w ), float( y + h ), 1.0f, end };
			break;
		case GRADIENT_HORIZONTAL:
			v[ 0 ] = { float( x ), float( y ), 1.0f, start };
			v[ 1 ] = { float( x + w ), float( y ), 1.0f, end };
			v[ 2 ] = { float( x ), float( y + h ), 1.0f, start };
			v[ 3 ] = { float( x + w ), float( y + h ), 1.0f, end };
			break;
		}

		//m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
		m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &v, VERTEX_SIZE );
		//m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, false );
	}

	void c_renderer::draw_circle( clr_t color, int x, int y, int r, int res ) {
		constexpr float PI = 3.1415926f;
		const float step = PI * 2.0f / float( res );

		int point_x = x + r,
			point_y = y - r,
			point_x_o{ },
			point_y_o{ };

		m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
		for( int i{ }; i <= res; i++ ) {
			float theta = float( i ) * step;

			point_x = x + ( int )( r * cos( theta ) );
			point_y = y - ( int )( r * sin( theta ) );
			if( i ) draw_line( color, point_x, point_y, point_x_o, point_y_o );
			point_x_o = point_x;
			point_y_o = point_y;
		}
		m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, false );
	}

	void c_renderer::draw_filled_circle( clr_t color, int x, int y, int r, int res ) {
		d3d_vertex_t* v = ( d3d_vertex_t* )_alloca( VERTEX_SIZE * res );
		const float step = M_PI * 2.0f / res;

		for( size_t i{ }; i < res; ++i ) {
			float theta = i * step;
			float x_off = r * cos( theta );
			float y_off = r * sin( theta );

			v[ i ] = { float( x + x_off ), float( y + y_off ), 1.0f, color };
		}

		m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
		m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, res, v, VERTEX_SIZE );
		m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, false );
	}

	void c_renderer::draw_text( ID3DXFont* font, clr_t color,
		int x, int y, FontAlign_t align, long font_flags, const char* msg ) {
		if( !msg ) return;
		if( !font ) return;

		auto d3d_black = D3DCOLOR_RGBA( 0, 0, 0, color.a( ) );
		auto d3d_color = D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) );
		auto buf = msg;

		if( align == ALIGN_CENTER ) x -= get_text_width( font, font_flags, msg ) / 2;
		if( align == ALIGN_RIGHT ) x -= get_text_width( font, font_flags, msg );
		RECT rect{ x, y, 1000, 100 };

		ulong_t flags = DT_NOCLIP | DT_LEFT | DT_TOP;

		if( font_flags & D3DFONTFLAG_DROPSHADOW ) {
			RECT r{ rect };
			r.left++;
			r.top++;
			font->DrawTextA( 0, buf, -1, &r, flags, d3d_black );
		}

		if( font_flags & D3DFONTFLAG_OUTLINE ) {
			for( int i = -1; i < 2; i++ ) {
				if( !i ) continue;
				RECT r{ rect };
				r.left += i;
				r.top += i;
				font->DrawTextA( 0, buf, -1, &r, flags, d3d_black );
			}
		}

		font->DrawTextA( 0, buf, -1, &rect, flags, d3d_color );
	}

	int c_renderer::get_text_width( ID3DXFont* font, long flags, const char* msg, ... ) {
		char* buffer = ( char* )_alloca( 2048 );
		va_list list{ };

		memset( buffer, 0, 2048 );

		__crt_va_start( list, msg );
		vsprintf_s( buffer, 2048, msg, list );
		__crt_va_end( list );

		RECT temp{ };
		font->DrawTextA( 0, buffer, -1, &temp, DT_CALCRECT, 0x0 );

		return ( temp.right - temp.left );
	}

	int c_renderer::get_text_height( ID3DXFont* font, long flags, const char* msg, ... ) {
		char* buffer = ( char* )_alloca( 2048 );
		va_list list{ };

		memset( buffer, 0, 2048 );

		__crt_va_start( list, msg );
		vsprintf_s( buffer, 2048, msg, list );
		__crt_va_end( list );

		RECT temp{ };
		font->DrawTextA( 0, buffer, -1, &temp, DT_CALCRECT, 0x0 );

		return ( temp.bottom - temp.top );
	}
}