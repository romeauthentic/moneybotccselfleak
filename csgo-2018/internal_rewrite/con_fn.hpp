#pragma once
#include <functional>

#include "simple_settings.hpp"

//KEEP THIS UP TO DATE!!
enum ConFnVarType_t {
	TYPE_NULL = 0,
	TYPE_FLOAT = 'F',
	TYPE_STRING = 'S',
	TYPE_INTEGER = 'D',
	TYPE_HEX = 'X'
};

class con_fn_base {
public:
	virtual bool execute( const char* str ) = 0;
	virtual const char* get_syntax( ) = 0;
	virtual hash_t get_hash( ) = 0;
};

class con_fn : public con_fn_base {
public:
	con_fn( hash_t hash, std::function< void __cdecl( const char*, const char* ) > function, const char* syntax ) {
		m_hash = hash;
		m_function = function;
		strcpy_s< 64 >( m_syntax, syntax );
		for( size_t i{ }; i < strlen( m_syntax ); ++i ) {
			if( m_syntax[ i ] == '%' ) {
				m_args++;
			}
		}
	}

	static ConFnVarType_t get_var_type( size_t arg, const char* syntax ) {
		size_t cur_arg = 0;
		for( size_t i{ }; i < strlen( syntax ); ++i ) {
			if( syntax[ i ] == '%' ) {
				if( cur_arg++ == arg ) {
					return ( ConFnVarType_t )( syntax[ i + 1 ] );
				}
			}
		}

		return TYPE_NULL;
	}

	template < typename t >
	static t get_arg( const char* str, size_t arg_index, const char* syntax ) {
		auto arg_type = get_var_type( arg_index, syntax );

		size_t cur_arg = 0;
		for( size_t i{ }; i < strlen( str ); ++i ) {
			if( str[ i ] == ' ' ) {
				if( cur_arg++ == arg_index ) {
					size_t start = i + 1;
					size_t end = strlen( str );
					for( size_t i2 = start; i2 < strlen( str ); ++i2 ) {
						if( str[ i2 ] == ' ' ) {
							end = i2;
							break;
						}
					}

					std::string str( str + start, end - start );

					char* end_ptr = ( char* )str.c_str( ) + end;
					if constexpr( std::is_integral_v< t > ) {
						auto radix = get_var_type( arg_index, syntax ) == TYPE_HEX ? 16 : 10;
						return std::strtol( str.c_str( ), &end_ptr, radix );
					}

					if constexpr( std::is_floating_point_v< t > ) {
						return std::strtof( str.c_str( ), &end_ptr );
					}

					return ( t )str.c_str( );
				}
			}
		}

		return t{ };
	}

	virtual const char* get_syntax( ) override {
		return m_syntax;
	}

	virtual hash_t get_hash( ) override {
		return m_hash;
	}

	ConFnVarType_t get_var_type( size_t arg ) {
		size_t cur_arg = 0;
		for( size_t i{ }; i < strlen( m_syntax ); ++i ) {
			if( m_syntax[ i ] == '%' ) {
				if( cur_arg++ == arg ) {
					return ( ConFnVarType_t )( m_syntax[ i + 1 ] );
				}
			}
		}

		return TYPE_NULL;
	}

	//returns false on failed execution
	virtual bool execute( const char* str ) override {
		size_t arg_count{ };
		for( size_t i{ }; i < strlen( str ); ++i ) {
			if( str[ i ] == ' ' ) {
				arg_count++;
			}
		}

		if( arg_count != m_args ) {
			return false;
		}

		std::string pass_str( " " );

		size_t cur_arg = 0;
		for( size_t i{ }; i < strlen( str ); ++i ) {
			if( str[ i ] == ' ' ) {
				size_t end = strlen( str );
				for( size_t i2 = i + 2; i2 < strlen( str ); ++i2 ) {
					if( str[ i2 ] == ' ' ) {
						end = i2;
					}
				}

				char* end_ptr = ( char* )( str + i + end );
				switch( get_var_type( cur_arg ) ) {
				case TYPE_FLOAT: {
					pass_str += std::to_string( strtof( str + i, &end_ptr ) );
					break;
				}
				case TYPE_INTEGER: {
					pass_str += std::to_string( strtol( str + i, &end_ptr, 10 ) );
					break;
				}
				case TYPE_HEX: {
					char buf[ 10 ];
					sprintf_s< 10 >( buf, "%08x", strtol( str + i, &end_ptr, 16 ) );
					pass_str += buf;
					break;
				}
				case TYPE_STRING: {
					std::string add_str( str + i, end - i );
					pass_str += add_str;
					break;
				}
				default:
					return false;
				}

				if( end != strlen( str ) ) {
					pass_str += ' ';
				}
				++cur_arg;
			}
		}

		m_function( pass_str.c_str( ), m_syntax );
		return true;
	}

private:
	hash_t m_hash{ };
	size_t m_args{ };
	char m_syntax[ 64 ]{ };
	std::function< void( __cdecl )( const char*, const char* ) > m_function{ };
};