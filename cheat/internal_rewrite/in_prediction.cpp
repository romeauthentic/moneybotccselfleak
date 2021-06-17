#include "mem.hpp"
#include "hooks.hpp"
#include "context.hpp"

#undef max

bool __fastcall hooks::in_prediction( void* ecx_, void* edx_ ) {
#ifdef HEADER_MODULE
	static auto ret_address = g_header.patterns.in_prediction_retaddr;
#else
	static auto ret_address = pattern::first_code_match( g_csgo.m_chl.dll( ),
		xors( "84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06" ) );
#endif

	if( g_ctx.m_local && ret_address && g_settings.rage.enabled( ) ) {
		stack_t stack( get_baseptr( ) );
		int local_team = g_ctx.m_local->m_iTeamNum( );
		
		if( stack.return_address( ) == ret_address ) {
			for( size_t i{ }; i < 65; ++i ) {
				auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

				if( !ent || !ent->is_valid( ) || ent == g_ctx.m_local || !ent->has_valid_anim( ) )
					continue;

				if( ent->m_iTeamNum( ) != local_team || g_settings.rage.friendlies ) {
					//printf( "model bone counter: %d %d\n", ent->m_iMostRecentModelBoneCounter( ), c_base_player::g_iModelBoneCounter( ) );
					ent->m_iMostRecentModelBoneCounter( ) = c_base_player::g_iModelBoneCounter( );
					ent->m_flLastBoneSetupTime( ) = g_csgo.m_globals->m_curtime;
				}
			}
		}
	}

	return in_prediction_o( ecx_, 0 );
}