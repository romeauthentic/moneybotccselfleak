#pragma once
#include "UtlMem.h"

class c_attribute {
private:
	void* vtable;
public:
	uint16_t m_attribute_definition_index;
	float m_value;
private:
	uint32_t unk;

public:
	__forceinline c_attribute( unsigned short index, float value ) {
		m_attribute_definition_index = index;
		m_value = value;
	}
};

class c_attribute_list {
private:
	uint32_t unk;
public:
	CUtlVector< c_attribute, CUtlMemory< c_attribute > > m_attributes;

	__forceinline void add_attribute( int index, float value ) {
		if ( m_attributes.Count( ) >= 15 ) return; //setting this above 15 will crash
		if ( has_attrib( index ) ) return;

		c_attribute attribute( index, value );
		m_attributes.AddToTail( attribute );
	}

	__forceinline bool has_attrib( int index ) {
		for ( int i{ }; i < m_attributes.Count( ); i++ ) {
			if ( !m_attributes ) continue;
			if ( m_attributes[ i ].m_attribute_definition_index == index ) return true;
		}
		return false;
	}
};