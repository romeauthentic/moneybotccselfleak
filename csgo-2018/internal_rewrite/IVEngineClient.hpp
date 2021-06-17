#pragma once
#include <string>
#include <algorithm>

#include "util.hpp"
#include "vector.hpp"

using matrix3x4 = float[ 3 ][ 4 ];

typedef struct player_info_s {
private:
	uint32_t __pad0[ 2 ];
public:
	unsigned int m_xuidlow;
	unsigned int m_xuidhigh;
	char name[ 128 ];//32
	int m_userid;
	char m_steamid[ 33 ];//32
	unsigned m_steam3id;
	char m_friendsname[ 128 ];
	bool m_isfakeplayer;
	bool m_ishltv;
	uint32_t m_customfiles[ 4 ];
	uint8_t m_filesdownloaded;
private:
	int __pad1;
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
	float m_fClearTime;
	float m_TimeOut;
	char m_Name[ 32 ];


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
	char pad_0000[ 0x9c ]; //0x0000
	class INetChannel* m_netchannel; //0x0094
	char pad_0098[ 16 ]; //0x0098
	unsigned int m_retrynum; //0x00A8
	char pad_00AC[ 84 ]; //0x00AC
	int m_signon_state; //0x0100
	char pad_0104[ 8 ]; //0x0104
	float m_next_cmd_time; //0x010C
	int m_servercount; //0x0110
	int m_current_sequence; //0x0114
	char pad_0118[ 75 ]; //0x0118
	int m_server_tick; //0x0163
	int m_client_tick; //0x0167
	int m_delta_tick; //0x016B
	char pad_016F[ 4 ]; //0x016F
	int m_view_entity; //0x0173
	char pad_0177[ 8 ]; //0x0177
	char m_level_name[ 260 ]; //0x017F
	char m_level_name_short[ 40 ]; //0x0283
	char pad_02AB[ 18932 ]; //0x02AB
	char pad_0x2DA0; //0x4CA7
	int m_lastoutgoingcommand; //0x4CA8
	int m_chokedcommands; //0x4CAC
	int m_last_acknowledged_cmd; //0x4CB0
	int m_command_ack; //0x4CB4
	int m_sound_seq; //0x4CB8
	char pad_4CBC[ 8 ]; //0x4CBC
	bool m_ishltv; //0x4CC4
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
		return util::get_vfunc< fn >( this, 18 )( this, ang );
	}

	vec3_t GetViewAngles( ) {
		vec3_t ret;
		GetViewAngles( ret );
		return ret;
	}

	void SetViewAngles( vec3_t& ang ) {
		using fn = void( __thiscall* )( void*, vec3_t& );
		return util::get_vfunc< fn >( this, 19 )( this, ang );
	}

	int GetMaxClients( ) {
		using fn = int( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 20 )( this );
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

	const matrix3x4& WorldToScreenMatrix( ) {
		using fn = const matrix3x4& ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 37 )( this );
	}

	const char* GetLevelName( ) {
		using fn = const char* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 52 )( this );
	}

	INetChannelInfo* GetNetChannelInfo( ) {
		using fn = INetChannelInfo* ( __thiscall* )( void* );
		return util::get_vfunc< fn >( this, 78 )( this );
	}
};