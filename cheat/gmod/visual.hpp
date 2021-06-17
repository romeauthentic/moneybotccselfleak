#pragma once
#include <vector>
#include "interface.hpp"
#include "vector.hpp"
#include "color.hpp"
#include "d3d.hpp"
#include <unordered_map>
#include "strings.hpp"
#include "IVRenderView.hpp"

class c_base_player;

namespace features
{
	class c_visuals
	{
		void draw_line( const vec2_t& a, const vec2_t& b, const clr_t& clr );
		void draw_line( int x, int y, int x1, int y1, const clr_t& clr );
		void draw_rect( int x, int y, int w, int h, const clr_t& clr );
		void draw_filled_rect( int x, int y, int w, int h, const clr_t& clr );
		void draw_circle( int x, int y, int r, const clr_t& clr, int res = 48 );

		void draw_really_big_string( int x, int y, const clr_t& clr, const char* msg, ... );
		void draw_string( int x, int y, int align, bool big, const clr_t& clr, const char* msg, ... );

		struct stored_player_data_t
		{
			std::string m_rank{ };
			clr_t m_team_color{ };
			matrix3x4 m_matrix[ 128 ]{ };
			vec3_t m_pos{ };
			matrix3x4 m_coordinate_frame{ };
		};

		struct stored_data_t
		{
			std::array< stored_player_data_t, 256 > m_player{ };
			VMatrix m_matrix{ };
		};

		stored_data_t m_data{ };

		void spectator_list( );
		void draw_players( );

	public:
		void operator( )( );
		void store_data( );
		VMatrix& get_matrix( );
	};
}