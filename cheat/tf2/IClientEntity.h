#pragma once
#include "vector.hpp"
#include "VFunc.h"
#include "dt_common.h"
#include "dt_recv.h"
#include "search.h"
#include "util.hpp"
#include "ClientClass.h"

using matrix3x4 = float[ 3 ][ 4 ];
class bf_read;

class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientThinkable;
class IClientEntity;
class IClientAlphaProperty;

class IClientUnknown {
public:
	virtual ICollideable* GetCollideable( ) = 0;
	virtual IClientNetworkable* GetClientNetworkable( ) = 0;
	virtual IClientRenderable* GetClientRenderable( ) = 0;
	virtual IClientEntity* GetIClientEntity( ) = 0;
	virtual IClientEntity* GetBaseEntity( ) = 0;
	virtual IClientThinkable* GetClientThinkable( ) = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty( ) = 0;
};

class IClientNetworkable {
public:
	virtual IClientUnknown* GetIClientUnknown( ) = 0;
	virtual void Release( ) = 0;
	virtual client_class_t* GetClientClass( ) = 0;
	virtual void NotifyShouldTransmit( int state ) = 0;
	virtual void OnPreDataChanged( int updateType ) = 0;
	virtual void OnDataChanged( int updateType ) = 0;
	virtual void PreDataUpdate( int updateType ) = 0;
	virtual void PostDataUpdate( int updateType ) = 0;
	virtual void OnDataUnchangedInPVS( void ) = 0;
	virtual bool IsDormant( void ) = 0;
	virtual int EntIndex( void ) const = 0;
	virtual void ReceiveMessage( int classID, bf_read& msg ) = 0;
	virtual void* GetDataTableBasePtr( ) = 0;
	virtual void SetDestroyedOnRecreateEntities( void ) = 0;
};

class IClientEntity {
public:
	void* get_client_renderable( ) {
		return reinterpret_cast< void* >( this + 0x4 );
	}

	IClientNetworkable* get_client_networkable( ) {
		return reinterpret_cast< IClientNetworkable* >( this + 0x8 );
	}

public:
	uintptr_t get_ehandle( ) {
		return call_vfunc< uintptr_t( __thiscall* )( void* ) >( this, 1 )( this );
	}

	bool setup_bones( matrix3x4* bonetoworld, int maxbones, long bonemask, float curtime ) {
		return call_vfunc< bool( __thiscall* )( void*, matrix3x4*, int, long, float ) >( get_client_renderable( ), 16 )( get_client_renderable( ), bonetoworld, maxbones, bonemask, curtime );
	}

	int DrawModel( int flags, uint8_t alpha ) {
		using fn = int( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this->get_client_renderable( ), 10 )( this->get_client_renderable( ), flags );
	}

	vec3_t& get_render_origin( ) {
		using fn = vec3_t & ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 10 )( this );
	}

	vec3_t& get_abs_origin( ) {
		return call_vfunc< vec3_t&( __thiscall* )( void* ) >( this, 9 )( this );
	}

	vec3_t& get_abs_angles( ) {
		return call_vfunc< vec3_t&( __thiscall* )( void* ) >( this, 10 )( this );
	}

	void* get_model( ) {
		return call_vfunc< void*( __thiscall* )( void* ) >( get_client_renderable( ), 9 )( get_client_renderable( ) );
	}

	client_class_t* get_client_class( ) {
		return call_vfunc< client_class_t*( __thiscall* )( void* ) >( get_client_networkable( ), 2 )( get_client_networkable( ) );
	}

	bool is_dormant( ) {
		return call_vfunc< bool( __thiscall * )( void * ) >( get_client_networkable( ), 8 )( get_client_networkable( ) );
	}

	int index( ) {
		return call_vfunc< int( __thiscall* )( void* ) >( get_client_networkable( ), 9 )( get_client_networkable( ) );
	}

	void update_glow_effect( ) {
		//static auto fn = pattern::first_code_match< void( __thiscall* )( void* ) >( GetModuleHandleA( "client.dll" ), "8B 06 8D 4D F4 57 51 8D 4D F8", -0x2c );
		//if ( fn ) fn( this );
		return call_vfunc< void( __thiscall* )( void* ) >( this, 226 )( this );
	}

	void destroy_glow_effect( ) {
		//static auto fn_offset = pattern::first_code_match< intptr_t >( GetModuleHandleA( "client.dll" ), "E8 ? ? ? ? 8B 9F ? ? ? ? 85 DB 74 3D 8B 33" );
		//static auto fn = reinterpret_cast< void( __thiscall* )( void* ) >( fn_offset + 5 + *( intptr_t* )( fn_offset + 1 ) );
		//if ( fn ) fn( this );
		return call_vfunc< void( __thiscall* )( void* ) >( this, 227 )( this );
	}

	client_class_t* GetClientClass( ) {
		using fn = client_class_t* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( get_client_networkable( ), 2 )( this );
		/*
		try {
			return get_client_networkable( )->GetClientClass( );
		}
		catch( ... ) {
			return 0;
		}*/
	}

public:
	template< typename T = uintptr_t >
	__forceinline T& get( std::ptrdiff_t offset = 0 ) {
		return( *reinterpret_cast< T* >( ( uintptr_t )this + offset ) );
	}

	template< std::ptrdiff_t offset, typename T = uintptr_t >
	__forceinline T& get( ) {
		return( *reinterpret_cast< T* >( ( uintptr_t )this + offset ) );
	}

	template< typename T = uintptr_t >
	__forceinline T at( std::ptrdiff_t offset = 0 ) {
		return( reinterpret_cast< T >( uintptr_t( this ) + offset ) );
	}

	template< std::ptrdiff_t offset, typename T = uintptr_t >
	__forceinline T at( ) {
		return( reinterpret_cast< T >( uintptr_t( this ) + offset ) );
	}

	template < typename t >
	t* as( ) {
		return reinterpret_cast< t* >( this );
	}

};