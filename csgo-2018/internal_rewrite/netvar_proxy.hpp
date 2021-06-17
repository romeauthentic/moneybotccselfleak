#pragma once
#include "Valve/dt_recv.h"

//dont leave things out of namespaces
namespace hooks
{
	class c_netvar_proxy {
	public:
		c_netvar_proxy( RecvProp* target_property, RecvVarProxyFn new_proxy ) {
			m_target = target_property;
			m_original = target_property->m_ProxyFn;
			m_target->m_ProxyFn = new_proxy;
		}

		c_netvar_proxy( ) = default;

		void init( RecvProp* target, RecvVarProxyFn new_proxy ) {
			m_target = target;
			m_original = target->m_ProxyFn;
			m_target->m_ProxyFn = new_proxy;
		}

		~c_netvar_proxy( ) { 
			if( !m_target ) return; 
			m_target->m_ProxyFn = m_original;
		}

		RecvVarProxyFn get_old_function( ) const {
			return m_original;
		}

	private:
		//overcomplicated...
		RecvProp* m_target;
		RecvVarProxyFn m_original;
	};
}