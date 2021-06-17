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
	
	bool c_lagmgr::predict_position( float dist ) {
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

			//fuck getting pulled back on peek
			position += velocity_dir * 20.f;

			float dmg = g_cheat.m_autowall.run( ent, g_ctx.m_local, position, false );

			if( dmg > 25.f )
				return false;
		}

		return true;
	}

	void c_lagmgr::fakelag( ) {
        if ( !g_settings.rage.fakelag.mode ) {
            return;
        }

		//2017 sucked
		//i hope 2018 goes better

		//2018 sucked
		//but 2019 will be better
		
		//2019 was better
		//maybe 2020 will be good

		static float last_rand = 0.f;

		auto& settings = g_settings.rage.fakelag;

		auto weapon = g_ctx.m_local->get_weapon( );

		//if( g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) ) {
		//	return;
		//}

		bool moving = g_ctx.m_local->m_vecVelocity( ).length2d( ) > 0.f;

		vec3_t delta = g_ctx.m_last_origin - g_ctx.m_local->m_vecOrigin( );
		int max_ticks = g_settings.rage.fakelag.ticks( );
		if( settings.fluctuate )
			max_ticks *= last_rand;

		static bool changed = false;
		static auto cl_sendmove = pattern::first_code_match( g_csgo.m_engine.dll( ), xors( "55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98" ), 0 );
		
		if( !changed ) {
			ulong_t old;
			VirtualProtect( ( void* )( cl_sendmove + 0xbd ), 1, PAGE_EXECUTE_READWRITE, &old );
			*( uint8_t* )( cl_sendmove + 0xbd ) = 50;
			VirtualProtect( ( void* )( cl_sendmove + 0xbd ), 1, old, nullptr );
		}

		changed = true;

		max_ticks = math::min( max_ticks, 16 );

		bool force_send = m_held_ticks >= max_ticks;
		bool send = true;

		if( ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) && g_settings.rage.anti_aim( ) && g_settings.rage.fake_yaw ) {
			if( !moving )
				send = get_choked( ) >= 3;
			else if( !settings.in_move )
				send = get_choked( ) >= 3;
		}

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



		static bool was_onground{ };

		if( settings.in_move && ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) && moving ) {
			send = false;
		}

		if( settings.on_peek && moving ) {
			static int	choke_ticks = 0;

			bool peeking = predict_position( 20.f + g_ctx.m_local->m_vecVelocity( ) * TICK_INTERVAL( ) * 2.f );
			bool predicted = predict_position( 20.f );
			
			if( !peeking && predicted && !choke_ticks ) {
				send = true;
			}
			else if( !predicted ) {
				choke_ticks = settings.ticks;
				if( get_choked( ) > 1 )
					send = true;
			}

			if( choke_ticks ) {
				send = !choke_ticks--;
				if( !m_is_peeking )
					memcpy( m_peek_matrix, g_ctx.m_local->m_CachedBoneData( ).GetElements( ), sizeof( m_peek_matrix ) );

				m_is_peeking = true;
			}
			else {
				m_is_peeking = false;
			}
		}
		else {
			m_is_peeking = false;
		}

		if( settings.in_air && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
			send = false;
		}

		if( settings.avoid_ground ) {
			static int choked = 0;

			if( !was_onground && ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
				choked = settings.ticks;
			}

			if( choked )
				send = !--choked;
		}

		if( settings.jump_reset ) {
			if( settings.avoid_ground ) {
				if( was_onground && !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
					send = get_choked( ) > 0;
				}
			}
			else {
				if( !was_onground && ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
					send = get_choked( ) > 0;
				}
			}
		}

		was_onground = ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND );

		if( settings.on_duck && ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) { 
			float amt = g_ctx.m_local->m_flDuckAmount( );
			if( m_cmd->m_buttons & IN_DUCK ) {
				if( amt < 1.0f )
					send = false;
			} else if( amt > 0.f )
					send = false;
		}

		static int wep_choke = 0;
		if( wep_choke )
			send = !--wep_choke;

		if( m_cmd->m_weaponselect && settings.on_select ) {
			wep_choke = max_ticks;
		}

		if( ( m_cmd->m_buttons & IN_ATTACK ) && g_ctx.m_local->can_attack( ) &&
			!settings.in_attack ) {
			if( !weapon->is_grenade( ) || ( weapon->is_grenade( ) && weapon->m_fThrowTime( ) < TICK_INTERVAL( ) ) ) {
				send = get_choked( ) > 0;
			}
		}

		*m_sendpacket = send || force_send;

		if( *m_sendpacket ) {
			m_breaking_lc = delta.length2d( ) > 64.f;
			last_rand = ( 1.f + math::random_number( -settings.fluctuate, settings.fluctuate( ) ) * 0.01f );
		}
	}

	void c_lagmgr::on_cmove_end( ) {
		if( !m_sendpacket ) return;

		if( *m_sendpacket ) {
			m_last_choke = m_held_ticks;
			++m_sent_ticks;
			m_held_ticks = 0;
			m_has_fired = false;
		}
		else {
			++m_held_ticks;
			m_sent_ticks = 0;

			if( ( m_cmd->m_buttons & IN_ATTACK ) && g_ctx.m_local->can_attack( ) ) {
				g_ctx.m_thirdperson_angle = g_ctx.m_last_realangle = m_cmd->m_viewangles;

				g_ctx.m_local->get_animdata( ).m_poseparams.at( BODY_PITCH ) = ( m_cmd->m_viewangles.x + 90.f ) / 180.f;
				m_has_fired = true;
			}
		}
	}
}