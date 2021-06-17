#pragma once

#include <memory>
#include "sdk.h"
#include "vmt.h"
#include "ctx.hpp"
#include "listener.hpp"
#include "factory.h"
struct IDirect3DDevice9;
namespace tf2
{
template < typename t >
class interface_base {
	friend class client;
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


	class client {
	public:
		interface_base< IEngineVGui >			m_vgui;
		interface_base< IClientMode >			m_clientmode;
		interface_base< IEngineTrace >			m_trace;
		interface_base< IVEngineClient >		m_engine;
		interface_base< IInputSystem >			m_input;
		interface_base< chl_client >			m_chl;
		interface_base< IVDebugOverlay >		m_overlay;
		interface_base< client_ent_list >		m_entlist;
		interface_base< IPlayerInfoManager >	m_playerinfo;
		interface_base< ISurface >				m_surface;
		interface_base< IPanel >				m_panel;
		interface_base< ICVar >					m_cvar;
		interface_base< IVRenderView >			m_renderview;
		interface_base< IPrediction >			m_prediction;
		interface_base< IGameMovement >			m_movement;
		interface_base< IVModelInfo >			m_modelinfo;
		interface_base< uintptr_t >				m_d3d;
		interface_base< IVModelRender >			m_model_render;
		interface_base< IVRenderView >			m_render_view;
		interface_base< uintptr_t >				m_engine_sound;
		interface_base< IGameEventManager2 >	m_event_mgr;
		interface_base< IMaterialSystem >		m_mat_system;
		interface_base< CStringTable >			m_string_table;
		interface_base< IViewRenderBeams >		m_beams;
		IKeyValuesSystem*						 m_keyvalues;
		CGlobalVarsBase*						m_globals;
		IMoveHelper*							m_movehelper;
		CInput*									m_hl_input;
		bool									m_panic;
		HWND									m_hwnd{ };
		float									m_frametime;

		struct {
			generic_listener_t player_hurt{ xors( "player_hurt" ), &listeners::player_hurt };
		} listeners;

	public:
		//find the interface inside of the module
		void create( void* iface, const std::string& name, const std::string& mod ) {
			auto* interface_ = static_cast< interface_base< uintptr_t >* >( iface );
			auto ptr = g_factory.find_interface( mod, name );
			if( !ptr )
				return;

			( *interface_ )( ptr );
			m_container.push_back( interface_ );
		}

		void create( void* iface, uintptr_t pointer ) {
			auto interface_ = static_cast< interface_base< uintptr_t >* >( iface );

			//msvc is retarded
			( *interface_ )( ( void* )pointer );
			m_container.push_back( interface_ );
		}

		void create( void* iface, const std::string& name ) {
			auto* interface_ = static_cast< interface_base< uintptr_t >* >( iface );
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
			for( const auto& it : m_container ) {
				it->m_hook->restore( );
			}
		}

	private:
		std::vector< interface_base< uintptr_t >* > m_container;
	};

	bool create_interfaces( client * );
}

extern tf2::client cl;
