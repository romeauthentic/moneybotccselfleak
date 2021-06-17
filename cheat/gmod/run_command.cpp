#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

void __fastcall hooks::run_command( void* ecx_, void* edx_, IClientEntity* entity, user_cmd_t* ucmd, IMoveHelper* move_helper) {
	static auto run_command_o = g_gmod.m_prediction->get_old_function< decltype( &hooks::run_command ) >( 17 );
	if( g_gmod.m_panic ) return run_command_o( ecx_, edx_, entity, ucmd, move_helper );

	if( g_settings.misc.no_recoil ) {
		vec3_t angle;
		g_gmod.m_engine( )->GetViewAngles( angle );
		run_command_o( ecx_, edx_, entity, ucmd, move_helper ); // weapon events are done during prediction, any viewangle change( could be recoil could be not ) during these weapon events will be changed back to its original state. 
		g_gmod.m_engine( )->SetViewAngles( angle );
	}
	else {
		run_command_o( ecx_, 0, entity, ucmd, move_helper );
	}
}
