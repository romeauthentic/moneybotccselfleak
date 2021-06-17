#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

bool __fastcall hooks::do_post_screen_space_effects(void* ecx_, void* edx_, CViewSetup* setup) {
	static auto old_fn = g_csgo.m_clientmode->get_old_function< decltype(&do_post_screen_space_effects) >(44);

	g_ctx.m_drawing_postscreenspace = true;
	if (g_csgo.m_engine()->IsInGame() && g_ctx.m_local) {
		g_cheat.m_visuals.update_glow();
	}

	bool ret = old_fn(ecx_, 0, setup);
	g_ctx.m_drawing_postscreenspace = false;

	return ret;
}