#pragma once

#include "context.hpp"
#include "d3d.hpp"
#include "interface.hpp"
#include "ui_draw.h"

#include <map>
#include <algorithm>

NAMESPACE_REGION( features )

class c_player_list
{
	struct playeritem_t
	{
		playeritem_t( ) = default;
		playeritem_t( char* name, bool* friends ) {
			strcpy( m_name, name );
			m_friend = friends;
		}

		char m_name[ 32 ]{ };
		bool* m_friend{ };
	};

	bool m_open;
	int m_x, m_y, m_w, m_h;
	int m_tabwidth;
	int m_tab;
	int m_scrw, m_scrh;
	int m_index;

	std::vector< playeritem_t > m_items;
	std::map< std::string, bool > m_friends;
public:
	inline bool& is_open( ) {
		return m_open; 
	}

	inline int& get_index( ) { 
		return m_index;
	}

	inline auto get_list( ) {
		return m_items;
	}

	bool is_friend( const std::string& guid );

	void operator()( );
};

END_REGION