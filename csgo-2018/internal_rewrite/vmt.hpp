#pragma once
#include <vector>
#include "util.hpp"

// todo - dex; rewrite this, VirtualQuery (except for custom codeptr / readptr) and VirtualProtect shouldnt be used 
//             and we need to copy rtti over too or stuff will break later on

//             correct me if im wrong, but the vtable isnt replaced, instead the original is edited during hooking, rtti should be intact and accessable by game
//             class could definetly do with a rewrite tho!

//             ^ you're corrct, but changing page rights and replacing ptrs direclty in rdata (or usually the heap, since thats where vmts go) is not a safe solution
//             copying table + aligning it to compensate for rtti is safer
//             vac loves scanning memory regions but it doesnt really do much outside of game servers (only loads some shit for checking DEP and stuff)

// trash

//llama is a fucking nigger
//true

namespace hooks
{
	class c_vmt {
		uintptr_t* m_table;
		uintptr_t* m_original;
		std::vector< uintptr_t > m_new;
	public:
		int count( ) {
			int vfunc_count{ };

			while ( m_original[ vfunc_count ] ) {
				vfunc_count++;
			};

			return vfunc_count;
		}

		c_vmt( void* table ) {
			if ( !table ) {
				return;
			}

			this->m_table = reinterpret_cast< uintptr_t* >( table );
			this->m_original = *reinterpret_cast< uintptr_t** >( this->m_table );

			for ( int i = -1; i < this->count( ); ++i ) {
				this->m_new.push_back( this->m_original[ i ] );
			}

			auto data = this->m_new.data( );
			*this->m_table = uintptr_t( &data[ 1 ] );
		}

		~c_vmt( ) { }

		template< typename T = uintptr_t > T get_function( int index ) {
			return( ( T )( this->m_new.at( index + 1 ) ) );
		}

		template< typename T = uintptr_t > T get_old_function( int index ) {
			return( ( T )( this->m_original[ index ] ) );
		}

		void hook( int index, uintptr_t new_func ) {
			this->m_new.at( index + 1 ) = new_func;
		}

		void unhook( int index ) {
			this->m_new.at( index + 1 ) = this->m_original[ index ];
		}

		void hook( int index, void* new_func ) {
			hook( index, reinterpret_cast< uintptr_t >( new_func ) );
		}

		void restore( ) const {
			*this->m_table = uintptr_t( m_original );
		}
	};
}