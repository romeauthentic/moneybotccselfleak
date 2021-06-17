#include "identity.hpp"
#include "settings.hpp"
#include "interface.hpp"
#include "pattern.hpp"

#include "context.hpp"

namespace features
{
	void c_identity::clantag_changer( ) {
		static auto fn_offset = pattern::first_code_match( g_csgo.m_engine.dll( ), xors( "53 56 57 8B DA 8B F9 FF 15" ) );
		static auto fn = reinterpret_cast< void( __fastcall* )( const char*, const char* ) >( fn_offset );
		static bool was_enabled = false;

		if ( !g_settings.misc.clantag_changer( ) ) {
			if ( was_enabled ) {
				fn( "", " " );
				static cvar_t* cl_clanid = g_csgo.m_cvar( )->FindVar( xors( "cl_clanid" ) );
				auto backup_val = cl_clanid->get_int( );
				cl_clanid->set_value( 0 );
				cl_clanid->set_value( backup_val );
				was_enabled = false;
			}

			return;
		}

		static const std::string moneybot_string = xors( "moneybot      " );
		static std::string clantag_str = moneybot_string;

		static auto next_settime = g_csgo.m_globals->m_curtime;
		auto curtime = g_csgo.m_globals->m_curtime;

		if( !was_enabled || std::abs( curtime - next_settime ) > 1.f ) {
			auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );

			if( nci ) {
				auto tick = TIME_TO_TICKS( g_ctx.pred_time( ) + nci->GetLatency( 0 ) );
				clantag_str = moneybot_string;
				if( tick % 15 )
					return;

				next_settime = curtime;
			}
		}

		was_enabled = true;

		if ( curtime > next_settime ) {
			const int length = clantag_str.length( );
			clantag_str.insert( 0, 1, clantag_str[ length - 2 ] );
			clantag_str.erase( length - 1, 1 );

			std::string set = clantag_str;
			set.resize( 15 );
			set[ 14 ] = '$';

			auto is_full = set.find( xors( "moneybot" ) ) != std::string::npos;

			next_settime = curtime + ( is_full ? 0.85f : 0.4f );
			fn( set.c_str( ), set.c_str( ) );
		}
	}

	void c_identity::name_changer( ) {
		static auto cvar = g_csgo.m_cvar( )->FindVar( xors( "name" ) );
		auto set_name = [ ]( const char* name ) {
			*( int* )( uintptr_t( &cvar->m_change_callback ) + 0xc ) = 0;
			cvar->set_value( name );
		};

		static bool activated = false;
		static char original_name[ 100 ]{ };

		if( !g_settings.misc.name_changer ) {
			if( activated ) {
				set_name( original_name );
			}

			activated = false;
			return;
		}

		if( !activated ) {
			strcpy( original_name, cvar->get_string( ) );
			if( g_settings.misc.name_changer == 1 ) {
				char new_name[ 128 ];
				memset( new_name, '$', sizeof( new_name ) );
				set_name( new_name );
			}
			if( g_settings.misc.name_changer == 2 ) {
				set_name( xors( "moneybot.cc" ) );
			}
			if( g_settings.misc.name_changer == 101 ) {
				set_name( "ннн\n\xAD\xAD\xAD" );
			}
		}
		else if( g_settings.misc.name_changer == 101 ) {
			char new_name[ 15 ];
			for( size_t i{ }; i < 15; ++i ) {
				auto is_upper = !( math::random_number( 0, 256 ) & 1 );
				new_name[ i ] = is_upper ? math::random_number( 65, 90 ) : math::random_number( 97, 122 );
			}

			set_name( new_name );
		}

		activated = true;
	}
}