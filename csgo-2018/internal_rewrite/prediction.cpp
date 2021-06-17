#include "prediction.hpp"
#include "hooks.hpp"
#include "context.hpp"

NAMESPACE_REGION( features )

void c_prediction::player_data_t::update( int ent_index ) {
	auto player = g_csgo.m_entlist( )->GetClientEntity( ent_index );
	if( !player || !player->is_valid( ) || player == g_ctx.m_local ) {
		m_valid = false;
		return;
	}

	float simtime = player->m_flSimulationTime( );

	if( std::abs( simtime - m_simtime ) ) {
		float delta = std::abs( m_simtime - simtime );
		if( !m_valid ) {
			m_old_velocity = player->m_vecVelocity( );
		}
		else {
			m_last_choke = TIME_TO_TICKS( delta );
			m_old_velocity = m_velocity;
			m_breaking_lc = player->m_vecOrigin( ).dist_to( m_position ) > 64;
		}

		m_velocity = player->get_animdata( ).m_last_velocity;
		m_position = player->m_vecOrigin( );

		m_valid = true;

		if( m_breaking_lc )
			m_records.push_front( { m_velocity, m_last_choke } );
	}

	m_simtime = simtime;

	while( m_records.size( ) > 32 )
		m_records.pop_back( );
}

void c_prediction::frame_stage_notify( ) {
	for( int i{ 1 }; i < 65; ++i ) {
		m_players[ i ].update( i );
	}
}

int c_prediction::get_predicted_choke( int idx ) {
	auto& data = m_players[ idx ];

	int adaptive_dtc = 0;
	int static_dtc	 = 0;
	int min_choke	 = 16;
	int max_choke	 = 0;

	if( !data.m_records.empty( ) ) {
		int prev_choke = data.m_last_choke;
		float last_dist = 0.f;
		for( auto& it : data.m_records ) {
			if( it.m_tick == prev_choke )
				static_dtc++;

			float speed = it.m_velocity.length2d( );
			float dist = speed * it.m_tick * TICK_INTERVAL( );

			if( dist > 62.f && std::abs( last_dist - dist ) < 12.f )
				adaptive_dtc++;

			prev_choke = it.m_tick;
			last_dist = dist;
			min_choke = math::min( it.m_tick, min_choke );
			max_choke = math::max( it.m_tick, max_choke );
		}
	}

	if( adaptive_dtc > 10 ) {
		return TIME_TO_TICKS( 64.f / data.m_velocity.length2d( ) ) + 1;
	}
	else if( static_dtc > 16 || data.m_records.empty( ) ) {
		return data.m_last_choke;
	}

	float factor = math::random_number( 0.f, 1.f );
	return ( 1.0f - factor ) * min_choke + factor * max_choke;
}

vec3_t c_prediction::aimware_extrapolate( c_base_player* ent, vec3_t origin, vec3_t& velocity ) {
	static auto sv_jump_impulse = g_csgo.m_cvar( )->FindVar( xors( "sv_jump_impulse" ) );
	static auto sv_gravity = g_csgo.m_cvar( )->FindVar( xors( "sv_gravity" ) );
	
	auto min = ent->m_vecMins( );
	auto max = ent->m_vecMaxs( );

	auto start = origin;
	auto end = start + velocity * TICK_INTERVAL( );

	CTraceFilter f;
	CGameTrace tr;
	Ray_t ray;
	
	ray.Init( start, end, min, max );
	f.pSkip = ent;

	g_csgo.m_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &f, &tr );

	if( tr.fraction != 1.f ) {
		for( int i{ }; i < 2; ++i ) {
			velocity -= tr.plane.normal * velocity.dot( tr.plane.normal );

			auto dot = velocity.dot( tr.plane.normal );
			if( dot < 0.f ) {
				velocity -= dot * tr.plane.normal;
			}

			end = tr.endpos + ( velocity * TICK_INTERVAL( ) * ( 1.f - tr.fraction ) );
			ray.Init( tr.endpos, end, min, max );
			g_csgo.m_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &f, &tr );

			if( tr.fraction == 1.f )
				break;
		}
	}

	end = tr.endpos;
	end.z -= 2.f;

	ray.Init( tr.endpos, end, min, max );
	g_csgo.m_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &f, &tr );

	if( tr.fraction != 1.f && tr.plane.normal.z > 0.7f ) {
		velocity.z = sv_jump_impulse->get_float( );
	}
	else {
		velocity.z -= sv_gravity->get_float( ) * TICK_INTERVAL( );
	}

	return tr.endpos;
}

vec3_t c_prediction::full_walk_move( c_base_player* player, int ticks ) {
	auto index = player->ce( )->GetIndex( );
	auto data = get_player_data( index );
	vec3_t origin = data.m_position;
	vec3_t velocity = data.m_velocity;
	vec3_t old_velocity = data.m_velocity;
	vec3_t acceleration;

	bool is_on_ground = player->m_fFlags( ) & FL_ONGROUND;

	m_player = data;

	if( !data.m_valid )
		return origin;

	float velocity_dir = RAD2DEG( atan2( velocity.y, velocity.x ) );
	float angle_dir = velocity_dir - RAD2DEG( atan2( data.m_old_velocity.y, data.m_old_velocity.x ) );

	angle_dir *= TICKS_TO_TIME( data.m_last_choke );

	if( velocity_dir <= 180.f ) {
		if( velocity_dir < -180.f )
			velocity_dir += 360.f;
	}
	else {
		velocity_dir -= 360.f;
	}

	float length = velocity.length2d( );

	for( int i{ }; i < ticks; ++i ) {
		float extrapolated_dir = velocity_dir + angle_dir;

		velocity.x = cos( DEG2RAD( extrapolated_dir ) ) * length;
		velocity.y = sin( DEG2RAD( extrapolated_dir ) ) * length; // hey.... fix please In Fucking Correct

		start_gravity( player, origin, velocity );

		if( is_on_ground ) {
			check_jump_button( player, origin, velocity ); // Won't jump all the time
		}

		if( is_on_ground ) {
			//velocity.z = 0.f; go to hell
			friction( player, origin, velocity );
		}

		check_velocity( player, origin, velocity );

		// fuck walking
		if( !is_on_ground ) {
			air_move( player, origin, velocity, old_velocity, acceleration );
		}

		try_player_move( player, origin, velocity );

		is_on_ground = categorize_position( player, origin, velocity ); 

		check_velocity( player, origin, velocity );

		finish_gravity( player, origin, velocity );

		if( is_on_ground ) {
			velocity.z = 0.f;
		}

		old_velocity = velocity;

		velocity_dir = extrapolated_dir;
	}

	return origin;
}

void c_prediction::check_jump_button( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	static auto sv_jump_impulse = g_csgo.m_cvar( )->FindVar( xors( "sv_jump_impulse" ) );
	float ground_factor = 1.f; 
	vec3_t ground_point = origin;

	ground_point.z -= 2.f;

	CGameTrace pm;
	trace_player_bbox( player, origin, ground_point, &pm );

	if( pm.m_pEnt ) {
		auto surface_data = g_csgo.m_phys_props( )->GetSurfaceData( pm.surface.surfaceProps );
		if( surface_data ) {
			ground_factor = surface_data->game.jumpfactor;
		}
	}


	if( !ground_factor ) {
		ground_factor = 1.f;
	}

	//if( player->m_fFlags( ) & FL_DUCKING ) {
		//velocity.z += ground_factor * sv_jump_impulse->get_float( ); // how they do it in csgo
	//}
	//else {
		velocity.z = ground_factor * sv_jump_impulse->get_float( );
	//}

	finish_gravity( player, origin, velocity );

}

void c_prediction::start_gravity( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	static auto sv_gravity = g_csgo.m_cvar( )->FindVar( xors( "sv_gravity" ) );

	float m_flGravity = player->m_flGravity( );

	//if( !m_flGravity ) {
		m_flGravity = 1.f;
	//}

	velocity.z -= ( m_flGravity * sv_gravity->get_float( ) * 0.5f * TICK_INTERVAL( ) );

	check_velocity( player, origin, velocity );
}

void c_prediction::finish_gravity( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	static auto sv_gravity = g_csgo.m_cvar( )->FindVar( xors( "sv_gravity" ) );

	float m_flGravity = player->m_flGravity( );
	//if( !m_flGravity ) {
		m_flGravity = 1.f;
	//}

	velocity.z -= ( m_flGravity * sv_gravity->get_float( ) * 0.5f * TICK_INTERVAL( ) );

	check_velocity( player, origin, velocity );
}

void c_prediction::friction( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	static auto sv_friction = g_csgo.m_cvar( )->FindVar( xors( "sv_friction" ) );
	static auto sv_stopspeed = g_csgo.m_cvar( )->FindVar( xors( "sv_stopspeed" ) );
	const float m_surfaceFriction = player->m_surfaceFriction( );

	float speed = velocity.length( );

	if( speed < 0.1f )
		return;

	float friction = sv_friction->get_float( ) * m_surfaceFriction;
	float control = ( speed < sv_stopspeed->get_float( ) ) ? sv_stopspeed->get_float( ) : speed;
	float drop = control * friction * TICK_INTERVAL( );

	float newspeed = speed - drop;
	if( newspeed < 0.f )
		newspeed = 0.f;

	if( newspeed != speed ) {
		newspeed /= speed;
		velocity *= newspeed;
	}
}

void c_prediction::air_move( c_base_player* player, vec3_t& origin, vec3_t& velocity, vec3_t& old_velocity, vec3_t& acceleration ) {
	vec3_t		wishvel;
	vec3_t		wishdir;
	float		wishspeed;
	vec3_t		forward, right, up;

	//fmove = m_player.m_movement.x;
	//smove = m_player.m_movement.y;
	//
	//math::angle_vectors( m_player.m_angles, &forward, &right, &up );
	//
	//forward.z = right.z = 0.f;
	//
	//forward.normalize_vector( );
	//right.normalize_vector( );
	//
	//for( int i{ }; i < 2; ++i ) {
	//	wishvel[ i ] = forward[ i ] * fmove + right[ i ] * smove;
	//}


	wishvel = velocity;

	wishvel[ 2 ] = 0.f;

	wishdir = wishvel;
	wishdir.normalize_vector( );

	wishspeed = acceleration.length( ); // probably wrong

	air_accelerate( player, origin, old_velocity, wishdir, wishspeed );
}

void c_prediction::air_accelerate( c_base_player* player, vec3_t& origin, vec3_t& velocity, vec3_t& wishdir, float wishspeed ) {
	static auto sv_airaccelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_airaccelerate" ) );

	float wishspd = wishspeed;

	if( wishspd > 30.f )
		wishspd = 30.f;

	float currentspeed = velocity.dot( wishdir );

	float addspeed = wishspd - currentspeed;

	if( addspeed <= 0 )
		return;

	float accelspeed = sv_airaccelerate->get_float( ) * wishspeed * TICK_INTERVAL( );

	if( accelspeed > addspeed )
		accelspeed = addspeed;

	for( int i{ }; i < 2; ++i ) {
		velocity[ i ] += accelspeed * wishdir[ i ];
	}
}

void c_prediction::try_player_move( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	CGameTrace pm;
	vec3_t end_pos = origin + velocity * TICK_INTERVAL( );	

	trace_player_bbox( player, origin, end_pos, &pm );

	if( pm.fraction != 1.f ) {
		end_pos = pm.endpos;
	}

	origin = end_pos;
}



//there are supposed to be some ladder checks etc here, but we're really only supposed to be doing this when people are breaking lag comp etc
//also ugly code, please fix :(
bool c_prediction::categorize_position( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	constexpr float NON_JUMP_VELOCITY = 140.f;
	vec3_t ground_point = origin;
	bool is_moving_up_rapidally = velocity.z > NON_JUMP_VELOCITY;
	CGameTrace pm;

	ground_point.z -= 2.f;

	if( is_moving_up_rapidally ) {
		return false;
	}
	else {
		trace_player_bbox( player, origin, ground_point, &pm );
		if( !pm.m_pEnt || pm.plane.normal.z < 0.7 ) {
			try_touch_ground_in_quadrants( player, origin, ground_point, &pm );
			if( !pm.m_pEnt || pm.plane.normal.z < 0.7 ) {
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return true;
		}
	}

	return true;
}


void c_prediction::check_velocity( c_base_player* player, vec3_t& origin, vec3_t& velocity ) {
	static auto sv_max_velocity = g_csgo.m_cvar( )->FindVar( xors( "sv_maxvelocity" ) );
	const float max_velocity = sv_max_velocity->get_float( );

	for( int i{ }; i < 3; ++i ) {
		if( !std::isfinite( velocity[ i ] ) ) {
			velocity[ i ] = 0.f;
		}

		if( !std::isfinite( origin[ i ] ) ) {
			origin[ i ] = 0.f;
		}

		velocity[ i ] = std::clamp( velocity[ i ], -max_velocity, max_velocity );
	}
}



// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L2025
vec3_t c_prediction::extrapolate_player( c_base_player* player, int ticks ) {
	static auto sv_gravity = g_csgo.m_cvar( )->FindVar( xors( "sv_gravity" ) );
	static auto sv_jump_impulse = g_csgo.m_cvar( )->FindVar( xors( "sv_jump_impulse" ) );

	auto index = player->ce( )->GetIndex( );
	auto data = get_player_data( index );

	vec3_t&		m_vecBaseVelocity = player->get< vec3_t >( 0x11C );

	vec3_t		predicted( data.m_position );
	vec3_t		velocity( data.m_velocity );
	vec3_t		acceleration{ };

	if( !data.m_valid )
		return predicted;

	float velocity_dir = RAD2DEG( atan2( velocity.y, velocity.x ) );
	float angle_dir = velocity_dir - RAD2DEG( atan2( data.m_old_velocity.y, data.m_old_velocity.x ) );
	
	angle_dir *= TICKS_TO_TIME( data.m_last_choke );

	if( velocity_dir <= 180.f ) {
		if( velocity_dir < -180.f )
			velocity_dir += 360.f;
	}
	else {
		velocity_dir -= 360.f;
	}

	float length = velocity.length2d( );

	float dir = velocity_dir;

	for( int i = 0; i < ticks; ++i ) {
		float extrapolated_dir = velocity_dir + angle_dir;

		velocity.x = cos( DEG2RAD( extrapolated_dir ) ) * length;
		velocity.y = sin( DEG2RAD( extrapolated_dir ) ) * length;

		predicted = aimware_extrapolate( player, predicted, velocity );

		velocity_dir = extrapolated_dir;
	}

	return predicted;
}

void c_prediction::trace_player_bbox( c_base_player* player, const vec3_t& start, const vec3_t& end, CGameTrace* pm ) {
	Ray_t ray;
	ray.Init( start, end, player->m_vecMins( ), player->m_vecMaxs( ) );

	CTraceFilter filter;
	filter.pSkip = player;

	g_csgo.m_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, pm );
}

void c_prediction::try_touch_ground( c_base_player* player, const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs, CGameTrace* pm ) {
	Ray_t ray;
	ray.Init( start, end, mins, maxs );

	CTraceFilter filter;
	filter.pSkip = player;

	g_csgo.m_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, pm );
}

void c_prediction::try_touch_ground_in_quadrants( c_base_player* player, const vec3_t& start, const vec3_t& end, CGameTrace* pm ) {
	vec3_t mins, maxs;

	vec3_t mins_src = player->m_vecMins( );
	vec3_t maxs_src = player->m_vecMaxs( );

	float fraction = pm->fraction;
	vec3_t end_pos = pm->endpos;

	mins = mins_src;
	maxs = vec3_t( std::min( 0.f, maxs_src.x ), std::min( 0.f, maxs_src.y ), maxs_src.z );

	try_touch_ground( player, start, end, mins, maxs, pm );
	if( pm->m_pEnt && pm->plane.normal.z >= 0.7 ) {
		pm->fraction = fraction;
		pm->endpos = end_pos;
		return;
	}

	mins = vec3_t( std::max( 0.f, mins_src.x ), std::max( 0.f, mins_src.y ), mins_src.z );
	maxs = maxs_src;

	try_touch_ground( player, start, end, mins, maxs, pm );
	if( pm->m_pEnt && pm->plane.normal.z >= 0.7 ) {
		pm->fraction = fraction;
		pm->endpos = end_pos;
		return;
	}

	mins = vec3_t( mins_src.x, std::max( 0.f, mins_src.y ), mins_src.z );
	maxs = vec3_t( std::min( 0.f, maxs_src.x ), maxs_src.y, maxs_src.z );

	try_touch_ground( player, start, end, mins, maxs, pm );
	if( pm->m_pEnt && pm->plane.normal.z >= 0.7 ) {
		pm->fraction = fraction;
		pm->endpos = end_pos;
		return;
	}

	mins = vec3_t( std::max( 0.f, mins_src.x ), mins_src.y, mins_src.z );
	maxs = vec3_t( maxs_src.x, std::min( 0.f, maxs_src.y ), maxs_src.z );

	try_touch_ground( player, start, end, mins, maxs, pm );
	if( pm->m_pEnt && pm->plane.normal.z >= 0.7 ) {
		pm->fraction = fraction;
		pm->endpos = end_pos;
		return;
	}

	pm->fraction = fraction;
	pm->endpos = end_pos;
}

void c_prediction::local_pred( user_cmd_t* ucmd ) {
	if( !ucmd || !g_ctx.m_local || !g_ctx.m_local->is_alive( ) )
		return;

	static uintptr_t run_command_address = g_csgo.m_prediction->get_old_function< uintptr_t >( 19 );

	CMoveData move_data{ };
	IClientEntity* local_ent = g_ctx.m_local->ce( );

	//backup data
	int old_buttons = ucmd->m_buttons;
	float old_curtime = g_csgo.m_globals->m_curtime;
	float old_frame_time = g_csgo.m_globals->m_frametime;
	int old_tickbase = g_ctx.m_local->m_nTickBase( );
	int old_flags = g_ctx.m_local->m_fFlags( );
	MoveType_t old_move_type = g_ctx.m_local->m_nMoveType( );
	vec3_t old_velocity = g_ctx.m_local->m_vecVelocity( );

	//set globals
	g_csgo.m_globals->m_curtime = g_csgo.m_globals->m_interval_per_tick * old_tickbase;
	g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval_per_tick;

	//random seed is already being calculated and set in createmove
	**( uintptr_t** )( run_command_address + 0x3E ) = ucmd->m_random_seed; //prediction seed
	**( uintptr_t** )( run_command_address + 0x54 ) = uintptr_t( g_ctx.m_local ); //prediction player

																				  //start prediction
	g_csgo.m_move_helper( )->SetHost( local_ent );
	g_csgo.m_game_movement( )->StartTrackPredictionErrors( local_ent );

	//run prediction
	g_csgo.m_prediction( )->SetupMove( local_ent, ucmd, g_csgo.m_move_helper( ), &move_data );
	g_csgo.m_game_movement( )->ProcessMovement( local_ent, &move_data );
	g_csgo.m_prediction( )->FinishMove( local_ent, ucmd, &move_data );

	//finish prediction
	g_csgo.m_game_movement( )->FinishTrackPredictionErrors( local_ent );
	g_csgo.m_move_helper( )->SetHost( nullptr );

	**( uintptr_t** )( run_command_address + 0x3E ) = 0xffffffff;
	**( uintptr_t*** )( run_command_address + 0x54 ) = nullptr;

	//good to have, can be used for edge jump and such
	m_predicted_flags = g_ctx.m_local->m_fFlags( );

	//restore
	ucmd->m_buttons = old_buttons;
	g_csgo.m_globals->m_curtime = old_curtime;
	g_csgo.m_globals->m_frametime = old_frame_time;
	g_ctx.m_local->m_nTickBase( ) = old_tickbase;
	g_ctx.m_local->m_fFlags( ) = old_flags;
	g_ctx.m_local->m_nMoveType( ) = old_move_type;
	g_ctx.m_local->m_vecVelocity( ) = old_velocity;
}

END_REGION
