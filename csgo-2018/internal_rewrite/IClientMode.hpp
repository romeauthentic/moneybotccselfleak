#pragma once
#include "valve/checksum_crc.h"

#include "util.hpp"
#include "vector.hpp"

enum player_controls_t {
	IN_ATTACK = 1 << 0,
	IN_JUMP = 1 << 1,
	IN_DUCK = 1 << 2,
	IN_FORWARD = 1 << 3,
	IN_BACK = 1 << 4,
	IN_USE = 1 << 5,
	IN_CANCEL = 1 << 6,
	IN_LEFT = 1 << 7,
	IN_RIGHT = 1 << 8,
	IN_MOVELEFT = 1 << 9,
	IN_MOVERIGHT = 1 << 10,
	IN_ATTACK2 = 1 << 11,
	IN_RUN = 1 << 12,
	IN_RELOAD = 1 << 13,
	IN_ALT1 = 1 << 14,
	IN_ALT2 = 1 << 15,
	IN_SCORE = 1 << 16,
	IN_SPEED = 1 << 17,
	IN_WALK = 1 << 18,
	IN_ZOOM = 1 << 19,
	IN_WEAPON1 = 1 << 20,
	IN_WEAPON2 = 1 << 21,
	IN_BULLRUSH = 1 << 22,
	IN_GRENADE1 = 1 << 23,
	IN_GRENADE2 = 1 << 24,
	IN_ATTACK3 = 1 << 25,
};

class user_cmd_t {
public:
	virtual ~user_cmd_t( ) { };

	int		m_cmd_nr; // 0x04 For matching server and client commands for debugging
	int		m_tick_count; // 0x08 the tick the client created this command
	vec3_t	m_viewangles; // 0x0C Player instantaneous view angles.
	vec3_t	m_aimdirection; // 0x18
	float	m_forwardmove; // 0x24
	float	m_sidemove; // 0x28
	float	m_upmove; // 0x2C
	int		m_buttons; // 0x30 Attack button states
	uint8_t m_impulse; // 0x34
	int		m_weaponselect; // 0x38 Current weapon id
	int		m_weaponsubtype; // 0x3C
	int		m_random_seed; // 0x40 For shared random functions
	short	m_mousedx; // 0x44 mouse accum in x from create move
	short	m_mousedy; // 0x46 mouse accum in y from create move
	bool	m_predicted; // 0x48 Client only, tracks whether we've predicted this command at least once
	vec3_t  headangles; // 0x49
	vec3_t	headoffset; // 0x55

	__forceinline user_cmd_t clamp( bool angles = true ) {
		if( angles )
			m_viewangles.clamp( );
		
		m_forwardmove = std::clamp( m_forwardmove, -450.f, 450.f );
		m_sidemove = std::clamp( m_sidemove, -450.f, 450.f );
		m_upmove = std::clamp( m_upmove, -450.f, 450.f );

		return *this;
	}

	CRC32_t get_check_sum( ) {
		CRC32_t crc;
		CRC32_Init( &crc );

		CRC32_ProcessBuffer( &crc, &m_cmd_nr, sizeof( m_cmd_nr ) );
		CRC32_ProcessBuffer( &crc, &m_tick_count, sizeof( m_tick_count ) );
		CRC32_ProcessBuffer( &crc, &m_viewangles, sizeof( m_viewangles ) );
		CRC32_ProcessBuffer( &crc, &m_aimdirection, sizeof( m_aimdirection ) );
		CRC32_ProcessBuffer( &crc, &m_forwardmove, sizeof( m_forwardmove ) );
		CRC32_ProcessBuffer( &crc, &m_sidemove, sizeof( m_sidemove ) );
		CRC32_ProcessBuffer( &crc, &m_upmove, sizeof( m_upmove ) );
		CRC32_ProcessBuffer( &crc, &m_buttons, sizeof( m_buttons ) );
		CRC32_ProcessBuffer( &crc, &m_impulse, sizeof( m_impulse ) );
		CRC32_ProcessBuffer( &crc, &m_weaponselect, sizeof( m_weaponselect ) );
		CRC32_ProcessBuffer( &crc, &m_weaponsubtype, sizeof( m_weaponsubtype ) );
		CRC32_ProcessBuffer( &crc, &m_random_seed, sizeof( m_random_seed ) );
		CRC32_ProcessBuffer( &crc, &m_mousedx, sizeof( m_mousedx ) );
		CRC32_ProcessBuffer( &crc, &m_mousedy, sizeof( m_mousedy ) );

		CRC32_Final( &crc );
		return crc;
	}

private:
	
	//char pad_0x4C[ 0x18 ]; // 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

class CHudChat {
public:
	void ChatPrintf( int iPlayerIndex, int iFilter, const char* fmt ) {
		using fn = void( __cdecl* )( void*, int, int, const char* );
		util::get_vfunc< fn >( this, 26 )( this, iPlayerIndex, iFilter, fmt );
	}
};

class CViewSetup {
public:
	int m_x;
	int m_oldX;
	int m_y;
	int m_oldY;
	int m_width;
	int m_oldWidth;
	int m_height;
	int m_oldHeight;

	bool m_bOrtho;
	float m_OrthoLeft;
	float m_OrthoTop;
	float m_OrthoRight;
	float m_OrthoBottom;

private:
	char pad1[ 0x7C ];

public:
	float m_flFov;
	float m_flFovViewmodel;
	vec3_t m_vecOrigin;
	vec3_t m_vecAngles;

	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;

	float m_flAspectRatio;
	float m_flNearBlurDepth;
	float m_flNearFocusDepth;
	float m_flFarFocusDepth;
	float m_flFarBlurDepth;
	float m_flNearBlurRadius;
	float m_flFarBlurRadius;
	int m_nDoFQuality;
	int m_nMotionBlurMode;

	float m_flShutterTime;
	vec3_t m_vShutterOpenPosition;
	vec3_t m_shutterOpenAngles;
	vec3_t m_vShutterClosePosition;
	vec3_t m_shutterCloseAngles;

	float m_flOffCenterTop;
	float m_flOffCenterBottom;
	float m_flOffCenterLeft;
	float m_flOffCenterRight;

	bool m_bOffCenter : 1;
	bool m_bRenderToSubrectOfLargerScreen : 1;
	bool m_bDoBloomAndToneMapping : 1;
	bool m_bDoDepthOfField : 1;
	bool m_bHDRTarget : 1;
	bool m_bDrawWorldNormal : 1;
	bool m_bCullFrontFaces : 1;
	bool m_bCacheFullSceneState : 1;
	bool m_bRenderFlashlightDepthTranslucents : 1;
private:
	char pad2[ 0x40 ];
};

class IClientMode {
public:
	char _pad[ 28 ];

	CHudChat* m_pChatElement; //0x2A
};