#include "lag_mgr.hpp"
#include "c_base_player.hpp"
#include "context.hpp"
#include "interface.hpp"
#include "base_cheat.hpp"

namespace features
{
	void c_lagmgr::on_antiaim( ) {
		if( m_sent_ticks ) {
			*m_sendpacket = false;
		}
	}
	
	bool c_lagmgr::predict_position( ) {
		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );

		auto i = util::get_closest_player( );
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

		if( !ent || !ent->is_valid( ) )
			return true;
		
		auto wep = ent->get_weapon( );
		if( !wep ) {
			return true;
		}

		float dmg_begin = g_cheat.m_autowall.run( ent, g_ctx.m_local, local_pos, false );
		bool is_visible = dmg_begin > 10.f;

		if( !is_visible ) {
			vec3_t position = local_pos;

			vec3_t velocity_ang = math::vector_angles( vec3_t( ), g_ctx.m_local->m_vecVelocity( ) );
			vec3_t velocity_dir = math::angle_vectors( velocity_ang );

			position += velocity_dir * 15.f;

			float dmg = g_cheat.m_autowall.run( ent, g_ctx.m_local, position, false );

			if( dmg > 25.f )
				return false;
		}

		return true;
	}

	void c_lagmgr::fakelag( ) {
		//2017 sucked
		//i hope 2018 goes better

		if( !g_settings.rage.fakelag.mode( ) ||
			!g_settings.rage.fakelag.ticks( ) ) {
			m_breaking_lc = false;
			return;
		}

		static float last_rand = 0.f;

		auto& settings = g_settings.rage.fakelag;

		auto weapon = g_ctx.m_local->get_weapon( );

		if( g_ctx.m_local->m_vecVelocity( ).length( ) < 0.1f ||
			g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) ) {
			return;
		}

		vec3_t delta = g_ctx.m_last_origin - g_ctx.m_local->m_vecOrigin( );
		int max_ticks = g_settings.rage.fakelag.ticks( );
		if( settings.fluctuate )
			max_ticks *= last_rand;

		max_ticks = math::min( max_ticks, 14 );

		bool force_send = m_held_ticks > max_ticks;
		if( settings.mode == 1 ) {
			if( delta.length2dsqr( ) > 4096.f ) {
				force_send = get_choked( ) > 3;
			}
		}

		if( settings.mode == 2 ) {
			static bool broken_lc = false;
			bool should_send = false;

			if( broken_lc ) {
				should_send = math::random_number( 0, 100 ) % 2;
				if( get_choked( ) > 1 )
					broken_lc = false;
			}

			if( should_send )
				force_send = true;
			else if( delta.length2dsqr( ) > 4096.f ) {
				force_send = get_choked( ) > 3;
				broken_lc = true;
			}
		}

		bool send = false;
		static bool was_onground{ };

		if( !settings.in_move ) {
			send = true;
		}

		if( settings.on_peek ) {
			static int	choke_ticks = 0;
			bool predicted = predict_position( );
			if( !predicted )
				choke_ticks = settings.ticks;

			if( choke_ticks ) {
				send = !choke_ticks--;
			}
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

		if( ( m_cmd->m_buttons & IN_ATTACK ) && g_ctx.m_local->can_attack( ) &&
			!settings.in_attack ) {
			if( !weapon->is_grenade( ) || ( weapon->is_grenade( ) && weapon->m_fThrowTime( ) < TICK_INTERVAL( ) ) ) {
				send = true;
			}
		}

		*m_sendpacket = ( send || force_send ) && !get_sent( );
		if( *m_sendpacket ) {
			m_breaking_lc = delta.length2dsqr( ) > 4096.f;
			last_rand = ( 1.f + math::random_number( -settings.fluctuate, settings.fluctuate( ) ) * 0.01f );
		}
	}
}