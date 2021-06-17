#pragma once

#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#pragma warning ( disable : 4018 )

namespace pattern
{
	inline bool bin_match( uint8_t* code, uint8_t* pattern, size_t size ) {
		for( size_t j = 0; j < size; j++ ) {
			if( pattern[ j ] && code[ j ] != pattern[ j ] ) {
				return false;
			}
		}
		return true;
	}

	template< typename t = uintptr_t > static t first_match( uintptr_t start, std::string sig, size_t len, std::ptrdiff_t offset = 0 ) {
		std::istringstream iss( sig );
		std::vector< std::string > tokens{ std::istream_iterator< std::string >{ iss }, std::istream_iterator< std::string >{} };
		std::vector< uint8_t > sig_bytes;

		for( const auto& hex_byte : tokens ) {
			sig_bytes.push_back( static_cast< uint8_t >( std::strtoul( hex_byte.c_str( ), nullptr, 16 ) ) );
		}

		if( sig_bytes.empty( ) || sig_bytes.size( ) < 2 ) {
			return t{ };
		}

		auto sig_data = sig_bytes.data( );
		auto sig_size = sig_bytes.size( );

		for( size_t i{ }; i < len; i++ ) {
			uint8_t* code_ptr = reinterpret_cast< uint8_t* >( start + i );

			if( bin_match( code_ptr, sig_data, sig_size ) ) {
				return( ( t )( start + i + offset ) );
			}
		}

		return t{ };
	}

	//ultimately the function you want to call to sigscan ( ida style )
	template< typename t = uintptr_t > static t first_code_match( HMODULE start, std::string sig, std::ptrdiff_t offset = 0 ) {
		auto dos_hdr = reinterpret_cast< PIMAGE_DOS_HEADER >( start );

		if( !dos_hdr ) return t{ };

		//DOS header, verifies if module is valid
		if( dos_hdr->e_magic != 0x5a4d ) {
			return t{ };
		}

		auto nt_hdrs = reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< uintptr_t >( dos_hdr ) + dos_hdr->e_lfanew );

		return first_match< t >( reinterpret_cast< uintptr_t >( dos_hdr ) + nt_hdrs->OptionalHeader.BaseOfCode, sig, nt_hdrs->OptionalHeader.SizeOfCode, offset );
	}
};