#pragma once
#include "sdk.hpp"

namespace features
{
	constexpr int CHEATER_SIMTIME_THRESHOLD = 10;
	constexpr int CHEATER_AA_THRESHOLD = 50;
	constexpr int CHEATER_MAX_DETECTIONS = 200;

	class c_player_record {
		float m_last_simtime;
		int m_simtime_detections;
		int m_aa_detections;
		bool m_is_cheater;
		
		c_base_player* m_ent;

		void clear( ) {
			m_last_simtime = { };
			m_simtime_detections = { };
			m_aa_detections = { };
			m_is_cheater = { };
			m_ent = { };
		}

		void update_simtime( ) {
			if( !m_ent ) return;

			if( m_ent->m_flSimulationTime( ) == m_last_simtime ) {
				m_simtime_detections += 2;
			}
			else if( m_simtime_detections > 0 ) {
				m_simtime_detections--;
			}
			m_last_simtime = m_ent->m_flSimulationTime( );
		}

		void update_antiaim( ) {
			if( !m_ent ) return;

			auto pitch = m_ent->m_angEyeAngles( ).x;
			if( pitch > 75.f && m_aa_detections < CHEATER_MAX_DETECTIONS ) {
				m_aa_detections++;
			}
			else if( m_aa_detections > 0 ) {
				m_aa_detections--;
			}
		}

		void update_cheater( ) {
			m_is_cheater = ( m_simtime_detections > CHEATER_SIMTIME_THRESHOLD
				|| m_aa_detections > CHEATER_AA_THRESHOLD )
				&& m_simtime_detections;
		}

	public:
		c_player_record( ) { clear( ); }
		bool is_cheater( ) const { return m_is_cheater; }
		void update( int ent_index );
	};

	class c_player_manager {
		std::array< c_player_record, 65 > m_players;
	public:
		void frame_stage_notify( );
		bool is_cheater( int ent_index );
	};
}