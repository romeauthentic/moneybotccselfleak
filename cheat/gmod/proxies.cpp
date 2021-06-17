#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

void __cdecl hooks::lby_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	lby_proxy.get_old_function( )( proxy_data_const, entity, output );

	if( !g_csgo.m_panic ) {
		auto player = ( c_base_player* )( entity );
		if( player && player == g_ctx.m_local ) {
			g_cheat.m_ragebot.m_antiaim->on_lby_proxy( );
		}
	}
}

void __cdecl hooks::last_shot_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	last_shot_proxy.get_old_function( )( proxy_data_const, entity, output );

	if( !g_csgo.m_panic && proxy_data_const ) {
		auto wep = ( c_base_weapon* )( entity );
		if( wep ) {
			auto owner = g_csgo.m_entlist( )->GetClientEntityFromHandle( wep->m_hOwner( ) );
			if( owner && owner->is_valid( ) ) {

				auto records = g_cheat.m_ragebot.m_lagcomp->get_records( owner->ce( )->GetIndex( ), RECORD_NORMAL );
				if( records && records->size( ) ) {
					for( auto& it : *records ) {
						float sim = it.m_flSimulationTime;
						if( sim == wep->get< float >( 0x3314 ) ) {
							it.m_tickcount -= it.m_choked;
							auto ang = math::vector_angles( owner->get_hitbox_pos( 0 ), g_ctx.m_local->get_eye_pos( ) );
							//g_con->log( "found shot record for %d: %f angles: %f %f", owner->ce( )->GetIndex( ), sim, it.m_vecAngles.x, it.m_vecAngles.y );

							it.m_vecAngles = it.m_vecRenderAngles = ang;
							it.m_vecAbsAngles.y = ang.y;
							break;
						}
					}
				}
			}
		}
	}
}