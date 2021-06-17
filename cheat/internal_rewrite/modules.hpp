#pragma once
#include <cinttypes>

struct patterns_t
{
	uint32_t beam_ptr; // B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9
	uint32_t move_helper; // 8B 0D ? ? ? ? 8B 46 08 68
	uint32_t calc_abs_velocity; // 55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7 87
	uint32_t animstate_update; // 55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24
	uint32_t draw_small_entities_retaddr; // FF 90 ? ? ? ? FF 15 ? ? ? ? 84 C0 74 28
	uint32_t is_breakable_ptr; // 55 8B EC 51 56 8B F1 85 F6 74 68 83 BE
	uint32_t set_abs_angles; // 55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8
	uint32_t set_abs_origin; // 55 8B EC 83 E4 F8 51 53 56 57 8B F1
	uint32_t glow_manager; // 0F 11 05 00 00 00 00 83 C8 01
	uint32_t smoke_count; // 55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0
	uint32_t in_prediction_retaddr; // 84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06
	uint32_t net_update_retaddr; // 5F 5E 5D C2 04 00 83 3D ? ? ? ? ?
};


struct header_t
{
	int32_t days_left;
	char username[ 32 ];
};

extern header_t g_header;