#include "CBaseWeapon.h"
#include "interfaces.h"

int c_base_weapon::get_max_clip_1( ) {
	return call_vfunc< int( __thiscall* )( void* ) >( this, 318 )( this );
}

int c_base_weapon::get_max_clip_2( ) {
	return call_vfunc< int( __thiscall* )( void* ) >( this, 319 )( this );
}

int c_base_weapon::get_slot( ) {
	return call_vfunc< int( __thiscall* )( void* ) >( this, 327 )( this );
}

char* c_base_weapon::get_name( ) {
	return call_vfunc< char *( __thiscall* )( void* ) >( this, 329 )( this );
}

char* c_base_weapon::get_print_name( ) {
	return call_vfunc<  char *( __thiscall* )( void* ) >( this, 330 )( this );
}

vec3_t& c_base_weapon::get_bullet_spread( ) {
	return call_vfunc< vec3_t& ( __thiscall* )( void* ) >( this, 286 )( this );
}

float c_base_weapon::get_spread( ) {
	return call_vfunc< float( __thiscall* )( void* ) >( this, 455 )( this );
}

int c_base_weapon::get_weaponid( ) {
	return call_vfunc< int( __thiscall* )( void* ) >( this, 422 )( this );
}

bool c_base_weapon::can_fire( ) {
	auto cur_time = g_ctx.m_local->get_tick_base( ) * cl.m_globals->interval_per_tick;
	auto next_primary_attack = m_flNextPrimaryAttack( );
	return cur_time >= next_primary_attack;
}

bool c_base_weapon::is_non_aim( ) {
	if ( get_slot( ) >= 2 ) return true;
	//more checks to be added
	return false;
}

int& c_base_weapon::get_inspect_stage( ) {
	return m_nInspectStage( );
}

float& c_base_weapon::get_inspect_time( ) {
	return m_flInspectAnimTime( );
}

c_attribute_list* c_base_weapon::get_attribute_list( ) {
	return reinterpret_cast< c_attribute_list* >( this + 0xA8 );
}

float c_base_weapon::get_distance( ) {

	if( is_melee( ) )
		return 100.f;

	if( is_flame_thrower( ) )
		return 185.f;

	return 8012.f;
}
