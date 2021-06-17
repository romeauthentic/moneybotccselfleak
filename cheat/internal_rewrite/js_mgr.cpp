#include "js_mgr.h"
#include "console.hpp"
#include <filesystem>

std::shared_ptr< js::c_js > g_js = std::make_shared< js::c_js >( );

js::c_js::~c_js( ) {
	if( m_js_ctx )
		duk_destroy_heap( m_js_ctx );
}

void js::c_js::init( ) {
	m_js_ctx = duk_create_heap( NULL, NULL, NULL, NULL, exception_handler );
}

void js::c_js::add_func( duk_c_function func, size_t args, const char* name ) {
	duk_push_c_function( m_js_ctx, func, args );
	duk_put_global_string( m_js_ctx, name );
}

void js::c_js::parse_string( const char* str ) {
	duk_peval_string_noresult( m_js_ctx, str );
}

void js::c_js::clear_files( ) {
	for( auto& it : m_files )
		if( it ) free( it );

	m_files.clear( );
}

void js::c_js::parse_files( ) {
	m_visual_funcs.clear( );
	m_fsn_funcs.clear( );
	m_cmove_funcs.clear( );

	m_parsed.clear( );
	clear_files( );

	static char str[ MAX_PATH ];

	GetModuleFileNameA( nullptr, str, MAX_PATH - 1 );
	PathRemoveFileSpecA( str );
	strcat_s< MAX_PATH >( str, xors( "/csgo/cfg/mb_scripts" ) );

	if( !std::filesystem::exists( str ) )
		std::filesystem::create_directory( str );

	//hack hack
	for( auto& entry : std::filesystem::directory_iterator( str ) ) {
		auto file = entry.path( ).string( );

		if( file.find( xors( ".js" ) ) == std::string::npos )
			continue;

		if( file.find( xors( "mb_scripts\\-" ) ) != std::string::npos )
			continue;

		if( parse_file( file.c_str( ) ) ) {
			auto str = entry.path( ).filename( ).string( );

			auto new_file = malloc( str.length( ) + 1 );
			memset( new_file, 0, str.length( ) + 1 );

			memcpy( new_file, str.data( ), str.length( ) );

			m_files.push_back( ( char* )( new_file ) );

			m_parsed.push_back( { ( char* )new_file, ( int )( m_parsed.size( ) + 1 ) } );
		}
	}
}

bool js::c_js::parse_file( const char* path ) {
	g_con->log( xors( "parsing file %s: " ), path );
	FILE* f = fopen( path, "r" );

	fseek( f, 0, SEEK_END );
	size_t size = ftell( f );

	if( !size )
		return false;

	rewind( f );
	char* str = ( char* )malloc( size );
	memset( str, 0, size );

	fread( str, size, 1, f );

	if( duk_peval_string( m_js_ctx, str ) != 0 || iserr( ) ) {
	    g_con->log( "%s", duk_safe_to_string( m_js_ctx, -1 ) );
		duk_pop( m_js_ctx );
		
		fclose( f );
		return false;
	}

	fclose( f );

	//g_con->log( "parsing done" );
	return true;
}

void js::c_js::exception_handler( void* udata, const char* msg ) {
	g_con->log( xors( "JS %s" ), msg ? msg : "undefined" );
	g_js->m_thrown = true;
}