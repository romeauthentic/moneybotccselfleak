#include "base_cheat.h"
#include "hooks.h"
#include "settings.h"

//eenie meenie miney moe i  kepy this in here incase i wanted to ever do something with it i could fix it to match tf2's emitsound but yknow what? i prolly wont watadearhiugaer sorry i spilt water on my desk and smashed my face into the keyboard

void __fastcall hooks::emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash,
	const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin,
	const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry ) {
	static auto emit_sound_o = cl.m_engine_sound->get_old_function< decltype( &hooks::emit_sound ) >( 5 );

	if( !cl.m_panic && origin ) {
		auto entity = cl.m_entlist( )->get_client_entity< c_base_player >( ent );
		if( entity && entity->is_player( ) && entity->ce( )->is_dormant( ) &&
			ent >= 0 && ent <= 64 )
			g_cheat.visuals.update_position( ent, *origin );
	}

	if( strstr( sample, xors( "weapon" ) ) && g_settings.misc.no_sound )
		volume = 0.f;

	emit_sound_o( ecx_, 0, filter, ent, channel, sound, hash, sample,
		volume, attenuation, seed, flags, pitch, origin, direction,
		origins, update_pos, sound_time, speaker_entry );
}