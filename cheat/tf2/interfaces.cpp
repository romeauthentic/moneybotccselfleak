#include "interfaces.h"
#include "console.hpp"
#include "hooks.h"
#include "renderer.hpp"
#include "pattern.hpp"
#include "base_cheat.h"
#include "settings.h"
#include "ctx.hpp"
#include "netvars.h"
#include <thread>

tf2::client cl;
tf2::interfaces::c_interface_manager g_factory;

bool tf2::create_interfaces( client *instance )
{
	if ( !instance ) {
		return false;
	}

	

	auto d3d_device = pattern::first_code_match( GetModuleHandleA( xors( "shaderapidx9.dll" ) ),
		xors( "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C" ) ) + 0x1;

	auto move_helper = pattern::first_code_match( GetModuleHandleA( xors( "client.dll" ) ),
		xors( "8B 0D ?? ?? ?? ?? 8B 46 08 68" ) ) + 0x2;

	//sig to containing function: (+0x69) E9 ? ? ? ? CC 55 8B EC 5D 
	//auto beam_ptr = pattern::first_code_match( GetModuleHandleA( xors( "client.dll" ) ), xors( "" ) ); 

	//g_con->print( "beam_found: 0x%08x\n", beam_ptr );

	instance->create( &instance->m_d3d, **reinterpret_cast< uintptr_t** >( d3d_device ) );
	instance->create( &instance->m_movehelper, **reinterpret_cast< uintptr_t** >( move_helper ) );
	//instance->create( &instance->m_beams, *reinterpret_cast< uintptr_t* >( beam_ptr ) );


	/*
	

	important

	issues:

	the background is white but so am i so really this isnt an issue
	^this got fixed

	THIS ENTIRE THING IS MESSY AND RIDDLED WITH SHIT COMMENTS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

	chams

	fucking keyvalues
	^this needs a new sig

	glow count aint happy cbf looking into it

	menu goes invis because ????? gj menu
	^this is fixed

	too lazy to fix d3d
	^only sprites are broken ( menu tabs )

	emitsound hook is different
	^not important, might remove

	aimbot is meh meh rn
	^works, mostly, just paste the one from csgo over (it's been updated)

	lots of stuff not done that i want to get done, just wanna get it out there so i can keep working on it and not get harrased for the push




	
	*/

	//tf2::interfaces::interface_manager factory;
	instance->create( &instance->m_chl, xors( "VClient" ) );
	instance->create( &instance->m_vgui, xors( "VEngineVGui" ) );
	instance->create( &instance->m_surface, xors( "VGUI_Surface" ) );
	instance->create( &instance->m_input, xors( "InputSystemVersion" ) );
	instance->create( &instance->m_panel, xors( "VGUI_Panel" ) );
	instance->create( &instance->m_engine, xors( "VEngineClient" ) );
	instance->create( &instance->m_entlist, xors( "VClientEntityList" ) );
	instance->create( &instance->m_prediction, xors( "VClientPrediction" ) );
	instance->create( &instance->m_cvar, xors( "VEngineCvar" ) );
	instance->create( &instance->m_movement, xors( "GameMovement" ) );
	instance->create( &instance->m_modelinfo, xors( "VModelInfoClient" ) );
	instance->create( &instance->m_overlay, "VDebugOverlay" );
	instance->create( &instance->m_trace, xors( "EngineTraceClient" ) );
	instance->create( &instance->m_model_render, xors( "VEngineModel" ) );
	instance->create( &instance->m_mat_system, xors( "VMaterialSystem" ) );
	instance->create( &instance->m_render_view, xors( "VEngineRenderView" ) );
	instance->create( &instance->m_engine_sound, xors( "IEngineSoundClient" ) );
	instance->create( &instance->m_event_mgr, xors( "GAMEEVENTSMANAGER002" ) );
	instance->create( &instance->m_string_table, xors( "VEngineClientStringTable" ) );
	printf( "A\n" );
	instance->create( &instance->m_clientmode, **reinterpret_cast< uintptr_t** >( instance->m_chl->get_function( 10 ) + 5 ) );

	instance->m_globals = *reinterpret_cast< CGlobalVarsBase** >( pattern::first_code_match( instance->m_engine.dll( ), "A1 ? ? ? ? 8B 11 68", 0x8 ) );

	uintptr_t* input = *reinterpret_cast< uintptr_t** >(
		instance->m_chl->get_old_function( 1 ) + 0x1 );
	instance->m_hl_input = reinterpret_cast< CInput* >( input );

	auto kv_fn = ( void*( __cdecl* )( ) )( GetProcAddress(
		GetModuleHandleA( xors( "vstdlib.dll" ) ),
		xors( "KeyValuesSystem" ) ) );

	instance->m_keyvalues = ( IKeyValuesSystem* )kv_fn( );
	instance->listeners.player_hurt.init( );

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
	cl.m_panic = true;
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

void tf2::client::initialize( ) {
	//m_panic = true;
	g_con->create( );
	auto now = std::chrono::high_resolution_clock::now( );
	g_con->print( xors( "hello\n" ) );
	//hi
	//bye

	tf2::create_interfaces( &cl );
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

	hooks::commit( &cl );


	auto offset = g_netvars.get_netvar( fnv( "DT_WeaponTFBaseGun" ), fnv( "m_fLastShotTime" ) );
	g_con->log( "lastshottime: %08x", offset );
	g_con->log( "prop: %08x", g_netvars.get_prop( fnv( "DT_WeaponTFBaseGun" ), fnv( "m_fLastShotTime" ) ) );

	auto after = std::chrono::high_resolution_clock::now( );
	std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	std::chrono::duration< float > delta = after - now;
	g_con->log( xors( "cheat initialized\n" ) );
	g_con->log( xors( "time taken: %f\n" ), delta.count( ) );



	m_panic = false;
}

void tf2::client::uninitialize( ) {
	m_panic = true;
	g_con->destroy( );

	//restore wndproc
	if( hooks::window_procedure_o ) {
		SetWindowLongA( m_hwnd, GWLP_WNDPROC, ( long )hooks::window_procedure_o );
	}

	//unreference materials
	//g_cheat.chams.m_materials.destroy_materials( );

	//restore all interfaces
	restore( );

	Sleep( 100 );

	//free the library
	FreeLibraryAndExitThread( g_dll, 0 );
}