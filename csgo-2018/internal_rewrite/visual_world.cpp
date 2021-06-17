#include <unordered_map>
#include <cmath>
#include "context.hpp"
#include "visual.hpp"
#include "interface.hpp"
#include "renderer.hpp"
#include "base_cheat.hpp"
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

	void c_visuals::store_firegrenades( int index, vec3_t pos ) {
		firegrenade_t new_grenade;

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( index );
		if( !ent || !ent->is_player( ) )
			return;
		

		new_grenade.m_time = g_csgo.m_globals->m_curtime;
		new_grenade.m_ent = index;
		new_grenade.m_pos = pos;
		m_firegrenades.emplace_back( new_grenade );
	}

	void c_visuals::draw_world( ) {
		if( !g_settings.visuals.active )
			return;

		static const auto m_bShouldGlow = g_netvars.get_netvar( fnv( "DT_DynamicProp" ), fnv( "m_bShouldGlow" ) );
		static auto c4_time = g_csgo.m_cvar( )->FindVar( xors( "mp_c4timer" ) );

		draw_firegrenade( );

		for( int i{ 64 }; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< IClientEntity >( i );
			if( !ent ) continue;

			auto client_class = ent->GetClientClass( );
			if( !client_class ) continue;

			int class_id = client_class->m_class_id;

			if( g_settings.visuals.bomb_info && class_id == CPlantedC4 && ( strstr( client_class->m_name, xors( "Planted" ) ) ) ) {
				//yea i know this is yuck i just want to get it out there for now, ill revisit it sometime in the week
				//not bad at all tbh
				int screen_w, screen_h;
				g_csgo.m_engine( )->GetScreenSize( screen_w, screen_h );

				auto pos = ent->GetRenderOrigin( );
				auto w2s = util::screen_transform( pos );
				auto bomb = ent->as< c_base_weapon >( );
				auto to_target = pos - g_ctx.m_local->ce( )->GetRenderOrigin( );

				float bomb_time = bomb->m_flC4Blow( ) - g_csgo.m_globals->m_curtime;
				float bomb_percentage = bomb_time / c4_time->get_float( );
				float time = ( bomb_time < 0 || bomb->m_bBombDefused( ) ) ? 0.f : bomb_time;

				if( time <= 0.f )
					return;

				bool out_of_pov = w2s.x > screen_w || w2s.x < 0.f || w2s.y > screen_h || w2s.y < 0.f;
				if( out_of_pov ) {
					static auto safezoney = g_csgo.m_cvar( )->FindVar( xors( "safezoney" ) );
					static auto hud_scaling = g_csgo.m_cvar( )->FindVar( xors( "hud_scaling" ) );

					w2s.x = screen_w / 2;
					w2s.y = ( 45 * hud_scaling->get_float( ) ) + ( screen_h - screen_h * safezoney->get_float( ) + 1 ) * 0.5f;
				}

				float bomb_damage = 500 * exp( -((to_target.length( ) * to_target.length( ) ) / ( ( ( 1750 * 0.33333334 ) * 2.f  ) * ( 1750 * 0.33333334 ) ) ) );
				float damage = bomb_damage;

				if( g_ctx.m_local->m_ArmorValue( ) > 0 ) {
					damage *= 0.5f;
					float armor = ( bomb_damage - damage ) * 0.5f;
					if( armor > g_ctx.m_local->m_ArmorValue( ) )
						damage =  bomb_damage - ( g_ctx.m_local->m_ArmorValue( ) * ( 1.f / 0.5f ) );
				}

				int health_remaining = g_ctx.m_local->m_iHealth( ) - round(damage);

				float damage_percentage = std::clamp( round( damage ) / g_ctx.m_local->m_iHealth( ), 0.f, 1.0f );

				clr_t bomb_col = clr_t::blend( esp_red( ), esp_green( ), bomb_percentage );
				clr_t damage_col = clr_t::blend( esp_green( ), esp_red( ), damage_percentage );

				char health_str[ 100 ];

				strenc::w_sprintf_s( health_str, 100, xors( "health: %d" ), health_remaining );

				if( health_remaining <= 0 ) {
					strenc::w_sprintf_s( health_str, 100, xors( "dead" ) );
					damage_percentage = 100.f;
				}

				draw_string( w2s.x, w2s.y + 20, ALIGN_CENTER, false, damage_col, health_str );

				auto draw_timer = [ & ]( const char* text, float progress, bool red = false ) {
					clr_t col = red ? esp_red( ) : clr_t::blend( esp_green( ), esp_red( ), progress );

					const float full = 78.f;
					float fill = full * progress;

					draw_filled_rect( w2s.x - 40, w2s.y, 80, 18, clr_t( 0, 0, 0, 170 ) );
					draw_filled_rect( w2s.x - 39, w2s.y + 1, fill, 16, col );

					draw_string( w2s.x, w2s.y + 3, ALIGN_CENTER, false, clr_t( 255, 255, 255 ), text );
				};

				if( bomb->m_hBombDefuser( ) != ULONG_MAX ) {
					auto defuser = g_csgo.m_entlist( )->GetClientEntity( bomb->m_hBombDefuser( ) & 0xfff );

					float defuse_time = std::clamp( bomb->m_flDefuseCountDown( ) - g_csgo.m_globals->m_curtime, 0.f, 10.f );
					float defuse_percentage = defuser->m_bHasDefuser( ) ? defuse_time / 5.f : defuse_time / 10.f;

					char defuse_str[ 32 ];
					strenc::w_sprintf_s( defuse_str, 32, xors( "defuse: %.1f" ), defuse_time );

					draw_timer( defuse_str, 1.f - defuse_percentage, defuse_time > bomb_time );
				}
				else {
					char bomb_str[ 32 ];
					strenc::w_sprintf_s( bomb_str, 32, xors( "bomb: %.1f" ), bomb_time );

					draw_timer( bomb_str, 1.f - bomb_percentage );
				}
			}

			if( class_id != CBaseWeaponWorldModel && ( strstr( client_class->m_name, xors( "Weapon" ) )
				|| class_id == CDEagle || class_id == CAK47 ) ) {
				auto owner = ( int )ent->as< c_base_weapon >( )->m_hOwner( ) & 0xfff;
				if( owner <= 0 || owner > 64 ) {
					auto origin = ent->GetRenderOrigin( );
					if( origin ) {
						bool draw = g_settings.visuals.weapon_esp( ) == 1;
						bool glow = g_settings.visuals.weapon_esp( ) == 2;

						if( draw || g_settings.visuals.weapon_esp == 3 ) {
							if( !glow && !g_settings.misc.hide_from_obs ) {
								*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = false;
							}

							auto w2s = util::screen_transform( origin );
							auto def_index = ent->as< c_base_weapon >( )->m_iItemDefinitionIndex( );
							auto name = util::definition_index_to_name( def_index );

							auto dist = g_ctx.m_local->m_vecOrigin( ).dist_to( origin );

							clr_t col = g_settings.visuals.weapon_esp_clr;
							if( dist > 250.f ) {
								col.a( ) *= std::clamp( ( 750.f - ( dist - 250.f ) ) / 750.f, 0.f, 1.f );
							};

							draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
								col, name );
						}
						if( ( glow || g_settings.visuals.weapon_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
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

					if( ( g_settings.visuals.grenade_esp == 2 || g_settings.visuals.grenade_esp == 3 ) && !g_settings.misc.hide_from_obs ) {
						*( bool* )( uintptr_t( ent ) + m_bShouldGlow ) = true;
					}

					if( g_settings.visuals.grenade_esp == 1 || g_settings.visuals.grenade_esp == 3 ) {
						auto pos = ent->GetRenderOrigin( );
						auto w2s = util::screen_transform( pos );
						auto cutie = g_csgo.m_entlist( )->GetClientEntityFromHandle( ent->as< c_base_weapon >( )->m_hOwnerEntity( ) );

						auto dist = g_ctx.m_local->m_vecOrigin( ).dist_to( pos );

						clr_t col = g_settings.visuals.grenade_esp_clr;
						if( dist > 250.f && !strstr( hdr->name, xors( "thrown" ) ) ) {
							col.a( ) *= ( 750.f - ( dist - 250.f ) ) / 750.f;
						}

						draw_string( w2s.x, w2s.y, ALIGN_CENTER, false,
							col, name.c_str( ) );

						if( g_settings.visuals.grenade_owner( ) )
							draw_string( w2s.x, w2s.y + 10, ALIGN_CENTER, false, col, cutie->get_info( ).name );
					}
				}
			}
		}
	}
	void c_visuals::draw_firegrenade( ) {
		static auto life_span = g_csgo.m_cvar()->FindVar(xors("inferno_flame_lifetime"));
		float time = g_csgo.m_globals->m_curtime;

		if (m_firegrenades.empty())
			return;

		for (size_t i{ }; i < m_firegrenades.size() && !m_firegrenades.empty(); ++i) {
			auto& tr = m_firegrenades[i];

			float delta = g_csgo.m_globals->m_curtime - tr.m_time;
			if (std::abs(delta) > life_span->get_float()) m_firegrenades.erase(m_firegrenades.begin() + i);
		}

		if (!m_firegrenades.empty()) {
			for (auto& it : m_firegrenades) {
				auto ent = g_csgo.m_entlist()->GetClientEntity(it.m_ent);
				if (!ent || !ent->is_player())
					continue;

				clr_t col = g_settings.visuals.grenade_esp_clr;

				vec3_t text_pos = it.m_pos;
				text_pos.z += 5.f;

				auto w2s = util::screen_transform(text_pos);

				float delta = g_csgo.m_globals->m_curtime - it.m_time;

				if (g_ctx.m_local->is_valid() && (ent->m_iTeamNum() != g_ctx.m_local->m_iTeamNum() || ent == g_ctx.m_local)) {
					vec3_t last_pos;

					const float fill = (life_span->get_float() - delta) / (life_span->get_float()) * 180.f;

					for (float rot = -fill; rot <= fill; rot += 3.f) {
						auto rotation = rot + delta * 90.f;

						while (rotation > 360.f)
							rotation -= 360.f;

						vec3_t rotated_pos = it.m_pos;

						rotated_pos.z -= 5.f;

						const auto radius = 50.f;
						rotated_pos.x += cos(DEG2RAD(rotation)) * radius;
						rotated_pos.y += sin(DEG2RAD(rotation)) * radius;

						if (rot != -fill) {
							auto w2s_new = util::screen_transform(rotated_pos);
							auto w2s_old = util::screen_transform(last_pos);
							auto alpha = 1.f - (std::abs(rot) / fill);

							float threshold = life_span->get_float() * 0.2f;

							if (life_span->get_float() - delta < threshold) {
								float diff = (life_span->get_float() - delta) / (life_span->get_float()) * 5.f;

								alpha *= diff;
							}

							clr_t col = g_settings.visuals.grenade_esp_clr;

							col.a() *= alpha;

							draw_line(w2s_old, w2s_new, col);
						}

						last_pos = rotated_pos;
					}
				}

				draw_string(w2s.x, w2s.y, ALIGN_CENTER, false, col, xors("molotov"));
				if (g_settings.visuals.grenade_owner)
					draw_string(w2s.x, w2s.y + 10, ALIGN_CENTER, false, col, ent->get_info().name);
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

		if( !g_settings.visuals.world_modulate || !g_csgo.m_engine( )->IsInGame( ) || g_settings.misc.hide_from_obs ) {
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