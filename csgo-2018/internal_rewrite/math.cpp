#include <xmmintrin.h>  
#include "math.hpp"
#include "interface.hpp"

using _m128 = __m128;

namespace math
{
	uint32_t md5_pseudorandom( uint32_t seed ) {
		using fn = uint32_t( __thiscall * )( uint32_t seed );

		static auto func = pattern::first_code_match< fn >( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 70 6A 58" ) );
		if( !func ) {
			return 0;
		}

		return func( seed );
	}

	static const __declspec( align( 16 ) ) uint32_t g_simd_component_mask[ 4 ][ 4 ] = {
		{ INT_MAX, 0, 0, 0 },
		{ 0, INT_MAX, 0, 0 },
		{ 0, 0, INT_MAX, 0 },
		{ 0, 0, 0, INT_MAX }
	};

	void concat_transforms( const matrix3x4& in, const matrix3x4& in2, matrix3x4& out ) {
		//SSE concat transforms - turbo mode engaged

		_m128 last_mask = *( _m128* )( &g_simd_component_mask[ 3 ] );
		_m128 row_a0 = _mm_loadu_ps( in[ 0 ] );
		_m128 row_a1 = _mm_loadu_ps( in[ 1 ] );
		_m128 row_a2 = _mm_loadu_ps( in[ 2 ] );

		_m128 row_b0 = _mm_loadu_ps( in2[ 0 ] );
		_m128 row_b1 = _mm_loadu_ps( in2[ 1 ] );
		_m128 row_b2 = _mm_loadu_ps( in2[ 2 ] );

		_m128 a0 = _mm_shuffle_ps( row_a0, row_a0, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		_m128 a1 = _mm_shuffle_ps( row_a0, row_a0, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		_m128 a2 = _mm_shuffle_ps( row_a0, row_a0, _MM_SHUFFLE( 2, 2, 2, 2 ) );

		_m128 mul00 = _mm_mul_ps( a0, row_b0 );
		_m128 mul01 = _mm_mul_ps( a1, row_b1 );
		_m128 mul02 = _mm_mul_ps( a2, row_b2 );
		_m128 out0  = _mm_add_ps( mul00, _mm_add_ps( mul01, mul02 ) );

		a0 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		a1 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		a2 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 2, 2, 2, 2 ) );

		_m128 mul10 = _mm_mul_ps( a0, row_b0 );
		_m128 mul11 = _mm_mul_ps( a1, row_b1 );
		_m128 mul12 = _mm_mul_ps( a2, row_b2 );
		_m128 out1  = _mm_add_ps( mul10, _mm_add_ps( mul11, mul12 ) );

		a0 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 0, 0, 0, 0 ) );
		a1 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 1, 1, 1, 1 ) );
		a2 = _mm_shuffle_ps( row_a2, row_a2, _MM_SHUFFLE( 2, 2, 2, 2 ) );

		_m128 mul20 = _mm_mul_ps( a0, row_b0 );
		_m128 mul21 = _mm_mul_ps( a0, row_b1 );
		_m128 mul22 = _mm_mul_ps( a0, row_b2 );
		_m128 out2  = _mm_add_ps( mul20, _mm_add_ps( mul21, mul22 ) );

		a0 = _mm_and_ps( row_a0, last_mask );
		a1 = _mm_and_ps( row_a1, last_mask );
		a2 = _mm_and_ps( row_a2, last_mask );

		out0 = _mm_add_ps( out0, a0 );
		out1 = _mm_add_ps( out1, a1 );
		out2 = _mm_add_ps( out2, a2 );

		*( _m128* )( out[ 0 ] ) = out0;
		*( _m128* )( out[ 1 ] ) = out1;
		*( _m128* )( out[ 2 ] ) = out2;
	}

	void math::set_matrix_position( vec3_t pos, matrix3x4& matrix ) {
		for( size_t i{ }; i < 3; ++i ) {
			matrix[ i ][ 3 ] = pos[ i ];
		}
	}

	vec3_t math::get_matrix_position( const matrix3x4& src ) {
		return vec3_t( src[ 0 ][ 3 ], src[ 1 ][ 3 ], src[ 2 ][ 3 ] );
	}

	void angle_matrix( vec3_t angles, matrix3x4& matrix ) {
		float sr, sp, sy, cr, cp, cy;

		sp = sinf( angles.x * M_PIRAD );
		cp = cosf( angles.x * M_PIRAD );
		sy = sinf( angles.y * M_PIRAD );
		cy = cosf( angles.y * M_PIRAD );
		sr = sinf( angles.z * M_PIRAD );
		cr = cosf( angles.z * M_PIRAD );

		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 1 ][ 0 ] = cp * sy;
		matrix[ 2 ][ 0 ] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[ 0 ][ 1 ] = sp * srcy - crsy;
		matrix[ 1 ][ 1 ] = sp * srsy + crcy;
		matrix[ 2 ][ 1 ] = sr * cp;

		matrix[ 0 ][ 2 ] = ( sp*crcy + srsy );
		matrix[ 1 ][ 2 ] = ( sp*crsy - srcy );
		matrix[ 2 ][ 2 ] = cr * cp;

		matrix[ 0 ][ 3 ] = 0.f;
		matrix[ 1 ][ 3 ] = 0.f;
		matrix[ 2 ][ 3 ] = 0.f;
	}

	void angle_imatrix( vec3_t angles, matrix3x4& matrix ) {
		float sr, sp, sy, cr, cp, cy;

		sp = sinf( angles.x * M_PIRAD );
		cp = cosf( angles.x * M_PIRAD );
		sy = sinf( angles.y * M_PIRAD );
		cy = cosf( angles.y * M_PIRAD );
		sr = sinf( angles.z * M_PIRAD );
		cr = cosf( angles.z * M_PIRAD );

		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 0 ][ 1 ] = cp * sy;
		matrix[ 0 ][ 2 ] = -sp;
		matrix[ 1 ][ 0 ] = sr * sp*cy + cr * -sy;
		matrix[ 1 ][ 1 ] = sr * sp*sy + cr * cy;
		matrix[ 1 ][ 2 ] = sr * cp;
		matrix[ 2 ][ 0 ] = ( cr*sp*cy + -sr * -sy );
		matrix[ 2 ][ 1 ] = ( cr*sp*sy + -sr * cy );
		matrix[ 2 ][ 2 ] = cr * cp;
		matrix[ 0 ][ 3 ] = 0.f;
		matrix[ 1 ][ 3 ] = 0.f;
		matrix[ 2 ][ 3 ] = 0.f;
	}

	void angle_matrix( vec3_t angles, matrix3x4& matrix, vec3_t origin ) {
		angle_matrix( angles, matrix );
		set_matrix_position( origin, matrix );
	}

	vec3_t matrix_angles( const matrix3x4& matrix ) {
		//thx strackoverflow
		vec3_t angles;
		float forward[ 3 ];
		float left[ 3 ];
		float up[ 3 ];

		forward[ 0 ] = matrix[ 0 ][ 0 ];
		forward[ 1 ] = matrix[ 1 ][ 0 ];
		forward[ 2 ] = matrix[ 2 ][ 0 ];
		left[ 0 ] = matrix[ 0 ][ 1 ];
		left[ 1 ] = matrix[ 1 ][ 1 ];
		left[ 2 ] = matrix[ 2 ][ 1 ];
		up[ 2 ] = matrix[ 2 ][ 2 ];

		float xy_dist = sqrtf( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );

		if( xy_dist > 0.001f ) {
			angles.y = RAD2DEG( atan2f( forward[ 1 ], forward[ 0 ] ) );
			angles.x = RAD2DEG( atan2f( -forward[ 2 ], xy_dist ) );
			angles.z = RAD2DEG( atan2f( left[ 2 ], up[ 2 ] ) );
		}
		else {
			angles.y = RAD2DEG( atan2f( -left[ 0 ], left[ 1 ] ) );
			angles.x = RAD2DEG( atan2f( -forward[ 2 ], xy_dist ) );

			angles.z = 0;
		}

		return angles;
	}

	void rotate_matrix( vec3_t angles, vec3_t origin, float degrees, matrix3x4& matrix ) {
		angles.y += degrees;
		angles.clamp( );

		vec3_t rotated( 0, degrees, 0 );
		matrix3x4 rotated_matrix;
		angle_matrix( rotated, rotated_matrix );

		vec3_t delta = get_matrix_position( matrix ) - origin;
		vec3_t out = vector_transform( delta, rotated_matrix );

		matrix3x4 bone_rotation, matrix_out;
		memcpy( &bone_rotation, &matrix, sizeof( matrix3x4 ) );

		set_matrix_position( vec3_t( ), bone_rotation );
		concat_transforms( rotated_matrix, bone_rotation, matrix_out );
		auto angles_out = matrix_angles( matrix_out );
		angle_matrix( angles_out, matrix, out );
	}

	float approach_angle( float to, float from, float speed ) {
		float delta = std::remainderf( to - from, 360.f );

		if( delta > speed )
			from += speed;
		else if( delta < -speed )
			from -= speed;
		else from = to;

		return std::clamp( from, -180.f, 180.f );
	}

	float snap_yaw( float value ) {
		//this is actually what the code looks like in the game btw
		float sign = 1.0f;
		if( value < 0.0f ) {
			sign = -1.0f;
			value = -value;
		}

		if( value < 23.0f )
			value = 0.0f;
		else if( value < 67.0f )
			value = 45.0f;
		else if( value < 113.0f )
			value = 90.0f;
		else if( value < 157 )
			value = 135.0f;
		else
			value = 180.0f;

		return ( value * sign );
	}

	float __vectorcall dist_segment_to_segment( vec3_t s1, vec3_t s2, vec3_t k1, vec3_t k2 ) {
		vec3_t   u = s2 - s1;
		vec3_t   v = k2 - k1;
		vec3_t   w = s1 - k1;
		float    a = u.dot( u );
		float    b = u.dot( v );
		float    c = v.dot( v );
		float    d = u.dot( w );
		float    e = v.dot( w );
		float    D = a*c - b*b;
		float    sc, sN, sD = D;
		float    tc, tN, tD = D;

		if( D < SMALL_NUM ) {
			sN = 0.0f;
			sD = 1.0f;
			tN = e;
			tD = c;
		}
		else {
			sN = ( b*e - c*d );
			tN = ( a*e - b*d );
			if( sN < 0.0f ) {
				sN = 0.0f;
				tN = e;
				tD = c;
			}
			else if( sN > sD ) {
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if( tN < 0.0f ) {
			tN = 0.0;

			if( -d < 0.0f )
				sN = 0.0;
			else if( -d > a )
				sN = sD;
			else {
				sN = -d;
				sD = a;
			}
		}
		else if( tN > tD ) {
			tN = tD;

			if( ( -d + b ) < 0.0f )
				sN = 0;
			else if( ( -d + b ) > a )
				sN = sD;
			else {
				sN = ( -d + b );
				sD = a;
			}
		}

		sc = ( abs( sN ) < SMALL_NUM ? 0.0f : sN / sD );
		tc = ( abs( tN ) < SMALL_NUM ? 0.0f : tN / tD );

		vec3_t  dP = w + ( u * sc ) - ( v * tc );

		return dP.length();
	}
}