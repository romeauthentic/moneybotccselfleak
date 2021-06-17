#include <algorithm>

#include "visuals.hpp"
#include "ctx.hpp"
#include "base_cheat.h"
#include "renderer.hpp"
#include "input_system.hpp"
#include "math.h"
#include "settings.h"
#include "interfaces.h"
//2k lines of code here
int screen_w, screen_h;

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

		cl.m_surface( )->play_sound( xors( "buttons\\arena_switch_press_02.wav" ) );
		m_last_hit = cl.m_globals->curtime;
	}


	void c_visuals::draw_hits( ) {
		if( !g_settings.visuals.hitmarkers || !g_ctx.run_frame( ) )
			return;

		static const clr_t col_start = clr_t( 231, 75, 75, 255 );
		static const clr_t col_end = clr_t( 0xf4, 0x7c, 0xa8, 255 );

		float delta = ( cl.m_globals->curtime - m_last_hit ) * 1.5f;
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
		const matrix3x4& matrix = ent->m_CollisionGroup( );

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
			decltype( auto ) w2s_matrix = cl.m_engine( )->GetWorldToScreenMatrix( );
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

		static auto glow_object_manager = *pattern::first_code_match< GlowObjectManager_t** >( cl.m_chl.dll( ), xors( "B9 ? ? ? ? E8 ? ? ? ? B0 01 5D" ), 0x1 );
		if( !glow_object_manager ) return;

		static bool invalidated;
		if( !g_settings.visuals.glow( ) ) {
			if( !invalidated ) {
				invalidate_glow( );
				invalidated = true;
			}
			return;
		}

		invalidated = false;
		for( int i{ }; i < 64; ++i ) {
			auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );
			if( !ent ) continue;

			if( !ent->is_valid( ) ) {
				continue;
			}

			if( ent->is_enemy( ) || g_settings.visuals.friendlies ) {
				*( bool* )( uintptr_t( ent ) + 0xdbd ) = true;
				//ent->update_glow_effect( );
			}
		}

		if( glow_object_manager->data.Count( ) <= 0 ) return; //data count crashed yea lazy cool cool? cool

		for( int i{ }; i < glow_object_manager->data.Count( ); ++i ) {
			auto ent = ( c_base_player* )cl.m_entlist( )->get_entity_from_handle( glow_object_manager->data[ i ].ent_ptr );
			if( ent && ent->is_valid( ) && ( ent->is_enemy( ) || g_settings.visuals.friendlies( ) ) ) {
				clr_t clr = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.glow_friendly : g_settings.visuals.glow_enemy;
				glow_object_manager->data[ i ].glow_color = clr.to_fclr( );
			}
		}

	}

	void c_visuals::invalidate_glow( ) {
		for( int i{ }; i < 64; i++ ) {
			auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );
			if( !ent ) continue;
			if( ent->get_client_class( )->m_class_id == CTFPlayer ) {
				*( bool* )( uintptr_t( ent ) + 0xdbd ) = false;
				ent->destroy_glow_effect( );
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

	void c_visuals::draw_players( ) {
		static constexpr float anim_rate = 1.0f / 0.3f;
		static float pov_progress[ 65 ]{ };

		auto resource = c_base_player::get_player_resource( );

		for( int i{ }; i < 65; ++i ) {
			auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );

			if( !ent || !ent->is_player( ) || !ent->is_alive( ) || ent == g_ctx.m_local )
				continue;

			//if( i == ( g_ctx.m_local->m_hObserverTarget( ) & 0xfff ) )
			//	continue;

			//printf( "%d valid (%s)\n", i, ent->get_info( ).name );

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) &&
				!g_settings.visuals.friendlies( ) )
				continue;

			float   rate = cl.m_globals->frametime * anim_rate;
			float&	anim = m_anim_progress[ i ];
			float	alpha = anim;
			bool	dormant = ent->ce( )->is_dormant( );
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
					anim = std::clamp( anim += cl.m_globals->frametime * anim_rate, 0.f, 1.0f );
					if( dormant )
						anim = std::clamp( anim -= cl.m_globals->frametime * anim_rate, 0.f, 1.0f );
					else
						anim = std::clamp( anim += cl.m_globals->frametime * anim_rate, 0.f, 1.0f );

					col.a( ) *= anim;
					out_of_fov( ent, origin, col );
				}
				continue;
			}

			pov_progress[ i ] = 0.f;

			if( g_settings.visuals.skeleton( ) && !dormant ) {
				clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.skeleton_friendly : g_settings.visuals.skeleton_enemy;
				col.a( ) *= alpha;

				auto hdr = cl.m_modelinfo( )->GetStudioModel( ent->ce( )->get_model( ) );
				if( hdr ) {
					matrix3x4 matrix[ 128 ];
					ent->ce( )->setup_bones( matrix, 128, 0x100, 0.f );

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

			if( g_settings.visuals.box( ) ){
				auto _y = box.y - 14;

				int text_w, text_h;
				char name[ 32 ];
				ent->get_name_safe( name );

				get_text_size( text_w, text_h, name );

				text_w = math::max( text_w, 10 );

				int mid = box.x + box.w / 2;
				int _x = mid - text_w / 2 - 5;

				draw_filled_rect( _x, _y, text_w + 10, text_h + 1, clr_t( 21, 21, 21, 170 * alpha ) );
				draw_string( mid, _y, ALIGN_CENTER, false, clr_t( 221, 221, 221, 255 * alpha ), name );

				auto fill = text_w + 10;
				fill *= std::clamp( health, 0, 100 ) * 0.01f;
				fill *= 0.5f;

				auto hp_col = clr_t(
					std::min< int >( 510 * ( 100 - health ) / 100, 255 ),
					std::min< int >( 510 * health / 100, 255 ),
					0,
					255 * alpha );

				draw_line( vec2_t( mid - fill, _y + text_h + 1 ), vec2_t( mid, _y + text_h + 1 ), hp_col );
				draw_line( vec2_t( mid, _y + text_h + 1 ), vec2_t( mid + fill - 1, _y + text_h + 1 ), hp_col );
			}

			/*if( g_settings.visuals.health( ) ) {
				auto fill = box.h - 1;

				float hp_max = ent->get_max_health( );				
				float hp_percent = health / hp_max;

				fill *= std::clamp( hp_percent, 0.f, 1.f );

				auto hp_col = hp_percent > 1.f ? clr_t( 50, 180, 255 ) : clr_t(
					std::min< int >( 510 * ( 1.f - hp_percent ), 255 ),
					std::min< int >( 510 * hp_percent, 255 ),
					0,
					255 * alpha );

				draw_filled_rect( box.x - 4, box.y, 3, box.h + 1, clr_t( 0, 0, 0, 170 * alpha ) );
				draw_filled_rect( box.x - 3, box.y + box.h - fill, 1, fill, hp_col );

				if( health > hp_max )
					draw_string( box.x - 2, box.y + 1 + box.h - fill - 3, ALIGN_CENTER, false, clr_t( 255, 255, 255, 255 * alpha ), "%d", health );
			}*/

			/*if( g_settings.visuals.name( ) ) {
				draw_string( box.x + box.w / 2, box.y - 12, ALIGN_CENTER, true,
					blend_clr( clr_t( 255, 255, 255, alpha * 255 ), anim ), ent->get_info( ).name );
			}*/

			if( g_settings.visuals.player_class( ) ) {
				int x_pos = box.x + box.w + 2;
				int y_pos = box.y - 1 + right_pos;
				draw_string( x_pos, y_pos, ALIGN_LEFT, false,
					blend_clr( esp_green( 255 * alpha ), anim ), ent->get_class_name( ) );

				right_pos += 9;
			}

			if( g_settings.visuals.weapon ){
				auto weapon = ent->get_active_weapon( );

				if( weapon ){
					int max = weapon->get_max_clip_1( );

					std::string name = weapon->get_name( );
					name.erase( name.find( "tf_weapon_" ), strlen( "tf_weapon_" ) );

					auto _y = box.y + box.h + 13;
					float progress = 1.f;


					if( max >= 1 ){
						/*if( ent->is_reloading( ) ){
							auto gun_layer = ent->m_AnimOverlay( ).GetElements( )[ 1 ];
							progress = gun_layer.m_flCycle;

							int bullets = max * progress;
							name += " [" + std::to_string( bullets ) + "]";
						} else {
							progress = float( weapon->m_iClip1( ) ) / max;
							name += " [" + std::to_string( weapon->m_iClip1( ) ) + "]";
						}*/
						progress = float( weapon->m_iClip1( ) ) / max;
						name += " [" + std::to_string( weapon->m_iClip1( ) ) + "]";
					}




					int text_w, text_h;

					get_text_size( text_w, text_h, name.c_str( ) );

					int mid = box.x + box.w / 2;
					int _x = mid - text_w / 2 - 5;

					_y -= text_h;

					draw_filled_rect( _x, _y, text_w + 10, text_h + 1, clr_t( 21, 21, 21, 170 * alpha ) );
					draw_string( mid, _y + 2, ALIGN_CENTER, false, clr_t( 221, 221, 221, 255 * alpha ), name.c_str( ) );

					int fill = text_w + 10;
					fill *= progress;
					fill *= 0.5f;

					auto color = g_settings.visuals.ammo_bar_clr( );
					color.a( ) *= alpha;
					draw_line( vec2_t( mid - fill, _y ), vec2_t( mid, _y ), color );
					draw_line( vec2_t( mid, _y ), vec2_t( mid + fill - 1, _y ), color );
				}
			}

			/*if( g_settings.visuals.weapon( ) ) {
				auto weapon = ent->get_active_weapon( );
				if( weapon ) { //magic font
					int max = weapon->get_max_clip_1( );

					if( g_settings.visuals.ammo && max > 1 && g_settings.visuals.weapon == 1 && !g_settings.misc.hide_from_obs ) {
						auto progress = float( weapon->m_iClip1( ) ) / max;
						float fill = box.w - 2.f;
						float percent = fill * progress;

						draw_filled_rect( box.x, box.y + box.h + 3 + bottom_pos,
							box.w, 3, clr_t( 0, 0, 0, 180 * alpha ) );

						draw_filled_rect( box.x + 1, box.y + box.h + 4 + bottom_pos,
							percent, 1, blend_clr( clr_t( 66, 143, 244, 255 * alpha ), anim ) );

						if( progress < 0.25f ) {
							draw_string(
								box.x + percent, box.y + box.h + 2 + bottom_pos, ALIGN_LEFT, false,
								clr_t( 255, 255, 255, 180 * alpha ), "%d", weapon->m_iClip1( ) );
						}

						bottom_pos += 4;

					}

					/*if( g_settings.visuals.weapon == 1 && !g_settings.misc.hide_from_obs ) {
						g_renderer.draw_string< ALIGN_CENTER >( 0xa1, box.x + box.w / 2, box.y + box.h + bottom_pos + 3,
							clr_t( 255, 255, 255, alpha * 255 ), "%c", weapon->get_hud_icon( ) );
					}
						char wep_str[ 64 ];
						if( !g_settings.visuals.ammo( ) && weapon->m_iClip1( ) > -1 )
							sprintf_s< 64 >( wep_str, "%s [%d]", weapon->get_print_name( ), weapon->m_iClip1( ) );
						else
							sprintf_s< 64 >( wep_str, "%s", weapon->get_print_name( ) );

						draw_string( box.x + box.w / 2, box.y + box.h + 3 + bottom_pos, ALIGN_CENTER, false,
							clr_t( 255, 255, 255, alpha * 255 ), wep_str );
				}
			}*/

			if( g_settings.visuals.conditions( ) ) {
				int x_pos = box.x + box.w + 3;
				int y_pos = box.y - 1;

				int c = ent->m_nPlayerCond( );

				//this is a bitflag
				//so you do if( ent->m_nPlayerCond( ) & TF_SLOWED ) { //draw shit
				switch( ent->m_nPlayerCond( ) ) {
				case TFCond_Slowed:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "slow" ) );
					right_pos += 9;
					break;
				case TFCond_Zoomed: //dont work too lazy
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "scoped" ) );
					right_pos += 9;
					break;
				case TFCond_Bonked:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "bonk" ) );
					right_pos += 9;
					break;
				case TFCond_Disguised:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "disg" ) );
					right_pos += 9;
					break;
				case TFCond_Cloaked:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "cloak" ) );
					right_pos += 9;
					break;
				case TFCond_Ubercharged:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "uber" ) );
					right_pos += 9;
					break;
				case TFCond_UberchargeFading:
					draw_string( x_pos, y_pos + right_pos, ALIGN_LEFT, false, clr_t( 255, 255, 255, 255 * alpha ), xors( "uber" ) );
					right_pos += 9;
					break;
				}

			}

		}
	}

	void c_visuals::operator()( ) {
		cl.m_engine( )->GetScreenSize( screen_w, screen_h );
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
		draw_hits( );
	}
}