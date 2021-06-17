#include "hooks.hpp"
#include "base_cheat.hpp"
void __fastcall hooks::play_sound( void* ecx_, void*, const char* file ) {
#ifdef HEADER_MODULE
	// static auto is_ready = g_header.patterns.is_ready;
	static auto is_ready = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "85 C0 75 12 8D 50 02 33 C9 E8" ), 0x13 );
#else
	static auto is_ready = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "85 C0 75 12 8D 50 02 33 C9 E8" ), 0x13 );
#endif
	//static bool was_played = false;
	//static float played_tick = 0.f;

	play_sound_o( ecx_, 0, file );

	/*if( !g_settings.misc.auto_accept || g_csgo.m_engine( )->IsInGame( ) ) {
		was_played = false;
		return;
	}

	if( strstr( file, xors( "UIPanorama.popup_accept_match_beep" ) ) && !was_played ) {
		was_played = true;
		played_tick = GetTickCount( ) * 0.001f + g_settings.misc.auto_accept_delay;
		g_cheat.m_visuals.reset_local_dmg( );
	}

	if( was_played && GetTickCount( ) * 0.001f > played_tick ) {
		reinterpret_cast< bool( __stdcall* )( const char* ) >( is_ready )( "" );
		was_played = false;
	}*/
}