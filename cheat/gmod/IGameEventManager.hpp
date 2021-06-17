#pragma once
#include "util.hpp"

class IGameEvent
{
public:
	const char* GetName( ) {
		return util::get_vfunc< 1, const char* >( this );
	}

	int GetInt( const char* szKeyName, int nDefault = 0 ) {
		return util::get_vfunc< 6, int >( this, szKeyName, nDefault );
	}

	float GetFloat( const char* name, float default_ = 0 ) {
		return util::get_vfunc< 8, float >( this, name, default_ );
	}

	const char* GetString( const char* szKeyName ) {
		return util::get_vfunc< 9, const char* >( this, szKeyName, 0 );
	}

	void SetString( const char* szKeyName, const char* szValue ) {
		return util::get_vfunc< 16, void >( this, szKeyName, szValue );
	}
};

class IGameEventListener2 {
public:
	virtual ~IGameEventListener2( void ) {};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent( IGameEvent* event ) = 0;

	virtual int GetEventDebugID( void ) = 0;
};

class IGameEventManager2 {
public:
	virtual ~IGameEventManager2( void ) {};

	// load game event descriptions from a file eg "resource\gameevents.res"
	virtual int LoadEventsFromFile( const char* filename ) = 0;

	// removes all and anything
	virtual void Reset( ) = 0;

	// adds a listener for a particular event
	virtual bool AddListener( IGameEventListener2* listener, const char* name, bool bServerSide ) = 0;

	// returns true if this listener is listens to given event
	virtual bool FindListener( IGameEventListener2* listener, const char* name ) = 0;

	// removes a listener 
	virtual void RemoveListener( IGameEventListener2* listener ) = 0;

	// create an event by name, but doesn't fire it. returns NULL is event is not
	// known or no listener is registered for it. bForce forces the creation even if no listener is active
	virtual IGameEvent* CreateEvent( const char* name, bool bForce = false, int* pCookie = nullptr ) = 0;

	// fires a server event created earlier, if bDontBroadcast is set, event is not send to clients
	virtual bool FireEvent( IGameEvent* event, bool bDontBroadcast = false ) = 0;

	// fires an event for the local client only, should be used only by client code
	virtual bool FireEventClientSide( IGameEvent* event ) = 0;

	// create a new copy of this event, must be free later
	virtual IGameEvent* DuplicateEvent( IGameEvent* event ) = 0;

	// if an event was created but not fired for some reason, it has to bee freed, same UnserializeEvent
	virtual void FreeEvent( IGameEvent* event ) = 0;

	// write/read event to/from bitbuffer
	virtual bool SerializeEvent( IGameEvent* event, int* buf ) = 0;
	virtual IGameEvent* UnserializeEvent( void* buf ) = 0; // create new KeyValues, must be deleted
};