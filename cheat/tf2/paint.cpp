#include "hooks.h"
#include "interfaces.h"
#include "search.h"
#include "graphics.h"
#include "settings.h"
#include "base_cheat.h"


void __fastcall hooks::paint_traverse( void *ecx_, void *edx_, unsigned int panel, bool forcerepaint, bool allowforce ) {
	static auto paint_o = cl.m_panel->get_old_function< decltype( hooks::paint_traverse )* >( 41 );
	paint_o( ecx_, edx_, panel, forcerepaint, allowforce );

	if ( cl.m_panic /*|| !g_ctx->run_frame( ) this will disable esp when dead u dummie!!!*/ ) {
		return;
	}

	static uint32_t focus_overlay_panel{ };
	if ( !focus_overlay_panel ) {
		std::string panelname( cl.m_panel( )->GetName( panel ) );
		if ( panelname.find( "FocusOverlayPanel" ) != std::string::npos ) { //matsystemtoppanel is glitched the fuck out in tf2
			focus_overlay_panel = panel;									//hovewer this draws on shit other than the main screen
		}																	//so we should get paint working
	}

	if ( panel == focus_overlay_panel ) { //i still wonder why dont we do this in paint
		//dank 2007 esp here
		cl.m_panel( )->SetMouseInputEnabled( panel, g_settings.menu.open || g_con->m_open );
	}
}

void __fastcall hooks::paint( void* ecx_, void* edx_, int mode ) {
	static auto engine_paint_o = cl.m_vgui->get_old_function< decltype( hooks::paint )* >( 13 );
	static auto start_d = pattern::first_code_match< void( __thiscall* )( void* ) >( cl.m_surface.dll( ), "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14" );
	static auto finish_d = pattern::first_code_match< void( __thiscall* )( void* ) >( cl.m_surface.dll( ), "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00" );

	engine_paint_o( ecx_, edx_, mode );

	if ( cl.m_panic ) return;

	if ( mode & PAINT_UIPANELS ) {
		//since we need to call this in a game thread
		static bool init = false;
		if( !init ) {
			g_cheat.chams.m_materials.initialize_materials( );
			init = true;
		}

		start_d( cl.m_surface( ) );

		//did you say lazy?

		g_renderer.draw_box( 0, 0, 1, 1, clr_t( 0, 0, 0, 1 ) );
		if( !g_settings.misc.hide_from_obs )
			g_cheat.visuals( );

		finish_d( cl.m_surface( ) );
	}
}