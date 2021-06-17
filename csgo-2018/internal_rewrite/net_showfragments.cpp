#include "hooks.hpp"
#include "mem.hpp"
#include "context.hpp"

int __fastcall hooks::net_showfragments_get_int( void* ecx_, void* edx_ ) {
	static auto old_fn = g_csgo.m_netshowfragments->get_old_function< decltype( &net_showfragments_get_int ) >( 13 );

	static auto ret_checkreceivinglist = pattern::first_code_match( g_csgo.m_engine.dll( ), xors( "FF 50 34 8B 1D ? ? ? ? 85 C0 74 16 FF B6" ), 0x3 );
	static auto ret_readsubchanneldata = pattern::first_code_match( g_csgo.m_engine.dll( ), xors( "FF 50 34 85 C0 74 12 53 FF 75 0C" ), 0x3 );

	static auto last_fragment = 0;
	static auto last_time = 0.f;
	
	stack_t stack( get_baseptr( ) );
	auto cl = g_csgo.m_global_state->get_client_state( );
	auto nc = cl->m_netchannel;

	auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );

	if( nci && ( ( g_settings.misc.net_fakeping_amount( ) && g_settings.misc.net_fakeping_active ) || nci->GetLatency( 1 ) > 0.06f ) && g_ctx.m_local ) {
		if( stack.return_address( ) == ret_readsubchanneldata ) {
			auto e = ( uint32_t* )( nc );
			auto data = &e[ 0x54 ];

			auto fragment = data[ 0x43 ];

			if( fragment == last_fragment ) {
				float delta = GetTickCount( ) * 0.001f - last_time;
				
				if( delta <= std::max( nci->GetLatency( 1 ), 0.3f ) ) {
					( ( uint32_t* )data )[ 0x42 ] = 0;
					last_time = GetTickCount( ) * 0.001f;
				}
			}
		}

		if( stack.return_address( ) == ret_checkreceivinglist ) {
			auto e = ( uint32_t* )( nc );
			auto data = &e[ 0x54 ];

			auto fragment = data[ 0x43 ];

			last_fragment = fragment;
			last_time = GetTickCount( ) * 0.001f;
		}
	}

	return old_fn( ecx_, 0 );
}