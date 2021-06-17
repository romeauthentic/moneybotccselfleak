#pragma once
#include "util.hpp"
#include "IClientEntityList.hpp"
#include "netvars.hpp"
#include "pattern.hpp"
#include "c_base_weapon.hpp"
#include "CUtlVector.hpp"
#include "settings.hpp"

enum PlayerState_t {
	FL_ONGROUND = 1 << 0,
	FL_DUCKING = 1 << 1,
	FL_WATERJUMP = 1 << 2,
	FL_ONTRAIN = 1 << 3,
	FL_INRAIN = 1 << 4,
	FL_FROZEN = 1 << 5,
	FL_ATCONTROLS = 1 << 6,
	FL_CLIENT = 1 << 7,
	FL_FAKECLIENT = 1 << 8,
	FL_INWATER = 1 << 9,
};

enum MoveType_t {
	MOVETYPE_NONE = 0, // never moves
	MOVETYPE_ISOMETRIC, // For players -- in TF2 commander view, etc.
	MOVETYPE_WALK, // Player only - moving on the ground
	MOVETYPE_STEP, // gravity, special edge handling -- monsters use this
	MOVETYPE_FLY, // No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY, // flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS, // uses VPHYSICS for simulation
	MOVETYPE_PUSH, // no clip to world, push and crush
	MOVETYPE_NOCLIP, // No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER, // Used by players only when going onto a ladder
	MOVETYPE_OBSERVER, // Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM, // Allows the entity to describe its own physics					
	MOVETYPE_LAST = MOVETYPE_CUSTOM, // should always be defined as the last item in the list
	MOVETYPE_MAX_BITS = 4,
};

enum LifeState_t {
	LIFE_ALIVE = 0, // alive
	LIFE_DYING, // playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD, // dead. lying still.
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

class fire_bullets_info
{
public:
	int32_t shots; //0x0000
	vec3_t src; //0x0004
	vec3_t dir; //0x0010
	vec3_t spread; //0x001C
	float distance; //0x0028
	int32_t ammo_type; //0x002C
	int32_t tracer_freq; //0x0030
	float damage; //0x0034
	int32_t player_damager; //0x0038
	int32_t flags; //0x003C
	char pad_0040[ 18 ]; //0x0040
}; //Size: 0x0054



struct clientanimating_t {
	void*			m_pAnimating;
	unsigned int	m_fFlags;
	clientanimating_t( void* _pAnim, unsigned int _flags ) : m_pAnimating( _pAnim ), m_fFlags( _flags ) {}
};



class VarMapEntry_t {
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void* data;
	void* watcher;
};

struct VarMapping_t {
	CUtlVector< VarMapEntry_t > m_Entries;
	int m_nInterpolatedEntries; // +0x14
	float m_lastInterpolationTime; // +0x18
};

class c_base_player {
private:

public:
	//NETVAR( m_YourFunctionName, "Name", "Table", extra, type )

	NETVAR( m_nMoveType, "m_Collision", "DT_BaseEntity", -0x24, MoveType_t );
	NETVAR( m_iHealth, "m_iHealth", "DT_BaseEntity", 0, int );
	NETVAR( m_lifeState, "m_lifeState", "DT_BasePlayer", 0, uint8_t );
	NETVAR( m_nTickBase, "m_nTickBase", "DT_BasePlayer", 0, int );
	NETVAR( m_fFlags, "m_fFlags", "DT_BasePlayer", 0, int );
	//NETVAR( m_vecVelocity, "m_vecVelocity[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_nHitboxSet, "m_nHitboxSet", "DT_BaseAnimating", 0, int );
	NETVAR( m_vecOrigin, "m_vecOrigin", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_vecViewOffset, "m_vecViewOffset[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_hActiveWeapon, "m_hActiveWeapon", "DT_BaseCombatCharacter", 0, ulong_t );
	NETVAR( m_iTeamNum, "m_iTeamNum", "DT_BaseEntity", 0, int );
	NETVAR( m_dwBoneMatrix, "m_nForceBone", "DT_BaseAnimating", 28, uintptr_t );
	NETVAR( m_bHasHelmet, "m_bHasHelmet", "DT_CSPlayer", 0, bool );
	NETVAR( m_ArmorValue, "m_ArmorValue", "DT_CSPlayer", 0, int );
	NETVAR( m_flNextAttack, "m_flNextAttack", "DT_BaseCombatCharacter", 0, float );
	NETVAR( m_angEyeAngles, "m_angEyeAngles[0]", "DT_CSPlayer", 0, vec3_t );
	NETVAR( m_AnimOverlay, "m_hLightingOrigin", "DT_BaseAnimating", 0x3c, int );
	NETVAR( m_flSimulationTime, "m_flSimulationTime", "DT_BaseEntity", 0, float );
	NETVAR( m_flCycle, "m_flCycle", "DT_BaseAnimating", 0, float );
	NETVAR( m_nSequence, "m_nSequence", "DT_BaseAnimating", 0, int );
	PNETVAR( m_flPoseParameter, "m_flPoseParameter", "DT_BaseAnimating", 0, float );
	NETVAR( m_bClientSideAnimation, "m_bClientSideAnimation", "DT_BaseAnimating", 0, bool );
	NETVAR( m_flLastBoneSetupTime, "m_hLightingOrigin", "DT_BaseAnimating", -0x20, float );
	NETVAR( m_iMostRecentModelBoneCounter, "m_nForceBone", "DT_BaseAnimating", 0x4, int );
	NETVAR( m_nWriteableBones, "m_nForceBone", "DT_BaseAnimating", 0x20, int );
	NETVAR( m_vecThirdpersonAngles, "deadflag", "DT_BasePlayer", 0x4, vec3_t );
	NETVAR( m_hObserverTarget, "m_hObserverTarget", "DT_BasePlayer", 0, uint32_t );
	NETVAR( m_iObserverMode, "m_iObserverMode", "DT_BasePlayer", 0, int );
	NETVAR( m_csgoAnimState, "m_bIsScoped", "DT_CSPlayer", -0xe, void* );
	NETVAR( m_flOldSimulationTime, "m_flSimulationTime", "DT_BaseEntity", 0x4, float );
	NETVAR( m_CoordinateFrame, "m_CollisionGroup", "DT_BaseEntity", -0x4C, matrix3x4 );
	NETVAR( m_vecMins, "m_vecMins", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_vecMaxs, "m_vecMaxs", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_bIsScoped, "m_bIsScoped", "DT_CSPlayer", 0, bool );
	NETVAR( m_viewPunchAngle, "m_viewPunchAngle", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_flMaxFlashAlpha, "m_flFlashMaxAlpha", "DT_CSPlayer", 0, float );
	NETVAR( m_flFlashDuration, "m_flFlashDuration", "DT_CSPlayer", 0, float );
	NETVAR( m_flFlashTime, "m_flFlashMaxAlpha", "DT_CSPlayer", -0x10, float );
	NETVAR( m_bGunGameImmunity, "m_bGunGameImmunity", "DT_CSPlayer", 0, bool );
	NETVAR( m_hViewModel, "m_hViewModel[0]", "DT_BasePlayer", 0, uint32_t );
	NETVAR( m_bSpotted, "m_bSpotted", "DT_CSPlayer", 0, bool );
	NETVAR( m_clrRender, "m_clrRender", "DT_BaseEntity", 0, clr_t );
	NETVAR( m_vecPunchAngle, "m_vecPunchAngle", "DT_BasePlayer", 0, vec3_t );

	OFFSET( m_CachedBoneData, 0x28FC + sizeof( void* ), CUtlVector< matrix3x4 > );
	OFFSET( m_flSpawnTime, 0xa290, float );
	OFFSET( m_vecVelocity, 0xf4, vec3_t );

public:
	static uintptr_t get_player_resource( );

	/*makes it much more organized: functions from IClientEntity
	dont interfere with our defined functions*/
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

	__forceinline bool is_alive( ) {
		return m_iHealth( ) > 0;
	}

	bool is_flashed( );
	bool is_reloading( );

	__forceinline auto& m_aimPunchAngle( bool real = false ) {
		static vec3_t backup_var{ };
		static auto offset = g_netvars.get_netvar( fnv( "DT_BasePlayer" ), fnv( "m_aimPunchAngle" ) );
		if( g_settings.misc.no_recoil( ) && !real ) {
			backup_var = { 0.f, 0.f, 0.f };
			return backup_var;
		}

		return get< vec3_t >( offset );
	}

	__forceinline void* get_animstate( ) {
		static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( "DT_CSPlayer" ),
			fnv( "m_bIsScoped" ) );

		if( !offset ) {
			return nullptr;
		}

		return *reinterpret_cast< void** >( uintptr_t( this ) + offset - 0xa );
	}

	__forceinline void disable_interpolaton( ) {
		VarMapping_t* varmapping = ( VarMapping_t* )( uintptr_t( this ) + 0x24 );

		for( int i{ }; i < varmapping->m_nInterpolatedEntries; ++i ) {
			if( !varmapping->m_Entries.GetElements( ) ) {
				continue;
			}

			auto e = &varmapping->m_Entries.GetElements( )[ i ];

			if( e )
				e->m_bNeedsToInterpolate = false;
		}

		//this is the only thing that actually disables interpolation
		//to re-enable it, u want to backup and restore m_nInterpolatedEntries
		//theres no need to re-enable it because the game will do it for you
		//as soon as you stop forcing it off
		varmapping->m_nInterpolatedEntries = 0;
		varmapping->m_lastInterpolationTime = 0.f;
	}

	__forceinline void update_clientside_animation( ) {
		bool backup = m_bClientSideAnimation( );
		m_bClientSideAnimation( ) = true;
		util::get_vfunc< 218, void >( this );
		m_bClientSideAnimation( ) = backup;
	}

	__forceinline void invalidate_bone_cache( ) {
		m_flLastBoneSetupTime( ) = -FLT_MAX;
		m_iMostRecentModelBoneCounter( ) -= 1;
		m_nWriteableBones( ) = 0;
	}

	__forceinline vec3_t get_eye_pos( ) {
		vec3_t yes;
		util::get_vfunc< void( __thiscall* )( void*, vec3_t& ) >( this, 286 )( this, yes );
		return yes;
	}

	__forceinline bool is_valid( ) {
		return !ce( )->IsDormant( ) && is_alive( );
	}

	c_base_player* get_observer_target( );
	const char* get_rank( );
	clr_t get_team_color( );
	bool is_fakewalking( );
	player_info_t get_info( );
	void get_name_safe( char* buf );
	c_base_weapon* get_weapon( );
	vec3_t get_hitbox_pos( int hitbox );
	bool is_visible( int hitbox );
	bool is_visible( const vec3_t& pos );
	bool can_attack( bool ignore_rapid = false );
	int	 get_choked_ticks( );
	float get_hitbox_radius( int hitbox );

	void set_abs_origin( vec3_t origin );
	void set_abs_angles( vec3_t angles );
	void enforce_animations( );

	void create_animstate( void* state );
	int get_seq_activity( int sequence );

	void fix_animations( void* saved_state = nullptr );
	bool is_breaking_lc( );
};