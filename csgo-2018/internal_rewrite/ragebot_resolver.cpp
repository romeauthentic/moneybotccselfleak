#include "ragebot.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "math.hpp"
#include "input_system.hpp"

namespace features
{
	constexpr float FLT_ANG_LBY = 360.f;
	constexpr float FLT_ANG_MOVING_LBY_UP = -360.f;
	constexpr float FLT_ANG_180 = 720.f;
	constexpr float FLT_ANG_FREESTANDING = -720.f;
	constexpr float FLT_ANG_90 = 480.f;
	constexpr float FLT_ANG_MINUS_90 = -480.f;
	constexpr float FLT_ANG_MINUS_135 = -700.f;
	constexpr float FLT_ANG_135 = 700.f;
	constexpr float FLT_ANG_PITCH_UP = 1080.f;

	static const std::vector< float > possible_angles_none = {
		FLT_ANG_MOVING_LBY_UP,
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		FLT_ANG_MINUS_135,
		FLT_ANG_135,
	};


	static const std::vector< float > possible_angles_adjust = {
		FLT_ANG_FREESTANDING,
		FLT_ANG_MOVING_LBY_UP,
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		FLT_ANG_135,
		FLT_ANG_MINUS_135,
		//180.f,
	};

	bool dbg_resolver( ) {
		static con_var< bool > var{ &data::holder_, fnv( "dbg_resolver" ), false };
		return var( );
	}

	int c_ragebot::c_resolver::try_freestanding( c_base_player* ent ) {
		if( !( ent->m_fFlags( ) & FL_ONGROUND ) || ( ent->m_vecVelocity( ).length2d( ) > 0.1f && !ent->is_fakewalking( ) ) )
			return -1;

		int ret_dir		 = -1;
		float cur_damage = 0.f;

		vec3_t enemy_pos = ent->m_vecOrigin( );
		enemy_pos.z		 = ent->get_hitbox_pos( 0 ).z;

		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );
		local_pos -= g_ctx.m_local->m_vecOrigin( );
		local_pos += g_ctx.m_last_origin;

		vec3_t aim_angle = math::vector_angles(
			enemy_pos,
			local_pos
		);

		auto get_damage = [ & ]( const vec3_t& start, const vec3_t& end, c_base_player* a, c_base_player* b ) {
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
		};

		float start_dmg = get_damage( local_pos, enemy_pos, g_ctx.m_local, ent );

		auto trace_ent_pos = [ & ]( vec3_t start, vec3_t end, c_base_player* ent ) {
			Ray_t ray;
			CGameTrace tr;
			CTraceFilter filter;

			const vec3_t min( -2.f, -2.f, -2.f );
			const vec3_t max( 2.f, 2.f, 2.f );

			ray.Init( start, end, min, max );
			filter.pSkip = ent;

			g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );

			return tr.endpos;
		};

		auto test_dmg = [ & ]( float dist ) {
			vec3_t enemy_left = math::get_rotated_pos( enemy_pos, aim_angle.y - 90.f, dist );
			vec3_t enemy_right = math::get_rotated_pos( enemy_pos, aim_angle.y + 90.f, dist );

			enemy_left = trace_ent_pos( enemy_pos, enemy_left, ent );
			enemy_right = trace_ent_pos( enemy_pos, enemy_right, ent );

			vec3_t local_left = math::get_rotated_pos( local_pos, aim_angle.y + 90.f, dist );
			vec3_t local_right = math::get_rotated_pos( local_pos, aim_angle.y - 90.f, dist );

			local_left = trace_ent_pos( local_pos, local_left, g_ctx.m_local );
			local_right = trace_ent_pos( local_pos, local_right, g_ctx.m_local );

			float dmg_left = get_damage( local_left, enemy_left, ent, g_ctx.m_local )
				+ get_damage( local_right, enemy_left, ent, g_ctx.m_local );

			float dmg_right = get_damage( local_left, enemy_right, ent, g_ctx.m_local )
				+ get_damage( local_right, enemy_right, ent, g_ctx.m_local );

			if( std::abs( dmg_left - dmg_right ) < 50.f )
				return false;

			float max_dmg = math::max( dmg_left, dmg_right );
			ret_dir = dmg_left > dmg_right;

			cur_damage = max_dmg;
			return max_dmg > 50.f;
		};

		float dist = ( ent->m_vecOrigin( ) - ent->get_hitbox_pos( 0 ) ).length2d( );

		if( !test_dmg( dist + 1.f ) ) {
			if( cur_damage < start_dmg * 2.f )
				return -1;
		}

		if( !test_dmg( 50.f ) )
			return -1;

		return ret_dir;
	}

	void c_ragebot::c_resolver::resolve_log_t::update( int ent_index ) {
		static C_AnimationLayer last_layer[ 65 ]{ };
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		if( !ent || ent == g_ctx.m_local || !ent->is_player( ) || !ent->is_alive( ) || ent->ce( )->IsDormant( ) || !ent->has_valid_anim( ) ) {
			m_was_invalid = true;
			m_has_valid_lby = false;
			return;
		}

		if( m_was_invalid && m_last_pos.dist_to( ent->m_vecOrigin( ) ) > 4.f )
			reset( );

		if( !( ent->m_fFlags( ) & FL_ONGROUND ) ) {
			m_breaking = BREAKING_NONE;
			return;
		}

		if( ent->m_vecVelocity( ).length2d( ) > 0.1f && !ent->is_fakewalking( ) ) {
			m_breaking = BREAKING_NONE;
			m_has_valid_lby = true;
			return;
		}

		m_last_pos = ent->m_vecOrigin( );

		m_was_invalid = false;
		m_shots %= 100;
		float lby = ent->m_flLowerBodyYawTarget( );
		if( lby != m_last_lby ) {
			m_last_lby = lby;
			m_last_update = ent->m_flSimulationTime( );
			m_breaking = BREAKING_NONE;
		}

		if( ent->m_flSimulationTime( ) - m_last_update > TICK_INTERVAL( ) ) {
			m_breaking = BREAKING;
		}

		m_has_valid_lby = true;
	}

	void c_ragebot::c_resolver::resolve_log_t::reset( ) {
		//assuming theyre STILL not moving this shouldnt change
		//however if they are, it will be reset to none anyway
		m_breaking = BREAKING_NONE;
		m_last_update = 0.f;
		m_last_lby = 0.f;
		m_missed_shots = 0;
		m_shots = 0;
		m_logged = false;
		m_logged_shots = 0;
	}

	std::vector< float > c_ragebot::c_resolver::get_shot_vec( int ent ) {
		auto& data = m_data[ ent ];

		if( data.m_breaking )
			return possible_angles_adjust;

		return possible_angles_none;
	}

	float c_ragebot::c_resolver::get_shot_yaw( int shots, int ent_index ) {
		auto& data = m_data[ ent_index ];
		auto  ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		//if( data.m_logged && data.m_breaking ) {
		//	return data.m_logged_lby;
		//}

		auto vec = get_shot_vec( ent_index );
		return vec.at( shots % vec.size( ) );
	}

	void c_ragebot::c_resolver::aimbot( int ent_index, int hitbox, vec3_t angle, vec3_t position, vec3_t min, vec3_t max, float radius ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		bool moving = ent->m_vecVelocity( ).length2d( ) > 0.1f && ( ent->m_fFlags( ) & FL_ONGROUND );

		if( moving && !ent->is_fakewalking( ) ) {
			return;
		}

		if( !data.m_breaking ) {
			increment_shots( ent_index );
			return;
		}

		auto model = ent->ce( )->GetModel( );
		auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
		auto set = hdr->pHitboxSet( ent->m_nHitboxSet( ) );

		if( hitbox == HITBOX_HEAD ) {
			auto box = set->pHitbox( 0 );
			if( box ) {
				auto dist = box->bbmin.dist_to( box->bbmax );

				context::shot_data_t new_shot{ };
				new_shot.m_angle = angle;
				new_shot.m_enemy_pos = position;
				new_shot.m_enemy_index = ent_index;
				new_shot.m_local_pos = g_ctx.m_local->get_eye_pos( );
				new_shot.m_resolver_shots = data.m_shots;
				new_shot.m_resolver_state = true;
				new_shot.m_hitbox.min = min;
				new_shot.m_hitbox.min = max;
				new_shot.m_hitbox.radius = radius;
				new_shot.m_missed = true;
				new_shot.m_hitgroup = HITGROUP_HEAD;

				g_ctx.m_last_shot++;
				g_ctx.m_last_shot %= 128;
				g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;

				g_ctx.m_has_incremented_shots = true;
				

				data.m_snapshot[ g_ctx.m_last_shot ] = data;

				if( data.m_logged ) {
					if( !--data.m_logged_shots ) {
						data.m_logged = false;
					}
				}
				else {
					int shots = data.m_shots;

					increment_shots( ent_index );

#if _DEBUG
					if( dbg_resolver( ) ) {
						char str[ 128 ];
						sprintf_s< 128 >( str, "[\3moneybot\1] incrementing shots from %d to %d", shots, data.m_shots );

						g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
					}
#endif
				}
			}
		}
		else {
			matrix3x4 bone_matrix[ 128 ];
			if( model && hdr && set && ent->ce( )->SetupBones( bone_matrix, 128, 0x100, g_csgo.m_globals->m_curtime ) ) {
				auto box = set->pHitbox( hitbox );
				if( !box ) return;

				auto origin = ent->m_vecOrigin( );

				vec3_t shot_pos = position;
				vec3_t rotated_center;

				vec3_t offset = position - origin;
				float  radius = offset.length2d( );
				float  cos_ = offset.x / radius;

				float deg = RAD2DEG( acos( cos_ ) );
				if( offset.y < 0 ) deg = -deg;

				float rot = ( deg - 180.f ) * ( M_PI / 180.f );
				rotated_center.x = origin.x + cos( rot ) * radius;
				rotated_center.y = origin.y + sin( rot ) * radius;
				rotated_center.z = position.z;

				Ray_t ray;
				CTraceFilterOneEntity filter;
				CGameTrace tr_center;
				CGameTrace tr_rot;

				filter.ent = ent;

				ray.Init( g_ctx.m_local->get_eye_pos( ), position );
				g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr_center );

				ray.Init( g_ctx.m_local->get_eye_pos( ), rotated_center );
				g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr_rot );

				bool is_within = tr_center.hitbox == tr_rot.hitbox;

				if( !is_within ) {
					auto dist = box->bbmin.dist_to( box->bbmax );

					context::shot_data_t new_shot{ };
					new_shot.m_angle = angle;
					new_shot.m_enemy_pos = position;
					new_shot.m_enemy_index = ent_index;
					new_shot.m_local_pos = g_ctx.m_local->get_eye_pos( );
					new_shot.m_resolver_shots = data.m_shots;
					new_shot.m_resolver_state = true;
					new_shot.m_hitbox.min = min;
					new_shot.m_hitbox.min = max;
					new_shot.m_hitbox.radius = radius;
					new_shot.m_missed = true;
					new_shot.m_hitgroup = util::hitbox_to_hitgroup( hitbox );

					g_ctx.m_last_shot++;
					g_ctx.m_last_shot %= 128;
					g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;

					g_ctx.m_has_incremented_shots = true;

					if( data.m_logged ) {
						if( !--data.m_logged_shots ) {
							data.m_logged = false;
						}
					}
					else {
						int shots = data.m_shots;
						increment_shots( ent_index );
						
#if _DEBUG
						if( dbg_resolver( ) ) {
							char str[ 128 ];
							sprintf_s< 128 >( str, "[\3moneybot\1] incrementing shots from %d to %d", shots, data.m_shots );

							g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
						}
#endif
					}

					data.m_snapshot[ g_ctx.m_last_shot ] = data;
				}
			}
		}

		//printf( "resolver: logging aimbot shot %d missed: %d state: %d time: %f\n",
		//	data.m_shots, data.m_missed_shots[ data.m_breaking ], data.m_breaking, g_Interfaces.Globals->curtime );
	}

	void c_ragebot::c_resolver::increment_shots( int ent_index ) {
		m_data[ ent_index ].m_shots++;
		m_data[ ent_index ].m_missed_shots++;
		m_data[ ent_index ].m_angle_change = true;
	}

	void c_ragebot::c_resolver::on_missed_spread( int ent_index, int shots ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots % 150 ];

		data.m_shots = snapshot.m_shots;

		if( snapshot.m_logged ) {
			data.m_logged = true;
			data.m_logged_shots = snapshot.m_logged_shots;
		}
	}

	void c_ragebot::c_resolver::listener( int ent_index, int shots ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots ];

		auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
		if( !nci )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon )
			return;

		auto rate = weapon->get_wpn_info( )->cycle_time;

		data.m_shots = snapshot.m_shots;
		data.m_logged = true;

		int latency_shots = ( nci->GetLatency( 0 ) + g_csgo.m_globals->m_frametime ) / rate + 1;

		data.m_logged_shots = latency_shots;

#if _DEBUG
		if( dbg_resolver( ) ) {
			char str[ 128 ];
			sprintf_s< 128 >( str, "[\3moneybot\1] resolver: logging %d shots for %d with %d", data.m_logged_shots, ent_index, data.m_shots );
		
			g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
		}
#endif
	}

	void c_ragebot::c_resolver::update_player( int i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity( i );
		bool was_invalid = m_data[ i ].m_was_invalid;

		m_data[ i ].update( i );
		if( g_settings.rage.resolver_override( ) ) {
			if( g_input.is_key_pressed( g_settings.rage.resolver_override_key( ) ) ) {
				int dir = manual_override( ent );
				if( dir ) {
					if( dir != m_data[ i ].m_overriding )
						m_data[ i ].m_angle_change = true;

					//to be finished
					m_data[ i ].m_overriding = dir;
					return;
				}
			}
		}

		m_data[ i ].m_overriding = 0;
		brute_force( ent );
	}

	void c_ragebot::c_resolver::frame_stage_notify( ) {
		if( !g_settings.rage.enabled( ) || !g_settings.rage.resolver( ) )
			return;

		if( !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) )
			return;

		for( int i{ 1 }; i < g_csgo.m_globals->m_maxclients; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if( !ent || ent == g_ctx.m_local || ent->ce( )->IsDormant( ) || !ent->is_player( )
				|| !ent->is_alive( ) || !g_cheat.m_player_mgr.is_cheater( i ) ) {
				m_data[ i ].m_was_invalid = true;
				m_data[ i ].m_has_valid_lby = false;
				continue;
			}

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
				continue;

			update_player( i );
		}
	}

	void c_ragebot::c_resolver::force_yaw( c_base_player* ent, float yaw ) {
		ent->m_angEyeAngles( ).y = yaw;// + 180.f;
	}

	int c_ragebot::c_resolver::manual_override( c_base_player* ent ) {
		if( ent->m_vecVelocity( ).length( ) > 0.1f && !ent->is_fakewalking( ) ) { //no point in overriding moving targets, it will also fuck up backtrack records otherwise
			return OVERRIDE_NONE;
		}

		vec3_t	pos;
		vec3_t	angle;
		vec3_t	local_angle;
		vec3_t	local_pos;
		float	delta;
		float	dist;


		local_pos = g_ctx.m_local->m_vecOrigin( );
		g_csgo.m_engine( )->GetViewAngles( local_angle );
		pos = ent->get_hitbox_pos( 0 );

		angle = math::vector_angles( local_pos, pos );
		dist = pos.dist_to( local_pos );
		delta = ( angle - local_angle ).clamp( ).y;
		float yaw_delta = sin( DEG2RAD( ( angle - local_angle ).clamp( ).y ) ) * dist;

		if( std::abs( yaw_delta ) < 25.f ) {
			force_yaw( ent, angle.y );
			return OVERRIDE_CENTER;
		}
		else {
			force_yaw( ent, angle.y + ( ( delta < 0.f ) ? 90.f : -90.f ) );
			return delta > 0.f ? OVERRIDE_LEFT : OVERRIDE_RIGHT;
		}
	}

	void c_ragebot::c_resolver::force_lby( c_base_player* ent ) {
		force_yaw( ent, ent->m_flLowerBodyYawTarget( ) );
	}

	void c_ragebot::c_resolver::brute_force( c_base_player* ent ) {
		if( ent->m_flSimulationTime( ) == g_cheat.m_ragebot.m_lagcomp->get_last_updated_simtime( ent->ce( )->GetIndex( ) ) )
			return;

		if( ent->m_vecVelocity( ).length( ) > 0.1f && ( ent->m_fFlags( ) & FL_ONGROUND ) && !ent->is_fakewalking( ) ) {
			m_data[ ent->ce( )->GetIndex( ) ].m_last_moving_lby = ent->m_flLowerBodyYawTarget( );
			m_data[ ent->ce( )->GetIndex( ) ].m_has_valid_lby = true;
			force_lby( ent );
			return;
		}

		int i = ent->ce( )->GetIndex( );
		auto& data = m_data[ i ];
		float start = ent->m_flLowerBodyYawTarget( );
		float new_yaw{ };
		float new_ang{ };
		int shots{ };

		shots = data.m_shots;
		new_ang = get_shot_yaw( shots, i );
		float ang;
		if( g_ctx.run_frame( ) ) {
			auto local_pos = g_ctx.m_local->m_vecOrigin( );
			auto enemy_pos = ent->m_vecOrigin( );

			ang = math::vector_angles( enemy_pos, local_pos ).y;
		}
		else {
			ang = start;
		}

		float pitch = ent->m_angEyeAngles( ).x;

		if( g_settings.rage.pitch_resolver ) {
			if( new_ang == FLT_ANG_MOVING_LBY_UP )
				ent->m_angEyeAngles( ).x = -89.f;
			else
				ent->m_angEyeAngles( ).x = 89.f;
		}

		if( new_ang == FLT_ANG_LBY )
			new_yaw = ent->m_flLowerBodyYawTarget( );
		else if( new_ang == FLT_ANG_MOVING_LBY_UP ) {
			if( !data.m_has_valid_lby && !g_settings.rage.pitch_resolver && ( ent->m_fFlags( ) & FL_ONGROUND ) ) {
				data.m_shots++;
				return brute_force( ent );
			}

			auto delta = std::remainderf( data.m_last_moving_lby - data.m_last_lby, 360.f );
			bool breaking_lby = std::abs( delta ) > 35.f;

			new_yaw = breaking_lby ? data.m_last_moving_lby : data.m_last_lby;
		}
		else if( new_ang == FLT_ANG_180 ) {
			if( data.m_last_freestanding == -1 && ( ent->m_fFlags( ) & FL_ONGROUND ) ) {
				data.m_shots++;
				return brute_force( ent );
			}

			new_yaw = ang - 180.f;
		}
		else if( new_ang == FLT_ANG_FREESTANDING ) {
			int freestanding = try_freestanding( ent );
			if( freestanding == -1 )
				new_yaw = ang + 180.f;
			else
				new_yaw = ang + ( freestanding ? 90.f : -90.f );

			if( data.m_last_freestanding != freestanding )
				data.m_angle_change = true;

			data.m_last_freestanding = freestanding;
		}
		else if( new_ang == FLT_ANG_MINUS_90 ) {
			new_yaw = ang - 90.f;
		}
		else if( new_ang == FLT_ANG_90 ) {
			new_yaw = ang + 90.f;
		}
		else if( new_ang == FLT_ANG_135 )
			new_yaw = ang + 135.f;
		else if( new_ang == FLT_ANG_MINUS_135 )
			new_yaw = ang - 135.f;
		else
			new_yaw = std::remainderf( start + new_ang, 360.f );

		new_yaw = std::remainderf( new_yaw, 360.f );

		force_yaw( ent, new_yaw );
	}
}