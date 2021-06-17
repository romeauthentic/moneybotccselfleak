#pragma once
#include "vector.hpp"

//forward declarations
class user_cmd_t;
class IClientEntity;

class IMoveHelper {
private:
	virtual void UnknownVirtual( ) = 0;
public:
	virtual void SetHost( IClientEntity* host ) = 0;
};

class CMoveData {
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	unsigned long m_nPlayerHandle;
	int m_nImpulseCommand;
	vec3_t m_vecViewAngles;
	vec3_t m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	vec3_t m_vecVelocity;
	vec3_t m_vecOldVelocity;
	float somefloat;
	vec3_t m_vecAngles;
	vec3_t m_vecOldAngles;

	float m_outStepHeight;
	vec3_t m_outWishVel;
	vec3_t m_outJumpVel;

	vec3_t m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;

	void SetAbsOrigin( const vec3_t& vec );
	const vec3_t& GetAbsOrigin( ) const;

private:
	vec3_t m_vecAbsOrigin; // edict::origin
	uint8_t pad_ending_001[0x160];
};

class ICSGameMovement {
public:
	void ProcessMovement( IClientEntity* ent, void* data ) {
		using fn = void( __thiscall* )( void*, IClientEntity*, void* );
		return util::get_vfunc< fn >( this, 1 )( this, ent, data );
	}

	void StartTrackPredictionErrors( IClientEntity* ent ) {
		using fn = void( __thiscall* )( void*, IClientEntity* );
		return util::get_vfunc< fn >( this, 3 )( this, ent );
	}

	void FinishTrackPredictionErrors( IClientEntity* ent ) {
		using fn = void( __thiscall* )( void*, IClientEntity* );
		return util::get_vfunc< fn >( this, 4 )( this, ent );
	}
};

class IPrediction {
public:
	virtual			~IPrediction( ) { };

	virtual void	Init( ) = 0;
	virtual void	Shutdown( ) = 0;

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
	virtual void	PostEntityPacketReceived( ) = 0;
	virtual void	PostNetworkDataReceived( int commands_acknowledged ) = 0;

	virtual void	OnReceivedUncompressedPacket( ) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin( vec3_t& org ) = 0;
	virtual void	SetViewOrigin( vec3_t& org ) = 0;
	virtual void	GetViewAngles( vec3_t& ang ) = 0;
	virtual void	SetViewAngles( vec3_t& ang ) = 0;
	virtual void	GetLocalViewAngles( vec3_t& ang ) = 0;
	virtual void	SetLocalViewAngles( vec3_t& ang ) = 0;

	virtual bool	InPrediction( ) const = 0;
	virtual bool	IsFirstTimePredicted( ) const = 0;

	virtual int		GetLastAcknowledgedCommandNumber( ) const = 0;

	virtual int		GetIncomingPacketNumber( ) const = 0;

	virtual void	CheckMovingGround( IClientEntity* player, double frametime ) = 0;
	virtual void	RunCommand( IClientEntity *player, user_cmd_t *ucmd, IMoveHelper *moveHelper ) = 0;

	virtual void	SetupMove( IClientEntity *player, user_cmd_t *ucmd, IMoveHelper *pHelper, CMoveData *move ) = 0;
	virtual void	FinishMove( IClientEntity *player, user_cmd_t *ucmd, CMoveData *move ) = 0;
	virtual void	SetIdealPitch( int nSlot, IClientEntity *player, const vec3_t& origin, const vec3_t& angles, const vec3_t& viewheight ) = 0;

	virtual void	CheckError( int nSlot, IClientEntity *player, int commands_acknowledged ) = 0;

	virtual void	_Update
	(
		int nSlot,
		bool received_new_world_update,
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) = 0;
};
