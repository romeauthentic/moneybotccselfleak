#pragma once
#include "UtlMem.h"
#include <inttypes.h>
#include "color.hpp"

static const int END_OF_FREE_LIST = -1;
static const int ENTRY_IN_USE = -2;

struct GlowObject_t {
	uintptr_t ent_ptr;
	fclr_t glow_color;

	bool render_when_occluded;
	bool render_when_unoccluded;
	int pad;

	int next_free_slot;
};

struct GlowObjectManager_t {
	CUtlVector< GlowObject_t > data;
	int first_slot;

	int RegisterGlowObject( IClientEntity* ent ) {
		for( int i{ }; i < data.Count( ); i++ ) {
			if( data[ i ].ent_ptr == ent->get_ehandle( ) ) return 0;
		}
		int index{ };
		if( first_slot == -1 ) index = data.AddToTail( );
		else {
			index = first_slot;
			first_slot = data[ index ].next_free_slot;
		}
		data[ index ].ent_ptr = ent->get_ehandle( );
		data[ index ].glow_color = clr_t( 255, 255, 255, 255 ).to_fclr( );
		data[ index ].render_when_occluded = true;
		data[ index ].render_when_unoccluded = true;
		data[ index ].pad = -1;
		data[ index ].next_free_slot = -2;

		return index;
	}
};