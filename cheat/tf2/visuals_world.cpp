#include <unordered_map>
#include <cmath>
#include "ctx.hpp"
#include "visuals.hpp"
#include "interfaces.h"
#include "renderer.hpp"
#include "base_cheat.h"
#include "settings.h"
namespace features
{
	void fix_static_props( ) {
		static auto cvar = cl.m_cvar( )->FindVar( xors( "r_drawspecificstaticprop" ) );
		static bool fixed{ };
		if( !fixed ) {
			cvar->m_nFlags |= 0;
			cvar->set_value( 1 );

			fixed = true;
		}
	}


	void c_visuals::draw_world( ) {
		if( !g_settings.visuals.active )
			return;

		static const auto m_bShouldGlow = g_netvars.get_netvar( fnv( "DT_DynamicProp" ), fnv( "m_bShouldGlow" ) );


		for( int i{ }; i < cl.m_entlist( )->get_highest_entity_index( ); ++i ) {
			auto ent = cl.m_entlist( )->get_client_entity< IClientEntity >( i );
			if( !ent ) continue;

			auto client_class = ent->get_client_class( );
			if( !client_class ) continue;

			int class_id = client_class->m_class_id;
			std::string name = "";

			auto is_object = [ &name ]( int class_id ) -> bool {
				switch( class_id ) {
				case CObjectSentrygun:
					name = xors( "sentry" );
					return true;
				case CObjectDispenser:
					name = xors( "dispenser" );
					return true;
				case CObjectTeleporter:
					name = xors( "teleporter" );
					return true;
				default:
					return false;
				}

				return false;
			};

			auto is_projectile = [ &name ]( int class_id ) -> bool {
				switch( class_id ) {
				case CTFProjectile_Arrow:
					name = xors( "arrow" );
					return true;
				case CTFProjectile_Cleaver:
					name = xors( "cleaver" );
					return true;
				case CTFProjectile_EnergyBall:
					name = xors( "energy ball" );
					return true;
				case CTFProjectile_EnergyRing:
					name = xors( "energy ring" );
					return true;
				case CTFProjectile_Flare:
					name = xors( "flare" );
					return true;
				case CTFProjectile_HealingBolt:
					name = xors( "healing bolt" );
					return true;
				case CTFProjectile_Jar:
					name = xors( "jar" );
					return true;
				case CTFProjectile_JarMilk:
					name = xors( "jar milk" );
					return true;
				case CTFGrenadePipebombProjectile:
					name = xors( "pipe bomb" );
					return true;
				case CTFProjectile_Rocket:
					name = xors( "rocket" );
					return true;
				case CTFProjectile_Throwable:
					name = xors( "throwable" );
					return true;
				case CTFProjectile_ThrowableBreadMonster:
					name = xors( "bread monster" );
					return true;
				case CTFProjectile_ThrowableBrick:
					name = xors( "brick" );
					return true;
				case CTFProjectile_ThrowableRepel:
					name = xors( "repel" );
					return true;
				case CTFProjectile_SentryRocket:
					name = xors( "rocket" );
					return true;
				default:
					return false;
				}
				return false;
			};

				//objects
			if( is_object( class_id ) ) {
				
				//wont work
				if( ( g_settings.visuals.building_esp == 2 || g_settings.visuals.building_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
					//*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
				}

				if( g_settings.visuals.building_esp == 1 || g_settings.visuals.building_esp == 3 ) {
					auto pos = ent->get_abs_origin( );
					auto w2s = util::screen_transform( pos );
					//currently they are not cute
					//auto cutie = cl.m_entlist( )->get_entity_from_handle< c_base_player >( ent->as< c_base_weapon >( )->m_hOwnerEntity( ) );
					draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
						g_settings.visuals.building_esp_clr, name.c_str( ) );

					//later

					//if( g_settings.visuals.building_owner( ) )
					//	draw_string( w2s.x, w2s.y + 10, ALIGN_CENTER, false, g_settings.visuals.building_esp_clr, cutie->get_info( ).name );
				}
			}




				//projectiles
			if( is_projectile( class_id ) ) {
				//wont work
				if( ( g_settings.visuals.projectile_esp == 2 || g_settings.visuals.projectile_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
					//*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
				}

				if( g_settings.visuals.projectile_esp == 1 || g_settings.visuals.projectile_esp == 3 ) {
					auto pos = ent->get_abs_origin( );
					auto w2s = util::screen_transform( pos );
					//again not cute
					//auto cutie = cl.m_entlist( )->get_entity_from_handle< c_base_player >( ent->as< c_base_weapon >( )->m_hOwnerEntity( ) );
					draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
						g_settings.visuals.projectile_esp_clr, name.c_str( ) );

					//later when i can be fucked

					//if( g_settings.visuals.building_owner( ) )
					//	draw_string( w2s.x, w2s.y + 10, ALIGN_CENTER, false, g_settings.visuals.projectile_esp_clr, cutie->get_info( ).name );
				}
			}





				/*if( class_id == CObjectTeleporter || class_id == CObjectDispenser || class_id == CObjectSentrygun ) {
					auto owner = ( int )ent->as< c_base_weapon >( )->m_hBuilder( ) & 0xfff;
					if( owner <= 0 || owner > 64 ) {
						auto origin = ent->get_render_origin( );
						if( origin ) {
							bool draw = g_settings.visuals.grenade_esp( ) == 1;
							bool glow = g_settings.visuals.grenade_esp( ) == 2;

							if( draw || g_settings.visuals.grenade_esp == 3 ) {
								if( !glow && !g_settings.misc.hide_from_obs ) {
									*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = false;
								}

								auto w2s = util::screen_transform( origin );
								auto name = util::object_index_to_name( class_id );

								draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
									g_settings.visuals.grenade_esp_clr( ), name );
							}
							if( ( glow || g_settings.visuals.grenade_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
								*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
							}
						}
					}
				}*/
				/*else {
					if( g_settings.visuals.weapon_esp && strstr( client_class->m_name, xors( "Projectile" ) ) ) {
						auto model = ent->get_model( );
						if( !model ) continue;

						std::string name = xors( "Rocket" );

						auto hdr = cl.m_modelinfo( )->GetStudioModel( model );
						if( !hdr )
							continue;

						if( !strstr( hdr->name, xors( "thrown" ) ) && !strstr( hdr->name, xors( "dropped" ) ) )
							continue;




						if( strstr( hdr->name, xors( "JarGas" ) ) ) {
							name = xors( "jargas" );
						}
						else if( strstr( hdr->name, xors( "Cleaver" ) ) ) {
							name = xors( "Cleaver" );
						}
						else if( strstr( hdr->name, xors( "JarMilk" ) ) ) {
							name = xors( "Milk" );
						}
						else if( strstr( hdr->name, xors( "incendiary" ) ) || strstr( hdr->name, xors( "molotov" ) ) ) {
							name = xors( "molotov" );
						}

						if( ( g_settings.visuals.weapon_esp == 2 || g_settings.visuals.weapon_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
							*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
						}

						if( g_settings.visuals.weapon_esp == 1 || g_settings.visuals.weapon_esp == 3 ) {
							auto pos = ent->get_render_origin( );
							auto w2s = util::screen_transform( pos );
							auto cutie = cl.m_entlist( )->get_entity_from_handle< c_base_player >( ent->as< c_base_weapon >( )->m_hOwnerEntity( ) );
							draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
								g_settings.visuals.grenade_esp_clr, name.c_str( ) );

							if( g_settings.visuals.grenade_owner( ) )
								draw_string( w2s.x, w2s.y + 10, ALIGN_CENTER, false, g_settings.visuals.grenade_esp_clr, cutie->get_info( ).name );
						}
					}
				}
				}*/
		
	}
}
	

	void c_visuals::world_modulate( ) {
		//indexes updated - free to implement

		static std::unordered_map< MaterialHandle_t, fclr_t > world_materials;
		static std::unordered_map< MaterialHandle_t, fclr_t > world_materials2;
		static c_base_player* local_player = nullptr;
		static auto night_mode = false;
		static bool modulated = false;
		static bool alpha = false;

		if( !g_settings.visuals.world_modulate || !cl.m_engine( )->IsInGame( ) || g_settings.misc.hide_from_obs ) {
			modulated = false;
			if( !world_materials2.empty( ) ) {
				for( auto& it : world_materials2 ) {
					auto mat = cl.m_mat_system( )->GetMaterial( it.first );
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


			for( auto i = cl.m_mat_system( )->FirstMaterial( );
				i != cl.m_mat_system( )->InvalidMaterial( );
				i = cl.m_mat_system( )->NextMaterial( i ) ) {

				auto mat = cl.m_mat_system( )->GetMaterial( i );
				if( !mat ) continue;

				const char* group = mat->GetTextureGroupName( );
				bool is_world = strstr( group, xors( "World" ) );
				bool is_sky = strstr( group, xors( "Sky" ) );
				bool is_prop = strstr( group, xors( "StaticProp" ) );

				if( is_world || is_sky || is_prop ) {
					const char* name = mat->GetName( );
					if( world_materials.find( i ) == world_materials.end( ) ) {
						fclr_t clr;

						mat->GetColorModulation( &clr.r( ), &clr.g( ), &clr.b( ) );
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