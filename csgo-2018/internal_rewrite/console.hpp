#pragma once
#include <memory>
#include <Windows.h>
#include <vector>
#include <functional>

#include "con_fn.hpp"
#include "con_alias.hpp"
#include "input_system.hpp"
#include "math.hpp"

#pragma warning(disable: 4996)

class console {
public:
	console( ) {
		m_input = std::make_shared< util::c_input_manager >( );
	}

	static constexpr size_t TOP_HEIGHT = 22;
	static constexpr size_t WIDTH = 400;
	static constexpr size_t HEIGHT = 450;

	bool m_consuming_input{ };
	bool m_active{ };
	bool m_open{ };

	struct con_log {
		float m_time;
		char m_msg[ 128 ];
	};

	void draw_text( int x, int y, bool greyed, const char* msg, ... );
	void draw_text( int x, int y, const char* msg, ... );
	void draw_rect( int x, int y, int w, int h, clr_t col );
	void register_fn( con_fn_base* fn ) { m_functions.push_back( fn ); }

	void input( );
	void draw( );

	auto& get_input( ) {
		return m_input;
	}

	void log( const char* fmt, ... ) {
		va_list args;
		char buf[ 128 ];

		__crt_va_start( args, fmt );
		vsprintf_s< 128 >( buf, fmt, args );
		__crt_va_end( args );

		con_log log;
		log.m_time = ( float )GetTickCount( ) * 0.001f;
		memcpy( log.m_msg, buf, 128 );

		print( "%s\n", buf );

		m_logs.push_back( log );
	}

	void print( const char* fmt, ... ) {
		va_list args;
		va_start( args, fmt );
		vprintf_s( fmt, args );
		va_end( args );
	}

	void create( ) {
#ifdef _DEBUG
		AllocConsole( );
		freopen( xors( "CONOUT$" ), "w", stdout );
		const char* hack_names[ ] = {
			xors( "dunk hack" ),
			xors( "dweeb dumpster" ),
			xors( "nova killER" ),
			xors( "moms credit card" ),
			xors( "dad hook" ),
			xors( "retard remover" ),
			xors( "slam hook" ),
			xors( "swoosh" ),
			xors( "retard remover" ),
			xors( "shotgun hack" ),
		};

		SetConsoleTitleA( hack_names[ math::random_number( 0, 9 ) ] );
#endif
		game_console_print = reinterpret_cast< msg_t >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "Msg" ) );
	}

	void destroy( ) {
#ifdef _DEBUG
		FreeConsole( );
		fclose( stdout );
#endif
	}

	std::vector< con_log > m_logs;
	std::vector< con_fn_base* > m_functions;
	std::vector< std::shared_ptr< ISetting > > m_aliases;

	void register_alias( std::shared_ptr< ISetting >& alias );

	using msg_t = void( __cdecl* )( const char*, ... );
	msg_t game_console_print;

private:
	ISetting* find_var( hash_t name );
	ISetting* find_alias( hash_t name );

	std::string impl_alias_str( std::string str );

	con_fn_base* find_fn( hash_t name );
	void capture_command( );
	void execute_command( const char* cmd );

	int m_x{ 100 }, m_y{ 100 };

	char m_cur_cmd[ 200 ]{ };
	uint8_t m_key_states[ 256 ]{ };
	float m_last_key_input[ 256 ]{ };
	std::shared_ptr< util::c_input_manager > m_input;
};

extern std::shared_ptr< console > g_con;