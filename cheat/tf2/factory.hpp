#pragma once

#include <Windows.h>
#include <winternl.h>
#include <inttypes.h>
#include <stdio.h>
#include <Psapi.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <functional>

namespace tf2
{
	namespace interfaces
	{
		// this interface linked list iteration code is tf2 specific !
		// in the future i will write it game/engine version independent
		// using a disassembly engine

		struct regs {
			void *( *m_CreateFn )( );
			const char *m_pName;
			regs *m_pNext;
		};

		static inline bool is_create_fn( uintptr_t m_createfn ) {
			return( *( uint8_t * )( m_createfn ) == 0xB8
				&& *( uint8_t * )( m_createfn + 5 ) == 0xC3 );
		}

		static inline bool is_createinterface_fn( uintptr_t m_createfn ) {
			return( *( uint8_t * )( m_createfn + 4 ) == 0xE9
				&& *( uint8_t * )( m_createfn + 9 ) == 0xCC );
		}

		static inline uintptr_t follow_createinterface_export( uintptr_t export_ ) {
			if ( *( uint8_t * )( export_ + 4 ) != 0xE9 ) return 0;

			return( export_ + 4 + *( uintptr_t * )( export_ + 5 ) + 5 );
		}

		static inline regs *reg_from_createinterface( uintptr_t ci ) {
			if ( *( short * )( ci + 4 ) != 0x358B ) return nullptr;

			return( **( regs *** )( ci + 6 ) );
		}

		static inline uintptr_t interface_from_reg( regs *reg ) {
			uintptr_t reg_func = *( uintptr_t * )reg;

			if ( is_create_fn( reg_func ) ) {
				return( *( uintptr_t * )( reg_func + 1 ) );
			}

			return( 0 );
		}


		class interface_manager {
		public:
			struct interface_data {
			public:
				std::string name;
				std::string module;
				int version;
				void *ptr;

				template< typename T > inline T get( ) { return reinterpret_cast< T >( ptr ); }

				interface_data( std::string name_T, std::string module_T, void *ptr_T, int version_T = 0 )
					: name( name_T ), module( module_T ), ptr( ptr_T ), version( version_T ) {}
			};

			interface_manager() {
				auto root = &get_peb( )->Ldr->InMemoryOrderModuleList;

				for ( auto entry = root->Flink->Flink->Flink->Flink; entry != root; entry = entry->Flink ) {
					auto data_table = reinterpret_cast< PLDR_DATA_TABLE_ENTRY >( entry );
					auto module_base = reinterpret_cast< HMODULE >( data_table->Reserved2[ 0 ] );
					auto export_ = reinterpret_cast< uintptr_t >( ::GetProcAddress( module_base, "CreateInterface" ) );

					if ( !export_ || !interfaces::is_createinterface_fn( export_ ) ) {
						continue;
					}

					auto CreateInterface = interfaces::follow_createinterface_export( export_ );

					if ( !CreateInterface ) {
						continue;
					}

					auto regs = interfaces::reg_from_createinterface( CreateInterface );

					while ( regs ) {
						std::string name( regs->m_pName ), temp_version( name );

						name.erase( std::remove_if( name.begin( ), name.end( ), &isdigit ), name.end( ) );

						temp_version.erase(
							std::remove_if( temp_version.begin( ), temp_version.end( ),
								[ ]( int i ) { return !isdigit( i ); }
							), temp_version.end( )
						);

						unsigned int version{ };
						std::stringstream( temp_version ) >> version;

						sorted_list.push_back( interface_data( name, unicode_to_string( data_table->FullDllName ), reinterpret_cast< void * >( interfaces::interface_from_reg( regs ) ), version ) );

						regs = regs->m_pNext;
					}
				}

				std::sort( sorted_list.begin( ), sorted_list.end( ), [ ]( const interface_data &lhs, const interface_data &rhs ) {
					return( lhs.version > rhs.version );
				} );
			}
		public:
			template< typename T = void * > T create( std::string name ) {
				std::vector< interface_data >::iterator it = std::find_if( sorted_list.begin( ), sorted_list.end( ), [ & ]( const interface_data &data ) {
					return ( !data.name.compare( name ) );
				} );

				if ( it == sorted_list.end( ) || !it->ptr ) {
					return T{ };
				}

				printf( "%s\n -> 0x%X\n -> module: %s \n", name.c_str( ), reinterpret_cast< uintptr_t >( it->ptr ), it->module.c_str( ) );

				return( ( T )( it->ptr ) );
			}

			template< typename T = void* > T create( std::string name, std::string module ) {
				std::vector< interface_data >::iterator it = std::find_if( sorted_list.begin( ), sorted_list.end( ), [ & ]( const interface_data &data ) {
					return ( !data.name.compare( name ) && data.module.find( module ) != std::string::npos );
				} );


				if ( it == sorted_list.end( ) || !it->ptr ) {
					return T{ };
				}

				//#ifdef DEBUG_PRINT
				printf( "%s\n -> 0x%x\n -> module: %s \n", name.c_str( ), reinterpret_cast< uintptr_t >( it->ptr ), it->module.c_str( ) );
				//#endif

				return( static_cast< T >( it->ptr ) );
			}

			template< typename T = void * > T create_from_hash( uint32_t hash ) {
				std::vector< interface_data >::iterator it = std::find_if( sorted_list.begin( ), sorted_list.end( ), [ & ]( const interface_data &data )
				{
					return ( !data.name.compare( name ) );
				} );

				if ( it == sorted_list.end( ) || !it->ptr ) {
					return T{ };
				}

				return( ( T )( it->ptr ) );
			}
		protected:
			static inline PTEB get_teb( ) {
#if defined( _M_X64 )
				auto teb = reinterpret_cast< PTEB >( __readgsqword( reinterpret_cast< uintptr_t >( &static_cast< NT_TIB * >( nullptr )->Self ) ) );
#else
				auto teb = reinterpret_cast< PTEB >( __readfsdword( reinterpret_cast< uintptr_t >( &static_cast< NT_TIB * >( nullptr )->Self ) ) );
#endif
				

				return teb;
			}

			static inline PPEB get_peb( ) {
				return( get_teb( )->ProcessEnvironmentBlock );
			}

			static inline std::string unicode_to_string( UNICODE_STRING wide ) {
				std::wstring wide_str( wide.Buffer, wide.Length );
				return std::string( wide_str.begin( ), wide_str.end( ) );
			}

			std::vector< interface_data > sorted_list{ };
		};
	};
}