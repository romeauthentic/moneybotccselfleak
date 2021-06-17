#pragma once
#include "util.hpp"
#include "ISurface.hpp"

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

	auto GetName( ) {
		return util::get_vfunc< 0, const char* >( this );
	}

	auto GetTextureGroupName( ) {
		return util::get_vfunc< 1, const char* >( this );
	}

	void IncrementReferenceCount( ) {
		return util::get_vfunc< 12, void >( this );
	}

	void DecrementReferenceCount( ) {
		return util::get_vfunc< 13, void >( this );
	}

	void AlphaModulate( float alpha ) {
		return util::get_vfunc< 27, void >( this, alpha );
	}

	void ColorModulate( float r, float g, float b ) {
		return util::get_vfunc< 28, void >( this, r, g, b );
	}

	void SetMaterialVarFlag( MaterialVarFlags_t flag, bool on ) {
		return util::get_vfunc< 29, void >( this, flag, on );
	}

	bool GetMaterialVarFlag( MaterialVarFlags_t flag ) {
		return util::get_vfunc< 30, bool >( this, flag );
	}

	auto GetAlphaModulation( ) {
		return util::get_vfunc< 44, float >( this );
	}

	void GetColorModulate( float* r, float* g, float* b ) {
		return util::get_vfunc< 45, void >( this, r, g, b );
	}

	void Refresh( ) {
		return util::get_vfunc< 37, void >( this );
	}
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

	ImageFormat GetBackBufferFormat( ) {
		return util::get_vfunc< 36, ImageFormat >( this );
	}

	IMaterial* CreateMaterial( const char* pMaterialName, void* pVMTKeyValues ) {
		return util::get_vfunc< 83, IMaterial* >( this, pMaterialName, pVMTKeyValues );
	}

	IMaterial* FindMaterial( const char* pMaterialName, const char* pTextureGroupName = "Model textures", bool complain = true, const char* pComplainPrefix = nullptr ) {
		return util::get_vfunc< 84, IMaterial* >( this, pMaterialName, pTextureGroupName, complain, pComplainPrefix );
	}

	MaterialHandle_t FirstMaterial( ) {
		return util::get_vfunc< 86, MaterialHandle_t >( this );
	}

	MaterialHandle_t NextMaterial( MaterialHandle_t h ) {
		return util::get_vfunc< 87, MaterialHandle_t >( this, h );
	}

	MaterialHandle_t InvalidMaterial( ) {
		return util::get_vfunc< 88, MaterialHandle_t >( this );
	}

	IMaterial* GetMaterial( MaterialHandle_t h ) {
		return util::get_vfunc< 89, IMaterial* >( this, h );
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