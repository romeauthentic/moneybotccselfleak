#include "hooks.hpp"
#include "context.hpp"

int __fastcall hooks::debug_show_spread_get_int( void* ecx_, void* edx_ ) {
	static auto original_fn = g_csgo.m_debug_show_spread->get_old_function< decltype( &debug_show_spread_get_int ) >( 13 );
	if( !g_settings.visuals.snipers_crosshair || !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) || g_ctx.m_local->m_bIsScoped( ) ) {
		return original_fn( ecx_, 0 );
	}

	auto weapon = g_ctx.m_local->get_weapon( );
	if( !weapon || !weapon->is_sniper( ) ) {
		return original_fn( ecx_, 0 );
	}

	return 3;
}