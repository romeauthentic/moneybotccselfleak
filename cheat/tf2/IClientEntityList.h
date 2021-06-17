#pragma once
#include "VFunc.h"

class IClientEntity;
class c_base_player;
using model_t = uintptr_t;
class client_ent_list {
public:
	template< typename T = IClientEntity > T* get_client_entity( int index ) {
		return call_vfunc< T* ( __thiscall* )( void*, int ) >( this, 3 )( this, index );
	}

	template< typename T = IClientEntity > T* get_entity_from_handle( uintptr_t handle ) {
		return call_vfunc< T*( __thiscall* )( void*, uintptr_t ) >( this, 4 )( this, handle );
	}

	int get_highest_entity_index( ) {
		return call_vfunc< int( __thiscall* )( void* ) >( this, 6 )( this );
	}

	template< typename T = c_base_player > __forceinline T* get_local_player( ) {
		return get_client_entity< T >( cl.m_engine( )->GetLocalPlayer( ) );
	}
};