#include "hooks.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::render_view( void* ecx, void* edx, CViewSetup& view, int clear_flags, int what_to_draw ) {
	static auto render_view_o = g_gmod.m_view_render->get_old_function< decltype( &hooks::render_view ) >( 6 );
	if( g_gmod.m_panic ) return render_view_o( ecx, edx, view, clear_flags, what_to_draw );
	
	render_view_o( ecx, edx, view, clear_flags, what_to_draw );
	g_cheat.m_visuals.store_data( );
}