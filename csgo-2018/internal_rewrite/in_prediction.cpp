#include "mem.hpp"
#include "hooks.hpp"
#include "context.hpp"

#undef max

bool __fastcall hooks::in_prediction( void* ecx_, void* edx_ ) {
	static auto in_prediction_o = g_csgo.m_prediction->get_old_function< decltype(&hooks::in_prediction) >(14);
	static auto ret_address = pattern::first_code_match(g_csgo.m_chl.dll(),
		xors("84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06"));

	if (g_ctx.m_local && ret_address && g_settings.rage.enabled()) {
		stack_t stack(get_baseptr());
		int local_team = g_ctx.m_local->m_iTeamNum();

		if (stack.return_address() == ret_address) {
			for (size_t i{ }; i < 65; ++i) {
				auto ent = g_csgo.m_entlist()->GetClientEntity< >(i);

				if (!ent || !ent->is_valid() || ent == g_ctx.m_local)
					continue;

				if (ent->m_iTeamNum() != local_team || g_settings.rage.friendlies) {
					ent->invalidate_bone_cache();

					ent->m_flLastBoneSetupTime() = std::numeric_limits< float >::quiet_NaN();
				}
			}
		}
	}

	return in_prediction_o(ecx_, 0);
}