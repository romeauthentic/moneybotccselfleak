#include <thread>

#include "interface.hpp"
#include "console.hpp"
#include "hooks.hpp"
#include "renderer.hpp"
#include "pattern.hpp"
#include "hooks.hpp"
#include "base_cheat.hpp"
#include "settings.hpp"
#include "context.hpp"

using namespace factory;

interfaces::c_interface_manager	g_factory;
factory::c_csgo					g_csgo;

bool factory::create_interfaces( c_csgo* instance ) {

	instance->create( &instance->m_chl,			  xors( "VClient" ) );
	instance->create( &instance->m_surface,		  xors( "VGUI_Surface" ) );
	instance->create( &instance->m_panel,		  xors( "VGUI_Panel" ) );
	instance->create( &instance->m_engine,		  xors( "VEngineClient" ) );
	instance->create( &instance->m_entlist,		  xors( "VClientEntityList" ) );
	instance->create( &instance->m_prediction,    xors( "VClientPrediction" ) );
	instance->create( &instance->m_cvar,		  xors( "VEngineCvar" ) );
	instance->create( &instance->m_game_movement, xors( "GameMovement" ) );
	instance->create( &instance->m_model_info,	  xors( "VModelInfoClient" ) );
	instance->create( &instance->m_trace,		  xors( "EngineTraceClient" ) );
	instance->create( &instance->m_model_render,  xors( "VEngineModel" ) );
	instance->create( &instance->m_mat_system,    xors( "VMaterialSystem" ) );
	instance->create( &instance->m_render_view,   xors( "VEngineRenderView" ) );
	instance->create( &instance->m_phys_props,	  xors( "VPhysicsSurfaceProps" ) );
	instance->create( &instance->m_engine_sound,  xors( "IEngineSoundClient" ) );
	instance->create( &instance->m_event_mgr,	  xors( "GAMEEVENTSMANAGER002" ) );
	instance->create( &instance->m_string_table,  xors( "VEngineClientStringTable" ) );
	instance->create( &instance->m_partition,	  xors( "SpatialPartition" ) );
	instance->create( &instance->m_clientleaf,	  xors( "ClientLeafSystem002" ) );
	instance->create( &instance->m_mdl_cache,	  xors( "MDLCache" ) );
	instance->create( &instance->m_engine_vgui,	  xors( "VEngineVGui" ) );
	instance->create( &instance->m_input_sys,	  xors( "InputSystemVersion" ) );

	auto d3d_device = pattern::first_code_match( GetModuleHandleA( xors( "shaderapidx9.dll" ) ),
		xors( "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) ) + 0x1;

	auto move_helper = pattern::first_code_match( g_csgo.m_chl.dll( ),
		xors( "8B 0D ?? ?? ?? ?? 8B 46 08 68" ) ) + 0x2;

	auto beam_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ), 0x1 );

	auto mm_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "89 4C 24 10 8B 0D ? ? ? ? 53" ), 0x6 );

	instance->create( &instance->m_d3d, **reinterpret_cast< uintptr_t** >( d3d_device ) );
	instance->create( &instance->m_move_helper, **reinterpret_cast< uintptr_t** >( move_helper ) );
	instance->create( &instance->m_beams, *reinterpret_cast< uintptr_t* >( beam_ptr ) );
	//instance->create( &instance->m_mm, **reinterpret_cast< uintptr_t** >( mm_ptr ) );

	instance->m_steam_pipe_handle = ( ( SteamPipeHandle( __cdecl* )( ) )GetProcAddress( GetModuleHandleA( xors( "steam_api.dll" ) ), xors( "SteamAPI_GetHSteamPipe" ) ) )( );
	instance->m_steam_user_handle = ( ( SteamUserHandle( __cdecl* )( ) )GetProcAddress( GetModuleHandleA( xors( "steam_api.dll" ) ), xors( "SteamAPI_GetHSteamUser" ) ) )( );

	uintptr_t steam_client_ptr = ( ( uintptr_t ( __cdecl* )( void ) )GetProcAddress( GetModuleHandleA( xors( "steam_api.dll" ) ), xors( "SteamClient" ) ) )( );
	instance->create( &instance->m_steam_client, steam_client_ptr );

	uintptr_t steam_friends_ptr = ( uintptr_t )instance->m_steam_client( )->GetISteamFriends( instance->m_steam_user_handle, instance->m_steam_pipe_handle, "SteamFriends015" );
	instance->create( &instance->m_steam_friends, steam_friends_ptr );


	auto kv_fn = ( void*( __cdecl* )( ) )( GetProcAddress(
		GetModuleHandleA( xors( "vstdlib.dll" ) ),
		xors( "KeyValuesSystem" ) ) );

	instance->m_keyvalues = ( IKeyValuesSystem* )kv_fn( );

	instance->m_alloc = *( IMemAlloc** )GetProcAddress( 
		GetModuleHandleA( xors( "tier0.dll" ) ), 
		xors( "g_pMemAlloc" ) );

	uintptr_t* globals = **reinterpret_cast< uintptr_t*** >( 
		instance->m_chl->get_old_function( 0 ) + 0x1b );
	instance->m_globals = reinterpret_cast< global_vars_t* >( globals );

	uintptr_t* client_mode = **reinterpret_cast< uintptr_t*** >( 
		instance->m_chl->get_old_function( 10 ) + 0x5 );
	instance->create( &instance->m_clientmode, uintptr_t( client_mode ) );

	uintptr_t input = *reinterpret_cast< uintptr_t* >(
		instance->m_chl->get_old_function( 16 ) + 0x1 );
	instance->create( &instance->m_input, input );

	uintptr_t** global_state = pattern::first_code_match< uintptr_t** >( instance->m_engine.dll( ),
		xors( "B9 ? ? ? ? 56 FF 50 14 8B 34 85" ), 0x1 );
	instance->m_global_state = reinterpret_cast< CGlobalState* >( *global_state );

	auto clientstate = instance->m_global_state->get_client_state( );
	if( clientstate && clientstate->m_netchannel )
		instance->create( &instance->m_net_channel, ( uintptr_t )clientstate->m_netchannel );

	uintptr_t var_ptr = ( uintptr_t )instance->m_cvar( )->FindVar( xors( "weapon_debug_spread_show" ) );
	instance->create( &instance->m_debug_show_spread, var_ptr );

	uintptr_t cl_interpolate = ( uintptr_t )( instance->m_cvar( )->FindVar( xors( "cl_interpolate" ) ) );
	instance->create( &instance->m_interpolate, cl_interpolate );

	uintptr_t net_showfragments = ( uintptr_t )( instance->m_cvar( )->FindVar( xors( "net_showfragments" ) ) );
	instance->create( &instance->m_netshowfragments, net_showfragments );

	instance->listeners.player_death.init();
	instance->listeners.bullet_impact.init( );
	instance->listeners.player_hurt.init( );
	instance->listeners.round_start.init( );
	instance->listeners.molotov_detonate.init( );
	instance->listeners.weapon_fire.init( );

	return true;
}


con_fn clear_fn{ fnv( "clear" ), [ ]( const char*, const char* ) { g_con->m_logs.clear( ); }, "" };
con_fn enable_logs_fn{ fnv( "log_enable" ),
	[ ]( const char* a, const char* ) {
		g_settings.menu.logs_enable ^= 1;
		g_con->log( g_settings.menu.logs_enable ? xors( "logs enabled" ) : xors( "logs disabled" ) );
	},
	""
};

con_fn fnv_fn{ fnv( "fnv" ), 
	[ ]( const char* a, const char* ) { 
		auto print = hash::fnv1a( a ); 
		g_con->log( "%08x", print ); 
	}, 
	"%S" 
};

con_fn sum_fn{ fnv( "sum" ),
	[ ]( const char* a, const char* syntax ) {
		auto arg_1 = con_fn::get_arg< int >( a, 0, syntax );
		auto arg_2 = con_fn::get_arg< int >( a, 1, syntax );
		g_con->log( "%d + %d = %d", arg_1, arg_2, arg_1 + arg_2 );
	},
	"%D %D"
};

con_fn dump_cfg{ fnv( "dump_cfg" ),
	[ ]( const char* , const char* ) {
		for( auto& it : data::holder_.get_nodes( ) ) {
			auto setting = static_cast< ISetting* >( it );
			g_con->log( "%08x: %s", setting->get_hash( ), setting->get_string( ).c_str( ) );
		}
	},
	""
};

con_fn retard_fn{ fnv( "retard" ),
	[ ]( const char*, const char* ) {
		g_con->log( "snuze" );
	},
	""
};

con_fn unload_fn{ fnv( "unload" ),
	[ ]( const char*, const char* ) {
		g_csgo.m_panic = true;
		ExitProcess( 0 );
	},
	""
};


/*con_fn alias_fn{ fnv( "alias" ),
	[ ]( const char* a, const char* b ) {
		auto var_type = std::string( b );
		if( var_type.length( ) > 1 ) {
			g_con->log( xors( "unknown var type: %s" ), var_type.c_str( ) );
			return;
		}

		switch( b[ 0 ] ) {
		case TYPE_STRING: {
			std::shared_ptr< con_alias< const char* > >( fnv(  ) )
		}
		}
	},
	"%S %S"
};*/

void factory::c_csgo::initialize( ) {
	m_panic = true;
	g_con->create( );
	auto now = std::chrono::high_resolution_clock::now( );
	g_con->print( xors( "hello\n" ) );
									//hi


	factory::create_interfaces( &g_csgo );
	g_fonts.initialize( );

	g_netvars.init( );	
	g_settings.load( );
	g_con->register_fn( &clear_fn );
	g_con->register_fn( &enable_logs_fn );
	g_con->register_fn( &fnv_fn );
	g_con->register_fn( &sum_fn );
	g_con->register_fn( &dump_cfg );
	g_con->register_fn( &retard_fn );
	g_con->register_fn( &unload_fn );

	hooks::commit( &g_csgo );
		
	g_cheat.m_chams.m_materials.initialize_materials( );

	auto after = std::chrono::high_resolution_clock::now( );
	std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	std::chrono::duration< float > delta = after - now;
	g_con->log( xors( "cheat initialized\n" ) );
	g_con->log( xors( "time taken: %f\n" ), delta.count( ) );



	m_panic = false;
}

void factory::c_csgo::uninitialize( ) {
	m_panic = true;
	g_con->destroy( );
		
	//restore wndproc
	if ( hooks::window_procedure_o ) {
		SetWindowLongA( m_hwnd, GWLP_WNDPROC, ( long )hooks::window_procedure_o );
	}

	//unreference materials
	g_cheat.m_chams.m_materials.destroy_materials( );

	//restore all interfaces
	restore( );

	Sleep( 100 );

	//free the library
	FreeLibraryAndExitThread( g_dll, 0 );
}
