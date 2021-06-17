#pragma once
#include "sdk.hpp"
#include "vector.hpp"

namespace features
{
	class c_move_recorder {
	public:
		void play_back( );
		void stop_playback( );
		void clear_recording( );

		bool is_recording( ) const { return m_recording; }
		bool is_playing( ) const { return m_playing; }
		int  get_record_count( ) const { return m_move_data.size( ); }
		auto get_current_record( ) const { return m_record_index; }

		void operator()( user_cmd_t* );
	private:
		void record_cmd( user_cmd_t* cmd );
		void start_recording( );
		void stop_recording( );

		bool m_recording = false;
		bool m_playing = false;
		size_t m_record_index = 0;
		std::vector< user_cmd_t > m_move_data;
	};
}