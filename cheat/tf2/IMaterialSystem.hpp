#pragma once
#include "util.hpp"
#include "ISurface.h"

using MaterialHandle_t = unsigned short;

enum MaterialVarFlags_t {
	MATERIAL_VAR_DEBUG = 1 << 0,
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = 1 << 1,
	MATERIAL_VAR_NO_DRAW = 1 << 2,
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = 1 << 3,
	MATERIAL_VAR_VERTEXCOLOR = 1 << 4,
	MATERIAL_VAR_VERTEXALPHA = 1 << 5,
	MATERIAL_VAR_SELFILLUM = 1 << 6,
	MATERIAL_VAR_ADDITIVE = 1 << 7,
	MATERIAL_VAR_ALPHATEST = 1 << 8,
	MATERIAL_VAR_MULTIPASS = 1 << 9,
	MATERIAL_VAR_ZNEARER = 1 << 10,
	MATERIAL_VAR_MODEL = 1 << 11,
	MATERIAL_VAR_FLAT = 1 << 12,
	MATERIAL_VAR_NOCULL = 1 << 13,
	MATERIAL_VAR_NOFOG = 1 << 14,
	MATERIAL_VAR_IGNOREZ = 1 << 15,
	MATERIAL_VAR_DECAL = 1 << 16,
	MATERIAL_VAR_ENVMAPSPHERE = 1 << 17,
	MATERIAL_VAR_NOALPHAMOD = 1 << 18,
	MATERIAL_VAR_ENVMAPCAMERASPACE = 1 << 19,
	MATERIAL_VAR_BASEALPHAENVMAPMASK = 1 << 20,
	MATERIAL_VAR_TRANSLUCENT = 1 << 21,
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = 1 << 22,
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = 1 << 23,
	MATERIAL_VAR_OPAQUETEXTURE = 1 << 24,
	MATERIAL_VAR_ENVMAPMODE = 1 << 25,
	MATERIAL_VAR_SUPPRESS_DECALS = 1 << 26,
	MATERIAL_VAR_HALFLAMBERT = 1 << 27,
	MATERIAL_VAR_WIREFRAME = 1 << 28,
};

class ITexture {
public:
	int GetActualWidth( ) {
		return util::get_vfunc< 3, int >( this );
	}

	int GetActualHeight( ) {
		return util::get_vfunc< 4, int >( this );
	}
};

struct MaterialVideoMode_t {
	int m_width;
	int m_height;
	int m_format;
	int m_hz;
};

struct IMaterialSystem_Config_t {
	MaterialVideoMode_t m_vid_mode;
	float m_monitor_gamma;
	float m_monitor_max;
	float m_monitor_min;
	float m_monitor_exp;
	bool m_gamma_enabled;
	bool m_triple_buffer;
	int m_aa_samples;
	int m_force_anisotropic_level;
	int m_skip_mip_levels;
	int m_dx_level;
	int m_flags;
	bool m_edit_mode;
	char m_proxy_test_mode;
	bool m_compressed_textures;
	bool m_filter_lightmaps;
	bool m_filter_textures;
	bool m_reverse_depth;
	bool m_buffer_primitives;
	bool m_draw_flat;
	bool m_measure_fillrate;
	bool m_visualize_fillrate;
	bool m_no_transperacy;
	bool m_software_lighting;
	bool m_allow_cheats;
	char m_show_mipmap;
	bool m_low_res;
	bool m_normal_mapping;
	bool m_mipmap_textures;
	char m_fullbright;
	bool m_fast_nobumd;
	bool m_supress_rendering;
	bool m_drawgray;
	bool m_show_specular;
	bool m_show_diffuse;
	int m_winsize_widthlimit;
	int m_winsize_heightlimit;
	int m_aa_quality;
	bool m_shadow_depth_texture;
	bool m_motion_blur;
	bool m_flashlight;
	bool m_paint_enabled;
	char pad[ 0xC ];
};

class IMaterial {
	char pad[ 0x1C ];
public:
	int m_ref_count;

	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName( ) const = 0;
	virtual const char *	GetTextureGroupName( ) const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual void GetPreviewImageProperties( int *width, int *height,
		ImageFormat *imageFormat, bool* isTranslucent ) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual void GetPreviewImage( unsigned char *data,
		int width, int height,
		ImageFormat imageFormat ) const = 0;
	// 
	virtual int				GetMappingWidth( ) = 0;
	virtual int				GetMappingHeight( ) = 0;

	virtual int				GetNumAnimationFrames( ) = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage( void ) = 0;
	virtual	void			GetMaterialOffset( float *pOffset ) = 0;
	virtual void			GetMaterialScale( float *pScale ) = 0;
	virtual IMaterial		*GetMaterialPage( void ) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual void*	FindVar( const char *varName, bool *found, bool complain = true ) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount( void ) = 0;
	virtual void			DecrementReferenceCount( void ) = 0;

	inline void AddRef( ) { IncrementReferenceCount( ); }
	inline void Release( ) { DecrementReferenceCount( ); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID( void ) const = 0;

	virtual void			GetLowResColorSample( float s, float t, float *color ) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots( ) = 0;

	// Are we translucent?
	virtual bool			IsTranslucent( ) = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested( ) = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit( ) = 0;

	// Gets the vertex format
	virtual void	GetVertexFormat( ) const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy( void ) const = 0;

	virtual bool			UsesEnvCubemap( void ) = 0;

	virtual bool			NeedsTangentSpace( void ) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture( bool bCheckSpecificToThisFrame = true ) = 0;
	virtual bool			NeedsFullFrameBufferTexture( bool bCheckSpecificToThisFrame = true ) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning( void ) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate( float alpha ) = 0;
	virtual void			ColorModulate( float r, float g, float b ) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag( MaterialVarFlags_t flag, bool on ) = 0;
	virtual bool			GetMaterialVarFlag( MaterialVarFlags_t flag ) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity( ) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag( ) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided( ) = 0;

	// Sets the shader associated with the material
	virtual void			SetShader( const char *pShaderName ) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses( void ) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes( void ) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh( ) = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha( void ) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting( void ) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount( ) const = 0;
	virtual void			**GetShaderParams( void ) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial( ) const = 0;

	virtual void			SetUseFixedFunctionBakedLighting( bool bEnable ) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation( ) = 0;
	virtual void			GetColorModulation( float *r, float *g, float *b ) = 0;

	// Gets the morph format
	virtual void			GetMorphFormat( ) const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual void *	FindVarFast( char const *pVarName, unsigned int *pToken ) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams( void* pKeyValues ) = 0;
	virtual const char *	GetShaderName( ) const = 0;

	virtual void			DeleteIfUnreferenced( ) = 0;

	virtual bool			IsSpriteCard( ) = 0;

	virtual void			CallBindProxy( void *proxyData ) = 0;

	virtual IMaterial		*CheckProxyReplacement( void *proxyData ) = 0;

	virtual void			RefreshPreservingMaterialVars( ) = 0;

	virtual bool			WasReloadedFromWhitelist( ) = 0;

	virtual bool			IsPrecached( ) const = 0;
};

class IMaterialSystem
{
public:
	enum RenderTargetSizeMode_t {
		RT_SIZE_NO_CHANGE = 0,			// Only allowed for render targets that don't want a depth buffer
										// (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
										RT_SIZE_DEFAULT = 1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
										RT_SIZE_PICMIP = 2,				// Apply picmip to the render target's width and height.
										RT_SIZE_HDR = 3,					// frame_buffer_width / 4
										RT_SIZE_FULL_FRAME_BUFFER = 4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
										RT_SIZE_OFFSCREEN = 5,			// Target of specified size, don't mess with dimensions
										RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6 // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
	};

	enum MaterialRenderTargetDepth_t {
		MATERIAL_RT_DEPTH_SHARED = 0x0,
		MATERIAL_RT_DEPTH_SEPARATE = 0x1,
		MATERIAL_RT_DEPTH_NONE = 0x2,
		MATERIAL_RT_DEPTH_ONLY = 0x3,
	};

	//these should work, according to ida anyway
	ImageFormat GetBackBufferFormat( ) {
		return util::get_vfunc< 36, ImageFormat >( this );
	}

	IMaterial* CreateMaterial( const char* pMaterialName, void* pVMTKeyValues ) {
		return util::get_vfunc< 72, IMaterial* >( this, pMaterialName, pVMTKeyValues );
	}

	IMaterial* FindMaterial( const char* pMaterialName, const char* pTextureGroupName = "Model textures", bool complain = true, const char* pComplainPrefix = nullptr ) {
		return util::get_vfunc< 73, IMaterial* >( this, pMaterialName, pTextureGroupName, complain, pComplainPrefix );
	}

	MaterialHandle_t FirstMaterial( ) {
		return util::get_vfunc< 75, MaterialHandle_t >( this );
	}

	MaterialHandle_t NextMaterial( MaterialHandle_t h ) {
		return util::get_vfunc< 76, MaterialHandle_t >( this, h );
	}

	MaterialHandle_t InvalidMaterial( ) {
		return util::get_vfunc< 77, MaterialHandle_t >( this );
	}

	IMaterial* GetMaterial( MaterialHandle_t h ) {
		return util::get_vfunc< 78, IMaterial* >( this, h );
	}

	void BeginRenderTargetAllocation( ) {
		return util::get_vfunc< 94, void >( this );
	}

	void EndRenderTargetAllocation( ) {
		return util::get_vfunc< 95, void >( this );
	}

	ITexture* CreateNamedRenderTargetTextureEx( const char* name, int w, int h, RenderTargetSizeMode_t sizeMode,
		ImageFormat format, MaterialRenderTargetDepth_t depth )
	{
		const int textureFlags = 0x4 | 0x8; //TEXTUREFLAGS_CLAMPS, TEXTUREFLAGS_CLAMPT
		const int renderTargetFlags = 0x1; //CREATERENDERTARGETFLAGS_HDR
		return util::get_vfunc< 97, ITexture* >( this, name, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags );
	}

	/*void* GetRenderContext( ) {
	typedef IMatRenderContext*( __thiscall* GetRenderContextFn )( void* );
	return CallVFunction<GetRenderContextFn>( this, 115 )( this );
	}*/
};