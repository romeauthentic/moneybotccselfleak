#include "movement.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"

#include <algorithm>
#include "base_cheat.hpp"
#include "input_system.hpp"

NAMESPACE_REGION( features )

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
	if ( !g_settings.misc.auto_strafe )
		return;

	float speed = g_ctx.m_local->m_vecVelocity( ).length2d( );

	if ( m_ucmd->m_buttons & IN_JUMP && speed > 1.0f ) {
		if ( !m_ucmd->m_forwardmove && !m_ucmd->m_sidemove ) {
			if ( !m_ucmd->m_mousedx ) {
				m_ucmd->m_forwardmove = std::min< float >( 450.f, 5850.f / speed );
				m_ucmd->m_sidemove = ( m_ucmd->m_cmd_nr % 2 ) == 0 ? -450.f : 450.f;
			}
			else {
				m_ucmd->m_sidemove = m_ucmd->m_mousedx < 0.f ? -450.f : 450.f;
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

void c_movement::auto_jumpbug( ) {
	if ( !g_settings.misc.auto_jumpbug( ) ) 
		return;

	if ( !g_input.is_key_pressed( ( VirtualKeys_t )g_settings.misc.auto_jumpbug_key( ) ) )
		return;

	static bool jumped = false;
	static bool jump_next = false;

	m_ucmd->m_buttons |= IN_DUCK;

	bool pre_onground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;
	bool post_onground = g_cheat.m_prediction.get_predicted_flags( ) & FL_ONGROUND;

	if ( jump_next ) {
		m_ucmd->m_buttons &= ~IN_DUCK;
		jump_next = false;
	}
	else if ( !pre_onground && post_onground && !jumped ) {
		jump_next = true;
		jumped = true;
	}
	else if ( !pre_onground && !post_onground ) {
		jumped = false;
	}
}

//fuck hardcode
const vec3_t mins( -17.f, -17.f, 0 );
const vec3_t maxs( 17.f, 17.f, 42.f );

void rotate_movement( user_cmd_t* cmd, float rotation ) {
	rotation = rotation * M_PIRAD;

	float cos_rot = cos( rotation );
	float sin_rot = sin( rotation );

	float new_forwardmove = ( cos_rot * cmd->m_forwardmove ) - ( sin_rot * cmd->m_sidemove );
	float new_sidemove = ( sin_rot * cmd->m_forwardmove ) + ( cos_rot * cmd->m_sidemove );

	cmd->m_forwardmove = new_forwardmove;
	cmd->m_sidemove = new_sidemove;
}

float get_ideal_strafe_step( float speed ) {
	static auto* sv_airaccelerate = g_gmod.m_cvar( )->FindVar( xors( "sv_airaccelerate" ) );
	float airaccel = std::min< float >( sv_airaccelerate->get_float( ), 30.f );

	float step = std::atan2( airaccel, speed ) * M_RADPI;

	//tickcount correction
	float tickcount = 1.0f / g_gmod.m_globals->m_interval_per_tick;
	step *= ( 64.f / tickcount );

	return step;
}

float trace_ideal_step( bool direction, vec3_t direction_vec, float ideal_step, float multiplier, float max_correction ) {
	Ray_t ray;
	CTraceFilterWorldOnly filter;
	filter.pSkip = g_ctx.m_local;
	vec3_t start = g_ctx.m_local->m_vecOrigin( );
	start.z += 5.0f;

	CGameTrace trace;

	float step = ideal_step;
	float correction = 0.5f;

	if( direction ) {
		step = -step;
		correction = -correction;
		max_correction = -max_correction;
	}

	while( true ) {
		vec3_t new_dir = direction_vec;
		new_dir.y += step;

		vec3_t forward = math::angle_vectors( new_dir );
		forward *= multiplier;

		vec3_t end = start + forward;
		ray.Init( start, end, mins, maxs );

		g_gmod.m_trace( )->TraceRay( ray, MASK_SOLID, &filter, &trace );

		if( !trace.DidHit( ) ) {
			break;
		}
		else {
			new_dir.x -= 20.f;
			vec3_t forward_up = math::angle_vectors( new_dir );
			forward_up *= multiplier;

			vec3_t endUp = start + forward_up;
			ray.Init( start, endUp, mins, maxs );

			CGameTrace trace_up;
			g_gmod.m_trace( )->TraceRay( ray, MASK_SOLID, &filter, &trace_up );
			if( !trace_up.DidHit( ) ) {
				break;
			}
		}

		step += correction;

		if( direction && step <= max_correction ) {
			break;
		}
		if( !direction && step >= max_correction ) {
			break;
		}
	}

	return step;
}

float get_trace_length_multiplier( float speed ) {
	float multiplier = speed * 0.2f;
	if( multiplier < 32.f ) multiplier = 32.f;
	if( multiplier > 256.f ) multiplier = 256.f;

	return multiplier;
}

bool c_movement::get_best_direction( float ideal_step, float left, float right, float weight ) {
	float left_delta = std::fabs( ideal_step - left );
	float right_delta = std::fabs( -ideal_step - right );

	if( m_direction ) right_delta -= weight;
	else left_delta -= weight;

	return ( left_delta > right_delta );
}

float c_movement::get_best_strafe_step( float speed, vec3_t direction ) {
	float multiplier = get_trace_length_multiplier( speed );
	float ideal_step = get_ideal_strafe_step( speed );

	float left_step = trace_ideal_step( false, direction, ideal_step, multiplier, 90.f );
	float right_step = trace_ideal_step( true, direction, ideal_step, multiplier, 90.f );

	m_direction = get_best_direction( ideal_step, left_step, right_step, 25.f );

	if( m_direction ) {
		float max_clamped_correction = -ideal_step - 5.0f;
		if( right_step < max_clamped_correction ) right_step = max_clamped_correction;
		return right_step;
	}
	else {
		float max_clamped_correction = ideal_step + 5.0f;
		if( left_step > max_clamped_correction ) left_step = max_clamped_correction;
		return left_step;
	}
}

float c_movement::get_best_strafe_angle( ) {
	vec3_t velocity = g_ctx.m_local->m_vecVelocity( );
	velocity.z = 0.0f;
	float speed = velocity.length2d( );

	vec3_t direction = math::vector_angles( vec3_t( 0.0f, 0.0f, 0.0f ), velocity );
	float step = get_best_strafe_step( speed, direction );

	return direction.y + step;
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
				cmd->m_sidemove = m_direction ? 450.f : -450.f;
				rotate_movement( cmd, delta );

				vec3_t current_view;
				g_gmod.m_engine( )->GetViewAngles( current_view );
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