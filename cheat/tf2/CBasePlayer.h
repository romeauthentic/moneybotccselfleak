#pragma once
#include "vector.hpp"
#include "CBaseWeapon.h"
#include "IVEngineClient.h"

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

enum TFClasses_t {
	TF2_None = 0,
	TF2_Scout,
	TF2_Sniper,
	TF2_Soldier,
	TF2_Demoman,
	TF2_Medic,
	TF2_Heavy,
	TF2_Pyro,
	TF2_Spy,
	TF2_Engineer
};

enum LifeStates_t {
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

enum PlayerStates_t {
	FL_ONGROUND = ( 1 << 0 ),
	FL_DUCKING = ( 1 << 1 ),
	FL_WATERJUMP = ( 1 << 2 ),
	FL_ONTRAIN = ( 1 << 3 ),
	FL_INRAIN = ( 1 << 4 ),
	FL_FROZEN = ( 1 << 5 ),
	FL_ATCONTROLS = ( 1 << 6 ),
	FL_CLIENT = ( 1 << 7 ),
	FL_FAKECLIENT = ( 1 << 8 ),
	FL_INWATER = ( 1 << 9 ),
};

struct dynamic_box_t {
	int x, y, w, h;
};

enum PlayerConds_t
{
	TFCond_Slowed = ( 1 << 0 ), //Toggled when a player is slowed down. 
	TFCond_Zoomed = ( 1 << 1 ), //Toggled when a player is zoomed. 
	TFCond_Disguising = ( 1 << 2 ), //Toggled when a Spy is disguising.  
	TFCond_Disguised = ( 1 << 3 ), //Toggled when a Spy is disguised. 
	TFCond_Cloaked = ( 1 << 4 ), //Toggled when a Spy is invisible. 
	TFCond_Ubercharged = ( 1 << 5 ), //Toggled when a player is ÜberCharged. 
	TFCond_TeleportedGlow = ( 1 << 6 ), //Toggled when someone leaves a teleporter and has glow beneath their feet. 
	TFCond_Taunting = ( 1 << 7 ), //Toggled when a player is taunting. 
	TFCond_UberchargeFading = ( 1 << 8 ), //Toggled when the ÜberCharge is fading. 
	TFCond_CloakFlicker = ( 1 << 9 ), //Toggled when a Spy is visible during cloak. 
	TFCond_Teleporting = ( 1 << 10 ), //Only activates for a brief second when the player is being teleported; not very useful. 
	TFCond_Kritzkrieged = ( 1 << 11 ), //Toggled when a player is being crit buffed by the KritzKrieg. 
	TFCond_TmpDamageBonus = ( 1 << 12 ), //Unknown what this is for. Name taken from the AlliedModders SDK. 
	TFCond_DeadRingered = ( 1 << 13 ), //Toggled when a player is taking reduced damage from the Deadringer. 
	TFCond_Bonked = ( 1 << 14 ), //Toggled when a player is under the effects of The Bonk! Atomic Punch. 
	TFCond_Stunned = ( 1 << 15 ), //Toggled when a player's speed is reduced from airblast or a Sandman ball. 
	TFCond_Buffed = ( 1 << 16 ), //Toggled when a player is within range of an activated Buff Banner. 
	TFCond_Charging = ( 1 << 17 ), //Toggled when a Demoman charges with the shield. 
	TFCond_DemoBuff = ( 1 << 18 ), //Toggled when a Demoman has heads from the Eyelander. 
	TFCond_CritCola = ( 1 << 19 ), //Toggled when the player is under the effect of The Crit-a-Cola. 
	TFCond_InHealRadius = ( 1 << 20 ), //Unused condition, name taken from AlliedModders SDK. 
	TFCond_Healing = ( 1 << 21 ), //Toggled when someone is being healed by a medic or a dispenser. 
	TFCond_OnFire = ( 1 << 22 ), //Toggled when a player is on fire. 
	TFCond_Overhealed = ( 1 << 23 ), //Toggled when a player has >100% health. 
	TFCond_Jarated = ( 1 << 24 ), //Toggled when a player is hit with a Sniper's Jarate. 
	TFCond_Bleeding = ( 1 << 25 ), //Toggled when a player is taking bleeding damage. 
	TFCond_DefenseBuffed = ( 1 << 26 ), //Toggled when a player is within range of an activated Battalion's Backup. 
	TFCond_Milked = ( 1 << 27 ), //Player was hit with a jar of Mad Milk. 
	TFCond_MegaHeal = ( 1 << 28 ), //Player is under the effect of Quick-Fix charge. 
	TFCond_RegenBuffed = ( 1 << 29 ), //Toggled when a player is within a Concheror's range. 
	TFCond_MarkedForDeath = ( 1 << 30 ), //Player is marked for death by a Fan O'War hit. Effects are similar to TFCond_Jarated. 
	TFCond_NoHealingDamageBuff = ( 1 << 31 ), //Unknown what this is used for.

	TFCondEx_SpeedBuffAlly = ( 1 << 0 ), //Toggled when a player gets hit with the disciplinary action. 
	TFCondEx_HalloweenCritCandy = ( 1 << 1 ), //Only for Scream Fortress event maps that drop crit candy. 
	TFCondEx_CritCanteen = ( 1 << 2 ), //Player is getting a crit boost from a MVM canteen.
	TFCondEx_CritDemoCharge = ( 1 << 3 ), //From demo's shield
	TFCondEx_CritHype = ( 1 << 4 ), //Soda Popper crits. 
	TFCondEx_CritOnFirstBlood = ( 1 << 5 ), //Arena first blood crit buff. 
	TFCondEx_CritOnWin = ( 1 << 6 ), //End of round crits. 
	TFCondEx_CritOnFlagCapture = ( 1 << 7 ), //CTF intelligence capture crits. 
	TFCondEx_CritOnKill = ( 1 << 8 ), //Unknown what this is for. 
	TFCondEx_RestrictToMelee = ( 1 << 9 ), //Unknown what this is for. 
	TFCondEx_DefenseBuffNoCritBlock = ( 1 << 10 ), //MvM Buff.
	TFCondEx_Reprogrammed = ( 1 << 11 ), //MvM Bot has been reprogrammed.
	TFCondEx_PyroCrits = ( 1 << 12 ), //Player is getting crits from the Mmmph charge. 
	TFCondEx_PyroHeal = ( 1 << 13 ), //Player is being healed from the Mmmph charge. 
	TFCondEx_FocusBuff = ( 1 << 14 ), //Player is getting a focus buff.
	TFCondEx_DisguisedRemoved = ( 1 << 15 ), //Disguised remove from a bot.
	TFCondEx_MarkedForDeathSilent = ( 1 << 16 ), //Player is under the effects of the Escape Plan/Equalizer or GRU.
	TFCondEx_DisguisedAsDispenser = ( 1 << 17 ), //Bot is disguised as dispenser.
	TFCondEx_Sapped = ( 1 << 18 ), //MvM bot is being sapped.
	TFCondEx_UberchargedHidden = ( 1 << 19 ), //MvM Related
	TFCondEx_UberchargedCanteen = ( 1 << 20 ), //Player is receiving ÜberCharge from a canteen.
	TFCondEx_HalloweenBombHead = ( 1 << 21 ), //Player has a bomb on their head from Merasmus.
	TFCondEx_HalloweenThriller = ( 1 << 22 ), //Players are forced to dance from Merasmus.
	TFCondEx_BulletCharge = ( 1 << 26 ), //Player is receiving 75% reduced damage from bullets.
	TFCondEx_ExplosiveCharge = ( 1 << 27 ), //Player is receiving 75% reduced damage from explosives.
	TFCondEx_FireCharge = ( 1 << 28 ), //Player is receiving 75% reduced damage from fire.
	TFCondEx_BulletResistance = ( 1 << 29 ), //Player is receiving 10% reduced damage from bullets.
	TFCondEx_ExplosiveResistance = ( 1 << 30 ), //Player is receiving 10% reduced damage from explosives.
	TFCondEx_FireResistance = ( 1 << 31 ), //Player is receiving 10% reduced damage from fire.

	TFCondEx2_Stealthed = ( 1 << 0 ),
	TFCondEx2_MedigunDebuff = ( 1 << 1 ),
	TFCondEx2_StealthedUserBuffFade = ( 1 << 2 ),
	TFCondEx2_BulletImmune = ( 1 << 3 ),
	TFCondEx2_BlastImmune = ( 1 << 4 ),
	TFCondEx2_FireImmune = ( 1 << 5 ),
	TFCondEx2_PreventDeath = ( 1 << 6 ),
	TFCondEx2_MVMBotRadiowave = ( 1 << 7 ),
	TFCondEx2_HalloweenSpeedBoost = ( 1 << 8 ), //Wheel has granted player speed boost.
	TFCondEx2_HalloweenQuickHeal = ( 1 << 9 ), //Wheel has granted player quick heal.
	TFCondEx2_HalloweenGiant = ( 1 << 10 ), //Wheel has granted player giant mode.
	TFCondEx2_HalloweenTiny = ( 1 << 11 ), //Wheel has granted player tiny mode.
	TFCondEx2_HalloweenInHell = ( 1 << 12 ), //Wheel has granted player in hell mode.
	TFCondEx2_HalloweenGhostMode = ( 1 << 13 ), //Wheel has granted player ghost mode.
	TFCondEx2_Parachute = ( 1 << 16 ), //Player has deployed the BASE Jumper.
	TFCondEx2_BlastJumping = ( 1 << 17 ), //Player has sticky or rocket jumped.

	TFCond_MiniCrits = ( TFCond_Buffed | TFCond_CritCola ),
	TFCond_IgnoreStates = ( TFCond_Ubercharged | TFCond_Bonked ),
	TFCondEx_IgnoreStates = ( TFCondEx_PyroHeal )
};

enum tf_hitbox : int {
	head = 0,
	pelvis = 1,
	spine_0 = 2,
	spine_1 = 3,
	spine_2 = 4,
	spine_3 = 5,
	upperArm_L = 6,
	lowerArm_L = 7,
	hand_L = 8,
	upperArm_R = 9,
	lowerArm_R = 10,
	hand_R = 11,
	hip_L = 12,
	knee_L = 13,
	foot_L = 14,
	hip_R = 15,
	knee_R = 16,
	foot_R = 17,
};

class c_base_player : public IClientEntity {
public:

	NETVAR( m_nMoveType, "m_nRenderMode", "DT_BaseEntity", 1, MoveType_t );
	NETVAR( m_fFlags, "m_fFlags", "DT_BasePlayer", 0, int );
	NETVAR( m_vecVelocity, "m_vecVelocity[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_flSimulationTime, "m_flSimulationTime", "DT_BaseEntity", 0, float );
	NETVAR( m_vecOrigin, "m_vecOrigin", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_angRotation, "m_angRotation", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_CollisionGroup, "m_CollisionGroup", "DT_BaseEntity", -0x4c, matrix3x4 );
	NETVAR( m_Collision, "m_Collision", "DT_BaseEntity", 0, std::ptrdiff_t );
	NETVAR( m_vecViewOffset, "m_vecViewOffset[0]", "DT_BasePlayer", 0, vec3_t );
	NETVAR( m_nTickBase, "m_nTickBase", "DT_BasePlayer", 0, int );
	NETVAR( m_lifeState, "m_lifeState", "DT_BasePlayer", 0, byte );
	NETVAR( m_iHealth, "m_iHealth", "DT_BasePlayer", 0, int );
	NETVAR( m_iTeamNum, "m_iTeamNum", "DT_BasePlayer", 0, int );
	NETVAR( m_nHitboxSet, "m_nHitboxSet", "DT_BaseAnimating", 0, int );
	NETVAR( m_PlayerClass, "m_PlayerClass", "DT_TFPlayer", 0, int );
	NETVAR( m_iClass, "m_iClass", "DT_TFPlayer", 0, int );
	NETVAR( m_angEyeAngles, "m_angEyeAngles[0]", "DT_TFPlayer", 0, vec3_t );
	NETVAR( m_hActiveWeapon, "m_hActiveWeapon", "DT_BaseCombatCharacter", 0, uintptr_t );
	NETVAR( m_iMaxHealth, "m_iMaxHealth", "DT_TFPlayerResource", 0, std::ptrdiff_t );
	NETVAR( m_hObserverTarget, "m_hObserverTarget", "DT_BasePlayer", 0, uint32_t );
	NETVAR( m_bIsScoped, "m_bIsScoped", "DT_TFPlayer", 0, bool );
	NETVAR( m_vecMins, "m_vecMins", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_vecMaxs, "m_vecMaxs", "DT_BaseEntity", 0, vec3_t );
	NETVAR( m_iObserverMode, "m_iObserverMode", "DT_Base_player", 0, int );
	NETVAR( m_bGlowEnabled, "m_bGlowEnabled", "DT_TFPlayer", 0, bool );
	NETVAR( m_nPlayerCond, "m_nPlayerCond", "DT_TFPlayer", 0, int );

	//fuck you this is for ragdolls
	NETVAR( m_vecRagdollVelocity, "m_vecRagdollVelocity", "DT_TFRagdoll", 0, vec3_t );



	static uintptr_t get_player_resource( );
	static uintptr_t get_game_rules( );

	__forceinline IClientEntity* ce( ) {
		return reinterpret_cast< IClientEntity* >( this );
	}

	__forceinline bool is_player( ) {
		client_class_t* client_class = ce( )->GetClientClass( );
		return client_class && client_class->m_class_id == CTFPlayer;
	}

	player_info_t get_info( );
	void get_name_safe( char* buf );
	bool is_valid( );
	vec3_t get_eye_pos( );
	vec3_t get_origin( );
	vec3_t get_view_offset( );
	vec3_t get_bone_position( int );
	vec3_t get_hitbox_position( int );
	vec3_t get_eye_angles( );
	vec3_t get_velocity( );
	int get_team( );
	bool is_enemy( );
	const char* get_class_name( );
	int get_class( );
	byte get_life_state( );
	int get_flags( );
	bool is_alive( );
	bool is_visible( );
	bool is_visible( int );
	dynamic_box_t get_dynamic_box( );
	int get_health( );
	int get_max_health( );
	bool is_buffed( );
	int get_tick_base( );
	c_base_weapon* get_active_weapon( );
};