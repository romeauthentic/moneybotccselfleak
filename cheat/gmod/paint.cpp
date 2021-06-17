#include <intrin.h>

#include "hooks.hpp"
#include "interface.hpp"
#include "context.hpp"
#include "math.hpp"

#include "base_cheat.hpp"

void __fastcall hooks::paint( void* ecx, void* edx, int mode ) {
	static auto paint_o = g_gmod.m_engine_vgui->get_old_function< decltype( &hooks::paint ) >( 13 );
	if( g_gmod.m_panic ) return paint_o( ecx, edx, mode );

	paint_o( ecx, edx, mode );

	if( mode & 2 ) {
		if( g_ctx.run_frame( ) ) {
			g_cheat.m_visuals.store_data( );
		}
	}
}