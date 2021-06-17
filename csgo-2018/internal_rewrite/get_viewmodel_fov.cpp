#include "hooks.hpp"
#include "settings.hpp"

float __fastcall hooks::get_viewmodel_fov( void* ecx, void* edx ) {
	static auto get_viewmodel_fov_o = g_csgo.m_clientmode->get_old_function< decltype( &hooks::get_viewmodel_fov ) >( 35 );

	return get_viewmodel_fov_o( ecx, nullptr ) + float( g_settings.misc.viewmodel_fov );
}
