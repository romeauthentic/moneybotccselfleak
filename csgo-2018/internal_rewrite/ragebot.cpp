#include "ragebot.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "math.hpp"
#include "base_cheat.hpp"
#include "input_system.hpp"

namespace features
{
	c_ragebot::c_ragebot( ) :
		m_antiaim( std::make_shared< c_antiaim >( ) ),
		m_lagcomp( std::make_shared< c_lagcomp >( ) ),
		m_resolver( std::make_shared< c_resolver >( ) ) { }

	void c_ragebot::operator()( user_cmd_t* cmd ) {
		if( !cmd ) return;
		m_cmd = cmd;

		m_antiaim->run( cmd );
		run( );
		m_antiaim->fix_movement( );
	}

	vec3_t head_scale( c_base_player* ent, vec3_t pos ) {

		// broke, crashes the hack
		//not what it is
		//return pos;

		vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );
		CTraceFilterOneEntity filter;
		Ray_t ray;
		CGameTrace trace;

		filter.ent = ent;

		float height{ };
		float highest_dmg{ };
		float max_height{ };

		const float max = 6.5f * g_settings.rage.active->m_hitbox_scale;

		for( float i = 0.f; i < max; i += 0.25f ) {
			vec3_t end = pos;
			end.z += i;

			ray.Init( eye_pos, end );
			g_csgo.m_trace( )->TraceRay( ray, 0x46004003, &filter, &trace );

			if( trace.m_pEnt == ent->ce( ) ) {
				max_height = i;
				if( trace.hitgroup == HITGROUP_HEAD && !height ) {
					height = i;
				}
			}
		}

		if( height < 0.5f ) {
			pos.z += height;
			return pos;
		}

		pos.z += ( height + max_height ) * 0.5f;
		return pos;
	}

	int c_ragebot::get_min_dmg( c_base_player* ent ) {
		int hp = ent->m_iHealth( );
		int dmg = g_settings.rage.active->m_damage( );
		int scale = g_settings.rage.active->m_damage_scale( );
		return std::min< int >( ( int )( hp * scale * 0.01f ) + dmg, 100 );
	}

	vec3_t c_ragebot::multipoint( c_base_player* ent, int hitbox, float* out_dmg ) {
		auto should_multipoint = [](int hitbox, bool moving) -> bool {
			switch (hitbox) {
			case HITBOX_HEAD:
			case HITBOX_PELVIS:
				return g_settings.rage.multipoint >= 1;
			case HITBOX_BODY:
			case HITBOX_UPPER_CHEST:
				if (util::is_low_fps() && g_settings.rage.preserve_fps)
					return false;
				return g_settings.rage.multipoint >= 2;
			case HITBOX_THORAX:
			case HITBOX_LEFT_FOOT:
			case HITBOX_RIGHT_FOOT:
				if (util::is_low_fps() && g_settings.rage.preserve_fps)
					return false;
				return g_settings.rage.multipoint >= 3 && !moving;
			default:
				return false;
			}
		};

		bool moving = ent->m_vecVelocity().length2d() > 0.1f && !ent->is_fakewalking();

		vec3_t pos = ent->get_hitbox_pos(hitbox);
		if (hitbox == HITBOX_HEAD)
			pos = head_scale(ent, pos);

		float dmg = g_cheat.m_autowall.run(g_ctx.m_local, ent, pos);
		if (dmg > g_settings.rage.active->m_damage) {
			*out_dmg = dmg;
			return pos;
		}

		if (should_multipoint(hitbox, moving)) {
			const auto model = ent->ce()->GetModel();
			if (!model) return vec3_t{ };

			auto hdr = g_csgo.m_model_info()->GetStudiomodel(model);
			if (!hdr) return vec3_t{ };

			matrix3x4 bone_matrix[128];
			if (!ent->ce()->SetupBones(bone_matrix, 128, 0x100, g_csgo.m_globals->m_curtime)) {
				return vec3_t{ };
			}

			auto set = hdr->pHitboxSet(ent->m_nHitboxSet());
			if (!set) return vec3_t{ };

			//literally 20000 iq, the best multipoint
			//im an actual fucking retard jesus christ
			auto box = set->pHitbox(hitbox);
			if (!box) return vec3_t{ };

			vec3_t center = (box->bbmax + box->bbmin) * 0.5f;

			float dist = box->m_flRadius;

			if (box->m_flRadius == -1.f)
				dist = center.dist_to(box->bbmin) * 0.85f;
			vec3_t min_dir = math::angle_vectors(math::vector_angles(center, box->bbmin));
			vec3_t min = center + min_dir * dist * g_settings.rage.active->m_hitbox_scale * 1.1f;

			if (box->m_flRadius == -1.f)
				dist = center.dist_to(box->bbmax) * 0.85f;
			vec3_t max_dir = math::angle_vectors(math::vector_angles(center, box->bbmax));
			vec3_t max = center + max_dir * dist * g_settings.rage.active->m_hitbox_scale * 1.1f;

			//float radius = box->m_flRadius * 1.1f * g_settings.rage.active->m_hitbox_scale;
			//min -= vec3_t( radius, radius, radius );
			//max += vec3_t( radius, radius, radius );

			std::array< vec3_t, 8 > points = {
				vec3_t{ min.x, min.y, min.z },
				vec3_t{ min.x, max.y, min.z },
				vec3_t{ max.x, max.y, min.z },
				vec3_t{ max.x, min.y, min.z },
				vec3_t{ max.x, max.y, max.z },
				vec3_t{ min.x, max.y, max.z },
				vec3_t{ min.x, min.y, max.z },
				vec3_t{ max.x, min.y, max.z }
			};

			vec3_t  lt{ },
				rt{ },
				lb{ },
				rb{ };

			vec3_t lt_ang{ FLT_MAX, FLT_MAX, 0.f },
				rt_ang{ FLT_MAX, -FLT_MAX, 0.f },
				lb_ang{ -FLT_MAX, FLT_MAX, 0.f },
				rb_ang{ -FLT_MAX, -FLT_MAX, 0.f };

			vec3_t transformed_center = math::vector_transform(vec3_t(min + max) * 0.5f, bone_matrix[box->bone]);
			vec3_t center_ang = math::vector_angles(g_ctx.m_local->get_eye_pos(), transformed_center);

			float min_dmg = std::min< float >(g_settings.rage.active->m_damage, ent->m_iHealth());

			for (auto& it : points) {
				vec3_t trans_point = math::vector_transform(it, bone_matrix[box->bone]);
				CTraceFilterOneEntity filter;
				CGameTrace trace;
				Ray_t ray;

				filter.ent = ent;
				ray.Init(g_ctx.m_local->get_eye_pos(), trans_point);
				g_csgo.m_trace()->TraceRay(ray, MASK_SHOT, &filter, &trace);
				if (trace.m_pEnt == ent->ce()) {
					float dmg = g_cheat.m_autowall.run(g_ctx.m_local, ent, trans_point);
					if (dmg > min_dmg) {
						*out_dmg = dmg;
						return trans_point;
					}
				}

				/*vec3_t ang = math::vector_angles( g_ctx.m_local->get_eye_pos( ), trans_point );

				vec3_t delta = center_ang - ang;
				delta.clamp( );


				if( delta.x < lt_ang.x && delta.y < lt_ang.y ) {
					lt_ang = delta;
					lt = trans_point;
				}

				if( delta.x < rt_ang.x && delta.y > rt_ang.y ) {
					rt_ang = delta;
					rt = trans_point;
				}

				if( delta.x > rb_ang.x && delta.y > rb_ang.y ) {
					rb_ang = delta;
					rb = trans_point;
				}

				if( delta.x > lb_ang.x && delta.y < lb_ang.y ) {
					lb_ang = delta;
					lb = trans_point;
				}*/
			}

			vec3_t point_list[4] = {
				lt,
				rt,
				lb,
				rb
			};

			/*
			for( size_t i{ }; i < 4; ++i ) {
				if( point_list[ i ] ) {
					CTraceFilterOneEntity filter;
					CGameTrace trace;
					Ray_t ray;

					filter.ent = ent;
					ray.Init( g_ctx.m_local->get_eye_pos( ), point_list[ i ] );
					g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &trace );
					if( trace.hitbox == hitbox ) {
						float dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, point_list[ i ] );
						if( dmg > g_settings.rage.active->m_damage ) {
							*out_dmg = dmg;
							return point_list[ i ];
						}
					}
				}
			}*/
		}

		return ent->get_hitbox_pos(hitbox);
	}

	int c_ragebot::hitscan( c_base_player* ent, bool baim, vec3_t& out_pos, float& out_dmg ) {
		auto should_histcan = [](int hitbox, bool moving) -> bool {
			switch (hitbox) {
			case HITBOX_HEAD:
			case HITBOX_NECK:
				return g_settings.rage.hitscan.head;
			case HITBOX_PELVIS:
			case HITBOX_BODY:
			case HITBOX_THORAX:
				return g_settings.rage.hitscan.stomach;
			case HITBOX_CHEST:
			case HITBOX_UPPER_CHEST:
				return g_settings.rage.hitscan.chest;
			case HITBOX_RIGHT_HAND:
			case HITBOX_LEFT_HAND:
				if (moving && g_settings.rage.ignore_limbs_moving)
					return false;

				if (g_settings.rage.preserve_fps && util::is_low_fps())
					return false;

				return g_settings.rage.hitscan.arms;
			case HITBOX_RIGHT_FOOT:
			case HITBOX_LEFT_FOOT:
				if (moving && g_settings.rage.ignore_limbs_moving)
					return false;

				if (g_settings.rage.preserve_fps && util::is_low_fps())
					return false;

				return g_settings.rage.hitscan.legs;
			default:
				return false;
			}
		};

		constexpr std::array< int, 11 > hitbox_order = {
			HITBOX_HEAD,
			HITBOX_NECK,

			HITBOX_PELVIS,
			HITBOX_BODY,
			HITBOX_THORAX,
			HITBOX_CHEST,
			HITBOX_UPPER_CHEST,

			HITBOX_RIGHT_HAND,
			HITBOX_LEFT_HAND,

			HITBOX_RIGHT_FOOT,
			HITBOX_LEFT_FOOT,
		};

		bool moving = ent->m_vecVelocity().length() > 0.1f && !ent->is_fakewalking();
		float min_dmg = std::min< float >(ent->m_iHealth() + 5.f,
			g_settings.rage.active->m_damage);

		if (g_settings.rage.hitbox != -1) {
			float dmg{ };
			vec3_t pos = multipoint(ent, g_settings.rage.hitbox, &dmg);

			if (dmg > min_dmg && pos) {
				out_dmg = dmg;
				out_pos = pos;
				return g_settings.rage.hitbox;
			}

			return -1;
		}

		for (size_t i = (baim ? 2 : 0); i < hitbox_order.size(); ++i) {
			int hitbox = hitbox_order[i];
			if (!should_histcan(hitbox, moving)) {
				continue;
			}

			float dmg{ };
			vec3_t pos = multipoint(ent, hitbox, &dmg);

			if (dmg > min_dmg && pos) {
				out_dmg = dmg;
				out_pos = pos;
				return hitbox;
			}
		}

		if (baim && g_settings.rage.hitscan.head) {
			float damage{ };
			vec3_t pos = multipoint(ent, 0, &damage);

			if (damage > min_dmg && pos) {
				out_pos = pos;
				out_dmg = damage;
				return 0;
			}

			pos = multipoint(ent, 1, &damage);
			if (damage > min_dmg && pos) {
				out_pos = pos;
				out_dmg = damage;
				return 1;
			}
		}

		return -1;
	}

	void c_ragebot::update_settings( ) {
		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon ) return;
		int  weapon_id = weapon->m_iItemDefinitionIndex( );

		switch ( weapon_id ) {
		case WEAPON_DEAGLE:
		case WEAPON_R8REVOLVER:
			g_settings.rage.active = &g_settings.rage.heavy_pistols;
			break;
		case WEAPON_AWP:
		case WEAPON_SSG08:
			g_settings.rage.active = &g_settings.rage.snipers;
			break;
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
			g_settings.rage.active = &g_settings.rage.auto_snipers;
			break;
		default:
			g_settings.rage.active = &g_settings.rage.general;
			break;
		}
	}

	bool c_ragebot::should_baim_entity( c_base_player* ent, int weapon_damage, bool backtracked ) {
		int index = ent->ce()->GetIndex();

		auto weap = g_ctx.m_local->get_weapon();
		if (weap->m_iItemDefinitionIndex() == WEAPON_TASER)
			return true;

		if (!g_cheat.m_player_mgr.is_cheater(index))
			return false;

		int min_hp = g_settings.rage.active->m_baim_health;
		int min_shots = g_settings.rage.active->m_baim_shots;

		if (m_resolver->get_shots(index) >= min_shots)
			return true;

		if (ent->m_iHealth() <= min_hp)
			return true;

		if (!(ent->m_fFlags() & FL_ONGROUND) && g_settings.rage.active->m_baim_air)
			return true;

		if (!backtracked && g_settings.rage.active->m_baim_fake)
			return true;

		if (weapon_damage > ent->m_iHealth() && g_settings.rage.active->m_baim_lethal)
			return true;

		return false;
	}

	void quick_stop( user_cmd_t* cmd ) {
		if (g_cheat.m_ragebot.m_antiaim->is_fakewalking())
			return;

		if (!(g_ctx.m_local->m_fFlags() & FL_ONGROUND))
			return;

		vec3_t vel = g_ctx.m_local->m_vecVelocity();
		float speed = vel.length2d();

		if (g_settings.rage.active->m_auto_stop() == 1 || g_settings.rage.active->m_auto_stop() == 4) {
			auto wep = g_ctx.m_local->get_weapon();
			auto wep_info = wep->get_wpn_info();

			bool scoped = g_ctx.m_local->m_bIsScoped();
			float max_vel = scoped ? wep_info->max_speed_alt : wep_info->max_speed;

			if (speed < max_vel / 3)
				return;
		}

		if (speed < 0.1f) {
			g_ctx.get_last_cmd()->m_forwardmove = 0.f;
			g_ctx.get_last_cmd()->m_sidemove = 0.f;
			return;
		}

		static auto sv_accelerate = g_csgo.m_cvar()->FindVar(xors("sv_accelerate"));
		float accel = sv_accelerate->get_float();
		float max_speed = g_ctx.m_local->get_weapon()->get_wpn_info()->max_speed;
		if (g_ctx.m_local->get_weapon()->is_sniper() && g_ctx.m_local->m_bIsScoped()) {
			max_speed = g_ctx.m_local->get_weapon()->get_wpn_info()->max_speed_alt;
		}

		// thx ida
		max_speed = std::min< float >(max_speed, 250.f);

		if (g_ctx.m_local->m_fFlags() & FL_DUCKING) {
			//	max_speed /= 3.f;
			//	accel /= 3.f;
		}

		float surf_friction = 1.f;
		float max_accelspeed = accel * g_csgo.m_globals->m_interval_per_tick * max_speed * surf_friction;

		float wishspeed{ };

		if (speed - max_accelspeed <= -1.f) {
			wishspeed = max_accelspeed / (speed / (accel * g_csgo.m_globals->m_interval_per_tick));
		}
		else {
			wishspeed = max_accelspeed;
		}

		vec3_t ndir = math::vector_angles(vel * -1.f);
		ndir.y = g_csgo.m_engine()->GetViewAngles().y - ndir.y;
		ndir = math::angle_vectors(ndir);

		g_ctx.get_last_cmd()->m_forwardmove = ndir.x * wishspeed;
		g_ctx.get_last_cmd()->m_sidemove = ndir.y * wishspeed;
	}

	std::array< int, 65 > c_ragebot::get_sorted_targets( ) {
		typedef struct {
			int index;
			float dist;
			float fov;
			bool moving;
			int hp;
			float simtime;
		} target;

		std::array< target, 65 > targets{ };
		std::array< int, 65 > ret{ };

		vec3_t local_pos = g_ctx.m_local->get_eye_pos();
		vec3_t cur_ang, dir_vec;
		g_csgo.m_engine()->GetViewAngles(cur_ang);

		dir_vec = math::angle_vectors(cur_ang);

		for (size_t i{ }; i < 65; ++i) {
			auto& cur_target = targets.at(i);
			auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);
			cur_target.index = i;

			if (!ent || ent == g_ctx.m_local || !ent->is_valid())
				continue;

			if (ent->m_iTeamNum() == g_ctx.m_local->m_iTeamNum() &&
				!g_settings.rage.friendlies())
				continue;

			auto pos = ent->m_vecOrigin();
			pos.z += 30.f; //eh

			auto dir = (pos - local_pos);
			dir.normalize_vector();

			float fov = dir_vec.fov_to(dir) * M_RADPI; // epic fov fix by friendly....

			cur_target.fov = fov;
			cur_target.dist = pos.dist_to(local_pos);
			cur_target.hp = ent->m_iHealth();
			cur_target.moving = ent->m_vecVelocity().length2d() > 0.1f
				&& !ent->is_fakewalking()
				&& (ent->m_fFlags() & FL_ONGROUND);
			cur_target.simtime = ent->m_flSimulationTime();
		}

		auto sort_fn = [](const target& a, const target& b) {
			if (g_settings.rage.prefer_low_hp) {
				if (a.hp != b.hp)
					return a.hp < b.hp;
			}

			if (g_settings.rage.prefer_moving) {
				if (a.moving != b.moving)
					return a.moving > b.moving;
			}

			if (g_settings.rage.selection_type == 0) {
				if (a.fov != b.fov)
					return a.fov < b.fov;
			}
			else if (g_settings.rage.selection_type == 1) {
				if (a.dist != b.dist)
					return a.dist < b.dist;
			}
			else if (g_settings.rage.selection_type == 2) {
				if (a.simtime != b.simtime)
					return a.simtime > b.simtime;
			}

			return a.index > b.index;
		};

		std::sort(targets.begin(), targets.end(), sort_fn);
		for (size_t i{ }; i < 65; ++i) {
			if (targets[i].fov < g_settings.rage.fov) {
				ret[i] = targets[i].index;
			}
		}

		return ret;
	}

	bool c_ragebot::can_hit_without_ping( int entity ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity( entity );

		if( !ent )
			return false;

		int min_dmg = get_min_dmg( ent );
		auto check_records = [ &ent, &min_dmg ]( std::deque< lag_record_t >* records ) {
			auto& front = records->front( );
			vec3_t pred_pos = front.m_vecPelvisPos;

			float dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, pred_pos, false );
			if( dmg > min_dmg )
				return true;

			pred_pos = front.m_vecHeadPos;
			dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, pred_pos, false );
			if( dmg > min_dmg )
				return true;

			return false;
		};

		auto lby_records = m_lagcomp->get_records( entity, RECORD_LBY );
		auto sim_records = m_lagcomp->get_records( entity, RECORD_NORMAL );

		if( lby_records->size( ) ) {
			if( check_records( lby_records ) )
				return true;
		}

		if( sim_records->size( ) ) {
			if( check_records( sim_records ) )
				return true;
		}

		return false;
	}

	int c_ragebot::knifebot( ) {
		auto aim_pos = g_ctx.m_local->get_eye_pos( );

		float dist = FLT_MAX;
		int ideal_ent = 0;
		int ideal_tick = 0;
		int hp = 0;
		vec3_t pos;

		for( size_t i{ }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );

			if( !ent || !ent->is_valid( ) || ent == g_ctx.m_local || !ent->has_valid_anim( ) || ent->m_bGunGameImmunity( ) )
				continue;

			auto newest_record = m_lagcomp->get_newest_valid_record( i );
			if( !newest_record )
				continue;

			if( newest_record->m_vecPelvisPos.dist_to( aim_pos ) < dist ) {
				hp = ent->m_iHealth( );
				ideal_ent = i;
				ideal_tick = newest_record->m_tickcount;
				pos = newest_record->m_vecPelvisPos;

				dist = pos.dist_to( aim_pos );
			}
		}

		if( ideal_ent && dist < 80.f ) {
			int attack = 0;

			if( dist < 70.f )
				attack = IN_ATTACK2;
			else if( hp < 35 )
				attack = IN_ATTACK;
			else
				return -1;

			vec3_t ang = math::vector_angles( aim_pos, pos );
			float fov = ang.dist_to( g_csgo.m_engine( )->GetViewAngles( ) );

			if( fov < g_settings.rage.fov ) {
				m_cmd->m_viewangles = math::vector_angles( aim_pos, pos );
				m_cmd->m_tick_count = ideal_tick;
				m_cmd->m_buttons |= attack;

				return ideal_ent;
			}
			else {
				return -1;
			}
		}

		return -1;
	}

	c_ragebot::aim_target_t c_ragebot::find_best_target( ) {
		aim_target_t ret{ -1, m_cmd->m_tick_count, -1, vec3_t{ }, vec3_t{ }, vec3_t{ }, 0.f, 0 };
		vec3_t	aim_angle{ };
		vec3_t	local_pos{ };
		vec3_t	aim_pos{ };
		auto	weapon_info = g_ctx.m_local->get_weapon()->get_wpn_info();

		int damage = 0;
		bool is_zeus = false;

		if (weapon_info) {
			is_zeus = g_ctx.m_local->get_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER;
			damage = weapon_info->damage;
		}

		local_pos = g_ctx.m_local->get_eye_pos();

		auto players = get_sorted_targets();
		auto data = players.data();

		for (size_t i{ }; i < players.size(); ++i) {
			auto& it = data[i];

			auto entity = g_csgo.m_entlist()->GetClientEntity< >(it);
			if (!entity || !entity->is_valid() || entity == g_ctx.m_local) continue;
			if (entity->m_bGunGameImmunity()) continue;

			bool baim = false;
			bool should_baim = true;
			bool backtracked = false;
			bool restore = false;
			float hit_damage = 0.f;
			vec3_t extrapolate{ };

			lag_record_t   original_record(entity);
			lag_record_t* backtracked_record{ };

			int tick_count = m_cmd->m_tick_count;
			float lerp = util::get_lerptime();
			auto record_type = m_lagcomp->can_backtrack_entity(it);
			if (!g_settings.rage.resolver()) {
				if (!util::is_tick_valid(entity->m_flSimulationTime() + lerp)) {
					int new_tickcount = m_lagcomp->backtrack_entity(it,
						record_type, &backtracked_record);

					if (new_tickcount != -1) {
						restore = true;
						tick_count = new_tickcount;
					}
				}
			}
			else {
				int new_tickcount = m_lagcomp->backtrack_entity(it, record_type, &backtracked_record);
				if (new_tickcount != -1) {
					tick_count = new_tickcount;
					should_baim = record_type != RECORD_LBY;
					backtracked = record_type == RECORD_LBY;
					restore = true;
				}
				else {

					lag_record_t* last_simtime_record = m_lagcomp->get_newest_valid_record(it);
					if (last_simtime_record) {
						//ENSURE THE MOST ACCURATE DATA
						entity->m_vecVelocity() = entity->get_animdata().m_last_velocity;

						auto velocity = entity->m_vecVelocity();
						auto min_vel = 64 / TICK_INTERVAL() / 14;

						//IS HE A SLOW BOI OR A FAST NIGGER
						if (g_settings.rage.fakelag_resolver() && velocity.length2d() > min_vel - 10.f) {
							auto new_record = *last_simtime_record;

							//FUCK FAKE PINGERS WE IN THIS BITCH
							auto nci = g_csgo.m_engine()->GetNetChannelInfo(); //GOD FUCKING KNOWS WHY
							float latency = nci->GetLatency(0) + g_csgo.m_globals->m_frametime;

							auto pred_choke = g_cheat.m_prediction.get_predicted_choke(it);
							auto latency_ticks = TIME_TO_TICKS(latency);

							float time_since_update = g_ctx.pred_time() + latency - entity->m_flSimulationTime();

							//NIGGA UPDATED ALREADY FUCK ADAPTIVE
							if (TIME_TO_TICKS(time_since_update) > pred_choke && pred_choke > 0) {
								if (g_settings.rage.fakelag_resolver() == 1)
									continue;

								else if (latency_ticks > 0) {
									int update_count = TIME_TO_TICKS(time_since_update) / pred_choke;

									vec3_t extrapolated_origin = g_cheat.m_prediction.full_walk_move(entity, update_count * pred_choke);
									new_record.m_vecRenderOrigin = extrapolated_origin;
									new_record.m_tickcount += update_count * pred_choke;
								}
							}

							//if( !new_record.is_valid( ) )
							//	continue;

							int shots = m_resolver->get_shots(it);
							bool recalc = shots != last_simtime_record->m_shots;

							new_record.restore(true, recalc);
							tick_count = new_record.m_tickcount;
							backtracked_record = last_simtime_record;
							restore = true;
						}
						else {
							int shots = m_resolver->get_shots(it);
							bool recalc = shots != last_simtime_record->m_shots;

							last_simtime_record->restore(recalc, recalc);
							tick_count = last_simtime_record->m_tickcount;
							backtracked_record = last_simtime_record;
							restore = true;
						}
					}
					else {
						//fake ping, just wait for an update
						continue;
					}
				}
			}

			baim = should_baim_entity(entity, damage, !should_baim);

			auto restore_player = [&original_record, &restore, &entity, this]() {
				original_record.restore(false);
			};

			int hitbox = hitscan(entity, baim, aim_pos, hit_damage);
			if (hitbox == -1) {
				restore_player();
				continue;
			}

			if (aim_pos.dist_to(local_pos) > 150.f && is_zeus) {
				restore_player();
				continue;
			}

			aim_angle = math::vector_angles(local_pos, aim_pos).clamp();


			if (g_settings.rage.active->m_auto_stop() == 2) {
				g_con->log("2");
				quick_stop(m_cmd);
			}
			else if (g_settings.rage.active->m_auto_stop() == 3) {
				m_antiaim->run_fakewalk();
			}

			if (g_settings.rage.active->m_auto_stop() == 5) {
				m_cmd->m_buttons |= IN_DUCK;
			}

			if (!util::hitchance(it, aim_angle, g_settings.rage.active->m_hitchance)) {
				if (g_settings.rage.active->m_auto_stop() == 1 || g_settings.rage.active->m_auto_stop() == 4)
					quick_stop(m_cmd);

				if (g_settings.rage.active->m_auto_stop() == 5) {
					if (g_ctx.m_local->m_flDuckAmount() < 1.f)
						quick_stop(m_cmd);
				}

				if (g_settings.rage.active->m_auto_stop() == 4 && g_cheat.m_lagmgr.get_choked() < 10 && !m_antiaim->is_fakewalking())
					g_cheat.m_lagmgr.set_state(false);

				if (g_settings.rage.auto_scope() && g_ctx.m_local->get_weapon()->is_sniper()) {
					if (!g_ctx.m_local->m_bIsScoped()) {
						m_cmd->m_buttons |= IN_ATTACK2;
					}
				}

				restore_player();
				continue;
			}

			ret.m_ent_index = it;
			ret.m_position = aim_pos;
			ret.m_tick_count = tick_count;
			ret.m_hitbox = hitbox;
			ret.m_backtracked = backtracked;
			ret.m_min = entity->get_hitbox_mins(hitbox);
			ret.m_max = entity->get_hitbox_maxs(hitbox);
			ret.m_radius = entity->get_hitbox_radius(hitbox);

			memcpy(m_shot_matrix, (void*)entity->m_dwBoneMatrix(), sizeof(matrix3x4) * 128);
			m_shot_target = it;

			restore_player();
			if (backtracked_record && backtracked_record->m_valid && backtracked_record->m_balanceadjust) {
				backtracked_record->m_shot = true;
			}

			break;
		}

		return ret;
	}

	void c_ragebot::set_angles( const vec3_t& angles ) {
		auto weapon = g_ctx.m_local->get_weapon( );
		if( g_settings.rage.compensate_spread ) {
			int seed = m_cmd->m_random_seed;

			weapon->update_accuracy_penalty( );

			float inaccuracy = weapon->get_inaccuracy( );
			float spread = weapon->get_spread( );

			util::set_random_seed( ( seed & 0xff ) + 1 );

			float rand_a = util::get_random_float( 0.0f, 1.0f );
			float pi_rand_a = util::get_random_float( 0.f, 2.f * M_PI );
			float rand_b = util::get_random_float( 0.f, 1.0f );
			float pi_rand_b = util::get_random_float( 0.f, 2.f * M_PI );

			float spread_x = cos( pi_rand_a ) * ( rand_a * inaccuracy ) + cos( pi_rand_b ) * ( rand_b * spread );
			float spread_y = sin( pi_rand_a ) * ( rand_a * inaccuracy ) + sin( pi_rand_b ) * ( rand_b * spread );

			vec3_t forward, right, up;
			math::angle_vectors( angles, &forward, &right, &up );

			vec3_t spread_dir = forward + ( right * -spread_x ) + ( up * -spread_y );
			spread_dir.normalize_vector( );

			vec3_t comp = math::vector_angles( vec3_t( ), spread_dir );
			
			m_cmd->m_viewangles = comp;
			m_cmd->m_random_seed = seed;
		}

		if( g_settings.rage.silent == 2 ) {
			g_cheat.m_lagmgr.set_state( weapon->m_iItemDefinitionIndex( ) == WEAPON_R8REVOLVER );
			m_send_next = weapon->m_iItemDefinitionIndex( ) != WEAPON_R8REVOLVER;
		}
		
		if( g_settings.rage.silent == 1 && !m_antiaim->is_fakewalking( ) ) {
			if( weapon->m_iItemDefinitionIndex( ) != WEAPON_R8REVOLVER )
				g_cheat.m_lagmgr.set_state( !g_cheat.m_lagmgr.get_sent( ) );
		}

		m_cmd->m_viewangles = angles;
		if( !g_settings.rage.silent( ) ) {
			g_csgo.m_engine( )->SetViewAngles( m_cmd->m_viewangles );
		}
	}

	void c_ragebot::aim_at_target( const aim_target_t& target ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( target.m_ent_index );
		if( !ent )	
			return;

		if( g_ctx.m_local->get_weapon( )->m_iItemDefinitionIndex( ) != WEAPON_R8REVOLVER ) {
			if( g_settings.rage.active->m_delay_type == 2 ) {
				static auto time_to_fire = 0.f;
				const float delay = float( g_settings.rage.active->m_delay( ) ) * 0.001f;
				const float time = g_ctx.pred_time( );

				float delta = time_to_fire - time;

				if( delta > delay ) {
					time_to_fire = time + delay;
				}

				if( delta < 0.f ) {
					time_to_fire = time + delay;
					delta = delay;
				}

				if( delta > g_csgo.m_globals->m_interval_per_tick ) {
					return;
				}
			}
			if( g_settings.rage.active->m_delay_type == 3 ) {
				static float time_to_fire = 0.f;
				const float time = g_ctx.pred_time( );

				float delta = time_to_fire - time;

				vec3_t velocity = ent->m_vecVelocity( );
				float speed = velocity.length2d( );
				if( speed > g_ctx.m_local->m_vecVelocity( ).length2d( ) && speed > 100.f ) {
					vec3_t head_pos = ent->get_hitbox_pos( 0 );
					vec3_t origin = ent->m_vecOrigin( );
					origin.z = head_pos.z = 0.f;

					float dist = origin.dist_to( head_pos ); //1 / 64
					auto delay = dist / speed + 0.015625f;

					if( delta > delay )
						time_to_fire = time + delay;

					if( delta < 0.f ) {
						time_to_fire = time + delay;
						delta = delay;
					}

					if( delta > g_csgo.m_globals->m_interval_per_tick )
						return;
				}
			}
		}

		if (g_settings.rage.active->m_auto_stop && !m_antiaim->is_fakewalking() && !g_settings.rage.compensate_spread)
			m_cmd->m_forwardmove = m_cmd->m_sidemove = 0.f;

		m_cmd->m_buttons |= IN_ATTACK;
		m_cmd->m_buttons &= ~IN_USE;

		vec3_t position = target.m_position;
		m_cmd->m_tick_count = target.m_tick_count;
	
		auto angle = math::vector_angles(
			g_ctx.m_local->get_eye_pos( ),
			target.m_position );

		static auto weapon_recoil_scale = g_csgo.m_cvar( )->FindVar( xors( "weapon_recoil_scale" ) );
		angle -= g_ctx.m_local->m_aimPunchAngle( ) * weapon_recoil_scale->get_float( );

		g_cheat.m_visuals.store_shot( target.m_position );

		if( !target.m_backtracked && g_cheat.m_player_mgr.is_cheater( target.m_ent_index ) ) {
			m_resolver->aimbot( 
				target.m_ent_index, 
				target.m_hitbox, angle, 
				target.m_position,
				target.m_min,
				target.m_max,
				target.m_radius );
		}
		else {
			context::shot_data_t new_shot{ };
			new_shot.m_angle = angle;
			new_shot.m_enemy_index = target.m_ent_index;
			
			new_shot.m_hitbox.max = target.m_min;
			new_shot.m_hitbox.min = target.m_max;
			new_shot.m_hitbox.radius = target.m_radius;
			new_shot.m_hitgroup = target.m_hitbox;

			new_shot.m_enemy_pos = target.m_position;

			g_ctx.m_last_shot++;
			g_ctx.m_last_shot %= 128;

			g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;
			g_ctx.m_has_incremented_shots = true;
		}

		set_angles( angle.clamp( ) );

		if (g_settings.rage.active->m_auto_stop && !g_settings.rage.compensate_spread())
			quick_stop(m_cmd);
	}

	void c_ragebot::run( ) {
		if( !g_settings.rage.enabled( ) || !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) ) {
			m_target = -1;
			return;
		}

		update_settings( );

		if( m_send_next ) {
			if( !g_cheat.m_lagmgr.get_sent( ) && !m_antiaim->is_fakewalking( ) )
				g_cheat.m_lagmgr.set_state( true );

			m_send_next = false;
		}

		auto wep = g_ctx.m_local->get_weapon( );
		if (!wep || !wep->m_iClip1() || wep->is_grenade()) {
			m_target = -1;
			return;
		}


		bool in_attack = false;

		if( !g_ctx.m_local->can_attack( ) )
			return;

		m_can_fakeping = true;

		if ( g_settings.rage.activation_type == 0 ) in_attack = true;
		else in_attack = g_input.is_key_pressed(
			( VirtualKeys_t )g_settings.rage.aimkey( ) );

		if( !in_attack ) {
			m_target = -1;
			return;
		}

		if( wep->is_knife( ) ) {
			m_target = knifebot( );
			return;
		}

		auto target = find_best_target( );
		if ( target.m_ent_index != -1 ) {
			aim_at_target( target );
		}
		m_target = target.m_ent_index;
	}
}