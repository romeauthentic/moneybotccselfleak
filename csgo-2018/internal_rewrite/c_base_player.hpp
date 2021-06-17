#pragma once
#include <array>

#include "util.hpp"
#include "IClientEntityList.hpp"
#include "netvars.hpp"
#include "pattern.hpp"
#include "c_base_weapon.hpp"
#include "CUtlVector.hpp"
#include "IPhysicsSurfaceProps.hpp"
#include "settings.hpp"
#include "IPhysicsSurfaceProps.hpp"

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

enum PlayerHitboxes_t {
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX,
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

enum PoseParam_t {
	STRAFE_YAW,
	STAND,
	LEAN_YAW,
	SPEED,
	LADDER_YAW,
	LADDER_SPEED,
	JUMP_FALL,
	MOVE_YAW,
	MOVE_BLEND_CROUCH,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	BODY_YAW,
	BODY_PITCH,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_COURCH_IDLE,
	AIM_BLEND_CROUCH_WALK,
	DEATH_YAW
};

class IKContext {
public:
	void Init( void* hdr, vec3_t& angles, vec3_t& origin, float curtime, int framecount, int boneMask );
	void UpdateTargets( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed );
	void SolveDependencies( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed );

	//THANK YOU IDA
	void ClearTargets( ) {
		int max = *( int* )( ( uintptr_t )this + 4080 );
		int count = 0;

		if( max > 0 ) {
			uintptr_t v60 = ( uintptr_t )( ( uintptr_t )this + 208 );
			do {
				*( int* )( v60 ) = -9999;
				v60 += 340;
				++count;
			} while( count < max );
		}
	}
};

class C_AnimationLayer {
private:
	char  pad_0x8_[ 0x8 ];
public:
	uint32_t m_unk1;
	uint32_t m_unk2;
	uint32_t m_unk3;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float m_flPrevCycle; //0x001C
	float m_flWeight; //0x0020
	float m_flWeightDeltaRate; //0x0024
	float m_flPlaybackRate; //0x0028
	float m_flCycle; //0x2C
	c_base_player* m_player; //0x30
	char pad_0x8[ 0x4 ]; //0x34
}; //Size: 0x0038

struct clientanimating_t {
	void*			m_pAnimating;
	unsigned int	m_fFlags;
	clientanimating_t( void* _pAnim, unsigned int _flags ) : m_pAnimating( _pAnim ), m_fFlags( _flags ) {}
};

struct CCSGOPlayerAnimState
{
	void update( float yaw, float pitch );
	void update_ex( float yaw, float pitch );
	void setup_velocity( float );
	void setup_aim_matrix( );
	void setup_weapon_action( );
	void setup_movement( );
	void setup_alive_loop( );
	void setup_whole_body_action( );
	void setup_flashed_reaction( );
	void setup_flinch( );
	bool cache_sequences( );

	void reset( );

	char pad[ 3 ];
	char bUnknown; //0x4
	char pad2[ 91 ];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flLastUpdateDelta; //0x74 //b8
	float m_flEyeYaw; //0x78 //c0
	float m_flPitch; //0x7C //b8
	float m_flGoalFeetYaw; //0x80 //c0
	float m_flCurrentFeetYaw; //0x84 //c8
	float m_flCurrentTorsoYaw; //0x88 //d0
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground //d8
	float m_flLeanAmount; //0x90 //e0
	char pad4[ 4 ]; //NaN //e8
	float m_flFeetCycle; //0x98 0 to 1 //100 //f0
	float m_flFeetYawRate; //0x9C 0 to 1 //108 //f8
	char pad11[ 4 ];
	float m_fDuckAmount; //0xA4 //118 //108
	float m_fLandingDuckAdditiveSomething; //0xA8 //110
	float m_fUnknown3; //0xAC //118
	vec3_t m_vOrigin; //0xB0, 0xB4, 0xB8 //120
	vec3_t m_vLastOrigin; //0xBC, 0xC0, 0xC4
	vec3_t m_vecVelocity;
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[ 8 ];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float m_velocity; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[ 10 ];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[ 4 ]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[ 4 ]; //NaN
	float m_flUnknown3;
	char pad10[ 0x210 ];
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

struct ent_animdata_t {
	std::array< C_AnimationLayer, 13 > m_animlayers;
	std::array< C_AnimationLayer, 13 > m_last_layers;
	std::array< float, 24 >			   m_poseparams;
	matrix3x4						   m_bones[128];
	int								   m_activity;
	float							   m_simtime;
	float							   m_last_simtime;
	vec3_t							   m_last_interp_origin;
	float							   m_fraction;
	vec3_t							   m_interp_origin;
	vec3_t							   m_last_origin;
	vec3_t							   m_last_velocity;
	vec3_t							   m_anim_velocity;
	bool							   m_valid{ };
	float							   m_last_duck;
	int								   m_prev_flags;
	int								   m_anim_flags;
	float							   m_adjust_rate;
	float							   m_adjust_cycle;
	bool							   m_is_stopping;
	float							   m_last_animtime;
};

class c_base_player {
private:
	static ent_animdata_t sm_animdata[ 65 ];
	static CCSGOPlayerAnimState* sm_player_animstate[65];
public:
	//NETVAR( m_YourFunctionName, "Name", "Table", extra, type )

	NETVAR( m_nMoveType, "m_nRenderMode", "DT_BaseEntity", 1, MoveType_t );
	NETVAR( m_iAccount, "m_iAccount", "DT_CSPlayer", 0, int );
	NETVAR( m_iHealth, "m_iHealth", "DT_BasePlayer", 0, int );
	NETVAR( m_lifeState, "m_lifeState", "DT_BasePlayer", 0, uint8_t );
	NETVAR( m_nTickBase, "m_nTickBase", "DT_BasePlayer", 0, int );
	NETVAR( m_fFlags, "m_fFlags", "DT_BasePlayer", 0, int );
	NETVAR( m_vecVelocity, "m_vecVelocity[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_nHitboxSet, "m_nHitboxSet", "DT_BaseAnimating", 0, int );
	NETVAR( m_vecOrigin, "m_vecOrigin", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_vecViewOffset, "m_vecViewOffset[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_hActiveWeapon, "m_hActiveWeapon", "DT_BaseCombatCharacter", 0, ulong_t );
	NETVAR( m_iTeamNum, "m_iTeamNum", "DT_BaseEntity", 0, int );
	NETVAR(m_dwBoneMatrix, "m_nForceBone", "DT_BaseAnimating", 28, uintptr_t);
	NETVAR( m_bHasHelmet, "m_bHasHelmet", "DT_CSPlayer", 0, bool );
	NETVAR( m_bHasDefuser, "m_bHasDefuser", "DT_CSPlayer", 0, bool );
	NETVAR( m_ArmorValue, "m_ArmorValue", "DT_CSPlayer", 0, int );
	NETVAR( m_flNextAttack, "m_flNextAttack", "DT_BaseCombatCharacter", 0, float );
	NETVAR( m_flLowerBodyYawTarget, "m_flLowerBodyYawTarget", "DT_CSPlayer", 0, float );
	NETVAR( m_angEyeAngles, "m_angEyeAngles[0]", "DT_CSPlayer", 0, vec3_t );
	NETVAR( m_AnimOverlay, "m_hLightingOrigin", "DT_BaseAnimating", 0x3c, CUtlVector< C_AnimationLayer > );
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
	NETVAR( m_flOldSimulationTime, "m_flSimulationTime", "DT_BaseEntity", 0x4, float );
	NETVAR( m_CoordinateFrame, "m_CollisionGroup", "DT_BaseEntity", -0x30, matrix3x4 );
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
	NETVAR( m_pSurfaceData, "m_flLaggedMovementValue", "DT_CSPlayer", 0x18, surfacedata_t* );
	NETVAR( m_vecBaseVelocity, "m_vecBaseVelocity", "DT_CSPlayer", 0, vec3_t );
	NETVAR( m_vecAbsVelocity, "m_vecVelocity", "DT_CSPlayer", 0x12, vec3_t );
	NETVAR( m_flGravity, "m_iTeamNum", "DT_CSPlayer", -0x14, float );
	NETVAR( m_surfaceFriction, "m_vecLadderNormal", "DT_CSPlayer", -0x4, float );
	NETVAR( m_flDuckAmount, "m_flDuckAmount", "DT_CSPlayer", 0, float );
	NETVAR( m_flDuckSpeed, "m_flDuckSpeed", "DT_CSPlayer", 0, float );
	PNETVAR( m_hMyWearables, "m_hMyWearables", "DT_BaseCombatCharacter", 0, uint32_t );
	PNETVAR( m_hMyWeapons, "m_hMyWeapons", "DT_BaseCombatCharacter", 0, uint32_t );
	NETVAR( m_hGroundEntity, "m_hGroundEntity", "DT_CSPlayer", 0, uint32_t );
	NETVAR(m_flHealthShotBoostExpirationTime, "m_flHealthShotBoostExpirationTime", "DT_CSPlayer", 0, float);

	OFFSET( m_CachedBoneData, 0x28FC + sizeof( void* ), CUtlVector< matrix3x4 > );
	OFFSET( m_flSpawnTime, 0xa290, float );
	OFFSET( m_IKContext, 0x265c, IKContext* );
public:
	static uintptr_t get_player_resource( );
	static uintptr_t get_game_rules( );
	static bool& s_bInvalidateBoneCache( );
	static int& g_iModelBoneCounter( );

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

	__forceinline auto& get_animdata( ) {
		return sm_animdata[ ce( )->GetIndex( ) ];
	}

	__forceinline bool is_alive( ) {
		return m_iHealth( ) > 0 && m_lifeState( ) == LIFE_ALIVE;
	}

	__forceinline void update_ik_locks( float curtime ) {
		return util::get_vfunc< 186, void >( this, curtime );
	}

	__forceinline void calculate_ik_locks( float curtime ) {
		return util::get_vfunc< 187, void >( this, curtime );
	}

	__forceinline void standard_blending_rules( void* hdr, vec3_t* pos, void* q, float curtime, int bone_mask ) {
		return util::get_vfunc< 200, void >( this, hdr, pos, q, curtime, bone_mask );
	}

	__forceinline void build_transformations( void* hdr, vec3_t* pos, void* q, const matrix3x4& transform, int bone_mask, char* computed ) {
		return util::get_vfunc< 184, void >( this, hdr, pos, q, transform, bone_mask, computed );
	}

	__forceinline bool has_valid_anim( ) {
		if( m_bClientSideAnimation( ) )
			return true;

		return sm_animdata[ ce( )->GetIndex( ) ].m_valid;
	}

	bool is_flashed( );
	bool is_reloading( );

	int get_ping( );

	int get_c4_carrier();

	__forceinline auto& m_aimPunchAngle( bool real = false ) {
		static vec3_t backup_var{ };
		static auto offset = g_netvars.get_netvar( fnv( "DT_BasePlayer" ), fnv( "m_aimPunchAngle" ) );
		if( g_settings.misc.no_recoil( ) && !real ) {
			backup_var = { 0.f, 0.f, 0.f };
			return backup_var;
		}

		return get< vec3_t >( offset );
	}

	__forceinline vec3_t get_anim_velocity( ) {
		auto index = ce( )->GetIndex( );
		return sm_animdata[ index ].m_anim_velocity;
	}

	__forceinline CCSGOPlayerAnimState* get_animstate( ) {
		static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( "DT_CSPlayer" ),
			fnv( "m_bIsScoped" ) );

		if( !offset ) {
			return nullptr;
		}

		return *reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( this ) + offset - 0xa );
	}

	__forceinline void update_clientside_animation( ) {
		bool backup = m_bClientSideAnimation( );
		m_bClientSideAnimation( ) = true;
		util::get_vfunc< 218, void >( this );
		m_bClientSideAnimation( ) = backup;
	}

	void invalidate_bone_cache( );

	__forceinline vec3_t get_eye_pos( ) {
		return m_vecOrigin() + m_vecViewOffset();
	}

	__forceinline bool is_player( ) {
		if( !this )
			return false;

		ClientClass* client_class = ce( )->GetClientClass( );
		return client_class && client_class->m_class_id == CCSPlayer;
	}

	__forceinline bool is_valid( ) {
		return !!this && is_player( ) && !ce( )->IsDormant( ) && is_alive( );
	}

	bool is_fakewalking( );
	player_info_t get_info( );
	void get_name_safe( char* buf );
	c_base_weapon* get_weapon( );
	vec3_t get_hitbox_pos( int hitbox );
	vec3_t get_hitbox_mins( int hitbox );
	vec3_t get_hitbox_maxs( int hitbox );
	bool is_visible( int hitbox );
	bool can_attack( bool ignore_rapid = false );
	int	 get_choked_ticks( );
	float get_hitbox_radius( int hitbox );

	void set_abs_origin( vec3_t origin );
	void set_abs_angles( vec3_t angles );
	void set_abs_velocity( vec3_t velocity );
	void enforce_animations();
	void calc_abs_velocity( );

	void set_needs_interpolate( bool interp );
	void do_ent_interpolation( bool reset );

	void invalidate_physics_recursive( int flags );
	void create_animstate( CCSGOPlayerAnimState* state );
	void setup_bones_ex( matrix3x4* matrix_out, int bone_mask, float curtime );
	int get_seq_activity( int sequence );

	void fix_animations( bool reset = false, bool resolver_change = false );
	void validate_animation_layers( );
	void compute_move_cycle( bool reset, bool moving );
	void calculate_duckamount( bool reset );
	void cache_anim_data( );
	void restore_anim_data( bool layers = false );
	void handle_taser_animation( );
	void estimate_layers( bool reset );
	void calc_anim_velocity( bool reset );

	bool is_breaking_lc( );
};