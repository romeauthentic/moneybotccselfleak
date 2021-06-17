#pragma once

class IPanel {
public:
	const char* GetName( int iPanel ) {
		return call_vfunc< const char*( __thiscall* )( void*, int ) >( this, 36 )( this, iPanel );
	}

	void SetMouseInputEnabled( unsigned int iPanel, bool state ) {
		return call_vfunc< void( __thiscall* )( void*, unsigned int, bool ) >( this, 32 )( this, iPanel, state );
	}

};