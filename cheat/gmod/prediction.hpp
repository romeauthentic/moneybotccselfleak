#pragma once
#include "util.hpp"

class user_cmd_t;
class c_base_player;

NAMESPACE_REGION( features )

class c_prediction {
	user_cmd_t* m_ucmd{ };
	int m_predicted_flags{ };

	void run( user_cmd_t* ucmd );
public:
	void operator()( user_cmd_t* ucmd ) {
		m_ucmd = ucmd;
		run( m_ucmd );
	}

	int get_predicted_flags( ) const {
		return m_predicted_flags;
	}

	void predict_player( c_base_player* player );
};

END_REGION