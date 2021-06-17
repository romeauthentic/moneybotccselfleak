#include "hooks.hpp"
#include "chams.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::shut_down( void* ecx_, void* edx_ ) {
	static auto original_fn = g_csgo.m_chl->get_old_function< decltype( &shut_down ) >( 3 );
	
	g_csgo.m_engine( )->ClientCmd_Unrestricted( xors( "host_writeconfig" ) );
	//fuck you
	abort( );

	g_cheat.m_chams.m_materials.destroy_materials( );
	return original_fn( ecx_, edx_ );
}