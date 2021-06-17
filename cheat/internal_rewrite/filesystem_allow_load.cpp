#include <intrin.h>

#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

int __fastcall hooks::filesystem_allow_load( void* ecx_, void* edx_ ) {
	static void* return_1 = pattern::first_code_match<void*>( g_csgo.m_chl.dll( ), "83 F8 02 0F 85 ? ? ? ? 8B 3D ? ? ? ? FF D7" );
	static void* return_2 = pattern::first_code_match<void*>( g_csgo.m_engine.dll( ), "83 F8 02 75 6C 68 ? ? ? ? FF 15 ? ? ? ? 8B 4C 24 28 83 C4 04" );

	if ( _ReturnAddress( ) == return_1 || _ReturnAddress( ) == return_2 )
		return 0;

	return filesystem_allow_load_o( ecx_, edx_ );
}