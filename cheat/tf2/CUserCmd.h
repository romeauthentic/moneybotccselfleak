#pragma once
#include "vector.hpp"

enum PlayerControls_t {
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
	virtual ~user_cmd_t( ) {};
	int command_number; //0
	int tick_count; //4
	vec3_t viewangles; //16
	float forwardmove; //20
	float sidemove; //24
	float upmove; //28
	int	buttons; //32
	byte impulse; //33
	int weaponselect; //
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	int SetValue;
};