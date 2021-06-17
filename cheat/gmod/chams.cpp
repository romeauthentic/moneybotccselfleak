#include "chams.hpp"
#include "interface.hpp"

static auto chams_mat = xors_raw( (
	R"("VertexLitGeneric"
{
	"$basetexture"	"vgui/white"
	"$model"		"1"
	"$envmap"    "env_cubemap"
	"$envmaptint" "[.3 .3 .3]"
	"$envmapcontrast" ".4"
	"$envmapsaturation" "1.0"
	"$flat"			"0"
	"$nocull"		"1"
	"$halflambert"	"1"
	"$nofog"		"1"
	"$ignorez"		"0"
	"$znearer"		"0"
	"$wireframe"	"0"
	"$phong"		"1"
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

		m_mat = g_csgo.m_mat_system( )->CreateMaterial( name, m_keyvalues );
		m_mat->IncrementReferenceCount( );
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

	void c_materials::update_materials( ) {
		if( !m_initialized ) return;
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
}