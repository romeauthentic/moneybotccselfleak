#include "skins.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "interface.hpp"

namespace features
{
	int c_skins::get_knife_index( ) {
		switch( g_settings.misc.skins.knife( ) ) {
		case 0:
			return 0;
		case 1:
			return 500;
		case 2:
			return 505;
		case 3:
			return 506;
		case 4:
			return 507;
		case 5:
			return 508;
		case 6:
			return 509;
		}

		return 0;
	}

	const char* c_skins::get_model_str( ) {
		switch( g_settings.misc.skins.knife( ) ) {
		case 0:
			return 0;
		case 1:
			return xors( "models/weapons/v_knife_bayonet.mdl" );
		case 2:
			return xors( "models/weapons/v_knife_flip.mdl" );
		case 3:
			return xors( "models/weapons/v_knife_gut.mdl" );
		case 4:
			return xors( "models/weapons/v_knife_karam.mdl" );
		case 5:
			return xors( "models/weapons/v_knife_m9_bay.mdl" );
		case 6:
			return xors( "models/weapons/v_knife_tactical.mdl" );
		}

		return nullptr;
	}

	void c_skins::override_knife( ) {
		auto weapons = ( uint32_t* )( uintptr_t( g_ctx.m_local ) + 0x2DE8 );
		for( int i{ }; weapons[ i ]; ++i ) {
			auto wep = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( weapons[ i ] );
			auto vm = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( g_ctx.m_local->m_hViewModel( ) );
			if( wep && wep->is_knife( ) ) {
				int index = get_knife_index( );
				if( index ) {
					int model = g_csgo.m_model_info( )->GetModelIndex( get_model_str( ) );

					wep->m_iItemDefinitionIndex( ) = index;
					wep->m_nModelIndex( ) = model;
					if( weapons[ i ] == g_ctx.m_local->m_hActiveWeapon( ) ) {
						vm->m_nModelIndex( ) = model;
					}
				}
			}
		}
	}

	void c_skins::operator()( ) {
		if( g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
			override_knife( );
		}
	}
}