#pragma once
#include "color.hpp"
#include "d3d.hpp"
#include "icons.hpp"
#include "d3d_sprite.hpp"
#include "input_system.hpp"

namespace ui
{
	extern float get_csgo_frametime( );
	/*__forceinline auto ui_get_background_texture( ) {
		static auto buffer		 = std::make_shared< byte[ 512 ] >( );
		static auto color		 = D3DCOLOR_RGBA( 27, 27, 27, 233 );
		static auto color_bright = D3DCOLOR_RGBA( 31, 31, 31, 255 );
		static IDirect3DTexture9* texture;

		if ( !texture ) {
			for ( int i = 0; i < 512; i += 4 ) {
				*( ulong_t* )( uintptr_t( buffer.get( ) ) + i ) = !( i % 12 ) ? color : color_bright;
			}

			D3DXCreateTextureFromFileInMemory( g_d3d.get_device( ), buffer.get( ), 512, &texture );
		}

		return texture;
	}*/

	extern float anim_time;
	
	__forceinline void set_animtime( float animtime ) {
		anim_time = animtime;
	}

	__forceinline void setup_sprites( IDirect3DDevice9* device ) {
		//fuck msvc
		icons::sprite_legit.init( device, icons::legit_icon, icons::legit_size, 66, 66 );
		icons::sprite_rage.init( device, icons::rage_icon, icons::rage_size, 66, 66 );
		icons::sprite_visuals.init( device, icons::raw::visuals, icons::visuals_size, 66, 66 );
		icons::sprite_misc.init( device, icons::misc_icon, icons::misc_size, 66, 66 );
		icons::sprite_config.init( device, icons::config_icon, icons::config_size, 66, 66 );
	}

	__forceinline clr_t ui_get_accent_col( ) {
		clr_t col_start = {
			g_settings.menu.menu_color_1r,
			g_settings.menu.menu_color_1g,
			g_settings.menu.menu_color_1b
		};

		clr_t col_end = {
			g_settings.menu.menu_color_2r,
			g_settings.menu.menu_color_2g,
			g_settings.menu.menu_color_2b
		};

		clr_t col = clr_t::blend( col_start, col_end, anim_time );

		return col;
	}

	__forceinline clr_t& ui_get_disabled_col( ) {
		static clr_t col = clr_t( 61, 61, 61, 255 );
		return col;
	}

	__forceinline clr_t& ui_get_bg_col( ) {
		static clr_t col = clr_t( 24, 25, 27, 255 );
		return col;
	}

	__forceinline clr_t& ui_get_text_col( ) {
		static clr_t col = clr_t( 221, 221, 221, 255 );
		return col;
	}

	__forceinline void ui_draw_gradient( int x, int y, int w, int h, clr_t start, 
		clr_t end, GradientType_t type = GRADIENT_HORIZONTAL ) {

		g_d3d.draw_gradient( start, end, x, y, w, h, type );
	}

	__forceinline void ui_draw_line( int x, int y, int x1, int y1, clr_t color ) {
		g_d3d.draw_line( color, x, y, x1, y1 );
	}

	__forceinline void ui_draw_rect( int x, int y, int w, int h, clr_t color ) {
		g_d3d.draw_filled_rect( color, x, y, w, h );
	}

	__forceinline void ui_draw_outlined_rect( int x, int y, int w, int h, clr_t color ) {
		g_d3d.draw_rect( color, x, y, w, h );
	}

	__forceinline void ui_draw_circle( int x, int y, int r, clr_t color, int res = 48 ) {
		g_d3d.draw_circle( color, x, y, r, res );
	}

	__forceinline void ui_draw_filled_circle( int x, int y, int r, clr_t color, int res = 48 ) {
		g_d3d.draw_filled_circle( color, x, y, r, res );
	}

	__forceinline void ui_draw_string( int x, int y, bool center, clr_t color, const char* str, ... ) {
		char buf[ 2048 ]{ };
		va_list list{ };

		__crt_va_start( list, str );
		vsprintf_s( buf, 2048, str, list );
		__crt_va_end( list );

		g_d3d.draw_text( d3d::fonts.f_menu, color, x, y,
			center ? ALIGN_CENTER : ALIGN_LEFT, D3DFONTFLAG_DROPSHADOW, buf );
	}

	__forceinline void ui_get_text_size( int& w, int& h, const char* text, ... ) {
		char*	buf = ( char* )_alloca( 2048 );
		va_list list{ };

		__crt_va_start( list, text );
		vsprintf_s( buf, 2048, text, list );
		__crt_va_end( list );

		w = g_d3d.get_text_width( d3d::fonts.f_menu, 0, buf );
		h = g_d3d.get_text_height( d3d::fonts.f_menu, 0, buf );
	}

	__forceinline void ui_get_cursor_pos( int& x, int& y ) {
		g_input.get_cursor_pos( x, y );
	}

	__forceinline float ui_get_frametime( ) {
		return get_csgo_frametime( );
	}

	__forceinline void ui_draw_cursor( ) {
		const clr_t black( 0, 0, 0, 255 ), accent( ui_get_accent_col( ) );
		int x, y;
		ui_get_cursor_pos( x, y );


		for ( int i{ }; i <= 9; ++i ) {
			ui_draw_line( x, y, x + i, y + 11, accent );
		}

		for ( int i{ }; i <= 7; ++i ) {
			ui_draw_line( x, y + 9 + i, x + i, y + 9, accent );
		}

		for ( int i{ }; i <= 3; ++i ) {
			ui_draw_line( x + 6 + i, y + 11, x, y + i, accent );
		}

		ui_draw_line( x + 5, y + 11, x + 8, y + 18, accent );
		ui_draw_line( x + 4, y + 11, x + 7, y + 18, accent );

		ui_draw_line( x, y, x, y + 17, black );
		ui_draw_line( x, y + 17, x + 3, y + 14, black );
		ui_draw_line( x + 4, y + 14, x + 7, y + 19, black );
		ui_draw_line( x + 7, y + 18, x + 9, y + 18, black );
		ui_draw_line( x + 10, y + 18, x + 7, y + 12, black );
		ui_draw_line( x + 7, y + 12, x + 11, y + 12, black );
		ui_draw_line( x + 11, y + 12, x, y, black );
	}
}