#include "hooks.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "base_cheat.hpp"


void __fastcall hooks::frame_stage_notify( void* ecx_, void* edx_, ClientFrameStage_t stage ) {
	static auto frame_stage_notify_o = g_gmod.m_chl->get_old_function< decltype( &hooks::frame_stage_notify ) >( 35 );
	if ( g_gmod.m_panic ) return frame_stage_notify_o( ecx_, edx_, stage );

	g_ctx.m_stage = stage;

	switch( stage ) {
	case FRAME_NET_UPDATE_START:
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		break;
	case FRAME_RENDER_START:
		break;
	case FRAME_RENDER_END:
		break;
	}

	frame_stage_notify_o( ecx_, edx_, stage );
}