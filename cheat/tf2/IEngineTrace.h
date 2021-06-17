#pragma once
#include "vector.hpp"
#include "VFunc.h"

enum trace_type_t {
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

enum SurfaceFlags_t {
	DISPSURF_FLAG_SURFACE = ( 1 << 0 ),
	DISPSURF_FLAG_WALKABLE = ( 1 << 1 ),
	DISPSURF_FLAG_BUILDABLE = ( 1 << 2 ),
	DISPSURF_FLAG_SURFPROP1 = ( 1 << 3 ),
	DISPSURF_FLAG_SURFPROP2 = ( 1 << 4 ),
};

struct csurface_t {
	const char		*name;
	short			surfaceProps;
	unsigned short	flags;
};

struct cplane_t {
	vec3_t	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[ 2 ];
};

class ITraceFilter {
public:
	virtual bool ShouldHitEntity( void *pEntity, int contentsMask ) = 0;
	virtual trace_type_t	GetTraceType( ) const = 0;
};

class CTraceFilter : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) {
		return !( pEntityHandle == pSkip );
	}
	virtual trace_type_t	GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter {
public:
	bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) override {
		return false;
	}

	trace_type_t GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterSkipTwoEntities : public ITraceFilter {
public:
	CTraceFilterSkipTwoEntities( void* pPassEnt1, void* pPassEnt2 )
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity( void* pEntityHandle, int contentsMask ) {
		return !( pEntityHandle == passentity1 || pEntityHandle == passentity2 );
	}

	virtual trace_type_t GetTraceType( ) const {
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};

class __declspec( align( 16 ) )VectorAligned : public vec3_t {
public:
	VectorAligned& operator=( const vec3_t &vOther ) {
		x = vOther.x;
		y = vOther.y;
		z = vOther.z;
		return *this;
	}

	VectorAligned& operator=( const float &other ) {
		*this = { other, other, other };
		return *this;
	}
	float w;
};

struct Ray_t {
	VectorAligned   m_Start;
	VectorAligned   m_Delta;
	VectorAligned   m_StartOffset;
	VectorAligned   m_Extents;

	bool    m_IsRay;
	bool    m_IsSwept;

	void Init( vec3_t& start, vec3_t& end ) {
		m_Delta = end - start;

		m_IsSwept = ( m_Delta.lengthsqr( ) != 0 );

		m_Extents = 0.f;
		m_IsRay = true;

		m_StartOffset = 0.f;
		m_Start = start;
	}

	void Init( vec3_t& start, vec3_t& end, vec3_t& mins, vec3_t& maxs ) {
		m_Delta = end - start;

		m_IsSwept = ( m_Delta.lengthsqr( ) != 0 );

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = ( m_Extents.lengthsqr( ) < 1e-6 );

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start - m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

class CBaseTrace {
public:
	bool IsDispSurface( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFACE ) != 0 ); } //not in csgo
	bool IsDispSurfaceWalkable( void ) { return ( ( dispFlags & DISPSURF_FLAG_WALKABLE ) != 0 ); }
	bool IsDispSurfaceBuildable( void ) { return ( ( dispFlags & DISPSURF_FLAG_BUILDABLE ) != 0 ); }
	bool IsDispSurfaceProp1( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFPROP1 ) != 0 ); }
	bool IsDispSurfaceProp2( void ) { return ( ( dispFlags & DISPSURF_FLAG_SURFPROP2 ) != 0 ); }

public:
	vec3_t			startpos;
	vec3_t			endpos;
	cplane_t		plane;

	float			fraction;

	int				contents;
	unsigned short	dispFlags;

	bool			allsolid;
	bool			startsolid;

	CBaseTrace( ) {}

private:
	CBaseTrace( const CBaseTrace& vOther );
};

class CGameTrace : public CBaseTrace {
public:
	bool DidHitWorld( ) const;

	bool DidHitNonWorldEntity( ) const;

	int GetEntityIndex( ) const;

	bool DidHit( ) const {
		return fraction < 1 || allsolid || startsolid;
	}

public:
	float			fractionleftsolid;
	csurface_t		surface;

	int				hitgroup;

	short			physicsbone;

	IClientEntity*	m_pEnt;
	int				hitbox;

	CGameTrace( ) {}
	CGameTrace( const CGameTrace& vOther );
};

typedef CGameTrace trace_t;

class IEngineTrace {
public:
	void clip_ray_to_ent( const Ray_t& ray, unsigned int fMask, IClientEntity* pEnt, trace_t* pTrace ) {
		return call_vfunc< void( __thiscall* )( void*, const Ray_t&, unsigned int, IClientEntity*, trace_t* ) >( this, 2 )( this, ray, fMask, pEnt, pTrace );
	}

	void trace_ray( const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace ) {
		return call_vfunc< void( __thiscall* )( void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t* ) >( this, 4 )( this, ray, fMask, pTraceFilter, pTrace );
	}
};


#define	CONTENTS_EMPTY			0		/**< No contents. */
#define	CONTENTS_SOLID			0x1		/**< an eye is never valid in a solid . */
#define	CONTENTS_WINDOW			0x2		/**< translucent, but not watery (glass). */
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		/**< alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't. */
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_MIST			0x40
#define CONTENTS_OPAQUE			0x80		/**< things that cannot be seen through (may be non-solid though). */
#define	LAST_VISIBLE_CONTENTS	0x80
#define ALL_VISIBLE_CONTENTS 	(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))
#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED5		0x200
#define CONTENTS_UNUSED6		0x4000
#define CONTENTS_TEAM1			0x800		/**< per team contents used to differentiate collisions. */
#define CONTENTS_TEAM2			0x1000		/**< between players and objects on different teams. */
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000		/**< ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW. */
#define CONTENTS_MOVEABLE		0x4000		/**< hits entities which are MOVETYPE_PUSH (doors, plats, etc) */
#define	CONTENTS_AREAPORTAL		0x8000		/**< remaining contents are non-visible, and don't eat brushes. */
#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

/**
* @section currents can be added to any other contents, and may be mixed
*/
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

/**
* @endsection
*/

#define	CONTENTS_ORIGIN			0x1000000	/**< removed before bsping an entity. */
#define	CONTENTS_MONSTER		0x2000000	/**< should never be on a brush, only in game. */
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	/**< brushes to be added after vis leafs. */
#define	CONTENTS_TRANSLUCENT	0x10000000	/**< auto set if any surface has trans. */
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	/**< use accurate hitboxes on trace. */

/**
* @section Trace masks.
*/
#define	MASK_ALL				(0xFFFFFFFF)
#define	MASK_SOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 			/**< everything that is normally solid */
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 	/**< everything that blocks player movement */
#define	MASK_NPCSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) /**< blocks npc movement */
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME) 							/**< water physics in these contents */
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE) 							/**< everything that blocks line of sight for AI, lighting, etc */
#define MASK_OPAQUE_AND_NPCS	(MASK_OPAQUE|CONTENTS_MONSTER)										/**< everything that blocks line of sight for AI, lighting, etc, but with monsters added. */
#define	MASK_VISIBLE			(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE) 								/**< everything that blocks line of sight for players */
#define MASK_VISIBLE_AND_NPCS	(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE) 							/**< everything that blocks line of sight for players, but with monsters added. */
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX) 	/**< bullets see these as solid */
#define MASK_SHOT_HULL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE) 	/**< non-raycasted weapons see this as solid (includes grates) */
#define MASK_SHOT_PORTAL		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW) 							/**< hits solids (not grates) and passes through everything else */
#define MASK_SOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE) 					/**< everything normally solid, except monsters (world+brush only) */
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for player movement, except monsters (world+brush only) */
#define MASK_NPCSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for npc movement, except monsters (world+brush only) */
#define MASK_NPCWORLDSTATIC		(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 					/**< just the world, used for route rebuilding */
#define MASK_SPLITAREAPORTAL	(CONTENTS_WATER|CONTENTS_SLIME) 									/**< These are things that can split areaportals */
#define MASK_AIM				( 0x200400B | CONTENTS_HITBOX ) //this fixes the aimbot not shooting through shit like invis gates
