#pragma once

#include "vector.hpp"
#include "IClientMode.hpp"

#define MULTIPLAYER_BACKUP 150

struct verified_cmd_t {
	user_cmd_t m_cmd;
	CRC32_t	   m_crc;
};

class CInput {
	void* pvftable; //0x00
public:
	bool m_fTrackIRAvailable; //0x04
	bool m_fMouseInitialized; //0x05
	bool m_fMouseActive; //0x06
	bool m_fJoystickAdvancedInit; //0x07
private:
	char pad_0x08[ 0x2C ]; //0x08
public:
	void* m_pKeys; //0x34
private:
	char pad_0x38[ 0x64 ]; //0x38
	int pad_0x41;
	int pad_0x42;
public:
	bool m_fCameraInterceptingMouse; //0x9C
	bool m_fCameraInThirdPerson; //0x9D
	bool m_fCameraMovingWithMouse; //0x9E
	vec3_t m_vecCameraOffset; //0xA0
	bool m_fCameraDistanceMove; //0xAC
	int m_nCameraOldX; //0xB0
	int m_nCameraOldY; //0xB4
	int m_nCameraX; //0xB8
	int m_nCameraY; //0xBC
	bool m_CameraIsOrthographic; //0xC0
	vec3_t m_angPreviousViewAngles; //0xC4
	vec3_t m_angPreviousViewAnglesTilt; //0xD0
	float m_flLastForwardMove; //0xDC
	int m_nClearInputState; //0xE0
public:
	user_cmd_t* m_pCommands; //0xEC
	verified_cmd_t* m_pVerifiedCommands; //0xF0
	int m_hSelectedWeapon;

	user_cmd_t* GetUserCmd( int slot, int seq_num ) {
		return util::get_vfunc< 8, user_cmd_t* >( this, slot, seq_num );
	}

	void CreateMove( int sequence_number, float input_sample_time, bool active ) {
		return util::get_vfunc< 3, void >( this, sequence_number, input_sample_time, active );
	}
};
