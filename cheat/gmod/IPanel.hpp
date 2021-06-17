#pragma once
#include "util.hpp"

class IPanel {
public:
	const char* GetName( int panel ) {
		using fn = const char* ( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 36 )( this, panel );
	}

	void SetMouseInputEnabled( unsigned int panel, bool enable ) {
		return util::get_vfunc< 32, void >( this, panel, enable );
	}
};