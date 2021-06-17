#pragma once
#include <inttypes.h>
#include <string>
#include "strings.hpp"

//#define COMIC_SANS

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
	if ( !classbase ) return fn{ };
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

	for( size_t i{ }, j{ ( hex_len - 1 ) * 4 } ; i < hex_len; ++i, j -= 4 )
		rc[ i ] = hex_digits[ ( w >> j ) & 0x0f ];

	return rc;
}

extern void clip_trace_to_player( IClientEntity* player, const vec3_t& src, const vec3_t& end,
	unsigned mask, CTraceFilter* filter, CGameTrace* trace );

extern bool trace_ray( const vec3_t& start, const vec3_t& end, IClientEntity* a, IClientEntity* b );
extern bool is_low_fps( );
extern bool is_tick_valid( int tickcount );
extern void set_random_seed( int seed );
extern vec3_t get_spread_dir( float inaccuracy, float spread, vec3_t angles, int seed );
extern float get_random_float( float min, float max );
extern bool __vectorcall intersects_hitbox( vec3_t eye_pos, vec3_t end_pos, vec3_t min, vec3_t max, float radius );
extern bool hitchance( int target, const vec3_t& angles, int percentage );
extern bool hitchance( int target, const vec3_t& angle, int percentage, int hi );
extern float get_total_latency( );
extern float get_lerptime( );
extern int get_closest_player( );
extern vec2_t screen_transform( vec3_t world );
extern const char* definition_index_to_name( int index );
extern void disable_pvs( );
extern int hitbox_to_hitgroup( int hitbox );
extern std::string hitgroup_to_string( int hitgroup );

END_REGION