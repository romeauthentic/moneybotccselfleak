#pragma once
#include "util.hpp"
#include "vector.hpp"
#include "ClientClass.hpp"
#include "IVEngineClient.hpp"

class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientThinkable;
class IClientEntity;
class IClientAlphaProperty;
class bf_read;
using model_t = uintptr_t;


class c_base_player;

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
	virtual ClientClass* GetClientClass( ) = 0;
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
	void* GetClientRenderable( ) {
		return reinterpret_cast< void* >( this + 0x4 );
	}

	IClientNetworkable* GetClientNetworkable( ) {
		return reinterpret_cast< IClientNetworkable* >( this + 0x8 );
	}

	ICollideable* GetCollideable( ) {
		using fn =  ICollideable* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 2 )( this );
	}	

	vec3_t& GetRenderOrigin( ) {
		using fn = vec3_t& ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 10 )( this );
	}

	vec3_t& GetRenderAngles( ) {
		using fn = vec3_t& ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 11 )( this );
	}

	ClientClass* GetClientClass( ) {
		try {
			return GetClientNetworkable()->GetClientClass();
		}
		catch (...) {
			return 0;
		}
	}

	bool IsDormant( ) {
		return GetClientNetworkable( )->IsDormant( );
	}

	int GetIndex( ) {
		if( !this ) return 0;

		return GetClientNetworkable( )->EntIndex( );
	}

	model_t* GetModel( ) {
		using fn = model_t* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this->GetClientRenderable( ), 8 )( this->GetClientRenderable( ) );
	}

	int DrawModel( int flags, uint8_t alpha ) {
		using fn = int( __thiscall* )( void*, int, uint8_t );
		return util::get_vfunc< fn >( this->GetClientRenderable( ), 9 )( this->GetClientRenderable( ), flags, alpha );
	}

	bool SetupBones( matrix3x4* bone_to_world, int max_bones, int mask, float time ) {
		using fn = bool(__thiscall*)(void*, matrix3x4*, int, int, float);
		return util::get_vfunc< fn >(GetClientRenderable(), 13)
			(GetClientRenderable(), bone_to_world, max_bones, mask, time);
	}

	template < typename t >
	t* as( ) {
		return reinterpret_cast< t* >( this );
	}
};

class IClientEntityList
{
public:
	IClientNetworkable* GetClientNetworkable( int index ) {
		using fn = IClientNetworkable* ( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 1 )( this, index );
	}

	template < typename t = c_base_player >
	t* GetClientEntity( int index ) {
		using fn = t* ( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 3 )( this, index );
	}

	template < typename t = c_base_player >
	t* GetClientEntityFromHandle( uint32_t handle ) {
		using fn = t* ( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 4 )( this, handle );
	}

	int GetHighestEntityIndex( ) {
		using fn = int( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 6 )( this );
	}
};