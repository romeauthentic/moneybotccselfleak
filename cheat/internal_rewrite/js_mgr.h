#pragma once
#include "js/duktape.h"

#include "util.hpp"
#include "math.hpp"

#include "ui_dropdown_item.h"
#include "../internal_rewrite/console.hpp"

namespace js {
  class c_js {
  public:
    ~c_js( );

    auto ctx( ) { return m_js_ctx; }

    clr_t m_clr = clr_t( 255, 255, 255, 255 );

    //these should be inlined
    void add_on_draw( const std::string& what ) {
      m_visual_funcs.push_back( what );
      check_duplicate_declaration( m_visual_funcs );
    }

    void add_on_stage( const std::string& what ) {
      m_fsn_funcs.push_back( what );
      check_duplicate_declaration( m_fsn_funcs );
    }

    void add_on_cmove( const std::string& what ) {
      m_cmove_funcs.push_back( what );
      check_duplicate_declaration( m_cmove_funcs );
    }

    void add_on_event( const std::string& what ) {
      m_events.push_back( what );
      check_duplicate_declaration( m_events );
    }

    void run_on_draw( ) {
      for( auto& it : m_visual_funcs ) {
        if( duk_peval_string_noresult( m_js_ctx, ( it + "( )" ).c_str( ) ) != 0 ) {
          g_con->log( "eval failed for %s", it.c_str( ) );
        }
      }
    }

    void run_on_stage( ) {
      for( auto& it : m_fsn_funcs ) {
        if( duk_peval_string_noresult( m_js_ctx, ( it + "( )" ).c_str( ) ) != 0 ) {
          g_con->log( "eval failed for %s", it.c_str( ) );
        }
      }
    }

    void run_on_cmove( ) {
      for( auto& it : m_cmove_funcs ) {
        if( duk_peval_string_noresult( m_js_ctx, ( it + "( )" ).c_str( ) ) != 0 ) {
          g_con->log( "eval failed for %s", it.c_str( ) );
        }
      }
    }

    void init( );

    static void exception_handler( void* udata, const char* msg );

    //add a func that can be executed inside the js env
    void add_func( duk_c_function function, size_t args, const char* name );

    //debug only
    void parse_string( const char* str );

    //parse a single file
    bool parse_file( const char* filename );

    //parse all files in a directory - we need to create one to parse from (e.g. /csgo/cfg/mb_scripts)
    void parse_files( );

    auto get_parsed( ) {
      return &m_parsed;
    }

  private:
    bool iserr( ) {
      bool ret = m_thrown;
      m_thrown = false;

      return ret;
    }

    // this is dumb, could just give each script their own context
    void check_duplicate_declaration( std::vector< std::string > vec ) {
      std::sort( vec.begin( ), vec.end( ) );

      const auto duplicate = std::adjacent_find( vec.begin( ), vec.end( ) );
      if( duplicate != vec.end( ) ) {
        g_con->log( std::string( *duplicate + xors( "( ) is already declared in another script." ) ).c_str( ) );
      }
    }

    void clear_files( );

    duk_context* m_js_ctx;
    bool m_thrown{ };

    std::vector< char* > m_files;
    std::vector< ui::dropdowns::dropdown_item_t< int > > m_parsed;

    std::vector< std::string > m_visual_funcs;
    std::vector< std::string > m_fsn_funcs;
    std::vector< std::string > m_cmove_funcs;
    std::vector< std::string > m_events;
  };
}

extern std::shared_ptr< js::c_js > g_js;