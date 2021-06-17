#include "interface.hpp"
#include "input_system.hpp"
#include "context.hpp"
#include "legitbot.hpp"
#include "settings.hpp"
#include "math.hpp"

namespace features
{
	int c_legitbot::get_aim_target( float fov ) {
		float	best_fov = fov ? fov : g_settings.legit.active->m_fov;
		int		target{ -1 };
		vec3_t	aim_angle{ };
		vec3_t	cur_angle{ };
		vec3_t	local_pos{ };
		vec3_t	aim_pos{ };

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

			bool visible = entity->is_visible( 0 );
			if( !visible && g_settings.legit.backtracking_target( ) ) {
				auto record = m_lagcomp.find_best_record( i );
				if( record ) {
					visible = util::trace_ray( local_pos, record->m_position,
						g_ctx.m_local->ce( ), entity->ce( ) );
				}
			}

			if ( !visible ) {
				continue;
			}

			aim_angle = math::vector_angles( local_pos, entity->get_hitbox_pos( 0 ) );
			aim_angle.clamp( );

			float fov = ( cur_angle - aim_angle ).clamp( ).length2d( );
			if( fov < best_fov ) {
				best_fov = fov;
				target = i;
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

		local_pos = g_ctx.m_local->get_eye_pos( );
		enemy_pos = target->get_hitbox_pos( 0 );

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

		if( !g_settings.legit.triggerbot( ) )
			return;

		if( !g_input.is_key_pressed( ( VirtualKeys_t )g_settings.legit.trigger_key( ) ) && !time_at ) // sometimes people will let go of their triggerbot key too quickly and will cause them to not shoot.
			return;

		vec3_t viewangles{ };
		vec3_t forward{ };
		g_csgo.m_engine( )->GetViewAngles( viewangles );

		auto wep = g_ctx.m_local->get_weapon( );
		if( !wep ) return;
		if( wep->is_knife( ) ) return;

		auto wpn_info = wep->get_wpn_info( );
		if( !wpn_info ) return;

		const float length		 = wpn_info->range;
		const float current_time = TICKS_TO_TIME( g_csgo.m_globals->m_tickcount );

		if( time_at && abs( current_time - time_at ) > 0.3f ) {
			time_at = 0.f; // delta too big
		}

		if( length > 1.0f ) {
			if( !time_at ) {
				viewangles += g_ctx.m_local->m_aimPunchAngle( ) * 2.f;

				forward = math::angle_vectors( viewangles );
				forward *= length;

				vec3_t src = g_ctx.m_local->get_eye_pos( );
				vec3_t dst = src + forward;

				CTraceFilter filter{ };
				CGameTrace	 tr{ };
				Ray_t		 ray{ };

				filter.pSkip = g_ctx.m_local;
				ray.Init( src, dst );

				g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

				if( tr.m_pEnt ) {
					auto ent = tr.m_pEnt->as< c_base_player >( );
					if( ent->is_valid( ) ) {
						if( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || g_settings.legit.friendlies ) {
							time_at = current_time;
						}
					}
				}
			}
			
			if( time_at ) {
				if( ( current_time - time_at ) >= g_settings.legit.trigger_delay( ) ) {
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
		static auto m_yaw	= g_csgo.m_cvar( )->FindVar( xors( "m_yaw" ) );
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

		if ( weapon->is_grenade( ) || weapon->is_knife( ) ) {
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

	void c_legitbot::aim_at_target( c_base_player* target, float* x, float* y ) {
		vec3_t aim_ang;
		vec3_t cur_ang;
		vec3_t delta;
		vec2_t pixels;
		vec2_t mouse;

		g_csgo.m_engine( )->GetViewAngles( cur_ang );

		if ( g_settings.legit.backtracking( ) && m_lagcomp.find_best_record( target->ce( )->GetIndex( ) ) ) {
			aim_ang = math::vector_angles( g_ctx.m_local->get_eye_pos( ),
				m_lagcomp.get_backtracked_position( target->ce( )->GetIndex( ) ) ).clamp( );
		}
		else {
			aim_ang = math::vector_angles( g_ctx.m_local->get_eye_pos( ),
				target->get_hitbox_pos( 0 ) ).clamp( );
		}

		aim_ang -= g_ctx.m_local->m_aimPunchAngle( ) * 2.f * ( g_settings.legit.active->m_rcs / 100.f );

		delta = ( aim_ang - cur_ang ).clamp( );
		float delta_length = delta.length( );

		if ( delta_length ) {
			float final_time = delta_length / ( g_settings.legit.active->m_speed( ) / 100.f );
			m_curtime += m_deltatime;

			if ( m_curtime > final_time ) {
				m_curtime = final_time;
			}

			float aim_progress = m_curtime / final_time;
			delta *= aim_progress;
			aim_ang = delta;

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
			&& g_input.is_key_pressed( ( VirtualKeys_t )g_settings.legit.key( ) ) ) {
			in_attack = true;
		}
		if( g_settings.legit.trigger_magnet
			&& g_settings.legit.triggerbot
			&& g_input.is_key_pressed( g_settings.legit.trigger_key ) ) {
			in_attack = true;
		}

		int  target_index = get_aim_target( );
		auto target = g_csgo.m_entlist( )->GetClientEntity< >( target_index );

		if ( target_index != -1 ) {
			if( in_attack ) {
				m_aiming = true;
				aim_at_target( target, x, y );
			}
			if( g_settings.legit.assist ) {
				assist( target, x, y );
			}
		}
		else {
			m_curtime = 0;
		}
	}
}