#pragma once
#include "VFunc.h"

class vec3_t;
class IVDebugOverlay {
public:
	int ScreenPosition( const vec3_t& point, vec3_t& screen ) {
		return call_vfunc< int( __thiscall* )( void*, const vec3_t&, vec3_t& ) >( this, 10 )( this, point, screen );
	}
};