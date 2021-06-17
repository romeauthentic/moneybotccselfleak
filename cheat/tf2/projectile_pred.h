#pragma once
class c_base_weapon;
class c_base_player;
class vec3_t;
namespace util 
{
	class c_projectile_pred {
	public:
		vec3_t predict_player( c_base_player* );
	private:
		float get_projectile_speed( c_base_weapon* );
	};
}

extern util::c_projectile_pred projectile_pred;