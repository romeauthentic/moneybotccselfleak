#include "ragebot.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "math.hpp"
#include "input_system.hpp"

namespace features
{
	constexpr float FLT_ANG_LBY = 360.f;
	constexpr float FLT_ANG_MOVING_LBY = -360.f;
	constexpr float FLT_ANG_180 = 720.f;
	constexpr float FLT_ANG_90 = -720.f;
	constexpr float FLT_ANG_MINUS_90 = -480.f;

	static const std::vector< float > possible_angles_none = {
		FLT_ANG_LBY,
		FLT_ANG_MOVING_LBY,
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		180.f
	};

	static const std::vector< float > possible_angles_adjust = {
		FLT_ANG_MOVING_LBY,
		180.f,
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		-130.f,
		130.f,
	};

	static const std::vector< float > possible_angles_noadjust = {
		FLT_ANG_MOVING_LBY,
		110.f,
		FLT_ANG_180,
		FLT_ANG_90,
		FLT_ANG_MINUS_90,
		90.f,
		-90.f
	};

	bool dbg_resolver( ) {
		static con_var< bool > var{ &data::holder_, fnv( "dbg_resolver" ), false };
		return var( );
	}

	c_ragebot::c_resolver::resolve_log_t::resolve_log_t( ) {
		auto register_vec = [ ]( c_ragebot::c_resolver::angle_vec_t* vec, const std::vector< float >& vec1 ) {
			vec->clear( );

			for( auto& it : vec1 ) {
				vec->push_back( it );
			}
		};

		register_vec( &m_logged_angles[ 0 ], possible_angles_none );
		register_vec( &m_logged_angles[ 1 ], possible_angles_adjust );
		register_vec( &m_logged_angles[ 2 ], possible_angles_noadjust );
	}

	void c_ragebot::c_resolver::resolve_log_t::update( int ent_index ) {
		static C_AnimationLayer last_layer[ 65 ]{ };
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		if( !ent || ent == g_ctx.m_local || !ent->is_player( ) || !ent->is_alive( ) || ent->ce( )->IsDormant( ) ) {
			m_was_invalid = true;
			return;
		}

		if( !( ent->m_fFlags( ) & FL_ONGROUND ) ) {
			m_breaking = BREAKING_NONE;
			return;
		}

		//they moved since the last time they were dormant - reset the data
		if( m_was_invalid && m_last_pos.dist_to( ent->m_vecOrigin( ) ) > 4.f ) {
			m_last_pos = ent->m_vecOrigin( );
			reset( );
		}

		m_was_invalid = false;
		m_shots %= 100;
		float lby = ent->m_flLowerBodyYawTarget( );
		if( lby != m_last_lby ) {
			m_last_lby = lby;
			m_last_update = ent->m_flSimulationTime( );
			m_breaking = BREAKING_NONE;
		}

		//filter out the first lby update
		if( ent->m_flSimulationTime( ) - m_last_update > 0.1f ) {
			C_AnimationLayer* adjust_layer = &ent->m_AnimOverlay( ).GetElements( )[ 3 ];
			C_AnimationLayer* prev_layer = &last_layer[ ent_index ];

			auto cur_seq = ent->get_seq_activity( adjust_layer->m_nSequence );
			auto old_seq = ent->get_seq_activity( prev_layer->m_nSequence );

			if( old_seq == 979 && cur_seq == 979 && ( adjust_layer->m_flCycle != prev_layer->m_flCycle
				|| adjust_layer->m_flWeight == 1.f ) ) {
				m_breaking = BREAKING_ADJUST;
				m_last_adjust = ent->m_flSimulationTime( );
			}
			else if( ent->m_flSimulationTime( ) - m_last_adjust > TICKS_TO_TIME( 14 ) ) {
				m_breaking = BREAKING_NOADJUST;
			}

			*prev_layer = *adjust_layer;
		}
		else {
			m_breaking = BREAKING_NONE;
		}
	}

	void c_ragebot::c_resolver::resolve_log_t::reset( ) {
		//assuming theyre STILL not moving this shouldnt change
		//however if they are, it will be reset to none anyway
		m_breaking = BREAKING_NONE;
		m_last_adjust = 0.f;
		m_last_update = 0.f;
		m_last_lby = 0.f;
		m_missed_shots = 0;
	}

	void c_ragebot::c_resolver::resolve_log_t::reset_angles( ) {
		std::vector< float > ang_vec;
		auto& cur_vec = m_logged_angles[ m_breaking ];

		switch( m_breaking ) {
		case BREAKING_NONE:
			ang_vec = possible_angles_none;
			break;
		case BREAKING_NOADJUST:
			ang_vec = possible_angles_noadjust;
			break;
		case BREAKING_ADJUST:
			ang_vec = possible_angles_adjust;
			break;
		}

		cur_vec.clear( );
		for( auto& it : ang_vec ) cur_vec.emplace_back( it );
	}

	void c_ragebot::c_resolver::resolve_log_t::blacklist_shot( int shots ) {
		auto& angles = m_logged_angles[ m_breaking ];

		if( angles.empty( ) ) {
			reset_angles( );
		}

		shots %= angles.size( );

		if( dbg_resolver( ) )
			g_con->log( "blacklisting angle: %f", angles.at( shots ).ang );

		angles.at( shots ).missed = true;
	}

	float c_ragebot::c_resolver::get_shot_yaw( int shots, int ent_index ) {
		auto& data = m_data[ ent_index ];
		auto  ang_vec = &data.m_logged_angles[ data.m_breaking ];

		std::vector< float > angles;

		for( size_t i{ }; i < ang_vec->size( ); ++i ) {
			if( !ang_vec->at( i ).missed ) {
				angles.push_back( ang_vec->at( i ).ang );
			}
		}

		if( angles.empty( ) ) {
			data.reset_angles( );
			return ang_vec->at( 0 ).ang;
		}

		if( angles.size( ) <= 1 ) {
			return angles.at( 0 );
		}

		return angles[ shots % angles.size( ) ];
	}

	void c_ragebot::c_resolver::aimbot( int ent_index, int hitbox, vec3_t angle, vec3_t position ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];

		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		bool moving = ent->m_vecVelocity( ).length2d( ) > 0.1f && ( ent->m_fFlags( ) & FL_ONGROUND );

		if( moving && !ent->is_fakewalking( ) ) {
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
				new_shot.m_hitbox_radius = dist;
				new_shot.m_missed = true;
				new_shot.m_hitgroup = HITGROUP_HEAD;

				g_ctx.m_last_shot++;
				g_ctx.m_last_shot %= 128;
				g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;

				g_ctx.m_has_incremented_shots = true;
				data.m_snapshot[ g_ctx.m_last_shot ] = data;

				increment_shots( ent_index );
				data.blacklist_shot( data.m_shots );
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
					new_shot.m_hitbox_radius = dist;
					new_shot.m_missed = true;
					new_shot.m_hitgroup = util::hitbox_to_hitgroup( hitbox );

					g_ctx.m_last_shot++;
					g_ctx.m_last_shot %= 128;
					g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;

					g_ctx.m_has_incremented_shots = true;
					data.m_snapshot[ g_ctx.m_last_shot ] = data;

					increment_shots( ent_index );
					data.blacklist_shot( data.m_shots );
				}
			}
		}
		//printf( "resolver: logging aimbot shot %d missed: %d state: %d time: %f\n",
		//	data.m_shots, data.m_missed_shots[ data.m_breaking ], data.m_breaking, g_Interfaces.Globals->curtime );
	}

	void c_ragebot::c_resolver::on_missed_spread( int ent_index, int shots ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots ];

		auto& shot_vec = data.m_logged_angles[ snapshot.m_breaking ];

		if( shot_vec.empty( ) ) {
			data.reset_angles( );
		}

		if( shot_vec.size( ) <= 1 ) {
			shot_vec.at( 0 ).missed = false;
		}
		else
			shot_vec.at( snapshot.m_shots % shot_vec.size( ) ).missed = false;


		if( dbg_resolver( ) )
			g_con->log( "shot missed due to spread: %f", shot_vec.size( ) <= 1 ? shot_vec.at( 0 ).ang : shot_vec.at( snapshot.m_shots % shot_vec.size( ) ).ang );

	}

	void c_ragebot::c_resolver::listener( int ent_index, int shots ) {
		if( !g_cheat.m_player_mgr.is_cheater( ent_index ) )
			return;

		auto& data = m_data[ ent_index ];
		auto& snapshot = data.m_snapshot[ shots ];

		auto& shot_vec = data.m_logged_angles[ snapshot.m_breaking ];

		if( shot_vec.empty( ) ) {
			data.reset_angles( );
		}

		if( shot_vec.size( ) <= 1 ) {
			shot_vec.at( 0 ).missed = false;
		}
		else
			shot_vec.at( snapshot.m_shots % shot_vec.size( ) ).missed = false;

		data.m_shots = snapshot.m_shots;

		if( dbg_resolver( ) )
			g_con->log( "hit angle: %f", shot_vec.size( ) <= 1 ? shot_vec.at( 0 ).ang : shot_vec.at( snapshot.m_shots % shot_vec.size( ) ).ang );

	}

	void c_ragebot::c_resolver::frame_stage_notify( ) {
		if( !g_settings.rage.enabled( ) || !g_settings.rage.resolver( ) )
			return;

		for( int i{ 1 }; i < g_csgo.m_globals->m_maxclients; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if( !ent || ent == g_ctx.m_local || ent->ce( )->IsDormant( )
				|| !ent->is_alive( ) || !g_cheat.m_player_mgr.is_cheater( i ) ) {
				m_data[ i ].m_was_invalid = true;
				continue;
			}

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
				continue;

			m_data[ i ].update( i );
			if( g_settings.rage.resolver_override( ) ) {
				if( g_input.is_key_pressed( g_settings.rage.resolver_override_key( ) ) ) {
					int dir = manual_override( ent );
					if( dir ) {
						//to be finished
						m_data[ i ].m_overriding = dir;
						continue;
					}
				}
			}

			m_data[ i ].m_overriding = 0;
			brute_force( ent );
		}
	}

	void c_ragebot::c_resolver::force_yaw( c_base_player* ent, float yaw ) {
		//could be used to just store resolved yaw on entities
		//so then this data can be used in CM
		ent->m_angEyeAngles( ).y = yaw;// + 180.f;

		//ent->ce( )->GetRenderAngles( ).y = yaw;
		//auto new_ang = ent->get< vec3_t >( 0xc4 );
		//new_ang.y = yaw;

		//ent->set_abs_angles( new_ang );
	}

	int c_ragebot::c_resolver::manual_override( c_base_player* ent ) {
		if( ent->m_vecVelocity( ).length2d( ) > 35.f ) { //no point in overriding moving targets, it will also fuck up backtrack records otherwise
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
		if( ent->m_vecVelocity( ).length( ) > 0.1f && !ent->is_fakewalking( ) ) {
			m_data[ ent->ce( )->GetIndex( ) ].m_last_moving_lby = ent->m_flLowerBodyYawTarget( );
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

			ang = math::vector_angles( enemy_pos, local_pos );
		}
		else {
			ang = start;
		}

		if( new_ang == FLT_ANG_LBY )
			new_yaw = ent->m_flLowerBodyYawTarget( );
		else if( new_ang == FLT_ANG_MOVING_LBY )
			new_yaw = data.m_last_moving_lby;
		else if( new_ang == FLT_ANG_180 )
			new_yaw = ang - 180.f;
		else if( new_ang == FLT_ANG_90 )
			new_yaw = ang + 90.f;
		else if( new_ang == FLT_ANG_MINUS_90 )
			new_yaw = ang - 90.f;
		else
			new_yaw = std::remainderf( start + new_ang, 360.f );

		force_yaw( ent, new_yaw );
	}
}