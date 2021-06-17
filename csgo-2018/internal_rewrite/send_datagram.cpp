#include "mem.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "hooks.hpp"

uintptr_t cl_sendmove = 0;

int __fastcall hooks::send_datagram( INetChannel* channel, void* edx_, void* datagram ) {
	auto send_datagram_o = g_csgo.m_net_channel->get_old_function< decltype( &hooks::send_datagram ) >( 46 );
	if( g_csgo.m_panic || !g_settings.misc.net_fakelag || datagram ) {
		return send_datagram_o( channel, 0, datagram );
	}

	auto cl = g_csgo.m_global_state->get_client_state( );
	if( !cl || cl->m_delta_tick <= 0 ) {
		return send_datagram_o( channel, 0, datagram );
	}

	int backup_state = channel->m_nInReliableState;
	int backup_sequence = channel->m_nInSequenceNr;

	g_cheat.m_extra.add_latency( channel );

	auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );

	int ret = send_datagram_o( channel, 0, datagram );

	channel->m_nInReliableState = backup_state;
	channel->m_nInSequenceNr = backup_sequence;

	return ret;
}