#pragma once

#include "ui_base_item.h"

namespace ui
{
	template < typename t >
	class c_slider : public base_item {
	public:
		c_slider( int x, int y, int w, t min, t max, const char* text, t* setting, t full, const char* suffix = 0 ) :
			base_item( x, y, w, 5, text ), m_setting( setting ), m_suffix( suffix ),
			m_min( float( min ) ), m_max( float( max ) ), m_full( full ), m_has_text( true ) { };

		c_slider( int x, int y, int w, t min, t max, const char* text, t* setting, const char* suffix = 0 ) :
			base_item( x, y, w, 5, text ), m_setting( setting ), m_suffix( suffix ),
			m_min( float( min ) ), m_max( float( max ) ), m_full( max ), m_has_text( true ) { };

		c_slider( int x, int y, int w, t min, t max, t* setting, const char* suffix = 0 ) :
			base_item( x, y, w, 5, nullptr ), m_setting( setting ), m_suffix( suffix ),
			m_min( float( min ) ), m_max( float( max ) ), m_full( max ), m_has_text( false ) { }

		virtual bool is_hovered( ) override {
			if ( m_disabled ) return false;

			int x = get_relative_x( );
			int y = get_relative_y( ) + ( m_has_text ? 11 : 2 );

			int mouse_x, mouse_y;
			ui_get_cursor_pos( mouse_x, mouse_y );

			if( !m_has_text ) {
				x += 4;
				return mouse_x >= x && mouse_x <= x + m_width - 8
					&& mouse_y >= y && mouse_y <= y + m_height + 2;
			}

			return mouse_x >= x - 1 && mouse_x <= x + m_width + 1
				&& mouse_y >= y && mouse_y <= y + m_height + 2;
		}

		void input( ) {
			bool mouse_clicked  = g_input.is_key_pressed( KEYS_MOUSE1 );
			bool hovered		= is_hovered( );
			float progress		= 0.f;
			int x				= get_relative_x( );
			int y				= get_relative_y( ) + 2;
			int mouse_x, mouse_y;

			*m_setting = ( t )( std::clamp< float >( *m_setting, m_min, m_max ) );
			ui_get_cursor_pos( mouse_x, mouse_y );

			if ( hovered && mouse_clicked ) {

				float progress{ };

				if( m_has_text ) {
					progress = std::clamp< float >( float( mouse_x - x ) / ( m_width - 3 ), 0.f, 1.0f );
				}
				else {
					progress = std::clamp< float >( float( mouse_x - x - 4 ) / ( m_width - 10 ), 0.f, 1.0f );
				}

				*m_setting = progress == 1.0f ? m_full : ( t )( ( ( m_max - m_min ) * progress ) + m_min );
			}

			if( !m_has_text ) {
				bool y_hover = mouse_y >= y && mouse_y <= y + m_height + 1;

				bool minus_hovered = mouse_x >= x - 1 && mouse_x <= x + 3 && y_hover;
				bool plus_hovered = mouse_x >= x + m_width - 2 && mouse_x <= x + m_width + 2 && y_hover;

				if( mouse_clicked ) {
					if( !m_mouse_held ) {
						if( !std::is_floating_point< t >::value ) {
							if( minus_hovered ) *m_setting -= ( t )1;
							if( plus_hovered )  *m_setting += ( t )1;
						}
						else if( m_max - m_min <= 2.0f ) {
							if( minus_hovered ) *m_setting -= ( t )0.1f;
							if( plus_hovered )  *m_setting += ( t )0.1f;
						}
					}

					m_mouse_held = true;
				}
				else {
					m_mouse_held = false;
				}
			}
		};

		void draw_slider( int x, int y ) {
			float val		= float( *m_setting );
			float progress	= ( val - m_min ) / ( m_max - m_min );

			ui_draw_rect( x, y, m_width, m_height, ui_get_disabled_col( ) );

			ui_draw_rect( x, y, ( int )( ( m_width - 3 ) * progress ), m_height, ui_get_accent_col( ) );
			ui_draw_rect( x + ( int )( ( m_width - 3 ) * progress ), y, 3, m_height, 
				is_hovered( ) ? ui_get_text_col( ) : ui_get_accent_col( ) * 0.7f );
		}

		void draw_slider_small( int x, int y ) {
			float val		= float( *m_setting );
			float progress  = ( val - m_min ) / ( m_max - m_min );
			x -= 1; //i couldnt be fucked

			ui_draw_rect( x + 5, y, m_width - 8, m_height, ui_get_disabled_col( ) );

			ui_draw_rect( x + 5, y, ( int )( ( m_width - 10 ) * progress ), m_height, ui_get_accent_col( ) );
			ui_draw_rect( x + ( int )( ( m_width - 10 ) * progress ) + 3, y, 3, m_height,
				is_hovered( ) ? ui_get_text_col( ) : ui_get_accent_col( ) * 0.7f );

			ui_draw_string( x, y - 3, false, ui_get_text_col( ), "-" );
			ui_draw_string( x + m_width - 2, y - 3, false, ui_get_text_col( ), "+" );

			char val_str[ 12 ];
			if( m_suffix )
				sprintf_s( val_str, 12, t( 0.1f ) == t( 0 ) ? "%d %s" : "%0.2f %s", *m_setting, m_suffix );
			else
				sprintf_s( val_str, 12, t( 0.1f ) == t( 0 ) ? "%d" : "%0.2f", *m_setting );

			int text_h, text_w;
			ui_get_text_size( text_w, text_h, val_str );
			ui_draw_string( x + ( int )( ( m_width - 8 ) * progress ), y + 2, false, ui_get_text_col( ), val_str );
		}

		virtual int get_total_height( ) const override {
			return m_height + ( m_has_text ? 12 : 6 );
		}

		virtual void render( ) override {
			int x = get_relative_x( );
			int y = get_relative_y( );
			int text_w, text_h;
			char val[ 12 ];

			input( );

			//weird hacky fix for floating point vars
			if ( m_suffix )
				sprintf_s( val, 12, t( 0.1f ) == t( 0 ) ? "%d %s" : "%0.2f %s", *m_setting, m_suffix );
			else 
				sprintf_s( val, 12, t( 0.1f ) == t( 0 ) ? "%d" : "%0.2f", *m_setting );

			if( m_has_text ) {
				ui_draw_string( x + 2, y, false, ui_get_text_col( ), m_text );
				ui_get_text_size( text_w, text_h, val );
				ui_draw_string( x + m_width - text_w - 1, y, false, ui_get_text_col( ), val );
				draw_slider( x, y + 12 );
			}
			else {
				draw_slider_small( x, y + 2 );
			}
		}


	protected:
		t*			m_setting;
		t			m_full;
		float		m_min;
		float		m_max;
		bool		m_has_text = true;
		const char* m_suffix;
		bool		m_mouse_held = false;
	};
}