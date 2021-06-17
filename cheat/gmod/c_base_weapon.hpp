#pragma once
#include "util.hpp"
#include "IClientEntityList.hpp"
#include "netvars.hpp"
#include "pattern.hpp"



class c_base_weapon {
public:
	NETVAR( m_iClip1, "m_iClip1", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_flNextPrimaryAttack, "m_flNextPrimaryAttack", "DT_BaseCombatWeapon", 0, float );
	NETVAR( m_hOwner, "m_hOwner", "DT_BaseCombatWeapon", 0, uint32_t );
	NETVAR( m_nModelIndex, "m_nModelIndex", "DT_BaseEntity", 0, int );

	__forceinline IClientEntity* ce( ) {
		return reinterpret_cast< IClientEntity* >( this );
	}

	__forceinline operator IClientEntity*( ) {
		return reinterpret_cast< IClientEntity* >( this );
	}

	template < typename t >
	__forceinline t& get( std::ptrdiff_t offset ) {
		return *reinterpret_cast< t* >( uintptr_t( this ) + offset );
	}

	int get_max_clip1( ) {
		return util::get_vfunc< 355, int >( this );
	}
	
	const char* get_class_name( ) {
		return util::get_vfunc< 366, const char* >( this );
	}

	const char* get_print_name( ) {
		return util::get_vfunc< 367, const char* >( this );
	}

	void* get_wpn_data( );

	char get_hud_icon( );

	bool is_cw20( );
	bool is_fas2( );
	float get_custom_cone( );
};