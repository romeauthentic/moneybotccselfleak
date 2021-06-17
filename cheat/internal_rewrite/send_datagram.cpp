#include "mem.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "hooks.hpp"

uintptr_t cl_sendmove = 0;

// loopback:reliable state invalid (0).
// loopback:reliable state invalid (0).
// loopback:reliable state invalid (0).
// fucks up here:
// 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C 83 C8 FF 5F 5E 5B 8B E5 5D C2 04 00

int __fastcall hooks::send_datagram( INetChannel* channel, void* edx_, void* datagram ) {
	if( g_csgo.m_panic || !g_settings.misc.net_fakelag || datagram ) {
		return send_datagram_o( channel, 0, datagram );
	}

	auto cl = g_csgo.m_client_state;
	if( !cl || cl->m_delta_tick <= 0 ) {
		return send_datagram_o( channel, 0, datagram );
	}

	int backup_sequence = channel->m_nInSequenceNr;
	int backup_reliable = channel->m_nInReliableState;

	g_cheat.m_extra.add_latency( channel );

	int ret = send_datagram_o( channel, 0, datagram );

	channel->m_nInSequenceNr = backup_sequence;
	channel->m_nInReliableState = backup_reliable;

	return ret;
}