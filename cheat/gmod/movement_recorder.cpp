#include "movement_recorder.hpp"
#include "input_system.hpp"
#include "settings.hpp"
#include "interface.hpp"

namespace features
{
	void c_move_recorder::operator()( user_cmd_t* cmd ) {
		if( !g_settings.misc.recorder_enable )
			return;

		if( g_input.is_key_pressed( g_settings.misc.recording_start_key( ) ) )
			start_recording( );

		if( g_input.is_key_pressed( g_settings.misc.recording_stop_key( ) ) )
			stop_recording( );

		if( m_recording ) {
			record_cmd( cmd );
		}

		if( m_playing ) {
			if( m_move_data.empty( ) ) {
				m_playing = false;
			}
			else if( ++m_record_index >= m_move_data.size( ) ) {
				m_playing = false;
			}
			else {
				auto& old_cmd = m_move_data.at( m_record_index );

				if( g_settings.misc.recording_show_angles == 1 && ( old_cmd.m_buttons & IN_ATTACK ) ||
					g_settings.misc.recording_show_angles == 2 ) {
					g_csgo.m_engine( )->SetViewAngles( old_cmd.m_viewangles );
				}

				cmd->m_buttons = old_cmd.m_buttons;
				cmd->m_aimdirection = old_cmd.m_aimdirection;
				cmd->m_forwardmove = old_cmd.m_forwardmove;
				cmd->m_impulse = old_cmd.m_impulse;
				cmd->m_mousedx = old_cmd.m_mousedx;
				cmd->m_mousedy = old_cmd.m_mousedy;
				cmd->m_sidemove = old_cmd.m_sidemove;
				cmd->m_upmove = old_cmd.m_upmove;
				cmd->m_viewangles = old_cmd.m_viewangles;
				cmd->m_weaponselect = old_cmd.m_weaponselect;
				cmd->m_weaponsubtype = old_cmd.m_weaponsubtype;
			}
		}
		else {
			m_record_index = 0;
		}
	}

	void c_move_recorder::record_cmd( user_cmd_t* cmd ) {
		m_move_data.push_back( *cmd );
	}

	void c_move_recorder::start_recording( ) {
		m_recording = true;
	}

	void c_move_recorder::stop_recording( ) {
		m_recording = false;
	}

	void c_move_recorder::play_back( ) {
		m_recording = false;
		m_playing = true;
	}

	void c_move_recorder::stop_playback( ) {
		m_playing = false;
	}

	void c_move_recorder::clear_recording( ) {
		m_move_data.clear( );
	}
}