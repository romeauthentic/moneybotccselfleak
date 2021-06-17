#pragma once

#include "ui_base_item.h"
#include "d3d.hpp"

namespace ui
{
	class c_color_picker : public base_item {
		static constexpr int BOX_WIDTH = 106;
		static constexpr int BOX_HEIGHT = 125;
	public:
		c_color_picker( int x, int y, int w, const char* name, clr_t* setting ) :
			base_item( x, y, w, 4, name ), m_setting( setting ),
			m_has_text( true ) { }

		c_color_picker( int x, int y, int w, clr_t* setting ) :
			base_item( x, y, w, 8, xors( "COLOR_PICKER" ) ), m_setting( setting ),
			m_has_text( false ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( );

			int cursor_x, cursor_y;
			ui_get_cursor_pos( cursor_x, cursor_y );

			if ( m_has_text ) y += 12;

			if ( !m_active ) {
				return cursor_x >= x && cursor_x <= x + m_width
					&& cursor_y >= y && cursor_y <= y + m_height;
			}

			return cursor_x >= x && cursor_x <= x + BOX_WIDTH + 23
				&& cursor_y >= y && cursor_y <= y + BOX_HEIGHT + 2;
		}

		virtual int get_total_height( ) const override {
			return m_has_text ? ( m_height + 12 ) : m_height;
		}

		void input( ) {
			bool active_backup = m_active;
			bool active_changed = false;

			if ( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				if ( !m_active ) {
					m_mouse_held = true;
				}

				m_active = true;
			}
			else if ( m_active && !is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				m_active = false;
			}
			else {
				m_mouse_held = false;
			}

			active_changed = active_backup != m_active;
			if ( active_changed ) {
				set_disabled_callbacks( m_active );
			}

			m_hue = m_setting->hue( );
			if ( m_hue > 1.0f ) {
				m_hue -= 359.f;
			}

			m_saturation = m_setting->saturation( );
			m_brightness = m_setting->brightness( ) / 255.f;
			m_alpha = m_setting->a( );
		}

		void output( ) {
			*m_setting = clr_t::from_hsb( m_hue, m_saturation, m_brightness );
			m_setting->a( ) = m_alpha;
		}

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			if ( m_has_text ) {
				ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );
				y += 12;
			}

			input( );
			
			if ( m_active ) {
				RECT old_rect;
				g_d3d.get_device( )->GetScissorRect( &old_rect );
				
				RECT new_rect{
					x - 1, y - 1, 
					x + BOX_WIDTH + 22,
					y + BOX_HEIGHT + 2
				};

				g_d3d.get_device( )->SetScissorRect( &new_rect );

				int mouse_x, mouse_y;
				ui_get_cursor_pos( mouse_x, mouse_y );

				clr_t bg_col( 0, 0, 0, 90 );
				ui_draw_rect( x, y, BOX_WIDTH + 20, BOX_HEIGHT, bg_col );
				ui_draw_outlined_rect( x - 1, y - 1, BOX_WIDTH + 22, BOX_HEIGHT + 2, ui_get_accent_col( ) );

				for ( int i{ }; i < 100; i += 3 ) {
					for ( int i2{ }; i2 < 100; i2 += 3 ) {
						ui_draw_rect( x + i + 1, y + i2 + 1, 3, 3,
							clr_t::from_hsb( m_hue, float( i2 ) * 0.01f, float( i ) * 0.01f ) );
					}
				}

				if ( g_input.is_key_pressed( KEYS_MOUSE1 ) && !m_mouse_held
					&& mouse_x > x && mouse_x <= x + 100
					&& mouse_y > y && mouse_y <= y + 100 ) {

					int mouse_x_c = std::clamp( mouse_x, x, x + 100 );
					int mouse_y_c = std::clamp( mouse_y, y, y + 100 );

					int delta_y = std::clamp( mouse_y_c - y, 0, 100 );
					int delta_x = std::clamp( mouse_x_c - x, 0, 100 );

					m_saturation = float( delta_y ) * 0.01f;
					m_brightness = float( delta_x ) * 0.01f;
				}

				auto is_hue_slider_hovered = [&]( ) -> bool {
					return mouse_x > x + 110 && mouse_x < x + 122
						&& mouse_y > y && mouse_y < y + 100;
				};

				auto draw_slider_hue = [&]( ) {
					for ( int i{ }; i < 100; ++i ) {
						auto cur_col = clr_t::from_hsb( float( i ) * 0.01f, m_saturation, m_brightness );

						ui_draw_rect( x + 110, y + i + 1, 12, 2, cur_col );
					}

					ui_draw_outlined_rect( x + 109, y + int( m_hue * 100.f ) + 1, 14, 3,
						is_hue_slider_hovered( ) ? ui_get_text_col( ) : ui_get_disabled_col( ) );
				};

				auto is_alpha_slider_hovered = [&]( ) -> bool {
					return mouse_x > x + 1 && mouse_x < x + 122
						&& mouse_y > y + 110 && mouse_y < y + 124;
				};

				auto draw_slider_alpha = [&]( ) {
					for ( int i{ 121 }; i >= 0; --i ) {
						auto col = *m_setting;
						col.a( ) = ( int )( float( i ) * 255.f / 121.f );

						ui_draw_rect( x + i + 1, y + 110, 1, 12, col );
					}

					int a_pos = ( int )( float( m_alpha ) * 121.f / 255.f );
					ui_draw_outlined_rect( x + 1 + a_pos, y + 109, 3, 14,
						is_alpha_slider_hovered( ) ? ui_get_text_col( ) : ui_get_disabled_col( ) );
				};

				draw_slider_hue( );
				if ( is_hue_slider_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
					int delta = std::clamp( mouse_y - y, 0, 100 );
					m_hue = float( delta ) * 0.01f;
				}

				draw_slider_alpha( );
				if ( is_alpha_slider_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
					int delta = std::clamp( mouse_x - x, 0, 121 );
					m_alpha = ( int )( float( delta ) * 255.f / 121.f );
				}

				output( );

				g_d3d.get_device( )->SetScissorRect( &old_rect );
			}
			else {
				if ( is_hovered( ) ) {
					ui_draw_rect( x - 1, y - 1, m_width + 2, m_height + 2, ui_get_text_col( ) );
				}
				ui_draw_rect( x, y, m_width, m_height, *m_setting );
			}
		}

	protected:
		clr_t* m_setting	= nullptr;
		bool m_active		= false;
		bool m_mouse_held	= false;
		float m_saturation	= 1.0f;
		float m_brightness	= 1.0f;
		float m_hue			= 0.f;
		uint8_t m_alpha		= 255;
		bool m_has_text		= false;
	};
}