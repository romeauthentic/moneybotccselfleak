#pragma once
#include "util.hpp"

class IKeyValuesSystem {
public:
	void FreeKeyValuesMemory( void* memory );
	int GetSymbolForString( const char* string, bool create );
	const char* GetStringForSymbol( int symbol );
};

class KeyValues
{
public:
	KeyValues( const char *setName );
	~KeyValues( );

	bool LoadFromBuffer( char const *resourceName, const char *pBuffer, void* pFileSystem = nullptr, const char *pPathID = nullptr, bool( *unknown )( const char* ) = nullptr );

	const char* get_string( const char* name, const char* value );

	int m_iKeyName;

	char *m_sValue;
	wchar_t *m_wsValue;

	// we don't delete these
	union {
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[ 4 ];
	};

	char	   m_iDataType;
	char	   m_bHasEscapeSequences;
	char	   m_bEvaluateConditionals;

	KeyValues* FindKey( const char* key_name );

	KeyValues *m_pPeer;	// pointer to next key in list
	KeyValues *m_pSub;	// pointer to Start of a new sub key list
	KeyValues *m_pChain;// Search here if it's not in our list
};