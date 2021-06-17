#include "hooks.hpp"
#include "settings.hpp"

float __fastcall hooks::get_viewmodel_fov( void* ecx, void* edx ) {
	return get_viewmodel_fov_o( ecx, nullptr ) + float( g_settings.misc.viewmodel_fov );
}
