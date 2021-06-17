#include "util.hpp"
using SteamPipeHandle = uint32_t;
using SteamUserHandle = uint32_t;

enum game_coordinator_result_t {
	result_ok,
	result_no_message,
	result_small_buffer,
	result_not_logged_on,
	result_invalid,

};

class ISteamFriends {
public:
	bool set_rich_presence( const char* key, const char* value ) {
		return util::get_vfunc< 43, bool >( this, key, value );
	}

	void clear_rich_presence( ) {
		util::get_vfunc< 44, void >( this );
	}
};

class ISteamClient {
public:

	void* GetISteamFriends( SteamUserHandle user, SteamPipeHandle pipe, const char* name ) {
		return util::get_vfunc< 8, void* >( this, user, pipe, name );
	}

	void* GetISteamGenericInterface( SteamUserHandle user, SteamPipeHandle pipe, const char* name ) {
		return util::get_vfunc< 12, void* >( this, user, pipe, name );
	}

};

class ISteamGameCoordinator {
public:
	virtual game_coordinator_result_t send_message( uint32_t msg_type, const void* pub_data, uint32_t data ) = 0;
	virtual bool IsMessageAvailable( uint32_t* msg_size ) = 0;
	virtual game_coordinator_result_t RetrieveMessage( uint32_t* msg_type, void* pub_dest, uint32_t dest, uint32_t* msg_size ) = 0;
};
