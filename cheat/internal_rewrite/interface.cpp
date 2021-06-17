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
#include "modules.hpp"
#include "wipe.hpp"

#include "js_funcs.h"

using namespace factory;

interfaces::c_interface_manager	g_factory;
factory::c_csgo					g_csgo;

__declspec( safebuffers ) bool factory::create_interfaces( c_csgo* instance, header_t* header ) {
	DELETE_START( 0 );

	instance->create( &instance->m_chl, xors( "VClient" ) );
	instance->create( &instance->m_surface, xors( "VGUI_Surface" ) );
	instance->create( &instance->m_panel, xors( "VGUI_Panel" ) );
	instance->create( &instance->m_engine, xors( "VEngineClient" ) );
	instance->create( &instance->m_entlist, xors( "VClientEntityList" ) );
	instance->create( &instance->m_prediction, xors( "VClientPrediction" ) );
	instance->create( &instance->m_cvar, xors( "VEngineCvar" ) );
	instance->create( &instance->m_game_movement, xors( "GameMovement" ) );
	instance->create( &instance->m_model_info, xors( "VModelInfoClient" ) );
	instance->create( &instance->m_trace, xors( "EngineTraceClient" ) );
	instance->create( &instance->m_model_render, xors( "VEngineModel" ) );
	instance->create( &instance->m_mat_system, xors( "VMaterialSystem" ) );
	instance->create( &instance->m_render_view, xors( "VEngineRenderView" ) );
	instance->create( &instance->m_phys_props, xors( "VPhysicsSurfaceProps" ) );
	instance->create( &instance->m_engine_sound, xors( "IEngineSoundClient" ) );
	instance->create( &instance->m_event_mgr, xors( "GAMEEVENTSMANAGER002" ) );
	instance->create( &instance->m_string_table, xors( "VEngineClientStringTable" ) );
	instance->create( &instance->m_partition, xors( "SpatialPartition" ) );
	instance->create( &instance->m_clientleaf, xors( "ClientLeafSystem002" ) );
	instance->create( &instance->m_mdl_cache, xors( "MDLCache" ) );
	instance->create( &instance->m_engine_vgui, xors( "VEngineVGui" ) );
	instance->create( &instance->m_input_sys, xors( "InputSystemVersion" ) );
	instance->create( &instance->m_effects, xors( "IEffects" ) );

	auto d3d_device = pattern::first_code_match( GetModuleHandleA( "shaderapidx9.dll" ),
		xors( "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) ) + 0x1;

#ifdef HEADER_MODULE
	auto move_helper = header->patterns.move_helper + 0x2;
#else
	auto move_helper = pattern::first_code_match( g_csgo.m_chl.dll( ),
		xors( "8B 0D ?? ?? ?? ?? 8B 46 08 68" ) ) + 0x2;
#endif

#ifdef HEADER_MODULE
	auto beam_ptr = header->patterns.beam_ptr + 0x1;
#else
	auto beam_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9" ), 0x1 );
#endif

	auto mm_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "89 4C 24 10 8B 0D ? ? ? ? 53" ), 0x6 );

	instance->create( &instance->m_d3d, **reinterpret_cast<uintptr_t**>( d3d_device ) );
	instance->create( &instance->m_move_helper, **reinterpret_cast<uintptr_t**>( move_helper ) );
	instance->create( &instance->m_beams, *reinterpret_cast<uintptr_t*>( beam_ptr ) );
	//instance->create( &instance->m_mm, **reinterpret_cast< uintptr_t** >( mm_ptr ) );

	instance->m_steam_pipe_handle = ( ( SteamPipeHandle( __cdecl* )( ) )GetProcAddress( (HMODULE) GetModuleHandleA( "steam_api.dll" ), xors( "SteamAPI_GetHSteamPipe" ) ) )( );
	instance->m_steam_user_handle = ( ( SteamUserHandle( __cdecl* )( ) )GetProcAddress( (HMODULE) GetModuleHandleA( "steam_api.dll" ), xors( "SteamAPI_GetHSteamUser" ) ) )( );

	uintptr_t steam_client_ptr = ( ( uintptr_t( __cdecl* )( void ) )GetProcAddress( (HMODULE) GetModuleHandleA( "steam_api.dll" ), xors( "SteamClient" ) ) )( );
	instance->create( &instance->m_steam_client, steam_client_ptr );

	uintptr_t steam_friends_ptr = (uintptr_t) instance->m_steam_client( )->GetISteamFriends( instance->m_steam_user_handle, instance->m_steam_pipe_handle, "SteamFriends015" );
	instance->create( &instance->m_steam_friends, steam_friends_ptr );

	auto kv_fn = ( void* ( __cdecl* )( ) )( GetProcAddress(
		(HMODULE) GetModuleHandleA( "vstdlib.dll" ),
		xors( "KeyValuesSystem" ) ) );

	instance->m_keyvalues = (IKeyValuesSystem*) kv_fn( );

	uintptr_t* globals = **reinterpret_cast<uintptr_t***>(
		instance->m_chl->get_old_function( 0 ) + 0x1b );
	instance->m_globals = reinterpret_cast<global_vars_t*>( globals );

	uintptr_t* client_mode = **reinterpret_cast<uintptr_t***>(
		instance->m_chl->get_old_function( 10 ) + 0x5 );
	instance->create( &instance->m_clientmode, uintptr_t( client_mode ) );

	uintptr_t input = *reinterpret_cast<uintptr_t*>(
		instance->m_chl->get_old_function( 16 ) + 0x1 );
	instance->create( &instance->m_input, input );

	instance->m_client_state = **reinterpret_cast<CClientState***>( instance->m_engine->get_old_function( 12 ) + 16 );
	auto clientstate = instance->m_client_state;

	if ( clientstate && clientstate->m_netchannel )
		instance->create( &instance->m_net_channel, (uintptr_t) clientstate->m_netchannel );

	uintptr_t var_ptr = (uintptr_t) instance->m_cvar( )->FindVar( xors( "weapon_debug_spread_show" ) );
	instance->create( &instance->m_debug_show_spread, var_ptr );

	uintptr_t cl_interpolate = (uintptr_t) ( instance->m_cvar( )->FindVar( xors( "cl_interpolate" ) ) );
	instance->create( &instance->m_interpolate, cl_interpolate );

	uintptr_t net_showfragments = (uintptr_t) ( instance->m_cvar( )->FindVar( xors( "net_showfragments" ) ) );
	instance->create( &instance->m_netshowfragments, net_showfragments );

	uintptr_t cl_csm_shadows = (uintptr_t) ( instance->m_cvar( )->FindVar( xors( "cl_csm_shadows" ) ) );
	instance->create( &instance->m_csm_shadows, cl_csm_shadows );

	instance->listeners.bullet_impact.init( );
	instance->listeners.player_hurt.init( );
	instance->listeners.round_start.init( );
	instance->listeners.molotov_detonate.init( );
	instance->listeners.weapon_fire.init( );
	instance->listeners.player_death.init( );
	instance->listeners.item_purchase.init( );

	DELETE_END( 0 );
	return true;
}


con_fn clear_fn{ fnv( "clear" ), [] ( const char*, const char* ) { g_con->m_logs.clear( ); }, "" };
con_fn enable_logs_fn{ fnv( "log_enable" ),
	[] ( const char* a, const char* ) {
		g_settings.menu.logs_enable ^= 1;
		g_con->log( g_settings.menu.logs_enable ? xors( "logs enabled" ) : xors( "logs disabled" ) );
	},
	""
};

con_fn fnv_fn{ fnv( "fnv" ),
	[] ( const char* a, const char* ) {
		auto print = hash::fnv1a( a );
		g_con->log( "%08x", print );
	},
	"%S"
};

con_fn sum_fn{ fnv( "sum" ),
	[] ( const char* a, const char* syntax ) {
		auto arg_1 = con_fn::get_arg< int >( a, 0, syntax );
		auto arg_2 = con_fn::get_arg< int >( a, 1, syntax );
		g_con->log( "%d + %d = %d", arg_1, arg_2, arg_1 + arg_2 );
	},
	"%D %D"
};

con_fn dump_cfg{ fnv( "dump_cfg" ),
	[] ( const char* , const char* ) {
		for ( auto& it : data::holder_.get_nodes( ) ) {
			auto setting = static_cast<ISetting*>( it );
			g_con->log( "%08x: %s", setting->get_hash( ), setting->get_string( ).c_str( ) );
		}
	},
	""
};

con_fn retard_fn{ fnv( "retard" ),
	[] ( const char*, const char* ) {
		g_con->log( "snuze" );
	},
	""
};

con_fn unload_fn{ fnv( "unload" ),
	[] ( const char*, const char* ) {
		g_csgo.m_panic = true;
		ExitProcess( 0 );
	},
	""
};

#undef PlaySound

con_fn play_sound{ fnv( "play_sound" ),
	[ & ] ( const char* a, const char* ) {
		std::string snd( a );
		snd.resize( snd.length( ) - 1 );
		g_con->log( "playing sound: %s (%d)", a, strlen( a ) );
		g_csgo.m_surface( )->PlaySound( snd.c_str( ) );
	},
	"%S"
};

con_fn set_hitsound{ fnv( "set_hitsound" ),
	[ & ] ( const char* a, const char* ) {
		std::string snd( a );
		snd.resize( snd.length( ) - 1 );
		g_con->log( "hitsound set to: %s", a );

		g_csgo.m_surface( )->PlaySound( snd.c_str( ) );
		strcpy( g_settings.misc.hitsound( ).data( ), snd.c_str( ) );
	},
	"%S"
};

con_fn parse_fn{ fnv( "parse_js" ),
	[] ( const char*, const char* ) {
		ui::was_setup = false;
		g_js->parse_files( );
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

__declspec( safebuffers ) void factory::c_csgo::initialize( ) {
	DELETE_START( 0 )

	m_panic = false;

	g_con->create( );
	auto now = std::chrono::high_resolution_clock::now( );
	g_con->print( xors( "hello\n" ) );

	factory::create_interfaces( &g_csgo, &g_header );

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
	g_con->register_fn( &play_sound );
	g_con->register_fn( &set_hitsound );
	g_con->register_fn( &parse_fn );

	hooks::commit( &g_csgo );

	g_cheat.m_chams.m_materials.initialize_materials( );

	util::calculate_spread_runtime( );

	auto after = std::chrono::high_resolution_clock::now( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) ); sleep is for the weak

	std::chrono::duration< float > delta = after - now;

	g_js->init( );

	g_js->add_func( js::fn::add_on_draw, 1, "add_on_draw" );
	g_js->add_func( js::fn::add_on_stage, 1, "add_on_stage" );
	g_js->add_func( js::fn::add_on_cmove, 1, "add_on_cmove" );

	g_js->add_func( js::fn::g_get_cur_time, 0, "g_get_cur_time" );
	g_js->add_func( js::fn::g_get_real_time, 0, "g_get_real_time" );
	g_js->add_func( js::fn::g_get_frame_time, 0, "g_get_frame_time" );
	g_js->add_func( js::fn::g_get_max_players, 0, "g_get_max_players" );
	g_js->add_func( js::fn::g_get_tick_count, 0, "g_get_tick_count" );
	g_js->add_func( js::fn::g_get_tick_interval, 0, "g_get_tick_interval" );
	g_js->add_func( js::fn::g_get_frame_count, 0, "g_get_frame_count" );
	g_js->add_func( js::fn::g_get_screen_size, 0, "g_get_screen_size" );
	g_js->add_func( js::fn::g_get_max_entities, 0, "g_get_max_entities" );
	g_js->add_func( js::fn::g_get_frame_stage, 0, "g_get_frame_stage" );
	g_js->add_func( js::fn::g_get_latency, 0, "g_get_latency" );
	g_js->add_func( js::fn::g_get_key_pressed, 1, "g_get_key_pressed" );
	g_js->add_func( js::fn::g_exec, 1, "g_exec" );
	g_js->add_func( js::fn::g_get_cmd, 0, "g_get_cmd" );
	g_js->add_func( js::fn::g_trace_ray, 3, "g_trace_ray" );
	g_js->add_func( js::fn::g_get_view_angles, 0, "g_get_view_angles" );
	g_js->add_func( js::fn::g_set_view_angles, 1, "g_set_view_angles" );
	g_js->add_func( js::fn::g_print_to_chat, 1, "g_print_to_chat" );
	g_js->add_func( js::fn::g_log, 1, "g_log" );

	// drawing
	g_js->add_func( js::fn::v_draw_string, 4, "v_draw_string" );
	g_js->add_func( js::fn::v_draw_line, 4, "v_draw_line" );
	g_js->add_func( js::fn::v_draw_rect, 4, "v_draw_rect" );
	g_js->add_func( js::fn::v_draw_filled_rect, 4, "v_draw_filled_rect" );
	g_js->add_func( js::fn::v_draw_circle, 3, "v_draw_circle" );
	g_js->add_func( js::fn::v_set_clr, 4, "v_set_clr" );
	g_js->add_func( js::fn::v_world_to_screen, 1, "v_world_to_screen" );

	// ui
	g_js->add_func( js::fn::ui_get_value, DUK_VARARGS, "ui_get_value" );
	g_js->add_func( js::fn::ui_set_value, DUK_VARARGS, "ui_set_value" );
	g_js->add_func( js::fn::ui_add_form, DUK_VARARGS, "ui_add_form" );
	g_js->add_func( js::fn::ui_add_checkbox, DUK_VARARGS, "ui_add_checkbox" );
	g_js->add_func( js::fn::ui_add_slider_int, DUK_VARARGS, "ui_add_slider_int" );
	g_js->add_func( js::fn::ui_add_slider_float, DUK_VARARGS, "ui_add_slider_float" );
	g_js->add_func( js::fn::ui_add_hotkey, DUK_VARARGS, "ui_add_hotkey" );
	g_js->add_func( js::fn::ui_add_dropdown, DUK_VARARGS, "ui_add_dropdown" );
	g_js->add_func( js::fn::ui_set_visible, DUK_VARARGS, "ui_set_visible" );
	g_js->add_func( js::fn::ui_is_menu_open, 0, "ui_is_menu_open" );

	// entity
	g_js->add_func( js::fn::e_get_all, 0, "e_get_all" );
	g_js->add_func( js::fn::e_get_players, 0, "e_get_players" );
	g_js->add_func( js::fn::e_get_local_player, 0, "e_get_local_player" );
	g_js->add_func( js::fn::e_is_valid, 1, "e_is_valid" );
	g_js->add_func( js::fn::e_is_enemy, 1, "e_is_enemy" );
	g_js->add_func( js::fn::e_is_dormant, 1, "e_is_dormant" );
	g_js->add_func( js::fn::e_is_alive, 1, "e_is_alive" );
	g_js->add_func( js::fn::e_get_hitbox_pos, 2, "e_get_hitbox_pos" );
	g_js->add_func( js::fn::e_get_name, 1, "e_get_name" );
	g_js->add_func( js::fn::e_get_steam64, 1, "e_get_steam64" );
	g_js->add_func( js::fn::e_get_ping, 1, "e_get_ping" );
	g_js->add_func( js::fn::e_get_prop, 3, "e_get_prop" );
	g_js->add_func( js::fn::e_set_prop, 4, "e_set_prop" );
	g_js->add_func( js::fn::e_get_class_id, 1, "e_get_class_id" );
	g_js->add_func( js::fn::e_get_class_name, 1, "e_get_class_name" );
	g_js->add_func( js::fn::e_get_eye_pos, 1, "e_get_eye_pos" );

	// cvar
	g_js->add_func( js::fn::cvar_get_int, 1, "cvar_get_int" );
	g_js->add_func( js::fn::cvar_set_int, 2, "cvar_set_int" );
	g_js->add_func( js::fn::cvar_get_float, 1, "cvar_get_float" );
	g_js->add_func( js::fn::cvar_set_float, 2, "cvar_set_float" );
	g_js->add_func( js::fn::cvar_get_string, 1, "cvar_get_string" );
	g_js->add_func( js::fn::cvar_set_string, 2, "cvar_set_string" );

	g_con->log( xors( "cheat initialized\n" ) );
	g_con->log( xors( "time taken: %f\n" ), delta.count( ) );

	g_con->game_console_print( xors( "lol hacked in %f\n" ), delta.count( ) );

	g_con->print( "this works too\n" );

	// open the menu so people actually know when the cheat loaded
	g_settings.menu.open = true;

	while ( !ui::was_setup ) {
		Sleep( 1000 );
	}

	g_js->parse_files( );

	DELETE_END( 0 );
}

__declspec( safebuffers ) void factory::c_csgo::uninitialize( ) {
	m_panic = true;
	g_con->destroy( );

	//restore wndproc
	if ( hooks::window_procedure_o ) {
		SetWindowLongA( m_hwnd, GWLP_WNDPROC, (long) hooks::window_procedure_o );
	}

	//unreference materials
	g_cheat.m_chams.m_materials.destroy_materials( );

	//restore all interfaces
	restore( );

	Sleep( 100 );

	//free the library
	FreeLibraryAndExitThread( g_dll, 0 );
}
