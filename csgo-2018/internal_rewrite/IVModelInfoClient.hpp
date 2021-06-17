#pragma once
#include "IVEngineClient.hpp"
#include "IClientEntityList.hpp"

enum BoneMask_t {
	BONE_USED_MASK = 0x0007FF00,
	BONE_USED_BY_ANYTHING = 0x0007FF00,
	BONE_USED_BY_HITBOX = 0x00000100,	// bone (or child) is used by a hit box
	BONE_USED_BY_ATTACHMENT = 0x00000200,	// bone (or child) is used by an attachment point
	BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
	BONE_USED_BY_VERTEX_LOD0 = 0x00000400,	// bone (or child) is used by the toplevel model via skinned vertex
	BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
	BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
	BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
	BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
	BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
	BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
	BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
	BONE_USED_BY_BONE_MERGE = 0x00040000	// bone is available for bone merge to occur against it
};

struct DrawModelState_t;

class INetworkStringTable;

struct mstudiobbox_t {
	int bone;
	int group;
	vec3_t bbmin;
	vec3_t bbmax;
	int szhitboxnameindex;
	int m_iPad01[ 3 ];
	float m_flRadius;
	int m_iPad02[ 4 ];
};

struct mstudiohitboxset_t {
	int sznameindex;
	const char* pszName( ) const { return reinterpret_cast< const char* >( uintptr_t( this ) + sznameindex ); }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t* pHitbox( int i ) const { return reinterpret_cast< mstudiobbox_t* >( uintptr_t( this ) + hitboxindex ) + i; };
};

struct mstudiobone_t {
	int sznameindex;
	char* GetName( ) const { return reinterpret_cast< char* >( reinterpret_cast< uintptr_t >( this ) ) + sznameindex; }
	int parent;
	int bonecontroller[ 6 ];
	vec3_t pos;
	float quat[ 4 ];
	vec3_t rot;
	vec3_t posscale;
	vec3_t rotscale;
	matrix3x4 poseToBone;
	float qAlignment[ 4 ];
	int flags;
	int proctype;
	int procindex;
	mutable int physicsbone;
	void* GetProcedure( ) const { return procindex == 0 ? nullptr : reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( this ) + procindex ); };
	int surfacepropidx;
	char* GetSurfaceProps( ) const { return reinterpret_cast<char*>( reinterpret_cast<uintptr_t>( this ) ) + surfacepropidx; }
	int contents;
	unsigned char pad[ 32 ];
};

struct mstudioseqdesc_t {
	int					baseptr;
	int					szlabelindex;
	int					szactivitynameindex;
	int					flags;		// looping/non-looping flags
	int					activity;	// initialized at loadtime to game DLL values
	int					actweight;
	int					numevents;
	int					eventindex;
	vec3_t				bbmin;		// per sequence bounding box
	vec3_t				bbmax;
	int					numblends;
	int					animindexindex;
	int					movementindex;	// [blend] float array for blended movement
	int					groupsize[ 2 ];
	int					paramindex[ 2 ];	// X, Y, Z, XR, YR, ZR
	float				paramstart[ 2 ];	// local (0..1) starting value
	float				paramend[ 2 ];	// local (0..1) ending value
	int					paramparent;
	float				fadeintime;		// ideal cross fate in time (0.2 default)
	float				fadeouttime;	// ideal cross fade out time (0.2 default)
	int					localentrynode;		// transition node at entry
	int					localexitnode;		// transition node at exit
	int					nodeflags;		// transition rules
	float				entryphase;		// used to match entry gait
	float				exitphase;		// used to match exit gait
	float				lastframe;		// frame that should generation EndOfSequence
	int					nextseq;		// auto advancing sequences
	int					pose;			// index of delta animation between end and nextseq
	int					numikrules;
	int					numautolayers;	//
	int					autolayerindex;
	int					weightlistindex;
	int					posekeyindex;
	int					numiklocks;
	int					iklockindex;
	int					keyvalueindex;
	int					keyvaluesize;
	int					cycleposeindex;		// index of pose parameter to use as cycle index
	int					activitymodifierindex;
	int					numactivitymodifiers;
	int					unused[ 5 ];		// remove/add as appropriate (grow back to 8 ints on version change!)
};

struct studiohdr_t {
	unsigned char pad00[ 12 ];
	char name[ 64 ];
	unsigned char pad01[ 80 ];
	int numbones;
	int boneindex;
	unsigned char pad02[ 12 ];
	int hitboxsetindex;
	unsigned char pad03[ 8 ];
	int	numlocalseq;
	int	localseqindex;
	mutable int                     activitylistversion;
	mutable int                     eventsindexed;

	int                                     numtextures;
	int                                     textureindex;

	int                                     numcdtextures;
	int                                     cdtextureindex;
	unsigned char pad04[ 188 ];

	mstudiobone_t *GetBone( int i ) const {
		return reinterpret_cast<mstudiobone_t*>( uintptr_t( this ) + boneindex ) + i;
	};

	mstudiohitboxset_t* pHitboxSet( int i ) const {
		return reinterpret_cast<mstudiohitboxset_t*>( uintptr_t( this ) + hitboxsetindex ) + i;
	};

	mstudiobbox_t* pHitbox( int i, int set ) const {
		const mstudiohitboxset_t* s = pHitboxSet( set );
		if ( !s )
			return nullptr;

		return s->pHitbox( i );
	};

	int iHitboxCount( int set ) const {
		const mstudiohitboxset_t* s = pHitboxSet( set );
		if ( !s )
			return 0;

		return s->numhitboxes;
	};
};

struct ModelRenderInfo_t {
	vec3_t m_origin;
	vec3_t m_angles;
	char pad[ 0x4 ];
	void* m_renderable;
	const model_t* m_model;
	const matrix3x4* m_model2world;
	const matrix3x4* m_light_offset;
	const vec3_t* m_light_origin;
	int m_flags;
	int m_entity_index;
	int m_skin;
	int m_body;
	int m_hitboxset;
	uint16_t* m_instance;

	ModelRenderInfo_t( ) {
		m_model2world = nullptr;
		m_light_offset = nullptr;
		m_light_origin = nullptr;
	}
};

class IVModelInfoClient {
public:
	model_t* GetModel( int index ) {
		using fn = model_t*( __thiscall* )( void*, int );
		return util::get_vfunc< fn >( this, 1 )( this, index );
	}

	int GetModelIndex( const char* name ) {
		using fn = int( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 2 )( this, name );
	}

	const char* GetModelName( const model_t* model ) {
		using fn = const char* ( __thiscall* )( void*, const model_t* );
		return util::get_vfunc< fn >( this, 3 )( this, model );
	}

	studiohdr_t* GetStudiomodel( const model_t* mod ) {
		using fn = studiohdr_t* ( __thiscall* )( void*, const model_t* );
		return util::get_vfunc< fn >( this, 30 )( this, mod );
	}

	model_t* FindOrLoadModel( const char* name ) {
		using fn = model_t* ( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 43 )( this, name );
	}

	void GetModelMaterials( const model_t *model, int count, class IMaterial** ppMaterial ) {
		using fn = const void( __thiscall* )( void*, const model_t*, int, IMaterial** );
		util::get_vfunc< fn >( this, 19 )( this, model, count, ppMaterial );
	}
};

enum OverrideType_t {
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

class IVModelRender {
public:
	void ForcedMaterialOverride( IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL, int unk = 0 ) {
		using fn = void( __thiscall* )( void*, IMaterial*, OverrideType_t, int );
		return util::get_vfunc< fn >( this, 1 )( this, newMaterial, nOverrideType, unk );
	}

	void DrawModelExecute( void* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld ) {
		using fn = void( __thiscall* )( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4* );
		util::get_vfunc< fn >( this, 21 )( this, matctx, state, pInfo, pCustomBoneToWorld );
	}
};

class CStringTable {
public:
	INetworkStringTable* FindTable( const char* table ) {
		using fn = INetworkStringTable * ( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 3 )( this, table );
	}
};

class INetworkStringTable {
public:

	virtual					~INetworkStringTable( void ) {};

	// Table Info
	virtual const char		*GetTableName( void ) const = 0;
	virtual int				GetTableId( void ) const = 0;
	virtual int				GetNumStrings( void ) const = 0;
	virtual int				GetMaxStrings( void ) const = 0;
	virtual int				GetEntryBits( void ) const = 0;

	// Networking
	virtual void			SetTick( int tick ) = 0;
	virtual bool			ChangedSinceTick( int tick ) const = 0;

	// Accessors (length -1 means don't change user data if string already exits)
	virtual int				AddString( bool bIsServer, const char *value, int length = -1, const void *userdata = 0 ) = 0;

	virtual const char		*GetString( int stringNumber ) = 0;
	virtual void			SetStringUserData( int stringNumber, int length, const void *userdata ) = 0;
	virtual const void		*GetStringUserData( int stringNumber, int *length ) = 0;
	virtual int				FindStringIndex( char const *string ) = 0; // returns INVALID_STRING_INDEX if not found

																	   // Callbacks
	virtual void			SetStringChangedCallback( void *object, void* changeFunc ) = 0;
};