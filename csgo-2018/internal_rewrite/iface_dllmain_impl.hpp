#pragma once

namespace factory
{
	namespace interfaces
	{
		class c_interface_manager {
			struct reg {
				char	  m_key;
				uintptr_t m_ptr;
				uintptr_t m_module;
				char	  m_module_name[ 64 ];
				char	  m_name[ 64 ];
			};

			size_t	m_count;
			reg*	m_regs;
		
			void decrypt_str( char* buf, size_t size, char key ) {
				for( size_t i{ }; i < size; ++i ) {
					buf[ i ] ^= key;
				}
			}

		public:
			void init( uintptr_t iface_addr ) {
				m_count = *( size_t* )( iface_addr );
				m_regs  = ( reg* )( iface_addr + 4 );
			}

			template < typename t = void* >
			t find_interface( const std::string& module_, std::string name ) {
				if( !::isdigit( name[ name.length( ) - 1 ] ) )
					name += '0';

				char name_buf[ 64 ];
				char module_buf[ 64 ];
				
				for( size_t i{ }; i < m_count; ++i ) {
					auto& reg = m_regs[ i ];

					memcpy( name_buf, reg.m_name, 64 );
					memcpy( module_buf, reg.m_module_name, 64 );

					decrypt_str( name_buf, 64, reg.m_key );
					decrypt_str( module_buf, 64, reg.m_key );

					if( !module_.compare( module_buf ) && strstr( name_buf, name.c_str( ) ) ) {
						return ( t )( reg.m_ptr );
					}
				}

				return t{ };
			}

			template < typename t = void* >
			t find_interface( std::string name ) {
				if( !::isdigit( name[ name.length( ) - 1 ] ) )
					name += '0';

				char name_buf[ 64 ];
				char module_buf[ 64 ];

				for( size_t i{ }; i < m_count; ++i ) {
					auto& reg = m_regs[ i ];

					memcpy( name_buf, reg.m_name, 64 );
					memcpy( module_buf, reg.m_module_name, 64 );

					decrypt_str( name_buf, 64, reg.m_key );
					decrypt_str( module_buf, 64, reg.m_key );

					if( strstr( name_buf, name.c_str( ) ) ) {
						return ( t )( reg.m_ptr );
					}
				}

				return t{ };
			}
		};
	}
}