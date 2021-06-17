#include <unordered_map>

#include "context.hpp"
#include "visual.hpp"
#include "interface.hpp"
#include "renderer.hpp"

namespace features
{
	void fix_static_props( ) {
		static auto cvar = g_csgo.m_cvar( )->FindVar( xors( "r_drawspecificstaticprop" ) );
		static bool fixed{ };
		if( !fixed ) {
			cvar->m_flags |= 0;
			cvar->set_value( 1 );

			fixed = true;
		}
	}

	void c_visuals::draw_world( ) {
		if( !g_settings.visuals.active )
			return;

		static const auto m_bShouldGlow = g_netvars.get_netvar( fnv( "DT_DynamicProp" ), fnv( "m_bShouldGlow" ) );

		for( int i{ 64 }; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< IClientEntity >( i );
			if( !ent ) continue;

			auto client_class = ent->GetClientClass( );
			if( !client_class ) continue;

			int class_id = client_class->m_class_id;

			if( class_id != CBaseWeaponWorldModel && ( strstr( client_class->m_name, xors( "Weapon" ) ) 
				|| class_id == CDEagle || class_id == CAK47 ) ) {
				auto owner = ( int )ent->as< c_base_weapon >( )->m_hOwner( ) & 0xfff;
				if( owner <= 0 || owner > 64 ) {
					auto origin = ent->GetRenderOrigin( );
					if( origin ) {
						bool draw = g_settings.visuals.weapon_esp( ) == 1;
						bool glow = g_settings.visuals.weapon_esp( ) == 2;

						if( draw || g_settings.visuals.weapon_esp == 3 ) {
							if( !glow ) {
								*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = false;
							}

							auto w2s = util::screen_transform( origin );
							auto def_index = ent->as< c_base_weapon >( )->m_iItemDefinitionIndex( );
							auto name = util::definition_index_to_name( def_index );

							g_renderer.draw_string< ALIGN_CENTER >( g_fonts.f_esp_small, w2s.x, w2s.y,
								g_settings.visuals.weapon_esp_clr( ), name );
						}
						if( glow || g_settings.visuals.weapon_esp == 3 ) {
							*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
						}
					}
				}
			}
			else {
				if( g_settings.visuals.grenade_esp && strstr( client_class->m_name, xors( "Projectile" ) ) ) {
					auto model = ent->GetModel( );
					if( !model ) continue;

					auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
					if( !strstr( hdr->name, xors( "thrown" ) ) && !strstr( hdr->name, xors( "dropped" ) ) )
						continue;

					std::string name = xors( "nade" );
					if( strstr( hdr->name, xors( "flash" ) ) ) {
						name = xors( "flash" );
					}
					else if( strstr( hdr->name, xors( "smoke" ) ) ) {
						name = xors( "smoke" );
					}
					else if( strstr( hdr->name, xors( "decoy" ) ) ) {
						name = xors( "decoy" );
					}
					else if( strstr( hdr->name, xors( "incendiary" ) ) || strstr( hdr->name, xors( "molotov" ) ) ) {
						name = xors( "molotov" );
					}

					if( g_settings.visuals.grenade_esp == 2 || g_settings.visuals.grenade_esp == 3 ) {
						*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
					}

					if( g_settings.visuals.grenade_esp == 1 || g_settings.visuals.grenade_esp == 3 ) {
						auto pos = ent->GetRenderOrigin( );
						auto w2s = util::screen_transform( pos );

						g_renderer.draw_string< ALIGN_CENTER >( g_fonts.f_esp_small, w2s.x, w2s.y,
							g_settings.visuals.grenade_esp_clr, name.c_str( ) );
					}
				}
			}
			
		}
	}

	void c_visuals::world_modulate( ) {
		static std::unordered_map< MaterialHandle_t, fclr_t > world_materials;
		static std::unordered_map< MaterialHandle_t, fclr_t > world_materials2;
		static c_base_player* local_player = nullptr;
		static auto night_mode = false;
		static bool modulated = false;
		static bool alpha = false;

		if( !g_settings.visuals.world_modulate || !g_csgo.m_engine( )->IsInGame( ) ) {
			modulated = false;
			if( !world_materials2.empty( ) ) {
				for( auto& it : world_materials2 ) {
					auto mat = g_csgo.m_mat_system( )->GetMaterial( it.first );
					if( !mat ) continue;

					auto original_col = it.second;
					mat->ColorModulate( original_col.r( ), original_col.g( ), original_col.b( ) );
					mat->AlphaModulate( original_col.a( ) );
				}

				world_materials.clear( );
				world_materials2.clear( );
			}

			return;
		}

		if( g_ctx.m_stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END ) {
			fix_static_props( );

			if( local_player == g_ctx.m_local &&
				night_mode == g_settings.visuals.night_mode( ) &&
				alpha == g_settings.visuals.transparent_props( ) &&
				modulated == g_settings.visuals.world_modulate( ) )
				return;


			for( auto i = g_csgo.m_mat_system( )->FirstMaterial( );
				i != g_csgo.m_mat_system( )->InvalidMaterial( );
				i = g_csgo.m_mat_system( )->NextMaterial( i ) ) {

				auto mat = g_csgo.m_mat_system( )->GetMaterial( i );
				if( !mat ) continue;

				const char* group = mat->GetTextureGroupName( );
				bool is_world = strstr( group, xors( "World" ) );
				bool is_sky = strstr( group, xors( "Sky" ) );
				bool is_prop = strstr( group, xors( "StaticProp" ) );

				if( is_world || is_sky || is_prop ) {
					const char* name = mat->GetName( );
					if( world_materials.find( i ) == world_materials.end( ) ) {
						fclr_t clr;

						mat->GetColorModulate( &clr.r( ), &clr.g( ), &clr.b( ) );
						clr.a( ) = mat->GetAlphaModulation( );

						world_materials.emplace( i, clr );
						world_materials2.emplace( i, clr );
					}

					fclr_t new_color;

					new_color = world_materials2.find( i )->second;
					bool apply = g_settings.visuals.night_mode && g_settings.visuals.world_modulate;

					if( is_world ) {
						if( apply ) {
							new_color.r( ) *= 0.15f;
							new_color.g( ) *= 0.15f;
							new_color.b( ) *= 0.15f;
						}
					}
					else if( is_prop ) {
						if( apply ) {
							new_color.r( ) *= 0.3f;
							new_color.g( ) *= 0.3f;
							new_color.b( ) *= 0.3f;
						}
						if( g_settings.visuals.world_modulate && g_settings.visuals.transparent_props ) {
							new_color.a( ) *= 0.6f;
						}
					}
					else if( is_sky ) {
						if( apply ) {
							new_color = fclr_t( 0.f, 0.f, 0.f );
						}
					}

					if( !( world_materials.at( i ) == new_color ) ) {
						mat->ColorModulate( new_color.r( ), new_color.g( ), new_color.b( ) );
						mat->AlphaModulate( new_color.a( ) );

						world_materials.at( i ) = new_color;
					}
				}
			}

			modulated = g_settings.visuals.world_modulate;
			night_mode = g_settings.visuals.night_mode;
			alpha = g_settings.visuals.transparent_props;
			local_player = g_ctx.m_local;
		}
	}
}