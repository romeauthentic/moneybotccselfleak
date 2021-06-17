#include "hooks.hpp"

int __fastcall hooks::cl_csm_shadows_get_int( void* ecx_, void* edx_ ) {
	static auto old_fn = g_csgo.m_csm_shadows->get_old_function< decltype( &cl_csm_shadows_get_int ) >( 13 );

	if( g_settings.visuals.no_cascade_shadows )
		return 0;

	return old_fn( ecx_, edx_ );
}