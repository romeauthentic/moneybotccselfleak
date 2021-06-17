#include "c_base_weapon.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

void* c_base_weapon::get_wpn_data( ) {
	static auto func = pattern::first_code_match< void*( __thiscall* )( void* ) >( g_gmod.m_chl.dll( ), "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3" );

	if ( func )
		return func( this );

	return nullptr;
}

bool c_base_weapon::is_cw20( ) {
	if( !g_ctx.m_lua )
		return false;

	const auto lua = g_ctx.m_lua;

	this->ce( )->PushEntity( );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_ENTITY ) ) {
		lua->Pop( 2 );
		return false;
	}

	lua->GetField( -1, xors( "CW20Weapon" ) );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_BOOL ) ) {
		lua->Pop( 2 );
		return false;
	}

	bool is_cw20 = lua->GetBool( -1 );

	lua->Pop( 2 );

	return is_cw20;
}

bool c_base_weapon::is_fas2( ) {
	if( !g_ctx.m_lua )
		return false;

	const auto lua = g_ctx.m_lua;

	this->ce( )->PushEntity( );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_ENTITY ) ) {
		lua->Pop( 2 );
		return false;
	}

	lua->GetField( -1, xors( "IsFAS2Weapon" ) );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_BOOL ) ) {
		lua->Pop( 2 );
		return false;
	}

	bool is_fas2 = lua->GetBool( -1 );

	lua->Pop( 2 );

	return is_fas2;
}

float c_base_weapon::get_custom_cone( ) {
	if( !g_ctx.m_lua )
		return 0.f;

	const auto lua = g_ctx.m_lua;

	this->ce( )->PushEntity( ); // thank god for this function.
	if( !lua->IsType( -1, LUA_TYPE::TYPE_ENTITY ) ) {
		lua->Pop( 2 );
		return false;
	}

	lua->GetField( -1, xors( "CurCone" ) );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_NUMBER ) ) {
		lua->Pop( 2 );
		return false;
	}

	float cone = lua->GetNumber( -1 );

	lua->Pop( 2 );

	return cone;
}