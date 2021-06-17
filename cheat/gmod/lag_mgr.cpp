#include "lag_mgr.hpp"
#include "c_base_player.hpp"
#include "context.hpp"
#include "interface.hpp"
#include "base_cheat.hpp"

namespace features
{
	void c_lagmgr::on_antiaim( ) {
		static bool lag_flip{ }; //fuck you and ur cmdnr
		*m_sendpacket = lag_flip;
		lag_flip ^= 1;

		if( m_sent_ticks ) {
			*m_sendpacket = false;
		}
	}
	

	void c_lagmgr::fakelag( ) {
		//2017 sucked
		//i hope 2018 goes better

		if( !g_settings.rage.fakelag.mode( ) ||
			!g_settings.rage.fakelag.ticks( ) ) {
			m_breaking_lc = false;
			return;
		}

		auto& settings = g_settings.rage.fakelag;

		if( g_ctx.m_local->m_vecVelocity( ).length( ) < 0.1f ) {
			return;
		}

		vec3_t delta = g_ctx.m_last_origin - g_ctx.m_local->m_vecOrigin( );
		int max_ticks = math::min( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ? 10 : 16, g_settings.rage.fakelag.ticks( ) );
		bool force_send = m_held_ticks > max_ticks;
		if( settings.mode == 1 ) {
			if( delta.length2dsqr( ) > 4096.f ) {
				force_send = get_choked( ) > 3;
			}
		}

		if( settings.mode == 2 ) {
			auto fluc = math::random_number( -settings.fluctuate( ), settings.fluctuate( ) ) * 0.01f;
			force_send = m_held_ticks > math::min( max_ticks + ( int )fluc, 15 );
		}

		bool send = false;
		static bool was_onground{ };

		if( !settings.in_move ) {
			send = true;
		}

		if( settings.in_air && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			send = false;
		}

		if( settings.avoid_ground ) {
			if( !was_onground && g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) {
				send = false;
			}
		}

		was_onground = g_ctx.m_local->m_fFlags( ) & FL_ONGROUND;

		if( ( m_cmd->m_buttons & IN_ATTACK ) &&
			!settings.in_attack ) {
				send = true;
		}

		*m_sendpacket = ( send || force_send ) && !get_sent( );
		if( *m_sendpacket ) {
			m_breaking_lc = delta.length2dsqr( ) > 4096.f;
		}
	}
}