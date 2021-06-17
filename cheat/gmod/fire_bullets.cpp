#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

decltype( &hooks::fire_bullets ) hooks::fire_bullets_o{ };
void __fastcall hooks::fire_bullets( void* ecx, void* edx, fire_bullets_info* info ) {
	if( g_gmod.m_panic ) return fire_bullets_o( ecx, edx, info );

	if( g_ctx.run_frame( ) &&
		g_ctx.m_local == ecx ) { // only our shots.
		g_cheat.m_aimbot.log_shot( ( c_base_player* )ecx, info ); // dynamically logging spread vectors for nospread.
	}

	return fire_bullets_o( ecx, edx, info );
}