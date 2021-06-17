#include "KeyValues.h"
#include "pattern.hpp"
#include "interfaces.h"

void* KeyValues::key_values_ptr;

void IKeyValuesSystem::FreeKeyValuesMemory( void* memory ) {
	return util::get_vfunc< 2, void >( this, memory );
}

int IKeyValuesSystem::GetSymbolForString( const char* str, bool create ) {
	static auto offset = pattern::first_code_match( GetModuleHandleA( xors( "client.dll" ) ), xors( "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6" ) );
	using fn = int( __thiscall* )( void*, const char*, bool );

	int symbol = ( ( fn )( offset ) )( this, str, create );
	return symbol;
}

const char* IKeyValuesSystem::GetStringForSymbol( int symbol ) {
	static auto offset = pattern::first_code_match( GetModuleHandleA( xors( "client.dll" ) ), xors( "55 8B EC 8B 55 08 83 FA FF" ) );
	using fn = const char*( __thiscall* )( void*, int );

	auto result = ( ( fn )( offset ) )( this, symbol );
	printf( "%d: %d\n", symbol, *( int* )( &result ) );

	return result;
}

//honestly keyvalues can suck my fucking cock while i force feeds that fucking whores ass rats to live in its cum infested shithole
KeyValues::KeyValues( const char* name ) {
	static std::string fun_pattern = "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6";
	static uintptr_t function_offset = pattern::first_code_match( cl.m_engine.dll( ), fun_pattern );
	static auto function = reinterpret_cast< void( __thiscall* )( void*, const char* ) >( function_offset );
	if( function ) function( this, name );
}

void KeyValues::init( const char* name ) {
	static std::string fun_pattern = "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6";
	static uintptr_t function_offset = pattern::first_code_match( cl.m_engine.dll( ), fun_pattern ) - 0x42;
	static auto function = reinterpret_cast< void( __thiscall* )( void*, const char* ) >( function_offset );
	if( function ) function( this, name );
}

bool KeyValues::LoadFromBuffer( const char* resource_name, const char* buffer, void* file_system, const char* path_id ) {
	static std::string fun_pattern = "55 8B EC 83 EC 38 53 8B 5D 0C";
	static auto function = pattern::first_code_match< bool( __thiscall* )( void*, char const*, const char*, void*, const char* ) >( cl.m_engine.dll( ), fun_pattern );
	if( function ) return function( this, resource_name, buffer, file_system, path_id );
	return false;
}

void* KeyValues::operator new( size_t alloc_size ) {
	if( !KeyValues::key_values_ptr ) {
		static auto get_key_values = reinterpret_cast< void*( __cdecl* )( ) >( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ), "KeyValuesSystem" ) );
		KeyValues::key_values_ptr = get_key_values( );
	}

	return call_vfunc< void*( __thiscall* )( void*, size_t ) >( KeyValues::key_values_ptr, 1 )( KeyValues::key_values_ptr, 32 );
}

void KeyValues::operator delete( void* mem ) {
	if( !KeyValues::key_values_ptr ) {
		static auto get_key_values = reinterpret_cast< void*( __cdecl* )( ) >( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ), "KeyValuesSystem" ) );
		KeyValues::key_values_ptr = get_key_values( );
	}

	call_vfunc< void( __thiscall* )( void*, void* ) >( KeyValues::key_values_ptr, 2 )( KeyValues::key_values_ptr, mem );
}
