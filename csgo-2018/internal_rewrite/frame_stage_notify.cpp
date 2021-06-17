#include "hooks.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "base_cheat.hpp"

#if 0
const char* crash_str = R"("
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
398274 entities in bone setup array.Should have been cleaned up by now
")";
#endif

void __fastcall hooks::frame_stage_notify( void* ecx_, void* edx_, ClientFrameStage_t stage ) {
	static auto frame_stage_notify_o = g_csgo.m_chl->get_old_function< decltype( &hooks::frame_stage_notify ) >( 36 );
	if ( g_csgo.m_panic ) return frame_stage_notify_o( ecx_, edx_, stage );
	g_ctx.m_stage = stage;
	g_cheat.m_visuals.world_modulate( );
	static bool rich_presence_active = false;
	switch (stage) {
	case FRAME_NET_UPDATE_START:
		if (g_settings.misc.rich_presence()) {
			g_cheat.m_extra.rich_presence_flex();
			rich_presence_active = true;
		}
		else if (rich_presence_active) {
			if (!g_settings.misc.rich_presence())
				g_cheat.m_extra.remove_rich_presence_flex();

			rich_presence_active = false;
		}
		g_cheat.m_visuals.draw_sound();
		g_cheat.m_visuals.draw_tracers();
		g_cheat.m_ragebot.m_lagcomp->fsn_net_update_start();
		frame_stage_notify_o(ecx_, 0, stage);

		return;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		g_cheat.m_extra.float_ragdolls();
		g_cheat.m_skins();
		g_cheat.m_ragebot.m_resolver->frame_stage_notify();
		g_cheat.m_ragebot.m_lagcomp->fsn_render_start();
		frame_stage_notify_o(ecx_, 0, stage);

		return;

	case FRAME_RENDER_START:
		g_cheat.m_extra.disable_post_processing();
		g_cheat.m_extra.no_flash();
		g_cheat.m_extra.no_recoil(false);
		util::disable_pvs();
		g_cheat.m_ragebot.m_lagcomp->fsn_render_start();
		//g_cheat.m_ragebot.m_lagcomp->fsn_render_start();
		//g_cheat.m_ragebot.m_lagcomp->invalidate_bone_caches( );
		if (g_csgo.m_input()->m_fCameraInThirdPerson) {
			bool real_yaw = g_settings.rage.anti_aim();
			if (g_ctx.get_last_cmd() && g_ctx.m_local && g_ctx.m_local->is_valid()) {
				g_ctx.m_local->m_vecThirdpersonAngles() = (real_yaw ? g_ctx.m_thirdperson_angle : g_ctx.get_last_cmd()->m_viewangles);
			}
		}

		g_cheat.m_extra.no_smoke();
		if (g_settings.misc.no_smoke) {
			static auto smoke_count = pattern::first_code_match(g_csgo.m_chl.dll(),
				xors("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
			**reinterpret_cast<int**>(smoke_count + 0x8) = 0;
		}
		break;

	case FRAME_RENDER_END:
		g_cheat.m_ragebot.m_lagcomp->store_visuals();
		frame_stage_notify_o(ecx_, 0, stage);
		g_cheat.m_chams.m_materials.update_materials();
		g_cheat.m_extra.no_recoil(true);
		break;
	}
	frame_stage_notify_o( ecx_, 0, stage );
}