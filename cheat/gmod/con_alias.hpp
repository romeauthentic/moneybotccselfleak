#pragma once

#include <string>
#include "simple_settings.hpp"

template< typename var_type = bool >
class con_alias : public ISetting {
public:
	con_alias( hash_t hash, con_var< var_type >* var ) :
		m_var( var ),
		m_value( var_type{ } ),
		m_name( hash ),
		m_is_var( true ) { };

	con_alias( hash_t hash ) :
		m_name( hash ),
		m_value( 0 ),
		m_var( nullptr ),
		m_is_var( false ) { };

	con_alias( hash_t hash, var_type&& rhs ) :
		m_name( hash ),
		m_value( rhs ),
		m_var( nullptr ),
		m_is_var( false ) { };

	virtual std::string get_string( ) override {
		if( m_is_var )
			return m_var->get_string( );
		else
			return std::to_string( m_value );
	}

	virtual void set_value( int value ) override {
		set_value_internal( value );
	}

	virtual void set_value( float value ) override {
		set_value_internal( value );
	}

	virtual void set_value( ulong_t value ) override {
		set_value_internal( value );
	}

private:
	template < typename t >
	void set_value_internal( t&& val ) {
		if( m_is_var )
			m_var->set( val );
		else
			m_value = ( var_type )( val );
	}

private:
	bool				 m_is_var;
	hash_t				 m_name;
	var_type			 m_value;
	con_var< var_type >* m_var;
};