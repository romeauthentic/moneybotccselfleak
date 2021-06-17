#include "legitbot.hpp"
#include "interface.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "math.hpp"
#include "base_cheat.hpp"
#include "input_system.hpp"
#include "d3d.hpp"

namespace features
{
	bool c_legitbot::lag_record_t::is_valid( ) {
		return util::is_tick_valid( m_tickcount );
	}

	void c_legitbot::c_lagcomp::store_player( int ent_index ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		vec3_t pos = ent->get_hitbox_pos( 0 );
		int tickcount = TIME_TO_TICKS( ent->m_flSimulationTime( ) );

		lag_record_t new_record;
		if ( ent->ce( )->SetupBones( new_record.m_matrix, 128, 0x100, 0.f ) ) {
			new_record.m_position = pos;
			new_record.m_tickcount = tickcount;

			m_data[ ent_index ].push_front( new_record );
		}

		while ( !m_data[ ent_index ].empty( ) &&
			( m_data[ ent_index ].size( ) > TIME_TO_TICKS( 1.0f ) ) ) {
			m_data[ ent_index ].pop_back( );
		}
	}

	bool c_legitbot::c_lagcomp::can_backtrack_entity( int ent_index ) {
		if ( m_data[ ent_index ].empty( ) ) {
			return false;
		}

		for( auto& it : m_data[ ent_index ] ) {
			if( it.is_valid( ) ) return true;
		}

		return false;
	}

	c_legitbot::lag_record_t* c_legitbot::c_lagcomp::find_best_record( int ent_index ) {
		c_legitbot::lag_record_t* best_record = nullptr;
		float best_fov{ g_settings.legit.backtracking_fov( ) };
		vec3_t cur_angle;
		vec3_t aim_angle;
		vec3_t aim_pos;
		vec3_t cur_pos;
		int cur_tick;

		if( m_data[ ent_index ].empty( ) ) {
			return nullptr;
		}

		if( g_settings.misc.net_fakelag( ) && g_settings.misc.net_fakeping_active ) {
			best_fov = 360.f;
		}
		
		g_csgo.m_engine( )->GetViewAngles( cur_angle );
		cur_pos = g_ctx.m_local->get_eye_pos( );
		cur_tick = g_csgo.m_globals->m_tickcount;

		for( auto& it : m_data[ ent_index ] ) {
			int delta = std::abs( cur_tick - it.m_tickcount );

			if( !( g_settings.misc.net_fakelag( ) && g_settings.misc.net_fakeping_active ) ) {
				auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
				if( nci ) {
					float max_latency = g_settings.legit.backtracking_time + nci->GetLatency( 0 );
					if( delta > TIME_TO_TICKS( max_latency ) )
						continue;
				}
			}

			if( !it.is_valid( ) )
				continue;

			aim_pos = it.m_position;
			aim_angle = math::vector_angles( cur_pos, aim_pos ).clamp( );

			float fov = ( cur_angle - aim_angle ).clamp( ).length2d( );
			if ( fov < best_fov ) {
				best_fov = fov;
				best_record = &it;
			}
		}

		return best_record;
	}

	vec3_t c_legitbot::c_lagcomp::get_backtracked_position( int ent_index ) {
		if( !m_data[ ent_index ].size( ) ) return vec3_t( );
		auto lag_record = find_best_record( ent_index );

		vec3_t last_position{ };
		for( auto& it : util::reverse_iterator( m_data[ ent_index ] ) ) {
			if( it.is_valid( ) ) last_position = it.m_position; break;
		}

		return lag_record ? lag_record->m_position : last_position;
	}

	void c_legitbot::c_lagcomp::store( ) {
		for ( int i = 1; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if ( ent && ent->is_valid( ) && ent != g_ctx.m_local ) {
				store_player( i );
			}
			else {
				m_data[ i ].clear( );
			}
		}
	}

	bool c_legitbot::c_lagcomp::backtrack_entity( int ent_index ) {
		if ( !m_cmd ) return false;
		
		if ( !can_backtrack_entity( ent_index ) ) return false;

		auto best_record = find_best_record( ent_index );

		if ( best_record ) {
			m_cmd->m_tick_count = best_record->m_tickcount;
			return true;
		}

		return false;
	}

	void c_legitbot::c_lagcomp::operator()( user_cmd_t* cmd ) {
		if ( !g_settings.legit.enabled( ) || !g_settings.legit.backtracking( ) )
			return;

		m_cmd = cmd;
		store( );

		auto target = g_cheat.m_legitbot.get_aim_target( g_settings.legit.backtracking_fov( ) );
		bool active = false;

		if ( g_settings.legit.activation_type( ) == 0 )
			active = true;
		if ( g_input.is_key_pressed( ( VirtualKeys_t )g_settings.legit.key( ) ) )
			active = true;

		if ( active && target != -1 ) {
			backtrack_entity( target );
		}
	}
}