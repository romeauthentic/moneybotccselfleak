#pragma once

#include "util.hpp"
#include <Windows.h>

#ifdef HEADER_MODULE
	#define EXPAND( x,y )x##y

	#define BEGIN___( MARKER_BEGIN )MARKER_BEGIN:

	#define DEL_END___( MARKER_BEGIN, MARKER_END )__asm { __asm PUSH MARKER_END __asm PUSH MARKER_BEGIN __asm CALL antidebug::erase } \
	MARKER_END: 

	#define BEGIN__( x, y ) BEGIN___( EXPAND2( MARKER_BEGIN_, x, y ) )
	#define END__( x, y ) DEL_END___( EXPAND2( MARKER_BEGIN_, x, y ), EXPAND2( MARKER_END_, x, y ) )

	#define DELETE_START( x ) BEGIN___( EXPAND( MARKER_BEGIN_, x ) )
	#define DELETE_END( x ) DEL_END___( EXPAND( MARKER_BEGIN_, x ), EXPAND( MARKER_END_, x ) )
#else
	#define DELETE_START( x )
	#define DELETE_END( x )
#endif

namespace antidebug
{
	static void __stdcall erase( uintptr_t start, uintptr_t end ) {
		uintptr_t size = end - start;

		DWORD protection_flag{ };
		VirtualProtect( ( void* )start, size, PAGE_EXECUTE_READWRITE, &protection_flag );

		for( unsigned int i = 0; i < size; i++ ) {
			int random = rand( ) % 0x90;
			memset( ( void* )( start + i ), random, 1 );
		}

		VirtualProtect( ( void* )start, size, protection_flag, &protection_flag );
	}

	static void fuck_skids( ) {
		DELETE_START( 0 );
		// stop most debuggers from working as breakpoint is patched to exit process call

		ulong_t old_protection = 0;

		uintptr_t exit_process = ( uintptr_t )GetProcAddress( GetModuleHandleA( xors( "kernel32.dll" ) ), xors( "ExitProcess" ) );
		uintptr_t dbg_ui_remote_breakin = ( uintptr_t )GetProcAddress( GetModuleHandleA( xors( "ntdll.dll" ) ), xors( "DbgUiRemoteBreakin" ) );
		uintptr_t dbg_break_point = ( uintptr_t )GetProcAddress( GetModuleHandleA( xors( "ntdll.dll" ) ), xors( "DbgBreakPoint" ) );

		// fuck DbgUiRemoteBreakin
		VirtualProtect( ( void* )dbg_ui_remote_breakin, 6, PAGE_EXECUTE_READWRITE, &old_protection );

		*( uint8_t* )( dbg_ui_remote_breakin ) = 0x68; // push
		*( uintptr_t* )( dbg_ui_remote_breakin + 1 ) = exit_process;
		*( uint8_t* )( dbg_ui_remote_breakin + 5 ) = 0xC3; // ret

		VirtualProtect( ( void* )dbg_ui_remote_breakin, 6, old_protection, &old_protection );

		// fuck DbgBreakPoint
		VirtualProtect( ( void* )dbg_break_point, 6, PAGE_EXECUTE_READWRITE, &old_protection );

		*( uint8_t* )( dbg_break_point ) = 0x68; // push
		*( uintptr_t* )( dbg_break_point + 1 ) = exit_process;
		*( uint8_t* )( dbg_break_point + 5 ) = 0xC3; // ret

		VirtualProtect( ( void* )dbg_break_point, 6, old_protection, &old_protection );

		DELETE_END( 0 );
	}
}