#include "playerlist.hpp"

/* something in here caused the file size to increase alot lol */

namespace features
{

	bool c_player_list::is_friend( const std::string& guid ) {
		return m_friends[ guid ]; // if a guid is not there, then it is automatically set to false
	}

	// handles players and drawing.
	void c_player_list::operator()( ) {
		if( g_gmod.m_engine( )->IsInGame( ) ) {
			g_gmod.m_engine( )->GetScreenSize( m_scrw, m_scrh );

			m_x = 50;
			m_y = 50;
			m_w = m_scrw - 100;
			m_h = m_scrh - 100;

			m_tabwidth = m_w / 3;

			m_tab = 0;
			m_items.clear( );

			player_info_t info;

			for( int i{ 1 }; i <= g_gmod.m_globals->m_maxclients; ++i ) {
				if( i == g_gmod.m_engine( )->GetLocalPlayer( ) )
					continue;

				auto player = g_gmod.m_entlist( )->GetClientEntity< >( i );
				if( !player )
					continue;

				if( !g_gmod.m_engine( )->GetPlayerInfo( i, &info ) )
					continue;

				m_items.push_back( playeritem_t( info.name, &m_friends[ info.m_steamid ] ) );
			}

			// sort first character alphabetically.
			std::sort( m_items.begin( ), m_items.end( ), []( const auto& lhs, const auto& rhs ) {
				return lhs.m_name[ 0 ] < rhs.m_name[ 0 ];
			} );


			if( m_open ) {
				int i = 0;
				int	j = 0;

				// get d3d font info.
				TEXTMETRICA metric{ };
				d3d::fonts.f_esp_small->GetTextMetricsA( &metric );

				// construct size of list and how much to draw.
				const int font_size = metric.tmHeight;
				const int list_height = m_items.size( ) * font_size;
				const int tabs_to_draw = std::min< int >( ( int )std::ceil( ( float )list_height / ( float )m_h ), 3 ); // template arg to stop the min define taking action

				// pixel padding.
				const int m_pad_y = 2;
				const int m_pad_x = 5;

				// draw blue background.
				g_d3d.draw_filled_rect( ui::ui_get_bg_col( ), m_x, m_y, m_tabwidth * tabs_to_draw, m_h );

				// for each player add to list.
				for( const auto& item : m_items ) {
					int str_pos_x{ };
					int str_pos_y = m_y + font_size * i + m_pad_y;

					if( str_pos_y > m_y + m_h ) {
						i = 0;
						m_tab++;

						str_pos_y = m_y + font_size * i + m_pad_y;
					}

					str_pos_x = m_x + m_tabwidth * m_tab + m_pad_x;

					m_tab = std::min< int >( m_tab, tabs_to_draw );

					// draw rectangle per element.
					g_d3d.draw_rect( clr_t( 0, 0, 0 ), m_x + m_tabwidth * m_tab + 1, m_y + font_size * i, m_tabwidth - 2, font_size );

					// if selected index is this current index, do special stuff.
					if( m_index == j ) {
						g_d3d.draw_filled_rect( clr_t( 200, 200, 200 ), m_x + m_tabwidth * m_tab + 1, m_y + font_size * i + 1, m_tabwidth - 1, font_size - 1 );

						g_d3d.draw_text< ALIGN_LEFT >( d3d::fonts.f_esp_small, clr_t( 255, 255, 255 ), str_pos_x + 10, str_pos_y - 2 , D3DFONTFLAG_DROPSHADOW, item.m_name );
						g_d3d.draw_text< ALIGN_RIGHT >( d3d::fonts.f_esp_small, clr_t( 255, 255, 255 ), m_x + m_tabwidth * ( m_tab + 1 ) - 5, str_pos_y - 2, D3DFONTFLAG_DROPSHADOW, *item.m_friend ? "true" : "false" );

					}
					else {
						g_d3d.draw_text< ALIGN_LEFT >( d3d::fonts.f_esp_small, clr_t( 255, 255, 255 ), str_pos_x, str_pos_y - 2, D3DFONTFLAG_DROPSHADOW, item.m_name );
						g_d3d.draw_text< ALIGN_RIGHT >( d3d::fonts.f_esp_small, clr_t( 255, 255, 255 ), m_x + m_tabwidth * ( m_tab + 1 ) - 5, str_pos_y - 2, D3DFONTFLAG_DROPSHADOW, *item.m_friend ? "true" : "false" );
					}

					i++;
					j++;
				}

				
				// draw pink outlines per tab.
				for( uint8_t k = 0; k < ( uint8_t )tabs_to_draw; k++ ) {
					g_d3d.draw_rect( ui::ui_get_accent_col( ), m_x + m_tabwidth * k, m_y, m_tabwidth, m_h );
				}

			}
		}
	}
}