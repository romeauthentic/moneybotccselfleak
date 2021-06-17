#include "hooks.h"
#include "interfaces.h"

void __fastcall hooks::process_movement( void* thisptr, void* edx_, c_base_player* player, CMoveData* move ) {
	static auto old_fn = cl.m_movement->get_old_function< decltype( hooks::process_movement )* >( 1 );

	if ( cl.m_input( )->IsButtonDown( MOUSE_MIDDLE ) ) {
		move->m_outStepHeight = -1.0f;
	}

	return old_fn( thisptr, edx_, player, move );
}