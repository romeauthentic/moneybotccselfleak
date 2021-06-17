#pragma once
class user_cmd_t;
class c_base_player;
namespace features
{
	class c_prediction {
	public:
		void run_prediction( user_cmd_t* cmd );
	};
}