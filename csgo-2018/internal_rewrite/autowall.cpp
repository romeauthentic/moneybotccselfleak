#include <algorithm>

#include "autowall.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "math.hpp"

namespace features
{
	bool c_autowall::is_armored(c_base_player* ent, int armor_value, int hitgroup) {
		if (armor_value <= 0) return false;

		switch (hitgroup) {
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		case HITGROUP_HEAD:
			return ent->m_bHasHelmet();
		}

		return false;
	}

	void c_autowall::trace_line(const vec3_t& start, const vec3_t& end, unsigned mask, void* ignore, CGameTrace* tr) {
		CTraceFilter filter;
		Ray_t		 ray;

		filter.pSkip = ignore;
		ray.Init(start, end);

		g_csgo.m_trace()->TraceRay(ray, mask, &filter, tr);
	}

	void c_autowall::scale_damage(c_base_player* ent, int hitgroup, float weapon_armor_ratio, float& damage) {
		float multiplier;
		float armor_ratio;
		float new_damage;
		int armor;

		auto get_hitgroup_mul = [](int hitgroup) {
			switch (hitgroup) {
			case HITGROUP_HEAD:
				return 4.f;
			case HITGROUP_STOMACH:
				return 1.25f;
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				return 0.75f;
			}

			return 1.0f;
		};

		multiplier = get_hitgroup_mul(hitgroup);
		damage *= multiplier;
		armor = ent->m_ArmorValue();

		if (is_armored(ent, armor, hitgroup)) {
			armor_ratio = weapon_armor_ratio * 0.5f;
			new_damage = damage * armor_ratio;
			if (hitgroup != HITGROUP_HEAD && damage - damage * armor_ratio * 0.5f > armor) {
				new_damage = damage - armor * 2.0f;
			}

			damage = new_damage;
		}
	}

	bool c_autowall::is_breakable(IClientEntity* ent) {
		static auto is_breakable_ptr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 51 56 8B F1 85 F6 74 68 83 BE"));

		auto& v2 = *reinterpret_cast<uint8_t*>(uintptr_t(ent) + 0x27c);
		uint8_t backup_value = v2;

		auto clientclass = ent->GetClientClass();
		if (clientclass->m_class_id != CFuncBrush && clientclass->m_class_id != CBaseDoor)
			v2 = 2;

		auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(void*)>(is_breakable_ptr);

		bool is_breakable = is_breakable_fn(ent);
		v2 = backup_value;

		return is_breakable;
	}

	bool c_autowall::trace_to_exit(vec3_t start, vec3_t& dir, vec3_t& out_end, CGameTrace& tr, CGameTrace* exit_trace) {
		float dist = 0.f;
		CTraceFilter filter;
		Ray_t ray, ray_2;

		while (dist <= 90.f) {
			dist += 4.f;

			out_end = start + dir * dist;

			int contents = g_csgo.m_trace()->GetPointContents(out_end, MASK_SHOT_HULL | CONTENTS_HITBOX);

			if (contents & MASK_SHOT_HULL && !(contents & CONTENTS_HITBOX))
				continue;

			ray.Init(out_end, out_end - dir * 4.f);
			g_csgo.m_trace()->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, 0, exit_trace);

			if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
				filter.pSkip = exit_trace->m_pEnt;

				ray_2.Init(out_end, start);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT_HULL, &filter, exit_trace);

				if (exit_trace->DidHit() && !exit_trace->startsolid) {
					out_end = exit_trace->endpos;
					return true;
				}
			}
			else if (!exit_trace->DidHit() || exit_trace->startsolid) {
				if (tr.m_pEnt && tr.m_pEnt->GetIndex()) {
					if (is_breakable(tr.m_pEnt))
						return true;

					start = tr.endpos;
				}
			}
			else if (((exit_trace->surface.flags >> 7) & 1) && !((tr.surface.flags >> 7) & 1))
				continue;
			else if (exit_trace->plane.normal.dot(dir) <= 1.0f) {
				auto fraction = exit_trace->fraction * 4.0f;
				out_end = out_end - dir * fraction;

				return true;
			}
		}

		return false;
	}

	bool c_autowall::handle_bullet_penetration(weapon_info_t* wpn_data, fire_bullet_data_t& data) {
		surfacedata_t* enter_surface_data = g_csgo.m_phys_props()->GetSurfaceData(data.enter_trace.surface.surfaceProps);
		int enter_material = enter_surface_data->game.material;
		float enter_surf_penetration_mod = enter_surface_data->game.penetrationmodifier;

		data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
		data.current_damage *= static_cast<float>(pow(wpn_data->range_modifier, data.trace_length * 0.002));

		if (data.trace_length > 3000.f || enter_surf_penetration_mod < 0.1f)
			data.penetrate_count = 0;

		if (data.penetrate_count <= 0)
			return false;

		vec3_t dummy;
		CGameTrace trace_exit;
		if (!trace_to_exit(data.enter_trace.endpos, data.direction, dummy, data.enter_trace, &trace_exit))
			return false;

		surfacedata_t* exit_surface_data = g_csgo.m_phys_props()->GetSurfaceData(trace_exit.surface.surfaceProps);

		int exit_material = exit_surface_data->game.material;
		float exit_surf_penetration_mod = exit_surface_data->game.penetrationmodifier;

		float final_damage_modifier = 0.16f;
		float combined_penetration_modifier = 0.0f;

		if ((data.enter_trace.contents & CONTENTS_GRATE) != 0 || enter_material == 89 || enter_material == 71) {
			combined_penetration_modifier = 3.0f;
			final_damage_modifier = 0.05f;
		}
		else {
			combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
		}

		if (enter_material == exit_material) {
			if (exit_material == 87 || exit_material == 85)
				combined_penetration_modifier = 3.0f;
			else if (exit_material == 76)
				combined_penetration_modifier = 2.0f;
		}

		float modifier = fmaxf(0.f, 1.0f / combined_penetration_modifier);
		float taken_damage = (data.current_damage * final_damage_modifier) + modifier * 3.0f * fmaxf(0.0f,
			(float)(3.0 / (wpn_data->penetration)) * 1.25f);
		float thickness = (trace_exit.endpos - data.enter_trace.endpos).lengthsqr();

		thickness *= modifier;
		thickness *= 0.041666668f;

		float lost_damage = fmaxf(0.0f, taken_damage + thickness);

		if (lost_damage > data.current_damage)
			return false;

		if (lost_damage >= 0.0f)
			data.current_damage -= lost_damage;

		if (data.current_damage < 1.0f)
			return false;

		data.src = trace_exit.endpos;
		data.penetrate_count--;

		return true;
	}

	bool c_autowall::fire_bullet(c_base_player* shooter, c_base_player* target, weapon_info_t* wep_data, fire_bullet_data_t& data, bool ent_check, bool scale) {
		data.penetrate_count = 4;
		data.trace_length = 0.f;

		if (!wep_data)
			return false;

		data.current_damage = (float)(wep_data->damage);

		while (data.penetrate_count > 0 && data.current_damage > 1.0f) {
			if (!ent_check) {
				data.trace_length_remaining = data.length_to_end - data.trace_length;
			}
			else {
				data.trace_length_remaining = wep_data->range - data.trace_length;
			}

			vec3_t end = data.src + data.direction * data.trace_length_remaining;
			trace_line(data.src, end, MASK_SHOT | CONTENTS_GRATE, shooter, &data.enter_trace);

			util::clip_trace_to_player(target->ce(), data.src, end + data.direction * 40.f, MASK_SHOT | CONTENTS_GRATE, &data.filter, &data.enter_trace);

			if (data.enter_trace.fraction == 1.0f) {
				if (!ent_check) {
					data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
					data.current_damage *= std::pow(wep_data->range_modifier, data.trace_length * 0.002f);

					if (scale)
						scale_damage(target, HITGROUP_HEAD, wep_data->armor_ratio, data.current_damage);
					return true;
				}

				break;
			}


			if (data.enter_trace.hitgroup <= HITGROUP_RIGHTLEG && data.enter_trace.hitgroup >= HITGROUP_HEAD
				&& data.enter_trace.m_pEnt == target->ce()) {
				data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
				data.current_damage *= std::pow(wep_data->range_modifier, data.trace_length * 0.002f);

				auto enemy = (c_base_player*)(data.enter_trace.m_pEnt);
				auto cl = enemy->ce()->GetClientClass();
				if (cl->m_class_id != CCSPlayer) {
					return false;
				}

				if (shooter->m_iTeamNum() == enemy->m_iTeamNum() && !g_settings.rage.friendlies) {
					return false;
				}

				if (scale)
					scale_damage(enemy, data.enter_trace.hitgroup, wep_data->armor_ratio, data.current_damage);

				return true;
			}

			if (!handle_bullet_penetration(wep_data, data))
				break;
		}

		return false;
	}

	float c_autowall::run(c_base_player* shooter, c_base_player* target, const vec3_t& end, bool ent_check) {
		if (!shooter || !target) {
			return 0.f;
		}

		auto wep = shooter->get_weapon();
		if (!wep) {
			return 0.f;
		}

		fire_bullet_data_t data;
		data.src = shooter->m_vecOrigin();
		data.filter.pSkip = shooter;

		if (shooter == g_ctx.m_local) {
			data.src += shooter->m_vecViewOffset();
		}
		else {
			data.src += 72.f;
		}

		data.length_to_end = (end - data.src).length();

		vec3_t angles = math::vector_angles(data.src, end);
		data.direction = math::angle_vectors(angles);

		data.direction.normalize_vector();

		if (fire_bullet(shooter, target, wep->get_wpn_info(), data, ent_check)) {
			return data.current_damage;
		}

		return 0.f;
	}
}
