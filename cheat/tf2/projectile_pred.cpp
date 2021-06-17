#include "projectile_pred.h"
#include "sdk.h"
#include "interfaces.h"

namespace util
{

	vec3_t c_projectile_pred::predict_player( c_base_player* player ) {
		if ( !player || !player->is_valid( ) ) return vec3_t{ };

		static auto sv_gravity = cl.m_cvar( )->FindVar( "sv_gravity" ); //unnecessary for now
		auto local = g_ctx.m_local;								//will be used for stuff like grenades
																		//will also need to calculate friction etc
		auto weapon = local->get_active_weapon( );
		if ( !weapon ) return player->get_origin( );

		float projectile_speed = get_projectile_speed( weapon );

		vec3_t player_vel = player->get_velocity( );
		vec3_t projectile_vel = vec3_t( );

		float estimated_travel_time = ( local->get_eye_pos( ) - player->get_origin( ) ).length( ) / projectile_speed + cl.m_globals->interval_per_tick;
		vec3_t estimated_predicted_pos = player->get_origin( ) + player_vel * estimated_travel_time;

		return estimated_predicted_pos;
	}

	float c_projectile_pred::get_projectile_speed( c_base_weapon* weapon ) {
		if ( !weapon )
			return -1.f;
		/*
			this is completely hardcoded
			if you can find a better way to figure this out
			then please tell me
			-- nave
		*/

		int id = weapon->GetClientClass( )->m_class_id;

		//return flSpeed;
		switch ( id ) {
		case ClientClassIDs::CTFGrenadeLauncher:
			return 840.0f;
		case ClientClassIDs::CTFRocketLauncher_DirectHit:
			return 1980.0f;
		case ClientClassIDs::CTFRocketLauncher:
		case ClientClassIDs::CTFRocketLauncher_Mortar:
			return 1100.0f;
		case ClientClassIDs::CTFFlareGun:
			return 1450.0f;
		case ClientClassIDs::CTFBat_Wood:
			return 1940.0f;
		case ClientClassIDs::CTFSyringeGun:
			return 990.0f;
		case ClientClassIDs::CTFCompoundBow: {
			return 0.f; //need more shit, cba now
		}

		default:
			return -1.0f;
		}

		return -1.0f;
	}
}

util::c_projectile_pred projectile_pred;