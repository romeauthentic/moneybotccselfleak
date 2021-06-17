#include "hooks.hpp"

#include <intrin.h>
#include "mem.hpp"

void __fastcall hooks::trace_ray( void* ecx_, void* edx_, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, CBaseTrace* pTrace ) {
	static auto trace_ray_o = g_csgo.m_trace->get_old_function< decltype( &trace_ray ) >( 5 );

/*	if( std::this_thread::get_id( ) == g_csgo.m_d3d_thread ) {
		void* address = _ReturnAddress( );
		//MEMORY_BASIC_INFORMATION info;
		//VirtualQuery( address, &info, sizeof( MEMORY_BASIC_INFORMATION ) );

		char mod[ MAX_PATH ];
		//GetModuleFileNameA( ( HMODULE )info.AllocationBase, mod, MAX_PATH );

		//printf( "TRACERAY CALLED FROM D3D\n" );
		stack_t stack( get_baseptr( ) );

		int i{ };
		for( ; stack.get( ) > 0x100; stack = stack.next( ), i++ ) {
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery( ( void* )( stack.return_address( ) ), &info, sizeof( MEMORY_BASIC_INFORMATION ) );

			GetModuleFileNameA( ( HMODULE )info.AllocationBase, mod, MAX_PATH );

			if( strstr( mod, "moneybot" ) )
				printf( "stack frame: %d retaddr: %08x %s\n", i, stack.return_address( ), mod );
		}
	}*/

	return trace_ray_o( ecx_, edx_, ray, fMask, pTraceFilter, pTrace );
}