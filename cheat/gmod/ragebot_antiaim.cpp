#include "ragebot.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "math.hpp"
#include "input_system.hpp"

namespace features
{
	void c_ragebot::c_lby_breaker::update_animstate( ) {
		static bool valid = false;
		static float last_spawn = 0.f;
		static float last_update = 0.f;
		if( !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) ) {
			if( m_animstate ) {
				g_csgo.m_alloc->Free( m_animstate );
				m_animstate = nullptr;
			}
			valid = false;
			return;
		}

		if( m_animstate && ( m_animstate->pBaseEntity != g_ctx.m_local || g_ctx.m_local->m_flSpawnTime( ) != last_spawn ) ) {
			last_spawn = g_ctx.m_local->m_flSpawnTime( );
			g_csgo.m_alloc->Free( m_animstate );
			m_animstate = nullptr;
			valid = false;
		}

		if( !m_animstate ) {
			m_animstate = ( CCSGOPlayerAnimState* )g_csgo.m_alloc->Alloc( sizeof( CCSGOPlayerAnimState ) );
		}

		if( !valid && m_animstate ) {
			g_ctx.m_local->create_animstate( m_animstate );
			valid = true;
		}
		else if( valid && m_animstate && !g_cheat.m_lagmgr.get_choked( ) ) {
			static float time_delta = g_csgo.m_globals->m_curtime - last_update;
			if( std::abs( time_delta ) > g_csgo.m_globals->m_interval_per_tick ) {
				vec3_t angles = g_ctx.m_last_realangle;

				C_AnimationLayer anim_backup[ 13 ];
				float param_backup[ 24 ];

				memcpy( anim_backup, g_ctx.m_local->m_AnimOverlay( ).GetElements( ), sizeof( anim_backup ) );
				memcpy( param_backup, g_ctx.m_local->m_flPoseParameter( ), sizeof( param_backup ) );

				m_animstate->update( angles.y, angles.x );
				m_animstate->m_flFeetSpeedForwardsOrSideWays = std::clamp( m_animstate->m_flFeetSpeedForwardsOrSideWays, 0.f, 2.f );
				m_animstate->m_flFeetSpeedUnknownForwardOrSideways = std::clamp( m_animstate->m_flFeetSpeedUnknownForwardOrSideways, 0.f, 3.f );

				memcpy( g_ctx.m_local->m_AnimOverlay( ).GetElements( ), anim_backup, sizeof( anim_backup ) );
				memcpy( g_ctx.m_local->m_flPoseParameter( ), param_backup, sizeof( param_backup ) );

				last_update = g_csgo.m_globals->m_curtime;
			}
		}

		bool moving = m_animstate->m_velocity > 0.1f && g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
		if( g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) ) moving = false;

		static bool was_moving = moving;
		float time = g_ctx.pred_time( );

		float lby = g_ctx.m_local->m_flLowerBodyYawTarget( );
		float real = g_ctx.m_last_realangle.y;

		if( moving ) {
			m_next_update = time + 0.22f;
			m_state = BS_MOVING;
		}
		else if( time >= m_next_update ) {
			//idk
			float latency = g_csgo.m_globals->m_interval_per_tick;

			m_next_update = time + ( was_moving ? 0.22f : 1.1f ) + latency;

			m_state = was_moving ? BS_WAS_MOVING : BS_BREAKING;
		}

		was_moving = moving;

		if( g_ctx.m_tickbase > 0 ) {
			auto& cur_update = m_lby_updates[ g_ctx.m_tickbase % 64 ];
			cur_update.m_tickbase = g_ctx.m_tickbase;
			cur_update.m_state = m_state;
		}
	}

	void c_ragebot::c_lby_breaker::on_lby_proxy( ) {
		float oldsimtime_1tickfuture = g_ctx.m_local->m_flOldSimulationTime( ) + TICK_INTERVAL( );
		int oldsimtime_ticks = TIME_TO_TICKS( oldsimtime_1tickfuture );

		float latency = g_csgo.m_engine( )->GetNetChannelInfo( )->GetLatency( 0 ) + g_csgo.m_globals->m_interval_per_tick;
		for( auto& it : m_lby_updates ) {
			if( it.m_tickbase == oldsimtime_ticks ) 
				if( it.m_state == BS_WAS_MOVING ) {
					//m_next_update = oldsimtime_1tickfuture + 0.22f - latency;
				
				if( it.m_state == BS_BREAKING ) 
					m_next_update = oldsimtime_1tickfuture + 1.1f - latency;
				
				break;
			}
		}
	}

	int c_ragebot::c_lby_breaker::get_next_update( ) const {
		auto current_tick = TIME_TO_TICKS( g_ctx.pred_time( ) );
		auto update_tick = TIME_TO_TICKS( m_next_update );

		return update_tick - current_tick;
	}

	void c_ragebot::c_lby_breaker::override_angles( float* angles, float original, float offset ) {
		const float angle = original;
		int update_ticks  = get_next_update( );

		if( g_cheat.m_lagmgr.get_state( ) )
			return;

		//yep
		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return;

		float break_angle = angle - offset;

		float lby = g_ctx.m_local->m_flLowerBodyYawTarget( );
		float real = angle;
		float delta = std::remainderf( lby - real, 360.f );

		if( std::abs( delta ) < 37.f && get_state( ) == BS_BREAKING ) {
			if( update_ticks < -TIME_TO_TICKS( 1.f ) ) {
				*angles = break_angle;
				return;
			}
		}

		if( update_ticks < -1 )
			return;

		if( ( get_state( ) == BS_WAS_MOVING || g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) ) 
			&& update_ticks >= 1 && offset < 140.f ) {
			*angles = angle + 140.f - offset;
		}

		if( /*get_state( ) > BS_MOVING &&*/ update_ticks < 1 ||
			get_state( ) == BS_WAS_MOVING && update_ticks < 3 ) {
			*angles = break_angle;
		}

		g_ctx.m_thirdperson_angle.y = *angles;
	}

	void c_ragebot::c_antiaim::fix_movement( ) {
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

	bool is_visible( const vec3_t& start, const vec3_t& end, void* skip ) {
		CTraceFilter filter;
		CGameTrace	 tr;
		Ray_t		 ray;
		
		filter.pSkip = skip;
		ray.Init( start, end );

		g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

		return tr.fraction >= 0.98f;
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

		int state = m_breaker.get_state( );
		int ticks = m_breaker.get_next_update( );
		switch( g_settings.rage.edge_dtc_type( ) ) {
		case 0: {
			break;
		}
		case 1: {
			m_breaker.override_angles( &final_yaw, edge_yaw, 110.f );
			m_cmd->m_viewangles.y = final_yaw;
			return true;
		}
		case 2: {
			m_breaker.override_angles( &final_yaw, edge_yaw, 180.f );
			m_cmd->m_viewangles.y = final_yaw;
			return true;	
		}
		default:
			break;
		}

		m_cmd->m_viewangles.y = final_yaw;
		return true;
	}

	bool c_ragebot::c_antiaim::run_freestanding( ) {
		if( !g_settings.rage.edge_detection )
			return false;

		bool   is_fake = g_cheat.m_lagmgr.get_state( );
		vec3_t enemy_pos;
		vec3_t local_pos;

		vec3_t aim_angle;
		vec3_t final_angle;
		vec3_t final_enemy_pos;
		vec3_t final_local_pos;

		float  cur_damage{ 30.f };
		bool   direction{ false };
		int	   enemy_index;

		enemy_index = util::get_closest_player( );

		if( enemy_index == -1 )
			return false;

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( enemy_index );

		enemy_pos	= ent->m_vecOrigin( );
		enemy_pos.z = ent->get_hitbox_pos( 0 ).z;

		local_pos	= g_ctx.m_local->get_eye_pos( );

		aim_angle	= math::vector_angles( enemy_pos, local_pos );

		for( int step_local = 1; step_local <= 4; ++step_local ) {
			float offset = step_local * 20.f;

			vec3_t local_left  = math::get_rotated_pos( local_pos, aim_angle.y + 90.f, -offset );
			vec3_t local_right = math::get_rotated_pos( local_pos, aim_angle.y + 90.f, offset );

			bool visible_left  = is_visible( local_pos, local_left, g_ctx.m_local );
			bool visible_right = is_visible( local_pos, local_right, g_ctx.m_local );

			if( !visible_left && !visible_right ) break;

			const float enemy_offset = 60.f;

			vec3_t enemy_left  = math::get_rotated_pos( enemy_pos, aim_angle.y + 90.f, enemy_offset );
			vec3_t enemy_right = math::get_rotated_pos( enemy_pos, aim_angle.y + 90.f, -enemy_offset );

			CTraceFilter filter;
			CGameTrace tr_right;
			CGameTrace tr_left;
			Ray_t ray;

			filter.pSkip = ent;
			ray.Init( enemy_pos, enemy_left );
			g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr_left );
			ray.Init( enemy_pos, enemy_right );
			g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr_right );

			enemy_left  = tr_left.endpos;
			enemy_right = tr_right.endpos;

			auto update_damage = [ & ]( const vec3_t& position, bool dir, bool center ) {
				if( is_visible( enemy_pos, position, ent ) || center ) {
					float dmg_left{ };
					float dmg_right{ };

					if( visible_left ) {
						dmg_left = get_damage( position, local_left, ent, g_ctx.m_local );
					}

					if( visible_right ) {
						dmg_right = get_damage( position, local_right, ent, g_ctx.m_local );
					}

					if( step_local == 1 ) {
						float delta = std::abs( dmg_left - dmg_right );
						if( delta < 35.f && dmg_left > 30.f && dmg_right > 30.f ) {
							return false;
						}
					}

					float  max_dmg = std::max< float >( dmg_left, dmg_right );
					vec3_t end_pos = dmg_left > dmg_right ? local_left : local_right;
					bool   new_dir = dmg_left > dmg_right;


					//add a bit of tolerance
					if( max_dmg > cur_damage + 10.f ) {
						final_enemy_pos = position;
						final_local_pos = end_pos;
						cur_damage		= max_dmg;
						direction		= new_dir;
					}
				}

				return true;
			};

			if( !update_damage( enemy_pos, true, true ) )
				return false;

			update_damage( enemy_left, true, false );
			update_damage( enemy_right, false, false );
		}
	

		if( cur_damage > 50.f ) {
			auto angle = math::vector_angles( final_local_pos, final_enemy_pos );
			
			m_direction = direction;
			angle.y += direction ? -90.f : 90.f;
			angle.y = math::find_closest_step( angle.y, 30.f );

			if( g_settings.rage.pitch == 3 && g_settings.rage.edge_detection > 1 && m_breaker.get_next_update( ) < 2 ) {
				vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );
				vec3_t rotated_pos = math::get_rotated_pos( eye_pos, angle.y - 180.f, 25.f );

				vec3_t velocity = g_ctx.m_local->m_vecVelocity( );

				vec3_t middle_pos = eye_pos + velocity * TICK_INTERVAL( ) * 2.f;
				middle_pos.z += 10.f;

				int dmg_rot = get_damage( final_enemy_pos, rotated_pos, ent, g_ctx.m_local );
				int dmg_mid = get_damage( final_enemy_pos, middle_pos, ent, g_ctx.m_local );

				if( dmg_mid < dmg_rot && ( dmg_mid < 100 || velocity.length2d( ) < 0.1f ) ) {
					m_cmd->m_viewangles.x = 0.f;
				}
			}

			if( is_fake ) {
				int jitter = g_settings.rage.edge_dtc_jitter;
				if( jitter ) {
					angle.y += math::random_number< float >( -jitter, jitter );
				}
				angle.y -= 180.f;
			}
			else {
				switch( g_settings.rage.edge_detection( ) ) {
				case 2:
					m_breaker.override_angles( &angle.y, angle.y, 110.f );
					break;
				case 3:
					m_breaker.override_angles( &angle.y, angle.y, 180.f );
					break;
				}
			}

			m_cmd->m_viewangles.y = angle.clamp( ).y;
			return true;
		}

		return false;
	}

	void c_ragebot::c_antiaim::run_fakewalk( ) {
		m_is_fakewalking = false;
		bool in_move = !( m_cmd->m_buttons & IN_FORWARD ) && !( m_cmd->m_buttons & IN_BACK ) && !( m_cmd->m_buttons & IN_LEFT ) && !( m_cmd->m_buttons & IN_RIGHT );

		if( !m_cmd->m_sidemove && !m_cmd->m_forwardmove && !g_ctx.m_local->m_vecVelocity( ) && !in_move )
			return;

		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return;
		
		if( m_cmd->m_buttons & IN_WALK ) {
			m_cmd->m_buttons &= ~IN_WALK;
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
			if( speed > 13.f ) {
				vec3_t direction = math::vector_angles( { 0.f, 0.f, 0.f }, vel );
				direction.y = g_ctx.get_last_cmd( )->m_viewangles.y - direction.y;

				vec3_t new_move = math::angle_vectors( direction );
				new_move *= -450.f;

				g_ctx.get_last_cmd( )->m_forwardmove = new_move.x;
				g_ctx.get_last_cmd( )->m_sidemove = new_move.y;
			}
			else {
				g_ctx.get_last_cmd( )->m_forwardmove = 0.f;
				g_ctx.get_last_cmd( )->m_sidemove = 0.f;
			}
		};

		//check how many ticks before LBY update comes in
		int ticks_to_update = m_breaker.get_next_update( ) - 1;

		int ticks_to_stop;
		for( ticks_to_stop = 0; ticks_to_stop < 15; ++ticks_to_stop ) {
			if( velocity.length2d( ) < 0.1f )
				break;

			predict_velocity( &velocity );
		}
		
		const int max_ticks = std::min< int >( g_settings.rage.fakewalk_ticks, ticks_to_update );
		const int choked	= g_cheat.m_lagmgr.get_choked( );
		int ticks_left		= max_ticks - choked;

		if( choked < max_ticks || ticks_to_stop ) {
			g_cheat.m_lagmgr.set_state( false );
		}

		if( !ticks_to_stop && !g_ctx.m_local->m_vecVelocity( ).length2d( ) && !g_cheat.m_lagmgr.get_sent( )
			&& choked > max_ticks ) {
			g_cheat.m_lagmgr.set_state( true );
		}

		if( ticks_to_stop > ticks_left || !choked || g_cheat.m_lagmgr.get_state( ) ) {
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
		case 4: //random
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
		if( g_ctx.m_local->m_vecVelocity( ).length( ) > 0.1f && !is_fakewalking( ) ) {
			if( fake && g_settings.rage.fake_yaw_moving( ) ) {
				setting = g_settings.rage.fake_yaw_moving( );
				jitter = g_settings.rage.fake_moving_jitter( );
				add = g_settings.rage.fake_yaw_moving_add( );
			}
			else if( g_settings.rage.real_yaw_moving( ) ) {
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
		case 1: //back
			return original - 180.f + rand;
		case 2: {//back spin
			float time = on_ground ? g_csgo.m_globals->m_curtime * 0.5f : ( g_csgo.m_globals->m_curtime - last_onground );
			float range = on_ground ? jitter : jitter * 2.f;
			return original - 180.f - range * 0.5f + ( no_jitter ? range : std::fmod( time * rate, range + 1.f ) );
		}
		case 3: //sideways
			//to be updated with the last saved yaw from freestanding/breaker
			return rand + fake ? ( side_switch ? original - 90.f : original + 90.f ) :
								 ( side_switch ? original + 90.f : original - 90.f );
		case 4: //slowpin lol
			return rand + std::fmod( g_csgo.m_globals->m_curtime * rate, 360.f );
		case 5: //random
			return math::random_number( -180.f, 180.f );
		case 6:
			return rand + fake ? ( m_direction ? original - 90.f : original + 90.f ) :
								 ( m_direction ? original + 90.f : original - 90.f );
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
				vec3_t ang = math::vector_angles( g_ctx.m_local->m_vecOrigin( ),
					ent->m_vecOrigin( ) );

				original = ang.y;
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
		if( !g_settings.rage.anti_aim( ) || !g_settings.rage.enabled( ) )
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

		auto weapon = g_ctx.m_local->get_weapon( );
		if( weapon && weapon->is_grenade( ) ) {
			if( weapon->m_fThrowTime( ) > g_csgo.m_globals->m_interval_per_tick ) {
				return;
			}
		}

		if( m_cmd->m_buttons & IN_USE )
			return;

		if( g_settings.rage.real_yaw( ) ||
			g_settings.rage.fake_yaw( ) ) {
			m_cmd->m_viewangles.y = get_yaw( );
			if( g_settings.rage.break_lby( ) ) {
				m_breaker.override_angles( &m_cmd->m_viewangles.y,
					get_yaw( true ),
					float( g_settings.rage.lby_delta ) );
			}
		}

		bool edge_detected = false;
		bool freestanding = false;

		if( g_settings.rage.pitch( ) ) {
			m_cmd->m_viewangles.x = get_pitch( );
		}

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

		if( edge_detected && g_cheat.m_lagmgr.get_choked( ) && m_breaker.get_next_update( ) > 1 ) {
			vec3_t vang{ };
			g_csgo.m_engine( )->GetViewAngles( vang );

			float delta = std::remainderf( vang.y - m_cmd->m_viewangles.y, 360.f );
			m_direction = delta < 0;
		}
	}
}