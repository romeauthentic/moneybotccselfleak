#pragma once
#include <vector>
#include "vector.hpp"
#include "color.hpp"
#include <unordered_map>
#include "strings.hpp"
#include "IVRenderView.h"
class c_base_player;

namespace features
{
	class c_visuals {
	private:
		static const clr_t esp_green( uint8_t alpha = 255 ) {
			return clr_t( 1, 216, 62, alpha );
		}
		static const clr_t esp_red( uint8_t alpha = 255 ) {
			return clr_t( 240, 0, 0, alpha );
		}
		static const clr_t esp_blue( uint8_t alpha = 255 ) {
			return clr_t( 84, 173, 247, alpha );
		}

		void draw_line( const vec2_t& a, const vec2_t& b, const clr_t& clr );
		void draw_line( int x, int y, int x1, int y1, const clr_t& clr );
		void draw_rect( int x, int y, int w, int h, const clr_t& clr );
		void draw_filled_rect( int x, int y, int w, int h, const clr_t& clr );
		void draw_circle( int x, int y, int r, const clr_t& clr, int res = 48 );

		void draw_really_big_string( int x, int y, const clr_t& clr, const char* msg, ... );
		void draw_string( int x, int y, int align, bool big, const clr_t& clr, const char* msg, ... );

		void get_text_size( int& w, int& h, const char* msg, ... );

		void draw_local( );
		void draw_players( );
		void draw_world( );

		void draw_hits( );
		void spectator_list( );
		//void throwable_prediction( );

		void invalidate_glow( );

		vec3_t m_stored_pos[ 65 ]{ };
		float  m_anim_progress[ 65 ]{ };
		float  m_last_hit{ };
		int    m_ent_dmg[ 65 ]{ };
	public:
		void update_glow( );
		void world_modulate( );
		void update_position( int index, const vec3_t& pos );
		void out_of_fov( c_base_player* ent, const vec3_t& pos, clr_t col );
		void store_hit( );
		void operator()( );
	};
}