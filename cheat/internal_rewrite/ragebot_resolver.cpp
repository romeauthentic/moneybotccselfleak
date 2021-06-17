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
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		FLT_ANG_MINUS_135,
		FLT_ANG_135,
	};

	static const std::vector< float > possible_angles_normal = {
		FLT_ANG_180,
		FLT_ANG_135,
		FLT_ANG_MINUS_135
	};

	static const std::vector< float > possible_angles_freestanding = {
		0.f,
		90.f,
		45.f,
		135.f,
		180.f,
	};

	bool dbg_resolver( ) {
		static con_var< bool > var{ &data::holder_, fnv( "dbg_resolver" ), false };
		return var( );
	}

	int c_ragebot::c_resolver::try_freestanding( c_base_player* ent ) {
		if ( !( ent->m_fFlags( ) & FL_ONGROUND ) )
			return -1;

		if ( g_ctx.m_past_origins.empty( ) )
			return -1; // sanity check

		int ret_dir = -1;
		float cur_damage = 0.f;

		vec3_t enemy_pos = ent->m_vecOrigin( );
		enemy_pos.z = ent->get_hitbox_pos( 0 ).z;

		auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
		float out_latency = nci->GetLatency( 0 );
		auto enemy_ping = ent->get_ping( ) * 0.001f;

		auto delay_ticks = std::clamp< int >( TIME_TO_TICKS( enemy_ping + out_latency ), 0, g_ctx.m_past_origins.size( ) - 1 );
		vec3_t delayed_pos = g_ctx.m_past_origins.at( delay_ticks );

		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );
		local_pos -= g_ctx.m_local->m_vecOrigin( );
		local_pos += delayed_pos;

		vec3_t aim_angle = math::vector_angles(
			enemy_pos,
			local_pos
		);

		auto get_damage = [ & ] ( const vec3_t& start, const vec3_t& end, c_base_player* a, c_base_player* b ) {
			static weapon_info_t wpn_data{ };
			wpn_data.damage = 200;
			wpn_data.range_modifier = 1.0f;
			wpn_data.penetration = 3.0f;
			wpn_data.armor_ratio = 0.5f;
			wpn_data.range = 8192.f;

			fire_bullet_data_t data{ };
			data.src = start;
			data.travel_range = ( end - data.src ).length( );
			data.traveled = 0.f;
			data.to_travel = data.travel_range;

			vec3_t angle = math::vector_angles( start, end );
			data.direction = math::angle_vectors( angle );

			data.direction.normalize_vector( );

			if ( g_cheat.m_autowall.fire_bullet( a, b, &wpn_data, data, false, false ) ) {
				return data.current_damage;
			}

			return -1.f;
		};

		auto trace_ent_pos = [ & ] ( vec3_t start, vec3_t end, c_base_player* ent ) {
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

		float local_dist = 75.f;
		float enemy_dist = 75.f;

		float start_dmg = get_damage( enemy_pos, local_pos, g_ctx.m_local, ent );

		auto test_dmg = [ & ] ( float dist, float enemy_dist, float delta = 50.f ) {
			vec3_t enemy_left = math::get_rotated_pos( enemy_pos, aim_angle.y - 90.f, enemy_dist );
			vec3_t enemy_right = math::get_rotated_pos( enemy_pos, aim_angle.y + 90.f, enemy_dist );

			enemy_left = trace_ent_pos( enemy_pos, enemy_left, ent );
			enemy_right = trace_ent_pos( enemy_pos, enemy_right, ent );

			vec3_t local_left = math::get_rotated_pos( local_pos, aim_angle.y + 90.f, dist );
			vec3_t local_right = math::get_rotated_pos( local_pos, aim_angle.y - 90.f, dist );

			local_left = trace_ent_pos( local_pos, local_left, g_ctx.m_local );
			local_right = trace_ent_pos( local_pos, local_right, g_ctx.m_local );

			float dmg_left = get_damage( local_left, enemy_left, g_ctx.m_local, ent )
				+ get_damage( local_right, enemy_left, g_ctx.m_local, ent );

			float dmg_right = get_damage( local_left, enemy_right, g_ctx.m_local, ent )
				+ get_damage( local_right, enemy_right, g_ctx.m_local, ent );

			if ( std::abs( dmg_left - dmg_right ) < delta )
				return false;

			float max_dmg = math::max( dmg_left, dmg_right );
			ret_dir = dmg_left > dmg_right;

			cur_damage = max_dmg;
			return max_dmg > delta;
		};

		float radius = std::floorf( ( g_ctx.m_local->get_hitbox_pos( 0 ) - g_ctx.m_local->get_hitbox_pos( HITBOX_PELVIS ) ).length2d( ) );
		radius = radius >= 15.f ? 20.f : 10.f;

		float enemy_radius = std::floorf( ( ent->get_hitbox_pos( 0 ) - ent->get_hitbox_pos( HITBOX_PELVIS ) ).length2d( ) );
		enemy_radius = enemy_radius >= 15.f ? 20.f : 10.f;

		if ( !test_dmg( enemy_radius + 1.f, radius + 1.f, 25.f ) ) {
			if ( cur_damage < start_dmg * 2.f )
				return -1;
		}

		bool found = false;

		for ( size_t i = 5; i > 0; --i ) {
			float cur_dist = local_dist / i;
			float cur_enemy_dist = enemy_dist / i;

			if ( cur_dist < radius )
				continue;

			if ( cur_enemy_dist < enemy_radius )
				continue;

			if ( test_dmg( cur_enemy_dist, cur_dist, 125.f / i ) ) {
				found = true;
				break;
			}
			else if ( cur_damage > 125.f / i )
				break;
		}

		if ( !found )
			return -1;

		return ret_dir;
	}

	void c_ragebot::c_resolver::resolve_log_t::update( int ent_index ) {
		static C_AnimationLayer last_layer[ 65 ]{ };
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		if ( !ent || ent == g_ctx.m_local || !ent->is_player( ) || !ent->is_alive( ) || ent->ce( )->IsDormant( ) || !ent->has_valid_anim( ) ) {
			m_was_invalid = true;
			m_has_valid_lby = false;
			return;
		}

		if ( m_was_invalid && m_last_pos.dist_to( ent->m_vecOrigin( ) ) > 4.f )
			reset( );

		if ( !( ent->m_fFlags( ) & FL_ONGROUND ) ) {
			m_state = R_NONE;
			return;
		}

		if ( ent->m_vecVelocity( ).length2d( ) > 0.1f ) {
			m_has_valid_lby = true;
			m_last_moving_lby = ent->m_flLowerBodyYawTarget( );

			m_state = R_MOVING;

			if ( !g_settings.rage.dbg_moving_resolver )
				return;

			float delta = ent->get_animstate( )->desync_delta( );
			float ang = ent->m_angEyeAngles( ).y;

			float desync_l = std::remainderf( ang + delta, 360.f );
			float desync_r = std::remainderf( ang - delta, 360.f );

			auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
			float out_latency = nci->GetLatency( 0 );
			auto enemy_ping = ent->get_ping( ) * 0.001f;

			auto delay_ticks = std::clamp< int >( TIME_TO_TICKS( enemy_ping + out_latency ), 0, g_ctx.m_past_origins.size( ) - 1 );
			vec3_t delayed_pos = g_ctx.m_past_origins.at( delay_ticks );

			float local_ang = math::vector_angles( ent->m_vecOrigin( ), delayed_pos ).y - 180.f;

			int freestanding = g_cheat.m_ragebot.m_resolver->try_freestanding( ent );
			float local_freestanding = ( freestanding == -1 ) ? local_ang : local_ang + ( freestanding == 1 ? 90.f : -90.f );

			float min_delta = 360.f;
			float best_ang = FLT_MAX;

			auto test_ang = [ & ] ( float ang ) {
				float ang_delta;
				ang_delta = std::abs( std::remainderf( ang + desync_l, 360.f ) );

				if ( ang_delta < min_delta ) {
					min_delta = ang_delta;
					best_ang = ang;
				}

				ang_delta = std::abs( std::remainderf( ang + desync_r, 360.f ) );

				if ( ang_delta < min_delta ) {
					min_delta = ang_delta;
					best_ang = ang;
				}
			};

			test_ang( local_ang );
			test_ang( local_freestanding );

			//if( min_delta < 15.f )
				//ent->m_angEyeAngles( ).y = best_ang;

			return;
		}

		m_last_pos = ent->m_vecOrigin( );

		m_was_invalid = false;

		int freestanding = g_cheat.m_ragebot.m_resolver->try_freestanding( ent );

		if ( freestanding != -1 ) {
			m_state = R_FREESTANDING;
		}
		else
			m_state = R_NORMAL;

		if ( m_last_freestanding != freestanding )
			m_angle_change = true;

		m_last_freestanding = freestanding;
		m_has_valid_lby = true;
	}

	void c_ragebot::c_resolver::resolve_log_t::reset( ) {
		//assuming theyre STILL not moving this shouldnt change
		//however if they are, it will be reset to none anyway
		m_state = R_NONE;
		m_last_update = 0.f;
		m_last_lby = 0.f;
		m_missed_shots = 0;
		memset( m_shots, 0, sizeof( m_shots ) );
		memset( m_logged, 0, sizeof( m_logged ) );
		memset( m_logged_shots, 0, sizeof( m_logged ) );
	}

	std::vector< float > c_ragebot::c_resolver::get_shot_vec( int ent ) {
		auto& data = m_data[ ent ];

		if ( data.m_state == R_NORMAL )
			return possible_angles_normal;
		if ( data.m_state == R_FREESTANDING )
			return possible_angles_freestanding;

		return possible_angles_none;
	}

	float c_ragebot::c_resolver::get_shot_yaw( int shots, int ent_index ) {
		auto& data = m_data[ ent_index ];
		auto  ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		auto vec = get_shot_vec( ent_index );

		float new_ang = vec.at( shots % vec.size( ) );
		float new_yaw = 0.f;

		float ang = math::vector_angles( ent->m_vecOrigin( ), g_ctx.m_last_origin ).y;

		if ( data.m_state == R_FREESTANDING ) {
			ang += ( data.m_last_freestanding == 1 ? 90.f : -90.f );

			float sign = data.m_last_freestanding == 1 ? 1.f : -1.f;

			return ang + new_ang * sign;
		}

		if ( new_ang == FLT_ANG_LBY )
			new_yaw = ent->m_flLowerBodyYawTarget( );
		else if ( new_ang == FLT_ANG_MOVING_LBY_UP ) {
			if ( !data.m_has_valid_lby && !g_settings.rage.pitch_resolver ) {
				return get_shot_yaw( ++get_resolver_shots( ent_index ), ent_index );
			}

			auto delta = std::remainderf( data.m_last_moving_lby - data.m_last_lby, 360.f );
			bool breaking_lby = std::abs( delta ) > 35.f;

			new_yaw = breaking_lby ? data.m_last_moving_lby : data.m_last_lby;
		}
		else if ( new_ang == FLT_ANG_180 ) {
			new_yaw = ang - 180.f;
		}
		else if ( new_ang == FLT_ANG_MINUS_90 ) {
			new_yaw = ang - 90.f;
		}
		else if ( new_ang == FLT_ANG_90 ) {
			new_yaw = ang + 90.f;
		}
		else if ( new_ang == FLT_ANG_135 )
			new_yaw = ang + 135.f;
		else if ( new_ang == FLT_ANG_MINUS_135 )
			new_yaw = ang - 135.f;
		else
			new_yaw = std::remainderf( ent->m_flLowerBodyYawTarget( ) + new_ang, 360.f );

		return new_yaw;
	}

	void c_ragebot::c_resolver::aimbot( int ent_index, int hitbox, vec3_t angle, vec3_t position, vec3_t min, vec3_t max, float radius ) {
		if ( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		bool moving = ent->get_anim_velocity( ).length2d( ) > 0.1f && ( ent->m_fFlags( ) & FL_ONGROUND );

		//if ( moving && !ent->is_fakewalking( ) ) {
			//return;
		//}

		if ( !data.m_state ) {
			increment_shots( ent_index );
			return;
		}

		auto model = ent->ce( )->GetModel( );
		auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
		auto set = hdr->pHitboxSet( ent->m_nHitboxSet( ) );

		auto do_rotate = [] ( int box ) {
			switch ( box ) {
				case HITBOX_HEAD:
				case HITBOX_LEFT_FOOT:
				case HITBOX_RIGHT_FOOT:
				case HITBOX_LEFT_HAND:
				case HITBOX_RIGHT_HAND:
				case HITBOX_LEFT_CALF:
				case HITBOX_RIGHT_CALF:
					return true;

				default: return false;
			}
		};

		if ( do_rotate( hitbox ) ) {
			auto box = set->pHitbox( 0 );
			if ( box ) {
				auto dist = box->bbmin.dist_to( box->bbmax );

				context::shot_data_t new_shot{ };
				new_shot.m_angle = angle;
				new_shot.m_enemy_pos = position;
				new_shot.m_enemy_index = ent_index;
				new_shot.m_local_pos = g_ctx.m_local->get_eye_pos( );
				new_shot.m_resolver_shots = get_resolver_shots( ent_index );
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


				data.m_snapshot[ g_ctx.m_last_shot ] = data;

				if ( data.m_logged[ data.m_state ] ) {
					if ( !--data.m_logged_shots[ data.m_state ] ) {
						data.m_logged[ data.m_state ] = false;
					}
				}
				else {
					int shots = get_resolver_shots( ent_index );

					increment_shots( ent_index );

#if _DEBUG
					if ( dbg_resolver( ) ) {
						char str[ 128 ];
						sprintf_s< 128 >( str, "[\3moneybot\1] incrementing shots from %d to %d"
							"[\3moneybot\1] resolver state: %d\n", shots, get_resolver_shots( ent_index ), data.m_state );

						g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
					}
#endif
				}
			}
		}
		else {
			matrix3x4 bone_matrix[ 128 ];
			if ( model && hdr && set ) {
				memcpy( bone_matrix, ent->m_CachedBoneData( ).GetElements( ), sizeof( bone_matrix ) );

				auto box = set->pHitbox( hitbox );
				if ( !box ) return;

				auto origin = ent->m_vecOrigin( );

				vec3_t shot_pos = position;
				vec3_t rotated_center;

				bool is_within = false;
				for ( float rotation = -180.f; rotation <= 180.f; rotation += 45.f ) {
					float off = ( shot_pos - origin ).length2d( );
					float deg = ent->ce( )->GetRenderAngles( ).y;

					float rot = std::remainderf( deg + rotation, 360.f );
					if ( rot < 0.f )
						rot += 360.f;
					rotated_center.x = origin.x + cos( DEG2RAD( rot ) ) * off;
					rotated_center.y = origin.y + sin( DEG2RAD( rot ) ) * off;
					rotated_center.z = position.z;

					vec3_t ang = math::vector_angles( g_ctx.m_local->get_eye_pos( ),
						shot_pos );

					vec3_t dir = math::angle_vectors( ang );
					vec3_t trace_pos = g_ctx.m_local->get_eye_pos( ) + dir * 8192.f;

					Ray_t ray;
					CTraceFilterOneEntity filter;
					CGameTrace tr_center;
					CGameTrace tr_rot;

					filter.ent = ent;

					ray.Init( g_ctx.m_local->get_eye_pos( ), position );
					g_csgo.m_trace( )->TraceRay( ray, MASK_ALL, &filter, &tr_center );

					ray.Init( g_ctx.m_local->get_eye_pos( ), trace_pos );
					g_csgo.m_trace( )->TraceRay( ray, MASK_ALL, &filter, &tr_rot );

					is_within = ( tr_center.hitbox == tr_rot.hitbox ) && ( tr_rot.DidHit( ) );

					if ( !is_within )
						break;
				}

				if ( !is_within ) {
					auto dist = box->bbmin.dist_to( box->bbmax );

					context::shot_data_t new_shot{ };
					new_shot.m_angle = angle;
					new_shot.m_enemy_pos = position;
					new_shot.m_enemy_index = ent_index;
					new_shot.m_local_pos = g_ctx.m_local->get_eye_pos( );
					new_shot.m_resolver_shots = get_resolver_shots( ent_index );
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

					data.m_snapshot[ g_ctx.m_last_shot ] = data;

					if ( data.m_logged[ data.m_state ] ) {
						if ( !--data.m_logged_shots[ data.m_state ] ) {
							data.m_logged[ data.m_state ] = false;
						}
					}
					else {
						int shots = get_resolver_shots( ent_index );
						increment_shots( ent_index );

#if _DEBUG
						if ( dbg_resolver( ) ) {
							char str[ 128 ];
							sprintf_s< 128 >( str, "[\3moneybot\1] incrementing shots from %d to %d"
								"[\3moneybot\1] resolver state: %d\n", shots, get_resolver_shots( ent_index ), data.m_state );

							g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
						}
#endif
					}
				}
			}
		}

		//printf( "resolver: logging aimbot shot %d missed: %d state: %d time: %f\n",
		//	data.m_shots, data.m_missed_shots[ data.m_breaking ], data.m_breaking, g_Interfaces.Globals->curtime );
	}

	void c_ragebot::c_resolver::increment_shots( int ent_index ) {
		get_resolver_shots( ent_index )++;
		m_data[ ent_index ].m_missed_shots++;
		m_data[ ent_index ].m_angle_change = true;
	}

	void c_ragebot::c_resolver::on_missed_spread( int ent_index, int shots ) {
		if ( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots % 150 ];

		data.m_shots[ snapshot.m_state ] = snapshot.m_shots[ snapshot.m_state ];

		if ( snapshot.m_logged[ snapshot.m_state ] ) {
			data.m_logged[ snapshot.m_state ] = true;
			data.m_logged_shots[ snapshot.m_state ] = snapshot.m_logged_shots[ snapshot.m_state ];
		}
	}

	void c_ragebot::c_resolver::listener( int ent_index, int shots ) {
		if ( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots ];

		auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
		if ( !nci )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if ( !weapon )
			return;

		auto rate = weapon->get_wpn_info( )->cycle_time;

		data.m_shots[ snapshot.m_state ] = snapshot.m_shots[ snapshot.m_state ];
		data.m_logged[ snapshot.m_state ] = true;

		int latency_shots = ( nci->GetLatency( 0 ) + g_csgo.m_globals->m_frametime ) / rate + 1;

		data.m_logged_shots[ snapshot.m_state ] = latency_shots;

#if _DEBUG
		if ( dbg_resolver( ) ) {
			char str[ 128 ];
			sprintf_s< 128 >( str, "[\3moneybot\1] resolver: logging %d shots for %d with %d", data.m_logged_shots[ data.m_state ], ent_index, get_resolver_shots( ent_index ) );

			g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, str );
		}
#endif
	}

	void c_ragebot::c_resolver::update_player( int i ) {
		if ( !g_settings.rage.enabled( ) || !g_settings.rage.resolver( ) )
			return;

		if ( !g_cheat.m_player_mgr.is_cheater( i ) )
			return;

		auto ent = g_csgo.m_entlist( )->GetClientEntity( i );
		bool was_invalid = m_data[ i ].m_was_invalid;

		//m_data[ i ].update( i );
		if ( g_settings.rage.resolver_override( ) ) {
			if ( g_input.is_key_pressed( g_settings.rage.resolver_override_key( ) ) ) {
				int dir = manual_override( ent );
				if ( dir ) {
					if ( dir != m_data[ i ].m_overriding )
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
		if ( !g_settings.rage.enabled( ) || !g_settings.rage.resolver( ) )
			return;

		if ( !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) )
			return;

		for ( int i{ 1 }; i < g_csgo.m_globals->m_maxclients; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if ( !ent || ent == g_ctx.m_local || ent->ce( )->IsDormant( ) || !ent->is_player( )
				|| !ent->is_alive( ) || !g_cheat.m_player_mgr.is_cheater( i ) ) {
				m_data[ i ].m_was_invalid = true;
				m_data[ i ].m_has_valid_lby = false;
				continue;
			}

			if ( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
				continue;

			update_player( i );
		}
	}

	void c_ragebot::c_resolver::force_yaw( c_base_player* ent, float yaw ) {
		ent->m_angEyeAngles( ).y = yaw;// + 180.f;
	}

	int c_ragebot::c_resolver::manual_override( c_base_player* ent ) {
		if ( ent->get_animdata( ).m_anim_velocity.length2d( ) > 0.1f && !ent->is_fakewalking( ) ) { //no point in overriding moving targets, it will also fuck up backtrack records otherwise
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

		if ( std::abs( yaw_delta ) < 25.f ) {
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
		//if ( ent->m_flSimulationTime( ) == g_cheat.m_ragebot.m_lagcomp->get_last_updated_simtime( ent->ce( )->GetIndex( ) ) )
			//return;
		/*
		if ( ent->get_anim_velocity( ).length( ) > 0.1f && ( ent->m_fFlags( ) & FL_ONGROUND ) && !ent->is_fakewalking( ) ) {
			m_data[ ent->ce( )->GetIndex( ) ].m_last_moving_lby = ent->m_flLowerBodyYawTarget( );
			m_data[ ent->ce( )->GetIndex( ) ].m_has_valid_lby = true;
			force_lby( ent );
			return;
		}
		*/
		int i = ent->ce( )->GetIndex( );
		auto vec = get_shot_vec( i );

		float yaw_delta{};
		int freestanding = try_freestanding( ent );

		switch ( get_resolver_shots( i ) ) {
			case 0: // force towards closest wall
				if ( freestanding != -1 ) {
					yaw_delta = ( freestanding == 0 ) ? ent->get_animstate( )->desync_delta( ) : -ent->get_animstate( )->desync_delta( );
					break;
				}
				else {
					get_resolver_shots( i )++;
				}

			case 1: // force away from closest wall
				if ( freestanding != -1 ) {
					yaw_delta = ( freestanding == 0 ) ? -ent->get_animstate( )->desync_delta( ) : ent->get_animstate( )->desync_delta( );
					break;
				}
				else {
					get_resolver_shots( i )++;
				}

			case 2: // force left
				yaw_delta = ent->get_animstate( )->desync_delta( );
				break;

			case 3: // force right
				yaw_delta = -ent->get_animstate( )->desync_delta( );
				break;

			case 4: // "fake-desync" left
				yaw_delta = ent->get_animstate( )->desync_delta( ) / 2;
				break;

			case 5: // "fake-desync" right
				yaw_delta = -ent->get_animstate( )->desync_delta( ) / 2;
				break;

			default: // fuck it
				get_resolver_shots( i ) = 0;
				yaw_delta = 0.0f;
				break;
		}

		auto records = g_cheat.m_ragebot.m_lagcomp->get_records( i, RECORD_NORMAL );
		if ( records->empty( ) )
			return;

		ent->get_animstate( )->m_flGoalFeetYaw = records->front().m_flLowerBodyYawTarget + yaw_delta;

		ent->invalidate_bone_cache( );
		ent->ce( )->SetupBones( nullptr, -1, BONE_USED_BY_ANYTHING, records->front().m_flSimulationTime );
		ent->m_bClientSideAnimation( ) = true;
	}
}