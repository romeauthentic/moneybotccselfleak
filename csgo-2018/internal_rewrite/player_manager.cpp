#include "player_manager.hpp"
#include "base_cheat.hpp"
#include "context.hpp"

namespace features
{
	void c_player_record::update( int ent_index ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		if( !ent ) { clear( ); return; }

		if( ent != m_ent || ent->m_flSimulationTime( ) > g_csgo.m_globals->m_curtime + 1.f )
			clear( );

		m_ent = ent;
		if( !ent->is_valid( ) ) 
			return;

		update_simtime( );
		update_antiaim( );

		update_cheater( );
	}

	bool c_player_manager::is_cheater( int ent_index ) {
#ifdef _DEBUG
		static con_var< bool > dbg_nocheater{ &data::holder_, fnv( "dbg_nocheater" ) };
		if( dbg_nocheater( ) )
			return true;
#endif

		return m_players[ ent_index ].is_cheater( );
	}

	void c_player_manager::frame_stage_notify( ) {
		if( !g_ctx.m_local ) return;

		for( int i{ 1 }; i < 65; ++i ) {
			if( i == g_csgo.m_engine( )->GetLocalPlayer( ) )
				continue;

			m_players[ i ].update( i );
		}
	}
}