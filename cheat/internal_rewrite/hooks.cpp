#include "hooks.hpp"
#include "interface.hpp"
#include "detours.h"
#include "wipe.hpp"

//god i'm so cool
namespace hooks {
	decltype( &paint_traverse ) paint_traverse_o;
	decltype( &create_move ) create_move_o;
	decltype( &frame_stage_notify ) fsn_o;
	decltype( &suppress_lists ) suppress_lists_o;
	decltype( &draw_small_entities ) draw_small_entities_o;
	decltype( &begin_lock ) begin_lock_o;
	decltype( &end_lock ) end_lock_o;
	decltype( &simulate ) simulate_o;
	decltype( &run_command ) run_command_o;
	decltype( &override_mouse_input ) override_mouse_input_o;
	decltype( &draw_model_execute ) draw_model_execute_o;
	decltype( &scene_end ) scene_end_o;
	decltype( &get_viewmodel_fov ) get_viewmodel_fov_o;
	decltype( &get_screen_size ) get_screen_size_o;
	decltype( &override_view ) override_view_o;
	decltype( &shut_down ) shut_down_o;
	decltype( &lock_cursor ) lock_cursor_o;
	decltype( &is_connected ) is_connected_o;
	decltype( &in_prediction ) in_prediction_o;
	decltype( &update_clientside_animation ) update_clientside_animation_o;
	decltype( &material_system ) material_system_o;
	decltype( &fire_event_clientside ) fire_event_clientside_o;
	decltype( &send_datagram ) send_datagram_o;
	decltype( &do_post_screen_space_effects ) do_post_screen_space_effects_o;
	decltype( &on_screen_size_changed ) on_screen_size_changed_o;
	decltype( &play_sound ) play_sound_o;
	decltype( &convar_getint ) convar_getint_o;
	decltype( &packet_start ) packet_start_o;
	decltype( &emit_sound ) emit_sound_o;
	decltype( &process_packet ) process_packet_o;
	decltype( &filesystem_allow_load ) filesystem_allow_load_o;

}

hooks::c_netvar_proxy hooks::lby_proxy;
hooks::c_netvar_proxy hooks::last_shot_proxy;
hooks::c_netvar_proxy hooks::simtime_proxy;
hooks::c_netvar_proxy hooks::set_viewmodel_sequence_proxy;

__declspec( safebuffers ) bool hooks::commit( factory::c_csgo* instance ) {
	DELETE_START( 5 );

	while ( !( instance->m_hwnd = FindWindowA( xors( "Valve001" ), 0 ) ) )
		Sleep( 100 );

	window_procedure_o = reinterpret_cast<decltype( window_procedure_o )>(
		SetWindowLongA( instance->m_hwnd, GWLP_WNDPROC,
		(long) window_procedure )
		);

	lby_proxy.init( g_netvars.get_prop( fnv( "DT_CSPlayer" ), fnv( "m_flLowerBodyYawTarget" ) ), &lby_proxy_fn );
	last_shot_proxy.init( g_netvars.get_prop( fnv( "DT_WeaponCSBaseGun" ), fnv( "m_fLastShotTime" ) ), &last_shot_proxy_fn );
	simtime_proxy.init( g_netvars.get_prop( fnv( "DT_BaseEntity" ), fnv( "m_flSimulationTime" ) ), &simtime_proxy_fn );
	set_viewmodel_sequence_proxy.init( g_netvars.get_prop( fnv( "DT_BaseViewModel" ), fnv( "m_nSequence" ) ), &set_viewmodel_sequence_proxy_fn );

	// god i'm thriving the d
	auto* d = &memory::detours;

	instance->m_d3d->hook( 42, &hooks::d3d::end_scene );
	instance->m_d3d->hook( 16, &hooks::d3d::reset );
	instance->m_d3d->hook( 17, &hooks::d3d::present );
	//instance->m_d3d->hook( 82, &hooks::d3d::draw );
	//instance->m_engine->hook( 93, &hooks::is_hltv_proxy );
	//instance->m_engine->hook( 90, &hooks::is_paused );
	//instance->m_engine->hook( 27, &hooks::is_connected );

	instance->m_debug_show_spread->hook( 13, &hooks::debug_show_spread_get_int );
	instance->m_netshowfragments->hook( 13, &hooks::net_showfragments_get_int );
	instance->m_csm_shadows->hook( 13, &hooks::cl_csm_shadows_get_int );

	paint_traverse_o = d->create_hook( &hooks::paint_traverse, instance->m_panel->get_old_function< void* >( 41 ) );
	create_move_o = d->create_hook( &hooks::create_move, instance->m_clientmode->get_old_function< void* >( 24 ) );
	override_mouse_input_o = d->create_hook( &hooks::override_mouse_input, instance->m_clientmode->get_old_function< void* >( 23 ) );
	get_viewmodel_fov_o = d->create_hook( &hooks::get_viewmodel_fov, instance->m_clientmode->get_old_function< void* >( 35 ) );
	do_post_screen_space_effects_o = d->create_hook( &hooks::do_post_screen_space_effects, instance->m_clientmode->get_old_function< void* >( 44 ) );
	override_view_o = d->create_hook( &hooks::override_view, instance->m_clientmode->get_old_function< void* >( 18 ) );
	lock_cursor_o = d->create_hook( &hooks::lock_cursor, instance->m_surface->get_old_function< void* >( 67 ) );
	run_command_o = d->create_hook( &hooks::run_command, instance->m_prediction->get_old_function< void* >( 19 ) );
	in_prediction_o = d->create_hook( &hooks::in_prediction, instance->m_prediction->get_old_function< void* >( 14 ) );
	draw_model_execute_o = d->create_hook( &hooks::draw_model_execute, instance->m_model_render->get_old_function< void* >( 21 ) );
	scene_end_o = d->create_hook( &hooks::scene_end, instance->m_render_view->get_old_function< void* >( 9 ) );
	get_screen_size_o = d->create_hook( &hooks::get_screen_size, instance->m_surface->get_old_function< void* >( 44 ) );
	is_connected_o = d->create_hook( &hooks::is_connected, instance->m_engine->get_old_function< void* >( 27 ) );
	//
	//// this has annoyed me for so long
	emit_sound_o = d->create_hook( &hooks::emit_sound, instance->m_engine_sound->get_old_function< void* >( 5 ) );
	//
	on_screen_size_changed_o = d->create_hook( &hooks::on_screen_size_changed, instance->m_surface->get_old_function< void* >( 116 ) );
	//play_sound_o = d->create_hook( &hooks::play_sound, instance->m_surface->get_old_function< void* >( 82 ) ); // no longer used in panorama
	material_system_o = d->create_hook( &hooks::material_system, instance->m_mat_system->get_old_function< void* >( 21 ) );
	suppress_lists_o = d->create_hook( &hooks::suppress_lists, instance->m_partition->get_old_function< void* >( 16 ) );
	draw_small_entities_o = d->create_hook( &hooks::draw_small_entities, instance->m_clientleaf->get_old_function< void* >( 42 ) );
	simulate_o = d->create_hook( &hooks::simulate, instance->m_engine_vgui->get_old_function< void* >( 39 ) );
	begin_lock_o = d->create_hook( &hooks::begin_lock, instance->m_mdl_cache->get_old_function< void* >( 33 ) );
	end_lock_o = d->create_hook( &hooks::end_lock, instance->m_mdl_cache->get_old_function< void* >( 34 ) );
	//
	auto packet_start = pattern::first_code_match< void* >( instance->m_engine.dll( ), xors( "55 8B EC 8B 45 08 89 81 ? ? ? ? 8B 45 0C 89 81 ? ? ? ? 5D C2 08 00 ? ? ? ? ? ? ? 56" ) );
	packet_start_o = d->create_hook( &hooks::packet_start, packet_start );
	//
	auto process_packet = pattern::first_code_match< void* >( instance->m_engine.dll( ), xors( "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B D9" ) );
	process_packet_o = d->create_hook( &hooks::process_packet, process_packet );
	//
	auto update_clientside_anim = pattern::first_code_match< void* >( instance->m_chl.dll( ), xors( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36" ) );
	update_clientside_animation_o = d->create_hook( &hooks::update_clientside_animation, update_clientside_anim );
	//
	auto send_datagram = pattern::first_code_match< void* >( instance->m_engine.dll( ), xors( "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18" ) );
	send_datagram_o = d->create_hook( &hooks::send_datagram, send_datagram );
	
	auto filesystem =  **reinterpret_cast<void***>( pattern::first_code_match<>( instance->m_engine.dll( ), xors( "8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6" ) ) + 0x2 );
	filesystem_allow_load_o = d->create_hook( &hooks::filesystem_allow_load, ( *(void***) filesystem )[ 128 ] ); // i do not feel like sigging this

	d->enable( );

	return true;

	DELETE_END( 5 );
}
