#pragma once
#include "ui_base_item.h"



namespace ui
{
	namespace {
		constexpr int BUTTON_WIDTH = 120;
		constexpr int BUTTON_HEIGHT = 80;

		constexpr int SUBTAB_HEIGHT = 25;
	}

	class c_tab_sheet : public base_item {
	public:
		c_tab_sheet( const char* tab_name ) :
			base_item( 0, 0, 0, 0, tab_name ), 
			m_fade_progress( 0 ) { };

		c_tab_sheet( const char* tab_name, d3d::c_sprite* sprite ) :
			base_item( 0, 0, 0, 0, tab_name ), m_sprite( sprite ),
			m_fade_progress( 0.f ) { };

		virtual void render( ) override { };
		virtual bool is_hovered( ) override {
			return false;
		};

	public:
		float m_fade_progress{ };
		d3d::c_sprite* m_sprite{ };
	};


	class c_tab_manager : public base_item {
	public:
		c_tab_manager( ) : base_item( BUTTON_WIDTH + 11, 0, 0, 0, 
			xors( "TAB_MANAGER" ) ) { };

		virtual bool is_hovered( ) override {
			return false;
		}

		inline void scale_button_fade( c_tab_sheet* item, bool hovered ) {
			if ( hovered ) {
				constexpr float frequency = 1.f / 0.3f;
				const float step = ui_get_frametime( ) * frequency;

				item->m_fade_progress = std::clamp( item->m_fade_progress + step, 0.f, 0.8f );
			}
			else {
				item->m_fade_progress = 0.f;
			}
		}

		auto get_selected_tab( ) {
			return m_selected_tab;
		}

		void draw_tab_button( decltype( m_parent )& button, int start, bool hovered ) {
			auto item		= ( c_tab_sheet* )button.get( );
			auto text		= item->get_text( );
			auto parent_x	= m_parent->x( ) + 5;
			auto parent_y	= m_parent->y( ) + 5;
			int item_height = BUTTON_HEIGHT;

			scale_button_fade( item, hovered );

			ui_draw_rect( parent_x, parent_y + start, BUTTON_WIDTH + 1, 
				item_height + 1, ui_get_bg_col( ) );

			ui_draw_line( parent_x + BUTTON_WIDTH - 8, parent_y + start - 1,
				parent_x + BUTTON_WIDTH - 8, parent_y + start + BUTTON_HEIGHT + 2,
				ui_get_disabled_col( ) );

			if( hovered ) {
				ui_draw_line( parent_x + BUTTON_WIDTH - 8, parent_y + start - 1,
					parent_x + BUTTON_WIDTH - 8, parent_y + start + BUTTON_HEIGHT + 2,
					ui_get_accent_col( ) * ( item->m_fade_progress + 0.2f ) * 0.8f );
			}

			if( item->m_sprite ) {
				auto sprite_color = hovered ? ui_get_text_col( ) * ( ( item->m_fade_progress + 0.3f ) * 0.7f ) : ui_get_disabled_col( );
				item->m_sprite->draw( parent_x + ( BUTTON_WIDTH - 8 ) / 2 - 2,
					parent_y + BUTTON_HEIGHT / 2 + start, sprite_color );
			}
			else {
				g_con->log( "sprite is null" );
			}
		}

		inline bool is_button_hovered( int start ) {
			auto item_x		= m_parent->x( ) + 5;
			auto item_y		= m_parent->y( ) + 5 + start;
			int	item_height = BUTTON_HEIGHT;
			int mouse_x, mouse_y;

			ui_get_cursor_pos( mouse_x, mouse_y );

			return mouse_x >= item_x && mouse_x <= item_x + BUTTON_WIDTH - 8
				&& mouse_y >= item_y && mouse_y <= item_y + item_height;
		}

		virtual void render( ) override {
			if ( !m_items.empty( ) ) {
				int cur_y{ 10 };
				for ( auto& it : m_items ) {
					it->set_visible( false );
					bool hovered = is_button_hovered( cur_y );
					if ( g_input.is_key_pressed( KEYS_MOUSE1 ) && hovered ) {
						//fix items that disable input mess it up when changing tabs
						set_disabled_callbacks( false );
						m_selected_tab = it;
					}

					draw_tab_button( it, cur_y, hovered || it == m_selected_tab );
					cur_y += BUTTON_HEIGHT + 4;
				}
			}
			
			if ( !m_selected_tab ) {
				m_selected_tab = m_items.front( );
			}

			m_selected_tab->set_visible( true );
		}

	protected:
		decltype( m_parent ) m_selected_tab{ };
	};


	class c_subtab_manager : public base_item {
	public:
		c_subtab_manager( ) : base_item( 0, 35, 0, 5, 
			xors( "SUBTAB_MANAGER" ) ) { }

		virtual bool is_hovered( ) override { return false; }

		int get_button_width( ) {
			int width = get_top_parent( )->w( ) - BUTTON_WIDTH - 26;

			if ( !m_items.empty( ) )
				return ( int )std::ceilf( float( width ) / float( m_items.size( ) ) );

			return width;
		}

		inline void scale_button_fade( c_tab_sheet* item, bool hovered ) {
			if ( hovered ) {
				constexpr float frequency = 1.f / 0.3f;
				const float step = ui_get_frametime( ) * frequency;

				item->m_fade_progress = std::clamp( item->m_fade_progress + step, 0.f, 0.8f );
			}
			else {
				item->m_fade_progress = 0.f;
			}
		}

		auto get_selected_tab( ) {
			return m_selected_tab;
		}

		void render_button( decltype( m_parent )& button, int start, bool hovered ) {
			auto item	= ( c_tab_sheet* )button.get( );
			auto item_x = get_relative_x( ) + start - 2;
			auto item_y = get_relative_y( ) + 3 - m_y;
			auto width	= get_button_width( );

			scale_button_fade( item, hovered );
			clr_t tab_clr = ui_get_disabled_col( );

			//ui_draw_rect( item_x, item_y, get_button_width( ), SUBTAB_HEIGHT, ui_get_bg_col( ) );
			ui_draw_rect( item_x + 1, item_y + SUBTAB_HEIGHT - 2, width, 2, tab_clr );
			if ( hovered ) {
				clr_t col = ui_get_accent_col( );
				col.a( ) *= item->m_fade_progress;
				ui_draw_rect( item_x + 1, item_y + SUBTAB_HEIGHT - 2, width, 2, col );
			}

			ui_draw_string( item_x + width / 2, item_y + 4, true, ui_get_text_col( ), item->get_text( ) );
		}

		bool is_button_hovered( decltype( m_parent )& button, int start ) {
			int item_x = get_relative_x( ) + start - 2;
			int item_y = get_relative_y( ) + 3 - m_y - 3;
			int item_w = get_button_width( );
			int mouse_x, mouse_y;

			ui_get_cursor_pos( mouse_x, mouse_y );

			return mouse_x >= item_x && mouse_x <= item_x + item_w &&
				mouse_y >= item_y && mouse_y <= item_y + SUBTAB_HEIGHT;
		}

		virtual void render( ) override {
			if ( !m_items.empty( ) ) {
				int start = 2;
				int width = get_button_width( );

				for ( auto& it : m_items ) {
					it->set_visible( false );
					bool hovered = is_button_hovered( it, start );
					if ( g_input.is_key_pressed( KEYS_MOUSE1 ) && hovered ) {
						//fix items that disable input mess it up when changing tabs
						set_disabled_callbacks( false );
						m_selected_tab = it;
					}

					render_button( it, start, hovered || it == m_selected_tab );
					start += width;
				}

				if ( !m_selected_tab ) {
					m_selected_tab = m_items.front( );
				}

				m_selected_tab->set_visible( true );
			}
		}

	protected:
		decltype( m_parent ) m_selected_tab;
	};
}