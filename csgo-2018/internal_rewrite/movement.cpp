#include "movement.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"

#include <algorithm>
#include "base_cheat.hpp"
#include "input_system.hpp"
#include "renderer.hpp"

NAMESPACE_REGION( features )

float get_ideal_strafe_step( float speed ) {
	static auto* sv_airaccelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_airaccelerate" ) );
	float airaccel = std::min< float >( sv_airaccelerate->get_float( ), 30.f );

	float step = std::atan2( airaccel, speed ) * M_RADPI;

	//tickcount correction
	float tickcount = 1.0f / g_csgo.m_globals->m_interval_per_tick;
	step *= ( 64.f / tickcount );

	return step;
}

void rotate_movement( user_cmd_t* cmd, float rotation ) {
	rotation = rotation * M_PIRAD;

	float cos_rot = cos( rotation );
	float sin_rot = sin( rotation );

	float new_forwardmove = ( cos_rot * cmd->m_forwardmove ) - ( sin_rot * cmd->m_sidemove );
	float new_sidemove = ( sin_rot * cmd->m_forwardmove ) + ( cos_rot * cmd->m_sidemove );

	cmd->m_forwardmove = new_forwardmove;
	cmd->m_sidemove = new_sidemove;
}

void c_movement::bhop( ) {
	if ( !g_settings.misc.bunny_hop )
		return;

	if ( g_ctx.m_local->m_nMoveType( ) == MOVETYPE_LADDER || 
		g_ctx.m_local->m_nMoveType( ) == MOVETYPE_NOCLIP )
		return;

	//jump like you nohat
	if ( m_ucmd->m_buttons & IN_JUMP && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
		m_ucmd->m_buttons &= ~IN_JUMP;
	}
}

void c_movement::auto_strafer( ) {
	if( !g_settings.misc.auto_strafe )
		return;

	vec3_t velocity = g_ctx.m_local->m_vecVelocity( );
	float speed = velocity.length2d( );
	auto cmd = g_ctx.get_last_cmd( );

	auto on_ground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
	if( cmd && ( m_ucmd->m_buttons & IN_JUMP ) && ( speed > 1.0f || g_settings.misc.air_duck( ) ) && !on_ground ) {
		if( !cmd->m_forwardmove && !cmd->m_sidemove ) {
			if( !cmd->m_mousedx ) {
				vec3_t velocity_dir = math::vector_angles( vec3_t( ), velocity );
				float yaw = g_csgo.m_engine( )->GetViewAngles( ).y;

				static auto* sv_airaccelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_airaccelerate" ) );
				float airaccel = std::min< float >( sv_airaccelerate->get_float( ), 30.f );

				float ideal_rotation = std::clamp( RAD2DEG( std::atan2f( airaccel, speed ) ), 0.f, 45.f );
				if( ( cmd->m_cmd_nr % 2 ) )
					ideal_rotation *= -1;

				cmd->m_sidemove = ( cmd->m_cmd_nr % 2 ) ? 450.f : -450.f;
				cmd->m_forwardmove = 0;

				rotate_movement( cmd, std::remainderf( ideal_rotation, 360.f ) );
			}
			else {
				cmd->m_sidemove = m_ucmd->m_mousedx < 0.f ? -450.f : 450.f;
			}
		}
	}
}


void c_movement::edge_jump( ) {
	if ( !g_settings.misc.edge_jump ) 
		return;

	if ( !g_input.is_key_pressed( ( VirtualKeys_t )g_settings.misc.edge_jump_key( ) ) )
		return;

	//needs key check here so its not always on
	//what??
	bool pre_onground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
	bool post_onground = g_cheat.m_prediction.get_predicted_flags( ) & FL_ONGROUND;

	if ( pre_onground && !post_onground ) {
		m_ucmd->m_buttons |= IN_JUMP;
	}
}

void c_movement::air_duck( ) {
	if( !g_settings.misc.air_duck )
		return;

	if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
		m_ucmd->m_buttons |= IN_DUCK;
}

void c_movement::auto_jumpbug( ) {
	if ( !g_settings.misc.auto_jumpbug( ) ) 
		return;

	if ( !g_input.is_key_pressed( ( VirtualKeys_t )g_settings.misc.auto_jumpbug_key( ) ) )
		return;

	static bool jumped = false;
	static bool jump_next = false;

	vec3_t origin = g_ctx.m_local->m_vecOrigin( );
	origin += g_ctx.m_local->m_vecVelocity( ) * TICK_INTERVAL( );

	Ray_t ray;
	ray.Init( origin, origin + vec3_t( 0.f, 0.f, 4.f ) );
	
	CTraceFilter f;
	f.pSkip = g_ctx.m_local;

	CGameTrace tr;
	g_csgo.m_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &f, &tr );

	bool unduck = tr.fraction != 1.0f;

	if( unduck ) {
		m_ucmd->m_buttons &= ~IN_DUCK;
		m_ucmd->m_buttons |= IN_JUMP;
	}
	else {
		m_ucmd->m_buttons |= IN_DUCK;
		m_ucmd->m_buttons &= ~IN_JUMP;
	}
}

void c_movement::jump_stats( ) {
	if( !g_settings.misc.show_jump_stats ) return;

	static auto sv_airaccelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_airaccelerate" ) );
	static bool was_onground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
	static vec3_t last_origin{ };
	static float ground_vel{ };
	static float last_jump_max_speed{ };
	static float last_height{ };
	static float last_dist{ };

	const float lj_threshold = sv_airaccelerate->get_float( ) < 15.f ? 190.f : 240.f;

	bool on_ground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
	bool ducking = g_ctx.m_local->m_fFlags( ) & FL_DUCKING;

	char jump_string[ 250 ] = { };

	if( on_ground ) {
		int vertical = 0;

		if( !was_onground ) {
			vec3_t cur_origin = g_ctx.m_local->m_vecOrigin( );
			last_dist = cur_origin.dist_to( last_origin );

			if( std::abs( cur_origin.z - last_origin.z ) >= ( ducking ? 10.f : 5.f ) ) {
				vertical = cur_origin.z > last_origin.z ? 1 : -1;
			}

			if( ground_vel > 200.f && last_jump_max_speed > 260.f && std::abs( last_height ) > 20.f ) {
				if( vertical ) {
					strenc::w_sprintf_s( jump_string, 250, xors( "[\3JUMP STAT\1] pre: %0.2f | max vel: %0.2f | height: %0.2f | duck: %d | \2%s\n" ),
						ground_vel, last_jump_max_speed, last_height, ducking, vertical == 1 ? xors( "vertical" ) : xors( "dropjump" ) );
				}
				else {
					bool is_lj = last_dist > lj_threshold;
					strenc::w_sprintf_s( jump_string, 250, xors( "[\3JUMP STAT\1]: pre: %0.2f | max vel: %0.2f | height: %0.2f | duck: %d | dist: %c%0.2f\n" ),
						ground_vel, last_jump_max_speed, last_height, ducking, is_lj ? 4 : 1, last_dist );
				}

				g_csgo.m_clientmode( )->m_pChatElement->ChatPrintf( 0, 0, jump_string );
			}
		}
		last_origin = g_ctx.m_local->m_vecOrigin( );
		last_jump_max_speed = 0.f;
		last_height = 0.f;
		ground_vel = g_ctx.m_local->m_vecVelocity( ).length2d( );

		was_onground = true;
	}
	else {
		was_onground = false;
		float vel = g_ctx.m_local->m_vecVelocity( ).length2d( );
		if( vel > last_jump_max_speed ) {
			last_jump_max_speed = vel;
		}
		float delta = g_ctx.m_local->m_vecOrigin( ).z - last_origin.z;
		if( std::abs( delta ) > std::abs( last_height ) ) {
			last_height = delta;
		}
	}
}

//fuck hardcode
const vec3_t mins( -26.f, -26.f, 0 );
const vec3_t maxs( 26.f, 26.f, 44.f );

constexpr int TRACE_STEP_MAX = 45;
bool trace_ideal_step( float step, float speed, vec3_t velocity, vec3_t start_pos ) {
	vec3_t direction = math::vector_angles( vec3_t( ), velocity );
	float  wish_step = direction.y + step;

	vec3_t origin = start_pos;

	vec3_t start = origin;
	vec3_t trace_step = math::angle_vectors( vec3_t( 0, wish_step, 0 ) ) * velocity.length2d( ) * TICK_INTERVAL( );

	vec3_t pos = start + trace_step;

	CGameTrace tr;
	CTraceFilter filter;
	filter.pSkip = g_ctx.m_local;

	for( size_t i{ }; i <= 1 / TICK_INTERVAL( ); ++i ) {
		start = pos;
		trace_step = math::angle_vectors( vec3_t( 0, wish_step += step, 0 ) ) * velocity.length2d( ) * TICK_INTERVAL( );
		pos += trace_step;

		Ray_t ray;
		ray.Init( start, pos, mins, maxs );

		g_csgo.m_trace( )->TraceRay( ray, MASK_SOLID, &filter, &tr );

		if( !tr.DidHit( ) ) {
			if( i == TRACE_STEP_MAX ) {
				return true;
			}
			continue;
		}

		break;
	}

	return false;
}

float c_movement::get_best_strafe_angle( ) {
	vec3_t velocity = g_ctx.m_local->m_vecVelocity( );
	float ideal_step = get_ideal_strafe_step( velocity.length2d( ) ) * 0.8f;

	float step = ideal_step;
	vec3_t start = g_ctx.m_local->m_vecOrigin( );

	for( size_t i{ }; i < 9; ++i ) {
		if( trace_ideal_step( step, velocity.length2d( ), velocity, start ) ) {
			vec3_t direction = math::vector_angles( vec3_t( ), velocity );
			return direction.y + step;
		}

		step -= step * 0.2f;
	}

	step = ideal_step;
	for( size_t i{ }; i < velocity.length2d( ) * 0.015f; ++i ) {
		step += step * 0.2f;
		if( trace_ideal_step( step, velocity.length2d( ), velocity, start ) ) {
			vec3_t direction = math::vector_angles( vec3_t( ), velocity );
			return direction.y + step;
		}
	}

	return math::vector_angles( vec3_t( ), velocity ).y;
}

void c_movement::circle_strafe( ) {
	if( g_settings.misc.circle_strafe ) {
		if( g_ctx.m_local->m_nMoveType( ) == MOVETYPE_LADDER || g_ctx.m_local->m_nMoveType( ) == MOVETYPE_NOCLIP )
			return;

		static bool can_finish = true;
		auto cmd = g_ctx.get_last_cmd( );
		if( g_input.is_key_pressed( g_settings.misc.circle_strafe_key ) || !can_finish ) {
			m_ucmd->m_buttons |= IN_JUMP;
			cmd->m_forwardmove = 450.f;

			float speed = g_ctx.m_local->m_vecVelocity( ).length2d( );
			if( speed > 1.f ) {
				can_finish = false;
				float angle = get_best_strafe_angle( );

				float delta = std::remainderf( m_ucmd->m_viewangles.y - angle, 360.f );

				cmd->m_forwardmove = 5850.f / speed;
				cmd->m_sidemove = -450.f;
				rotate_movement( cmd, delta );

				vec3_t current_view;
				g_csgo.m_engine( )->GetViewAngles( current_view );
				float view_delta = std::remainderf( current_view.y - angle, 360.f );

				if( std::fabs( view_delta ) < 10.0f || speed < 250.f ) {
					can_finish = true;
				}
			}
			else {
				can_finish = true;
			}
		}
	}
}

END_REGION