#pragma once

#include <memory>
#include <Windows.h>
#pragma warning(disable: 4996)

class console {
public:
	void create( ) {
		AllocConsole( );
		freopen( "CONOUT$", "w", stdout );
		SetConsoleTitleA( "tf2internal" );

		game_console_print = reinterpret_cast< decltype( game_console_print ) >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "Msg" ) );
	}

	void destroy( ) {
		FreeConsole( );
		fclose( stdout );
	}

	__forceinline void log_function( const char* msg ) {
		printf( "%s: %s", __FUNCTION__, msg );
	}

	using msg_t = void( __cdecl* )( const char*, ... );
	msg_t game_console_print;


	static console *con( ) {
		static console g_singleton_;
		return &g_singleton_;
	}
};