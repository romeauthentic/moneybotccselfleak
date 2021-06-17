#include "KeyValues.hpp"
#include "pattern.hpp"
#include "interface.hpp"

int IKeyValuesSystem::GetSymbolForString( const char* str, bool create ) {
	static auto offset = pattern::first_code_match( GetModuleHandleA( xors( "vstdlib.dll" ) ), xors( "55 8B EC 83 EC 0C 53 8B 5D 08 57" ) );
	using fn = int( __thiscall* )( void*, const char*, bool );

	int symbol = ( ( fn )( offset ) )( this, str, create );
	printf( "%s: %d\n", str, symbol );
	return symbol;
}

const char* IKeyValuesSystem::GetStringForSymbol( int symbol ) {
	static auto offset = pattern::first_code_match( GetModuleHandleA( xors( "vstdlib.dll" ) ), xors( "55 8B EC 8B 55 08 83 FA FF" ) );
	using fn = const char*( __thiscall* )( void*, int );

	auto result = ( ( fn )( offset ) )( this, symbol );
	printf( "%d: %d\n", symbol, *( int* )( &result ) );

	return result;
}

KeyValues::KeyValues( const char* setName ) {
	static bool inc{ };
	static auto key_values_fn = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "68 ? ? ? ? 8B C8 E8 ? ? ? ? 89 45 FC EB 07 C7 45 ? ? ? ? ? 8B 03 56" ) ) + 7;
	if ( !inc ) {
		key_values_fn += *reinterpret_cast< uintptr_t* >( key_values_fn + 1 ) + 5;
		inc = true;
	}

	reinterpret_cast< void( __thiscall* )( KeyValues*, const char* ) >( key_values_fn )( this, setName );
}

bool KeyValues::LoadFromBuffer( char const *resourceName, const char *pBuffer, void* pFileSystem, const char *pPathID, bool( *unknown )( const char* ) ) {
	static auto load_from_buffer = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04" ) );
	return reinterpret_cast< bool( __thiscall* )( KeyValues*, const char*, const char*, void*, const char*, bool( *)( const char* ) ) >( load_from_buffer )
		( this, resourceName, pBuffer, pFileSystem, pPathID, unknown );
}

KeyValues* KeyValues::FindKey( const char* name ) {
	auto key = g_gmod.m_keyvalues->GetSymbolForString( name, false );
	for( auto dat = m_pSub; !!dat; dat = dat->m_pPeer ) {
		if( dat->m_iKeyName == key ) {
			return dat;
		}
	}

	return nullptr;
}