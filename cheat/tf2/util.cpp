#include "util.hpp"

#include "interfaces.h"
#include "CBasePlayer.h"
#include "pattern.hpp"
#include "ctx.hpp"
#include "settings.h"
#include "math.h"
#include "base_cheat.h"

float TICK_INTERVAL( ) {
	return cl.m_globals->interval_per_tick;
}

int TIME_TO_TICKS( float dt ) {
	return static_cast< int >( 0.5f + dt / TICK_INTERVAL( ) );
}

float TICKS_TO_TIME( int tick ) {
	return tick * TICK_INTERVAL( );
}

bool util::is_low_fps( ) {
	return cl.m_globals->frametime > cl.m_globals->interval_per_tick;
}

int util::get_closest_player( ) {
	float	cur_fov{ 360.f };
	int		ret{ -1 };
	vec3_t	viewangles{ };
	vec3_t	local_pos{ };

	bool friendlies = g_settings.legit.friendlies;

	local_pos = g_ctx.m_local->get_eye_pos( );
	cl.m_engine( )->GetViewAngles( viewangles );

	for( int i{ 1 }; i < cl.m_globals->maxclients; ++i ) {
		auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );

		if( !ent ) continue;
		if( ent == g_ctx.m_local ) continue;
		if( !ent->is_valid( ) ) continue;
		int team = ent->get_team( );
		if( team == g_ctx.m_local->get_team( ) && !friendlies ) {
			continue;
		}

		auto ang = math::vector_angles( local_pos, ent->get_hitbox_position( 0 ) );
		ang.clamp( );

		float fov = ( viewangles - ang ).clamp( ).length2d( );
		if( fov < cur_fov ) {
			ret = i;
			cur_fov = fov;
		}
	}

	return ret;
}

void util::clip_trace_to_player( IClientEntity* player, vec3_t& src, vec3_t& end,
	unsigned mask, CTraceFilter* filter, CGameTrace* tr ) {
	CGameTrace player_trace;
	Ray_t ray;
	float smallest_fraction = tr->fraction;

	ray.Init( src, end );

	if( !filter->ShouldHitEntity( player, mask ) ) {
		return;
	}

	cl.m_trace( )->clip_ray_to_ent( ray, mask | CONTENTS_HITBOX, player, &player_trace );

	if( player_trace.fraction < smallest_fraction ) {
		*tr = player_trace;
		smallest_fraction = player_trace.fraction;
	}
}


bool util::trace_ray( vec3_t& start, vec3_t& end, IClientEntity* a, IClientEntity* b ) {
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = a;

	ray.Init( start, end );

	cl.m_trace( )->trace_ray( ray, 0x46004003 | CONTENTS_HITBOX, &filter, &tr );
	clip_trace_to_player( b, start, end, ( unsigned )0x46004003, &filter, &tr );

	return tr.m_pEnt == b || tr.fraction > 0.98f;
}

void util::set_random_seed( int seed ) {
	using fn = int( __cdecl* )( int );
	fn fn_ptr = ( fn )( GetProcAddress(
		GetModuleHandleA( xors( "vstdlib.dll" ) ),
		xors( "RandomSeed" ) ) );

	fn_ptr( seed );
}

float util::get_random_float( float min, float max ) {
	using fn = float( *)( float, float );
	fn fn_ptr = ( fn )( GetProcAddress(
		GetModuleHandleA( xors( "vstdlib.dll" ) ),
		xors( "RandomFloat" ) ) );

	return fn_ptr( min, max );
}

vec3_t util::get_spread_dir( float inaccuracy, float spread, vec3_t angles, int seed ) {
	set_random_seed( ( seed & 0xff ) + 1 );

	float rand_a = get_random_float( 0.0f, 1.0f );
	float pi_rand_a = get_random_float( 0.f, 2.f * M_PI );
	float rand_b = get_random_float( 0.f, 1.0f );
	float pi_rand_b = get_random_float( 0.f, 2.f * M_PI );

	float spread_x = cos( pi_rand_a ) * ( rand_a * inaccuracy ) + cos( pi_rand_b ) * ( rand_b * spread );
	float spread_y = sin( pi_rand_a ) * ( rand_a * inaccuracy ) + sin( pi_rand_b ) * ( rand_b * spread );

	vec3_t forward, right, up;
	math::angle_vectors( angles, &forward, &right, &up );

	vec3_t spread_dir = forward + ( right * spread_x ) + ( up * spread_y );
	spread_dir.normalize_vector( );

	return spread_dir;
}

bool __vectorcall util::intersects_hitbox( vec3_t eye_pos, vec3_t end_pos, vec3_t min, vec3_t max, float radius ) {
	auto dist = math::dist_segment_to_segment( eye_pos, end_pos, min, max );

	return ( dist < radius );
}

float util::get_total_latency( ) {
	auto nci = cl.m_engine( )->GetNetChannelInfo( );

	if( nci ) {
		float latency = nci->GetLatency( 0 ) + nci->GetLatency( 1 );
		return latency;
	}

	return 0.f;
}

float util::get_lerptime( ) {
	static cvar_t* cl_interpolate = cl.m_cvar( )->FindVar( xors( "cl_interpolate" ) );
	static cvar_t* cl_interp = cl.m_cvar( )->FindVar( xors( "cl_interp" ) );
	static cvar_t* cl_updaterate = cl.m_cvar( )->FindVar( xors( "cl_updaterate" ) );
	static cvar_t* cl_interp_ratio = cl.m_cvar( )->FindVar( xors( "cl_interp_ratio" ) );

	if( cl_interp && cl_interpolate && cl_updaterate && cl_interp_ratio ) {
		bool interpolate = cl_interpolate->get_int( );
		if( interpolate ) {
			float interp = cl_interp->get_float( );
			float interp_ratio = cl_interp_ratio->get_float( );
			float updaterate = cl_updaterate->get_float( );

			return std::max< float >( interp, interp_ratio / updaterate );
		}
	}

	return 0.f;
}

bool util::is_tick_valid( int tickcount ) {
	float latency = get_total_latency( );
	float correct = std::clamp( latency + get_lerptime( ), 0.f, 1.f );
	float delta = correct - ( g_ctx.pred_time( ) - TICKS_TO_TIME( tickcount ) );


	return std::abs( delta ) < 0.2f;
}

void util::disable_pvs( ) {
	for( int i{ 1 }; i < 65; ++i ) {
		auto ent = cl.m_entlist( )->get_client_entity< c_base_player >( i );

		if( !ent || !ent->is_valid( ) )
			continue;

		if( ent == g_ctx.m_local )
			continue;

		*( int* )( uintptr_t( ent ) + 0xa30 ) = cl.m_globals->framecount;
		*( int* )( uintptr_t( ent ) + 0xa28 ) = 0;
	}
}

vec2_t util::screen_transform( vec3_t world ) {
	//todo: replace with a diff matrix
	//this one spergs out sometimes

	const matrix3x4& w2s_matrix = cl.m_engine( )->GetWorldToScreenMatrix( );

	vec2_t screen;
	auto w2s = [ &world, &screen ]( ) -> bool {

		const matrix3x4& matrix = cl.m_engine( )->GetWorldToScreenMatrix( );
		screen.x = matrix[ 0 ][ 0 ] * world[ 0 ] + matrix[ 0 ][ 1 ] * world[ 1 ] + matrix[ 0 ][ 2 ] * world[ 2 ] + matrix[ 0 ][ 3 ];
		screen.y = matrix[ 1 ][ 0 ] * world[ 0 ] + matrix[ 1 ][ 1 ] * world[ 1 ] + matrix[ 1 ][ 2 ] * world[ 2 ] + matrix[ 1 ][ 3 ];

		float w = matrix[ 3 ][ 0 ] * world[ 0 ] + matrix[ 3 ][ 1 ] * world[ 1 ] + matrix[ 3 ][ 2 ] * world[ 2 ] + matrix[ 3 ][ 3 ];

		if( w < 0.001f ) {
			//screen.x *= 100000.f;
			//screen.y *= 100000.f;
			return true;
		}

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;

		return false;
	};

	w2s( );
	//if ( !w2s( ) ) {
	int w, h;
	cl.m_engine( )->GetScreenSize( w, h );

	screen.x = ( w * .5f ) + ( screen.x * w ) * .5f;
	screen.y = ( h * .5f ) - ( screen.y * h ) * .5f;

	return screen;
	//}

	return vec2_t{ };
}

const char* util::object_index_to_name( int index ) {
	index = std::clamp( index, 0, 65 );

	switch( index ) {
	case 89:
		return xors( "teleporter" );
	case 86:
		return xors( "dispenser" );
	case 88:
		return xors( "sentry" );
	default:
		return xors( "none" );
	}
}

