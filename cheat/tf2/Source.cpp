#pragma warning (disable:4018)

#include <Windows.h>
#include <iostream>

#include "console.hpp"
#include "interfaces.h"
#include "search.h"
#include "hooks.h"
#include <thread>
#include "base_cheat.h"

HMODULE g_dll;

//u need to run the game with -nod3d9ex in launch option
//alternatively set mat_supports_d3d9ex 0 and mat_disable_d3d9ex 1
//(normally they're hidden cvars)

void main_thread( HMODULE module ) {

	cl.initialize( );

	while ( !( GetAsyncKeyState( VK_END ) & 0x8000 ) ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}

}

bool __stdcall DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved ) {
	if ( fdwReason == DLL_PROCESS_ATTACH /*&& ( uintptr_t )( lpvReserved ) == 20*/ ) { //nigga suck who dick?
		DisableThreadLibraryCalls( hinstDLL );
		g_dll = hinstDLL;
		// coca cola coca cola yayo bought me everything

		//no i did not dummy

		CreateThread( nullptr, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( main_thread ), hinstDLL, 0, nullptr );
	}

	return true;
}