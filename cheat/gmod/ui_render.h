#pragma once

#include "ui_base_item.h"

namespace ui
{
	//recursively render all items
	static void render_item( base_item* item, int offset = 0, bool allow_rect = true ) {
		if( !item->get_visible( ) ) return;

		item->reset( );
		item->render( );
		bool reset = false;
		RECT original;

		if( allow_rect && item->is_form( ) ) {
			auto device = g_d3d.get_device( );
			device->GetScissorRect( &original );

			auto x = item->get_relative_x( );
			auto y = item->get_relative_y( );

			RECT new_rect{
				x,
				y + 4,
				x + item->w( ),
				y + item->get_total_height( ) - 7
			};

			device->SetScissorRect( &new_rect );
			reset = true;
		}

		bool draw = true;
		if( item->is_form( ) ) {
			auto form_cast = static_cast< c_form* >( item );
			draw = !form_cast->is_collapsed( );
		}

		if( item->get_items( ).size( ) && draw ) {
			//madr0fl
			int* height_offset = ( int* )_alloca( sizeof( int ) * item->get_items( ).size( ) );
			int cur_offset = 0;
			for( int i = ( int )item->get_items( ).size( ) - 1; i >= 0; --i ) {
				auto& cur_item = item->get_items( )[ i ];
				height_offset[ i ] = cur_offset;
				cur_offset += cur_item->get_visible( ) ? cur_item->get_total_height( ) + ITEM_OFFSET : 0;
			}

			int i{ };
			for( auto& it : item->get_items( ) ) {
				item->set_y_offset( height_offset[ i ] );
				render_item( it.get( ), height_offset[ i ], !reset && allow_rect );
				++i;
			}
		}

		if( reset ) {
			auto device = g_d3d.get_device( );
			device->SetScissorRect( &original );
		}
	}
}