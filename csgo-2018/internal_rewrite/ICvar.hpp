#pragma once
#include "IAppSystem.hpp"
#include "util.hpp"
#include "color.hpp"

using CVarDLLIdentifier_t = int;
class IConVar;
using FnChangeCallback_t = void( *)( IConVar *var, const char *pOldValue, float flOldValue );
class ConCommandBase;
class ConCommand;
class ICvarQuery;

class IConsoleDisplayFunc {
public:
	virtual void ColorPrint( const uint8_t* clr, const char *pMessage ) = 0;
	virtual void Print( const char *pMessage ) = 0;
	virtual void DPrint( const char *pMessage ) = 0;
};

#define FCVAR_NONE				0 

// Command to ConVars and ConCommands
// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 


class cvar_t {
public:
	void set_value( const char* value ) {
		using fn = void( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 14 )( this, value );
	}

	void set_value( float value ) {
		using fn = void( __thiscall* )( void*, float );
		return util::get_vfunc< fn >( this, 15 )( this, value );
	}

	void set_value( int value ) {
		using fn = void( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 16 )( this, value );
	}

	void set_value( clr_t value ) {
		using fn = void( __thiscall* )( void*, clr_t );
		return util::get_vfunc< fn >( this, 17 )( this, value );
	}

	char* get_name( ) {
		using fn = char*( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 5 )( this );
	}

	char* get_default( ) {
		return m_default;
	}

	const char* get_string( ) {
		return m_string;
	}

	float get_float( ) {
		using fn = float( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 12 )( this );
	}

	int get_int( ) {
		using fn = int( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 13 )( this );
	}

private:
	void* m_vft;
public:
	cvar_t* m_next; //0x0004 
	__int32 m_registered; //0x0008 
	char* m_name; //0x000C 
	char* m_help_str; //0x0010 
	__int32 m_flags; //0x0014 
private:
	char pad_0x0018[ 0x4 ]; //0x0018
public:
	cvar_t* m_parent; //0x001C 
	char* m_default; //0x0020 
	char* m_string; //0x0024 
	__int32 m_str_len; //0x0028 
	float m_flvalue; //0x002C 
	__int32 m_nvalue; //0x0030 
	__int32 m_has_min; //0x0034 
	float m_min; //0x0038 
	__int32 m_hax_max; //0x003C 
	float m_max; //0x0040 
	void* m_change_callback; //0x0044 
};//Size=0x0048

class ICVar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier( ) = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void			UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase*	FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase*	FindCommandBase( const char *name ) const = 0;
	virtual cvar_t			*FindVar( const char *var_name ) = 0;
	virtual const cvar_t	*FindVar( const char *var_name ) const = 0;
	virtual ConCommand		*FindCommand( const char *name ) = 0;
	virtual const ConCommand *FindCommand( const char *name ) const = 0;



	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			CallGlobalChangeCallbacks( cvar_t *var, const char *pOldString, float flOldValue ) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			ConsoleColorPrintf( const clr_t& clr, const char *pFormat, ... ) const = 0;
	virtual void			ConsolePrintf( const char *pFormat, ... ) const = 0;
	virtual void			ConsoleDPrintf( const char *pFormat, ... ) const = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars( int nFlag ) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery( ICvarQuery *pQuery ) = 0;


	virtual void			SetMaxSplitScreenSlots( int nSlots ) = 0;
	virtual int				GetMaxSplitScreenSlots( ) const = 0;

	virtual void			AddSplitScreenConVars( ) = 0;
	virtual void			RemoveSplitScreenConVars( CVarDLLIdentifier_t id ) = 0;

	virtual int				GetConsoleDisplayFuncCount( ) const = 0;
	virtual void			GetConsoleText( int nDisplayFuncIndex, char *pchText, size_t bufSize ) const = 0;

	// Utilities for convars accessed by the material system thread
	virtual bool			IsMaterialThreadSetAllowed( ) const = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, const char *pValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, int nValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( cvar_t *pConVar, float flValue ) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets( ) const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets( ) = 0;

	class ICVarIteratorInternal
	{
	public:
		virtual void		SetFirst( ) = 0;
		virtual void		Next( ) = 0;
		virtual	bool		IsValid( ) = 0;
		virtual ConCommandBase *Get( ) = 0;
	};

	virtual ICVarIteratorInternal*	FactoryInternalIterator( ) = 0;
};