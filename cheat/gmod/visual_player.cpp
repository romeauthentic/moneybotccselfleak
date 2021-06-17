#include <algorithm>

#include "visual.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "renderer.hpp"
#include "input_system.hpp"
#include "math.hpp"

/*
	Change hook where grabbing lua related from RenderView to CEngineVGui::Paint to see if that works.
	I do all my drawing in Paint and should not crash( I never crash ).
*/

namespace features
{

	struct box_t
	{
		int x, y, w, h;
	};

	box_t get_box( c_base_player* ent, matrix3x4& frame ) {
		const matrix3x4& matrix = frame;

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

		return { x, y, w, h };
	}

	void c_visuals::store_data( ) {
		g_ctx.m_lua = g_gmod.m_lua_shared( )->GetLuaInterface( LUA_CLIENT );
		if( !g_ctx.m_lua )
			return;

		m_data.m_matrix = *( VMatrix* )( 0x18C * 2 + *( uintptr_t* )( ( uintptr_t )g_gmod.m_engine_render( ) + 0xDC ) - 0x44 );

		const auto local_id = g_gmod.m_engine( )->GetLocalPlayer( );

		for( int i{ 1 }; i <= g_gmod.m_globals->m_maxclients; ++i ) {
			auto ent = g_gmod.m_entlist( )->GetClientEntity< >( i );
			if( !ent || !ent->is_valid( ) )
				continue;

			auto& data = m_data.m_player.at( i );

			if( g_settings.visuals.ignore_teamcolor ) {
				data.m_team_color = ent->get_team_color( );
			}

			if( i == local_id )
				continue;

			data.m_coordinate_frame = ent->m_CoordinateFrame( );

			if( g_settings.visuals.skeleton ) {
				ent->ce( )->SetupBones( data.m_matrix, 128, 0x100, g_gmod.m_globals->m_curtime ); // yep, 256 is the max bone count in gmod, but i doubt it's used for anything.
			}

			if( g_settings.visuals.rank || g_settings.visuals.spec_list ) {
				data.m_rank = ent->get_rank( );
			}
		}
	}

	void c_visuals::spectator_list( ) {
		if( !g_settings.visuals.spec_list )
			return;

		int cur_pos{ };
		player_info_t info{ };
		char buffer[ 128 ]{ };
		std::vector< const char* > list_of_names{ };

		const int local_id = g_gmod.m_engine( )->GetLocalPlayer( );

		for( int i{ 1 }; i <= g_gmod.m_globals->m_maxclients; ++i ) {
			if( i == local_id )
				continue;

			auto ent = g_gmod.m_entlist( )->GetClientEntity< >( i );
			if( !ent )
				continue;

			if( !g_gmod.m_engine( )->GetPlayerInfo( i, &info ) )
				continue;

			auto target = ent->get_observer_target( );
			if( !target )
				continue;

			if( target->ce( )->GetIndex( ) != local_id )
				continue;

			sprintf_s( buffer, "%s - (%s)", info.name, m_data.m_player.at( i ).m_rank.c_str( ) );

			list_of_names.push_back( buffer );
		}

		int screen_w, screen_h;
		g_gmod.m_engine( )->GetScreenSize( screen_w, screen_h );

		if( g_settings.misc.watermark ) {
			cur_pos = 20;
		}

		for( const auto& it : list_of_names ) {
			draw_string( screen_w - 3, cur_pos, ALIGN_RIGHT, true, clr_t( 255, 255, 255 ), it );
			cur_pos += 10;
		}
	}


	void c_visuals::draw_players( ) {
		const auto local_id = g_gmod.m_engine( )->GetLocalPlayer( );
		const auto local_team_color = m_data.m_player.at( local_id ).m_team_color;

		for( int i{ 1 }; i <= g_gmod.m_globals->m_maxclients; ++i ) {
			if( i == local_id )
				continue;

			auto ent = g_gmod.m_entlist( )->GetClientEntity< >( i );
			if( !ent || !ent->is_valid( ) )
				continue;

			auto& data = m_data.m_player.at( i );

			if( g_settings.visuals.ignore_team &&
				g_ctx.m_local->m_iTeamNum( ) == ent->m_iTeamNum( ) )
				continue;

			if( g_settings.visuals.ignore_teamcolor &&
				local_team_color == data.m_team_color )
				continue;

			//clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ?
			//	g_settings.visuals.box_friendly : g_settings.visuals.box_enemy;

			clr_t col = g_settings.visuals.box_enemy;

			auto box = get_box( ent, data.m_coordinate_frame );
			int	health = ent->m_iHealth( );
			int	bottom_pos = 0;

			if( g_settings.visuals.skeleton( ) ) {
				//clr_t col = ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) ? g_settings.visuals.skeleton_friendly : g_settings.visuals.skeleton_enemy;
				clr_t col = g_settings.visuals.skeleton_enemy;

				auto hdr = g_gmod.m_model_info( )->GetStudiomodel( ent->ce( )->GetModel( ) );
				if( hdr ) {	
					for( size_t bone{ }; bone < hdr->numbones; ++bone ) {
						auto b = hdr->get_bone( bone );
						if( b && b->flags & 0x100 && b->parent != -1 ) {
							vec3_t child = vec3_t{ data.m_matrix[ bone ][ 0 ][ 3 ], data.m_matrix[ bone ][ 1 ][ 3 ], data.m_matrix[ bone ][ 2 ][ 3 ] };
							vec3_t parent = vec3_t{ data.m_matrix[ b->parent ][ 0 ][ 3 ], data.m_matrix[ b->parent ][ 1 ][ 3 ], data.m_matrix[ b->parent ][ 2 ][ 3 ] };

							auto child_screen = util::screen_transform( child );
							auto parent_screen = util::screen_transform( parent );

							draw_line( child_screen, parent_screen, col );
						}
					}
				}
			}

			if( g_settings.visuals.box( ) ) {
				draw_rect( box.x + 1, box.y + 1, box.w - 2, box.h - 2, clr_t( 0, 0, 0, 180 ) );
				draw_rect( box.x, box.y, box.w, box.h, col );
			}

			if( g_settings.visuals.health( ) ) {
				auto fill = box.h - 1;
				fill *= std::clamp( health, 0, 100 ) * 0.01f;
				
				auto hp_col = clr_t(
					std::min< int >( 510 * ( 100 - health ) / 100, 255 ),
					std::min< int >( 510 * health / 100, 255 ),
					0,
					255 );

				draw_filled_rect( box.x - 4, box.y, 3, box.h + 1, clr_t( 0, 0, 0, 170 ) );
				draw_filled_rect( box.x - 3, box.y + box.h - fill, 1, fill, hp_col );

				if( health != 100 )
					draw_string( box.x - 2, box.y + 1 + box.h - fill - 3, ALIGN_CENTER, false, clr_t( 255, 255, 255, 255 ), "%d", health );
			}

			if( g_settings.visuals.name( ) ) {
				draw_string( box.x + box.w / 2, box.y - 12, ALIGN_CENTER, true,
					 clr_t( 255, 255, 255, 255 ), ent->get_info( ).name );
			}

			if( g_settings.visuals.weapon( ) ) {
				auto weapon = ent->get_weapon( );
				if( weapon ) {
					std::string wep_str = weapon->get_print_name( );
					if( !wep_str.empty( ) ) {
						std::transform( wep_str.begin( ), wep_str.end( ), wep_str.begin( ), ::tolower ); // lowercase gang

						draw_string( box.x + box.w / 2, box.y + box.h + 3 + bottom_pos, ALIGN_CENTER, true,
							clr_t( 255, 255, 255, 255 ), wep_str.c_str( ) ); // font is a little ugly

						bottom_pos += 11;
					}
				}
			}

			if( g_settings.visuals.rank( ) ) {
				auto rank = data.m_rank;
				if( !rank.empty( ) ) {
					draw_string( box.x + box.w / 2, box.y + box.h + 3 + bottom_pos, ALIGN_CENTER, true,
						clr_t( 255, 255, 255, 255 ), rank.c_str( ) );

					bottom_pos += 11;
				}
			}

			//if( g_settings.visuals.ammo ) { m_iClip1 just returns the max clip??? doesn't work.
			//	if( auto weapon = ent->get_weapon( ) ) { // hacker codes 8)				
			//		draw_string( box.x + box.w / 2, box.y + box.h + 3 + bottom_pos, ALIGN_CENTER, true,
			//			clr_t( 255, 255, 255, 255 ), "%d / %d", weapon->m_iClip1( ), m_stored_data.at( i ).m_max_clip1 );
			//	}
			//}
		}
	}

	void c_visuals::operator()( ) {
		g_ctx.m_lua = g_gmod.m_lua_shared( )->GetLuaInterface( LUA_CLIENT );
		if( !g_ctx.m_lua )
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
		case 3:
		{
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

		if( g_ctx.run_frame( ) ) {
			spectator_list( );
			g_cheat.m_playerlist( );
			if( g_settings.visuals.active ) {
				draw_players( );
			}
		}
	}

	VMatrix& c_visuals::get_matrix( ) {
		return m_data.m_matrix;
	}
}