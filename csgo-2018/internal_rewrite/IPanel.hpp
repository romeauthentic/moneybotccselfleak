#pragma once
#include "util.hpp"

class IPanel {
public:
	const char* GetName( int panel ) {
		using fn = const char* ( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 36 )( this, panel );
	}
};