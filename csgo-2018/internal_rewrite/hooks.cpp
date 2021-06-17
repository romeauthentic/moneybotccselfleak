#include "hooks.hpp"
#include "interface.hpp"

hooks::c_netvar_proxy hooks::lby_proxy;
hooks::c_netvar_proxy hooks::last_shot_proxy;

bool hooks::commit( factory::c_csgo* instance ) {
	while ( !( instance->m_hwnd = FindWindowA( xors( "Valve001" ), 0 ) ) )
		Sleep( 100 );

	window_procedure_o = reinterpret_cast< decltype( window_procedure_o ) >(
		SetWindowLongA( instance->m_hwnd, GWLP_WNDPROC,
			( long )window_procedure )
		);

	lby_proxy.init( g_netvars.get_prop( fnv( "DT_CSPlayer" ), fnv( "m_flLowerBodyYawTarget" ) ), &lby_proxy_fn );
	last_shot_proxy.init( g_netvars.get_prop( fnv( "DT_WeaponCSBaseGun" ), fnv( "m_fLastShotTime" ) ), &last_shot_proxy_fn );

	instance->m_d3d->hook( 42, &hooks::d3d::end_scene );
	instance->m_d3d->hook( 16, &hooks::d3d::reset );
	instance->m_d3d->hook( 17, &hooks::d3d::present );
	//instance->m_d3d->hook( 82, &hooks::d3d::draw );
	instance->m_engine->hook( 93, &hooks::is_hltv_proxy );
	instance->m_engine->hook( 90, &hooks::is_paused );
	instance->m_engine->hook( 27, &hooks::is_connected );
	instance->m_panel->hook( 41, &hooks::paint_traverse );
	instance->m_clientmode->hook( 24, &hooks::create_move );
	instance->m_clientmode->hook( 23, &hooks::override_mouse_input );
	instance->m_clientmode->hook( 35, &hooks::get_viewmodel_fov );
	instance->m_clientmode->hook( 44, &hooks::do_post_screen_space_effects );
	instance->m_clientmode->hook( 18, &hooks::override_view );
	instance->m_surface->hook( 67, &hooks::lock_cursor );
	//instance->m_input->hook( 3, &hooks::hl_create_move );
	instance->m_chl->hook( 36, &hooks::frame_stage_notify );
	//instance->m_chl->hook( 10, &hooks::hud_process_input );
	instance->m_prediction->hook( 19, &hooks::run_command );
	instance->m_prediction->hook( 14, &hooks::in_prediction );
	instance->m_model_render->hook( 21, &hooks::draw_model_execute );
	instance->m_render_view->hook( 9, &hooks::scene_end );
	instance->m_surface->hook( 44, &hooks::get_screen_size );
	instance->m_surface->hook( 116, &hooks::on_screen_size_changed );
	instance->m_surface->hook( 82, &hooks::play_sound );
	instance->m_engine_sound->hook( 5, &hooks::emit_sound );
	instance->m_debug_show_spread->hook( 13, &hooks::debug_show_spread_get_int );
	instance->m_interpolate->hook( 13, &hooks::cl_interpolate_get_int );
	//instance->m_event_mgr->hook( 9, &hooks::fire_event_clientside );
	instance->m_mat_system->hook( 21, &hooks::material_system );
	instance->m_partition->hook( 16, &hooks::suppress_lists );
	instance->m_clientleaf->hook( 39, &hooks::draw_small_entities );
	instance->m_engine_vgui->hook( 39, &hooks::simulate );
	instance->m_mdl_cache->hook( 33, &hooks::begin_lock );
	instance->m_mdl_cache->hook( 34, &hooks::end_lock );
	//instance->m_netshowfragments->hook( 13, &hooks::net_showfragments_get_int );
	//instance->m_trace->hook( 5, &hooks::trace_ray );

	return true;
}
