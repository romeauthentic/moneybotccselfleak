#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "ui_draw.h"

namespace ui
{
	//the offset between each item
	constexpr int ITEM_OFFSET = 5;

	class base_item : public std::enable_shared_from_this< base_item > {
	public:
		base_item( ) { }
		base_item( int x, int y, int w, int h, const char* name = nullptr ) :
			m_x( x ), m_y( y ), m_width( w ), m_height( h ) {
			if( name ) {
				strcpy( m_text, name );
			}
		}

		virtual void render( ) { };
		virtual bool is_hovered( ) { return false; }
		virtual bool is_form( ) const { return false; }

		virtual void reset( ) {
			m_y_offset = 0;
		}

		virtual int get_total_height( ) const {
			return m_height;
		}

		void set_y_offset( int offset ) {
			m_y_offset = offset;
		}

		int get_y_offset( ) const {
			return m_y_offset;
		}

		auto add_item( std::shared_ptr< base_item > item ) {
			item.get( )->m_parent = shared_from_this( );
			m_items.emplace( m_items.begin( ), item );

			return item;
		}

		auto& get_items( ) { return m_items; }
		auto  get_parent( ) { return m_parent; }

		virtual int x( ) const { return m_x; }
		virtual int y( ) const { return m_y; }
		virtual int w( ) const { return m_width; }
		virtual int h( ) const { return m_height; }

		void set_x( int x ) { m_x = x; }
		void set_y( int y ) { m_y = y; }

		void set_width( int w ) { m_width = w; }
		void set_height( int h ) { m_height = h; }

		bool get_visible( ) const {
			if( m_cond && !m_cond( ) ) {
				return false;
			}

			return m_visible;
		}

		void set_cond( std::function< bool( ) > func ) {
			m_cond = func;
		}

		void set_visible( bool vis ) { m_visible = vis; }
		void set_text( const char* text ) {
			strcpy( m_text, text );
		}
		auto get_text( ) const {
			return m_text;
		}

		std::shared_ptr< base_item > find_item( const char* name ) {
			if( !m_items.empty( ) ) {
				for( auto& it : m_items ) {
					if( it->get_text( ) && !strcmp( it->get_text( ), name ) ) {
						return it;
					}

					auto it_find = it->find_item( name );
					if( it_find != it ) return it_find;
				}
			}

			return shared_from_this( );
		}

		auto get_top_parent( ) {
			for( auto parent = m_parent; ;
				parent = parent->get_parent( ) ) {
				if( !parent->get_parent( ) ) {
					return parent;
				}
			}

			return shared_from_this( );
		}

		int get_relative_x( ) {
			int x = m_x;
			for( auto parent = get_parent( ); !!parent;
				parent = parent->get_parent( ) ) {
				x += parent->x( );
			}

			return x;
		}

		int get_relative_y( ) {
			int y = m_y + get_y_offset( );
			for( auto parent = get_parent( ); !!parent;
				parent = parent->get_parent( ) ) {
				y += parent->y( ) + parent->get_y_offset( );
			}

			return y;
		}

		void set_disabled( bool disabled ) {
			m_disabled = disabled;

			for( auto& it : m_items ) {
				it->set_disabled( disabled );
			}
		}

		void set_disabled_callbacks( bool disabled ) {
			auto top = get_top_parent( );

			top->set_disabled( disabled );

			m_disabled = false;
		}

	protected:
		int m_x{ };
		int m_y{ };

		int m_width{ };
		int m_height{ };

		//current y position for rendering
		int m_y_offset{ };

		bool m_visible = true;
		bool m_disabled = false;
		char m_text[ 256 ]{ };

		std::shared_ptr< base_item > m_parent;
		std::vector< std::shared_ptr< base_item > > m_items;
		std::function< bool( ) > m_cond;
	};
}