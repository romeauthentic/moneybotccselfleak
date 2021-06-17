#pragma once
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
	int		m_cmd_nr; // 0x0 virtual deconstructor removed
	int		m_tick_count; // 0x4
	vec3_t	m_viewangles; // 0x8
	float	m_forwardmove; // 0x14
	float	m_sidemove; // 0x18
	float	m_upmove; // 0x1C
	int		m_buttons; // 0x20
	uint8_t m_impulse; // 0x24
	int		m_weaponselect; // 0x25
	int		m_weaponsubtype; // 0x29
	int		m_random_seed; // 0x2D
	short	m_mousedx; // 0x31
	short	m_mousedy; // 0x33
	bool	m_predicted; // 0x35
private:
	uint8_t m_pad_0x36[ 0x6 ]; // 0x36
public:
	bool	m_world_clicking; // 0x3F
	vec3_t	m_world_click_direction; // 0x40


	__forceinline user_cmd_t clamp( ) {
		m_viewangles.clamp( );
		
		m_forwardmove = std::clamp( m_forwardmove, -10000.f, 10000.f );
		m_sidemove = std::clamp( m_sidemove, -10000.f, 10000.f );
		m_upmove = std::clamp( m_upmove, -10000.f, 10000.f );

		return *this;
	}

private:
	uint8_t m_pad_0x52[ 0xF2 ]; // 0x52 sizeof( user_cmd_t ) = 0x144 HUGE in gmod
};

class CHudChat {
public:
	void ChatPrintf( int iPlayerIndex, int iFilter, const char* fmt ) {
		using fn = void( __cdecl* )( void*, int, int, const char* );
		util::get_vfunc< fn >( this, 26 )( this, iPlayerIndex, iFilter, fmt );
	}
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};


class CViewSetup
{
public:
	int			x;
	int			m_nUnscaledX;
	int			y;
	int			m_nUnscaledY;
	int			width;
	int			m_nUnscaledWidth;
	int			height;
	StereoEye_t m_eStereoEye;
	int			m_nUnscaledHeight;

	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;

	float		fov;
	float		fovViewmodel;

	vec3_t		origin;

	vec3_t		angles;
	float		zNear;
	float		zFar;

	float		zNearViewmodel;
	float		zFarViewmodel;

	bool		m_bRenderToSubrectOfLargerScreen;
	float		m_flAspectRatio;

	bool		m_bOffCenter;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;

	bool		m_bDoBloomAndToneMapping;
	bool		m_bCacheFullSceneState;
	bool		m_bViewToProjectionOverride;
};


class IClientMode {
public:
	char _pad[ 28 ];

	CHudChat* m_pChatElement; //0x2A
};