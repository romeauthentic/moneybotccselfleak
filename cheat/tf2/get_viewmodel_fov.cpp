#include "hooks.h"
#include "settings.h"


//this is completely unnecessary, tf2 has unrestricted viewmodel fov
float __fastcall hooks::get_viewmodel_fov( void* ecx, void* edx ) {
	static auto get_viewmodel_fov_o = cl.m_clientmode->get_old_function< decltype( &hooks::get_viewmodel_fov ) >( 32 );

	return get_viewmodel_fov_o( ecx, nullptr );// +float( g_settings.misc.viewmodel_fov );
}
