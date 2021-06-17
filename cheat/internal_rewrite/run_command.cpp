#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

void __fastcall hooks::run_command( void* ecx_, void* edx_, IClientEntity* entity, user_cmd_t* ucmd, IMoveHelper* move_helper ) {
	if( !g_csgo.m_panic && g_settings.rage.anti_aim( ) && entity == g_ctx.m_local->ce( ) ) {
		
	}
	
	run_command_o( ecx_, edx_, entity, ucmd, move_helper );
}