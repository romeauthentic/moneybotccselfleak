#pragma once
#include "ui_base_item.h"

namespace ui
{
	class c_key_picker : public base_item {
	public:
		c_key_picker( int x, int y, int w, const char* name, int* setting ) :
			base_item( x, y, w, 16, name ), m_setting( setting ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( ) + 12;
			int h = m_height;

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );


			return mouse_x >= x && mouse_x <= x + m_width
				&& mouse_y >= y && mouse_y <= y + h;
		}

		virtual int get_total_height( ) const override {
			return m_height + 12;
		}

		void input( ) {
			bool active_backup	= m_active;
			bool active_changed = false;

			if ( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				if ( !m_mouse_held ) {
					m_active = true;
				}
				m_mouse_held = true;
			}

			if ( g_input.is_key_pressed( KEYS_ESCAPE ) ) {
				m_active = false;
				*m_setting = KEYS_NONE;
			}

			if ( m_active && !m_mouse_held ) {
				int key = g_input.is_any_key_pressed( );
				if ( key != KEYS_NONE ) {
					*m_setting = key;
					m_active = false;
				}
			}

			active_changed = active_backup != m_active;
			if ( active_changed ) {
				set_disabled_callbacks( m_active );
			}
		}

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );

			input( );

			ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );
			ui_draw_rect( x, y + 13, m_width, m_height, ui_get_disabled_col( ) );
			ui_draw_outlined_rect( x - 1, y + 12, m_width + 1, m_height + 1,
				is_hovered( ) || m_active ? ui_get_text_col( ) : ui_get_accent_col( ) );

			ui_draw_string( x + m_width / 2, y + 14, true, ui_get_text_col( ), 
				g_input.get_key_name( ( VirtualKeys_t )*m_setting ) );
		}

	protected:
		int* m_setting{ };
		bool m_active{ };
		bool m_mouse_held{ };
	};

	//skEeT PiCkErS
	class c_key_picker_small : public base_item {
	public:
		c_key_picker_small( int x, int y, int* setting ) :
			base_item( x, y, 0, 0, xors( "KEY_PICKER" ) ), m_setting( setting ) { }

		virtual int get_total_height( ) const override {
			const char* name = g_input.get_short_name( ( VirtualKeys_t )*m_setting );
			int w, h;
			ui_get_text_size( w, h, name );

			return h;
		}

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			const char* name = g_input.get_short_name( ( VirtualKeys_t )*m_setting );
			int x = get_relative_x( );
			int y = get_relative_y( );
			int mouse_x, mouse_y;
			int w, h;
			
			ui_get_text_size( w, h, "[%s]", name );
			ui_get_cursor_pos( mouse_x, mouse_y );

			return mouse_x >= x - w && mouse_x <= x
				&& mouse_y >= y && mouse_y <= y + h;
		}

		void input( ) {

			bool active_backup	= m_active;
			bool active_changed = false;

			if ( g_input.is_key_pressed( KEYS_ESCAPE ) ) {
				m_active = false;
				*m_setting = KEYS_NONE;
			}

			if ( m_active && !m_mouse_held ) {
				int key = g_input.is_any_key_pressed( );
				if ( key != KEYS_NONE ) {
					*m_setting = key;
					m_active = false;
				}
			}

			if ( is_hovered( ) && g_input.is_key_pressed( KEYS_MOUSE1 ) ) {
				if ( !m_mouse_held ) {
					m_active = true;
				}
				m_mouse_held = true;
			}
			else {
				m_mouse_held = false;
			}

			active_changed = active_backup != m_active;
			if ( active_changed ) {
				set_disabled_callbacks( m_active );
			}
		}

		virtual void render( ) override {
			const char* name = g_input.get_short_name( ( VirtualKeys_t )*m_setting );
			int x = get_relative_x( );
			int y = get_relative_y( );

			int w, h;
			ui_get_text_size( w, h, "[%s]", name );

			input( );

			ui_draw_string( x - w, y, false, is_hovered( ) || m_active ?
				ui_get_accent_col( ) : ui_get_text_col( ), "[%s]", name );
		}

	protected:
		int* m_setting;
		bool m_active{ };
		bool m_mouse_held{ };
	};
}