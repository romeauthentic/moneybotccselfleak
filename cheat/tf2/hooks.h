#pragma once
#include <windows.h>

#include "util.hpp"
#include "d3d.hpp"
#include "interfaces.h"
#include "netvar_proxy.hpp"

namespace hooks
{
	//hook like the hooker that u are

	void __fastcall paint( void* ecx_, void* edx_, int mode );
	long __stdcall window_procedure( HWND, uint32_t, uint32_t, long );
	extern decltype( &window_procedure ) window_procedure_o;
	void __fastcall process_movement( void*, void*, c_base_player*, CMoveData* );
	void __fastcall paint_traverse( void*, void*, unsigned int, bool, bool );
	bool __fastcall create_move( void*, void*, float, user_cmd_t* );
	void __stdcall hl_create_move_gate( int sequence_number, float input_sample_time, bool active );
	void __fastcall hl_create_move( void*, void*, int, float, bool, byte& );
	void __fastcall frame_stage_notify( void*, void*, frame_stages_t );
	void __fastcall override_mouse_input( void*, void*, float*, float* );
	void __fastcall draw_model_execute( IVModelInfo*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4* );
	void __fastcall scene_end( void*, void* );
	float __fastcall get_viewmodel_fov( void*, void* );
	void __fastcall override_view( void* ecx_, void* edx_, CViewSetup* setup );
	void __fastcall emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash,
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin,
		const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry );
	void __fastcall shut_down( void* ecx_, void* edx_ );

	bool __fastcall is_connected( void* ecx_, void* edx_ );

	bool __fastcall in_prediction( void* ecx_, void* edx_ );
	int __fastcall send_datagram( INetChannel* channel, void* edx, void* datagram );
	bool __fastcall do_post_screen_space_effects( void* ecx_, void* edx_, CViewSetup* setup );
	void __fastcall on_screen_size_changed( void* ecx_, void* edx_, int old_h, int old_w );



	//do we wanna only run menu in d3d or visuals too?
	namespace d3d {
		long __stdcall end_scene( IDirect3DDevice9* );
		long __stdcall reset( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
		long __stdcall present( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
		long __stdcall draw( IDirect3DDevice9*, D3DPRIMITIVETYPE, int, uint32_t, uint32_t, uint32_t, uint32_t );
	}

	extern bool commit( tf2::client* );
}