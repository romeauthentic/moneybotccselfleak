#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <string>

#include "util.hpp"
#include "fnv.hpp"

//nama stuff
extern void simple_save( const char* path, const char* name, const void* src, size_t size, const char* filename );
extern void simple_load( const char* path, const char* name, void* dest, size_t size, const char* filename );

class ISettingNode {
public:
	virtual ~ISettingNode( ) = default;

	virtual void load( const char* path, const char* file ) = 0;
	virtual void save( const char* path, const char* file ) const = 0;
	virtual void register_( ISettingNode* node_ptr ) = 0;
	virtual bool has_nodes( ) { return false; }
	virtual hash_t get_hash( ) { return 0; }
};

class SettingHolder : public ISettingNode {
public:
	__forceinline SettingHolder( const char* name ) : name_( name ) {}
	__forceinline SettingHolder( SettingHolder* holder_ptr, const char* name ) : name_( name ) { holder_ptr->register_( this ); }

	void register_( ISettingNode* node_ptr ) override {
		setting_nodes_.push_back( node_ptr );
	}

	void load( const char* path, const char* file ) override {
		char full_path[ 64 ];
		strcpy_s( full_path, path );
		strcat_s( full_path, "_" );
		strcat_s( full_path, name_ );
		for ( auto x : setting_nodes_ )
			x->load( full_path, file );
	}

	void save( const char* path, const char* file ) const override {
		char full_path[ 64 ];
		strcpy_s( full_path, path );
		strcat_s( full_path, "_" );
		strcat_s( full_path, name_ );
		for ( auto x : setting_nodes_ )
			x->save( full_path, file );
	}

	auto& get_nodes( ) {
		return setting_nodes_;
	}

	virtual bool has_nodes( ) {
		return true;
	}

private:
	const char* name_;
	bool has_nodes_;
	std::vector<ISettingNode*> setting_nodes_;
};

class ISetting : public ISettingNode {
	void register_( ISettingNode* node_ptr ) override {}
public:
	virtual void set( float value ) = 0;
	virtual void set( int value ) = 0;
	virtual void set( ulong_t value ) = 0;
	virtual std::string get_string( ) = 0;
	virtual hash_t get_hash( ) = 0;
	virtual std::string get_data_string( ) = 0;
	virtual void load_from_string( const std::string& ) = 0;
};

template < size_t L >
class SettingString : ISetting {
public:
	__forceinline SettingString( SettingHolder* holder_ptr, hash_t hash ) : 
		m_name( name ), m_value( "" ) {
		holder_ptr->register_( this );
	};

	void load( const char* path, const char* file ) override {
		GetPrivateProfileStringA( path, std::to_string( m_name ).c_str( ), "", m_value, L, file );
	}

	void save( const char* path, const char* file ) const override {
		WritePrivateProfileStringA( path, std::to_string( m_name ).c_str( ), m_value, file );
	}

	char* get( ) {
		return m_value;
	}

private:
	char m_value[ L ];
	hash_t m_name;
};

template < typename T >
class con_var : public ISetting {
public:
	__forceinline con_var( SettingHolder* holder_ptr, hash_t name ) : 
		name_( name ), 
		is_float_( std::is_floating_point_v< T > ),
		is_integral_( std::is_integral_v< T > ) { 
		holder_ptr->register_( this );
	}
	__forceinline con_var( SettingHolder* holder_ptr, hash_t name , const T& rhs ) : 
		value_( rhs ), 
		name_( name ),
		is_float_( std::is_floating_point_v< T > ),
		is_integral_( std::is_integral_v< T > ) {
		holder_ptr->register_( this );
	}

	void load( const char* path, const char* file ) override { simple_load( path, std::to_string( name_ ).c_str( ), &value_, sizeof( value_ ), file ); }

	void save( const char* path, const char* file ) const override { simple_save( path, std::to_string( name_ ).c_str( ), &value_, sizeof( value_ ), file ); }

	__forceinline operator T&( ) { return value_; }
	__forceinline T* operator &( ) { return &value_; }

	__forceinline T& operator ()( ) { return value_; }

	virtual std::string get_string( ) {
		if constexpr( std::is_arithmetic_v< T > ) {
			return std::to_string( value_ );
		}
		else {
			return util::to_hex_str( *( ulong_t* )( &value_ ) );
		}
	}

	virtual void set( float value ) {
		if constexpr( std::is_arithmetic_v< T > ) {
			value_ = ( T )( value );
		}
	}

	virtual void set( int value ) {
		if constexpr( std::is_arithmetic_v< T > ) {
			value_ = ( T )( value );
		}
		else {
			value_ = *( T* )( &value );
		}
	}

	virtual void set( ulong_t value ) {
		if constexpr( sizeof( T ) == sizeof( ulong_t ) ) {
			*( ulong_t* )( &value_ ) = value;
			return;
		}

		memcpy( &value_, &value, sizeof( value_ ) );
	}

	virtual hash_t get_hash( ) {
		return name_;
	}

	__forceinline bool is_floating_point( ) {
		return is_float_;
	}

	__forceinline bool is_integral( ) {
		return is_integral_;
	}

	virtual std::string get_data_string( ) override {
		auto buffer = reinterpret_cast< char* >( _alloca( sizeof( T ) * 2 + 1 ) );
		auto data = reinterpret_cast< const uint8_t* >( &value_ );

		for( size_t i = 0; i < sizeof( T ); i++ )
			sprintf( &buffer[ 2 * i ], "%02X", data[ i ] );

		return buffer;
	}

	virtual void load_from_string( const std::string& str ) override {
		auto data = reinterpret_cast< uint8_t* >( &value_ );
		auto buffer = str.data( );

		if( *buffer == 0 )
			return;

		for( size_t i = 0; i < sizeof( T ); i++ ) {
			unsigned temp;
			sscanf( &buffer[ 2 * i ], "%02X", &temp );
			data[ i ] = temp;
		}
	}


private:
	hash_t name_;
	bool is_float_;
	bool is_integral_;
	T value_;
};