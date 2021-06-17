#include "hooks.hpp"
#include "base_cheat.hpp"
void __fastcall hooks::play_sound( void* ecx_, void*, const char* file ) {
	static auto play_sound_o = g_csgo.m_surface->get_old_function< decltype( &play_sound ) >( 82 );
	static auto is_ready = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 00 00 00 00 57 83 BE" ), 0 );
	static bool was_played = false;
	static float played_tick = 0.f;

	play_sound_o( ecx_, 0, file );

	if( !g_settings.misc.auto_accept || g_csgo.m_engine( )->IsInGame( ) ) {
		was_played = false;
		return;
	}

	if( strstr( file, xors( "competitive_accept_beep.wav" ) ) && !was_played ) {
		was_played = true;
		played_tick = GetTickCount( ) * 0.001f + g_settings.misc.auto_accept_delay;
		g_cheat.m_extra.server_information( );
		g_cheat.m_visuals.reset_local_dmg( );
	}

	if( was_played && GetTickCount( ) * 0.001f > played_tick ) {
		reinterpret_cast< void( __cdecl* )( ) >( is_ready )( );
		was_played = false;
	}
}