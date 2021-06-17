#include "hooks.h"
#include "chams.h"
#include "base_cheat.h"

void __fastcall hooks::shut_down( void* ecx_, void* edx_ ) {
	static auto original_fn = cl.m_chl->get_old_function< decltype( &shut_down ) >( 3 );

	cl.m_engine( )->ClientCmd_Unrestricted( xors( "host_writeconfig" ) );
	//fuck you
	abort( );

	//how about fuck you instead dumb fucking tentacle hentai lookin ass mofo

	//g_cheat.chams.m_materials.destroy_materials( );
	return original_fn( ecx_, edx_ );
}