#pragma once
#include "dt_common.h"
#include "dt_recv.h"
#include "VFunc.h"
#include "ClientClass.h"

enum frame_stages_t {
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,

	FRAME_RENDER_START,
	FRAME_RENDER_END
};

class chl_client {
public:
	client_class_t* GetAllClasses( ) {
		return call_vfunc< client_class_t*( __thiscall* )( void* ) >( this, 8 )( this );
	}

	void in_activatemouse( ) {
		call_vfunc< void( __thiscall* )( void* ) >( this, 15 )( this );
	}

	void in_deactivatemouse( ) {
		call_vfunc< void( __thiscall* )( void* ) >( this, 16 )( this );
	}
};