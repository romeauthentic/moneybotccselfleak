#include "util.hpp"

#include "interface.hpp"
#include "c_base_player.hpp"
#include "pattern.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "math.hpp"
#include "base_cheat.hpp"

float TICK_INTERVAL( ) {
	return g_csgo.m_globals->m_interval_per_tick;
}

int TIME_TO_TICKS( float dt ) {
	return static_cast< int >( 0.5f + dt / TICK_INTERVAL( ) );
}

float TICKS_TO_TIME( int tick ) {
	return tick * TICK_INTERVAL( );
}

bool util::is_low_fps( ) {
	return g_csgo.m_globals->m_frametime > g_csgo.m_globals->m_interval_per_tick;
}

int util::get_closest_player( bool dist ) {
	float	cur_fov = FLT_MAX;
	int		ret{ -1 };
	vec3_t	viewangles{ };
	vec3_t	local_pos{ };

	bool friendlies = g_settings.rage.active ?
		g_settings.rage.friendlies : g_settings.legit.friendlies;

	local_pos = g_ctx.m_local->get_eye_pos( );
	g_csgo.m_engine( )->GetViewAngles( viewangles );

	for( int i{ 1 }; i < g_csgo.m_globals->m_maxclients; ++i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

		if( !ent ) continue;
		if( ent == g_ctx.m_local ) continue;
		if( !ent->is_valid( ) ) continue;
		int team = ent->m_iTeamNum( );
		if( team == g_ctx.m_local->m_iTeamNum( ) && !friendlies ) {
			continue;
		}

		float distance = 0.f;
		if( dist ) {
			distance = g_ctx.m_local->m_vecOrigin( ).dist_to( ent->m_vecOrigin( ) );
		}
		else {
			auto ang = math::vector_angles( local_pos, ent->get_hitbox_pos( 0 ) );
			ang.clamp( );
			distance = ( viewangles - ang ).clamp( ).length2d( );
		}

		if( distance < cur_fov ) {
			ret = i;
			cur_fov = distance;
		}
	}

	return ret;
}

void util::clip_trace_to_player( IClientEntity* player, const vec3_t& src, const vec3_t& end,
	unsigned mask, CTraceFilter* filter, CGameTrace* tr ) {
	CGameTrace player_trace;
	Ray_t ray;
	float smallest_fraction = tr->fraction;

	ray.Init( src, end );

	if ( !filter->ShouldHitEntity( player, mask ) ) {
		return;
	}

	g_csgo.m_trace( )->ClipRayToEntity( ray, mask | CONTENTS_HITBOX, player, &player_trace );

	if ( player_trace.fraction < smallest_fraction ) {
		*tr = player_trace;
		smallest_fraction = player_trace.fraction;
	}
}

std::string util::hitgroup_to_string( int hitgroup ) {
	switch( hitgroup ) {
	case HITGROUP_CHEST:
		return xors( "chest" );
	case HITGROUP_HEAD:
		return xors( "head" );
	case HITGROUP_LEFTARM:
		return xors( "left arm" );
	case HITGROUP_LEFTLEG:
		return xors( "left leg" );
	case HITGROUP_RIGHTARM:
		return xors( "right arm" );
	case HITGROUP_RIGHTLEG:
		return xors( "right leg" );
	case HITGROUP_STOMACH:
		return xors( "stomach" );
	default:
		return xors( "generic" );
	}
}

bool util::trace_ray( const vec3_t& start, const vec3_t& end, IClientEntity* a, IClientEntity* b ) {
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = a;

	ray.Init( start, end );

	g_csgo.m_trace( )->TraceRay( ray, 0x46004003 | CONTENTS_HITBOX, &filter, &tr );
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

int util::get_random_int( int min, int max ) {
	std::random_device rd;
	std::mt19937 eng( rd( ) );
	std::uniform_int_distribution<> distr( min, max );

	return distr( eng );
}

std::array< util::hitchance_data_t, 256 > hitchance_data;
void util::calculate_spread_runtime( ) {
	constexpr float pi_2 = 2.0f * ( float ) M_PI;
	
	for ( size_t n { }; n < 256; ++n ) {
		set_random_seed( n );

		const float rand_a	  = get_random_float( 0.0f, 1.0f );
		const float rand_pi_a = get_random_float( 0.0f, pi_2 );
		const float rand_b	  = get_random_float( 0.0f, 1.0f );
		const float rand_pi_b = get_random_float( 0.0f, pi_2 );

		hitchance_data [ n ] = { 
			{  rand_a, rand_b								 },
			{  std::cos( rand_pi_a ), std::sin( rand_pi_a )	 },
			{  std::cos( rand_pi_b ), std::sin( rand_pi_b )	 }
		};
	}
}

// This is gay, I didn't want to fuck up the project.
vec3_t util::get_spread_dir( void *weapon, float inaccuracy, float spread, vec3_t angles, int seed ) {
	const int   rnsd = ( seed & 0xFF );
	const auto *data = &hitchance_data [ rnsd ];
	auto		*wpn = ( c_base_weapon * ) weapon;

	if ( !wpn )
		return vec3_t{ };

	float rand_a = data->m_random[ 0 ];
	float rand_b = data->m_random[ 1 ];

	const uint16_t item_def = wpn->m_iItemDefinitionIndex( );

	// Because fuck you.
	if ( item_def == WEAPON_NEGEV ) {
		const int recoil_seed = wpn->get_wpn_info( )->recoil_seed;

		if ( recoil_seed < 3 ) {
			// Shoutouts to toast for the epic simplified math :^)
			rand_a = 1.0f - std::powf( rand_a, ( 3 - recoil_seed ) + 1 );
			rand_b = 1.0f - std::powf( rand_b, ( 3 - recoil_seed ) + 1 );
		}
	}

	// Calculate spread.
	const float rand_inacc  = rand_a * g_ctx.m_weapon_inaccuracy;
	const float rand_spread = rand_b * g_ctx.m_weapon_spread;

	const float spread_x = data->m_inaccuracy [ 0 ] * rand_inacc + data->m_spread [ 0 ] * rand_spread;
	const float spread_y = data->m_inaccuracy [ 1 ] * rand_inacc + data->m_spread [ 1 ] * rand_spread;

	// Transform angle to direction.
	vec3_t forward, right, up;
	math::angle_vectors( angles, &forward, &right, &up );

	return forward + right * spread_x + up * spread_y;
}

bool __vectorcall util::intersects_hitbox( vec3_t eye_pos, vec3_t end_pos, vec3_t min, vec3_t max, float radius ) {
	auto dist = math::dist_segment_to_segment( eye_pos, end_pos, min, max );

	return ( dist < radius );
}

// Note: abuse this as much as you can to achieve the full potential it offers.
bool util::hitchance( int target, const vec3_t& angles, int percentage, int hitbox ) {
	if( g_settings.rage.compensate_spread )
		return true;

	auto ent = g_csgo.m_entlist( )->GetClientEntity< >( target );

	if ( !ent ) return false;
	if ( percentage <= 1 ) return true;

	int hits_needed = percentage * 256 / 100;
	int hits = 0;

	auto wep = g_ctx.m_local->get_weapon( );
	if ( !wep ) return false;

	vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );
	auto wpn_info = wep->get_wpn_info( );
	if( !wpn_info ) return false;

	float length = wpn_info->range;

	float inaccuracy = g_ctx.m_weapon_inaccuracy;
	float spread = g_ctx.m_weapon_spread;

	if( g_settings.rage.active->m_spread_limit( ) ) {
		bool crouched = g_ctx.m_local->m_vecViewOffset( ).z < 50.f;
		bool alt_inaccuracy = wep->is_sniper( ) || wep->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER;
		float min_accuracy = crouched ? ( alt_inaccuracy ? wpn_info->inaccuracy_crouch_alt : wpn_info->inaccuracy_crouch )
			: ( alt_inaccuracy ? wpn_info->inaccuracy_stand_alt : wpn_info->inaccuracy_stand );

		float max_accuracy = alt_inaccuracy ? wpn_info->inaccuracy_move_alt : wpn_info->inaccuracy_move;
		min_accuracy += 0.0005f;

		float spread_accuracy_fraction = -( g_settings.rage.active->m_spread_limit_min( ) * 0.01f - 1.0f );
		float spread_accuracy_tolerance = max_accuracy * spread_accuracy_fraction;
		min_accuracy += spread_accuracy_tolerance;

		if( wep->m_iItemDefinitionIndex( ) == WEAPON_SSG08 && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			min_accuracy = 0.008750f;
		}
			
		if( min_accuracy >= inaccuracy )
			return true;
	}

	struct hitbox_data_t {
		hitbox_data_t( const vec3_t& min, const vec3_t& max, float radius ) {
			m_min = min;
			m_max = max;
			m_radius = radius;
		}

		vec3_t m_min{ };
		vec3_t m_max{ };
		float m_radius{ };
	};

	std::vector< hitbox_data_t > m_hitbox_data;

	auto model = ent->ce( )->GetModel( );
	if( !model ) return false;

	auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
	if( !hdr ) return false;

	matrix3x4 bone_matrix[ 128 ];
	memcpy( bone_matrix,
		ent->m_CachedBoneData( ).GetElements( ),
		ent->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

	auto set = hdr->pHitboxSet( ent->m_nHitboxSet( ) );
	if( !set ) return false;

	if( hitbox == -1 ) {
		for( int hitbox{ }; hitbox < set->numhitboxes; ++hitbox ) {
			auto box = set->pHitbox( hitbox );
			if( !box ) continue;

			auto min = math::vector_transform( box->bbmin, bone_matrix[ box->bone ] );
			auto max = math::vector_transform( box->bbmax, bone_matrix[ box->bone ] );
			auto radius = box->m_flRadius;

			if( radius == -1.f ) {
				radius = min.dist_to( max );
			}

			m_hitbox_data.push_back( hitbox_data_t( min, max, radius ) );
		}
	}
	else {
		auto box = set->pHitbox( hitbox );
		if( !box ) return false;

		auto min = math::vector_transform( box->bbmin, bone_matrix[ box->bone ] );
		auto max = math::vector_transform( box->bbmax, bone_matrix[ box->bone ] );
		auto radius = box->m_flRadius;

		if( radius == -1.f ) {
			radius = min.dist_to( max );
		}

		m_hitbox_data.push_back( hitbox_data_t( min, max, radius ) );
	}

	if( m_hitbox_data.empty( ) )
		return false;

	for ( int i{ }; i < 256; i ++ ) {
		vec3_t spread_dir = get_spread_dir( wep, inaccuracy, spread, angles, i );

		//Ray_t ray;
		//vec3_t end_pos = eye_pos + spread_dir * length;
		//ray.Init( eye_pos, end_pos );
		//
		//
		//CGameTrace tr;
		//g_csgo.m_trace( )->ClipRayToEntity( ray, 0x46004003, ent->ce( ), &tr );

		vec3_t end_pos = eye_pos + ( spread_dir * length );

		for( const auto& hitbox : m_hitbox_data ) {
			if( intersects_hitbox( eye_pos, end_pos, hitbox.m_min, hitbox.m_max, hitbox.m_radius ) ) {
				hits++;
				break; // cannot hit more than one hitbox
			}
		}

		if ( hits >= hits_needed )
			return true;
	}

	return false;
}
	

float util::get_total_latency( ) {
	auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );

	if ( nci ) {
		float latency = nci->GetLatency( 0 ) + nci->GetLatency( 1 );
		return latency;
	}

	return 0.f;
}

float util::get_lerptime( ) {
	static cvar_t* cl_interpolate = g_csgo.m_cvar( )->FindVar( xors( "cl_interpolate" ) );
	static cvar_t* cl_interp = g_csgo.m_cvar( )->FindVar( xors( "cl_interp" ) );
	static cvar_t* cl_updaterate = g_csgo.m_cvar( )->FindVar( xors( "cl_updaterate" ) );
	static cvar_t* cl_interp_ratio = g_csgo.m_cvar( )->FindVar( xors( "cl_interp_ratio" ) );

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
	float correct = std::clamp( latency + get_lerptime( ), 0.f, *( bool* )( c_base_player::get_game_rules( ) + 0x75 ) ? 0.2f : 1.0f );
	float delta = correct - ( g_ctx.pred_time( ) - TICKS_TO_TIME( tickcount ) );

	if( g_settings.rage.enabled ) {
		return delta > -0.2f + TICK_INTERVAL( ) * 2.f && delta < 0.2f;
	}
	
	return std::abs( delta ) < 0.2f;
}

void util::disable_pvs( ) {
	for( int i{ 1 }; i < 65; ++i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

		if( !ent || !ent->is_valid( ) )
			continue;

		if( ent == g_ctx.m_local )
			continue;

		*( int* )( uintptr_t( ent ) + 0xa30 ) = g_csgo.m_globals->m_framecount;
		*( int* )( uintptr_t( ent ) + 0xa28 ) = 0;
	}
}

vec2_t util::screen_transform( vec3_t world ) {
	vec2_t screen;
	auto w2s = [ &world, &screen ]( ) -> bool {
		static uintptr_t view_matrix = 0;
		if( !view_matrix ) {
			view_matrix = pattern::first_code_match( g_csgo.m_chl.dll( ),
				xors( "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) );

			view_matrix = *reinterpret_cast< uintptr_t* >( view_matrix + 0x3 ) + 176;
		}

		const matrix3x4& matrix = *( matrix3x4* )view_matrix;
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
	g_csgo.m_engine( )->GetScreenSize( w, h );

	screen.x = ( w * .5f ) + ( screen.x * w ) * .5f;
	screen.y = ( h * .5f ) - ( screen.y * h ) * .5f;

	return screen;
	//}

	return vec2_t{ };
}

bool util::line_goes_through_smoke( const vec3_t &start_pos, const vec3_t &end_pos ) {
	using       line_goes_through_smoke_t  = bool( * ) ( vec3_t, vec3_t, int16_t );
	static auto line_goes_through_smoke_fn = pattern::first_code_match< line_goes_through_smoke_t >( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0" ) );

	if( !line_goes_through_smoke_fn )
		return false;

	return line_goes_through_smoke_fn( start_pos, end_pos, 1 );
}

int util::hitbox_to_hitgroup( int hitbox ) {
	switch( hitbox ) {
	case HITBOX_HEAD:
	case HITBOX_NECK:
		return HITGROUP_HEAD;
	case HITBOX_BODY:
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_PELVIS:
		return HITGROUP_STOMACH;
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_THIGH:
	case HITBOX_LEFT_FOOT:
		return HITGROUP_RIGHTLEG;
	case HITBOX_RIGHT_CALF:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_RIGHT_FOOT:
		return HITGROUP_LEFTLEG;
	case HITBOX_LEFT_HAND:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_UPPER_ARM:
		return HITGROUP_LEFTARM;
	case HITBOX_RIGHT_HAND:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_GENERIC;
	}
}

const char* util::definition_index_to_name( int index ) {
	index = std::clamp( index, 0, 65 );

	switch( index ) {
	case 0:
		return xors( "knife" );
	case WEAPON_DEAGLE:
		return xors( "deagle" );
	case WEAPON_ELITE:
		return xors( "elite" );
	case WEAPON_FIVESEVEN:
		return xors( "five seven" );
	case WEAPON_GLOCK:
		return xors( "glock" );
	case WEAPON_AK47:
		return xors( "ak47" );
	case WEAPON_AUG:
		return xors( "aug" );
	case WEAPON_AWP:
		return xors( "awp" );
	case WEAPON_FAMAS:
		return xors( "famas" );
	case WEAPON_G3SG1:
		return xors( "g3sg1" );
	case WEAPON_GALIL:
		return xors( "galil" );
	case WEAPON_M249:
		return xors( "m249" );
	case WEAPON_M4A4:
		return xors( "m4a4" );
	case WEAPON_MAC10:
		return xors( "mac10" );
	case WEAPON_P90:
		return xors( "p90" );
	case WEAPON_UMP45:
		return xors( "ump45" );
	case WEAPON_XM1014:
		return xors( "xm1014" );
	case WEAPON_BIZON:
		return xors( "bizon" );
	case WEAPON_MAG7:
		return xors( "mag7" );
	case WEAPON_NEGEV:
		return xors( "negev" );
	case WEAPON_SAWEDOFF:
		return xors( "sawed-off" );
	case WEAPON_TEC9:
		return xors( "tec9" );
	case WEAPON_ZEUS:
		return xors( "zeus" );
	case WEAPON_P2000:
		return xors( "p2000" );
	case WEAPON_MP7:
		return xors( "mp7" );
	case WEAPON_MP9:
		return xors( "mp9" );
	case WEAPON_NOVA:
		return xors( "nova" );
	case WEAPON_P250:
		return xors( "p250" );
	case WEAPON_SCAR20:
		return xors( "scar20" );
	case WEAPON_SG553:
		return xors( "sg556" );
	case WEAPON_SSG08:
		return xors( "ssg08" );
	case WEAPON_KNIFEGG:
		return xors( "golden knife" ); // for the gun game hackers
	case WEAPON_KNIFE:
		return xors( "knife" );
	case WEAPON_FLASHBANG:
		return xors( "flash" );
	case WEAPON_HEGRENADE:
		return xors( "nade" );
	case WEAPON_SMOKEGRENADE:
		return xors( "smoke" );
	case WEAPON_MOLOTOV:
		return xors( "molotov" );
	case WEAPON_DECOY:
		return xors( "decoy" );
	case WEAPON_INC:
		return xors( "incendiary" );
	case WEAPON_C4:
		return xors( "c4" );
	case WEAPON_KNIFE_T:
		return xors( "knife" );
	case WEAPON_M4A1S:
		return xors( "m4a1" );
	case WEAPON_USPS:
		return xors( "usp" );
	case WEAPON_CZ75:
		return xors( "cz75" );
	case WEAPON_REVOLVER:
		return xors( "revolver" );
	case WEAPON_MP5SD:
		return xors( "mp5-sd" );
	default:
		return xors( "none" );
	}
}
