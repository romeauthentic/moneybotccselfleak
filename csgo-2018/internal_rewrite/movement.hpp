#pragma once
#include "util.hpp"

//forward declarations
class user_cmd_t;

NAMESPACE_REGION( features )

class c_movement {
	user_cmd_t* m_ucmd{ };
	bool		m_direction{ };

	void bhop( );
	void auto_strafer( );

	void edge_jump( );
	void auto_jumpbug( );
	void jump_stats( );

	bool  get_best_direction( float ideal_step, float left, float right, float weight );
	float get_best_strafe_step( float speed, vec3_t direction );
	float get_best_strafe_angle( );
	void circle_strafe( );
	void air_duck( );

public:
	void operator()( user_cmd_t* ucmd ) {
		m_ucmd = ucmd;
		auto_strafer( );
		circle_strafe( );
		bhop( );
		edge_jump( );
		auto_jumpbug( );
		jump_stats( );
		air_duck( );
	}
};

END_REGION