#pragma once
#include "sdk.hpp"
#include "settings.hpp"
#include "console.hpp"

namespace features
{
	class c_lagmgr {
	public:
		void operator()( user_cmd_t* cmd, byte* sendpacket ) {
			if( !sendpacket ) return;
			if( !cmd ) return;

			m_cmd = cmd;
			m_sendpacket = sendpacket;

			if( g_settings.rage.anti_aim( ) ) {
				on_antiaim( );
			}

			//let fakelag override whatever happened in antiaim
			//this also means we will call stuff like fakewalk etc
			//after this
			fakelag( );
		}

		void on_cmove_end( );

		void set_state( bool state ) {
			*m_sendpacket = state;
		}

		int get_choked( ) const {
			return m_held_ticks;
		}

		bool has_fired( ) const {
			return m_has_fired;
		}

		int get_sent( ) const {
			return m_sent_ticks;
		}

		bool get_state( ) const {
			return m_sendpacket ? !!*m_sendpacket : true;
		}

		bool is_breaking_lc( ) const {
			return m_breaking_lc;
		}

		int get_last_choke( ) const {
			return m_last_choke;
		}

	private:
		void on_antiaim( );
		bool predict_position( float dist );
		void fakelag( );

		user_cmd_t* m_cmd{ };
		byte* m_sendpacket{ };
		int m_sent_ticks{ };
		int m_held_ticks{ };
		int m_last_choke{ };
		bool m_breaking_lc{ };
		bool m_has_fired{ };

	public:
		bool m_is_peeking;
		matrix3x4 m_peek_matrix[ 128 ];
	};
}