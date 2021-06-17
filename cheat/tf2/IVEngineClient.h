#pragma once
#include "VFunc.h"

typedef float matrix3x4[ 3 ][ 4 ];
typedef struct player_info_s {
	char			name[ 32 ];
	int				userID;
	char			guid[ 33 ];
	unsigned long	friendsID;
	char			friendsName[ 32 ];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[ 4 ];
	byte	filesDownloaded;
} player_info_t;

class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName( void ) const = 0;	// get channel name
	virtual const char  *GetAddress( void ) const = 0; // get channel IP address as string
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;	// true if demo playback

	virtual float		GetLatency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int			GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual bool		IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int			GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool		GetStreamProgress( int flow, int *received, int *total ) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived( void ) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void		GetPacketResponseLatency( int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke ) const = 0;
	virtual void		GetRemoteFramerate( float *pflFrameTime, float *pflFrameTimeStdDeviation ) const = 0;

	virtual float		GetTimeoutSeconds( ) const = 0;
};

class INetChannel {
public:
	char pad_0000[ 20 ]; //0x0000
	bool m_bProcessingMessages; //0x0014
	bool m_bShouldDelete; //0x0015
	char pad_0016[ 2 ]; //0x0016
	int32_t m_nOutSequenceNr; //0x0018 last send outgoing sequence number
	int32_t m_nInSequenceNr; //0x001C last received incoming sequnec number
	int32_t m_nOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int32_t m_nOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int32_t m_nInReliableState; //0x0028 state of incoming reliable data
	int32_t m_nChokedPackets; //0x002C number of choked packets
	char pad_0030[ 1044 ]; //0x0030

	bool transmit( bool only_reliable ) {
		return util::get_vfunc< 49, bool >( this, only_reliable );
	}
}; //Size: 0x0444

struct INetMessage {
	virtual ~INetMessage( );
};

template<typename T>
class CNetMessagePB : public INetMessage, public T {};

class CCLCMsg_Move_ {
private:
	char __PAD0[ 0x8 ];
public:
	int numBackupCommands;
	int numNewCommands;
};

using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move_>;

class CClientState {
public:
	char pad0[ 0x9C ];
	INetChannel* netchannel;
	char pad1[ 0xD4 ];
	int32_t m_nDeltaTick; //0x174
	char pad2[ 0x4B34 ];
	float m_frameTime;
	int32_t lastoutgoingcommand;
	int32_t chokedcommands;
	int32_t last_command_ack;
};

class CGlobalState {
public:
	uintptr_t * m_manager;
	CClientState *m_def_clientstate;

	__forceinline CClientState* get_client_state( ) {
		return m_def_clientstate;
	}
};

class IVEngineClient {
public:
	void GetScreenSize( int& width, int& height ) {
		return call_vfunc< void( __thiscall* )( void*, int&, int& ) >( this, 5 )( this, width, height );
	}

	void ClientCmd( const char* command ) {
		return call_vfunc< void( __thiscall* )( void*, const char* ) >( this, 7 )( this, command );
	}

	bool GetPlayerInfo( int index, player_info_t* info ) {
		return call_vfunc< bool( __thiscall* )( void*, int, player_info_t* ) >( this, 8 )( this, index, info );
	}

	int GetPlayerForUserID( int UserID ) {
		using fn = int( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 9 )( this, UserID );
	}

	bool Cov_IsVisible( ) {
		return call_vfunc< bool( __thiscall* )( void* ) >( this, 11 )( this );
	}

	int GetLocalPlayer( ) {
		return call_vfunc< int( __thiscall* )( void* ) >( this, 12 )( this );
	}

	float Time( ) {
		return call_vfunc< float( __thiscall* )( void* ) >( this, 14 )( this );
	}

	void GetViewAngles( vec3_t& angles ) {
		return call_vfunc< void( __thiscall* )( void*, vec3_t& ) >( this, 19 )( this, angles );
	}

	void SetViewAngles( vec3_t& angles ) {
		return call_vfunc< void( __thiscall* )( void*, vec3_t& ) >( this, 20 )( this, angles );
	}

	const matrix3x4& GetWorldToScreenMatrix( ) {
		return call_vfunc< const matrix3x4&( __thiscall* )( void* ) >( this, 36 )( this );
	}

	int GetMaxClients( ) {
		return call_vfunc< int( __thiscall* )( void* ) >( this, 21 )( this );
	}

	bool IsInGame( ) {
		return call_vfunc< bool( __thiscall* )( void* ) >( this, 26 )( this );
	}

	bool IsConnected( ) {
		return call_vfunc< bool( __thiscall* )( void* ) >( this, 27 )( this );
	}

	void ClientCmd_Unrestricted( const char* command ) {
		return call_vfunc< void( __thiscall* )( void*, const char* ) >( this, 106 )( this, command );
	}

	INetChannelInfo* GetNetChannelInfo( ) {
		using fn = INetChannelInfo * ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 72 )( this );
	}
};