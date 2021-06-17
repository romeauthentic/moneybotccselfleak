#pragma once
#include <deque>
#include <memory>
#include <map>

#include "sdk.hpp"
#include "console.hpp"

namespace features
{
	class c_aimbot
	{
	public:
		c_aimbot( ) = default;

	private:
		struct aim_target_t
		{
			int m_ent_index;
			vec3_t m_position;
		};

		vec3_t m_shot_dir{ }, m_last_viewangles{ };
		user_cmd_t* m_cmd{ };
		std::map< const char*, vec3_t > m_spread{ }; // sorry not sorry

		void run( );

		aim_target_t find_best_target( ) const;
		vec3_t get_entity_position( c_base_player* ent ) const;
		void aim_at_target( const aim_target_t& target );
		void fix_accuracy( vec3_t& angle );
	public:
		void operator()( user_cmd_t* cmd );
		void silent( );
		void log_shot( c_base_player* local, fire_bullets_info* info );
	};
}
