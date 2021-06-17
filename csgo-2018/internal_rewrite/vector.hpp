#pragma once
#include <cmath>
#include <algorithm>


class vec2_t {
public:
	vec2_t( ) {
		x = y = 0.0f;
	}

	vec2_t( float X, float Y ) {
		x = X; y = Y;
	}

	vec2_t( float* v ) {
		x = v[ 0 ]; y = v[ 1 ];
	}

	vec2_t( const float* v ) {
		x = v[ 0 ]; y = v[ 1 ];
	}

	vec2_t( const vec2_t& v ) {
		x = v.x; y = v.y;
	}

	vec2_t& operator=( const vec2_t& v ) {
		x = v.x; y = v.y; return *this;
	}

	float& operator[]( int i ) {
		return ( ( float* )this )[ i ];
	}

	float operator[]( int i ) const {
		return ( ( float* )this )[ i ];
	}

	vec2_t& operator+=( const vec2_t& v ) {
		x += v.x; y += v.y; return *this;
	}

	vec2_t& operator-=( const vec2_t& v ) {
		x -= v.x; y -= v.y; return *this;
	}

	vec2_t& operator*=( const vec2_t& v ) {
		x *= v.x; y *= v.y; return *this;
	}

	vec2_t& operator/=( const vec2_t& v ) {
		x /= v.x; y /= v.y; return *this;
	}

	vec2_t& operator+=( float v ) {
		x += v; y += v; return *this;
	}

	vec2_t& operator-=( float v ) {
		x -= v; y -= v; return *this;
	}

	vec2_t& operator*=( float v ) {
		x *= v; y *= v; return *this;
	}

	vec2_t& operator/=( float v ) {
		x /= v; y /= v; return *this;
	}

	vec2_t operator+( const vec2_t& v ) const {
		return vec2_t( x + v.x, y + v.y );
	}

	vec2_t operator-( const vec2_t& v ) const {
		return vec2_t( x - v.x, y - v.y );
	}

	vec2_t operator*( const vec2_t& v ) const {
		return vec2_t( x * v.x, y * v.y );
	}

	vec2_t operator/( const vec2_t& v ) const {
		return vec2_t( x / v.x, y / v.y );
	}

	vec2_t operator+( float v ) const {
		return vec2_t( x + v, y + v );
	}

	vec2_t operator-( float v ) const {
		return vec2_t( x - v, y - v );
	}

	vec2_t operator*( float v ) const {
		return vec2_t( x * v, y * v );
	}

	vec2_t operator/( float v ) const {
		return vec2_t( x / v, y / v );
	}

	void set( float X = 0.0f, float Y = 0.0f ) {
		x = X; y = Y;
	}

	float length( void ) const {
		return sqrtf( x * x + y * y );
	}

	float lengthsqr( void ) const {
		return ( x * x + y * y );
	}

	float dist_to( const vec2_t& v ) const {
		return ( *this - v ).length( );
	}

	float dist_to_sqr( const vec2_t& v ) const {
		return ( *this - v ).lengthsqr( );
	}

	float dot( const vec2_t& v ) const {
		return ( x * v.x + y * v.y );
	}

	bool is_zero( void ) const {
		return ( x > -FLT_EPSILON && x < FLT_EPSILON &&
			y > -FLT_EPSILON && y < FLT_EPSILON );
	}

	operator bool( ) const noexcept {
		return !is_zero( );
	}

public:
	float x, y;
};

class vec3_t {
public:
	vec3_t( ) {
		x = y = z = 0.0f;
	}

	vec3_t( float X, float Y, float Z ) {
		x = X; y = Y; z = Z;
	}

	vec3_t( float* v ) {
		x = v[ 0 ]; y = v[ 1 ]; z = v[ 2 ];
	}

	vec3_t( const float* v ) {
		x = v[ 0 ]; y = v[ 1 ]; z = v[ 2 ];
	}

	vec3_t( const vec3_t& v ) {
		x = v.x; y = v.y; z = v.z;
	}

	vec3_t( const vec2_t& v ) {
		x = v.x; y = v.y; z = 0.0f;
	}

	__forceinline vec3_t& operator=( const vec3_t& v ) {
		x = v.x; y = v.y; z = v.z; return *this;
	}

	__forceinline vec3_t& operator=( const vec2_t& v ) {
		x = v.x; y = v.y; z = 0.0f; return *this;
	}

	float& operator[]( int i ) {
		return ( ( float* )this )[ i ];
	}

	__forceinline float operator[]( int i ) const {
		return ( ( float* )this )[ i ];
	}

	vec3_t& operator+=( const vec3_t& v ) {
		x += v.x; y += v.y; z += v.z; return *this;
	}

	vec3_t& operator-=( const vec3_t& v ) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	vec3_t& operator*=( const vec3_t& v ) {
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	vec3_t& operator/=( const vec3_t& v ) {
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	vec3_t& operator+=( float v ) {
		x += v; y += v; z += v; return *this;
	}

	vec3_t& operator-=( float v ) {
		x -= v; y -= v; z -= v; return *this;
	}

	vec3_t& operator*=( float v ) {
		x *= v; y *= v; z *= v; return *this;
	}

	vec3_t& operator/=( float v ) {
		x /= v; y /= v; z /= v; return *this;
	}

	__forceinline vec3_t operator+( const vec3_t& v ) const {
		return vec3_t( x + v.x, y + v.y, z + v.z );
	}

	__forceinline vec3_t operator-( const vec3_t& v ) const {
		return vec3_t( x - v.x, y - v.y, z - v.z );
	}

	__forceinline vec3_t operator*( const vec3_t& v ) const {
		return vec3_t( x * v.x, y * v.y, z * v.z );
	}

	__forceinline vec3_t operator/( const vec3_t& v ) const {
		return vec3_t( x / v.x, y / v.y, z / v.z );
	}

	__forceinline vec3_t operator+( float v ) const {
		return vec3_t( x + v, y + v, z + v );
	}

	__forceinline vec3_t operator-( float v ) const {
		return vec3_t( x - v, y - v, z - v );
	}

	__forceinline vec3_t operator*( float v ) const {
		return vec3_t( x * v, y * v, z * v );
	}

	__forceinline vec3_t operator/( float v ) const {
		return vec3_t( x / v, y / v, z / v );
	}

	__forceinline float length( ) const {
		return sqrtf( x * x + y * y + z * z );
	}

	__forceinline float lengthsqr( ) const {
		return ( x * x + y * y + z * z );
	}

	__forceinline float length2d( ) const {
		return sqrtf( x * x + y * y );
	}

	__forceinline float length2dsqr( ) const {
		return ( x * x + y * y );
	}

	__forceinline float dist_to( const vec3_t& v ) const {
		return ( *this - v ).length( );
	}

	__forceinline float dist_to_sqr( const vec3_t& v ) const {
		return ( *this - v ).lengthsqr( );
	}

	__forceinline float dot( const vec3_t& v ) const {
		return ( x * v.x + y * v.y + z * v.z );
	}

	__forceinline float fov_to( const vec3_t& to ) {	
		const float from_length = length( );
		const float to_length	= to.length( );

		if( from_length && to_length ) {
			return acos( dot( to ) / from_length * to_length );
		}

		return 0.f;
	}

	vec3_t cross( const vec3_t& v ) const {
		return vec3_t( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	}

	bool is_zero( ) const {
		return ( x > -FLT_EPSILON && x < FLT_EPSILON &&
			y > -FLT_EPSILON && y < FLT_EPSILON &&
			z > -FLT_EPSILON && z < FLT_EPSILON );
	}
	
	operator bool( ) const {
		return !is_zero( );
	}

	inline void normalize_vector( ) {
		vec3_t& v = *this;

		float iradius = 1.f / ( this->length( ) + FLT_EPSILON ); //FLT_EPSILON

		v.x *= iradius;
		v.y *= iradius;
		v.z *= iradius;
	}

	vec3_t abs( ) const {
		return vec3_t{ std::abs( x ), std::abs( y ), std::abs( z ) };
	}

	vec3_t clamp( ) {
		for ( size_t axis{ }; axis < 2; axis++ ) {
			auto &cur_axis = operator[]( axis );
			if ( !std::isfinite( cur_axis ) ) {
				cur_axis = 0.f;
			}
		}

		x = std::clamp( x, -89.f, 89.f );
		y = std::clamp( std::remainder( y, 360.f ), -180.f, 180.f );
		z = 0.f;
		return *this;
	}

public:
	float x, y, z;
};

__forceinline vec3_t operator*( float f, const vec3_t& v ) {
	return v * f;
}
