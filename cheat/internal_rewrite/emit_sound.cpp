#include "base_cheat.hpp"
#include "hooks.hpp"
#include "settings.hpp"

void __fastcall hooks::emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash,
	const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin,
	const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry, int unk01 ) {
#ifdef HEADER_MODULE
	// static auto is_ready = g_header.patterns.is_ready;
	static auto is_ready = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) );
#else
	static auto is_ready = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) );
#endif

	//static auto emit_sound_o = g_csgo.m_engine_sound->get_old_function< decltype( &hooks::emit_sound ) >( 5 );

	// this is already implemented in visual_player.cpp
	//if( !g_csgo.m_panic && origin ) {
	//	auto entity = g_csgo.m_entlist( )->GetClientEntity< >( ent );
	//	if( entity && entity->is_player( ) && entity->ce( )->IsDormant( ) &&
	//		ent >= 0 && ent <= 64 )
	//		g_cheat.m_visuals.update_position( ent, *origin );
	//}

	static bool was_played = false;
	static float played_tick = 0.f;

	if( g_settings.misc.auto_accept && !g_csgo.m_engine( )->IsInGame( ) ) {
		if( strstr( sound, xors( "UIPanorama.popup_accept_match_beep" ) ) && !was_played ) {
			was_played = true;
			played_tick = GetTickCount( ) * 0.001f + g_settings.misc.auto_accept_delay;
			g_cheat.m_visuals.reset_local_dmg( );
		}

		if( was_played && GetTickCount( ) * 0.001f > played_tick ) {
			reinterpret_cast< bool( __stdcall* )( const char* ) >( is_ready )( xors( "" ) );
			was_played = false;
		}	
	}
	else {
		was_played = false;
	}

	if( g_settings.misc.no_sound ) {
	    if( strstr( sample, xors( "weapon" ) ) ) volume = g_settings.misc.weapon_volume;
	}

    if( g_settings.misc.auto_accept ) {
        if ( std::strstr( sample, xors( "competitive_accept_beep" ) ) ) {
			volume = 0.0f;
		}
    }

	emit_sound_o( ecx_, 0, filter, ent, channel, sound, hash, sample,
		volume, attenuation, seed, flags, pitch, origin, direction,
		origins, update_pos, sound_time, speaker_entry, unk01 ); 
}