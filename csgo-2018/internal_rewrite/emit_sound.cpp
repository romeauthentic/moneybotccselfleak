#include "base_cheat.hpp"
#include "hooks.hpp"
#include "settings.hpp"
void __fastcall hooks::emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash,
	const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin,
	const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry) {
	static auto emit_sound_o = g_csgo.m_engine_sound->get_old_function< decltype(&hooks::emit_sound) >(5);

	//SND_STOP 
	if (strstr(sample, xors("null")))
		flags = (1 << 2) | (1 << 5);
	else {
		if (!g_csgo.m_panic && origin) {
			auto entity = g_csgo.m_entlist()->GetClientEntity< >(ent);
			if (entity && entity->is_player() && entity->ce()->IsDormant() &&
				ent >= 0 && ent <= 64)
				g_cheat.m_visuals.update_position(ent, *origin);
		}
	}

	if (strstr(sample, xors("weapon")) && g_settings.misc.no_sound)
		volume = 0.f;

	emit_sound_o(ecx_, 0, filter, ent, channel, sound, hash, sample,
		volume, attenuation, seed, flags, pitch, origin, direction,
		origins, update_pos, sound_time, speaker_entry);
}