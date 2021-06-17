#include "hooks.hpp"
#include "chams.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::level_init_pre_entity( void* ecx_, void* edx_, const char* map_name ) {
	static auto original_fn = g_csgo.m_chl->get_old_function< decltype( &level_init_pre_entity ) >( 5 );

	bool has_level_initialized = **( bool** )( ( uintptr_t )original_fn + 8 );

	// obviously source engine is perfect
	// "HACK: Bogus, but the logic is too complicated in the engine"

	if( !has_level_initialized ) {
		//g_cheat.m_chams.m_materials.initialize_materials( );
	}

	return original_fn( ecx_, edx_, map_name );
}