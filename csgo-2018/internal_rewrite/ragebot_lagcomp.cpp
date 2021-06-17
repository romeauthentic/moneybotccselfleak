#include "ragebot.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "mem.hpp"
#include "hooks.hpp"

namespace features
{
	c_ragebot::lag_record_t::lag_record_t( c_base_player* ent ) : m_ent( ent ) {
		if (!m_ent || !m_ent->is_valid()) return;
		m_valid = true;

		m_flSimulationTime = m_ent->m_flSimulationTime();
		m_flOldSimulationTime = m_ent->m_flOldSimulationTime();
		m_lastAnimTime = m_ent->get_animdata().m_last_duck;
		m_tickcount = TIME_TO_TICKS(m_flSimulationTime + util::get_lerptime());
		m_choked = m_ent->get_choked_ticks();

		m_flLowerBodyYawTarget = m_ent->m_flLowerBodyYawTarget();
		m_flCycle = m_ent->m_flCycle();
		m_fFlags = m_ent->m_fFlags();
		m_animDuck = m_ent->get_animdata().m_last_duck;
		m_animVelocity = m_ent->get_animdata().m_anim_velocity;
		m_prevVelocity = m_ent->get_animdata().m_last_velocity;
		m_flDuckSpeed = m_ent->m_flDuckSpeed();
		m_flDuckAmount = m_ent->m_flDuckAmount();
		m_animFlags = m_ent->get_animdata().m_anim_flags;

		m_vecHeadPos = ent->get_hitbox_pos(0);
		m_vecPelvisPos = ent->get_hitbox_pos(HITBOX_PELVIS);

		//we need mins/maxs because traces check them before checking if it hit a player
		m_vecMins = *(vec3_t*)(uintptr_t(m_ent) + 0x318 + 0x8);
		m_vecMaxs = *(vec3_t*)(uintptr_t(m_ent) + 0x318 + 0x14);

		m_vecOrigin = m_ent->m_vecOrigin();
		m_vecAngles = m_ent->m_angEyeAngles();
		m_vecRenderAngles = m_ent->ce()->GetRenderAngles();
		m_vecRenderOrigin = m_ent->ce()->GetRenderOrigin();

		memcpy(&m_state,
			m_ent->get_animstate(),
			sizeof(CCSGOPlayerAnimState));

		memcpy(m_PoseParameters,
			m_ent->m_flPoseParameter(),
			sizeof(float) * 24);

		memcpy(m_AnimLayers,
			m_ent->m_AnimOverlay().GetElements(),
			sizeof(C_AnimationLayer) * 13);

		for (int i{ }; i < 13; ++i)
			m_ent->m_AnimOverlay().GetElements()[i].m_player = ent;

		m_valid = m_ent->ce()->SetupBones(m_matrix, 128, BONE_USED_BY_HITBOX, m_flSimulationTime);
	}

	bool c_ragebot::lag_record_t::restore( bool recalculate, bool reanimate) {
		if (!m_ent || !m_ent->is_valid()) return false;

		*(vec3_t*)(uintptr_t(m_ent) + 0x318 + 0x8) = m_vecMins;
		*(vec3_t*)(uintptr_t(m_ent) + 0x318 + 0x14) = m_vecMaxs;

		memcpy((void*)(m_ent->m_CachedBoneData().GetElements()),
			m_matrix,
			sizeof(matrix3x4) * m_ent->m_CachedBoneData().GetSize());

		if (!recalculate) {
			return true;
		}

		m_ent->set_abs_angles(m_vecRenderAngles);

		//m_ent->m_flDuckAmount( ) = m_flDuckAmount;
		m_ent->m_flLowerBodyYawTarget() = m_flLowerBodyYawTarget;
		m_ent->m_fFlags() = m_fFlags;
		m_ent->m_flSimulationTime() = m_flSimulationTime;

		memcpy(m_ent->m_flPoseParameter(),
			m_PoseParameters,
			sizeof(float) * 24);

		memcpy(m_ent->m_AnimOverlay().GetElements(),
			m_AnimLayers,
			sizeof(C_AnimationLayer) * 13);

		//why? 
		//why not?
		if (reanimate) {
			vec3_t velocity = m_ent->m_vecVelocity();
			int backup_eflags = m_ent->get< int >(0xe8);
			int backup_byte = m_ent->get< byte >(0x35f8);

			m_ent->m_vecVelocity() = m_animVelocity;
			m_ent->get< vec3_t >(0x94) = m_animVelocity;
			m_ent->get< byte >(0x35f8) = 1;
			m_ent->get< int >(0xe8) &= ~0x1000;

			m_state.m_flFeetYawRate = 0.f;

			float backup_curtime = g_csgo.m_globals->m_curtime;
			float backup_frametime = g_csgo.m_globals->m_frametime;

			g_csgo.m_globals->m_curtime = m_flOldSimulationTime + TICK_INTERVAL();
			g_csgo.m_globals->m_frametime = TICK_INTERVAL();

			m_state.m_iLastClientSideAnimationUpdateFramecount -= 1;

			g_cheat.m_ragebot.m_resolver->brute_force(m_ent);

			float lby_delta = m_flLowerBodyYawTarget - m_ent->m_angEyeAngles().y;
			lby_delta = std::remainderf(lby_delta, 360.f);
			lby_delta = std::clamp(lby_delta, -60.f, 60.f);

			float feet_yaw = std::remainderf(m_ent->m_angEyeAngles().y + lby_delta, 360.f);
			if (feet_yaw < 0.f)
				feet_yaw += 360.f;

			m_state.m_flGoalFeetYaw = m_state.m_flCurrentFeetYaw = feet_yaw;

			float backup_duck = m_ent->m_flDuckAmount();
			float backup_duckspeed = m_ent->m_flDuckSpeed();

			m_ent->m_flDuckSpeed() = m_flDuckSpeed;
			m_ent->m_flDuckAmount() = m_animDuck;

			m_ent->m_fFlags() = m_animFlags;

			m_state.m_flLastClientSideAnimationUpdateTime = m_lastAnimTime;
			m_state.update(m_ent->m_angEyeAngles().y, m_ent->m_angEyeAngles().x);

			m_ent->m_fFlags() = m_fFlags;

			m_ent->m_flDuckAmount() = backup_duck;
			m_ent->m_flDuckSpeed() = backup_duckspeed;

			g_csgo.m_globals->m_curtime = backup_curtime;
			g_csgo.m_globals->m_frametime = backup_frametime;

			m_ent->m_vecVelocity() = velocity;
			m_ent->get< vec3_t >(0x94) = velocity;
			m_ent->get< byte >(0x35f8) = backup_byte;
			m_ent->get< int >(0xe8) = backup_eflags;
		}

		m_ent->set_abs_origin(m_vecRenderOrigin);

		byte backup = m_ent->get< byte >(0x270);

		m_ent->get< byte >(0x270) = 0;
		m_ent->invalidate_bone_cache();
		bool ret = m_ent->ce()->SetupBones(m_matrix, 128, BONE_USED_BY_HITBOX, m_flSimulationTime + TICK_INTERVAL());

		m_ent->get< byte >(0x270) = backup;

		return ret;
	}

	bool c_ragebot::lag_record_t::is_valid( ) {
		if( !m_valid ) return false;

		return util::is_tick_valid( m_tickcount );
	}

	void c_ragebot::c_lagcomp::restore_animations( ) {
		if (g_csgo.m_engine()->IsInGame()) {
			if (g_ctx.m_local) {
				for (int i{ 1 }; i < 65; ++i) {
					auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);

					if (ent && ent->is_player()) {
						ent->set_needs_interpolate(true);
						ent->invalidate_bone_cache();
						ent->ce()->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, 0.f);

						ent->m_bClientSideAnimation() = true;
					}
				}
			}
		}
	}

	void c_ragebot::c_lagcomp::store_visuals( ) {
		if (g_csgo.m_engine()->IsInGame()) {
			static float last_simtime[65]{ };

			if (g_ctx.m_local && g_ctx.m_local->is_valid()) {
				for (int i{ }; i < 65; ++i) {
					auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);
					auto& data = m_data_render[i];

					if (ent == g_ctx.m_local)
						continue;

					if (!ent || !ent->is_valid() || ent->m_bGunGameImmunity()) {
						data.clear();
						continue;
					}

					if (std::abs(last_simtime[i] - ent->m_flSimulationTime()) && ent->has_valid_anim()) {
						render_record_t new_record;
						new_record.m_simtime = ent->m_flSimulationTime() + util::get_lerptime();
						new_record.m_origin = ent->m_vecOrigin();

						float delta = ent->m_flSimulationTime() - ent->m_flOldSimulationTime();
						new_record.m_globaltime = g_csgo.m_globals->m_curtime;

						new_record.m_globaltime -= util::get_lerptime();

						new_record.m_validtime = 0.f;

						matrix3x4 matrix[128];
						ent->ce()->SetupBones(matrix, 128, BONE_USED_BY_ANYTHING, 0.f);

						for (size_t i{ }; i < 128; ++i) {
							vec3_t delta = math::get_matrix_position(matrix[i]) - ent->m_vecOrigin();
							math::set_matrix_position(delta + ent->m_vecOrigin(), matrix[i]);
						}

						memcpy(new_record.m_matrix, matrix, sizeof(matrix));

						data.push_front(new_record);

						last_simtime[i] = ent->m_flSimulationTime();
					}

					while (!data.empty() &&
						data.size() > TIME_TO_TICKS(1.f)) {
						data.pop_back();
					}
				}
			}
		}
	}

	bool c_ragebot::c_lagcomp::get_render_record( int ent_index, matrix3x4* out, bool legit ) {
		if (!g_ctx.m_local || !g_ctx.m_local->is_valid())
			return false;

		auto& data = m_data_render[ent_index];
		auto ent = g_csgo.m_entlist()->GetClientEntity< >(ent_index);

		if (data.empty()) {
			g_con->log("empty bt data");
		}

		auto is_time_valid = [&](float time) -> bool {
			auto nci = g_csgo.m_engine()->GetNetChannelInfo();
			if (!nci) return false;

			float latency = util::get_total_latency();
			float correct = std::clamp(latency + g_settings.legit.enabled ? 0.f : util::get_lerptime(), 0.f, 1.f);
			float delta = correct - (g_csgo.m_globals->m_curtime - time);

			return std::abs(delta) <= (legit ? g_settings.legit.backtracking_time : 0.2f);
		};

		for (auto it = data.rbegin(); it != data.rend(); it++) {
			if ((it + 1) == data.rend() || is_time_valid(it->m_simtime) || (it + 1)->m_simtime == ent->m_flSimulationTime() + util::get_lerptime()) {
				if (it->m_origin.dist_to(ent->m_vecOrigin()) < 1.f)
					return false;

				if (ent->is_breaking_lc())
					return false;

				auto next_record = (it + 1);

				if (std::abs(it->m_simtime - ent->m_flSimulationTime()) > TIME_TO_TICKS(16))
					continue;

				if (!it->m_validtime)
					it->m_validtime = g_csgo.m_globals->m_curtime;

				auto nci = g_csgo.m_engine()->GetNetChannelInfo();
				float latency = util::get_total_latency();
				float correct = latency + util::get_lerptime();

				float choke = ent->m_flSimulationTime() - ent->m_flOldSimulationTime();
				float lerptime = next_record == data.rend() ? choke : next_record->m_simtime - it->m_simtime;

				lerptime = std::max(lerptime, TICK_INTERVAL());

				float curtime = g_csgo.m_globals->m_curtime;

				float delta = (curtime - it->m_validtime) / lerptime;
				delta = std::clamp(delta, 0.f, 1.f);

				//EASE OUT BY NAVEWINDRE DO NOT STEAL
				if (lerptime > 0.075f)
					delta = math::ease_out(0.f, 1.f, delta);

				vec3_t next = (it + 1) == data.rend() ? ent->m_vecOrigin() : (it + 1)->m_origin;
				vec3_t lerp = math::lerp(it->m_origin, next, delta);

				matrix3x4 ret[128];

				util::memcpy(ret,
					it->m_matrix,
					sizeof(ret));

				for (size_t i{ }; i < 128; ++i) {
					vec3_t matrix_delta = math::get_matrix_position(it->m_matrix[i]) - it->m_origin;
					math::set_matrix_position(matrix_delta + lerp, ret[i]);
				}

				util::memcpy(out,
					ret,
					sizeof(ret));

				return true;
			}
		}

		return false;
	}

	void c_ragebot::c_lagcomp::fsn_net_update_start( ) {
		static bool restored = true;

		if (!g_settings.rage.enabled()) {
			if (!restored) {
				restore_animations();
				restored = true;
			}

			return;
		}

		restored = false;
		if (g_csgo.m_engine()->IsInGame()) {
			if (g_ctx.m_local && g_ctx.m_local->is_valid()) {
				for (int i{ 1 }; i < 65; ++i) {
					auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);

					if (ent && ent->is_valid() && ent != g_ctx.m_local) {
						ent->set_needs_interpolate(false);
					}
				}
			}
		}
	}

	void c_ragebot::c_lagcomp::fsn_render_start( ) {
		if (!g_settings.rage.enabled())
			return;

		if (!g_ctx.m_local)
			return;

		static bool invalidated = true;
		if (!g_ctx.m_local->is_valid()) {
			if (!invalidated)
				restore_animations();

			invalidated = true;
			return;
		}

		invalidated = false;

		static float stored_lby[65];
		static bool was_dormant[65];

		for (int i{ 1 }; i < 65; ++i) {
			auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);

			if (!ent || !ent->is_player()) {
				//invalidate_animstate( i );
				m_first_update[i] = true;
				continue;
			}

			if (ent == g_ctx.m_local) {
				if (g_ctx.m_stage == FRAME_RENDER_START) {
					//printf( "ya\n" );
					//hooks::update_clientside_animation( ent, 0 );
				}
				continue;
			}

			if (!ent->is_alive()) {
				ent->m_bClientSideAnimation() = true;
				was_dormant[i] = true;
				m_first_update[i] = true;
				continue;
			}

			if (ent->ce()->IsDormant()) {
				ent->m_bClientSideAnimation() = true;
				m_first_update[i] = true;
				was_dormant[i] = true;
				continue;
			}

			if (ent->m_iTeamNum() == g_ctx.m_local->m_iTeamNum()
				&& !g_settings.rage.friendlies) {
				ent->m_bClientSideAnimation() = true;
				continue;
			}

			ent->restore_anim_data(false);
			if (was_dormant[i]) {
				g_cheat.m_ragebot.m_resolver->update_player(i);
				ent->calc_anim_velocity(true);
				ent->fix_animations(true);
				ent->do_ent_interpolation(true);
				stored_lby[i] = ent->m_flLowerBodyYawTarget();
				m_last_simtime[i] = ent->m_flSimulationTime();
				m_flick_time[i] = FLT_MAX;
			}
			else if (g_ctx.m_stage == FRAME_RENDER_START) {
				//ent->m_bClientSideAnimation( ) = false;
				ent->do_ent_interpolation(was_dormant[i]);
				continue;
			}

			was_dormant[i] = false;

			if (m_data_lby[i].size() &&
				m_data_lby[i].front().m_ent != ent) {
				m_data_lby[i].clear();
			}

			if (m_data_normal[i].size() &&
				m_data_normal[i].front().m_ent != ent) {
				m_data_normal[i].clear();
			}

			auto update_anims = [this, &ent](int e) {
				ent->fix_animations();
			};



			float lby = ent->m_flLowerBodyYawTarget();

			float last_simtime = m_last_simtime[i];
			float simtime = ent->m_flSimulationTime();

			if (!!std::abs(simtime - last_simtime)) {
				g_cheat.m_ragebot.m_resolver->update_player(i);
				bool yaw_change = g_cheat.m_ragebot.m_resolver->yaw_change(i);

				ent->calc_anim_velocity(was_dormant[i]);

				bool is_moving = ent->get_anim_velocity().length2d() > 0.1f && !ent->is_fakewalking()
					&& (ent->m_fFlags() & FL_ONGROUND);

				if (ent->m_bGunGameImmunity()) {
					update_anims(i);
				}

				/*else if( is_moving && !was_dormant[ i ] && g_settings.rage.resolver ) {
					m_first_update[ i ] = false;
					ent->m_angEyeAngles( ).y = lby;

					update_anims( i );
					lag_record_t new_record( ent );

					m_data_lby[ i ].push_front( new_record );
					stored_lby[ i ] = lby;
				}*/

				/*else if( ( lby != stored_lby[ i ] ) && !was_dormant[ i ] && g_settings.rage.resolver ) {
					stored_lby[ i ] = lby;
					ent->m_angEyeAngles( ).y = lby;

					update_anims( i );
					lag_record_t new_record( ent );

					if( !m_first_update[ i ] ) {
						m_data_lby[ i ].push_front( new_record );
						m_flick_time[ i ] = ent->m_flOldSimulationTime( ) + TICK_INTERVAL( );
					}

					m_first_update[ i ] = false;
				}*/
				else {
					bool has_updated = false;
					/*if( g_settings.rage.resolver( ) ) {
						float anim_time = ent->m_flOldSimulationTime( ) + TICK_INTERVAL( );

						if( anim_time - m_flick_time[ i ] > 1.1f && !m_first_update[ i ] ) {
							has_updated = true;

							ent->m_angEyeAngles( ).y = lby;
							m_flick_time[ i ] = anim_time;
							update_anims( i );
							lag_record_t new_record( ent );
							new_record.m_balanceadjust = true;
							m_data_lby[ i ].push_front( new_record );
						}
					}*/

					if (!has_updated) {
						ent->fix_animations(false, yaw_change);

						lag_record_t new_record(ent);
						new_record.m_sim_record = true;

						new_record.m_shots = g_cheat.m_ragebot.m_resolver->get_shots(i);

						m_data_normal[i].push_front(new_record);
					}
				}


				g_cheat.m_ragebot.m_resolver->yaw_change(i) = false;
			}

			ent->do_ent_interpolation(false);
			m_last_simtime[i] = simtime;

			while (!m_data_lby[i].empty() &&
				m_data_lby[i].size() > TIME_TO_TICKS(1.0f)) {
				m_data_lby[i].pop_back();
			}

			while (!m_data_normal[i].empty() &&
				m_data_normal[i].size() > TIME_TO_TICKS(1.0f)) {
				m_data_normal[i].pop_back();
			}
		}
	}

	RecordType_t c_ragebot::c_lagcomp::can_backtrack_entity( int ent_index ) {
		auto ent = g_csgo.m_entlist()->GetClientEntity< >(ent_index);
		if (ent->is_breaking_lc()) {
			return RECORD_NONE;
		}

		auto data_lby = &m_data_lby[ent_index];
		auto data_normal = &m_data_normal[ent_index];

		if (!data_lby->empty()) {
			for (auto& it : *data_lby) {
				if (it.is_valid()) return RECORD_LBY;
			}
		}

		if (!data_normal->empty()) {
			for (auto& it : *data_normal) {
				if (it.is_valid()) return RECORD_NORMAL;
			}
		}

		return RECORD_NONE;
	}

	void c_ragebot::c_lagcomp::invalidate_bone_caches( ) {
		for( int i{ 1 }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );
			if( !ent || !ent->is_player( ) )
				continue;

			ent->m_iMostRecentModelBoneCounter( ) -= 1;
			ent->m_flLastBoneSetupTime( ) = -FLT_MAX;
		}
	}

	c_ragebot::lag_record_t* c_ragebot::c_lagcomp::get_newest_record( int ent_index ) {
		lag_record_t* newest_record{ };
		if (get_records(ent_index, RECORD_LBY)->size())
			newest_record = &get_records(ent_index, RECORD_LBY)->front();

		if (get_records(ent_index, RECORD_NORMAL)->size()) {
			auto& newest_rec = get_records(ent_index, RECORD_NORMAL)->front();
			if ((newest_record && newest_rec.m_flSimulationTime > newest_record->m_flSimulationTime) ||
				!newest_record) {
				newest_record = &newest_rec;
			}
		}

		return newest_record;
	}

	c_ragebot::lag_record_t* c_ragebot::c_lagcomp::get_newest_valid_record( int ent_index ) {
		lag_record_t* last_simtime_record{ };
		for (auto& it : *get_records(ent_index, RECORD_LBY)) {
			if (!it.is_valid()) continue;
			last_simtime_record = &it;
			break;
		}

		for (auto& it : *get_records(ent_index, RECORD_NORMAL)) {
			if (!it.is_valid()) continue;
			if (!last_simtime_record || (last_simtime_record && it.m_flSimulationTime > last_simtime_record->m_flSimulationTime)) {
				last_simtime_record = &it;
			}
			break;
		}

		return last_simtime_record;
	}

	int c_ragebot::c_lagcomp::backtrack_entity( int ent_index, RecordType_t type, lag_record_t** out_record ) {
		if (type == RECORD_NONE) return -1;

		auto& data = *get_records(ent_index, type);
		auto ent = g_csgo.m_entlist()->GetClientEntity< >(ent_index);

		auto check_record = [&ent, &out_record, &ent_index](lag_record_t* record, bool check_pelvis) {
			if (record->m_balanceadjust && record->m_shot) return -1;

			vec3_t head_pos = record->m_vecHeadPos;
			vec3_t pelvis_pos = record->m_vecPelvisPos;

			float damage = g_cheat.m_autowall.run(g_ctx.m_local, ent, head_pos, false) * 4.f;
			float min_damage = std::min< float >(ent->m_iHealth(),
				g_settings.rage.active->m_damage);

			//g_con->log( xors( "record: %d damage: %f" ), counter++, damage );

			int shots = g_cheat.m_ragebot.m_resolver->get_shots(ent_index);
			bool recalc = shots != record->m_shots && record->m_sim_record;


			if (damage > min_damage) {
				if (!record->restore(recalc, recalc))
					return -1;

				if (out_record) {
					*out_record = record;
				}
				return record->m_tickcount;
			}

			if (check_pelvis) {
				float pelvis_damage = g_cheat.m_autowall.run(g_ctx.m_local, ent, pelvis_pos, false);
				if (pelvis_damage > min_damage) {
					if (!record->restore(recalc, recalc))
						return -1;

					if (out_record) {
						*out_record = record;
					}
					return record->m_tickcount;
				}
			}

			return -1;
		};

		if (data.empty())
			return -1;

		auto* back = &data.back();
		auto* front = &data.front();

		for (auto& it : data) {
			if (!it.is_valid()) continue;
			front = &it;
			break;
		}

		for (auto& it : util::reverse_iterator(data)) {
			if (!it.is_valid()) continue;
			back = &it;
			break;
		}

		int front_result = check_record(front, true);
		if (front_result != -1) return front_result;

		int back_result = check_record(back, true);
		if (back_result != -1) return back_result;

		return -1;
	}
}