#pragma once
#include <deque>

#include "util.hpp"
#include "sdk.hpp"

NAMESPACE_REGION( context )

struct shot_data_t {
	vec3_t m_local_pos;
	vec3_t m_angle;
	vec3_t m_enemy_pos;
	int m_enemy_index;
	int m_resolver_shots;
	bool m_resolver_state;
	bool m_missed{ false };
	struct {
		vec3_t min;
		vec3_t max;
		float  radius;
	} m_hitbox;
	int	m_hitgroup;
	bool m_handled{ };
	int m_hit{ };
	int m_damage{ };
	bool m_logged{ };
};

//replacement to the old global:: namespace
class c_context {
public:
	//run localplayer check, pointer, is valid etc
	bool run_frame( );
	float pred_time( );

	void update_local_hook( );

	void on_cmove_end( user_cmd_t* cmd );
	bool create_snapshot( user_cmd_t* );
	//calculate accurate tickbase
	bool calculate_tickbase( );

	user_cmd_t* get_last_cmd( ) {
		return m_snapshot.empty( ) ? nullptr : &m_snapshot.front( );
	}

	bool precache_model( const char* model );

public:
	c_base_player*	m_local{ };
	int				m_stage{ };
	//accurate tickbase
	int				m_tickbase{ };

	bool			m_drawing_postscreenspace{ };
	bool			m_has_incremented_shots{ };
	bool			m_drawing_screneend{ };

	bool			m_has_fired_this_frame{ };

	vec3_t			m_last_realangle{ };
	vec3_t			m_last_fakeangle{ };
	vec3_t			m_thirdperson_angle{ };
	vec3_t			m_last_origin{ };

	float			m_fov{ 90.f };
public:
	void			reset_shot_queue( );

	int				m_last_shot_ack;
	int				m_last_shot;

	std::array< shot_data_t, 128 > m_shot_data;
private:
	std::deque< user_cmd_t > m_snapshot;
};
	
END_REGION

extern context::c_context g_ctx;