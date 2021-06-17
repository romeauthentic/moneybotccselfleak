#include <algorithm>

#include "visual.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "renderer.hpp"
#include "input_system.hpp"
#include "math.hpp"

namespace features
{
	void c_visuals::draw_local( ) {
		int screen_w, screen_h;
		g_csgo.m_engine( )->GetScreenSize( screen_w, screen_h );

		int cur_pos{ };

		draw_shots( );
		grenade_prediction( );
		draw_spread( );
		spectator_list( );

		if( g_ctx.m_local->is_valid( ) ) {
			//isvalveds

			if( g_settings.misc.team_dmg( ) && *( bool* )( c_base_player::get_game_rules( ) + 0x75 ) ) {
				const float kick = 300.f;
				float percentage = m_teamdmg / kick;

				clr_t col = clr_t::blend( esp_green( ), esp_red( ), std::clamp( percentage, 0.f, 1.f ) );

				draw_really_big_string( 10, cur_pos += 15, col, xors( "DMG: %d" ), m_teamdmg );
			}

			static float incoming_latency;
			if( g_settings.misc.net_fakelag ) {
				float desired_latency = incoming_latency + g_settings.misc.net_fakeping_amount * 0.001f;
				if( g_settings.misc.net_fakelag == 4 )
					desired_latency = 1.0f;

				auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
				if( nci ) {
					float cur_latency = nci->GetLatency( 1 );
					float percentage = cur_latency / desired_latency;
					percentage = std::clamp( percentage + 0.1f, 0.f, 1.f );

					clr_t col = clr_t::blend( esp_red( ), esp_green( ), percentage );

					bool visible = g_settings.misc.net_fakeping_active;
					if( g_settings.misc.net_fakelag == 4 )
						visible = true;

					if( percentage > 0.35f || visible ) {
						draw_really_big_string( 10, cur_pos += 15, col, xors( "PING" ) );
					}
				}
			}
			else {
				auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
				if( nci ) {
					incoming_latency = nci->GetLatency( 1 );
				}
			}

			if( g_settings.rage.anti_aim( ) ) {
				static float last_lby = 0.f;
				static float last_update = 0.f;
				float lby = g_ctx.m_local->m_flLowerBodyYawTarget( );
				if( lby != last_lby ) {
					last_update = g_csgo.m_globals->m_curtime;
					last_lby = lby;
				}

				float lby_delta = std::abs( std::remainderf( lby - g_ctx.m_thirdperson_angle.y, 360.f ) );
				float time_delta = g_csgo.m_globals->m_curtime - last_update;

				bool breaking = lby_delta > 35.f && time_delta > 0.2f;

				if( g_cheat.m_ragebot.m_antiaim->is_edging( ) ) {
					draw_really_big_string( 10, cur_pos += 15, esp_green( ), xors( "EDGE" ) );
				}
				
				if( g_settings.rage.break_lby( ) ) {
					draw_really_big_string( 10, cur_pos += 15, breaking ? esp_green( ) : esp_red( ), xors( "LBY" ) );

					auto animstate = g_ctx.m_local->get_animstate( );
					if( animstate && animstate->m_velocity < 0.1f && animstate->m_bOnGround ) {
						draw_filled_rect( 11, ( cur_pos += 4 ) + 14, 31, 2, clr_t( 0, 0, 0, 170 ) );

						float progress = ( float )g_cheat.m_ragebot.m_antiaim->get_next_update( ) / TIME_TO_TICKS( 1.1f );
						
						if( progress <= 1.f )
							draw_filled_rect( 11, cur_pos + 14, 30 * progress, 1, esp_green( ) );
					}
				}

				if( g_settings.rage.preserve_fps && g_settings.rage.enabled && util::is_low_fps( ) ) {
					draw_really_big_string( 10, cur_pos += 15, esp_red( ), xors( "FPS" ) );
				}

				if( g_settings.rage.draw_angles ) {
					auto get_rotated_point = [ ]( vec2_t point, float rotation, float distance ) {
						float rad = DEG2RAD( rotation );

						point.x += sin( rad ) * distance;
						point.y += cos( rad ) * distance;

						return point;
					};

					auto draw_rotated_triangle = [ &get_rotated_point, this ]( vec2_t point, float rotation, clr_t col ) {
						vec2_t rotated_pos_1 = get_rotated_point( point, rotation + 205.f, 30.f );
						vec2_t rotated_pos_2 = get_rotated_point( point, rotation + 155.f, 30.f );

						vertex_t v[ ] = {
							{ point },
						{ rotated_pos_1 },
						{ rotated_pos_2 }
						};
						
						if( !g_settings.misc.hide_from_obs )
							g_renderer.draw_polygon( 3, v, col );
						else {
							draw_line( point, rotated_pos_1, col );
							draw_line( point, rotated_pos_2, col );
							draw_line( rotated_pos_1, rotated_pos_2, col );
						}
					};

					vec2_t rotated_lby{ screen_w * 0.5f, screen_h * 0.5f };
					vec2_t rotated_real{ screen_w * 0.5f, screen_h * 0.5f };

					vec3_t angles{ };
					g_csgo.m_engine( )->GetViewAngles( angles );

					float rotation_lby = std::remainderf( g_ctx.m_local->m_flLowerBodyYawTarget( ) - angles.y, 360.f ) - 180.f;
					float rotation_real = std::remainderf( g_ctx.m_thirdperson_angle.y - angles.y, 360.f ) - 180.f;

					rotated_lby = get_rotated_point( rotated_lby, rotation_lby, 120.f );
					rotated_real = get_rotated_point( rotated_real, rotation_real, 120.f );

					draw_rotated_triangle( rotated_lby, rotation_lby, g_settings.rage.lby_color );
					draw_rotated_triangle( rotated_real, rotation_real, g_settings.rage.real_color );
				}
			}

			if( g_settings.rage.fakelag.mode( ) &&
				g_settings.rage.fakelag.ticks( ) &&
				g_settings.rage.fakelag.in_air( ) ) {
				int min_dist = 64;
				int max_ticks = g_settings.rage.fakelag.ticks;
				if( g_settings.rage.fakelag.mode == 2 ) {
					max_ticks += max_ticks * g_settings.rage.fakelag.fluctuate * 0.01f;
					max_ticks = math::min( max_ticks, 15 );
				}

				float speed = g_ctx.m_local->m_vecVelocity( ).length2d( );
				if( speed > 255.f && speed * g_csgo.m_globals->m_interval_per_tick * max_ticks > 50 ) {
					bool breaking = g_cheat.m_lagmgr.is_breaking_lc( );
					draw_really_big_string( 10, cur_pos += 15, breaking ? esp_green( ) : esp_red( ), xors( "LC" ) );
				}
			}
		}

		if( g_settings.misc.no_scope( ) &&
			g_ctx.m_local->m_bIsScoped( ) ) {
			int w, h;
			g_csgo.m_engine( )->GetScreenSize( w, h );

			draw_line( 0, h / 2, w, h / 2, clr_t( 0, 0, 0 ) );
			draw_line( w / 2, 0, w / 2, h, clr_t( 0, 0, 0 ) );
		}

		draw_autowall( );
	}

	void c_visuals::spectator_list( ) {
		if( !g_settings.visuals.spec_list )
			return;

		std::vector< std::string > spec_list;

		for( size_t i{ }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );
			if( ent && ent->is_player( ) && !ent->ce( )->IsDormant( ) ) {
				auto spec_handle = ent->m_hObserverTarget( );
				auto spec_ent = g_csgo.m_entlist( )->GetClientEntityFromHandle< >( spec_handle );

				if( spec_ent == g_ctx.m_local ) {
					char player_name[ 32 ];
					ent->get_name_safe( player_name );
					spec_list.push_back( player_name );
				}
			}
		}

		int screen_w, screen_h;
		g_csgo.m_engine( )->GetScreenSize( screen_w, screen_h );

		int cur_pos{ };

		if( g_settings.misc.watermark ) {
			cur_pos = 20;
		}

		for( auto& it : spec_list ) {
			draw_string( screen_w - 3, cur_pos, ALIGN_RIGHT, true, clr_t( 255, 255, 255 ), it.c_str( ) );
			cur_pos += 10;
		}
	}

	void c_visuals::grenade_prediction( ) {
		static auto molotov_detonate_time = g_csgo.m_cvar( )->FindVar( xors( "molotov_throw_detonate_time" ) );
		static auto molotov_detonate_slope = g_csgo.m_cvar( )->FindVar( xors( "weapon_molotov_maxdetonateslope" ) );
		static auto sv_gravity = g_csgo.m_cvar( )->FindVar( xors( "sv_gravity" ) );

		if( !g_settings.visuals.grenade_prediction )
			return;

		auto is_grenade = [ ]( int defindex ) {
			switch( defindex ) {
			case WEAPON_FLASHBANG:
			case WEAPON_HEGRENADE:
			case WEAPON_SMOKEGRENADE:
			case WEAPON_MOLOTOV:
			case WEAPON_INCGRENADE:
			case WEAPON_DECOY:
				return true;
			default: return false;
			}
		};

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

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon ) return;

		int def_index = weapon->m_iItemDefinitionIndex( );
		if( !is_grenade( def_index ) || !weapon->m_bPinPulled( ) )
			return;

		auto wpn_info = weapon->get_wpn_info( );

		vec3_t throw_ang, forward;
		g_csgo.m_engine( )->GetViewAngles( throw_ang );
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

		g_csgo.m_trace( )->TraceRay( ray, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_MOVEABLE | CONTENTS_CURRENT_90, &filter, &trace );
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

			g_csgo.m_trace( )->TraceRay( ray, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_CURRENT_90, &filter, &trace );
			if( trace.allsolid )
				throw_pos = vec3_t( );

			end_pos = trace.endpos;
			draw_3d_line( src, end_pos, clr_t( 66, 143, 244 ) );

			if( trace.fraction != 1.f ) {
				float surf_elasticity = 1.f;
				vec3_t throw_pos2{ };
				clip_velocity( throw_pos, trace.plane.normal, throw_pos2, 2.f );

				if( trace.m_pEnt && g_csgo.m_engine( )->GetPlayerInfo( trace.m_pEnt->GetIndex( ), &info ) ) {
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
				g_csgo.m_trace( )->TraceRay( ray, CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_CURRENT_90, &filter, &trace );

				draw_3d_line( end_pos, end, clr_t( 66, 143, 244 ), true );
				end_pos = trace.endpos;
				throw_pos = throw_pos2;
			}
		}
	}

	void c_visuals::store_shot( vec3_t pos, bool hit ) {
		if( !g_settings.visuals.target )
			return;

		m_shots.push_back( { g_csgo.m_globals->m_curtime, pos, hit } );
	}

	void c_visuals::draw_shots( ) {
		if( !g_settings.visuals.target )
			return;

		for( size_t i{ }; i < m_shots.size( ) && !m_shots.empty( ); ++i ) {
			if( m_shots[ i ].time + g_settings.visuals.target_time < g_csgo.m_globals->m_curtime ) {
				m_shots.erase( m_shots.begin( ) + i );
			}
		}

		for( auto& it : m_shots ) {
			vec3_t min = it.pos - vec3_t( 3.5f, 3.5f, 3.5f );
			vec3_t max = it.pos + vec3_t( 3.5f, 3.5f, 3.5f );

			std::array< vec3_t, 8 > points = {
				vec3_t{ min.x, min.y, min.z },
				vec3_t{ min.x, max.y, min.z },
				vec3_t{ max.x, max.y, min.z },
				vec3_t{ max.x, min.y, min.z },
				vec3_t{ max.x, max.y, max.z },
				vec3_t{ min.x, max.y, max.z },
				vec3_t{ min.x, min.y, max.z },
				vec3_t{ max.x, min.y, max.z }
			};

			vec2_t
				flb = util::screen_transform( points[ 3 ] ),
				blb = util::screen_transform( points[ 0 ] ),
				frb = util::screen_transform( points[ 2 ] ),
				blt = util::screen_transform( points[ 6 ] ),
				brt = util::screen_transform( points[ 5 ] ),
				frt = util::screen_transform( points[ 4 ] ),
				brb = util::screen_transform( points[ 1 ] ),
				flt = util::screen_transform( points[ 7 ] );

			vec2_t lines[ 12 ][ 2 ];

			lines[ 0 ][ 0 ] = { flb.x, flb.y };
			lines[ 0 ][ 1 ] = { frb.x, frb.y };
			lines[ 1 ][ 0 ] = { frb.x, frb.y };
			lines[ 1 ][ 1 ] = { brb.x, brb.y };
			lines[ 2 ][ 0 ] = { brb.x, brb.y };
			lines[ 2 ][ 1 ] = { blb.x, blb.y };
			lines[ 3 ][ 0 ] = { blb.x, blb.y };
			lines[ 3 ][ 1 ] = { flb.x, flb.y };

			// top
			lines[ 4 ][ 0 ] = { flt.x, flt.y };
			lines[ 4 ][ 1 ] = { frt.x, frt.y };
			lines[ 5 ][ 0 ] = { frt.x, frt.y };
			lines[ 5 ][ 1 ] = { brt.x, brt.y };
			lines[ 6 ][ 0 ] = { brt.x, brt.y };
			lines[ 6 ][ 1 ] = { blt.x, blt.y };
			lines[ 7 ][ 0 ] = { blt.x, blt.y };
			lines[ 7 ][ 1 ] = { flt.x, flt.y };

			//bottom to top
			lines[ 8 ][ 0 ] = { flb.x, flb.y };
			lines[ 8 ][ 1 ] = { flt.x, flt.y };
			lines[ 9 ][ 0 ] = { frb.x, frb.y };
			lines[ 9 ][ 1 ] = { frt.x, frt.y };
			lines[ 10 ][ 0 ] = { brb.x, brb.y };
			lines[ 10 ][ 1 ] = { brt.x, brt.y };
			lines[ 11 ][ 0 ] = { blb.x, blb.y };
			lines[ 11 ][ 1 ] = { blt.x, blt.y };

			for( size_t i{ }; i < 12; ++i ) {
				draw_line( lines[ i ][ 0 ], lines[ i ][ 1 ], it.hit ? esp_blue( 160 ) : esp_red( 160 ) );
			}
		}
	}

	void c_visuals::draw_autowall( ) {
		if (!g_settings.visuals.autowall_crosshair)
			return;

		auto weapon = g_ctx.m_local->get_weapon();
		if (!weapon || weapon->is_knife() || weapon->is_grenade())
			return;

		int screen_w, screen_h;
		g_csgo.m_engine()->GetScreenSize(screen_w, screen_h);

		auto wpn_data = weapon->get_wpn_info();

		vec3_t angles{ };
		g_csgo.m_engine()->GetViewAngles(angles);

		fire_bullet_data_t data{ };
		data.src = g_ctx.m_local->get_eye_pos();
		vec3_t end = data.src;
		end += math::angle_vectors(angles) * wpn_data->range;

		data.filter.pSkip = g_ctx.m_local;
		data.current_damage = wpn_data->damage;
		data.penetrate_count = 1;
		data.trace_length = 0.f;

		//data.length_to_end = ( end - data.src ).length( );
		data.trace_length_remaining = wpn_data->range - data.trace_length;
		data.direction = math::angle_vectors(angles);

		data.direction.normalize_vector();

		CTraceFilter filter;
		Ray_t		 ray;

		filter.pSkip = g_ctx.m_local;
		ray.Init(data.src, end);
		g_csgo.m_trace()->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &data.enter_trace);

		bool can_penetrate = g_cheat.m_autowall.handle_bullet_penetration(wpn_data, data);

		g_renderer.draw_box(screen_w / 2 - 1, screen_h / 2, 1, 1,
			can_penetrate ? clr_t(0, 220, 0) : clr_t(220, 0, 0));
	}

	void c_visuals::draw_spread( ) {
		if( !g_settings.visuals.draw_spread )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( weapon ) {
			int screen_w, screen_h;
			g_csgo.m_engine( )->GetScreenSize( screen_w, screen_h );
			int cross_x = screen_w / 2, cross_y = screen_h / 2;

			float recoil_step = screen_h / g_ctx.m_fov;

			cross_x -= ( int )( g_ctx.m_local->m_aimPunchAngle( ).y * recoil_step );
			cross_y += ( int )( g_ctx.m_local->m_aimPunchAngle( ).x * recoil_step );

			weapon->update_accuracy_penalty( );
			float inaccuracy = weapon->get_inaccuracy( );
			float spread = weapon->get_spread( );

			float cone = inaccuracy * spread;
			cone *= screen_h * 0.7f;
			cone *= 90.f / g_ctx.m_fov;

			for( int seed{ }; seed < 256; ++seed ) {
				util::set_random_seed( math::random_number( 0, 255 ) + 1 );
				float rand_a	= util::get_random_float( 0.f, 1.0f );
				float pi_rand_a = util::get_random_float( 0.f, 2.0f * M_PI );
				float rand_b	= util::get_random_float( 0.0f, 1.0f );
				float pi_rand_b = util::get_random_float( 0.f, 2.f * M_PI );

				float spread_x = cos( pi_rand_a ) * ( rand_a * inaccuracy ) + cos( pi_rand_b ) * ( rand_b * spread );
				float spread_y = sin( pi_rand_a ) * ( rand_a * inaccuracy ) + sin( pi_rand_b ) * ( rand_b * spread );

				float max_x = cos( pi_rand_a ) * cone + cos( pi_rand_b ) * cone;
				float max_y = sin( pi_rand_a ) * cone + sin( pi_rand_b ) * cone;

				float step = screen_h / g_ctx.m_fov * 90.f;
				int screen_spread_x = ( int )( spread_x * step * 0.7f );
				int screen_spread_y = ( int )( spread_y * step * 0.7f );

				float percentage = ( rand_a * inaccuracy + rand_b * spread ) / ( inaccuracy + spread );

				draw_filled_rect( cross_x + screen_spread_x, cross_y + screen_spread_y, 1, 1,
					clr_t( 255, 255, 255, 255 * ( 0.4f + percentage * 0.6f ) ) * ( 0.1f + percentage * 0.9f ) );
			}
		}
	}


	void c_visuals::store_tracer( int index, vec3_t pos ) {
		static float last_time[ 65 ]{ };
		tracer_t new_tracer;

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( index );
		if( ent && ent->is_valid( ) ) {
			bool valid = ent == g_ctx.m_local;
			float time = g_csgo.m_globals->m_curtime;
			auto ent_pos = ent->get_hitbox_pos( 0 );

			if( ent != g_ctx.m_local && g_ctx.m_local->is_valid( ) &&
				( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || g_settings.visuals.friendlies( ) ) ) {
				vec3_t ang = math::vector_angles( ent_pos, pos );
				vec3_t local_pos = g_ctx.m_local->get_hitbox_pos( 0 );
				float dist = local_pos.dist_to( ent_pos );

				vec3_t local_ang = math::vector_angles( ent_pos, local_pos );

				constexpr float max_delta_sqr = 30.f * 30.f;

				float pitch_delta = sin( DEG2RAD( std::abs( ( ang - local_ang ).clamp( ).x ) ) ) * dist;
				float yaw_delta = sin( DEG2RAD( std::abs( ( ang - local_ang ).clamp( ).y ) ) ) * dist;
				float delta_sqr = ( yaw_delta * yaw_delta + pitch_delta * pitch_delta );


				if( delta_sqr > max_delta_sqr ) return;

				vec3_t vec = math::angle_vectors( ang );
				vec *= dist;
				vec += ent_pos;

				pos = vec;

				valid = true;
			}

			if( valid ) {
				float delta = std::abs( g_csgo.m_globals->m_curtime - last_time[ index ] );
				if( delta > g_csgo.m_globals->m_interval_per_tick ) {
					new_tracer.m_time = time;
					new_tracer.m_start = ent_pos;
					new_tracer.m_end = pos;
					new_tracer.m_local = ent == g_ctx.m_local;
					m_tracers.emplace_back( new_tracer );

					last_time[ index ] = g_csgo.m_globals->m_curtime;
				}
			}
		}
	}

	void c_visuals::draw_tracers( ) {
		if( !g_settings.visuals.active ) return;
		float time = g_csgo.m_globals->m_curtime;

		if( m_tracers.empty( ) )
			return;

		for( size_t i{ }; i < m_tracers.size( ) && !m_tracers.empty( ); ++i ) {
			auto& tr = m_tracers[ i ];

			float delta = time - tr.m_time;
			if( delta > 1.0f || std::abs( delta ) > 5.f ) m_tracers.erase( m_tracers.begin( ) + i );
		}

		if( !m_tracers.empty( ) ) {
			for( auto& it : m_tracers ) {

				float delta = time - it.m_time;
				clr_t col = it.m_local ? clr_t::from_hsb( delta, 1.0f, 1.0f ) : clr_t( 255, 15, 46 );
				col.a( ) = 1.0f - delta * 255;
				auto w2s_begin = util::screen_transform( it.m_start );
				auto w2s_end = util::screen_transform( it.m_end );


				switch( g_settings.visuals.bullet_tracers( ) ) {
				case 1:
					draw_line( w2s_begin, w2s_end, col );
					break;
				case 2:
					if( !g_ctx.precache_model( xors( "materials/sprites/laserbeam.vmt" ) ) ) {
						g_con->log( "nigga cant get" );
						break;
					}

					BeamInfo_t beam_info;

					beam_info.m_nType = beam_normal;
					beam_info.m_pszModelName = xors( "materials/sprites/laserbeam.vmt" );
					beam_info.m_nModelIndex = g_csgo.m_model_info( )->GetModelIndex( xors( "materials/sprites/laserbeam.vmt" ) );
					beam_info.m_flHaloScale = 0.0f;
					beam_info.m_flLife = 0.09f; //0.09
					beam_info.m_flWidth = .6f;
					beam_info.m_flEndWidth = .75f;
					beam_info.m_flFadeLength = 3.0f;
					beam_info.m_flAmplitude = 0.f;
					beam_info.m_flBrightness = ( col.a( ) - 255.f ) * 0.8f;
					beam_info.m_flSpeed = 1.f;
					beam_info.m_nStartFrame = 1;
					beam_info.m_flFrameRate = 60;
					beam_info.m_flRed = col.r( );
					beam_info.m_flGreen = col.g( );
					beam_info.m_flBlue = col.b( );
					beam_info.m_nSegments = 4;
					beam_info.m_bRenderable = true;
					beam_info.m_nFlags = 0;

					beam_info.m_vecStart = it.m_start;
					beam_info.m_vecEnd = it.m_end;

					Beam_t* beam = g_csgo.m_beams( )->CreateBeamPoints( beam_info );

					if( beam ) {
						g_csgo.m_beams( )->DrawBeam( beam );
					}

					break;
				}
			}
		}
	}
}