#include "console.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "d3d.hpp"
#include "ui_draw.h"

std::shared_ptr< console > g_con = std::make_shared< console >( );


void console::draw_text( int x, int y, bool greyed, const char* fmt, ... ) {
	va_list args;
	char buf[ 512 ];

	__crt_va_start( args, fmt );
	vsprintf_s< 512 >( buf, fmt, args );
	__crt_va_end( args );

	g_d3d.draw_text< ALIGN_LEFT >( d3d::fonts.f_con, greyed ? clr_t( 160, 160, 160 ) : clr_t( 255, 255, 255 ), x, y, D3DFONTFLAG_DROPSHADOW, "%s", buf );
	//g_d3d.draw_text< ALIGN_LEFT >( d3d::fonts.f_con, greyed ? clr_t( 160, 160, 160 ) : clr_t( 255, 255, 255 ), x, y, D3DFONTFLAG_DROPSHADOW, buf );
}

void console::draw_text( int x, int y, const char* fmt, ... ) {
	va_list args;
	char buf[ 512 ];

	__crt_va_start( args, fmt );
	vsprintf_s< 512 >( buf, fmt, args );
	__crt_va_end( args );

	g_d3d.draw_text< ALIGN_LEFT >( d3d::fonts.f_con, clr_t( 255, 255, 255 ), x, y, D3DFONTFLAG_DROPSHADOW, buf );
}

void console::draw_rect( int x, int y, int w, int h, clr_t col ) {
	g_d3d.draw_filled_rect( col, x, y, w, h );
}

void console::register_alias( std::shared_ptr< ISetting >& alias ) {
	m_aliases.push_back( alias );
}

void console::capture_command( ) {
	float current_time = GetTickCount( ) * 0.001f;
	size_t length = strlen( m_cur_cmd );

	for( size_t i{ }; i < 0xfe; ++i ) {
		if( m_input->is_key_pressed( i ) ) {
			float delta_time = current_time - m_last_key_input[ i ];
			if( fabs( delta_time ) > 0.2f ) {
				if( i == KEYS_BACK ) {
					m_cur_cmd[ length - 1 ] = 0;
					m_last_key_input[ i ] = current_time;
					continue;
				}

				m_key_states[ i ] = 0xf0;
				wchar_t pressed_char;
				const auto scan = MapVirtualKeyA( i, 2 );
				auto ret = ToAscii( i, scan, ( BYTE* )m_key_states, ( LPWORD )&pressed_char, 1 );

				if( ret == 1 ) {
					if( length < 200 ) {
						m_cur_cmd[ length ] = ( char )( pressed_char );
						m_cur_cmd[ length + 1 ] = 0;
					}
				}
				m_last_key_input[ i ] = current_time;
			}
		}
		else {
			m_last_key_input[ i ] = 0.f;
			m_key_states[ i ] = 0;
		}
	}
}

ISetting* console::find_var( hash_t var ) {
	for( auto& it : data::holder_.get_nodes( ) ) {
		auto setting = static_cast< ISetting* >( it );
		if( var == setting->get_hash( ) ) {
			return setting;
		}
	}

	return nullptr;
}

ISetting* console::find_alias( hash_t alias ) {
	for( auto& it : m_aliases ) {
		if( it->get_hash( ) == alias )
			return it.get( );
	}

	return nullptr;
}

con_fn_base* console::find_fn( hash_t name ) {
	for( auto& it : m_functions ) {
		if( it->get_hash( ) == name ) {
			return it;
		}
	}

	return nullptr;
}

std::string console::impl_alias_str( std::string str ) {
	for( size_t i{ }; i < str.length( ); ++i ) {
		if( str[ i ] == ' ' && str[ i + 1 ] != ' ' ) {
			size_t end = 0;
			for( size_t i2{ i + 1 }; i2 < str.length( ); ++i ) {
				if( str[ i2 ] == ' ' )
					end = i2;
			}

			if( end ) {
				std::string alias_str( str.c_str( ) + i, end - i );
				auto alias = find_alias( hash::fnv1a( alias_str ) );
				if( alias ) {
					str.replace( alias_str.begin( ), alias_str.end( ), alias->get_string( ) );
				}
				else {
					g_con->log( xors( "couldnt find alias %s" ), alias_str.c_str( ) );
				}
			}
		}
	}

	return str;
}

void console::execute_command( const char* cmd ) {
	if( !cmd[ 0 ] ) return;
	std::string first_param( cmd, strlen( cmd ) );

	size_t end{ };
	bool one_arg = true;
	for( size_t i{ }; i < strlen( cmd ); ++i ) {
		if( cmd[ i ] == ' ' ) {
			first_param.resize( i );
			end = i;
			one_arg = false;
			break;
		}
	}

	if( one_arg ) {
		first_param.resize( first_param.length( ) - 1 );
	}

	//find a command to execute
	auto fn = find_fn( hash::fnv1a( first_param ) );
	if( fn ) {
		//std::string alias_cmd = impl_alias_str( cmd );

		if( !fn->execute( cmd ) ) {
			log( xors( "invalid syntax" ) );
		}
		return;
	}

	//command not found, we're managing vars
	if( !one_arg ) {
		std::string second_param = ( cmd + end + 1 );
		bool is_floating_point = second_param.find( '.' ) != std::string::npos;
		auto var = find_var( hash::fnv1a( first_param ) );

		if( var ) {
			auto setting_cast = static_cast< con_var< void* >* >( var );
			if( !setting_cast->is_integral( ) && !setting_cast->is_floating_point( ) ) {
				auto value = std::strtol( second_param.c_str( ), 0, 16 );
				printf( xors( "value: %08x\n" ), value );
				var->set( value );
			}

			if( is_floating_point ) {
				float value = std::strtof( second_param.c_str( ), 0 );
				var->set( value );
			}
			else {
				int value = std::strtol( second_param.c_str( ), 0, 10 );
				var->set( value );
			}
			log( xors( "%s %s\n" ), first_param.c_str( ), second_param.c_str( ) );
			return;
		}
	}


	log( xors( "unknown command: %s" ), first_param.c_str( ) );
}

void console::input( ) {
	static int drag_x, drag_y;
	int cursor_x, cursor_y;
	int x = m_x, y = m_y;
	bool clicked = m_input->is_key_pressed( KEYS_MOUSE1 );
	static bool is_hovered{ };
	m_input->get_cursor_pos( cursor_x, cursor_y );

	auto is_top_hovered = [ &x, &y, 
		&cursor_x, &cursor_y ]( ) {
		return cursor_x > x && cursor_x < x + WIDTH
			&& cursor_y > y && cursor_y < y + TOP_HEIGHT;
	};

	auto is_window_hovered = [ &x, &y,
		&cursor_x, &cursor_y ]( ) {
		return cursor_x > x && cursor_x < x + WIDTH
			&& cursor_y > y && cursor_y < y + HEIGHT;
	};

	auto is_bottom_hovered = [ &x, &y,
		&cursor_x, &cursor_y ]( ) {
		return cursor_x > x && cursor_x < x + WIDTH
			&& cursor_y > y + HEIGHT - TOP_HEIGHT 
			&& cursor_y < y + HEIGHT;
	};

	if( clicked ) {
		//m_consuming_input = is_window_hovered( );
	}

	if( !clicked && is_top_hovered( ) ) {
		drag_x = cursor_x - m_x;
		drag_y = cursor_y - m_y;
	}

	if( clicked && is_hovered ) {
		is_hovered = true;
		m_x = cursor_x - drag_x;
		m_y = cursor_y - drag_y;
	}
	else {
		is_hovered = is_top_hovered( );
	}

	if( clicked ) {
		m_active = is_bottom_hovered( );
	}

	if( m_active ) {
		capture_command( );

		static bool was_held{ };
		if( m_input->is_key_pressed( KEYS_RETURN ) ) {
			if( !was_held ) {
				execute_command( m_cur_cmd );
				m_cur_cmd[ 0 ] = 0;
			}
			was_held = true;
		}
		else was_held = false;
	}
}

void console::draw( ) {
	if( !m_open ) {
		m_consuming_input = false;
		m_active = false;
		return;
	}

	static clr_t col = clr_t( 31, 31, 31, 255 );

	input( );

	RECT prev_rect{ };
	RECT new_rect{ m_x - 1, m_y - 1,
		m_x + WIDTH + 1, m_y + HEIGHT + 1 };
	g_d3d.get_device( )->GetScissorRect( &prev_rect );
	
	g_d3d.get_device( )->SetScissorRect( &new_rect );

	draw_rect( m_x - 1, m_y - 1, WIDTH + 2, HEIGHT + 2, ui::ui_get_accent_col( ) );
	draw_rect( m_x, m_y, WIDTH, HEIGHT, col );
	draw_rect( m_x + 1, m_y, WIDTH - 2, TOP_HEIGHT, clr_t( 41, 41, 41 ) );

	draw_rect( m_x + 1, m_y + HEIGHT - TOP_HEIGHT, WIDTH - 2, TOP_HEIGHT, clr_t( 41, 41, 41 ) );
	if( m_active ) {
		draw_rect( m_x + 2, m_y + HEIGHT - TOP_HEIGHT + 1, WIDTH - 4, TOP_HEIGHT - 2, clr_t( 31, 31, 31 ) );
		auto fn = find_fn( hash::fnv1a( m_cur_cmd ) );
		if( fn ) {
			std::string fn_str = m_cur_cmd;
			fn_str += ' ';
			fn_str += fn->get_syntax( );

			draw_text( m_x + 2, m_y + HEIGHT - TOP_HEIGHT / 2 - 3, true, "%s", fn_str.c_str( ) );
		}

		else {
			auto var = find_var( hash::fnv1a( m_cur_cmd ) );
			if( var ) {
				std::string var_str = m_cur_cmd;
				var_str += ' ';

				var_str += var->get_string( );

				draw_text( m_x + 2, m_y + HEIGHT - TOP_HEIGHT / 2 - 3, true, var_str.c_str( ) );
			}
		}
	}

	std::string cur_cmd_str = m_cur_cmd;
	if( m_active ) {
		cur_cmd_str += '_';
	}
	draw_text( m_x + 2, m_y + HEIGHT - TOP_HEIGHT / 2 - 3, cur_cmd_str.c_str( ) );
	draw_text( m_x + 3, m_y + TOP_HEIGHT / 2 - 3, xors( "console" ) );

	int cur_y = m_y + HEIGHT - TOP_HEIGHT;
	for( int i = m_logs.size( ) - 1; i >= 0; --i ) {
		constexpr int LINE_HEIGHT = 14;
		auto& log = m_logs.at( i );

		if( cur_y - LINE_HEIGHT < m_y + TOP_HEIGHT ) {
			m_logs.erase( m_logs.begin( ) + i );
			continue;
		}
		
		draw_text( m_x + 2, cur_y -= LINE_HEIGHT, log.m_msg );
	}

	g_d3d.get_device( )->SetScissorRect( &prev_rect );
}