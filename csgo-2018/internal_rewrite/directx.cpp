#include <chrono>
#include <thread>

#include "settings.hpp"
#include "hooks.hpp"
#include "ui.h"
#include "base_cheat.hpp"
#include "context.hpp"
#include "mem.hpp"

using namespace d3d;

float ui::get_csgo_frametime( ) {
	return g_csgo.m_frametime;
}

float ui::anim_time;

void on_d3d( IDirect3DDevice9* device ) {
	constexpr float step = 1.0f / 2.f;
	constexpr float fade_step = 1.0f / 7.f;
	static float anim_time{ };
	static bool flip{ };
	static int anim_progress{ };

	static bool sprites_init{ };
	if( !sprites_init ) {
		ui::setup_sprites( device );
		sprites_init = true;
	}

	if( g_settings.misc.hide_from_obs )
		g_cheat.m_visuals( );

	if( g_settings.menu.open ) {
		if( anim_time >= 0.95f ) {
			flip = true;
		}
		if( anim_time <= 0.05f ) {
			flip = false;
		}

		if( flip )
			anim_time -= fade_step * ui::ui_get_frametime( );
		else
			anim_time += fade_step * ui::ui_get_frametime( );

		anim_time = std::clamp( anim_time, 0.f, 1.0f );

		ui::set_animtime( anim_time );

		anim_progress += ui::ui_get_frametime( ) * step * 255;
		anim_progress = std::clamp( anim_progress, 0, 50 );
		g_d3d.draw_filled_rect( clr_t( 0, 0, 0, anim_progress ),
			0, 0, g_d3d.m_width, g_d3d.m_height );

		ui::render( );
	}
	else {
		anim_progress = 0;
	}
}

long __stdcall hooks::d3d::draw( IDirect3DDevice9* device, D3DPRIMITIVETYPE type, int v_index, uint32_t min_index, uint32_t num_vert, uint32_t start_index, uint32_t prim_count ) {
	static auto draw_o = g_csgo.m_d3d->get_old_function< decltype( &hooks::d3d::draw ) >( 82 );

	static auto ret_addr = pattern::first_code_match( GetModuleHandleA( xors( "studiorender.dll" ) ), xors( "EB 1F FF 75 F0" ), -8 );

	std::function< void*( void** ) > find_ent;
	find_ent = [ & ]( void** ebp ) -> void* {
		if( !ebp )
			return nullptr;

		void** next = *( void*** )( ebp );
		if( ( uintptr_t )( ebp[ 1 ] ) == ret_addr )
			return next[ 4 ];

		return find_ent( next );
	};

	c_base_player* ent = ( c_base_player* )( find_ent( ( void** )( get_baseptr( ) ) ) );
	if( g_ctx.m_local && !!ent && ent->m_iHealth( ) > 0 && ent->m_iTeamNum( ) > 1 && g_settings.visuals.chams.enabled( ) && g_settings.misc.hide_from_obs ) {
		g_cheat.m_chams.d3d_render_chams( ent, type, v_index, min_index, num_vert, start_index, prim_count );
		return true;
	}

	auto result = draw_o( device, type, v_index, min_index, num_vert, start_index, prim_count );
	return result;
}

long __stdcall hooks::d3d::end_scene( IDirect3DDevice9* device ) {
	static auto end_scene_o = g_csgo.m_d3d->get_old_function< decltype( &hooks::d3d::end_scene ) >( 42 );
	if ( g_csgo.m_panic ) return end_scene_o( device );

	static uintptr_t return_address = 0;
	static uintptr_t gameoverlay_return_address = 0;

	g_csgo.m_d3d_thread = std::this_thread::get_id( );

	if( !return_address ) { 
		return_address = ( uintptr_t )( _ReturnAddress( ) );
	}

	if( !gameoverlay_return_address ) {
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery( _ReturnAddress( ), &info, sizeof( MEMORY_BASIC_INFORMATION ) );

		char mod[ MAX_PATH ];
		GetModuleFileNameA( ( HMODULE )info.AllocationBase, mod, MAX_PATH );

		if( strstr( mod, xors( "gameoverlay" ) ) )
			gameoverlay_return_address = ( uintptr_t )( _ReturnAddress( ) );
	}

	if( return_address != ( uintptr_t )( _ReturnAddress( ) ) && !g_settings.misc.hide_from_obs )
		return end_scene_o( device );

	if( gameoverlay_return_address != ( uintptr_t )( _ReturnAddress( ) ) && g_settings.misc.hide_from_obs ) 
		return end_scene_o( device );

	if( !g_csgo.m_panic && g_d3d.run_frame( device ) ) {
		for( auto& it : ::d3d::sprites )
			it->begin( device );
		g_d3d.begin( );

		//if( g_settings.misc.hide_from_obs )
			//g_cheat.m_chams.d3d_render_textures( );

		on_d3d( device );

		std::chrono::high_resolution_clock timer;
		static auto last = timer.now( );
		auto now = timer.now( );

		std::chrono::duration< double > delta = now - last;
		last = timer.now( );

		g_csgo.m_frametime = delta.count( );

		if( !g_con->m_logs.empty( ) && g_settings.menu.logs_enable ) {
			auto& logs = g_con->m_logs;
			float time = ( float )GetTickCount( ) * 0.001f;

			auto data = logs.data( );

			int cur_pos = 2;
			for( int i = logs.size( ) - 1; i >= 0; --i ) {
				if( cur_pos > g_d3d.m_height - 200 ) {
					cur_pos -= 8;
					break;
				}

				float delta = time - data[ i ].m_time;
				if( delta >= 5.0f ) {
					break;
				}

				clr_t col( 255, 255, 255 );
				if( delta > 4.f ) col.a( ) *= ( 6.f - delta );

				g_d3d.draw_text< ALIGN_RIGHT >( fonts.f_con, col, g_d3d.m_width - 5, cur_pos, D3DFONTFLAG_DROPSHADOW, data[ i ].m_msg );
				cur_pos += 10;
			}
		}

		if( g_settings.menu.open || g_con->m_open ) {
			ui::ui_draw_cursor( );
		}

		for( auto& it : ::d3d::sprites )
			it->end( );
		g_d3d.end( );
	}

	return end_scene_o( device );
}

long __stdcall hooks::d3d::present( IDirect3DDevice9* device, RECT* source, RECT* dest, HWND wnd_override, RGNDATA* reg ) {
	static auto present_o = g_csgo.m_d3d->get_old_function< decltype( &hooks::d3d::present ) >( 17 );

	static uintptr_t return_address = 0;
	if( !return_address ) {
		return_address = ( uintptr_t )( _ReturnAddress( ) );
	}

	if( return_address && return_address != ( uintptr_t )( _ReturnAddress( ) ) )
		return present_o( device, source, dest, wnd_override, reg );

	if( g_d3d.run_frame( device ) ) {
		g_d3d.begin( );
		if( g_settings.misc.watermark ) {
			static std::string date;
			std::string str;

			static bool transformed = false;
			if( !transformed ) {
				date += __DATE__;
				std::transform( date.begin( ), date.end( ), date.begin( ),
					[ ]( char c ) { return ::tolower( c ); } );

				transformed = true;
			}

			str = xors( "moneybot | " );
			auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
			if( nci ) {
				char latency_str[ 32 ];
				strenc::w_sprintf_s( latency_str, 32, xors( "lt: %1.2f + %1.2f | " ), nci->GetLatency( 0 ), nci->GetLatency( 1 ) );
				str += latency_str;
			}
			else {
				str += xors( "unconnected | " );
			}

			str += date;

			g_d3d.draw_text< ALIGN_RIGHT >( ::d3d::fonts.f_menu,
				ui::ui_get_text_col( ), g_d3d.m_width - 10, 8,
				D3DFONTFLAG_DROPSHADOW, str.c_str( ) );
		}

		g_con->draw( );
		if( ( g_settings.menu.open || g_con->m_open ) && !g_settings.misc.hide_from_obs ) {
			ui::ui_draw_cursor( );
		}
		g_d3d.end( );
	}

	return present_o( device, source, dest, wnd_override, reg );
}

long __stdcall hooks::d3d::reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params ) {
	static auto reset_o = g_csgo.m_d3d->get_old_function< decltype( &hooks::d3d::reset ) >( 16 );
	if ( g_csgo.m_panic ) {
		return reset_o( device, params );
	}

	for( auto& sprite : ::d3d::sprites )
		sprite->on_reset( );

	g_d3d.on_device_lost( );
	long result = reset_o( device, params );
	g_d3d.on_device_reset( );

	for( auto& sprite : ::d3d::sprites )
		sprite->on_reset_end( );

	return result;
}