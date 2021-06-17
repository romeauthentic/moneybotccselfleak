#pragma once
#pragma once

#include "vector.hpp"

using CBaseEntity = uintptr_t;
class CUserCmd;

typedef int EntityHandle_t, soundlevel_t;
class CGameTrace;

typedef CGameTrace trace_t;

#define MAX_CLIMB_SPEED 200.0

class CMoveData
{
public:
	bool			m_bFirstRunOfFunctions : 1;
	bool			m_bGameCodeMovedPlayer : 1;

	EntityHandle_t	m_nPlayerHandle;

	int				m_nImpulseCommand;	
	vec3_t			m_vecViewAngles;	
	vec3_t			m_vecAbsViewAngles;	
	int				m_nButtons;		
	int				m_nOldButtons;		
	float			m_flForwardMove;
	float			m_flSideMove;
	float			m_flUpMove;

	float			m_flMaxSpeed;
	float			m_flClientMaxSpeed;

	vec3_t			m_vecVelocity;	
	vec3_t			m_vecAngles;	
	vec3_t			m_vecOldAngles;
	float			m_outStepHeight;
	vec3_t			m_outWishVel;	
	vec3_t			m_outJumpVel;	

	vec3_t			m_vecConstraintCenter;
	float			m_flConstraintRadius;
	float			m_flConstraintWidth;
	float			m_flConstraintSpeedFactor;

	vec3_t			m_vecAbsOrigin;
};

class IGameMovement
{
public:
	virtual			~IGameMovement( void ) {}
	virtual void	ProcessMovement( CBaseEntity *pPlayer, CMoveData *pMove ) = 0;
	virtual void	StartTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( CBaseEntity *pPlayer ) = 0;
	virtual void	DiffPrint( char const *fmt, ... ) = 0;
	virtual vec3_t	GetPlayerMins( bool ducked ) const = 0;
	virtual vec3_t	GetPlayerMaxs( bool ducked ) const = 0;
	virtual vec3_t  GetPlayerViewOffset( bool ducked ) const = 0;
};

class IMoveHelper
{
public:
	virtual	char const*		GetName( EntityHandle_t handle ) const = 0;
	virtual void	ResetTouchList( void ) = 0;
	virtual bool	AddToTouched( const CGameTrace& tr, const vec3_t& impactvelocity ) = 0;
	virtual void	ProcessImpacts( void ) = 0;
	virtual void	Con_NPrintf( int idx, char const* fmt, ... ) = 0;
	virtual void	StartSound( const vec3_t& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch ) = 0;
	virtual void	StartSound( const vec3_t& origin, const char *soundname ) = 0;
	virtual void	PlaybackEventFull( int flags, int clientindex, unsigned short eventindex, float delay, vec3_t& origin, vec3_t& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 ) = 0;
	virtual bool	PlayerFallingDamage( void ) = 0;
	virtual void	PlayerSetAnimation( int playerAnim ) = 0;

	virtual void *GetSurfaceProps( void ) = 0;

	virtual bool IsWorldEntity( const uintptr_t &handle ) = 0;

protected:
	virtual			~IMoveHelper( ) {}
};

class IPrediction
{
public:
	virtual			~IPrediction( void ) {};

	virtual void	Init( void ) = 0;
	virtual void	Shutdown( void ) = 0;

	// Run prediction
	virtual void	Update
	(
		int startframe,				// World update ( un-modded ) most recently received
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) = 0;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void	PostEntityPacketReceived( void ) = 0;
	virtual void	PostNetworkDataReceived( int commands_acknowledged ) = 0;

	virtual void	OnReceivedUncompressedPacket( void ) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin( vec3_t& org ) = 0;
	virtual void	SetViewOrigin( vec3_t& org ) = 0;
	virtual void	GetViewAngles( vec3_t& ang ) = 0;
	virtual void	SetViewAngles( vec3_t& ang ) = 0;
	virtual void	GetLocalViewAngles( vec3_t& ang ) = 0;
	virtual void	SetLocalViewAngles( vec3_t& ang ) = 0;
};