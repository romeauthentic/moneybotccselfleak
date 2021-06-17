#pragma once
#include "util.hpp"
#include "IClientEntityList.hpp"
#include "netvars.hpp"

enum ItemDefinitionIndex_t {
	WEAPON_NONE,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFEGG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_R8REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};


class weapon_info_t {
public:
	char pad_0000[4]; //0x0000
	char* weapon_name; //0x0004
	char pad_0008[12]; //0x0008
	int max_clip_ammo; //0x0014
	char pad_0018[12]; //0x0018
	int max_ammo; //0x0024
	char pad_0028[4]; //0x0028
	char* world_model; //0x002C
	char* view_model; //0x0030
	char* dropped_model; //0x0034
	char pad_0038[4]; //0x0038
	char* N00000010; //0x003C
	char pad_0040[56]; //0x0040
	char* empty_sound; //0x0078
	char pad_007C[4]; //0x007C
	char* ammo_type; //0x0080
	char pad_0084[4]; //0x0084
	char* hud_name; //0x0088
	char* weapon_name2; //0x008C
	char pad_0090[60]; //0x0090
	int price; //0x00CC
	int kill_reward; //0x00D0
	char* anim_prefix;
	float cycle_time;
	float cycle_time_alt;
	float time_to_idle;
	float idle_interval;

	bool full_auto; //0x00E8
	char pad_00E9[3]; //0x00E9
	int damage; //0x00EC
	float armor_ratio; //0x00F0
	char pad_00F4[4]; //0x00F4
	float penetration; //0x00F8
	char pad_00FC[8]; //0x00FC
	float range; //0x0104
	float range_modifier; //0x0108
	float throw_velocity;
	char pad_010C[12]; //0x010C
	bool has_silencer; //0x011C
	char pad_011D[15]; //0x011D
	float max_speed; //0x012C
	float max_speed_alt; //0x0130
	float spread; //0x0134
	float spread_alt; //0x0138
	float inaccuracy_crouch; //0x013C
	float inaccuracy_crouch_alt; //0x0140
	float inaccuracy_stand; //0x0144
	float inaccuracy_stand_alt; //0x0148
	float inaccuracy_jump_start; //0x014C
	float inaccuracy_jump; //0x0150
	float inaccuracy_jump_alt; //0x0154
	float inaccuracy_land; //0x0158
	float inaccuracy_land_alt; //0x015C
	float inaccuracy_ladder; //0x0160
	float inaccuracy_ladder_alt; //0x0164
	float inaccuracy_fire; //0x0168
	float inaccuracy_fire_alt; //0x016C
	float inaccuracy_move; //0x0170
	float inaccuracy_move_alt; //0x0174
	float inaccuracy_reload; //0x0178
	int recoil_seed; //0x017C
	char pad_0180[60]; //0x0180
	int zoom_levels; //0x01BC
	int zoom_fov1; //0x01C0
	int zoom_fov2; //0x01C4
};

enum quality_t {
	Default,
	Geniune,
	Vintage,
	Unusual,
	Community = 5,
	Developer,
	Self_Made,
	Customized,
	Strange,
	Completed,
	Tournament = 12,
};

class c_base_weapon {
public:
	NETVAR( m_iItemDefinitionIndex, "m_iItemDefinitionIndex", "DT_BaseAttributableItem", 0, short );
	NETVAR( m_iClip1, "m_iClip1", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_flNextPrimaryAttack, "m_flNextPrimaryAttack", "DT_BaseCombatWeapon", 0, float );
	NETVAR( m_flNextSecondaryAttack, "m_flNextPrimaryAttack", "DT_BaseCombatWeapon", 4, float );
	NETVAR( m_flPostponeFireReadyTime, "m_flPostponeFireReadyTime", "DT_WeaponCSBase", 0, float );
	NETVAR( m_fThrowTime, "m_fThrowTime", "DT_BaseCSGrenade", 0, float );
	NETVAR( m_hOwner, "m_hOwner", "DT_BaseCombatWeapon", 0, uint32_t );
	NETVAR( m_bPinPulled, "m_bPinPulled", "DT_BaseCSGrenade", 0, bool );
	NETVAR( m_flThrowStrength, "m_flThrowStrength", "DT_BaseCSGrenade", 0, float );
	NETVAR( m_nModelIndex, "m_nModelIndex", "DT_BaseEntity", 0, int );

	NETVAR( m_iViewModelIndex, "m_iViewModelIndex", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_iWorldModelIndex, "m_iWorldModelIndex", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_iWorldDroppedModelIndex, "m_iWorldDroppedModelIndex", "DT_BaseCombatWeapon", 0, int );
	NETVAR( m_hWeaponWorldModel, "m_hWeaponWorldModel", "DT_BaseCombatWeapon", 0, uint32_t );

	NETVAR( m_iEntityQuality, "m_iEntityQuality", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_iItemIDHigh, "m_iItemIDHigh", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_iItemIDLow, "m_iItemIDLow", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_iAccountID, "m_iAccountID", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_szCustomName, "m_szCustomName", "DT_BaseAttributableItem", 0, char* );
	NETVAR( m_OriginalOwnerXuidLow, "m_OriginalOwnerXuidLow", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_OriginalOwnerXuidHigh, "m_OriginalOwnerXuidHigh", "DT_BaseAttributableItem", 0, int );

	NETVAR( m_nFallbackPaintKit, "m_nFallbackPaintKit", "DT_BaseAttributableItem" ,0, int );
	NETVAR( m_nFallbackSeed, "m_nFallbackSeed", "DT_BaseAttributableItem", 0, int );
	NETVAR( m_flFallbackWear, "m_flFallbackWear", "DT_BaseAttributableItem", 0, float );
	NETVAR( m_nFallbackStatTrak, "m_nFallbackStatTrak", "DT_BaseAttributableItem", 0, int );

	NETVAR( m_hOwnerEntity, "m_hOwnerEntity", "DT_BaseAttributableItem", 0, uint32_t );
	NETVAR( m_flC4Blow, "m_flC4Blow", "DT_PlantedC4", 0, float );
	NETVAR( m_bBombTicking, "m_bBombTicking", "DT_PlantedC4", 0, bool );
	NETVAR( m_hBombDefuser, "m_hBombDefuser", "DT_PlantedC4", 0, uint32_t );
	NETVAR( m_bBombDefused, "m_bBombDefused", "DT_PlantedC4", 0, bool );
	NETVAR( m_flTimerLength, "m_flTimerLength", "DT_PlantedC4", 0, float );
	NETVAR( m_flDefuseLength, "m_flDefuseLength", "DT_PlantedC4", 0, float );
	NETVAR( m_flDefuseCountDown, "m_flDefuseCountDown", "DT_PlantedC4", 0, float );
	NETVAR( m_fLastShotTime, "m_fLastShotTime", "DT_WeaponCSBaseGun", 0, float );

	__forceinline IClientEntity* ce( ) {
		return reinterpret_cast< IClientEntity* >( this );
	}

	__forceinline operator IClientEntity*( ) {
		return reinterpret_cast< IClientEntity* >( this );
	}

	template < typename t >
	__forceinline t& get( std::ptrdiff_t offset ) {
		return *reinterpret_cast< t* >( uintptr_t( this ) + offset );
	}

	__forceinline weapon_info_t* get_wpn_info() {
		return util::get_vfunc< 446, weapon_info_t* >(this);
	}

	__forceinline void update_accuracy_penalty() {
		return util::get_vfunc< 470, void >(this);
	}

	__forceinline float get_spread() {
		return util::get_vfunc< 439, float >(this);
	}

	__forceinline float get_inaccuracy() {
		return util::get_vfunc< 469, float >(this);
	}

	__forceinline void set_glove_model( int model_index ) {
		return util::get_vfunc< 75, void >( this, model_index );
	}

	bool is_pistol( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_DEAGLE:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_HKP2000:
		case WEAPON_P250:
		case WEAPON_TEC9:
		case WEAPON_USP_SILENCER:
		case WEAPON_R8REVOLVER:
			return true;
		default:
			return false;
		}
	}

	bool is_sniper( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_AWP:
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
		case WEAPON_SSG08:
			return true;
		default:
			return false;
		}
	}

	bool is_knife( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_T:
		case WEAPON_KNIFEGG:
		case WEAPON_KNIFE_BAYONET:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_PUSH:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_BOWIE:
			return true;
		default:
			return false;
		}
	}

	bool is_grenade( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_FLASHBANG:
		case WEAPON_HEGRENADE:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_MOLOTOV:
		case WEAPON_INCGRENADE:
		case WEAPON_DECOY:
			return true;
		default:
			return false;
		}
	}

	bool is_shotgun( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_NOVA:
		case WEAPON_SAWEDOFF:
		case WEAPON_XM1014:
		case WEAPON_MAG7:
			return true;
		default:
			return false;
		}
	}

	bool is_rifle( ) {
		switch( m_iItemDefinitionIndex( ) ) {
		case WEAPON_AK47:
		case WEAPON_AUG:
		case WEAPON_FAMAS:
		case WEAPON_GALILAR:
		case WEAPON_SG556:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
			return true;
		default:
			return false;
		}
	}

	char get_hud_icon( );
};