#pragma once

#include <cstdint>
#include <string>

#include "vector.hpp"
#include "color.hpp"

#define LUA_REGISTRYINDEX       (-10000)
#define LUA_ENVIRONINDEX        (-10001)
#define LUA_GLOBALSINDEX        (-10002)

class CLuaInterface;

enum STATE_TYPE
{
	LUA_CLIENT = 0,
	LUA_SERVER,
	LUA_MENU
};


enum LUA_SPECIAL
{
	SPECIAL_GLOB,
	SPECIAL_ENV,
	SPECIAL_REG,
};


enum LUA_TYPE : int8_t {
	TYPE_INVALID = -1,
	TYPE_NIL,
	TYPE_BOOL,
	TYPE_LIGHTUSERDATA,
	TYPE_NUMBER,
	_TYPE_STRING,
	TYPE_TABLE,
	TYPE_FUNCTION,
	TYPE_USERDATA,
	TYPE_THREAD,

	TYPE_ENTITY,
	TYPE_VECTOR,
	TYPE_ANGLE,
	TYPE_PHYSOBJ,
	TYPE_SAVE,
	TYPE_RESTORE,
	TYPE_DAMAGEINFO,
	TYPE_EFFECTDATA,
	TYPE_MOVEDATA,
	TYPE_RECIPIENTFILTER,
	TYPE_USERCMD,
	TYPE_SCRIPTEDVEHICLE,

	TYPE_MATERIAL,
	TYPE_PANEL,
	TYPE_PARTICLE,
	TYPE_PARTICLEEMITTER,
	TYPE_TEXTURE,
	TYPE_USERMSG,

	TYPE_CONVAR,
	TYPE_IMESH,
	TYPE_MATRIX,
	TYPE_SOUND,
	TYPE_PIXELVISHANDLE,
	TYPE_DLIGHT,
	TYPE_VIDEO,
	TYPE_FILE,

	TYPE_COUNT,

	TYPE_PLAYER = TYPE_ENTITY,
};

struct lua_State
{
	uint8_t header[ 69 ];
	CLuaInterface* m_lua_base;
};

struct UserData
{
	void* data;
	LUA_TYPE type;
};

using CLuaFunction = int( __cdecl* )( lua_State* );

class CLuaInterface
{
public:
	virtual int			Top( void ) = 0;
	virtual void		Push( int iStackPos ) = 0;
	virtual void		Pop( int iAmt = 1 ) = 0;
	virtual void		GetTable( int iStackPos ) = 0;
	virtual void		GetField( int iStackPos, const char* strName ) = 0;
	virtual void		SetField( int iStackPos, const char* strName ) = 0;
	virtual void		CreateTable( ) = 0;
	virtual void		SetTable( int i ) = 0;
	virtual void		SetMetaTable( int i ) = 0;
	virtual bool		GetMetaTable( int i ) = 0;
	virtual void		Call( int iArgs, int iResults ) = 0;
	virtual int			PCall( int iArgs, int iResults, int iErrorFunc ) = 0;
	virtual int			Equal( int iA, int iB ) = 0;
	virtual int			RawEqual( int iA, int iB ) = 0;
	virtual void		Insert( int iStackPos ) = 0;
	virtual void		Remove( int iStackPos ) = 0;
	virtual int			Next( int iStackPos ) = 0;
	virtual void*		NewUserdata( unsigned int iSize ) = 0;
	virtual void		ThrowError( const char* strError ) = 0;
	virtual void		CheckType( int iStackPos, int iType ) = 0;
	virtual void		ArgError( int iArgNum, const char* strMessage ) = 0;
	virtual void		RawGet( int iStackPos ) = 0;
	virtual void		RawSet( int iStackPos ) = 0;

	virtual const char*		GetString( int iStackPos = -1, unsigned int* iOutLen = NULL ) = 0;
	virtual double			GetNumber( int iStackPos = -1 ) = 0;
	virtual bool			GetBool( int iStackPos = -1 ) = 0;
	virtual CLuaFunction*	GetCFunction( int iStackPos = -1 ) = 0;
	virtual void*			GetUserdata( int iStackPos = -1 ) = 0;

	virtual void		PushNil( ) = 0;
	virtual void		PushString( const char* val, unsigned int iLen = 0 ) = 0;
	virtual void		PushNumber( double val ) = 0;
	virtual void		PushBool( bool val ) = 0;
	virtual void		PushCFunction( CLuaFunction* val ) = 0;
	virtual void		PushCClosure( CLuaFunction* val, int iVars ) = 0;
	virtual void		PushUserdata( void* ) = 0;

	//
	// If you create a reference - don't forget to free it!
	//
	virtual int			ReferenceCreate( ) = 0;
	virtual void		ReferenceFree( int i ) = 0;
	virtual void		ReferencePush( int i ) = 0;

	//
	// Push a special value onto the top of the stack ( see below )
	//
	virtual void		PushSpecial( int iType ) = 0;

	//
	// For type enums see Types.h 
	//
	virtual bool			IsType( int iStackPos, int iType ) = 0;
	virtual int				GetType( int iStackPos ) = 0;
	virtual const char*		GetTypeName( int iType ) = 0;

	//
	// Creates a new meta table of string and type and leaves it on the stack.
	// Will return the old meta table of this name if it already exists.
	//
	virtual void			CreateMetaTableType( const char* strName, int iType ) = 0;

	//
	// Like Get* but throws errors and returns if they're not of the expected type
	//
	virtual const char*		CheckString( int iStackPos = -1 ) = 0;
	virtual double	CheckNumber( int iStackPos = -1 ) = 0;


	virtual void ObjLen( int ) = 0;
	virtual void GetAngle( int ) = 0;
	virtual void GetVector( int ) = 0;
	virtual void PushAngle( vec3_t const& ) = 0;
	virtual void PushVector( vec3_t const& ) = 0;
	virtual void SetState( lua_State * ) = 0;
	virtual void CreateMetaTable( char const* ) = 0;
	virtual void PushMetaTable( int ) = 0;
	virtual void PushUserType( void *, int ) = 0;
	virtual void SetUserType( int, void * ) = 0;
	virtual void Init( void *, bool ) = 0;
	virtual void Shutdown( void ) = 0;
	virtual void Cycle( void ) = 0;
	virtual void Global( void ) = 0;
	virtual void* GetObject( int ) = 0;
	virtual void PushLuaObject( void * ) = 0;
	virtual void PushLuaFunction( CLuaFunction ) = 0;
	virtual void LuaError( char const*, int ) = 0;
	virtual void TypeError( char const*, int ) = 0;
	virtual void CallInternal( int, int ) = 0;
	virtual void CallInternalNoReturns( int ) = 0;
	virtual void CallInternalGetBool( int ) = 0;
	virtual void CallInternalGetString( int ) = 0;
	virtual void CallInternalGet( int, void * ) = 0;
	virtual void NewGlobalTable( char const* ) = 0;
	virtual void NewTemporaryObject( void ) = 0;
	virtual void isUserData( int ) = 0;
	virtual void GetMetaTableObject( char const*, int ) = 0;
	virtual void GetMetaTableObject( int ) = 0;
	virtual void GetReturn( int ) = 0;
	virtual void IsServer( void ) = 0;
	virtual void IsClient( void ) = 0;
	virtual void IsDedicatedServer( void ) = 0;
	virtual void DestroyObject( void * ) = 0;
	virtual void CreateObject( void ) = 0;
	virtual void SetMember( void *, void *, void * ) = 0;
	virtual void GetNewTable( void ) = 0;
	virtual void SetMember( void *, float ) = 0;
	virtual void SetMember( void *, float, void * ) = 0;
	virtual void SetMember( void *, char const* ) = 0;
	virtual void SetMember( void *, char const*, void * ) = 0;
	virtual void SetIsServer( bool ) = 0;
	virtual void PushLong( long ) = 0;
	virtual void GetFlags( int ) = 0;
	virtual void FindOnObjectsMetaTable( int, int ) = 0;
	virtual void FindObjectOnTable( int, int ) = 0;
	virtual void SetMemberFast( void *, int, int ) = 0;
	virtual void RunString( char const* filename, char const* path, char const* stringtoun, bool run = true, bool showerrors = true ) = 0;
	virtual void IsEqual( void *, void * ) = 0;
	virtual void Error( char const* ) = 0;
	virtual void GetStringOrError( int ) = 0;
	virtual void RunLuaModule( char const* ) = 0;
	virtual void FindAndRunScript( const char* path, bool run = true, bool showerrors = true, const char* type = "!CLIENT" ) = 0;
	virtual void SetPathID( char const* ) = 0;
	virtual void GetPathID( void ) = 0;
	virtual void ErrorNoHalt( char const*, ... ) = 0;
	virtual void Msg( char const*, ... ) = 0;
	virtual void PushPath( char const* ) = 0;
	virtual void PopPath( void ) = 0;
	virtual void GetPath( void ) = 0;
	virtual void GetColor( int ) = 0;
	virtual void PushColor( clr_t ) = 0;
	virtual int GetStack( int, void * ) = 0;
	virtual void GetInfo( char const*, void * ) = 0;
	virtual void GetLocal( void *, int ) = 0;
	virtual void GetUpvalue( int, int ) = 0;
	virtual void RunStringEx( void*, char const* filename, char const* path, char const* torun, bool run, bool showerrors, bool idk, bool idk2 ) = 0;
	virtual void GetDataString( int, void ** ) = 0;
	virtual void ErrorFromLua( char const*, ... ) = 0;
	virtual void GetCurrentLocation( void ) = 0;
	virtual void MsgColour( clr_t const&, char const*, ... ) = 0;
	virtual void GetCurrentFile( std::string & ) = 0;
	virtual void CompileString( int &, std::string const& ) = 0;
	virtual void CallFunctionProtected( int, int, bool ) = 0;
	virtual void Require( char const* ) = 0;
	virtual void GetActualTypeName( int ) = 0;
	virtual void PreCreateTable( int, int ) = 0;
	virtual void PushPooledString( int ) = 0;
	virtual void GetPooledString( int ) = 0;
	virtual void AddThreadedCall( void * ) = 0;

	lua_State* L;
};
