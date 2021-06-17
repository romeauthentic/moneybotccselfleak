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
	g_gmod.initialize( );

	while ( !g_gmod.m_panic ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	g_gmod.uninitialize( );
	ExitThread( 0 );
}

long __stdcall exception_handler( EXCEPTION_POINTERS* e ) {
	const char* title = xors( "you crashed, retard" );
	std::string msg = xors( "unknown" );

	ulong_t e_code = e->ExceptionRecord->ExceptionCode;
	ulong_t e_info_0 = e->ExceptionRecord->ExceptionInformation[ 0 ];
	ulong_t e_info_1 = e->ExceptionRecord->ExceptionInformation[ 1 ];
	ulong_t e_info_2 = e->ExceptionRecord->ExceptionInformation[ 2 ];

	switch( e_code ) {
	case EXCEPTION_ACCESS_VIOLATION:
		if( !e_info_0 )
			msg = xors( "read access violation at " );
		else if( e_info_0 == 1 )
			msg = xors( "write access violation at " );
		else
			msg = xors( "access violation at " );

		msg += util::to_hex_str( e_info_1 );
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		msg = xors( "array bounds exceeded" );
		break;

	case EXCEPTION_BREAKPOINT:
		msg = xors( "triggered breakpoint" );
		break;

	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_UNDERFLOW:
	case EXCEPTION_FLT_STACK_CHECK:
		msg = xors( "invalid floating point operation" );
		break;

	case EXCEPTION_IN_PAGE_ERROR:
		if( !e_info_0 )
			msg = xors( "page error - read access violation at " );
		else if( e_info_0 == 1 )
			msg = xors( "page error - write access violation at " );
		else
			msg = xors( "page error - access violation at " );

		msg += util::to_hex_str( e_info_1 );
		msg += '\n';
		msg += xors( "NTSTATUS: " );
		msg += util::to_hex_str( e_info_2 );
		break;

	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_OVERFLOW:
		msg = xors( "invalid integer operation" );
		break;

	case EXCEPTION_STACK_OVERFLOW:
		msg = xors( "stack overflow" );
		break;

	default:
		msg = xors( "exception code: " );
		msg += util::to_hex_str( e_code );
		msg += xors( "\nexception variables: \n" );
		msg += util::to_hex_str( e_info_0 ); msg += '\n';
		msg += util::to_hex_str( e_info_1 ); msg += '\n';
		msg += util::to_hex_str( e_info_2 ); msg += '\n';
		break;
	}

	msg += xors( "\nregister dump: \n" );
	msg += xors( "eax: " ); msg += util::to_hex_str( e->ContextRecord->Eax ); msg += '\n';
	msg += xors( "esi: " ); msg += util::to_hex_str( e->ContextRecord->Esi ); msg += '\n';
	msg += xors( "ebx: " ); msg += util::to_hex_str( e->ContextRecord->Ebx ); msg += '\n';
	msg += xors( "edi: " ); msg += util::to_hex_str( e->ContextRecord->Edi ); msg += '\n';
	msg += xors( "ecx: " ); msg += util::to_hex_str( e->ContextRecord->Ecx ); msg += '\n';
	msg += xors( "ebp: " ); msg += util::to_hex_str( e->ContextRecord->Ebp ); msg += '\n';
	msg += xors( "edx: " ); msg += util::to_hex_str( e->ContextRecord->Edx ); msg += '\n';
	msg += xors( "esp: " ); msg += util::to_hex_str( e->ContextRecord->Esp ); msg += '\n';

	if( e->ExceptionRecord->ExceptionAddress ) {
		msg += xors( "\nin module: " );
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery( ( void* )e->ExceptionRecord->ExceptionAddress, &info, sizeof( MEMORY_BASIC_INFORMATION ) );

		char module_buf[ 32 ];
		K32GetModuleBaseNameA( GetCurrentProcess( ), ( HMODULE )info.AllocationBase, module_buf, 32 );
		msg += std::string( module_buf );
		msg += " ( ";
		msg += util::to_hex_str( ( uintptr_t )info.AllocationBase );
		msg += " ) ";

		msg += xors( "at: " );
		msg += util::to_hex_str( ( uintptr_t )e->ExceptionRecord->ExceptionAddress );
	}

	auto file = CreateFileA( xors( "./moneybot_dump.dmp" ), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
		nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );

	if( file ) {
		WriteFile( file, msg.data( ), msg.size( ), nullptr, nullptr );
		CloseHandle( file );
	}

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
		g_gmod.m_panic = true;
		//SetUnhandledExceptionFilter( nullptr );
	}

	return 0;
}