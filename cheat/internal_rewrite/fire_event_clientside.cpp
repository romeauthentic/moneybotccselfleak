#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

bool __fastcall hooks::fire_event_clientside( void* ecx_, void* edx, IGameEvent* event_ ) {
	static auto original_fn = fire_event_clientside_o;

	if( g_ctx.m_local && g_ctx.m_local->is_valid( ) && strstr( event_->GetName( ), xors( "player_death" ) ) )
		g_cheat.m_skins.replace_deaths( event_ );

	g_cheat.m_extra.money_talk( event_ );

	return original_fn( ecx_, edx, event_ );
}