#pragma once
#include <string>
#include <algorithm>

#include "util.hpp"
#include "vector.hpp"

//using matrix3x4 = float[ 3 ][ 4 ];

struct VMatrix
{
	float m_matrix[ 4 ][ 4 ];

	float* operator[]( int i ) {
		return m_matrix[ i ];
	}

	const float* operator[]( int i ) const {
		return m_matrix[ i ];
	}
};

struct matrix3x4
{
	float m_matrix[ 3 ][ 4 ];

	matrix3x4( ) = default;

	matrix3x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 ) {
		m_matrix[ 0 ][ 0 ] = m00;	m_matrix[ 0 ][ 1 ] = m01; m_matrix[ 0 ][ 2 ] = m02; m_matrix[ 0 ][ 3 ] = m03;
		m_matrix[ 1 ][ 0 ] = m10;	m_matrix[ 1 ][ 1 ] = m11; m_matrix[ 1 ][ 2 ] = m12; m_matrix[ 1 ][ 3 ] = m13;
		m_matrix[ 2 ][ 0 ] = m20;	m_matrix[ 2 ][ 1 ] = m21; m_matrix[ 2 ][ 2 ] = m22; m_matrix[ 2 ][ 3 ] = m23;
	}

	float* operator[]( int i ) {
		return m_matrix[ i ];
	}

	const float* operator[]( int i ) const {
		return m_matrix[ i ];
	}
};


typedef struct player_info_s {
	char name[ 32 ];//32
	int m_userid;
	char m_steamid[ 33 ];//32
	unsigned m_friendId;
	char m_friendsname[ 32 ];
	bool m_isfakeplayer;
	bool m_ishltv;
	uint32_t m_customfiles[ 4 ];
	uint8_t m_filesdownloaded;
} player_info_t;

class INetChannelInfo {
public:
	enum
	{
		GENERIC = 0, // must be first and is default group
		LOCALPLAYER, // bytes for local player entity update
		OTHERPLAYERS, // bytes for other players update
		ENTITIES, // all other entity bytes
		SOUNDS, // game sounds
		EVENTS, // event messages
		TEMPENTS, // temp entities
		USERMESSAGES, // user messages
		ENTMESSAGES, // entity messages
		VOICE, // voice data
		STRINGTABLE, // a stringtable update
		MOVE, // client move cmds
		STRINGCMD, // string command
		SIGNON, // various signondata
		TOTAL, // must be last and is not a real group
	};

	virtual const char* GetName( ) const = 0; // get channel name
	virtual const char* GetAddress( ) const = 0; // get channel IP address as string
	virtual float GetTime( ) const = 0; // current net time
	virtual float GetTimeConnected( ) const = 0; // get connection time in seconds
	virtual int GetBufferSize( ) const = 0; // netchannel packet history size
	virtual int GetDataRate( ) const = 0; // send data rate in byte/sec

	virtual bool IsLoopback( ) const = 0; // true if loopback channel
	virtual bool IsTimingOut( ) const = 0; // true if timing out
	virtual bool IsPlayback( ) const = 0; // true if demo playback

	virtual float GetLatency( int flow ) const = 0; // current latency (RTT), more accurate but jittering
	virtual float GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float GetAvgLoss( int flow ) const = 0; // avg packet loss[0..1]
	virtual float GetAvgChoke( int flow ) const = 0; // avg packet choke[0..1]
	virtual float GetAvgData( int flow ) const = 0; // data flow in bytes/sec
	virtual float GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int GetTotalData( int flow ) const = 0; // total flow in/out in bytes
	virtual int GetTotalPackets( int flow ) const = 0;
	virtual int GetSequenceNr( int flow ) const = 0; // last send seq number
	virtual bool IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool GetStreamProgress( int flow, int* received, int* total ) const = 0; // TCP progress if transmitting
	virtual float GetTimeSinceLastReceived( ) const = 0; // get time since last recieved packet in seconds
	virtual float GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void GetRemoteFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const = 0;

	virtual float GetTimeoutSeconds( ) const = 0;
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

class CCLCMsg_Move {
private:
	char __PAD0[ 0x8 ];
public:
	int numBackupCommands;
	int numNewCommands;
};

using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;

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
	uintptr_t* m_manager;
	CClientState *m_def_clientstate;

	__forceinline CClientState* get_client_state( ) {
		return m_def_clientstate;
	}
};

class IVEngineClient
{
public:
	void GetScreenSize( int& width, int& height ) {
		using fn = void( __thiscall* )( void*, int&, int& );
		return util::get_vfunc< fn >( this, 5 )( this, width, height );
	}

	void ClientCmd( const char* szCommandString ) {
		std::string command( szCommandString );
		std::replace( command.begin( ), command.end( ), ';', ':' );
		std::replace( command.begin( ), command.end( ), '"', '*' );
		using fn = void( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 7 )( this, command.c_str( ) );
	}

	void ClientCmd_Unrestricted( const char* szCommandString ) {
		std::string command( szCommandString );
		std::replace( command.begin( ), command.end( ), ';', ':' );
		std::replace( command.begin( ), command.end( ), '"', '*' );
		using fn = void( __thiscall* )( void*, const char*, const char* );
		return util::get_vfunc< fn >( this, 114 )( this, command.c_str( ), nullptr );
	}

	bool GetPlayerInfo( int index, player_info_t* pInfo ) {
		using fn = bool( __thiscall* )( void*, int, player_info_t* );
		return util::get_vfunc< fn >( this, 8 )( this, index, pInfo );
	}

	int GetPlayerForUserID( int UserID ) {
		using fn = int( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 9 )( this, UserID );
	}

	int GetLocalPlayer( ) {
		using fn = int( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 12 )( this );
	}

	float Time( ) {
		using fn = float( __thiscall*  )( void* );
		return util::get_vfunc< fn >( this, 14 )( this );
	}

	void GetViewAngles( vec3_t& ang ) {
		using fn = void( __thiscall* )( void*, vec3_t& );
		return util::get_vfunc< fn >( this, 19 )( this, ang );
	}

	void SetViewAngles( vec3_t& ang ) {
		using fn = void( __thiscall* )( void*, vec3_t& );
		return util::get_vfunc< fn >( this, 20 )( this, ang );
	}

	int GetMaxClients( ) {
		using fn = int( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 21 )( this );
	}

	bool IsInGame( ) {
		using fn = bool( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 26 )( this );
	}

	bool IsConnected( ) {
		using fn = bool( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 27 )( this );
	}
	
	bool IsPaused( ) {
		return util::get_vfunc< 91, bool >( this );
	}

	const VMatrix& WorldToScreenMatrix( ) {
		using fn = const VMatrix& ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 36 )( this );
	}

	const char* GetLevelName( ) {
		using fn = const char* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 52 )( this );
	}

	INetChannelInfo* GetNetChannelInfo( ) {
		using fn = INetChannelInfo* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 72 )( this );
	}
};