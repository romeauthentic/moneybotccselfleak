#pragma once
#include <windows.h>

#include "util.hpp"
#include "d3d.hpp"
#include "interface.hpp"
#include "netvar_proxy.hpp"

namespace hooks
{
	//hook like the hooker that u are
	
	long __stdcall window_procedure( HWND, uint32_t, uint32_t, long );
	extern decltype( &window_procedure ) window_procedure_o;

	void __fastcall fire_bullets( void*, void*, fire_bullets_info* );
	extern decltype( &fire_bullets ) fire_bullets_o;

	void __fastcall lock_cursor( void*, void* );
	void __fastcall paint( void*, void*, int );
	void __fastcall render_view( void*, void*, CViewSetup&, int, int );
	void __fastcall paint_traverse( void*, void*, unsigned int, bool, bool );
	bool __fastcall create_move( void*, void*, float, user_cmd_t* );
	void __fastcall frame_stage_notify( void*, void*, ClientFrameStage_t );
	void __fastcall run_command( void*, void*, IClientEntity*, user_cmd_t*, IMoveHelper* );
	void __fastcall override_mouse_input( void*, void*, float*, float* );
	void __fastcall draw_model_execute( IVModelRender*, void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4* );
	void __fastcall scene_end( void*, void* );
	bool __fastcall is_hltv_proxy( void*, void* );
	bool __fastcall is_hltv( void* thisptr, uintptr_t pvs_entity );
	float __fastcall get_viewmodel_fov( void*, void* );
	void __fastcall get_screen_size( void*, void*, int&, int& );
	void __fastcall override_view( void* ecx_, void* edx_, CViewSetup* setup );
	void __fastcall emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash, 
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin, 
		const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry );

	bool __fastcall is_connected( void* ecx_, void* edx_ );

	bool __fastcall in_prediction( void* ecx_, void* edx_ );

	void __cdecl lby_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output );
	void __cdecl last_shot_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output );
	int __fastcall send_datagram( INetChannel* channel, void* edx, void* datagram );
	bool __fastcall is_paused( void* ecx_, void* edx_ );
	bool __fastcall do_post_screen_space_effects( void* ecx_, void* edx_, CViewSetup* setup );
	void __fastcall on_screen_size_changed( void* ecx_, void* edx_, int old_h, int old_w );
	void __fastcall play_sound( void* ecx_, void* edx_, const char* name );
	void __fastcall hud_process_input( void* ecx_, void* edx_, bool repaint );

	int __fastcall debug_show_spread_get_int( void* ecx_, void* edx_ );

	extern c_netvar_proxy lby_proxy;
	extern c_netvar_proxy last_shot_proxy;

	//do we wanna only run menu in d3d or visuals too?
	namespace d3d {
		long __stdcall end_scene( IDirect3DDevice9* );
		long __stdcall reset( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
		long __stdcall present( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
	}

	extern bool commit( factory::c_gmod* );
}