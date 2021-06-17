#pragma once
#include <inttypes.h>
#include <intrin.h>

template < typename Fn > __forceinline Fn call_vfunc( void* classbase, int index ) {
	if ( !classbase ) return Fn{ };
	return ( Fn )( ( uintptr_t* )*( uintptr_t** )classbase )[ index ];
}

template < typename T > __forceinline T get_ebp( ) {
	__asm mov eax, ebp
}