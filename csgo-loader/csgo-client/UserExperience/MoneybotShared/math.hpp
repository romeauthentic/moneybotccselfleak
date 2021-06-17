#pragma once
#include <random>
#include "util.hpp"

static constexpr long double M_PI = 3.14159265358979323846f;
static constexpr long double M_RADPI = 57.295779513082f;
static constexpr long double M_PIRAD = 0.01745329251f;
static constexpr float M_PI_F = ( ( float )( M_PI ) );
__forceinline float RAD2DEG( float x ) { return( ( float )( x ) * ( float )( 180.f / M_PI_F ) ); }
__forceinline float DEG2RAD( float x ) { return( ( float )( x ) * ( float )( M_PI_F / 180.f ) ); }

namespace {
	//make a random generator and seed it with a p random number
	static std::random_device rd;
	static std::mt19937 gen{ rd() };
}

NAMESPACE_REGION( math )

#undef min
#undef max

template < typename t >
t min( const t& t1, const t& t2 ) {
	return t1 < t2 ? t1 : t2;
}

template < typename t, typename... ts_ >
t min( const t& t1, const t& t2, ts_&&... ts ) {
	return t1 < t2 ?
		min( t1, std::forward< ts_ >( ts )... ) :
		min( t2, std::forward< ts_ >( ts )... );
}

template < typename t >
t max( const t& t1, const t& t2 ) {
	return t1 > t2 ? t1 : t2;
}

template < typename t, typename... ts_ >
t max( const t& t1, const t& t2, ts_&&... ts ) {
	return t1 > t2 ?
		max( t1, std::forward< ts_ >( ts )... ) :
		max( t2, std::forward< ts_ >( ts )... );
}

// todo - dex; make 2 random generator funcs here, this one only works for floats normally

template < typename t > __forceinline t random_number( t min, t max ) {
	if constexpr( !std::is_integral_v< t > ) {
		std::uniform_real_distribution< t > dist( min, max );
		return dist( gen );
	}
	else {
		std::uniform_int_distribution< t > dist( min, max );
		return dist( gen );
	}
}

END_REGION