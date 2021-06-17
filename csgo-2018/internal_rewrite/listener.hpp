#pragma once
#include "sdk.hpp"

struct generic_listener_t : public IGameEventListener2 {
	generic_listener_t( const char* event_name,
		void( *function )( IGameEvent* ) );

	generic_listener_t( ) = default;

	~generic_listener_t( );

	void init( );

	void FireGameEvent( IGameEvent* event ) override { m_function( event ); }
	int  GetEventDebugID( ) override { return 0x2a; }
private:
	void( *m_function )( IGameEvent* );
	const char* m_name{ };
	bool m_registered{ };
};

namespace listeners {
	extern void bullet_impact( IGameEvent* e );
	extern void player_hurt( IGameEvent* e );
	extern void round_start( IGameEvent* e );
	extern void molotov_detonate( IGameEvent* e );
	extern void weapon_fire( IGameEvent* e );
	extern void player_death(IGameEvent* e);
}