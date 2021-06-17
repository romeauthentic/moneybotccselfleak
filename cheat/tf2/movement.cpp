#include "movement.h"
#include "interfaces.h"
#include "settings.h"

namespace features {
	void c_movement::bhop( ) {
		if( !g_settings.misc.bunny_hop )
			return;

		if( g_ctx.m_local->m_nMoveType( ) == MOVETYPE_LADDER ||
			g_ctx.m_local->m_nMoveType( ) == MOVETYPE_NOCLIP )
			return;

		//jump like you nohat -> WELL THERES FUCKING HATS IN THIS GAME ISNT THERE CUNT
		if( m_cmd->m_buttons & IN_JUMP && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			m_cmd->m_buttons &= ~IN_JUMP;
		}
	}


	//ehhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh lazy
	void c_movement::autostrafe( ) {
		if( !g_settings.misc.auto_strafe )
			return;

		float velocity = g_ctx.m_local->m_vecVelocity( ).length2d( );
		auto cmd = g_ctx.get_last_cmd( );

		if( cmd && m_cmd->m_buttons & IN_JUMP && velocity > 1.0f ) {
			if( !cmd->m_forwardmove && !cmd->m_sidemove ) {
				if( !cmd->m_mousedx ) {
					cmd->m_forwardmove = std::min< float >( 450.f, 5850.f / velocity );
					cmd->m_sidemove = ( m_cmd->m_cmd_nr % 2 ) == 0 ? -450.f : 450.f;
				}
				else {
					cmd->m_sidemove = m_cmd->m_mousedx < 0.f ? -450.f : 450.f;
				}
			}
		}
	}
}
