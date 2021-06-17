#include <intrin.h>

#include "hooks.hpp"
#include "context.hpp"
#include "settings.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::draw_small_entities( void* ecx, void* edx, bool state ) {
	static auto draw_small_entities_o = g_csgo.m_clientleaf->get_old_function< decltype( &hooks::draw_small_entities ) >( 39 );

	static auto ret_addr = pattern::first_code_match< void* >(g_csgo.m_chl.dll(), "FF 90 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 28", 6);

	// please shoot me
	// i have given up
	// i want to leave
	if( _ReturnAddress( ) == ret_addr ) {
		g_ctx.m_stage = FRAME_RENDER_END;
		g_cheat.m_visuals.world_modulate( );

		// FRAME_RENDER_END
		g_cheat.m_ragebot.m_lagcomp->store_visuals( );
		g_cheat.m_chams.m_materials.update_materials( );
		g_cheat.m_extra.no_recoil( true );
	}

	draw_small_entities_o( ecx, edx, state );
}