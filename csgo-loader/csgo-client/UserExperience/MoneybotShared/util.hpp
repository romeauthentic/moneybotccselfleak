#pragma once
#include <inttypes.h>
#include <string>
#include <memory>

#define xors( s ) s

#define NAMESPACE_REGION( x ) namespace x {
#define END_REGION }

extern int TIME_TO_TICKS( float dt );
extern float TICKS_TO_TIME( int tick );
extern float TICK_INTERVAL( );

//WEE WOO WEE WOO ITS THE DWORD POLICE
using ulong_t = unsigned long;
using uword_t = unsigned short;

class IClientEntity;
class CTraceFilter;
class CGameTrace;
class vec3_t;
class vec2_t;

NAMESPACE_REGION( util )

typedef std::unique_ptr< void, void( ) > unique_handle;

template < typename t >
struct reverse_iterable {
	reverse_iterable( t&& it ) :
		iterable( it ) { }

	t& iterable;
	inline auto begin( ) {
		return std::rbegin( iterable );
	}

	inline auto end( ) {
		return std::rend( iterable );
	}
};

template< typename t >
reverse_iterable< t >
reverse_iterator( t&& iter ) {
	return reverse_iterable< t >{ iter };
}

template < typename fn > __forceinline fn get_vfunc( void* classbase, int index ) {
	if( !classbase ) return fn{ };
	return ( fn )( *( uintptr_t** )classbase )[ index ];
}

template < size_t index, typename ret, class ... args_ >
__forceinline ret get_vfunc( void* thisptr, args_... args ) {
	using fn = ret( __thiscall* )( void*, args_... );

	auto fn_ptr = ( fn )( *( uintptr_t** )thisptr )[ index ];
	return fn_ptr( thisptr, args... );
}

__forceinline std::string unicode_to_ascii( const std::wstring& unicode ) {
	std::string ascii_str( unicode.begin( ), unicode.end( ) );
	return ascii_str;
}

__forceinline std::wstring ascii_to_unicode( const std::string& ascii ) {
	std::wstring unicode_str( ascii.begin( ), ascii.end( ) );
	return unicode_str;
}

template < typename integer >
__forceinline auto to_hex_str( const integer& w,
	size_t hex_len = sizeof( integer ) << 1 ) {
	constexpr char* hex_digits = xors( "0123456789abcdef" );
	std::string rc( hex_len, 0 );

	for( size_t i{ }, j{ ( hex_len - 1 ) * 4 }; i < hex_len; ++i, j -= 4 )
		rc[ i ] = hex_digits[ ( w >> j ) & 0x0f ];

	return rc;
}

END_REGION