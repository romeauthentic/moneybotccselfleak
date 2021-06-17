#pragma once
#include <vector>
#include "vector.hpp"
#include "color.hpp"
#include <unordered_map>
#include "strings.hpp"
#include "IVRenderView.hpp"
class c_base_player;

enum HitFlag_t {
	HIT_NONE = 0,
	HIT_ALERT = 1,
	HIT_1W = 2
};

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


		void draw_local( );
		void get_text_size(int& w, int& h, const char* msg, ...);
		void draw_players( );
		void draw_world( );

		void draw_hits( );
		void spectator_list( );
		void grenade_prediction( );
		void draw_firegrenade( );

		struct tracer_t {
			float m_time;
			vec3_t m_start;
			vec3_t m_end;
			bool m_local;
		};

		//you should always store the entity index rather than the ptr
		struct sound_t {
			float m_time;
			vec3_t m_pos;
			int m_ent;
		};

		struct firegrenade_t {
			float m_time;
			vec3_t m_pos;
			int m_ent;
		};

		struct shot_t {
			float time;
			vec3_t pos;
			bool hit;
		};

		std::vector< firegrenade_t > m_firegrenades;
		std::vector< shot_t > m_shots;
		std::vector< sound_t > m_sounds;
		std::vector< tracer_t > m_tracers;

		vec3_t m_stored_pos[ 65 ]{ };
		float  m_anim_progress[ 65 ]{ };
		float  m_last_hit{ };
		int    m_ent_dmg[ 65 ]{ };
		int    m_teamdmg{ 0 };
		int	   m_hit_flag[ 65 ]{ };
		float  m_last_roundstart{ };
	public:
		void update_glow( );
		void world_modulate( );
		void update_position( int index, const vec3_t& pos );
		void store_tracer( int ent_index, vec3_t shot );
		void store_sound( int ent, vec3_t origin );
		void store_firegrenades( int ent, vec3_t origin );
		void draw_autowall( );
		void draw_spread( );
		void out_of_fov( c_base_player* ent, const vec3_t& pos, clr_t col );
		void store_hit( );
		void radar( );
		void draw_tracers( );
		void store_ent_dmg( int, int, int );
		void store_shot( vec3_t pos, bool hit = false );
		void draw_shots( );
		void reset_local_dmg( );
		void on_round_start( );
		void draw_sound( );
		void update_hit_flags( );
		void operator()( );
	};
}