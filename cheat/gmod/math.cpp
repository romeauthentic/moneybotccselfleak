#include "math.hpp"
#include "interface.hpp"

namespace math
{
	uint32_t md5_pseudorandom( uint32_t seed ) {
		using fn = uint32_t( __thiscall * )( uint32_t seed );

		static auto func = pattern::first_code_match< uint32_t( __cdecl* )( uint32_t ) >( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 EC 68 6A 58 8D 45 98 6A 00 50 E8 ? ? ? ? 6A" ) );
		if( !func ) {
			return 0;
		}

		return func( seed );
	}
}