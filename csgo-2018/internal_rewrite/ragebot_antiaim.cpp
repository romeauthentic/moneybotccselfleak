#include "ragebot.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "math.hpp"
#include "input_system.hpp"

namespace features
{
	//don't ask
	/*vec3_t func( vec3_t a1, float a3, float a4, float a5, float a6, float a7, float a8, float a9 ) {
		int v8; // ebp@0
		float v9; // xmm6@1
		float v10; // xmm7_4@1
		float v11; // xmm2@1
		float v12; // xmm5@1
		float v13; // xmm4_4@1
		float v14 = 0.f; // xmm3_4@1
		float v15; // xmm1_4@2
		vec3_t v16; // xmm0@2
		float v17; // eax@2
		vec3_t v18; // ecx@2
		float v20; // xmm1_4@4
		float v21 = 0.f; // eax@4
		vec3_t v22; // ecx@4
		float v23; // eax@5
		float v24; // [sp-10h] [bp-1Ch]@1
		vec3_t v25;
		int v28; // [sp+0h] [bp-Ch]@1
		int v29; // [sp+4h] [bp-8h]@1

		v12 = a4;
		v9 = a3;
		v11 = a5;
		v10 = a7;
		v11 = a5 - a8;
		v12 = a4 - a7;

		v13 = a6 - a9;
		v24 = a6 - a9;

		v14 = sqrt( v12 * v12 + v11 * v11 + v13 * v13 );

		if( v14 <= v9 ) {
			if( -v9 <= v14 ) {
				a1.x = a4;
				a1.y = a5;
				a1.z = a6;
				return a1;
			}
			else {
				vec3_t v25 = { v11, v12, v24 };
				v20 = 1.0f / ( v12 * v12 + v11 * v11 + v13 * v13 + FLT_EPSILON );

				a7 = v10 - v20 * v12 * v9;
				a8 = a8 - v25.y * v20 * v9;

				a9 = a9 - v25.z * v20 * v9;
				v12 = a9;

				v22.x = a7;
				v22.y = a8;
				v22.z = a9;

				return v22;
			}
		}
		else {
			vec3_t v25 = { v11, v12, v24 };

			v15 = 1.0f / ( v12 * v12 + v11 * v11 + v13 * v13 + FLT_EPSILON );

			a7 = v15 * v12 * v9 + v10;
			a8 = v12 * v15 * v9 + a8;

			v16.x = a7;
			v16.y = a8;

			a9 = v25.z * v15 * v9 + a9;

			v18.x = a7;
			v18.y = a8;
			v18.z = a9;

			return v18;
		}
	}*/


	void c_ragebot::c_lby_breaker::update_animstate( ) {
		auto cl = g_csgo.m_global_state->get_client_state( );
		if( !cl )
			return;

		if( g_cheat.m_lagmgr.get_choked( ) )
			return;

		bool moving = g_ctx.m_local->get_animstate( )->m_velocity >= 0.1f && g_ctx.m_local->get_animstate( )->m_bOnGround;
		float time = g_ctx.pred_time( );

		if( moving ) {
			m_next_update = time + 0.22f;
		}
		else if( time > m_next_update ) {
			m_next_update = time + 1.1f;
		}

		if( g_ctx.m_tickbase > 0 ) {
			//auto& cur_update = m_lby_updates[ g_ctx.m_tickbase % 64 ];
			//cur_update.m_tickbase = g_ctx.m_tickbase;
			//cur_update.m_state = m_state;
		}
	}

	void c_ragebot::c_lby_breaker::on_lby_proxy( ) {
		float oldsimtime_1tickfuture = g_ctx.m_local->m_flOldSimulationTime( ) + TICK_INTERVAL( );
		int oldsimtime_ticks = TIME_TO_TICKS( oldsimtime_1tickfuture );

		float latency = g_csgo.m_engine( )->GetNetChannelInfo( )->GetLatency( 0 ) + g_csgo.m_globals->m_interval_per_tick;
		for( auto& it : m_lby_updates ) {
			if( it.m_tickbase == oldsimtime_ticks ) {
				//if( it.m_state == BS_BREAKING )
					//m_next_update = oldsimtime_1tickfuture + 1.1f - latency;
				break;

			}
		}
	}

	int c_ragebot::c_lby_breaker::get_next_update( ) const {
		auto current_tick = TIME_TO_TICKS( g_ctx.pred_time( ) );
		auto update_tick = TIME_TO_TICKS( m_next_update );

		return update_tick - current_tick;
	}

	void c_ragebot::c_lby_breaker::override_angles( float* angles, float original, float offset, bool freestanding ) {
		static bool was_breaking = false;
		static float last_angle = 0.f;
		const float angle = original;
		int update_ticks  = get_next_update( );

		if( g_cheat.m_lagmgr.get_state( ) )
			return;

		if( g_cheat.m_lagmgr.get_choked( ) )
			return;

		//yep
		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return;

		bool breaking = g_ctx.m_local->get_animstate( )->m_velocity < 0.1f;
		float break_angle = angle + offset;

		float lby = g_ctx.m_local->m_flLowerBodyYawTarget( );
		float real = angle;
		float delta = std::remainderf( lby - real, 360.f );

		if( std::abs( delta ) < 37.f && breaking ) {
			if( update_ticks < -TIME_TO_TICKS( 1.f ) ) {
				*angles = break_angle;
				return;
			}
		}

		if( freestanding && g_settings.rage.lby_avoid_updates && was_breaking ) {
			if( std::abs( std::remainderf( original - last_angle, 360.f ) ) > 50.f
				&& update_ticks < 1 ) {
				break_angle = last_angle;
				offset = std::remainderf( original - last_angle, 360.f );
			}
		}

		if( std::abs( offset ) < 105 ) {
			int min_tick = g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) ? g_settings.rage.fakewalk_ticks( ) + 1 : 3;
			if( update_ticks < min_tick && update_ticks >= 2 ) {
				*angles = break_angle + ( offset < 0 ? 105 : -105 );
				//g_ctx.m_thirdperson_angle.y = *angles;
				return;
			}
		}

		if( breaking && update_ticks < 3 ) {
			*angles = break_angle;
			if( !freestanding ) {
				last_angle = break_angle;
				was_breaking = true;
			}
		}

		//g_ctx.m_thirdperson_angle.y = *angles;
	}

	void c_ragebot::c_antiaim::fix_movement( ) {
		if( !g_settings.rage.anti_aim && !g_settings.rage.enabled )
			return;

		float pitch = std::remainderf( m_cmd->m_viewangles.x, 360.f );
		bool fix_forward_move = ( pitch < -90.f || pitch > 90.f );

		auto last_cmd = g_ctx.get_last_cmd( );
		auto old_yaw  = last_cmd->m_viewangles.y;
		auto cur_yaw  = m_cmd->m_viewangles.y;

		float yaw_delta = cur_yaw - old_yaw;

		float f1 = old_yaw < 0.f ? old_yaw + 360.f : old_yaw;
		float f2 = cur_yaw < 0.f ? cur_yaw + 360.f : cur_yaw;

		if( f2 < f1 )
			yaw_delta = abs( f2 - f1 );
		else
			yaw_delta = 360.f - abs( f1 - f2 );
		yaw_delta = 360.f - yaw_delta;

		m_cmd->m_forwardmove = cos( yaw_delta * M_PIRAD ) * last_cmd->m_forwardmove + cos( ( yaw_delta + 90.f ) * M_PIRAD ) * last_cmd->m_sidemove;
		m_cmd->m_sidemove	 = sin( yaw_delta * M_PIRAD ) * last_cmd->m_forwardmove + sin( ( yaw_delta + 90.f ) * M_PIRAD ) * last_cmd->m_sidemove;

		if( fix_forward_move )
			m_cmd->m_forwardmove *= -1.f;

		if( g_ctx.m_local->m_nMoveType( ) != MOVETYPE_LADDER ) {
			if( m_cmd->m_forwardmove ) {
				m_cmd->m_buttons &= ~( m_cmd->m_forwardmove < 0 ? IN_FORWARD : IN_BACK );
				m_cmd->m_buttons |= ( m_cmd->m_forwardmove > 0 ? IN_FORWARD : IN_BACK );
			}
			if( m_cmd->m_sidemove ) {
				m_cmd->m_buttons &= ~( m_cmd->m_sidemove < 0 ? IN_MOVERIGHT : IN_MOVELEFT );
				m_cmd->m_buttons |= ( m_cmd->m_sidemove > 0 ? IN_MOVERIGHT : IN_MOVELEFT );
			}
		}
	}

	bool c_ragebot::c_antiaim::run_edge_dtc( ) {
		if( !g_settings.rage.edge_dtc_normal )
			return false;

		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return false;

		if( g_settings.rage.edge_dtc_normal == 1 ) {
			float speed = g_ctx.m_local->m_vecVelocity( ).length2d( );
			if( speed < 30.f ) return false;
		}

		bool peeking = g_ctx.m_local->m_vecVelocity( ).length( ) > 30.f;
		if( is_fakewalking( ) ) peeking = false;
		CTraceFilterWorldAndPropsOnly filter;

		float max_dist = peeking ? g_settings.rage.edge_dtc_moving : g_settings.rage.edge_dtc_standing;
		float min_dist = 64.f;
		float edge_yaw = 0.f;
		float final_yaw = 0.f;
		bool detected = false;

		const float step = 45.f;
		vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );

		for( float rot = -180.f; rot < 180.f; rot += step ) {
			float rot_left = rot - 90.f;
			float rot_right = rot + 90.f;

			vec3_t pos_left = math::get_rotated_pos( eye_pos, rot_left, 30.f );
			vec3_t pos_right = math::get_rotated_pos( eye_pos, rot_right, 30.f );

			vec3_t forward = math::angle_vectors( vec3_t( 0.f, rot, 0.f ) );
			forward *= 4000.f;

			CGameTrace  trace_left, trace_right;
			Ray_t		ray_left, ray_right;

			ray_left.Init( pos_left, pos_left + forward );
			ray_right.Init( pos_right, pos_right + forward );

			g_csgo.m_trace( )->TraceRay( ray_left, MASK_SHOT_HULL, &filter, &trace_left );
			g_csgo.m_trace( )->TraceRay( ray_right, MASK_SHOT_HULL, &filter, &trace_right );

			float dist_left = ( pos_left - trace_left.endpos ).length( );
			float dist_right = ( pos_right - trace_right.endpos ).length( );

			if( dist_right > dist_left ) {
				if( dist_right > max_dist && dist_left < min_dist ) {
					max_dist = dist_right;
					edge_yaw = rot_left;
					detected = true;
				}
			}
			else {
				if( dist_left > max_dist && dist_right < min_dist ) {
					max_dist = dist_left;
					edge_yaw = rot_right;
					detected = true;
				}
			}
		}

		if( !detected )
			return false;

		final_yaw = edge_yaw;
		if( g_cheat.m_lagmgr.get_state( ) ) {
			final_yaw = edge_yaw + 180.f;
		}

		m_cmd->m_viewangles.y = final_yaw;
		return true;
	}

	float get_damage( const vec3_t& start, const vec3_t& end, c_base_player* a, c_base_player* b ) {
		static weapon_info_t wpn_data{ };
		wpn_data.damage = 200;
		wpn_data.range_modifier = 1.0f;
		wpn_data.penetration = 3.0f;
		wpn_data.armor_ratio = 0.5f;
		wpn_data.range = 8192.f;

		fire_bullet_data_t data{ };
		data.src = start;
		data.filter.pSkip = a;
		data.length_to_end = ( end - start ).length( );

		vec3_t angle = math::vector_angles( start, end );
		data.direction = math::angle_vectors( angle );

		data.direction.normalize_vector( );

		if( g_cheat.m_autowall.fire_bullet( a, b, &wpn_data, data, false, false ) ) {
			return data.current_damage;
		}

		return -1.f;
	}

	vec3_t trace_ent_pos( vec3_t start, vec3_t end, c_base_player* ent ) {
		Ray_t ray;
		CGameTrace tr;
		CTraceFilter filter;

		const vec3_t min( -2.f, -2.f, -2.f );
		const vec3_t max( 2.f, 2.f, 2.f );

		ray.Init( start, end, min, max );
		filter.pSkip = ent;

		g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

		return tr.endpos;
	}

	bool c_ragebot::c_antiaim::run_freestanding( int player ) {
		if( !g_settings.rage.edge_detection )
			return false;

		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return false;

		bool   is_fake = g_cheat.m_lagmgr.get_state( );
		vec3_t enemy_pos;
		vec3_t local_pos;

		vec3_t aim_angle;

		float  cur_damage{ 30.f };
		bool   direction{ false };
		int	   enemy_index;

		enemy_index = player == -1 ? util::get_closest_player( ) : player;

		if( enemy_index == -1 )
			return false;

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( enemy_index );

		if( !ent || !ent->is_valid( ) )
			return false;

		enemy_pos	= ent->m_vecOrigin( );
		enemy_pos.z = ent->get_hitbox_pos( 0 ).z;

		local_pos	= g_ctx.m_local->get_eye_pos( );
		aim_angle	= math::vector_angles( enemy_pos, local_pos );

		float dist = ( float )( g_settings.rage.freestanding_distance( ) );
		
		float start_dmg = get_damage( enemy_pos, local_pos, ent, g_ctx.m_local );

		auto test_dmg = [ & ]( float dist ) {
			vec3_t enemy_left = math::get_rotated_pos( enemy_pos, aim_angle.y - 90.f, dist );
			vec3_t enemy_right = math::get_rotated_pos( enemy_pos, aim_angle.y + 90.f, dist );

			enemy_left = trace_ent_pos( enemy_pos, enemy_left, ent );
			enemy_right = trace_ent_pos( enemy_pos, enemy_right, ent );

			vec3_t local_left = math::get_rotated_pos( local_pos, aim_angle.y + 90.f, dist );
			vec3_t local_right = math::get_rotated_pos( local_pos, aim_angle.y - 90.f, dist );

			local_left = trace_ent_pos( local_pos, local_left, g_ctx.m_local );
			local_right = trace_ent_pos( local_pos, local_right, g_ctx.m_local );

			float dmg_left = get_damage( enemy_left, local_left, ent, g_ctx.m_local )
				+ get_damage( enemy_right, local_left, ent, g_ctx.m_local );

			float dmg_right = get_damage( enemy_left, local_right, ent, g_ctx.m_local )
				+ get_damage( enemy_right, local_right, ent, g_ctx.m_local );

			if( std::abs( dmg_left - dmg_right ) < 50.f )
				return false;

			float max_dmg = math::max( dmg_left, dmg_right );
			direction = dmg_left > dmg_right;

			cur_damage = max_dmg;
			return max_dmg > 50.f;
		};

		if( !test_dmg( 20.f ) ) {
			if( cur_damage < start_dmg * 2.f )
				return false;
		}

		if( !test_dmg( dist ) )
			return false;

		m_direction = direction;
		aim_angle.y += direction ? -90.f : 90.f;
		aim_angle.y = math::find_closest_step( aim_angle.y, 30.f );

		if( g_settings.rage.pitch == 3 && g_settings.rage.break_lby_edge && m_breaker.get_next_update( ) < 2 && g_ctx.m_local->get_animstate( )->m_velocity < 0.1f ) {
			vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );
			vec3_t rotated_pos = math::get_rotated_pos( eye_pos, aim_angle.y - 180.f, 25.f );

			vec3_t velocity = g_ctx.m_local->m_vecVelocity( );

			vec3_t middle_pos = eye_pos + velocity * TICK_INTERVAL( ) * 2.f;
			middle_pos.z += 10.f;

			int dmg_rot = get_damage( enemy_pos, rotated_pos, ent, g_ctx.m_local );
			int dmg_mid = get_damage( enemy_pos, middle_pos, ent, g_ctx.m_local );

			if( dmg_mid < dmg_rot ) 
				m_cmd->m_viewangles.x = 0.f;
		}

		if( is_fake ) {
			int jitter = g_settings.rage.edge_dtc_jitter;
			if( jitter ) {
				aim_angle.y += math::random_number< float >( -jitter, jitter );
			}
			aim_angle.y -= 180.f;
		}

		m_cmd->m_viewangles.y = aim_angle.clamp( ).y;
		return true;
	}

	void c_ragebot::c_antiaim::run_fakewalk( ) {
		m_is_fakewalking = false;
		if( !g_ctx.m_local->get_weapon( ) )
			return;

		bool in_move = !( m_cmd->m_buttons & IN_FORWARD ) && !( m_cmd->m_buttons & IN_BACK ) && !( m_cmd->m_buttons & IN_LEFT ) && !( m_cmd->m_buttons & IN_RIGHT );

		if( !m_cmd->m_sidemove && !m_cmd->m_forwardmove && !g_ctx.m_local->m_vecVelocity( ) && !in_move )
			return;

		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return;
		
		if( m_cmd->m_buttons & IN_WALK ) {
			m_cmd->m_buttons &= ~IN_WALK;
		}

		if( m_cmd->m_buttons & IN_SPEED ) {
			m_cmd->m_buttons &= ~IN_SPEED;
		}

		m_is_fakewalking = true;

		auto velocity = g_ctx.m_local->m_vecVelocity( );

		static auto predict_velocity = [ ]( vec3_t* velocity ) {
			static auto sv_friction = g_csgo.m_cvar( )->FindVar( xors( "sv_friction" ) );
			static auto sv_stopspeed = g_csgo.m_cvar( )->FindVar( xors( "sv_stopspeed" ) );

			float speed = velocity->length( );
			if( speed >= 0.1f ) {
				float friction = sv_friction->get_float( );
				float stop_speed = std::max< float >( speed, sv_stopspeed->get_float( ) );
				float time = std::max< float >( g_csgo.m_globals->m_interval_per_tick, g_csgo.m_globals->m_frametime );
				*velocity *= std::max< float >( 0.f, speed - friction * stop_speed * time / speed );
			}
		};

		static auto quick_stop = [ & ]( ) {
			vec3_t vel = g_ctx.m_local->m_vecVelocity( );
			float speed = vel.length2d( );

			if( speed < 1.f ) {
				g_ctx.get_last_cmd( )->m_forwardmove = 0.f;
				g_ctx.get_last_cmd( )->m_sidemove = 0.f;
				return;
			}

			static auto sv_accelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_accelerate" ) );
			float accel = sv_accelerate->get_float( );
			float max_speed = g_ctx.m_local->get_weapon( )->get_wpn_info( )->max_speed;
			if( g_ctx.m_local->get_weapon( )->is_sniper( ) && g_ctx.m_local->m_bIsScoped( ) ) {
				max_speed = g_ctx.m_local->get_weapon( )->get_wpn_info( )->max_speed_alt;
			}

			if( g_ctx.m_local->m_fFlags( ) & FL_DUCKING ) {
				max_speed /= 3.f;
				accel /= 3.f;
			}

			float surf_friction = 1.f;
			float max_accelspeed = accel * g_csgo.m_globals->m_interval_per_tick * max_speed * surf_friction;

			float wishspeed{ };

			if( speed - max_accelspeed <= -1.f ) {
				wishspeed = max_accelspeed / ( speed / ( accel * g_csgo.m_globals->m_interval_per_tick ) );
			}
			else {
				wishspeed = max_accelspeed;
			}

			vec3_t ndir = math::vector_angles( vel * -1.f );
			ndir.y = m_cmd->m_viewangles.y - ndir.y;
			ndir = math::angle_vectors( ndir );

			g_ctx.get_last_cmd( )->m_forwardmove = ndir.x * wishspeed;
			g_ctx.get_last_cmd( )->m_sidemove = ndir.y * wishspeed;
		};

		//int ticks_to_update = g_settings.rage.break_lby( ) ? m_breaker.get_next_update( ) - 1 : g_settings.rage.fakewalk_ticks;

		int ticks_to_stop;
		for( ticks_to_stop = 0; ticks_to_stop < 15; ++ticks_to_stop ) {
			if( velocity.length2d( ) < 0.1f )
				break;

			predict_velocity( &velocity );
		}
		

		float time_to_shoot = g_ctx.m_local->get_weapon( )->m_flPostponeFireReadyTime( ) - g_ctx.pred_time( );
		int revolver_ticks = TIME_TO_TICKS( time_to_shoot ) - 1;
		if( revolver_ticks < -1 )
			revolver_ticks = INT_MAX;

		int update_ticks = m_breaker.get_next_update( ) - 1;

		const int max_ticks = math::min< int >( g_settings.rage.fakewalk_ticks, update_ticks, revolver_ticks );
		const int choked	= g_cheat.m_lagmgr.get_choked( );
		int ticks_left		= max_ticks - choked;

		if( choked < max_ticks || ticks_to_stop ) {
			g_cheat.m_lagmgr.set_state( false );
		}

		if( !g_ctx.m_local->m_vecVelocity( ).length2d( ) && !g_cheat.m_lagmgr.get_sent( ) && choked > max_ticks ) {
			g_cheat.m_lagmgr.set_state( true );
		}

		if( ticks_to_stop > ticks_left - 1 || !choked ) {
			quick_stop( );
		}
	}

	float c_ragebot::c_antiaim::get_pitch( ) {
		switch( g_settings.rage.pitch( ) ) {
		case 1: //custom
			return float( g_settings.rage.pitch_offset( ) );
		case 2: //flip
			return g_cheat.m_lagmgr.get_state( ) ? 75.f : 89.f;
		case 3:
			return 89.f;
		case 4:
			return 1080.f + g_settings.rage.pitch_offset( );
		case 5:
			return -1080.f + g_settings.rage.pitch_offset( );
		case 6:
			return m_cmd->m_cmd_nr % 2 ? 1080.f : -1080.f;
		case 7: //random
			return math::random_number( -89.f, 89.f );
		}

		return 0.f;
	}

	float c_ragebot::c_antiaim::get_yaw( bool fake, float original, bool no_jitter ) {
		constexpr static double rate = 360.0 / 1.618033988749895;
		constexpr static int	switch_rate = 10;
		static float			last_onground = 0.f;
		static int				ticks = 0;
		static bool				side_switch = false;
		bool					on_ground = false;

		int setting = fake ? g_settings.rage.fake_yaw : g_settings.rage.real_yaw;
		int jitter = fake ? g_settings.rage.fake_yaw_jitter : g_settings.rage.real_yaw_jitter;
		int add = fake ? g_settings.rage.fake_yaw_add : g_settings.rage.real_yaw_add;
		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			if( !fake && g_settings.rage.air_yaw( ) ) {
				setting = g_settings.rage.air_yaw;
				jitter = g_settings.rage.air_yaw_jitter;
				add = g_settings.rage.air_yaw_add;
			}
		}
		else if( g_ctx.m_local->m_vecVelocity( ).length( ) > 0.1f && !is_fakewalking( ) ) {
			if( !fake && g_settings.rage.real_yaw_moving( ) ) {
				setting = g_settings.rage.real_yaw_moving( );
				jitter = g_settings.rage.real_moving_jitter( );
				add = g_settings.rage.real_yaw_moving_add( );
			}
		}

		if( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) {
			last_onground = g_ctx.pred_time( );
			on_ground = true;
		}

		ticks++;
		if( ticks > switch_rate ) {
			side_switch ^= 1;
			ticks = 0;
		}
		
		float rand = 0.f;
		if( setting && jitter && !no_jitter ) {
			rand = math::random_number( -float( jitter ), float( jitter ) );
		}

		if( setting && add ) {
			original += add;
		}

		switch( setting ) {
		case 0:
			return g_csgo.m_engine( )->GetViewAngles( ).y;
		case 1: //back
			return original - 180.f + rand;
		case 2: {//back spin
			float time = on_ground ? g_csgo.m_globals->m_curtime * 0.5f : ( g_csgo.m_globals->m_curtime - last_onground );
			float range = on_ground ? jitter : jitter * 2.f;
			return original - 180.f - range * 0.5f + ( no_jitter ? range : std::fmod( time * rate, range + 1.f ) );
		}
		case 3: //sideways
			return rand + ( fake ? ( side_switch ? original + 90.f : original - 90.f ) :
								   ( side_switch ? original - 90.f : original + 90.f ) );
		case 4: //slowpin lol
			return rand + std::fmod( g_csgo.m_globals->m_curtime * rate, 360.f );
		case 5: //random
			return math::random_number( -180.f, 180.f );
		case 6: //direction
			return rand + ( fake ? ( m_direction ? original - 90.f : original + 90.f ) :
								   ( m_direction ? original + 90.f : original - 90.f ) );
		case 7:
			return original - 180.f + ( no_jitter ? 0 : ( side_switch ? -jitter : jitter ) );
		}

		return original;
	}

	float c_ragebot::c_antiaim::get_yaw( bool no_jitter ) {
		const auto is_fake = g_cheat.m_lagmgr.get_state( );
		auto	   original = g_ctx.get_last_cmd( )->m_viewangles.y;

		if( g_settings.rage.at_players ) {
			auto closest_ent = util::get_closest_player( );
			if( closest_ent != -1 ) {
				auto ent = g_csgo.m_entlist( )->GetClientEntity< >( closest_ent );
				if( ent ) {
					vec3_t ang = math::vector_angles( g_ctx.m_local->m_vecOrigin( ),
						ent->m_vecOrigin( ) );

					original = ang.y;
				}
			}
		}

		if( is_fake && !g_settings.rage.fake_yaw( ) )
			return original;
		if( !is_fake && !g_settings.rage.real_yaw( ) )
			return original;

		if( g_settings.rage.angle_step( ) && g_settings.rage.angle_step_degrees( ) ) {
			float step = float( g_settings.rage.angle_step_degrees( ) );
			original = math::find_closest_step( original, step );
		}
		return get_yaw( is_fake, original, no_jitter );
	}

	void c_ragebot::c_antiaim::run( ) {
		m_is_edging = false;
		if( !g_settings.rage.anti_aim( ) )
			return;

		auto is_everyone_dormant = [ ]( ) {
			for( int i{ 1 }; i < g_csgo.m_globals->m_maxclients; ++i ) {
				auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );
				if( ent && ent->is_valid( ) &&
				  ( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( )
					|| g_settings.rage.friendlies ) ) {
					return false;
				}
			}

			return true;
		};

		if( ( m_cmd->m_forwardmove || m_cmd->m_sidemove ) && 
			( g_ctx.m_local->m_nMoveType( ) == MOVETYPE_LADDER ||
				g_ctx.m_local->m_nMoveType( ) == MOVETYPE_NOCLIP ) )
			return;

		if( g_ctx.m_local->m_fFlags( ) & FL_FROZEN )
			return;

		if( is_everyone_dormant( ) && g_settings.rage.dormant_check )
			return;

		if( g_settings.rage.fakewalk( ) && g_input.is_key_pressed( g_settings.rage.fakewalk_key( ) ) ) {
			run_fakewalk( );
		}
		else {
			m_is_fakewalking = false;
		}

		if( ( m_cmd->m_buttons & IN_ATTACK ) && g_ctx.m_local->can_attack( ) )
			return;

		if( *( bool* )( c_base_player::get_game_rules( ) + 0x20 ) )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( weapon && weapon->is_grenade( ) ) {
			if( weapon->m_fThrowTime( ) > g_csgo.m_globals->m_interval_per_tick ) {
				return;
			}
		}

		if( ( m_cmd->m_buttons & IN_ATTACK2 ) && weapon->is_knife( ) && g_ctx.m_local->can_attack( ) )
			return;

		if( m_cmd->m_buttons & IN_USE )
			return;

		if( g_settings.rage.real_yaw( ) ||
			g_settings.rage.fake_yaw( ) ) {
			m_cmd->m_viewangles.y = get_yaw( );
		}

		if( g_settings.rage.pitch( ) ) {
			m_cmd->m_viewangles.x = get_pitch( );
		}

		bool edge_detected = false;
		bool freestanding = false;

		if( !g_settings.rage.edge_dtc_priority ) {
			freestanding = run_freestanding( );
			if( !freestanding )
				edge_detected = run_edge_dtc( );
		}
		else {
			edge_detected = run_edge_dtc( );
			if( !edge_detected )
				freestanding = run_freestanding( );
		}

		m_is_edging = freestanding || edge_detected;

		if( g_settings.rage.break_lby( ) && ( !m_is_edging || g_settings.rage.break_lby_edge ) ) {
			float yaw = m_is_edging ? m_cmd->m_viewangles.y : get_yaw( true );

			m_breaker.override_angles( &m_cmd->m_viewangles.y,
				yaw,
				std::remainderf( 180.f - float( g_settings.rage.lby_delta ), 360.f ),
				freestanding );
		}

		if( g_cheat.m_lagmgr.get_state( ) && !g_settings.rage.fake_yaw( ) )
			m_cmd->m_viewangles.y = g_csgo.m_engine( )->GetViewAngles( ).y;

		//since we're going to have fake pitches most likely
		//make sure to clamp yaw anyway just in case some retard
		//decides to enable untrusted shit in an mm server or w/e
		m_cmd->m_viewangles.y = std::remainderf( m_cmd->m_viewangles.y, 360.f );
	}
}