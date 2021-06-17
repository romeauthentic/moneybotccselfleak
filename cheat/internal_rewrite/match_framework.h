#pragma once
#pragma once
#include "util.hpp"
#include "KeyValues.hpp"

class CMatchSessionOnlineHost {
public:
	KeyValues* GetSessionSettings( ) {
		using fn = KeyValues* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 1 )( this );
	}
	void UpdateSessionSettings( KeyValues* packet ) {
		util::get_vfunc< 2, void >( this, packet );
	}
};

class CMatchFramework {
public:
	CMatchSessionOnlineHost* GetMatchSession( ) {
		using fn = CMatchSessionOnlineHost* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 13 )( this );
	}
};
