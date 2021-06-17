#include "hooks.h"
#include "interfaces.h"
#include "base_cheat.h"

void __fastcall hooks::frame_stage_notify( void* ecx_, void* edx_, frame_stages_t stage ) {
	static auto fsn_o = cl.m_chl->get_old_function< decltype( hooks::frame_stage_notify )* >( 35 ); //35

	g_ctx.m_stage = stage;

	if ( cl.m_panic ) {
		return fsn_o( ecx_, edx_, stage );
	}

	switch( stage ) {
	case FRAME_NET_UPDATE_START:
		g_ctx.run_frame( );
		fsn_o( ecx_, 0, stage );

		return;

	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		g_cheat.extra.float_ragdolls( );
		//g_cheat.m_skins( );
		g_cheat.visuals.update_glow( );
		fsn_o( ecx_, 0, stage );

		return;

	case FRAME_RENDER_START:
		//g_cheat.extra.disable_post_processing( );
		util::disable_pvs( );
		break;

	case FRAME_RENDER_END:
		fsn_o( ecx_, 0, stage );
		g_cheat.chams.m_materials.update_materials( );
		return;
	}

	return fsn_o( ecx_, edx_, stage );
}
