#pragma once
#include <thread>

#include "factory.hpp"
#include "sdk.hpp"
#include "listener.hpp"

NAMESPACE_REGION( factory )

struct IDirect3DDevice9;

//interface holder
template < typename t >
class c_interface_base {
	friend class c_csgo;
public:
	t* operator( )( ) {
		return m_interface;
	}

	//calls a virtual
	template< size_t index, typename ret = void,
		class... Args >
		ret call( Args... args ) {
		return util::get_vfunc< ret( __thiscall* )( void*, Args... ) >
			( m_interface, index )( m_interface, args... );
	}

	//also calls a virtual but you supply the typedef instead of return type
	template< typename def, size_t index, class... Args >
	auto call( Args... args ) {
		return util::get_vfunc< def >
			( m_interface, index )( m_interface, args... );
	}

	//allows you to cast the interface
	template< typename t0 = t* >
	t0 get( ) {
		return ( t0 )m_interface;
	}

	void operator( )( t* ptr ) {
		m_interface = ptr;
		m_hook.reset( );
		m_hook = std::make_shared< hooks::c_vmt >( static_cast< void* >( ptr ) );

		/*find the module the interface is located in
		i know i could just pass the pointer but traps are gay*/
		MEMORY_BASIC_INFORMATION info;
		VirtualQuery( ptr, &info, sizeof( MEMORY_BASIC_INFORMATION ) );
		
		m_dll = static_cast< HMODULE >( info.AllocationBase );
	}

	void operator( )( void* ptr ) {
		operator( )( static_cast< t* >( ptr ) );
	}

	decltype( auto ) operator->( ) {
		return m_hook;
	}

	template< typename t2 = decltype( m_dll ) >
	t2 dll( ) {
		return ( t2 )m_dll;
	}

protected:
	t* m_interface;
	HMODULE m_dll;
	std::shared_ptr< hooks::c_vmt > m_hook;
};

//declare your interfaces here
class c_csgo {
public:
	c_interface_base< IBaseClientDLL >		 m_chl;
	c_interface_base< ISurface >			 m_surface;
	c_interface_base< IPanel >				 m_panel;
	c_interface_base< IClientMode >			 m_clientmode;
	c_interface_base< IVEngineClient >		 m_engine;
	c_interface_base< IClientEntityList >	 m_entlist;
	c_interface_base< IDirect3DDevice9 >	 m_d3d;
	c_interface_base< IPrediction >          m_prediction;
	c_interface_base< ICSGameMovement >      m_game_movement;
	c_interface_base< IMoveHelper >          m_move_helper;
	c_interface_base< IVModelInfoClient >	 m_model_info;
	c_interface_base< IVModelRender >		 m_model_render;
	c_interface_base< IMaterialSystem >		 m_mat_system;
	c_interface_base< IEngineTrace >		 m_trace;
	c_interface_base< IPhysicsSurfaceProps > m_phys_props;
	c_interface_base< IVRenderView >		 m_render_view;
	c_interface_base< ICVar >				 m_cvar;
	c_interface_base< IGameEventManager2 >   m_event_mgr;
	c_interface_base< CHudChat >			 m_hud_chat;
	c_interface_base< uintptr_t >			 m_engine_sound;
	c_interface_base< INetChannel >			 m_net_channel;
	c_interface_base< cvar_t >				 m_debug_show_spread;
	c_interface_base< cvar_t >				 m_interpolate;
	c_interface_base< cvar_t >				 m_netshowfragments;
	c_interface_base< ISteamClient >		 m_steam_client;
	c_interface_base< ISteamFriends >		 m_steam_friends;
	c_interface_base< IViewRenderBeams >	 m_beams;
	//c_interface_base< CMatchFramework >		 m_mm;
	c_interface_base< uintptr_t >			 m_input_sys;
	c_interface_base< CStringTable >		 m_string_table;
	c_interface_base< CInput >				 m_input;
	c_interface_base< uintptr_t	>			 m_partition;
	c_interface_base< uintptr_t >			 m_clientleaf;
	c_interface_base< uintptr_t >			 m_mdl_cache;
	c_interface_base< uintptr_t >			 m_engine_vgui;
	c_interface_base< c_base_player >		 m_local;
	SteamUserHandle							 m_steam_user_handle;
	SteamPipeHandle							 m_steam_pipe_handle;
	IKeyValuesSystem*						 m_keyvalues;
	global_vars_t*					         m_globals;
	CGlobalState*							 m_global_state;
	IMemAlloc*								 m_alloc;
	float									 m_frametime;
	std::thread::id							 m_main_thread;
	std::thread::id							 m_d3d_thread;


	HWND m_hwnd{ };
	bool m_panic{ };

	struct {
		generic_listener_t player_death{ xors("player_death"), &listeners::player_death };
		generic_listener_t bullet_impact{ xors( "bullet_impact" ), &listeners::bullet_impact };
		generic_listener_t player_hurt{ xors( "player_hurt" ), &listeners::player_hurt };
		generic_listener_t round_start{ xors( "round_start" ), &listeners::round_start };
		generic_listener_t molotov_detonate{ xors( "molotov_detonate" ), &listeners::molotov_detonate };
		generic_listener_t weapon_fire{ xors( "weapon_fire" ), &listeners::weapon_fire };
	} listeners;
public:
	//find the interface inside of the module
	void create( void* iface, const std::string& name, const std::string& mod ) {
		auto* interface_ = static_cast< c_interface_base< uintptr_t >* >( iface );
		auto ptr = g_factory.find_interface( mod, name );
		if( !ptr )
			return;

		( *interface_ )( ptr );
		m_container.push_back( interface_ );
	}
	
	void create( void* iface, uintptr_t pointer ) {
		auto interface_ = static_cast< c_interface_base< uintptr_t >* >( iface );

		//msvc is retarded
		( *interface_ )( ( void* )pointer );
		m_container.push_back( interface_ );
	}

	void create( void* iface, const std::string& name ) {
		auto* interface_ = static_cast< c_interface_base< uintptr_t >* >( iface );
		auto ptr = g_factory.find_interface( name );
		if( !ptr )
			return;

		( *interface_ )( ptr );
		m_container.push_back( interface_ );
	}

	void __cdecl initialize( );
	void __cdecl uninitialize( );

private:
	//restore the vmts to their original
	void restore( ) {
		for ( const auto& it : m_container ) {
			it->m_hook->restore( );
		}
	}

private:
	std::vector< c_interface_base< uintptr_t >* > m_container;
};

bool create_interfaces( c_csgo* instance );

END_REGION

extern factory::c_csgo g_csgo;