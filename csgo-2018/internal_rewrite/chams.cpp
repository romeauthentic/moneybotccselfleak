#include "chams.hpp"
#include "interface.hpp"
#include "d3d.hpp"
#include "context.hpp"
#include "hooks.hpp"

static auto chams_mat = xors_raw( (
	R"("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
	"$envmap"    "env_cubemap"
	"$envmaptint" "[.3 .3 .3]"
	"$envmapcontrast" ".4"
	"$envmapsaturation" "1.0"
	"$phongexponent" "255.0"
	"$normalmapalphaenvmask" "1"
	"$phongboost"                "6.0"
    "$phongfresnelranges"        "[0 0.5 1]" 
	"$BasemapAlphaPhongMask" "1" 
})"
) );

static auto chams_mat_shine = xors_raw( (
	R"("VertexLitGeneric" 
{ 
  "$basetexture"  "vgui/white" 
  "$envmap"    "env_cubemap"
  "$envmaptint" "[.3 .3 .3]"
  "$reflectivity" "[1.0 1.0 1.0]"
  "$envmapcontrast" ".4"
  "$envmapsaturation" "1.0"
  "$model"    "1" 
  "$flat"      "0" 
  "$nocull"    "1" 
  "$selfillum"  "1" 
  "$halflambert"  "1" 
  "$nofog"    "1" 
  "$ignorez"    "0" 
  "$znearer"    "0" 
  "$wireframe"  "0" 
})"
) );

static auto chams_mat_flat = xors_raw( (
	R"("UnlitGeneric"
{
	"$basetexture"	"vgui/white"
	"$model"		"1"
	"$flat"			"1"
	"$nocull"		"1"
	"$selfillum"	"1"
	"$halflambert"	"1"
	"$nofog"		"1"
	"$ignorez"		"0"
	"$znearer"		"0"
	"$wireframe"	"0"
})"
) );

namespace features
{
	void c_materials::make_cham_buffer( char* buf, int len, int type ) {
		switch ( type ) {
		case 1:
			sprintf_s( buf, len - 1, chams_mat_flat.decrypt( ) );
			break;
		case 2:
			sprintf_s( buf, len - 1, chams_mat_shine.decrypt( ) );
			break;
		default:
			sprintf_s( buf, len - 1, chams_mat.decrypt( ) );
		}
	}

	void c_material::init( const char* name, const char* buf ) {
		m_keyvalues = new KeyValues( name );
		m_keyvalues->LoadFromBuffer( name, buf );

		// CreateMaterial already increments ref counter
		m_mat = g_csgo.m_mat_system( )->CreateMaterial( name, m_keyvalues );
	}

	void c_material::destroy( ) {
		if( m_mat ) {
			m_mat->DecrementReferenceCount( );
			m_mat = nullptr;
		}
		
		if( m_keyvalues ) {
			delete m_keyvalues;
			m_keyvalues = nullptr;
		}
	}

	void c_materials::force_material( IMaterial* mat, fclr_t clr ) {
		if( !mat ) return;

		float col[ ] = { clr.r( ), clr.g( ), clr.b( ) };
		g_csgo.m_render_view( )->SetBlend( clr.a( ) );
		g_csgo.m_render_view( )->SetColorModulation( col );

		g_csgo.m_model_render( )->ForcedMaterialOverride( mat );
	}

	void c_materials::initialize_materials( ) {
		char buffer[ 1024 ];

		make_cham_buffer( buffer, sizeof( buffer ), 0 );
		m_chams.init( xors( "chamsmat" ), buffer );
		memset( buffer, 0, 1024 );

		make_cham_buffer( buffer, sizeof( buffer ), 1 );
		m_chams_flat.init( xors( "chamsmat_flat" ), buffer );
		memset( buffer, 0, 1024 );
		m_initialized = true;
	}

	void c_materials::destroy_materials( ) {
		m_initialized = false;

		m_chams.destroy( );
		m_chams_flat.destroy( );
	}

	void c_materials::update_materials( ) {
		if( !m_initialized ) return;
		if( m_chams.m_mat->m_ref_count <= 0 ) return;

		static float last_reflectivity{ };
		static float last_shine{ };
		static KeyValues* reflectivity;
		static KeyValues* shine;

		if( !reflectivity ) {
			for( auto it = m_chams.m_keyvalues->m_pSub; !!it; it = it->m_pPeer ) {
				if( it->m_iDataType != 1 ) continue;
				if( strstr( it->m_sValue, xors( "[.3 .3 .3]" ) ) ) {
					reflectivity = it;
				}
			}
		}

		if( !shine ) {
			for( auto it = m_chams.m_keyvalues->m_pSub; !!it; it = it->m_pPeer ) {
				if( it->m_flValue == 6.0f )
					shine = it;
			}
		}

		if( reflectivity ) {
			if( last_reflectivity != g_settings.visuals.chams.reflectivity( ) ) {
				char buf[ 32 ];
				sprintf_s< 32 >( buf, xors( "[%1.1f %1.1f %1.1f]" ),
					g_settings.visuals.chams.reflectivity( ),
					g_settings.visuals.chams.reflectivity( ),
					g_settings.visuals.chams.reflectivity( ) );

				strcpy( reflectivity->m_sValue, buf );
				m_chams.m_mat->Refresh( );
			}
			last_reflectivity = g_settings.visuals.chams.reflectivity( );
		}

		if( shine ) {
			if( last_shine != g_settings.visuals.chams.shine( ) ) {
				shine->m_flValue = g_settings.visuals.chams.shine( ) * 100.f;
				m_chams.m_mat->Refresh( );
			}
			last_shine = g_settings.visuals.chams.shine( );
		}
	}

	void c_chams::d3d_render_chams( c_base_player* ent, int type, int v_index, uint32_t min_index, uint32_t num_vert, uint32_t start_index, uint32_t prim_count ) {
		static auto d3d_draw = g_csgo.m_d3d->get_old_function< decltype( &hooks::d3d::draw ) >( 82 );

		int team = ent->m_iTeamNum( );
		clr_t visible_col = team == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.chams.color_visible_friendly( ) : g_settings.visuals.chams.color_visible_enemy( );
		clr_t hidden_col = team == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.chams.color_hidden_friendly( ) : g_settings.visuals.chams.color_hidden_enemy( );

		if( team == g_ctx.m_local->m_iTeamNum( ) && !g_settings.visuals.chams.friendlies )
			return;

		fclr_t fcol_vis = visible_col.to_fclr( );
		float value_vis[ 4 ] = { fcol_vis.r( ), fcol_vis.g( ), fcol_vis.b( ), fcol_vis.a( ) };

		fclr_t fcol_hid = hidden_col.to_fclr( );
		float value_hid[ 4 ] = { fcol_hid.r( ), fcol_hid.g( ), fcol_hid.b( ), fcol_hid.a( ) };

		float def[ 4 ] = { 1.f, 1.f, 1.f, 1.f };

		IDirect3DBaseTexture9* prev_texture;
		IDirect3DSurface9* prev_surface;
		ulong_t z_enable;

		g_d3d.get_device( )->GetTexture( 0, &prev_texture );
		g_d3d.get_device( )->GetRenderTarget( 0, &prev_surface );
		g_d3d.get_device( )->GetRenderState( D3DRS_ZENABLE, &z_enable );

		auto hr = g_d3d.get_device( )->SetRenderTarget( 0, g_d3d.m_surface );

		//printf( "hr: %08x\n", hr );

		if( g_settings.visuals.chams.ignore_z ) {
			g_d3d.get_device( )->SetRenderState( D3DRS_ZENABLE, false );
			g_d3d.get_device( )->SetPixelShaderConstantF( 0, value_hid, 1 );
			g_d3d.get_device( )->SetPixelShaderConstantF( 1, value_hid, 1 );
			//g_d3d.get_device( )->SetTexture( 0, nullptr );
			d3d_draw( g_d3d.get_device( ), ( D3DPRIMITIVETYPE )type, v_index, min_index, num_vert, start_index, prim_count );
		}

		g_d3d.get_device( )->SetRenderState( D3DRS_ZENABLE, z_enable );
		g_d3d.get_device( )->SetPixelShaderConstantF( 0, value_hid, 1 );
		g_d3d.get_device( )->SetPixelShaderConstantF( 1, value_vis, 1 );
		//g_d3d.get_device( )->SetTexture( 0, nullptr );

		d3d_draw( g_d3d.get_device( ), ( D3DPRIMITIVETYPE )type, v_index, min_index, num_vert, start_index, prim_count );

		g_d3d.get_device( )->SetPixelShaderConstantF( 0, def, 1 );
		g_d3d.get_device( )->SetPixelShaderConstantF( 1, def, 1 );
		g_d3d.get_device( )->SetRenderTarget( 0, prev_surface );
		g_d3d.get_device( )->SetRenderState( D3DRS_ZENABLE, z_enable );

		g_d3d.get_device( )->SetTexture( 0, prev_texture );

		if( prev_texture )
			prev_texture->Release( );

		if( prev_surface )
			prev_surface->Release( );
	}

	void c_chams::d3d_render_textures( ) {
		if( !g_d3d.m_sil_txt )
			return;

		IDirect3DSurface9* backbuffer;
		D3DSURFACE_DESC desc;

		g_d3d.get_device( )->GetRenderTarget( 0, &backbuffer );
		backbuffer->GetDesc( &desc );
		//backbuffer->Release( );

		struct tex_vertex {
			float x, y, z, r, h, w;
		};

		tex_vertex v[ ] = {
			{ 0.f, 0.f, 0.f, 1.0f, 0.f, 0.f },
			{ ( float )desc.Width, 0.f, 0.f, 1.0f, 1.0f, 0.f },
			{ 0.f, ( float )desc.Height, 0.f, 1.0f, 0.0f, 1.0f },
			{ ( float )desc.Width, ( float )desc.Height, 0.f, 1.0f, 1.0f, 1.0f }
		};

		void* p;

		g_d3d.m_buffer->Lock( 0, 0, &p, 0 );
		memcpy( p, v, sizeof( v ) );
		g_d3d.m_buffer->Unlock( );

		void* pixel_shader;
		void* texture;
		void* stream_src;
		uint32_t stride;
		uint32_t offset;
		ulong_t fvf;
		ulong_t dest_blend;
		ulong_t alpha_blend;

		g_d3d.get_device( )->GetPixelShader( ( IDirect3DPixelShader9** )&pixel_shader );
		g_d3d.get_device( )->GetTexture( 0, ( IDirect3DBaseTexture9** )&texture );
		g_d3d.get_device( )->GetStreamSource( 0, ( IDirect3DVertexBuffer9** )&stream_src, &offset, &stride );
		g_d3d.get_device( )->GetFVF( &fvf );
		g_d3d.get_device( )->GetRenderState( D3DRS_DESTBLEND, &dest_blend );
		g_d3d.get_device( )->GetRenderState( D3DRS_ALPHABLENDENABLE, &alpha_blend );

		g_d3d.get_device( )->SetRenderTarget( 0, g_d3d.m_surface );
		g_d3d.get_device( )->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		g_d3d.get_device( )->SetPixelShader( nullptr );
		g_d3d.get_device( )->SetTexture( 0, g_d3d.m_sil_txt );
		g_d3d.get_device( )->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_d3d.get_device( )->SetRenderState( D3DRS_ALPHABLENDENABLE, true );

		g_d3d.get_device( )->SetStreamSource( 0, g_d3d.m_buffer, 0, sizeof( tex_vertex ) );
		g_d3d.get_device( )->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		//D3DXSaveSurfaceToFileA( "backbuffer_1.bmp", D3DXIFF_BMP, backbuffer, 0, 0 );
		//D3DXSaveSurfaceToFileA( "backbuffer_2.bmp", D3DXIFF_BMP, g_d3d.m_surface, 0, 0 );


		D3DRECT bar_rect{ };
		D3DCOLOR col{ };

		bar_rect.x1 = 0;
		bar_rect.y1 = 0;
		bar_rect.x2 = desc.Width;
		bar_rect.y2 = desc.Height;

		g_d3d.get_device( )->SetPixelShader( ( IDirect3DPixelShader9* )pixel_shader );
		g_d3d.get_device( )->SetTexture( 0, ( IDirect3DTexture9* )texture );
		g_d3d.get_device( )->SetRenderState( D3DRS_DESTBLEND, dest_blend );
		g_d3d.get_device( )->SetRenderState( D3DRS_ALPHABLENDENABLE, alpha_blend );
		g_d3d.get_device( )->SetStreamSource( 0, ( IDirect3DVertexBuffer9* )stream_src, offset, stride );
		g_d3d.get_device( )->SetFVF( fvf );

		//g_d3d.get_device( )->Clear( 1, &bar_rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, col, 0, 0 );
		g_d3d.get_device( )->SetRenderTarget( 0, backbuffer );
		backbuffer->Release( );


	}
}