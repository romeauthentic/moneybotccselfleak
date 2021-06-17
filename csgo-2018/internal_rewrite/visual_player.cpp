#include <algorithm>

#include "visual.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "renderer.hpp"
#include "input_system.hpp"
#include "math.hpp"
#include "d3d.hpp"

#undef PlaySound

//2k lines of code here
int screen_w, screen_h;
con_var< bool > dbg_anims{ &data::holder_, fnv( "dbg_anims" ), false };

namespace features
{
	void c_visuals::update_position( int index, const vec3_t& pos ) {
		m_stored_pos[ index ] = pos;
		if( m_anim_progress[ index ] <= 0.3f && g_settings.visuals.dormant ) {
			m_anim_progress[ index ] = 0.3f;
		}
	}

	void c_visuals::store_hit( ) {
		if( !g_settings.visuals.hitmarkers )
			return;

		g_csgo.m_surface( )->PlaySound( xors( "buttons\\arena_switch_press_02.wav" ) );
		m_last_hit = g_csgo.m_globals->m_curtime;
	}

	void c_visuals::radar( ) {
		if( !g_settings.visuals.radar( ) )
			return;

		for( int i = 0; i < 32; i++ ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );
			if( !ent || !ent->is_valid( ) )
				continue;

			ent->m_bSpotted( ) = true;
		}
	}

	void c_visuals::draw_hits( ) {
		if( !g_settings.visuals.hitmarkers || !g_ctx.run_frame( ) )
			return;

		static const clr_t col_start = clr_t( 231, 75, 75, 255 );
		static const clr_t col_end = clr_t( 0xf4, 0x7c, 0xa8, 255 );

		float delta = ( g_csgo.m_globals->m_curtime - m_last_hit ) * 1.5f;
		if( std::abs( delta ) > 1.0f ) return;

		clr_t blend = clr_t::blend( col_start, col_end, delta );
		if( delta > 0.75f ) {
			blend.a( ) = 255 * ( 1.0f - delta ) * 4.f;
		}

		auto get_rotated_point = [ ]( vec2_t point, float rotation, float distance ) {
			float rad = DEG2RAD( rotation );

			point.x += sin( rad ) * distance;
			point.y += cos( rad ) * distance;

			return point;
		};


		for( size_t i{ }; i < 2; ++i ) {
			float rotation = 135.f + i * 90.f;

			vec2_t center = { screen_w * 0.5f, screen_h * 0.5f };
			vec2_t start = get_rotated_point( center, rotation, 8.f );
			vec2_t end = get_rotated_point( center, rotation, 17.f );

			vec2_t rot_start = get_rotated_point( center, rotation - 180.f, 8.f );
			vec2_t rot_end = get_rotated_point( center, rotation - 180.f, 17.f );

			draw_line( start, end, blend );
			draw_line( rot_start, rot_end, blend );
		}
	}

	struct box_t {
		int x, y, w, h;
	};

	box_t get_box( c_base_player* ent ) {
		const matrix3x4& matrix = ent->m_CoordinateFrame( );

		vec3_t min = ent->m_vecMins( );
		vec3_t max = ent->m_vecMaxs( );

		std::array< vec3_t, 8 > point_list = {
			vec3_t{ min.x, min.y, min.z },
			vec3_t{ min.x, max.y, min.z },
			vec3_t{ max.x, max.y, min.z },
			vec3_t{ max.x, min.y, min.z },
			vec3_t{ max.x, max.y, max.z },
			vec3_t{ min.x, max.y, max.z },
			vec3_t{ min.x, min.y, max.z },
			vec3_t{ max.x, min.y, max.z }
		};

		std::array< float, 8 > x_points;
		std::array< float, 8 > y_points;

		vec2_t origin = util::screen_transform( ent->m_vecOrigin( ) );

		for( auto& it : point_list ) {
			vec3_t backup = it;
			for( int i{ }; i < 3; ++i ) {
				it[ i ] = backup.dot( ( const vec3_t& )( matrix[ i ] ) ) + matrix[ i ][ 3 ];
			}
		}

		for( size_t i{ }; i < 8; ++i ) {
			vec2_t w2s = util::screen_transform( point_list[ i ] );
			x_points[ i ] = w2s.x;
			y_points[ i ] = w2s.y;
		}

		std::sort( x_points.begin( ), x_points.end( ) );
		std::sort( y_points.begin( ), y_points.end( ) );

		int x = ( int )x_points.front( );
		int w = ( int )x_points.back( ) - x;

		int y = ( int )y_points.front( );
		int h = ( int )y_points.back( ) - y;

		return { x - ( int )origin.x, y - ( int )origin.y, w, h };
	}

	void c_visuals::out_of_fov( c_base_player* ent, const vec3_t& pos, clr_t col ) {
		vec2_t	screen;
		vec2_t	circle;

		auto find_point = [ ]( vec2_t& point, float deg_x, float deg_y ) {
			float x2 = screen_w / 2.f;
			float y2 = screen_h / 2.f;

			float d = sqrt( pow( point.x - x2, 2 ) + pow( point.y - y2, 2 ) );
			float r_x = deg_x / d;
			float r_y = deg_y / d;

			point.x = r_x * point.x + ( 1.f - r_x ) * x2;
			point.y = r_y * point.y + ( 1.f - r_y ) * y2;
		};

		auto get_screen_point = [ ]( vec2_t& screen, const vec3_t& delta ) {
			decltype( auto ) w2s_matrix = g_csgo.m_engine( )->WorldToScreenMatrix( );
			float x;
			float w;
			float y;

			screen.x = w2s_matrix[ 0 ][ 0 ] * delta[ 0 ] + w2s_matrix[ 0 ][ 1 ] * delta[ 1 ] + w2s_matrix[ 0 ][ 2 ] * delta[ 2 ] + w2s_matrix[ 0 ][ 3 ];
			screen.y = w2s_matrix[ 1 ][ 0 ] * delta[ 0 ] + w2s_matrix[ 1 ][ 1 ] * delta[ 1 ] + w2s_matrix[ 1 ][ 2 ] * delta[ 2 ] + w2s_matrix[ 1 ][ 3 ];
			w = w2s_matrix[ 3 ][ 0 ] * delta[ 0 ] + w2s_matrix[ 3 ][ 1 ] * delta[ 1 ] + w2s_matrix[ 3 ][ 2 ] * delta[ 2 ] + w2s_matrix[ 3 ][ 3 ];

			if( w < 0.001f ) {
				float invw = -1.0f / w;
				screen.x *= invw;
				screen.y *= invw;
			}
			else {
				float invw = 1.0f / w;
				screen.x *= invw;
				screen.y *= invw;
			}

			x = float( screen_w ) / 2.f;
			y = float( screen_h ) / 2.f;
			x += 0.5f * screen.x * screen_w + 0.5f;
			y -= 0.5f * screen.y * screen_h + 0.5f;
			screen.x = x;
			screen.y = y;
		};

		screen = util::screen_transform( pos );//get_screen_point( screen, pos );
		circle = util::screen_transform( pos );// get_screen_point( circle, pos );

		float radius = g_settings.visuals.out_of_pov_radius * 0.49f;

		float ratio = g_settings.visuals.out_of_pov_radius;

		float w = screen_w * ratio + screen_h * ( 1.0f - ratio );

		find_point( screen, w * radius,
			float( screen_h ) * radius );

		auto min = std::min< int >( screen_w, screen_h ) * radius;
		find_point( circle, float( min ), float( min ) );

		auto rot_around_center = [ ]( vec2_t start, float rot ) {
			float rad = rot * ( M_PI / 180.f );

			start.x += sin( rad ) * float( g_settings.visuals.out_of_pov_size );
			start.y += cos( rad ) * float( g_settings.visuals.out_of_pov_size );

			return start;
		};

		float delta_x = ( float( screen_w / 2 ) - circle.x );
		float delta_y = ( float( screen_h / 2 ) - circle.y );

		auto hyp = sqrt( delta_x * delta_x + delta_y * delta_y );

		float cos_ = delta_x / hyp;

		float deg = RAD2DEG( acos( cos_ ) );

		if( screen.y < screen_h / 2 ) {
			deg *= -1.f;
		}

		auto rotated_pos_1 = rot_around_center( screen, deg + 115 );
		auto rotated_pos_2 = rot_around_center( screen, deg + 65 );

		col.a( ) *= 0.8f;

		vertex_t v[ ] = {
			{ screen },
		{ rotated_pos_1 },
		{ rotated_pos_2 }
		};

		if( !g_settings.misc.hide_from_obs )
			g_renderer.draw_polygon( 3, v, col );
		else {
			draw_line( screen, rotated_pos_1, col );
			draw_line( screen, rotated_pos_2, col );
			draw_line( rotated_pos_1, rotated_pos_2, col );
		}
	}

	void c_visuals::update_glow( ) {
		if( !g_settings.visuals.active || g_settings.misc.hide_from_obs )
			return;

		static auto manager_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "0F 11 05 00 00 00 00 83 C8 01" ), 0x3 );

		auto glow_object_manager = *( GlowObjectManager_t** )( manager_ptr );
		auto glow_count = glow_object_manager->Count;
		auto glow_objects = glow_object_manager->DataPtr;

		if( glow_count > 500 || glow_count <= 0 )
			return;

		for( int i{ }; i < glow_count; ++i ) {
			auto& object = glow_objects[ i ];

			auto ent = object.m_pEntity->as< c_base_player >( );
			if( !ent ) continue;

			if( ent->is_player( ) ) {
				if( g_settings.visuals.glow &&
					ent->is_valid( ) && ( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) ||
						g_settings.visuals.friendlies || g_settings.visuals.chams.friendlies ) ) {

					clr_t clr = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ?
						g_settings.visuals.glow_friendly : g_settings.visuals.glow_enemy;

					object.color = clr.to_fclr( );
					object.m_bRenderWhenOccluded = true;
					object.m_bRenderWhenUnoccluded = false;
					object.m_flBloomAmount = 0.85f;
				}
			}
			else {
				auto ce = ent->ce( );
				auto client_class = ce->GetClientClass( );
				if( !client_class ) continue;
				int class_id = client_class->m_class_id;
				if( class_id != CBaseWeaponWorldModel && ( strstr( client_class->m_name, xors( "Weapon" ) )
					|| class_id == CDEagle || class_id == CAK47 ) ) {
					bool glow = g_settings.visuals.weapon_esp == 2 || g_settings.visuals.weapon_esp == 3;
					object.color = g_settings.visuals.weapon_esp_clr( ).to_fclr( );
					object.m_bRenderWhenOccluded = true;
					object.m_bRenderWhenUnoccluded = false;
					object.m_flBloomAmount = glow ? 0.85f : 0.0f;
				}
				else {
					auto model = ce->GetModel( );
					if( !model ) continue;

					bool glow = g_settings.visuals.grenade_esp( ) == 2 || g_settings.visuals.grenade_esp( ) == 3;
					auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
					if( !strstr( hdr->name, xors( "thrown" ) ) && !strstr( hdr->name, xors( "dropped" ) ) )
						continue;

					object.color = g_settings.visuals.grenade_esp_clr( ).to_fclr( );
					object.m_bRenderWhenOccluded = true;
					object.m_bRenderWhenUnoccluded = false;
					object.m_flBloomAmount = glow ? 0.85f : 0.0f;
				}
			}
		}
	}

	inline clr_t blend_clr( clr_t in, float progress ) {
		static const clr_t clr_gray = { 160, 160, 160, 255 };
		int a = in.a( );

		clr_t ret = clr_t::blend( clr_gray, in, 0.1f + progress * 0.9f );
		ret.a( ) = a;
		return ret;
	}

	void c_visuals::get_text_size(int& w, int& h, const char* msg, ...) {
		char* buffer = (char*)_alloca(1024);
		va_list list{ };

		memset(buffer, 0, 1024);

		__crt_va_start(list, msg);
		vsprintf_s(buffer, 1024, msg, list);
		__crt_va_end(list);

		if (g_settings.misc.hide_from_obs()) {
			h = g_d3d.get_text_height(::d3d::fonts.f_12, D3DFONTFLAG_DROPSHADOW, buffer);
			w = g_d3d.get_text_width(::d3d::fonts.f_12, D3DFONTFLAG_DROPSHADOW, buffer);
		}
		else {
			g_csgo.m_surface()->GetTextSize(g_fonts.f_12, util::ascii_to_unicode(buffer).c_str(), w, h);
		}
	}

	void c_visuals::draw_players( ) {
		static constexpr float anim_rate = 1.0f / 0.3f;
		static float pov_progress[ 65 ]{ };

		auto resource = c_base_player::get_player_resource( );

		for( int i{ }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if( !ent || !ent->is_player( ) || !ent->is_alive( ) || ent == g_ctx.m_local )
				continue;

			if( i == ( g_ctx.m_local->m_hObserverTarget( ) & 0xfff ) )
				continue;

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) &&
				!g_settings.visuals.friendlies( ) )
				continue;

			float   rate = g_csgo.m_globals->m_frametime * anim_rate;
			float&	anim = m_anim_progress[ i ];
			float	alpha = anim;
			bool	dormant = ent->ce()->IsDormant();
			int		health = ent->m_iHealth( );
			auto	origin = ent->m_vecOrigin( );
			auto	box = get_box( ent );
			int		right_pos = 0;
			int		bottom_pos = 0;
			bool	too_distant = true;
			if( g_ctx.m_local )
				too_distant = ent->m_vecOrigin( ).dist_to( g_ctx.m_local->m_vecOrigin( ) ) > 2500.f;

			if( !dormant ) {
				update_position( i, origin );
				anim = 1.0f;
			}
			else {
				if( anim < 0.3f && g_settings.visuals.dormant && !too_distant ) {
					rate *= 0.01f;
				}
				anim = std::clamp( anim -= rate, 0.f, 1.0f );
				if( m_anim_progress[ i ] <= 0.f )
					continue;
			}

			clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ?
				g_settings.visuals.box_friendly : g_settings.visuals.box_enemy;

			if( dormant ) {
				col = blend_clr( col, anim );
				col.a( ) *= anim;
			}

			auto w2s_cur_origin = util::screen_transform( m_stored_pos[ i ] );

			box.x += w2s_cur_origin.x;
			box.y += w2s_cur_origin.y;

			if( box.x > screen_w || box.x + box.w < 0 ||
				box.y > screen_h || box.y + box.h < 0 ) {
				if( g_settings.visuals.out_of_pov ) {
					auto& anim = pov_progress[ i ];
					anim = std::clamp( anim += g_csgo.m_globals->m_frametime * anim_rate, 0.f, 1.0f );
					if( dormant )
						anim = std::clamp( anim -= g_csgo.m_globals->m_frametime * anim_rate, 0.f, 1.0f );
					else
						anim = std::clamp( anim += g_csgo.m_globals->m_frametime * anim_rate, 0.f, 1.0f );

					col.a( ) *= anim;
					out_of_fov( ent, ent->ce( )->GetRenderOrigin( ), col );
				}
				continue;
			}

			pov_progress[ i ] = 0.f;

			if( g_settings.visuals.skeleton( ) && !dormant ) {
				clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.skeleton_friendly : g_settings.visuals.skeleton_enemy;
				col.a( ) *= alpha;

				auto hdr = g_csgo.m_model_info( )->GetStudiomodel( ent->ce( )->GetModel( ) );
				if( hdr ) {
					matrix3x4 matrix[ 128 ];
					memcpy( matrix, ent->m_CachedBoneData( ).GetElements( ), 
						ent->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

					for( size_t bone{ }; bone < hdr->numbones; ++bone ) {
						auto b = hdr->GetBone( bone );
						if( b && b->flags & 0x100 && b->parent != -1 ) {
							vec3_t child = vec3_t{ matrix[ bone ][ 0 ][ 3 ], matrix[ bone ][ 1 ][ 3 ], matrix[ bone ][ 2 ][ 3 ] };
							vec3_t parent = vec3_t{ matrix[ b->parent ][ 0 ][ 3 ], matrix[ b->parent ][ 1 ][ 3 ], matrix[ b->parent ][ 2 ][ 3 ] };

							auto child_screen = util::screen_transform( child );
							auto parent_screen = util::screen_transform( parent );

							draw_line( child_screen, parent_screen, col );
						}
					}
				}
			}

			if (g_settings.visuals.box()) {
				auto _y = box.y - 14;

				int  text_w, text_h;
				char name[32];
				ent->get_name_safe(name);

				get_text_size(text_w, text_h, name);

				text_w = math::max(text_w, 10);

				int mid = box.x + box.w / 2;
				int _x = mid - text_w / 2 - 5;

				draw_filled_rect(_x, _y, text_w + 10, text_h + 1, clr_t(21, 21, 21, 170 * alpha));
				draw_string(mid, _y, ALIGN_CENTER, false, clr_t(221, 221, 221, 255 * alpha), name);

				auto fill = text_w + 10;
				fill *= std::clamp(health, 0, 100) * 0.01f;
				fill *= 0.5f;

				auto hp_col = clr_t(std::min< int >(510 * (100 - health) / 100, 255),
					std::min< int >(510 * health / 100, 255),
					0,
					255 * alpha);

				draw_line(
					vec2_t(mid - fill, _y + text_h + 1), vec2_t(mid, _y + text_h + 1), hp_col);
				draw_line(
					vec2_t(mid, _y + text_h + 1), vec2_t(mid + fill - 1, _y + text_h + 1), hp_col);
			}

			if (g_settings.visuals.weapon) {
				auto weapon = ent->get_weapon();

				if (weapon) {
					auto weapon_info = weapon->get_wpn_info();
					int  max = weapon_info->max_clip_ammo;

					std::string name =
						util::definition_index_to_name(weapon->m_iItemDefinitionIndex());

					auto  _y = box.y + box.h + 13;
					float progress = 1.f;

					if (max >= 1) {
						if (ent->is_reloading()) {
							auto gun_layer = ent->m_AnimOverlay().GetElements()[1];
							progress = gun_layer.m_flCycle;

							int bullets = max * progress;
							name += " [" + std::to_string(bullets) + "]";
						}
						else {
							progress = float(weapon->m_iClip1()) / max;
							name += " [" + std::to_string(weapon->m_iClip1()) + "]";
						}
					}

					int text_w, text_h;

					get_text_size(text_w, text_h, name.c_str());

					int mid = box.x + box.w / 2;
					int _x = mid - text_w / 2 - 5;

					_y -= text_h;

					draw_filled_rect(
						_x, _y, text_w + 10, text_h + 1, clr_t(21, 21, 21, 170 * alpha));
					draw_string(mid,
						_y + 2,
						ALIGN_CENTER,
						false,
						clr_t(221, 221, 221, 255 * alpha),
						name.c_str());

					int fill = text_w + 10;
					fill *= progress;
					fill *= 0.5f;

					auto color = g_settings.visuals.ammo_bar_clr();
					color.a() *= alpha;
					draw_line(vec2_t(mid - fill, _y), vec2_t(mid, _y), color);
					draw_line(vec2_t(mid, _y), vec2_t(mid + fill - 1, _y), color);
				}
			}



			if (g_settings.visuals.money()) {
				int x_pos = box.x + box.w + 2;
				int y_pos = box.y - 1 + right_pos;
				draw_string(x_pos,
					y_pos,
					ALIGN_LEFT,
					false,
					blend_clr(esp_green(255 * alpha), anim),
					xors("%d$"),
					ent->m_iAccount());

				right_pos += 9;
			}

			if (g_settings.visuals.scoped()) {
				int x_pos = box.x + box.w + 3;
				int y_pos = box.y - 1;

				if (ent->m_bIsScoped() && right_pos < box.h) {
					draw_string(x_pos,
						y_pos + right_pos,
						ALIGN_LEFT,
						false,
						clr_t(255, 255, 255, 255 * alpha),
						xors("scope"));
					right_pos += 9;
				}
			}

			if (g_settings.visuals.c4_carrier()) {
				int x_pos = box.x + box.w + 3;
				int y_pos = box.y - 1;

				if (ent->get_c4_carrier() && right_pos < box.h) {
					draw_string(x_pos,
						y_pos + right_pos,
						ALIGN_LEFT,
						false,
						clr_t(255, 255, 255, 255 * alpha),
						xors("c4"));
					right_pos += 9;
				}
			}

			if (g_settings.visuals.ping()) {
				auto ping = ent->get_ping();

				if (ping > 310) {
					int x_pos = box.x + box.w + 2;
					int y_pos = box.y - 1 + right_pos;
					draw_string(x_pos,
						y_pos,
						ALIGN_LEFT,
						false,
						blend_clr(esp_blue(255 * alpha), anim),
						xors("ping"));

					right_pos += 9;
				}
			}

			if (g_settings.visuals.one_way() && !(util::is_low_fps() && g_settings.rage.preserve_fps() && right_pos < box.h)) {
				if (m_hit_flag[i] && g_ctx.m_local->is_valid() &&
					i != g_cheat.m_ragebot.get_target() && g_settings.rage.enabled) {
					int x_pos = box.x + box.w + 3;
					int y_pos = box.y - 1;

					clr_t col = m_hit_flag[i] == HIT_1W ? clr_t(255, 255, 255, 255 * alpha)
						: esp_red(alpha * 255);
					draw_string(x_pos,
						y_pos + right_pos,
						ALIGN_LEFT,
						false,
						blend_clr(col, anim),
						xors("hit"));
					right_pos += 9;
				}
			}

			if( g_settings.visuals.resolver_indicator && g_settings.rage.resolver && g_cheat.m_player_mgr.is_cheater( i ) && !dormant && g_ctx.m_local->is_valid( ) ) {
				bool can_backtrack_lby = g_cheat.m_ragebot.m_lagcomp->can_backtrack_entity( i ) == RECORD_LBY && !ent->is_breaking_lc( );
				bool can_backtrack = !!g_cheat.m_ragebot.m_lagcomp->can_backtrack_entity( i ) && !ent->is_breaking_lc( );
				int x_pos = box.x + box.w + 3;
				int y_pos = box.y - 1 + right_pos;

				bool is_fake = !can_backtrack_lby;
				bool is_breaking_lc = g_cheat.m_prediction.is_breaking_lc( i );

				if( is_breaking_lc ) {
					draw_string( x_pos, y_pos, ALIGN_LEFT, false,
						clr_t( 255, 255, 255, 255 * alpha ),
						xors( "lc" ) );

					right_pos += 9;
				}
				else if( is_fake ) {
					draw_string( x_pos, y_pos, ALIGN_LEFT, false,
						clr_t( 255, 255, 255, 255 * alpha ),
						xors( "fake" ) );

					right_pos += 9;
				}

				if( !!g_cheat.m_ragebot.m_resolver->get_breaking_state( i ) && !dormant && g_ctx.m_local->is_valid( ) ) {
					float update = g_cheat.m_ragebot.m_lagcomp->get_flick_time( i );
					float delta = ( ent->m_flSimulationTime( ) - update ) * 0.9f;
					if( delta >= 0.f && delta <= 1.f ) {
						float max = box.w - 2.f;
						float fill = max * std::clamp( delta, 0.f, 1.f );

						draw_filled_rect( box.x, box.y + box.h + 3 + bottom_pos,
							box.w, 3, clr_t( 0, 0, 0, 180 * alpha ) );

						clr_t col = g_settings.visuals.lby_bar_clr;
						col.a( ) *= alpha;

						draw_filled_rect( box.x + 1, box.y + box.h + 4 + bottom_pos,
							fill, 1, blend_clr( col, anim ) );

						bottom_pos += 4;
					}
				}
			}

			if( g_settings.visuals.ping( ) ) {
				auto ping = ent->get_ping( );

				if( ping > 310 ) {
					int x_pos = box.x + box.w + 3;
					int y_pos = box.y - 1 + right_pos;

					draw_string( x_pos, y_pos, ALIGN_LEFT, false,
						blend_clr( esp_blue( 255 * alpha ), anim ),
						xors( "ping" ) );

					right_pos += 9;
				}
			}
			

#ifdef _DEBUG
			static con_var< bool > dbg_bt{ &data::holder_, fnv( "dbg_bt" ), false };
			if( dbg_bt( ) && i == g_cheat.m_ragebot.get_shot_target( ) ) {
				auto hdr = g_csgo.m_model_info( )->GetStudiomodel( ent->ce( )->GetModel( ) );
				if( hdr ) {
					auto matrix = g_cheat.m_ragebot.get_shot_matrix( );
					for( size_t bone{ }; bone < hdr->numbones; ++bone ) {
						auto b = hdr->GetBone( bone );
						if( b && b->flags & 0x100 && b->parent != -1 ) {
							vec3_t child = vec3_t{ matrix[ bone ][ 0 ][ 3 ], matrix[ bone ][ 1 ][ 3 ], matrix[ bone ][ 2 ][ 3 ] };
							vec3_t parent = vec3_t{ matrix[ b->parent ][ 0 ][ 3 ], matrix[ b->parent ][ 1 ][ 3 ], matrix[ b->parent ][ 2 ][ 3 ] };

							auto child_screen = util::screen_transform( child );
							auto parent_screen = util::screen_transform( parent );

							draw_line( child_screen, parent_screen, clr_t( 255, 255, 255, 200 ) );
						}
					}
				}
			}

			static con_var< bool > dbg_pose{ &data::holder_, fnv( "dbg_pose" ) };
			if( dbg_pose( ) ) {
				for( size_t i{ }; i < 24; ++i ) {
					int x = box.x + box.w + 15;
					int y = box.y + 11 * i;

					draw_string( x, y, ALIGN_LEFT, false, clr_t( 255, 255, 255 ), "%d %f", i, ent->m_flPoseParameter( )[ i ] );
				}
			}

			static con_var< bool > dbg_anim{ &data::holder_, fnv( "dbg_anim" ) };
			if( dbg_anim( ) ) {
				draw_string( box.x - 14, box.y - 15, ALIGN_RIGHT, false, clr_t( 255, 255, 255 ), "%f", ent->get_animdata( ).m_last_velocity.length2d( ) );
				draw_string( box.x - 14, box.y, ALIGN_RIGHT, false, clr_t( 255, 255, 255 ), "%f", ent->get_animdata( ).m_anim_velocity.length2d( ) );
				for( size_t i{ }; i < 13; ++i ) {
					int x = box.x - 15;
					int y = box.y + 11 * ( i + 2 );

					draw_string( x, y, ALIGN_RIGHT, false, clr_t( 255, 255, 255 ), "%d %f", i, ent->m_AnimOverlay( ).GetElements( )[ i ].m_flCycle );
				}
			}
#endif
		}
	}

	void c_visuals::store_sound( int index, vec3_t origin ) {
		if( !g_settings.visuals.sound )
			return;

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( index );
		if( ent && ent->is_player( ) && ent->is_alive( ) && ent != g_ctx.m_local ) {
			if( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || ( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && g_settings.visuals.friendlies( ) ) ) {
				sound_t new_sound;
				new_sound.m_time = g_csgo.m_globals->m_curtime;
				new_sound.m_ent = index;
				new_sound.m_pos = origin;
				new_sound.m_pos.z += 5.f;
				m_sounds.emplace_back( new_sound );
			}
		}
	}

	void c_visuals::update_hit_flags( ) {
		if( !g_settings.visuals.flags || !g_settings.rage.enabled )
			return;

		if( util::is_low_fps( ) && g_settings.rage.preserve_fps )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );

		for( size_t i{ }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );

			if( !ent || !ent->is_valid( ) || !ent->has_valid_anim( ) ||
				( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
				|| ent->m_bGunGameImmunity( ) ) {
				m_hit_flag[ i ] = HIT_NONE;
				continue;
			}

			vec3_t local_pos = g_ctx.m_local->get_eye_pos( );
			vec3_t enemy_pos = ent->get_hitbox_pos( 0 );

			auto local_wep = g_ctx.m_local->get_weapon( );
			auto enemy_wep = ent->get_weapon( );

			if( !local_wep || !enemy_wep || local_wep->is_knife( ) || enemy_wep->is_knife( ) || local_wep->is_grenade( ) || enemy_wep->is_grenade( ) ) {
				m_hit_flag[ i ] = HIT_NONE;
				continue;
			}

			float local_dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, enemy_pos, false );
			float enemy_dmg = g_cheat.m_autowall.run( ent, g_ctx.m_local, g_ctx.m_local->get_hitbox_pos( 0 ), false );

			auto min_dmg = g_cheat.m_ragebot.get_min_dmg( ent );

			if( enemy_dmg < 5.f && local_dmg > g_cheat.m_ragebot.get_min_dmg( ent ) )
				m_hit_flag[ i ] = HIT_1W;
			else if( enemy_dmg > 15.f && local_dmg < g_cheat.m_ragebot.get_min_dmg( ent ) )
				m_hit_flag[ i ] = HIT_ALERT;
			else
				m_hit_flag[ i ] = HIT_NONE;
		}
	}

	void c_visuals::store_ent_dmg( int attacker, int entindex, int dmg ) {
		if( entindex == g_ctx.m_local->ce( )->GetIndex( ) )
			return;
		
		bool dead = false;
		int hp = 100 - m_ent_dmg[ entindex ];
		
		if( hp - dmg < 0 ) {
			dmg = hp;
			dead = true;
		}

		m_ent_dmg[ entindex ] += dmg;

		auto ent = g_csgo.m_entlist( )->GetClientEntity( entindex );
		if( attacker == g_csgo.m_engine( )->GetLocalPlayer( ) && ent && ent->is_player( ) && ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && g_ctx.m_local->is_valid( ) ) {
			m_teamdmg += ( g_csgo.m_globals->m_curtime - m_last_roundstart > 10.f ) || dead ? dmg : dmg * 2;
		}
	}

	void c_visuals::on_round_start( ) {
		m_last_roundstart = g_csgo.m_globals->m_curtime;
		for( size_t i{ }; i < 65; ++i ) {
			m_ent_dmg[ i ] = 0;
		}
	}

	void c_visuals::reset_local_dmg( ) {
		m_teamdmg = 0;
	}

	void c_visuals::draw_sound( ) {
		if( !g_settings.visuals.active ) return;
		if( g_settings.misc.hide_from_obs ) return;

		float time = g_csgo.m_globals->m_curtime;

		if( m_sounds.empty( ) )
			return;


		for( size_t i{ }; i < m_sounds.size( ) && !m_sounds.empty( ); ++i ) {
			auto& tr = m_sounds[ i ];

			float delta = g_csgo.m_globals->m_curtime - tr.m_time;
			if( delta > 0.001f || std::abs( delta ) > 0.001f ) m_sounds.erase( m_sounds.begin( ) + i );
		}

		if( !m_sounds.empty( ) ) {
			for( auto& it : m_sounds ) {
				auto ent = g_csgo.m_entlist( )->GetClientEntity( it.m_ent );
				if( !ent || !ent->is_player( ) )
					continue;

				clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.box_friendly : g_settings.visuals.box_enemy;
				float delta = time - it.m_time;
				if( !g_ctx.precache_model( xors( "materials/sprites/laserbeam.vmt" ) ) ) {
					g_con->log( "nigga cant get" );
					continue;
				}

				BeamInfo_t beam_info;

				beam_info.m_nType = beam_ring_point;
				beam_info.m_pszModelName = xors( "materials/sprites/laserbeam.vmt" );
				beam_info.m_nModelIndex = g_csgo.m_model_info( )->GetModelIndex( xors( "materials/sprites/laserbeam.vmt" ) );
				beam_info.m_flHaloScale = 0.0f;
				beam_info.m_flLife = 0.75f; //0.09
				beam_info.m_flWidth = 1.5f;
				beam_info.m_flEndWidth = 1.5f;
				beam_info.m_flFadeLength = 1.0f;
				beam_info.m_flAmplitude = 0.f;
				beam_info.m_flBrightness = col.a( );
				beam_info.m_flSpeed = 2.f;
				beam_info.m_nStartFrame = 0;
				beam_info.m_flFrameRate = 60;
				beam_info.m_flRed = col.r( );
				beam_info.m_flGreen = col.g( );
				beam_info.m_flBlue = col.b( );
				beam_info.m_nSegments = 1;
				beam_info.m_bRenderable = true;
				beam_info.m_nFlags = 0;

				beam_info.m_vecCenter = it.m_pos;
				beam_info.m_flStartRadius = 0.f;
				beam_info.m_flEndRadius = ( float )g_settings.visuals.sound_range( );

				Beam_t* beam = g_csgo.m_beams( )->CreateBeamRingPoint( beam_info );

				if( beam ) {
					g_csgo.m_beams( )->DrawBeam( beam );
				}
			}
		}
	}

	void c_visuals::operator()( ) {
		g_csgo.m_engine( )->GetScreenSize( screen_w, screen_h );
		if( g_ctx.run_frame( ) ) {
			draw_local( );
		}

		if( !g_ctx.m_local )
			return;

		switch( g_settings.visuals.activation_type( ) ) {
		case 0:
			g_settings.visuals.active = false;
			break;
		case 1:
			g_settings.visuals.active = true;
			break;
		case 2:
			g_settings.visuals.active = g_input.is_key_pressed( g_settings.visuals.key );
			break;
		case 3: {
			static bool held = false;
			bool pressed = g_input.is_key_pressed( g_settings.visuals.key );
			if( pressed ) {
				if( !held )
					g_settings.visuals.active ^= 1;
				held = true;
			}
			else held = false;
		}
				break;
		default:
			g_settings.visuals.active = false;
			break;
		}

		if( g_settings.visuals.active ) {			
			draw_players( );
		}

		draw_world( );
		draw_tracers( );
		draw_hits( );
	}
}