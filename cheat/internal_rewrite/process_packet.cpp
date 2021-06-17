#include "hooks.hpp"

bool __fastcall hooks::process_packet(void *ecx, void *, void *packet, bool has_header) {
	bool result = process_packet_o(ecx, nullptr, packet, has_header);

	//auto *cl = g_csgo.m_global_state->get_client_state();
	//
	//if (!g_csgo.m_engine()->IsInGame() || !cl)
	//	return result;
	//
	//// new events could be coming in
	//uint32_t v3 = *(uint32_t*)cl + 19948;
	//
	//if (!v3)
	//	return result;
	//
	//for (; !!v3; ) {
	//	float &v8 = *(float *)((uint32_t)v3 + 4);
	//
	//	if (v8 > 0.0f)
	//		v8 = 0.0f; 
	//
	//	uint32_t v11 = *(uint32_t *)((uint32_t)v3 + 56);
	//	v3 = v11;
	//}
	//
	////printf("yea\n");
	//
	//// CL_FireEvents will make sure the events are triggered instantly
	//// i honestly can't wait for you to call me a retard for this :DDDDDDDDDDDDD
	//// 55 8B EC 83 EC 08 53 8B 1D ? ? ? ? 56 57 83 BB ? ? ? ? ? 74 12 8D 8B ? ? ? ? E8
	//static auto fire_events = pattern::first_code_match< void(*)() >(
	//	g_csgo.m_engine.dll(),
	//	xors("55 8B EC 83 EC 08 53 8B 1D ? ? ? ? 56 57 83 BB ? ? ? ? ? 74 12 8D 8B ? ? ? ? E8")
	//);
	//fire_events();

	return result;
}