#include "base_cheat.hpp"
#include "hooks.hpp"

int __fastcall hooks::send_datagram( INetChannel* channel, void* edx_, void* datagram ) {
	auto send_datagram_o = g_csgo.m_net_channel->get_old_function< decltype( &hooks::send_datagram ) >( 48 );
	if( g_csgo.m_panic || !g_settings.misc.net_fakelag || datagram ) {
		return send_datagram_o( channel, 0, datagram );
	}

	int backup_state = channel->m_nInReliableState;
	int backup_sequence = channel->m_nInSequenceNr;
	//int backup_o_state = channel->m_nOutReliableState;

	if( g_settings.misc.net_fakelag != 4 && g_settings.misc.net_fakeping_amount )
		g_cheat.m_extra.add_latency( channel );

	auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
	if( nci && g_settings.misc.net_fakelag == 4 && g_cheat.m_ragebot.can_fakeping( ) ) {
		channel->m_nInReliableState = backup_state;
		channel->m_nInSequenceNr = backup_sequence + 10;
	}

	int ret = send_datagram_o( channel, 0, datagram );

	channel->m_nInReliableState = backup_state;
	channel->m_nInSequenceNr = backup_sequence;

	return ret;
}