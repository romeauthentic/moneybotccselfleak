#pragma once
#include "sdk.hpp"
#include "pattern.hpp"
#include "interface.hpp"

struct fire_bullet_data_t {
	vec3_t src;
	CGameTrace enter_trace;
	vec3_t direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float length_to_end;
	float current_damage;
	int penetrate_count;
};

namespace features
{
	class c_autowall {
	private:
		bool is_armored( c_base_player* entity, int armor_value, int hitgroup );
		void scale_damage( c_base_player* entity, int hitgroup, float weapon_armor_ratio, float& damage );
		void trace_line( const vec3_t& abs_start, const vec3_t& abs_end, unsigned mask, void* ignore, CGameTrace* trace );
		bool is_breakable( IClientEntity* ent );

		bool trace_to_exit( vec3_t start, vec3_t& dir, vec3_t& out_end, CGameTrace& tr, CGameTrace* exit_trace );

	public:
		bool  handle_bullet_penetration( weapon_info_t* wpn_data, fire_bullet_data_t& data );
		bool  fire_bullet( c_base_player* shooter, c_base_player* target, weapon_info_t* wep_data, fire_bullet_data_t& data, bool ent_check = true, bool scale_damage = true );
		float run( c_base_player* shooter, c_base_player* target, const vec3_t& end, bool ent_check = true );
	};
}