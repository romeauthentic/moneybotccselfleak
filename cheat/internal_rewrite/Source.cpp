//this cheat was a mistake

#include <thread>
#include <iostream>

#include "hooks.hpp"
#include "console.hpp"
#include "input_system.hpp"
#include "mem.hpp"
#include "wipe.hpp"

HMODULE g_dll;
header_t g_header;

__declspec( safebuffers ) void main_thread( HINSTANCE uh ) {
#ifdef IFACE_DLLMAIN
	memcpy( &g_header, g_dll, sizeof( g_header ) );
#endif

	//Sleep( 1000 ); sleep is for the weak
	g_csgo.initialize( );

#ifdef HEADER_MODULE
	util::memset( (uint8_t*)uh, 0x90, 0x1000 );
#endif

	ExitThread( 0 );
}

__declspec( safebuffers ) int __stdcall DllMain( HINSTANCE inst, DWORD reason, LPVOID reserved ) {
    HANDLE thread;

	uintptr_t wanted_reason;
#ifdef _DEBUG
	wanted_reason = DLL_PROCESS_ATTACH;
#else
	wanted_reason = DLL_PROCESS_ATTACH;
#endif

	if( reason == DLL_PROCESS_ATTACH ) {
		g_con->create( );
		//g_con->print( "inst: %08x\n", inst );
		//g_con->print( "reserved: %08x\n", reserved );
	}

	if ( reason == wanted_reason 		
#ifdef IFACE_DLLMAIN
		&& !!reserved
#endif
		) {
#ifdef IFACE_DLLMAIN
		g_factory.init( ( uintptr_t )( reserved ) );
#endif
		g_dll = inst;

        //yayo
		//SetUnhandledExceptionFilter( exception_handler );

		DisableThreadLibraryCalls( inst );
#ifdef HEADER_MODULE
		antidebug::fuck_skids( );
#endif

        thread = CreateThread( nullptr, 0, 
			( LPTHREAD_START_ROUTINE )( main_thread ),
			inst, 0, nullptr );

        if( !thread )
            return 0;

        CloseHandle( thread );

        return 1;
	}
#ifdef IFACE_DLLMAIN
	else if( !reserved ) {
		MessageBoxA( nullptr, "interface data nullptr (loader error?)", "error", MB_OK );
		return 1;
	}
#endif
	
	if( reason == DLL_PROCESS_DETACH ) {
		g_csgo.m_panic = true;
		//SetUnhandledExceptionFilter( nullptr );	
	}

	return 0;
}