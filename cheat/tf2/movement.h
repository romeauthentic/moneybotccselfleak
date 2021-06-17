#pragma once
#include "sdk.h"

namespace features {
	class c_movement {
	public:
		void update( user_cmd_t* cmd ) {
			if ( !cmd ) return;
			m_cmd = cmd;

			bhop( );
		}

	private:
		void bhop( );
		void autostrafe( );
		user_cmd_t* m_cmd;
	};
}