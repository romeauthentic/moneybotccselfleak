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

	void __fastcall paint_traverse( void*, void*, unsigned int, bool, bool );
	extern decltype( &paint_traverse ) paint_traverse_o;

	bool __fastcall create_move( void*, void*, float, user_cmd_t* );
	extern decltype( &create_move ) create_move_o;

	//void __fastcall hl_create_move( void*, void*, int, float, bool );
	void __fastcall frame_stage_notify( void*, void*, ClientFrameStage_t );
	extern decltype( &frame_stage_notify ) fsn_o;

	void __fastcall suppress_lists( void*, void*, int, bool );
	extern decltype( &suppress_lists ) suppress_lists_o;

	void __fastcall draw_small_entities( void*, void*, bool );
	extern decltype( &draw_small_entities ) draw_small_entities_o;

	void __fastcall begin_lock( void*, void* );
	extern decltype( &begin_lock ) begin_lock_o;

	void __fastcall end_lock( void*, void* );
	extern decltype( &end_lock ) end_lock_o;

	void __fastcall simulate( void*, void* );
	extern decltype( &simulate ) simulate_o;

	void __fastcall run_command( void*, void*, IClientEntity*, user_cmd_t*, IMoveHelper* );
	extern decltype( &run_command ) run_command_o;

	void __fastcall override_mouse_input( void*, void*, float*, float* );
	extern decltype( &override_mouse_input ) override_mouse_input_o;

	void __fastcall draw_model_execute( IVModelRender*, void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4* );
	extern decltype( &draw_model_execute ) draw_model_execute_o;

	void __fastcall scene_end( void*, void* );
	extern decltype( &scene_end ) scene_end_o;

	float __fastcall get_viewmodel_fov( void*, void* );
	extern decltype( &get_viewmodel_fov ) get_viewmodel_fov_o;

	void __fastcall get_screen_size( void*, void*, int&, int& );
	extern decltype( &get_screen_size ) get_screen_size_o;

	void __fastcall override_view( void* ecx_, void* edx_, CViewSetup* setup );
	extern decltype( &override_view ) override_view_o;

	void __fastcall emit_sound( void* ecx_, void* edx_, void* filter, int ent, int channel, const char* sound, uint32_t hash, 
		const char* sample, float volume, float attenuation, int seed, int flags, int pitch, const vec3_t* origin, 
		const vec3_t* direction, vec3_t* origins, bool update_pos, float sound_time, int speaker_entry, int unk01 );
	extern decltype(&emit_sound) emit_sound_o;

	void __fastcall shut_down( void* ecx_, void* edx_ );
	extern decltype( &shut_down ) shut_down_o;

	void __fastcall lock_cursor( void* ecx_, void* edx_ );
	extern decltype( &lock_cursor ) lock_cursor_o;

	bool __fastcall is_connected( void* ecx_, void* edx_ );
	extern decltype( &is_connected ) is_connected_o;

	bool __fastcall in_prediction( void* ecx_, void* edx_ );
	extern decltype( &in_prediction ) in_prediction_o;

	void __fastcall update_clientside_animation( void* ecx_, void* edx_ );
	extern decltype( &update_clientside_animation ) update_clientside_animation_o;

	bool __fastcall material_system( void* ecx_, void* edx_, IMaterialSystem_Config_t* config, bool forceupdate );
	extern decltype( &material_system ) material_system_o;

	bool __fastcall fire_event_clientside( void* ecx_, void* edx, IGameEvent* event );
	extern decltype( &fire_event_clientside ) fire_event_clientside_o;

	void __cdecl lby_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output );
	void __cdecl last_shot_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output );
	void __cdecl simtime_proxy_fn( const CRecvProxyData* proxy_data_const, void* ent, void* output );
	void __cdecl set_viewmodel_sequence_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output );

	int __fastcall send_datagram( INetChannel* channel, void* edx, void* datagram );
	extern decltype( &send_datagram ) send_datagram_o;

	bool __fastcall do_post_screen_space_effects( void* ecx_, void* edx_, CViewSetup* setup );
	extern decltype( &do_post_screen_space_effects ) do_post_screen_space_effects_o;

	void __fastcall on_screen_size_changed( void* ecx_, void* edx_, int old_h, int old_w );
	extern decltype( &on_screen_size_changed ) on_screen_size_changed_o;

	void __fastcall play_sound( void* ecx_, void* edx_, const char* name );
	extern decltype( &play_sound ) play_sound_o;

	void __fastcall packet_start( void* ecx, void* edx, int in_seq, int out_ack );
	extern decltype( &packet_start ) packet_start_o;

	bool __fastcall process_packet(void *ecx, void *, void *packet, bool has_header);
	extern decltype(&process_packet) process_packet_o;

	int __fastcall cl_csm_shadows_get_int( void* ecx_, void* edx_ );
	int __fastcall debug_show_spread_get_int( void* ecx_, void* edx_ );
	int __fastcall net_showfragments_get_int( void* ecx_, void* edx_ );

	int __fastcall filesystem_allow_load( void* ecx_, void* edx_ );
	extern decltype(&filesystem_allow_load) filesystem_allow_load_o;

	extern c_netvar_proxy lby_proxy;
	extern c_netvar_proxy last_shot_proxy;
	extern c_netvar_proxy simtime_proxy;
	extern c_netvar_proxy set_viewmodel_sequence_proxy;

	//do we wanna only run menu in d3d or visuals too?
	namespace d3d {
		long __stdcall end_scene( IDirect3DDevice9* );
		long __stdcall reset( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
		long __stdcall present( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
		long __stdcall draw( IDirect3DDevice9*, D3DPRIMITIVETYPE, int, uint32_t, uint32_t, uint32_t, uint32_t );
	}

	int __fastcall convar_getint( void* ecx_, void* edx_ );
	extern decltype( &convar_getint ) convar_getint_o;

	extern bool commit( factory::c_csgo* );
}