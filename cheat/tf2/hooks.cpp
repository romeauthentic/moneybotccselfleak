#include "hooks.h"
#include "interfaces.h"
#include "netvars.h"


bool hooks::commit( tf2::client* instance ) {
	while( !( instance->m_hwnd = FindWindowA( xors( "Valve001" ), 0 ) ) )
		Sleep( 100 );

	window_procedure_o = reinterpret_cast< decltype( window_procedure_o ) >(
		SetWindowLongA( instance->m_hwnd, GWLP_WNDPROC,
		( long )window_procedure )
		);

	instance->m_clientmode->hook( 20, &hooks::override_mouse_input );//n
	instance->m_clientmode->hook( 32, &hooks::get_viewmodel_fov );//n
	instance->m_clientmode->hook( 39, &hooks::do_post_screen_space_effects ); //n
	instance->m_clientmode->hook( 16, &hooks::override_view ); //n
	instance->m_clientmode->hook( 21, &hooks::create_move ); //y
	instance->m_chl->hook( 35, &hooks::frame_stage_notify ); //y
	instance->m_chl->hook( 3, &hooks::shut_down ); //n

	//honestly fuck chams and keyvalues they can lick my mothers toes

	instance->m_model_render->hook( 19, &hooks::draw_model_execute ); //n
	instance->m_render_view->hook( 9, &hooks::scene_end ); //n


	instance->m_movement->hook( 1, &hooks::process_movement ); //y
	instance->m_d3d->hook( 42, &hooks::d3d::end_scene ); //y
	instance->m_d3d->hook( 16, &hooks::d3d::reset );//y
	instance->m_d3d->hook( 17, &hooks::d3d::present );//y
	instance->m_panel->hook( 41, &hooks::paint_traverse ); //y

	instance->m_vgui->hook( 13, &hooks::paint ); //y

	instance->m_surface->hook( 116, &hooks::on_screen_size_changed ); //n
	//instance->m_engine_sound->hook( 5, &hooks::emit_sound ); //n

	return true;
}
