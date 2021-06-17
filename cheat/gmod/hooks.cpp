#include "hooks.hpp"
#include "interface.hpp"

hooks::c_netvar_proxy hooks::lby_proxy;
hooks::c_netvar_proxy hooks::last_shot_proxy;





bool hooks::commit( factory::c_gmod* instance ) {
	while ( !( instance->m_hwnd = FindWindowA( xors( "Valve001" ), 0 ) ) )
		Sleep( 100 );

	window_procedure_o = reinterpret_cast< decltype( window_procedure_o ) >(
		SetWindowLongA( instance->m_hwnd, GWLP_WNDPROC,
			( long )window_procedure )
		);

	instance->m_d3d->hook( 42, &hooks::d3d::end_scene );
	instance->m_d3d->hook( 16, &hooks::d3d::reset );
	instance->m_d3d->hook( 17, &hooks::d3d::present );
	instance->m_surface->hook( 62, &hooks::lock_cursor );
	//instance->m_view_render->hook( 6, &hooks::render_view );
	instance->m_engine_vgui->hook( 13, &hooks::paint );
	//instance->m_engine->hook( 93, &hooks::is_hltv_proxy );
	//instance->m_engine->hook( 91, &hooks::is_paused );	
	//instance->m_engine->hook( 27, &hooks::is_connected );
	//instance->m_panel->hook( 41, &hooks::paint_traverse );
	instance->m_clientmode->hook( 21, &hooks::create_move );
	//instance->m_clientmode->hook( 23, &hooks::override_mouse_input );
	//instance->m_clientmode->hook( 35, &hooks::get_viewmodel_fov );
	//instance->m_clientmode->hook( 44, &hooks::do_post_screen_space_effects );
	instance->m_clientmode->hook( 16, &hooks::override_view );
	//instance->m_chl->hook( 35, &hooks::frame_stage_notify );
	//instance->m_chl->hook( 10, &hooks::hud_process_input );
	instance->m_prediction->hook( 17, &hooks::run_command );
	//instance->m_prediction->hook( 14, &hooks::in_prediction );
	//instance->m_model_render->hook( 21, &hooks::draw_model_execute );
	//instance->m_render_view->hook( 9, &hooks::scene_end );
	//instance->m_surface->hook( 44, &hooks::get_screen_size );
	//instance->m_surface->hook( 116, &hooks::on_screen_size_changed );
	//instance->m_surface->hook( 82, &hooks::play_sound );
	//instance->m_engine_sound->hook( 5, &hooks::emit_sound );
	//instance->m_debug_show_spread->hook( 13, &hooks::debug_show_spread_get_int );

	return true;
}
