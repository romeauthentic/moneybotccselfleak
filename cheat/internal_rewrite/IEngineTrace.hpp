#pragma once
#include "vector.hpp"
#include "IClientEntityList.hpp"

enum TraceType_t {
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY, // NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY, // NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS, // NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

enum SurfaceFlags_t {
	DISPSURF_FLAG_SURFACE = 1 << 0,
	DISPSURF_FLAG_WALKABLE = 1 << 1,
	DISPSURF_FLAG_BUILDABLE = 1 << 2,
	DISPSURF_FLAG_SURFPROP1 = 1 << 3,
	DISPSURF_FLAG_SURFPROP2 = 1 << 4,
};

enum Hitgroup_t {
	HITGROUP_GENERIC,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	HITGROUP_GEAR
};

struct csurface_t {
	const char* name;
	short surfaceProps;
	unsigned short flags;
};

struct cplane_t {
	vec3_t normal;
	float dist;
	uint8_t type;
	uint8_t signbits;
	uint8_t pad[ 2 ];
};

class ITraceFilter {
public:
	virtual bool ShouldHitEntity( void* pEntity, int contentsMask ) = 0;
	virtual int  GetTraceType( ) const = 0;
};

class CTraceFilter : public ITraceFilter {
public:
	CTraceFilter( ) {}

	CTraceFilter( void *ent )
		: pSkip( ent ) {}

	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return !( pEntityHandle == pSkip );
	}

	int GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterOneEntity : public ITraceFilter {
public:
	bool ShouldHitEntity( void* entity_handle, int contents ) override {
		return entity_handle == ent;
	}

	int GetTraceType( ) const override {
		return TRACE_ENTITIES_ONLY;
	}

	void* ent;
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return false;
	}

	int GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterEntitiesOnly : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return !( pEntityHandle == pSkip );
	}

	int GetTraceType( ) const override {
		return TRACE_ENTITIES_ONLY;
	}

	void* pSkip;
};

class CTraceFilterPlayersOnly : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		IClientEntity* hit_entity = ( IClientEntity* )pEntityHandle;
		int index = hit_entity->GetIndex( );
		return !( pEntityHandle == pSkip || index < 1 || index > 64 );
	}

	int GetTraceType( ) const override {
		return TRACE_ENTITIES_ONLY;
	}

	void* pSkip;
};

class CTraceFilterWorldOnly : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return false;
	}

	int GetTraceType( ) const override {
		return TRACE_WORLD_ONLY;
	}

	void* pSkip;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter {
public:
	CTraceFilterSkipTwoEntities( ) {

	}

	CTraceFilterSkipTwoEntities( void* pPassEnt1, void* pPassEnt2 ) {
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return !( pEntityHandle == passentity1 || pEntityHandle == passentity2 );
	}

	int GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};

class __declspec( align( 16 ) )VectorAligned : public vec3_t {
public:
	VectorAligned( float _x, float _y, float _z ) {
		x = _x;
		y = _y;
		z = _z;
		w = 0;
	}

	VectorAligned( ) { }

	void operator=( const vec3_t& vOther ) {
		x = vOther.x;
		y = vOther.y;
		z = vOther.z;
	}

	float w;
};

struct Ray_t
{
	VectorAligned m_Start;
	VectorAligned m_Delta;
	VectorAligned m_StartOffset;
	VectorAligned m_Extents;

	const matrix3x4* m_pWorldAxisTransform;

	bool m_IsRay;
	bool m_IsSwept;

	Ray_t( ) : m_pWorldAxisTransform( nullptr ) { }

	void Init( const vec3_t& start, const vec3_t& end ) {
		m_Delta = end - start;

		m_IsSwept = m_Delta.lengthsqr( ) != 0;

		m_Extents = vec3_t( 0.f, 0.f, 0.f );
		m_pWorldAxisTransform = nullptr;
		m_IsRay = true;

		m_StartOffset = vec3_t( 0.f, 0.f, 0.f );
		m_Start = start;
	}

	void Init( const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs ) {
		m_Delta = end - start;

		m_pWorldAxisTransform = nullptr;
		m_IsSwept = m_Delta.lengthsqr( ) != 0;

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = m_Extents.lengthsqr( ) < 1e-6;

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

class CBaseTrace
{
public:
	bool IsDispSurface( ) const { return ( dispFlags & DISPSURF_FLAG_SURFACE ) != 0; }
	bool IsDispSurfaceWalkable( ) const { return ( dispFlags & DISPSURF_FLAG_WALKABLE ) != 0; }
	bool IsDispSurfaceBuildable( ) const { return ( dispFlags & DISPSURF_FLAG_BUILDABLE ) != 0; }
	bool IsDispSurfaceProp1( ) const { return ( dispFlags & DISPSURF_FLAG_SURFPROP1 ) != 0; }
	bool IsDispSurfaceProp2( ) const { return ( dispFlags & DISPSURF_FLAG_SURFPROP2 ) != 0; }

public:
	vec3_t startpos;
	vec3_t endpos;
	cplane_t plane;

	float fraction;

	int contents;
	unsigned short dispFlags;

	bool allsolid;
	bool startsolid;

	CBaseTrace( ) {}

private:
	CBaseTrace( const CBaseTrace& vOther );
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld( ) const;

	bool DidHitNonWorldEntity( ) const;

	int GetEntityIndex( ) const;

	bool DidHit( ) const
	{
		return fraction < 1 || allsolid || startsolid;
	}

public:
	float fractionleftsolid;
	csurface_t surface;

	int hitgroup;

	short physicsbone;
	unsigned short worldSurfaceIndex;

	IClientEntity* m_pEnt;
	int hitbox;

	CGameTrace( ) {}
	CGameTrace( const CGameTrace& vOther );
};

class IEngineTrace {
public:
	int GetPointContents( const vec3_t& vecAbsPosition, int contentsMask = 0xffffffff ) {
		using fn = int( __thiscall* )( void*, const vec3_t&, int, void* );
		return util::get_vfunc< fn >( this, 0 )( this, vecAbsPosition, contentsMask, nullptr );
	}

	void TraceRay( const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, CBaseTrace* pTrace ) {
		return util::get_vfunc< 5, void >( this, ray, fMask, pTraceFilter, pTrace );
	}

	void ClipRayToEntity( const Ray_t& ray, unsigned int fMask, IClientEntity* pEnt, CBaseTrace* pTrace ) {
		return util::get_vfunc< 3, void >( this, ray, fMask, pEnt, pTrace );
	}
};

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_SOLID_SURF           0x3
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_LIGHT_SURF           0x7
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40
#define   CONTENTS_OPAQUE               0x80
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800
#define   CONTENTS_TEAM2                0x1000
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000

#define   CONTENTS_MONSTER              0x2000000
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001
#define   SURF_SKY2D                    0x0002
#define   SURF_SKY                      0x0004
#define   SURF_WARP                     0x0008
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020
#define   SURF_TRIGGER                  0x0040
#define   SURF_NODRAW                   0x0080

#define   SURF_HINT                     0x0100

#define   SURF_SKIP                     0x0200
#define   SURF_NOLIGHT                  0x0400
#define   SURF_BUMPLIGHT                0x0800
#define   SURF_NOSHADOWS                0x1000
#define   SURF_NODECALS                 0x2000
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000
#define   SURF_HITBOX                   0x8000

// ----------------------------------------------------- 
// spatial content masks - used for spatial queries (traceline,etc.) 
// ----------------------------------------------------- 
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)