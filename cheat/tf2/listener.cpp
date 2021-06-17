#include "listener.hpp"
#include "interfaces.h"
#include "base_cheat.h"
#include "ctx.hpp"
#include "settings.h"
#undef PlaySound

//how can you listen if youre deaf

generic_listener_t::generic_listener_t( const char* name, void( *function )( IGameEvent* ) ) :
	m_function( function ), m_name( name ) { }

generic_listener_t::~generic_listener_t( ) {
	if( m_registered )
		cl.m_event_mgr( )->RemoveListener( this );

}

void generic_listener_t::init( ) {
	cl.m_event_mgr( )->AddListener( this, m_name, false );
	m_registered = true;
}

namespace listeners
{

	void player_hurt( IGameEvent* e ) {
		if( !cl.m_panic && e ) {
			int user_id = e->GetInt( xors( "userid" ) );
			int attacker = e->GetInt( xors( "attacker" ) );
			int attacker_id = cl.m_engine( )->GetPlayerForUserID( attacker );
			int player_id = cl.m_engine( )->GetPlayerForUserID( user_id );


			//if( attacker_id == cl.m_engine( )->GetLocalPlayer( ) && user_id != attacker_id ) {
				g_cheat.visuals.store_hit( );

			//}
		}
	}

}