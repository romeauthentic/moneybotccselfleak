#include "interface.hpp"
#include "input_system.hpp"
#include "context.hpp"
#include "legitbot.hpp"
#include "settings.hpp"
#include "math.hpp"

namespace features
{
	int c_legitbot::get_bone( c_base_player* ent, c_legitbot::lag_record_t* record ) {
		auto s = g_settings.legit.active;

		auto should_scan = [ & ] ( int bone ) -> bool {
			switch ( bone ) {
				case HITBOX_HEAD:
					return s->h_head;
				case HITBOX_BODY:
				case HITBOX_THORAX:
				case HITBOX_CHEST:
				case HITBOX_PELVIS:
				case HITBOX_UPPER_CHEST:
					return s->h_body;
				case HITBOX_LEFT_CALF:
				case HITBOX_LEFT_THIGH:
				case HITBOX_RIGHT_CALF:
				case HITBOX_RIGHT_THIGH:
					return s->h_legs;
				case HITBOX_LEFT_UPPER_ARM:
				case HITBOX_RIGHT_UPPER_ARM:
				case HITBOX_LEFT_FOREARM:
				case HITBOX_RIGHT_FOREARM:
					return s->h_arms;
			}

			return false;
		};

		int hitboxes[] = {
			HITBOX_HEAD,
			HITBOX_CHEST,
			HITBOX_BODY,
			HITBOX_PELVIS,
			HITBOX_UPPER_CHEST,
			HITBOX_LEFT_UPPER_ARM,
			HITBOX_RIGHT_UPPER_ARM,
			HITBOX_LEFT_THIGH,
			HITBOX_RIGHT_THIGH,
			HITBOX_LEFT_FOREARM,
			HITBOX_RIGHT_FOREARM,
			HITBOX_LEFT_CALF,
			HITBOX_RIGHT_CALF
		};

		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );

		vec3_t min = ent->m_vecMins( );
		vec3_t max = ent->m_vecMaxs( );

		matrix3x4 backup[ 128 ];
		memcpy( backup, ent->m_CachedBoneData( ).GetElements( ),
			ent->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

		ent->m_vecMins( ) = record->m_mins;
		ent->m_vecMaxs( ) = record->m_maxs;

		memcpy( ent->m_CachedBoneData( ).GetElements( ), record->m_matrix,
			ent->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

		int best_hitbox = -1;

		for ( size_t i{ }; i < sizeof( hitboxes ); ++i ) {
			auto hitbox = hitboxes[ i ];

			if ( should_scan( hitbox ) ) {
				vec3_t pos = ent->get_hitbox_pos( hitbox );

				bool is_vis = util::trace_ray( local_pos, pos, g_ctx.m_local->ce( ), ent->ce( ) );

				if ( is_vis ) {
					best_hitbox = hitbox;
					break;
				}
			}
		}

		ent->m_vecMins( ) = min;
		ent->m_vecMaxs( ) = max;

		memcpy( ent->m_CachedBoneData( ).GetElements( ), backup,
			ent->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

		return best_hitbox;
	}

	c_legitbot::target_t c_legitbot::get_aim_target( float fov ) {
		float	 best_fov = fov ? fov : g_settings.legit.active->m_fov;
		target_t target{ -1, vec3_t( ) };
		vec3_t	 aim_angle{ };
		vec3_t	 cur_angle{ };
		vec3_t	 local_pos{ };
		vec3_t	 aim_pos{ };

		g_csgo.m_engine( )->GetViewAngles( cur_angle );
		local_pos = g_ctx.m_local->get_eye_pos( );

		for ( int i{ 1 }; i <= g_csgo.m_globals->m_maxclients; ++i ) {
			auto entity = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if ( !entity ) continue;
			if ( entity == g_ctx.m_local ) continue;
			if ( !entity->is_valid( ) ) continue;
			int team = entity->m_iTeamNum( );
			if ( team == g_ctx.m_local->m_iTeamNum( )
				&& !g_settings.legit.friendlies( ) ) {
				continue;
			}
			if ( entity->m_bGunGameImmunity( ) )
				continue;

			if ( !g_settings.legit.while_flashed && g_ctx.m_local->is_flashed( ) )
				continue;

			if ( !m_lagcomp.find_best_record( i ) )
				continue;

			int hitbox = get_bone( entity, m_lagcomp.find_best_record( i ) );

			if ( hitbox == -1 ) {
				continue;
			}

			aim_pos = entity->get_hitbox_pos( hitbox );

			if ( !g_settings.legit.through_smoke && util::line_goes_through_smoke( local_pos, aim_pos ) ) {
				continue;
			}

			aim_angle = math::vector_angles( local_pos, aim_pos );
			aim_angle.clamp( );

			float fov = ( cur_angle - aim_angle ).clamp( ).length2d( );
			if ( fov < best_fov ) {
				best_fov = fov;
				target.first = i;
				target.second = aim_pos;
			}
		}

		return target;
	}

	void c_legitbot::assist( c_base_player* target, float* x, float* y ) {
		vec3_t aim_ang;
		vec3_t move_ang;
		vec3_t view_ang;
		vec3_t enemy_pos;
		vec3_t local_pos;
		vec3_t view_delta;
		vec3_t delta;
		vec2_t pixels;

		auto& records = m_lagcomp.get_records( target->ce( )->GetIndex( ) );

		if ( !records.size( ) )
			return;

		int hitbox = get_bone( target, &records.at( 0 ) );
		if ( hitbox == -1 )
			return;

		local_pos = g_ctx.m_local->get_eye_pos( );
		enemy_pos = target->get_hitbox_pos( hitbox );

		aim_ang = math::vector_angles( local_pos, enemy_pos );
		aim_ang -= g_ctx.m_local->m_aimPunchAngle( ) * 2.f * g_settings.legit.active->m_rcs;

		move_ang = pixels_to_angle( vec2_t( *x, *y ) );
		g_csgo.m_engine( )->GetViewAngles( view_ang );

		view_delta = ( aim_ang - view_ang ).clamp( );

		move_ang *= g_settings.legit.active->m_assist_strength;
		float delta_y = std::abs( move_ang.y );
		float delta_x = std::abs( move_ang.x );

		delta.x = std::clamp( view_delta.x, -delta_x, delta_x );
		delta.y = std::clamp( view_delta.y, -delta_y, delta_y );

		pixels = angle_to_pixels( delta );
		*x += pixels.x; *y += pixels.y;
	}

	void c_legitbot::triggerbot( user_cmd_t* cmd ) {
		static float time_at = 0.f;

		if ( !g_settings.legit.triggerbot( ) )
			return;

		if ( !g_input.is_key_pressed( (VirtualKeys_t) g_settings.legit.trigger_key( ) ) && !time_at ) // sometimes people will let go of their triggerbot key too quickly and will cause them to not shoot.
			return;

		if ( !g_settings.legit.while_flashed && g_ctx.m_local->is_flashed( ) )
			return;

		vec3_t viewangles{ };
		vec3_t forward{ };
		g_csgo.m_engine( )->GetViewAngles( viewangles );

		auto wep = g_ctx.m_local->get_weapon( );
		if ( !wep ) return;
		if ( wep->is_knife( ) || wep->m_iClip1( ) == -1 ) return;

		auto wpn_info = wep->get_wpn_info( );
		if ( !wpn_info ) return;

		const float length = wpn_info->range;
		const float current_time = TICKS_TO_TIME( g_csgo.m_globals->m_tickcount );

		if ( time_at && abs( current_time - time_at ) > 0.3f ) {
			time_at = 0.f; // delta too big
		}

		if ( length > 1.0f ) {
			if ( !time_at ) {
				viewangles += g_ctx.m_local->m_aimPunchAngle( ) * 2.f;

				forward = math::angle_vectors( viewangles );
				forward *= length;

				vec3_t src = g_ctx.m_local->get_eye_pos( );
				vec3_t dst = src + forward;

				vec3_t aim_ang = math::vector_angles( src, dst ).clamp( );

				if ( !g_settings.legit.through_smoke && util::line_goes_through_smoke( src, dst ) ) {
					return;
				}

				if ( g_settings.legit.trigger_backtrack( ) ) {
					for ( int i{ 1 }; i <= g_csgo.m_globals->m_maxclients; ++i ) {
						auto entity = g_csgo.m_entlist( )->GetClientEntity< >( i );

						if ( !entity ) continue;
						if ( entity == g_ctx.m_local ) continue;
						if ( !entity->is_valid( ) ) continue;
						if ( entity->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.legit.friendlies( ) )
							continue;
						if ( entity->m_bGunGameImmunity( ) )
							continue;

						for ( auto& it : m_lagcomp.get_records( i ) ) {
							int delta = std::abs( g_csgo.m_globals->m_tickcount - it.m_tickcount );

							if ( !( g_settings.misc.net_fakelag( ) && g_settings.misc.net_fakeping_active ) ) {
								auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
								if ( nci ) {
									float max_latency = g_settings.legit.backtracking_time + nci->GetLatency( 0 );
									if ( delta > TIME_TO_TICKS( max_latency ) )
										continue;
								}
							}

							if ( !it.is_valid( ) ) continue;

							struct hitbox_data_t {
								hitbox_data_t( const vec3_t& min, const vec3_t& max, float radius ) {
									m_min = min;
									m_max = max;
									m_radius = radius;
								}

								vec3_t m_min{ };
								vec3_t m_max{ };
								float m_radius{ };
							};

							auto model = entity->ce( )->GetModel( );
							if ( !model ) continue;

							auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
							if ( !hdr ) continue;

							auto set = hdr->pHitboxSet( entity->m_nHitboxSet( ) );
							if ( !set ) continue;

							for ( size_t x{ }; x < set->numhitboxes; ++x ) {
								auto box = set->pHitbox( x );
								if ( !box || box->m_flRadius == -1.f ) continue;

								auto min = math::vector_transform( box->bbmin, it.m_matrix[ box->bone ] );
								auto max = math::vector_transform( box->bbmax, it.m_matrix[ box->bone ] );
								auto radius = box->m_flRadius;

								auto is_point_visible = [ & ] ( ) -> bool {
									CTraceFilter filter{ };
									CGameTrace	 tr{ };
									Ray_t		 ray{ };

									filter.pSkip = g_ctx.m_local;

									auto center = math::vector_transform( ( box->bbmax + box->bbmin ) * 0.5f, it.m_matrix[ box->bone ] );
									ray.Init( src, center );

									g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr ); 
									return tr.fraction > 0.98f || tr.m_pEnt == entity->ce( ) || !tr.m_pEnt;
								};

								if ( util::intersects_hitbox( src, dst, min, max, radius ) && is_point_visible( ) ) {
									time_at = current_time;
									m_lagcomp.backtrack_entity( i );
									break;
								}
							}
						}
					}
				}
				else {
					CTraceFilter filter{ };
					CGameTrace	 tr{ };
					Ray_t		 ray{ };

					filter.pSkip = g_ctx.m_local;
					ray.Init( src, dst );

					g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

					if ( tr.m_pEnt && tr.hitgroup <= HITGROUP_RIGHTLEG && tr.hitgroup > HITGROUP_GENERIC ) {
						auto ent = tr.m_pEnt->as< c_base_player >( );
						if ( ent->is_valid( ) && util::hitchance( ent->ce( )->GetIndex( ), aim_ang, g_settings.legit.active->m_triggerbot_hitchance, tr.hitbox ) ) {
							if ( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || g_settings.legit.friendlies ) {
								time_at = current_time;
							}
						}
					}
				}
			}

			if ( time_at > 0.1f ) {
				if ( ( current_time - time_at ) >= g_settings.legit.trigger_delay( ) ) {
					cmd->m_buttons |= IN_ATTACK;
					time_at = 0.f;
				}
			}
		}
	}

	void c_legitbot::sample_angle_data( const vec3_t& cur_angles ) {
		auto time = g_csgo.m_globals->m_curtime;

		if ( !m_aiming ) {
			m_angle_samples.push_front( { cur_angles, time } );
		}

		while ( m_angle_samples.size( ) > g_settings.legit.sample_size( ) ) {
			m_angle_samples.pop_back( );
		}
	}

	float c_legitbot::get_avg_delta( ) {
		if ( m_angle_samples.empty( ) )
			return 0.f;

		float avg_delta{ };

		for ( auto& it : m_angle_samples ) {
			static vec3_t last_angle = it.m_viewangles;

			//doing this implements nonsticky aswell
			//once you already are at the target
			//after lets say a flick
			//it will reduce the aim speed
			//making it not "stick" to the bone
			//as we dont store records when aiming
			float time_delta = g_csgo.m_globals->m_curtime - it.m_time;
			float delta_diff = m_deltatime / ( time_delta * 2.f );

			vec3_t	angle_delta = ( last_angle - it.m_viewangles ).clamp( );
			float	delta_length = angle_delta.length( ) * delta_diff;

			avg_delta += delta_length;
			last_angle = it.m_viewangles;
		}

		//scale the delta down
		return avg_delta / float( g_settings.legit.sample_size( ) );
	}

	vec2_t c_legitbot::angle_to_pixels( const vec3_t& angle ) {
		static auto m_yaw = g_csgo.m_cvar( )->FindVar( xors( "m_yaw" ) );
		static auto m_pitch = g_csgo.m_cvar( )->FindVar( xors( "m_pitch" ) );

		float x = angle.x / m_pitch->get_float( );
		float y = angle.y / m_yaw->get_float( );

		return vec2_t( -y, x );
	}

	vec3_t c_legitbot::pixels_to_angle( const vec2_t& pixel ) {
		static auto m_yaw = g_csgo.m_cvar( )->FindVar( xors( "m_yaw" ) );
		static auto m_pitch = g_csgo.m_cvar( )->FindVar( xors( "m_pitch" ) );

		float x = pixel.x * m_pitch->get_float( );
		float y = pixel.y * m_yaw->get_float( );

		return vec3_t( -y, x, 0.f ).clamp( );
	}

	bool c_legitbot::update_settings( ) {
		if ( !g_ctx.run_frame( ) ) return false;

		auto weapon = g_ctx.m_local->get_weapon( );
		if ( !weapon ) return false;

		if ( weapon->is_grenade( ) || weapon->is_knife( ) || weapon->m_iClip1( ) == -1 ) {
			return false;
		}

		if ( weapon->is_rifle( ) ) {
			g_settings.legit.active = &g_settings.legit.rifles;
		}
		else if ( weapon->is_sniper( ) ) {
			g_settings.legit.active = &g_settings.legit.snipers;
		}
		else if ( weapon->is_pistol( ) ) {
			g_settings.legit.active = &g_settings.legit.pistols;
		}
		else {
			g_settings.legit.active = &g_settings.legit.general;
		}

		return true;
	}

	void c_legitbot::aim_at_target( target_t data, float* x, float* y ) {
		vec3_t aim_ang;
		vec3_t cur_ang;
		vec3_t delta;
		vec2_t pixels;
		vec2_t mouse;

		g_csgo.m_engine( )->GetViewAngles( cur_ang );

		auto target = g_csgo.m_entlist( )->GetClientEntity< >( data.first );

		aim_ang = math::vector_angles( g_ctx.m_local->get_eye_pos( ), data.second );

		aim_ang -= g_ctx.m_local->m_aimPunchAngle( ) * 2.f * ( g_settings.legit.active->m_rcs / 100.f );

		delta = ( aim_ang - cur_ang ).clamp( );
		float delta_length = delta.length( );

		if ( delta_length ) {
			float final_time = delta_length / ( g_settings.legit.active->m_speed( ) / 75.f );
			m_curtime += m_deltatime;

			if ( m_curtime > final_time ) {
				m_curtime = final_time;
			}

			float aim_progress = m_curtime / final_time;
			aim_progress = math::ease_in( 0.f, 1.f, aim_progress, g_settings.legit.active->m_exponent );

			delta *= aim_progress;

			pixels = angle_to_pixels( delta );
			*x += pixels.x;
			*y += pixels.y;
		}
	}

	void c_legitbot::aimbot( float* x, float* y ) {
		if ( !g_settings.legit.enabled( ) ) return;
		if ( !update_settings( ) ) return;

		m_aiming = false;
		static float old_time = g_csgo.m_globals->m_curtime;
		float time = g_csgo.m_globals->m_curtime;

		m_deltatime = time - old_time;
		if ( g_settings.legit.dynamic_smoothing( ) ) {
			float avg_delta = get_avg_delta( ) * g_settings.legit.smooth_factor( );
			if ( avg_delta > m_deltatime ) {
				m_deltatime = avg_delta;
			}
		}

		old_time = time;

		bool in_attack = false;
		if ( g_settings.legit.activation_type == 0 )
			in_attack = true;
		if ( g_settings.legit.activation_type == 1
			&& g_input.is_key_pressed( (VirtualKeys_t) g_settings.legit.key( ) ) ) {
			in_attack = true;
		}
		if ( g_settings.legit.trigger_magnet && g_settings.legit.triggerbot
			&& g_input.is_key_pressed( g_settings.legit.trigger_key ) ) {
			if ( g_ctx.m_local->can_attack( ) ) {
				in_attack = true;
			}
		}

		auto target = get_aim_target( );
		auto target_index = get_aim_target( ).first;

		if ( target_index != -1 ) {
			if ( in_attack ) {
				m_aiming = true;
				aim_at_target( target, x, y );
			}
			if ( g_settings.legit.assist ) {
				assist( g_csgo.m_entlist( )->GetClientEntity< >( target.first ), x, y );
			}
		}
		else {
			m_curtime = 0;
		}
	}
}