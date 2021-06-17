#pragma once
#include <vector>

#include "Valve\dt_recv.h"
#include "util.hpp"
#include "fnv.hpp"

class RecvTable;

#define NETVAR( funcname, name, table, extra, type ) \
__forceinline auto& funcname( ) { \
	static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
	return get< type >( offset + extra ); \
}

#define PNETVAR( funcname, name, table, extra, type ) \
__forceinline type* funcname( ) { \
	static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
	return reinterpret_cast< type* >( uintptr_t( this ) + offset + extra ); \
}

#define OFFSET( funcname, offset, type ) \
__forceinline auto& funcname( ) { \
	return get< type >( offset ); \
}

NAMESPACE_REGION( factory )

class c_netvars {
public:
	void init( );
	//search through the netvar list
	std::ptrdiff_t get_netvar( hash_t table, hash_t hash ) const;
	RecvProp* get_prop( hash_t table, hash_t name ) const;
public:
	std::ptrdiff_t get_entry( hash_t hash, RecvTable* table ) const;
	RecvTable* get_table( hash_t hash ) const;
private:
	std::vector< RecvTable* > m_tables;
};

END_REGION

extern factory::c_netvars g_netvars;