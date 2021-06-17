#pragma once
#include <intrin.h>
#include "util.hpp"

#define get_baseptr( ) ( ( uintptr_t )( _AddressOfReturnAddress( ) ) - sizeof( uintptr_t ) )

class stack_t {
	uintptr_t m_ptr;

public:
	__forceinline stack_t( ) : m_ptr( get_baseptr( ) ) { }

	__forceinline stack_t( uintptr_t ptr ) : m_ptr( ptr ) { }

	template < typename t = uintptr_t >
	__forceinline t get( ) {
		return ( t )m_ptr;
	}

	template < typename t = uintptr_t >
	__forceinline t return_address( ) {
		return *( t* )( m_ptr + sizeof( void* ) );
	}

	template < typename t = uintptr_t >
	__forceinline t address_of_return_address( ) {
		return ( t )( m_ptr + sizeof( uintptr_t ) );
	}

	__forceinline stack_t& next( ) {
		return *( stack_t* )( m_ptr );
	}

	template < typename t = uintptr_t >
	__forceinline t local( size_t at ) {
		return ( t )( m_ptr - at );
	}

	template < typename t = uintptr_t >
	__forceinline t arg( size_t at ) {
		return ( t )( m_ptr + at );
	}
};

namespace util {
	template < typename t = uint8_t >
	__forceinline t* memcpy( t* dst, t* src, size_t size = sizeof( t ) ) {
		__movsb( 
			( uint8_t* )dst,
			( uint8_t* )src, 
			size 
		);

		return dst;
	}

	__forceinline void* memset( uint8_t* dst, uint8_t val, size_t size ) {
		__stosb(
			dst,
			val,
			size
		);

		return dst;
	}
}