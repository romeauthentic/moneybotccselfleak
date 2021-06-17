#include "hooks.hpp"
#include "base_cheat.hpp"

bool __fastcall hooks::get_player_info( void* ecx_, void* edx, int ent_num,  player_info_t* info ) {
	static auto original_fn = g_csgo.m_engine->get_old_function< decltype( &get_player_info ) >( 8 );



	return original_fn( ecx_, edx, ent_num, info );
}