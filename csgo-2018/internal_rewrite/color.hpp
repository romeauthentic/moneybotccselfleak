#pragma once
#include <cmath>
#include <cinttypes>

//this is a fucking mess

class fclr_t {
	float R, G, B, A;
public:
	fclr_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

	fclr_t( float r, float g, float b, float a ) : R( r ), G( g ), B( b ), A( a ) { }

	fclr_t( float r, float g, float b ) : R( r ), G( g ), B( b ), A( 255 ) { }

	float& r( ) { return R; }
	float& g( ) { return G; }
	float& b( ) { return B; }
	float& a( ) { return A; }

	fclr_t& operator =( fclr_t& c ) {
		R = c.r( );
		G = c.g( );
		B = c.b( );
		A = c.a( );
		return *this;
	}

	fclr_t operator+( const fclr_t& v ) const {
		return fclr_t( R + v.R, G + v.G, B + v.B, A + v.A );
	}

	explicit operator bool( ) const noexcept {
		return ( R > 0 || G > 0 || B > 0 || A > 0 );
	}

	bool operator==( fclr_t& c ) const {
		return ( R == c.r( ) && G == c.g( ) && B == c.b( ) );
	}
};

class clr_t {
	uint8_t R, G, B, A;
public:
	clr_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

	clr_t( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) : R( r ), G( g ), B( b ), A( a ) { }

	clr_t( uint8_t r, uint8_t g, uint8_t b ) : R( r ), G( g ), B( b ), A( 255 ) { }

	uint8_t& r( ) { return R; }
	uint8_t& g( ) { return G; }
	uint8_t& b( ) { return B; }
	uint8_t& a( ) { return A; }

	clr_t& operator =( clr_t& c ) {
		R = c.r( );
		G = c.g( );
		B = c.b( );
		A = c.a( );
		return *this;
	}

	clr_t operator+( const clr_t& v ) const {
		return clr_t( R + v.R, G + v.G, B + v.B, A + v.A );
	}

	clr_t operator*( float f ) {
		return clr_t( uint8_t( R * f ), uint8_t( G * f ), uint8_t( B * f ), A );
	}

	explicit operator bool( ) const noexcept {
		return ( R > 0 || G > 0 || B > 0 || A > 0 );
	}

	float brightness( ) {
		typedef struct {
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f ) {
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f ) {
			out.s = delta / max;
		}
		else {
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.v;
	}

	float saturation( ) {
		typedef struct {
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>( R < G ? R : G );
		min = static_cast<float>( min < B ? min : B );

		float max = static_cast<float>( R > G ? R : G );
		max = static_cast<float>( max > B ? max : B );

		out.v = max;
		float delta = max - min;
		if ( delta < 0.0010f ) {
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f ) {
			out.s = delta / max;
		}
		else {
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast<float>( G - B ) / delta;
		else if ( G >= max )
			out.h = 2.0f + static_cast<float>( B - R ) / delta;
		else
			out.h = 4.0f + static_cast<float>( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.s;
	}

	static clr_t from_hsb( float hue, float saturation, float brightness ) {
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - ( int )h;
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if ( h < 1 ) {
			return clr_t(
				( unsigned char )( brightness * 255 ),
				( unsigned char )( t * 255 ),
				( unsigned char )( p * 255 )
			);
		}
		else if ( h < 2 ) {
			return clr_t(
				( unsigned char )( q * 255 ),
				( unsigned char )( brightness * 255 ),
				( unsigned char )( p * 255 )
			);
		}
		else if ( h < 3 ) {
			return clr_t(
				( unsigned char )( p * 255 ),
				( unsigned char )( brightness * 255 ),
				( unsigned char )( t * 255 )
			);
		}
		else if ( h < 4 ) {
			return clr_t(
				( unsigned char )( p * 255 ),
				( unsigned char )( q * 255 ),
				( unsigned char )( brightness * 255 )
			);
		}
		else if ( h < 5 ) {
			return clr_t(
				( unsigned char )( t * 255 ),
				( unsigned char )( p * 255 ),
				( unsigned char )( brightness * 255 )
			);
		}
		else {
			return clr_t(
				( unsigned char )( brightness * 255 ),
				( unsigned char )( p * 255 ),
				( unsigned char )( q * 255 )
			);
		}
	}

	static clr_t blend( clr_t first, clr_t second, float t ) {
		return clr_t(
			first.r( ) + static_cast< int >( t * ( second.r( ) - first.r( ) ) ),
			first.g( ) + static_cast< int >( t * ( second.g( ) - first.g( ) ) ),
			first.b( ) + static_cast< int >( t * ( second.b( ) - first.b( ) ) ),
			first.a( ) + static_cast< int >( t * ( second.a( ) - first.a( ) ) )
		);
	}

	float hue( ) {
		typedef struct {
			float h, s, v;
		} hsv;
		hsv         out;
		float       min, max, delta;

		min = static_cast< float >( R < G ? R : G );
		min = static_cast< float >( min < B ? min : B );

		max = static_cast< float >( R > G ? R : G );
		max = static_cast< float >( max > B ? max : B );

		out.v = max;
		delta = max - min;
		if ( delta < 0.0010f ) {
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if ( max > 0.0f ) {
			out.s = ( delta / max );
		}
		else {
			out.s = 0.0f;
			out.h = ( float )NAN;
			return out.h;
		}
		if ( R >= max )
			out.h = static_cast< float >( G - B ) / delta;
		else
			if ( G >= max )
				out.h = 2.0f + static_cast< float >( B - R ) / delta;
			else
				out.h = 4.0f + static_cast< float >( R - G ) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if ( out.h < 0.0f )
			out.h += 360.0f;

		return out.h;
	}

	fclr_t to_fclr( ) {
		return fclr_t{ R / 255.f, G / 255.f, B / 255.f, A / 255.f };
	}

	operator fclr_t( ) {
		return this->to_fclr( );
	}

	bool operator==( clr_t& c ) const {
		return ( R == c.r( ) && G == c.g( ) && B == c.b( ) );
	}
};