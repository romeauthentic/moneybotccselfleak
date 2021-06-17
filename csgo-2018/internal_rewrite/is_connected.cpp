#include "mem.hpp"
#include "hooks.hpp"

bool __fastcall hooks::is_connected( void* ecx_, void* edx_ ) {
	static auto inventory = pattern::first_code_match(g_csgo.m_chl.dll(), xors("84 C0 75 04 B0 01 5F"));
	static auto is_connected_o = g_csgo.m_engine->get_old_function< decltype(&is_connected) >(27);
	stack_t stack(get_baseptr());

	if (stack.return_address() == inventory && g_settings.misc.unlock_inventory())
		return false;


	return is_connected_o(ecx_, 0);
}