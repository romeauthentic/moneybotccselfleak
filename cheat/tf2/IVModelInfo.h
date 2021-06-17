#pragma once
#include "IVEngineClient.h"
#include "IClientEntityList.h"
class IMaterial;
struct mstudiobbox_t {
	int bone;
	int group;
	vec3_t bbmin;
	vec3_t bbmax;
	int szhitboxnameindex;
	int unused[8];
};

enum OverrideType_t {
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};
struct DrawModelState_t;

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

struct mstudiohitboxset_t {
	int sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t *)(((BYTE *)this) + hitboxindex) + i; };
};

struct studiohdr_t {
	unsigned char		pad00[12];
	char				name[64];
	unsigned char		pad01[80];
	int					numbones;
	int					boneindex;
	unsigned char		pad02[12];
	int					hitboxsetindex;
	unsigned char		pad03[228];

	mstudiobone_t *GetBone( int i ) const {
		return reinterpret_cast<mstudiobone_t*>( uintptr_t( this ) + boneindex ) + i;
	};

	mstudiohitboxset_t	*pHitboxSet(int i) const {
		return (mstudiohitboxset_t *)(((unsigned char *)this) + hitboxsetindex) + i;
	};

	mstudiobbox_t *pHitbox(int i, int set) const {
		const mstudiohitboxset_t *s = pHitboxSet(set);
		if(!s)
			return 0;

		return s->pHitbox(i);
	};

	int	iHitboxCount(int set) const {
		const mstudiohitboxset_t *s = pHitboxSet(set);
		if(!s)
			return 0;

		return s->numhitboxes;
	};
};

struct ModelRenderInfo_t {
	vec3_t m_origin;
	vec3_t m_angles;
	//in fucking correct.
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

class IVModelInfo {
public:
	int GetModelIndex( const char* name ) {
		return call_vfunc< int( __thiscall* )( void*, const char* ) >( this, 2 )( this, name );
	}

	const char* GetModelName( const void* model ) {
		return call_vfunc< const char*( __thiscall* )( void*, const void* ) >( this, 3 )( this, model );
	}

	studiohdr_t* GetStudioModel( const void* model ) {
		return call_vfunc< studiohdr_t*( __thiscall* )( void*, const void* ) >( this, 28 )( this, model );
	}

	model_t* FindOrLoadModel( const char* name ) {
		using fn = model_t * ( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 43 )( this, name );
	}
};

class IVModelRender {
public:
	void ForcedMaterialOverride( IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL ) {
		using fn = void( __thiscall* )( void*, IMaterial*, OverrideType_t );
		return util::get_vfunc< fn >( this, 1 )( this, newMaterial, nOverrideType );
	}

	void DrawModelExecute( void* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld ) {
		using fn = void( __thiscall* )( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4* );
		util::get_vfunc< fn >( this, 19 )( this, matctx, state, pInfo, pCustomBoneToWorld );
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

class CStringTable {
public:
	INetworkStringTable * FindTable( const char* table ) {
		using fn = INetworkStringTable * ( __thiscall* )( void*, const char* );
		return util::get_vfunc< fn >( this, 3 )( this, table );
	}
};