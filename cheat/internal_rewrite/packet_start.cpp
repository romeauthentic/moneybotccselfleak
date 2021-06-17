#include "hooks.hpp"
#include "context.hpp"

void __fastcall hooks::packet_start( void* ecx, void* edx, int in_seq, int out_ack ) {
	if( !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) ) {
		g_ctx.m_cmd_numbers.clear( );
		*( int* )( uintptr_t( ecx ) + 0x114 ) = in_seq;
		*( int* )( uintptr_t( ecx ) + 0x4d2c ) = out_ack;
	}
	
	// okay now this is epic
	// printf("%08x -> %08x\n", &hooks::packet_start, hooks::packet_start_o);
	
	for( auto cmd_number = g_ctx.m_cmd_numbers.begin( ); cmd_number != g_ctx.m_cmd_numbers.end( ); ) {
		if( *cmd_number == out_ack ) {
			auto cl = ( uintptr_t )( ecx );
	
			*( int* )( cl + 0x114 ) = in_seq;
			*( int* )( cl + 0x4d2c ) = out_ack;
	
			g_ctx.m_cmd_numbers.erase( cmd_number );
			return;
		}
	
		cmd_number++;
	}
}