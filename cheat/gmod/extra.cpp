#include "base_cheat.hpp"
#include "context.hpp"
#include "input_system.hpp"
#include "math.hpp"
#include "hooks.hpp"
#include "interface.hpp"
#undef min

namespace features
{
	void c_extra::use_spammer( user_cmd_t* cmd ) {
		if( !g_settings.misc.use_spam( ) )
			return;

		if( !g_input.is_key_pressed( g_settings.misc.use_spam_key( ) ) )
			return;

		static bool toggle{ };

		if( toggle )
			cmd->m_buttons |= IN_USE;
		else
			cmd->m_buttons &= ~IN_USE;

		toggle ^= 1;
	}

	void c_extra::third_person( ) {
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
				g_gmod.m_input->m_fCameraInThirdPerson = false;
				enabled = false;
			}

			return;
		}

		vec3_t viewangles{ };
		if( g_ctx.m_local->is_alive( ) ) {
			g_gmod.m_engine( )->GetViewAngles( viewangles );
			g_gmod.m_input->m_fCameraInThirdPerson = true;
			g_gmod.m_input->m_vecCameraOffset = vec3_t( viewangles.x, viewangles.y, 150.0f );

			constexpr float    cam_hull_offset{ 16.f };

			const     vec3_t   cam_hull_min( -cam_hull_offset, -cam_hull_offset, -cam_hull_offset );
			const     vec3_t   cam_hull_max( cam_hull_offset, cam_hull_offset, cam_hull_offset );
			vec3_t             cam_forward, origin = g_ctx.m_local->get_eye_pos( );

			cam_forward = math::angle_vectors( vec3_t( viewangles.x, viewangles.y, 0.f ) );

			CTraceFilterWorldAndPropsOnly filter;
			Ray_t						  ray;
			CGameTrace					  tr;

			ray.Init( origin, origin - ( cam_forward * 150.f ), cam_hull_min, cam_hull_max );

			g_gmod.m_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr );
			g_gmod.m_input->m_vecCameraOffset.z = 150.f * tr.fraction;

			enabled = true;
		}
		else if( g_gmod.m_entlist( )->GetClientEntityFromHandle< >( g_ctx.m_local->m_hObserverTarget( ) ) ) {
			g_gmod.m_input->m_fCameraInThirdPerson = false;
			g_ctx.m_local->m_iObserverMode( ) = 5;
		}
	}
}