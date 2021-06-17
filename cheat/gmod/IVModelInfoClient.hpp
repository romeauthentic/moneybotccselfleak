#pragma once
#include "IVEngineClient.hpp"
#include "IClientEntityList.hpp"

struct DrawModelState_t;

struct mstudiobbox_t
{
	int bone;
	int group;
	vec3_t min;
	vec3_t max;
	int	szhitboxnameindex;

	const char* get_name( ) {
		if( szhitboxnameindex == 0 )
			return "";

		return ( ( const char* )this ) + szhitboxnameindex;
	}

	int unused[ 8 ];
};

struct mstudiobone_t
{
	int	sznameindex;
	inline char * const get_name( void ) const { return ( ( char * )this ) + sznameindex; }
	int parent;
	char pad01[ 152 ];
	int flags;
	char pad02[ 52 ];
};

struct mstudiohitboxset_t
{
	int					sznameindex;
	inline char * const	get_name( void ) const { return ( ( char * )this ) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t *get_hitbox( int i ) const { return ( mstudiobbox_t * )( ( ( unsigned char * )this ) + hitboxindex ) + i; };
};

struct studiohdr_t
{
	char pad00[ 12 ];
	char name[ 64 ];
	char pad01[ 80 ];
	int numbones;
	int boneIndex;
	inline mstudiobone_t *get_bone( int i ) { return ( mstudiobone_t * )( ( ( unsigned char* )this ) + boneIndex ) + i; }
	char pad02[ 8 ];
	int numhitboxsets;
	int hitboxsetindex;

	mstudiohitboxset_t* get_hitbox_set( int i ) {
		return ( mstudiohitboxset_t * )( ( ( char * )this ) + hitboxsetindex ) + i;
	}

	char pad03[ 24 ];
	int numtextures;
	inline mstudiobbox_t* get_hit_box( int index ) { return ( ( mstudiohitboxset_t* )( ( unsigned char* )this + hitboxsetindex ) )->get_hitbox( index ); }
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
		return util::get_vfunc< fn >( this, 28 )( this, mod );
	}

	void GetModelMaterials( const model_t *model, int count, class IMaterial** ppMaterial ) {
		using fn = const void( __thiscall* )( void*, const model_t*, int, IMaterial** );
		util::get_vfunc< fn >( this, 16 )( this, model, count, ppMaterial );
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