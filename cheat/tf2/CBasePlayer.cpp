#include "CBasePlayer.h"
#include "sdk.h"
#include <string>
#include "math.h"
#include "graphics.h"
#include "CBaseWeapon.h"
#include "ClientClass.h"
#include "IVEngineClient.h"

vec3_t c_base_player::get_eye_pos( ) {
	vec3_t offset = get_view_offset( );
	vec3_t origin = get_origin( );
	return offset + origin;
}

uintptr_t c_base_player::get_player_resource( ) {
	for( int i{ 1 }; i < cl.m_entlist( )->get_highest_entity_index( ); ++i ) {
		auto ent = cl.m_entlist( )->get_client_entity< IClientEntity >( i );
		if( !ent ) continue;
		auto class_ = ent->GetClientClass( );
		if( class_ ) {
			if( class_->m_class_id == CTFPlayerResource ) {
				return uintptr_t( ent );
			}
		}
	}

	return 0;
}

player_info_t c_base_player::get_info( ) {
	player_info_t info;
	cl.m_engine( )->GetPlayerInfo( ce( )->index( ), &info );

	return info;
}

void c_base_player::get_name_safe( char* buf ) {
	player_info_t info;
	if( cl.m_engine( )->GetPlayerInfo( this->ce( )->index( ), &info ) ) {
		for( size_t i{ }; i < 32; ++i ) {
			switch( info.name[ i ] ) {
			case '"':
			case '\\':
			case ';':
			case '\n':
				buf[ i ] = ' ';
				break;
			default:
				buf[ i ] = info.name[ i ];
				break;
			}
		}

		buf[ 31 ] = 0;
	}
}

uintptr_t c_base_player::get_game_rules( ) {
	static uintptr_t** game_rules = pattern::first_code_match< uintptr_t** >( cl.m_chl.dll( ), xors( "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05" ), 0x1 );
	return **game_rules;
}

bool c_base_player::is_valid( ) {
	auto local = cl.m_entlist( )->get_local_player( );
	return ( this != local && !is_dormant( ) && is_alive( ) && GetClientClass( )->m_class_id == CTFPlayer );
}

vec3_t c_base_player::get_origin( ) {
	return m_vecOrigin( );
}

vec3_t c_base_player::get_view_offset( ) {
	return m_vecViewOffset( );
}

vec3_t c_base_player::get_eye_angles( ) {
	return m_angEyeAngles( );
}

vec3_t c_base_player::get_velocity( ) {
	return m_vecVelocity( );
}

vec3_t c_base_player::get_bone_position( int bone ) {
	matrix3x4 matrix[ 128 ];

	setup_bones( matrix, 128, 0x100, cl.m_globals->curtime );

	return vec3_t( matrix[ bone ][ 0 ][ 3 ], matrix[ bone ][ 1 ][ 3 ], matrix[ bone ][ 2 ][ 3 ] );
}

// bye fps lol
vec3_t c_base_player::get_hitbox_position( int hitbox ) {
	const auto model = get_model( );
	if ( !model ) return vec3_t{ };

	auto hdr = cl.m_modelinfo( )->GetStudioModel( model );
	if ( !hdr ) return vec3_t{ };

	matrix3x4 matrix[ 128 ];
	if ( !setup_bones( matrix, 128, 0x100, cl.m_globals->curtime ) ) return vec3_t{ };

	auto set = hdr->pHitboxSet( m_nHitboxSet( ) );
	if ( !set ) return vec3_t{ };

	auto box = set->pHitbox( hitbox );

	vec3_t hitbox_center = ( box->bbmin + box->bbmax ) * 0.5f;

	vec3_t transformed = math::vector_transform( hitbox_center, matrix[ box->bone ] );

	return transformed;
}

int c_base_player::get_team( ) {
	return m_iTeamNum( );
}

bool c_base_player::is_enemy( ) {
	auto local = g_ctx.m_local;
	if ( !local ) return false;
	return get_team( ) != local->get_team( );
}

int c_base_player::get_class( ) {
	int class_num = m_iClass( );
	return std::clamp( class_num, 0, 10 ); //to prevent crashes if its out of bounds
}

const char* c_base_player::get_class_name( ) {
	static const char* tf2_class_names[ ] = {
		"none",
		"scout",
		"sniper",
		"soldier",
		"demoman",
		"medic",
		"heavy",
		"pyro",
		"spy",
		"engineer",
		"invalid"
	};
	return tf2_class_names[ get_class( ) ];
}

byte c_base_player::get_life_state( ) {
	return m_lifeState( );
}

int c_base_player::get_flags( ) {
	return m_fFlags( );
}

int c_base_player::get_health( ) {
	return m_iHealth( );
}

int c_base_player::get_max_health( ) {
	return util::get_vfunc< 107, int >( this );
}

bool c_base_player::is_buffed( ) {
	return get_health( ) > get_max_health( );
}

bool c_base_player::is_alive( ) {
	return m_iHealth( ) > 0 && get_life_state( ) == LIFE_ALIVE;
}

int c_base_player::get_tick_base( ) {
	return m_nTickBase( );
}

bool c_base_player::is_visible( ) {
	auto local = cl.m_entlist( )->get_local_player( );
	if ( !local ) return true;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = local;

	ray.Init( local->get_eye_pos( ), get_eye_pos( ) );

	cl.m_trace( )->trace_ray( ray, MASK_AIM, &filter, &tr );

	return ( tr.m_pEnt == this || tr.fraction > 0.99f );
}

bool c_base_player::is_visible( int hitbox ) {
	auto local = cl.m_entlist( )->get_local_player( );
	if ( !local ) return true;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = local;

	ray.Init( local->get_eye_pos( ), get_hitbox_position( hitbox ) );

	cl.m_trace( )->trace_ray( ray, MASK_AIM, &filter, &tr );

	return ( tr.m_pEnt == this || tr.fraction > 0.99f );
}

dynamic_box_t c_base_player::get_dynamic_box( ) {
	const matrix3x4& trans = m_CollisionGroup( );

	vec3_t min = *reinterpret_cast< vec3_t* >( this + m_Collision( ) + 0x8 );
	vec3_t max = *reinterpret_cast< vec3_t* >( this + m_Collision( ) + 0x14 );

	vec3_t point_list[ ] = {
		vec3_t( min.x, min.y, min.z ),
		vec3_t( min.x, max.y, min.z ),
		vec3_t( max.x, max.y, min.z ),
		vec3_t( max.x, min.y, min.z ),
		vec3_t( max.x, max.y, max.z ),
		vec3_t( min.x, max.y, max.z ),
		vec3_t( min.x, min.y, max.z ),
		vec3_t( max.x, min.y, max.z )
	};

	vec3_t transformed[ 8 ];

	for ( int i{ }; i < 8; i++ ) {
		transformed[ i ] = math::vector_transform( point_list[ i ], trans );
	}

	vec2_t screen_pos[ 8 ]{ vec2_t( 0.f, 0.f ) };

	for ( int i = 0; i < 8; i++ ) {
		screen_pos[ i ] = graphics.world_to_screen( transformed[ i ] );
	}

	float left = FLT_MAX, top = FLT_MIN, right = FLT_MIN, bottom = FLT_MAX;

	for ( int i{ }; i < 8; i++ ) {
		left = std::min< float >( left, screen_pos[ i ].x );
		top = std::max< float >( top, screen_pos[ i ].y );
		right = std::max< float >( right, screen_pos[ i ].x );
		bottom = std::min< float >( bottom, screen_pos[ i ].y );
	}

	if ( right - left > 4000.f || top - bottom > 4000.f ) return{ 10000, 10000, 0, 0 }; //sanity check
	return{ ( int )left, ( int )bottom, ( int )( right - left ), ( int )( top - bottom ) + 4 };
}

c_base_weapon* c_base_player::get_active_weapon( ) {
	auto wep_handle = m_hActiveWeapon( );
	auto weapon = reinterpret_cast< c_base_weapon* >( cl.m_entlist( )->get_entity_from_handle( wep_handle ) );
	return weapon;
}
