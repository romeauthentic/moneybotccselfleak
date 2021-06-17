//this cheat was a mistake

#include <thread>
#include <iostream>

#include "hooks.hpp"
#include "console.hpp"
#include "input_system.hpp"
#include "mem.hpp"

HMODULE g_dll;

void main_thread( HINSTANCE uh ) {
#ifdef IFACE_DLLMAIN
	util::memset( ( uint8_t* )uh, 0x90, 0x1000 );
#endif
	g_csgo.initialize( );

	while ( !g_csgo.m_panic ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	g_csgo.uninitialize( );
	ExitThread( 0 );
}

long __stdcall exception_handler( EXCEPTION_POINTERS* e ) {
	return EXCEPTION_CONTINUE_EXECUTION;
}

int __stdcall DllMain( HINSTANCE inst, DWORD reason, LPVOID reserved ) {
    HANDLE thread;

	uintptr_t wanted_reason;
#ifdef _DEBUG
	wanted_reason = DLL_PROCESS_ATTACH;
#else
	wanted_reason = DLL_PROCESS_ATTACH;
#endif

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