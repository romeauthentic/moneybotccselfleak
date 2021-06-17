#include <algorithm>

#include "visuals.hpp"
#include "ctx.hpp"
#include "base_cheat.h"
#include "renderer.hpp"
#include "input_system.hpp"
#include "math.h"
#include "settings.h"
#include "interfaces.h"
namespace features
{
	void c_visuals::draw_local( ) {
		int screen_w, screen_h;
		cl.m_engine( )->GetScreenSize( screen_w, screen_h );

		int cur_pos{ };


		if( g_settings.misc.no_scope( ) &&
			g_ctx.m_local->m_bIsScoped( ) ) {
			int w, h;
			cl.m_engine( )->GetScreenSize( w, h );

			draw_line( 0, h / 2, w, h / 2, clr_t( 0, 0, 0 ) );
			draw_line( w / 2, 0, w / 2, h, clr_t( 0, 0, 0 ) );
		}
		spectator_list( );
		//throwable_prediction( ); -- sometime when i can be fucked
	}

	void c_visuals::spectator_list( ) {
		if( !g_settings.visuals.spec_list )
			return;

		std::vector< std::string > spec_list;

		for( size_t i{ }; i < 65; ++i ) {
			auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );
			if( ent && ent->is_player( ) && !ent->ce( )->is_dormant( ) ) {
				auto spec_handle = ent->m_hObserverTarget( );
				auto spec_ent = cl.m_entlist( )->get_entity_from_handle< c_base_player >( spec_handle );

				if( spec_ent == g_ctx.m_local ) {
					char player_name[ 32 ];
					ent->get_name_safe( player_name );
					spec_list.push_back( player_name );
				}
			}
		}

		int screen_w, screen_h;
		cl.m_engine( )->GetScreenSize( screen_w, screen_h );

		int cur_pos{ };

		if( g_settings.misc.watermark ) {
			cur_pos = 20;
		}

		for( auto& it : spec_list ) {
			draw_string( screen_w - 3, cur_pos, ALIGN_RIGHT, true, clr_t( 255, 255, 255 ), it.c_str( ) );
			cur_pos += 10;
		}
	}
	//i wanna do this
	/*
	void c_visuals::throwable_prediction( ) {
		static auto sv_gravity = cl.m_cvar( )->FindVar( xors( "sv_gravity" ) );

		if( !g_settings.visuals.grenade_prediction )
			return;

		
		/*
		auto get_detonate_time = [ ]( int defindex ) {
			switch( defindex ) {
			case WEAPON_FLASHBANG:
			case WEAPON_HEGRENADE:
				return 1.5f;
			case WEAPON_INCGRENADE:
			case WEAPON_MOLOTOV:
				return molotov_detonate_time->get_float( );
			case WEAPON_DECOY:
				return 5.f;
			default: return 3.f;
			}
		};

		auto draw_3d_line = [ this ]( const vec3_t& start, const vec3_t& end, clr_t col, bool circle = false ) {
			vec2_t start_w2s = util::screen_transform( start );
			vec2_t end_w2s = util::screen_transform( end );

			draw_line( start_w2s, end_w2s, col );
			if( circle )
				draw_rect( end_w2s.x - 1, end_w2s.y - 1, 2, 2, clr_t( 230, 230, 230 ) );
		};

		auto clip_velocity = [ ]( const vec3_t& in, const vec3_t& normal, vec3_t& out, float overbounce ) {
			int blocked = 0;
			float angle = normal[ 2 ];

			if( angle > 0.f )
				blocked |= 1;

			if( !angle )
				blocked |= 2;

			float backoff = in.dot( normal ) * overbounce;

			for( int i{ }; i < 3; ++i ) {
				out[ i ] = in[ i ] - ( normal[ i ] * backoff );

				if( out[ i ] > -0.1f && out[ i ] < 0.1f ) {
					out[ i ] = 0.f;
				}
			}

			return blocked;
		};

		auto weapon = g_ctx.m_local->get_active_weapon( );
		if( !weapon ) return;

		int def_index = weapon->m_iItemDefinitionIndex( );
		if( weapon->has_trajectory( ) )
			return;

		auto wpn_info = weapon->get_wpn_info( );

		vec3_t throw_ang, forward;
		cl.m_engine( )->GetViewAngles( throw_ang );
		throw_ang.x -= ( 90.f - abs( throw_ang.x ) ) * 0.11111111f;
		throw_ang.x = std::remainderf( throw_ang.x, 360.f );

		forward = math::angle_vectors( throw_ang );

		float throw_strength = weapon->m_flThrowStrength( );
		float throw_velocity = std::min( std::max( wpn_info->throw_velocity * 0.9f, 15.f ), 750.f );

		float throw_height = ( throw_strength * 12.f ) - 12.f;
		float v68 = throw_velocity * ( ( 0.7f * throw_strength ) + 0.3f );

		vec3_t start_pos = g_ctx.m_local->get_eye_pos( ) + vec3_t( 0, 0, throw_height );
		vec3_t end_pos = start_pos + ( forward * 22.f );

		CTraceFilter filter;
		filter.pSkip = g_ctx.m_local;

		CGameTrace trace;
		Ray_t ray;
		ray.Init( start_pos, end_pos, vec3_t( -2.f, -2.f, -2.f ), vec3_t( 2.f, 2.f, 2.f ) );

		cl.m_trace( )->trace_ray( ray, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_MOVEABLE | CONTENTS_CURRENT_90, &filter, &trace );
		end_pos = trace.endpos - forward * 6.f;
		vec3_t throw_pos = g_ctx.m_local->m_vecVelocity( ) * 1.25f + forward * v68;

		//draw_3d_line( start_pos, end_pos, clr_t( 255, 255, 255 ) );
		float gravity = sv_gravity->get_float( ) * 0.4f;

		player_info_t info{ };

		for( int ticks = TIME_TO_TICKS( get_detonate_time( def_index ) ); ticks >= 0; --ticks ) {
			auto throw_dir = vec3_t( throw_pos.x, throw_pos.y, ( throw_pos.z + ( throw_pos.z - ( gravity * TICK_INTERVAL( ) ) ) ) * 0.5f );
			auto temp = throw_dir * TICK_INTERVAL( );
			throw_pos.z -= gravity * TICK_INTERVAL( );

			vec3_t src = end_pos, end = end_pos + temp;
			Ray_t ray;
			ray.Init( src, end, vec3_t( -2.f, -2.f, -2.f ), vec3_t( 2.f, 2.f, 2.f ) );

			cl.m_trace( )->trace_ray( ray, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_CURRENT_90, &filter, &trace );
			if( trace.allsolid )
				throw_pos = vec3_t( );

			end_pos = trace.endpos;
			draw_3d_line( src, end_pos, clr_t( 66, 143, 244 ) );

			if( trace.fraction != 1.f ) {
				float surf_elasticity = 1.f;
				vec3_t throw_pos2{ };
				clip_velocity( throw_pos, trace.plane.normal, throw_pos2, 2.f );

				if( trace.m_pEnt && cl.m_engine( )->GetPlayerInfo( trace.m_pEnt->index( ), &info ) ) {
					surf_elasticity = 0.3f;
				}

				throw_pos2 *= std::clamp( surf_elasticity * 0.45f, 0.f, 0.9f );
				end = end_pos + throw_pos2 * ( ( 1.f - trace.fraction ) * TICK_INTERVAL( ) );

				if( def_index == WEAPON_MOLOTOV || def_index == WEAPON_INCGRENADE ) {
					if( trace.plane.normal.z >= cos( DEG2RAD( molotov_detonate_slope->get_float( ) ) ) ) {
						return;
					}
				}

				ray.Init( end_pos, end, vec3_t( -2.f, -2.f, -2.f ), vec3_t( 2.f, 2.f, 2.f ) );
				cl.m_trace( )->trace_ray( ray, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_CURRENT_90, &filter, &trace );

				draw_3d_line( end_pos, end, clr_t( 66, 143, 244 ), true );
				end_pos = trace.endpos;
				throw_pos = throw_pos2;
			}
		}
	}*/

}