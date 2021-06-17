#include "base_cheat.h"
#include "ctx.hpp"
#include "input_system.hpp"
#include "math.h"
#include "hooks.h"
#include "interfaces.h"
#undef min
#include "extra.h"
#include "settings.h"

namespace features
{
	//crash too lazy to fix rn will do , who wants to fly out of their body and become their own guardian angel anyway?
	void c_extra::thirdperson( ) {
		if( !g_ctx.m_local )
			return;

		static bool enabled = false;
		static bool toggle = true;
		static bool key_held = false;


		if( g_input.is_key_pressed( ( VirtualKeys_t )g_settings.misc.thirdperson_key( ) ) ) {
			if( !key_held ) {
				toggle ^= 1;
			}
			key_held = true;
		}
		else {
			key_held = false;
		}

		if( !g_settings.misc.thirdperson || !toggle ) {
			if( enabled ) {
				cl.m_hl_input->m_fCameraInThirdPerson = false;
				enabled = false;
			}

			return;
		}

		vec3_t viewangles{ };
		if( g_ctx.m_local->is_alive( ) ) {
			cl.m_engine( )->GetViewAngles( viewangles );
			cl.m_hl_input->m_fCameraInThirdPerson = true;
			cl.m_hl_input->m_vecCameraOffset = vec3_t( viewangles.x, viewangles.y, 150.0f );

			float    cam_hull_offset{ 16.f };

			vec3_t   cam_hull_min( -cam_hull_offset, -cam_hull_offset, -cam_hull_offset );
			vec3_t   cam_hull_max( cam_hull_offset, cam_hull_offset, cam_hull_offset );
			vec3_t             cam_forward, origin = g_ctx.m_local->get_eye_pos( );

			cam_forward = math::angle_vectors( vec3_t( viewangles.x, viewangles.y, 0.f ) );

			CTraceFilterWorldAndPropsOnly filter;
			Ray_t						  ray;
			CGameTrace					  tr;

			ray.Init( origin, origin - ( cam_forward * 150.f ), cam_hull_min, cam_hull_max );

			cl.m_trace( )->trace_ray( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr );
			cl.m_hl_input->m_vecCameraOffset.z = 150.f * tr.fraction;

			enabled = true;
		}
		else if( cl.m_entlist( )->get_entity_from_handle< c_base_player >( g_ctx.m_local->m_hObserverTarget( ) ) ) {
			cl.m_hl_input->m_fCameraInThirdPerson = false;
			g_ctx.m_local->m_iObserverMode( ) = 5;
		}
	}

	//hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
	void c_extra::float_ragdolls( ) {
		for( int i{ }; i < cl.m_entlist( )->get_highest_entity_index( ); ++i ) {
			auto ent = cl.m_entlist( )->get_client_entity< IClientEntity >( i );
			if( !ent ) continue;

			auto client_class = ent->get_client_class( );
			if( !client_class ) continue;

			int class_id = client_class->m_class_id;
			if( class_id != CTFRagdoll )
				return;

			ent->as< c_base_player >( )->m_vecRagdollVelocity( ) *= -1;
		}
	}

	
}