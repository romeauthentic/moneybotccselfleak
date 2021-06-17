#include "hooks.hpp"
#include "chams.hpp"
#include "base_cheat.hpp"
#include "detours.h"

void __fastcall hooks::shut_down( void* ecx_, void* edx_ ) {
	static auto original_fn = shut_down_o;
	
	g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "host_writeconfig" ) );
	//fuck you

	memory::detours.restore( );

	g_con->destroy( );

	abort( );

	g_cheat.m_chams.m_materials.destroy_materials( );
	return original_fn( ecx_, edx_ );
}