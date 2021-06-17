#include "hooks.hpp"
#include "renderer.hpp"
#include "context.hpp"
#include "console.hpp"
#include "base_cheat.hpp"
#include "math.hpp"
#include "settings.hpp"

void __fastcall hooks::paint_traverse( void* ecx_, void* edx_, unsigned int panel, bool force_repaint, bool allowforce ) {
	static auto paint_traverse_o = g_gmod.m_panel->get_old_function< decltype( &hooks::paint_traverse ) >( 41 );
	if( g_gmod.m_panic ) return paint_traverse_o( ecx_, 0, panel, force_repaint, allowforce );


	static uint32_t mat_system_top_panel{ };
	if( !mat_system_top_panel ) {
		auto panel_name = g_gmod.m_panel( )->GetName( panel );
		auto panel_hash = hash::fnv1a( panel_name );
		if( fnv( "MatSystemTopPanel" ) == panel_hash ) {
			mat_system_top_panel = panel;
		}
	}

	if( panel && panel == mat_system_top_panel ) {
		g_gmod.m_panel( )->SetMouseInputEnabled( panel, g_settings.menu.open );
	}

	return paint_traverse_o( ecx_, 0, panel, force_repaint, allowforce );
}