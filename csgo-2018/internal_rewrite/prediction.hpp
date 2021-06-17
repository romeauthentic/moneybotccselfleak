#pragma once
#include "vector.hpp"
#include "util.hpp"

#include <array>
#include <deque>

class user_cmd_t;
class c_base_player;

NAMESPACE_REGION( features )

class c_prediction
{
	user_cmd_t* m_ucmd{ };
	int m_predicted_flags{ };

	struct player_data_t {
		struct lag_velocity_record_t {
			vec3_t m_velocity;
			int	   m_tick;
		};

		void update( int ent_index );

		float  m_simtime{ };
		int	   m_last_choke{ };
		vec3_t m_angles{ };
		vec3_t m_velocity{ };
		vec3_t m_old_velocity{ };
		vec3_t m_position{ };
		vec3_t m_movement{ };
		bool   m_valid{ };
		bool   m_breaking_lc{ };

		std::deque< lag_velocity_record_t > m_records{ };
	};

	std::array< player_data_t, 65 > m_players;

	void run( user_cmd_t* ucmd );
public:
	void local_pred( user_cmd_t* );

	int get_predicted_flags( ) const {
		return m_predicted_flags;
	}

	bool is_breaking_lc( int ent ) const {
		return m_players[ ent ].m_breaking_lc;
	}

	inline player_data_t get_player_data( int ent_index ) {
		return m_players[ ent_index ];
	}

	player_data_t m_player;

	//gamemovement functions
	vec3_t full_walk_move( c_base_player*, int );
	void check_jump_button( c_base_player*, vec3_t&, vec3_t& );
	void start_gravity( c_base_player*, vec3_t&, vec3_t& );
	void finish_gravity( c_base_player*, vec3_t&, vec3_t& );
	void friction( c_base_player*, vec3_t&, vec3_t& );
	void air_move( c_base_player*, vec3_t&, vec3_t&, vec3_t&, vec3_t& );
	void air_accelerate( c_base_player*, vec3_t&, vec3_t&, vec3_t&, float );
	// void walk_move( c_base_player*, vec3_t&, vec3_t& );
	bool categorize_position( c_base_player*, vec3_t&, vec3_t& );
	void check_velocity( c_base_player*, vec3_t&, vec3_t& );
	void try_player_move( c_base_player*, vec3_t&, vec3_t& );

	void frame_stage_notify( );
	vec3_t aimware_extrapolate( c_base_player* ent, vec3_t origin, vec3_t& velocity );
	void predict_player( c_base_player* player );
	int	 get_predicted_choke( int );	
	vec3_t extrapolate_player( c_base_player*, int );
	void trace_player_bbox( c_base_player*, const vec3_t& start, const vec3_t& end, CGameTrace* pm );
	void try_touch_ground( c_base_player*, const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs, CGameTrace* pm );
	void try_touch_ground_in_quadrants( c_base_player*, const vec3_t& start, const vec3_t& end, CGameTrace* pm );

};

END_REGION
