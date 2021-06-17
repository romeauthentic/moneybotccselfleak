#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

void __cdecl hooks::lby_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	lby_proxy.get_old_function( )( proxy_data_const, entity, output );

	if( !g_csgo.m_panic ) {
		auto player = ( c_base_player* )( entity );
		if( player && player == g_ctx.m_local ) {
			g_cheat.m_ragebot.m_antiaim->on_lby_proxy( );
		}
	}
}

void __cdecl hooks::last_shot_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	if (!proxy_data_const->m_Value.m_Int) {
		return;
	}

	last_shot_proxy.get_old_function()(proxy_data_const, entity, output);
	if (!g_csgo.m_panic && proxy_data_const && g_settings.rage.enabled() && g_settings.rage.resolver()) {
		auto wep = (c_base_weapon*)(entity);
		if (wep && !wep->is_knife() && !wep->is_grenade()) {
			auto owner = g_csgo.m_entlist()->GetClientEntityFromHandle(wep->m_hOwner());
			if (owner && owner->is_valid() && owner != g_ctx.m_local && g_ctx.m_local->is_valid() && owner->has_valid_anim()) {
				if (owner->m_iTeamNum() == g_ctx.m_local->m_iTeamNum() && !g_settings.rage.friendlies)
					return;

				static float last_time = 0.f;
				float time = wep->m_fLastShotTime();

				if (!time)
					return;

				float last_anim = owner->m_flOldSimulationTime();
				float anim_time = owner->m_flSimulationTime();
				auto record = g_cheat.m_ragebot.m_lagcomp->get_newest_record(owner->ce()->GetIndex());
				float& last_update = g_cheat.m_ragebot.m_lagcomp->get_last_updated_simtime(owner->ce()->GetIndex());

				if (time > last_anim && time <= anim_time && owner->m_flSimulationTime() != last_update) {
					last_update = owner->m_flSimulationTime();
					owner->fix_animations();

					features::c_ragebot::lag_record_t record(owner);
					record.m_shot = true;

					g_cheat.m_ragebot.m_lagcomp->store_record(owner->ce()->GetIndex(), RECORD_LBY, record);
					last_time = time;
				}
			}
		}
	}
}