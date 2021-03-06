#include "hooks.hpp"
#include "settings.hpp"

#include <intrin.h>
#include "context.hpp"
#include "mem.hpp"

void __fastcall hooks::get_screen_size( void* ecx, void* edx, int& w, int& h ) {
	// ref: D1 F8 89 44 24 10, Inaccuracy =\t%f\tSpread =\t%f\tSpreadDistance =\t%f\tPlayer Velocity =\t%f\n
	static auto draw_crosshair_h = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "8B 0D ? ? ? ? 0F 57 C9 99" ) );

	stack_t stack( get_baseptr( ) );
	auto	ret = stack.next( ).return_address( );
	//call original
	get_screen_size_o( ecx, edx, w, h );

	//alter screen width and height to adjust the crosshair position on screen
	if( g_settings.visuals.recoil_crosshair && g_ctx.m_local ) {
		if( ret == draw_crosshair_h || ret == draw_crosshair_h - 14 /* width call is 14 bytes above height call*/ ) {
			w /= 2;
			h /= 2;

			vec3_t punch_angles = g_ctx.m_local->m_aimPunchAngle( ) * 2.f;

			if( punch_angles.x < -0.1f ) {
				float fov = g_ctx.m_fov;
				float angle_step = h / fov;

				w -= int( punch_angles.y * angle_step );
				h += int( punch_angles.x * angle_step );
			}

			w *= 2;
			h *= 2;
		}
	}
}