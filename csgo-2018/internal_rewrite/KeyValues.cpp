#include "KeyValues.hpp"
#include "pattern.hpp"
#include "interface.hpp"

void IKeyValuesSystem::FreeKeyValuesMemory( void* memory ) {
	return util::get_vfunc< 2, void >( this, memory );
}

int IKeyValuesSystem::GetSymbolForString( const char* str, bool create ) {
	static auto offset = pattern::first_code_match( GetModuleHandleA( xors( "vstdlib.dll" ) ), xors( "55 8B EC 83 EC 0C 53 8B 5D 08 57" ) );
	using fn = int( __thiscall* )( void*, const char*, bool );

	int symbol = ( ( fn )( offset ) )( this, str, create );
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
	static void( __thiscall* keyvalues_constructor )( KeyValues*, const char* ) = 0;

	if( !keyvalues_constructor ) {
		auto address = pattern::first_code_match( g_csgo.m_chl.dll( ),
			xors( "68 ? ? ? ? 8B C8 E8 ? ? ? ? 89 45 FC EB 07 C7 45 ? ? ? ? ? 8B 03 56" ),
			0x8 );

		keyvalues_constructor = decltype( keyvalues_constructor )( address + *( uintptr_t* )( address ) + 4 );
	}

	keyvalues_constructor( this, setName );
}

KeyValues::~KeyValues( ) {
	static bool( __thiscall* keyvalues_remove_everything )( KeyValues* ) = 0;

	if( !keyvalues_remove_everything ) {
		auto address = pattern::first_code_match( g_csgo.m_chl.dll( ),
			xors( "E8 ? ? ? ? 81 23 ? ? ? ?" ),
			0x1 );

		keyvalues_remove_everything = decltype( keyvalues_remove_everything )( address + *( uintptr_t* )( address ) + 4 );
	}

	keyvalues_remove_everything( this );
	g_csgo.m_keyvalues->FreeKeyValuesMemory( this );
	
}


bool KeyValues::LoadFromBuffer( char const *resourceName, const char *pBuffer, void* pFileSystem, const char *pPathID, bool( *unknown )( const char* ) ) {
	static auto load_from_buffer = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04" ) );
	return reinterpret_cast< bool( __thiscall* )( KeyValues*, const char*, const char*, void*, const char*, bool( *)( const char* ) ) >( load_from_buffer )
		( this, resourceName, pBuffer, pFileSystem, pPathID, unknown );
}

const char* KeyValues::get_string( const char* name, const char* value ) {
	static auto offset = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 C0 81 EC ?? ?? ?? ?? 53 8B 5D 08" ) );
	using fn = const char*( __thiscall* )( void*, const char*, const char* );

	auto result = ( ( fn )( offset ) )( this, name, value );

	return result;

}

KeyValues* KeyValues::FindKey( const char* name ) {
	auto key = g_csgo.m_keyvalues->GetSymbolForString( name, false );
	for( auto dat = m_pSub; !!dat; dat = dat->m_pPeer ) {
		if( dat->m_iKeyName == key ) {
			return dat;
		}
	}

	return nullptr;
}