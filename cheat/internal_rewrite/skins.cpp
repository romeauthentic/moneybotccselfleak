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
			return WEAPON_KNIFE_BAYONET;
		case 2:
			return WEAPON_KNIFE_FLIP;
		case 3:
			return WEAPON_KNIFE_GUT;
		case 4:
			return WEAPON_KNIFE_KARAMBIT;
		case 5:
			return WEAPON_KNIFE_M9_BAYONET;
		case 6:
			return WEAPON_KNIFE_TACTICAL;
		case 7:
			return WEAPON_KNIFE_BUTTERFLY;
		case 8:
			return WEAPON_KNIFE_FALCHION;
		case 9:
			return WEAPON_KNIFE_SURVIVAL_BOWIE;
		case 10:
			return WEAPON_KNIFE_PUSH;
		case 11:
			return WEAPON_KNIFE_URSUS;
		case 12:
			return WEAPON_KNIFE_STILETTO;
		case 13:
			return WEAPON_KNIFE_WIDOWMAKER;
		case 14:
			return WEAPON_KNIFE_GYPSY_JACKKNIFE;
        case 15:
			return WEAPON_KNIFE_CSS;
        case 16:
			return WEAPON_KNIFE_PARACORD;
        case 17:
			return WEAPON_KNIFE_SURVIVAL;
        case 18:
			return WEAPON_KNIFE_NOMAD;
        case 19:
			return WEAPON_KNIFE_SKELETON;
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
		case 7:
			return xors( "models/weapons/v_knife_butterfly.mdl" );
		case 8:
			return xors( "models/weapons/v_knife_falchion_advanced.mdl" );
		case 9:
			return xors( "models/weapons/v_knife_survival_bowie.mdl" );
		case 10:
			return xors( "models/weapons/v_knife_push.mdl" );
		case 11:
			return xors( "models/weapons/v_knife_ursus.mdl" );
		case 12:
			return xors( "models/weapons/v_knife_stiletto.mdl" );
		case 13:
			return xors( "models/weapons/v_knife_widowmaker.mdl" );
		case 14:
			return xors( "models/weapons/v_knife_gypsy_jackknife.mdl" );
        case 15:
			return xors( "models/weapons/v_knife_css.mdl" );
        case 16:
			return xors( "models/weapons/v_knife_cord.mdl" );
        case 17:
			return xors( "models/weapons/v_knife_canis.mdl" );
        case 18:
			return xors( "models/weapons/v_knife_outdoor.mdl" );
        case 19:
			return xors( "models/weapons/v_knife_skeleton.mdl" );
		}

		return nullptr;
	}

	const char* c_skins::get_glove_model( ) {
		switch( g_settings.misc.skins.glove ) {
		case 5027:
			return xors( "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" );
		case 5030:
			return xors( "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" );
		case 5031:
			return xors( "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" );
		case 5032:
			return xors( "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" );
		case 5033:
			return xors( "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" );
		case 5034:
			return xors( "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" );
		}

		return nullptr;
	}

	const char* c_skins::get_killicon_str( ) {
		switch( g_settings.misc.skins.knife( ) ) {
		case 0:
			return 0;
		case 1:
			return xors( "bayonet" );
		case 2:
			return xors( "knife_flip" );
		case 3:
			return xors( "knife_gut" );
		case 4:
			return xors( "knife_karambit" );
		case 5:
			return xors( "knife_m9_bayonet" );
		case 6:
			return xors( "knife_tactical" );
		case 7:
			return xors( "knife_butterfly" ); // i have no idea if these are the correct filenames lol
		case 8:
			return xors( "knife_falchion" );
		case 9:
			return xors( "knife_bowie" );
		case 10:
			return xors( "knife_push" );
		case 11:
			return xors( "knife_ursus" );
		case 12:
			return xors( "knife_stiletto" );
		case 13:
			return xors( "knife_widowmaker" );
		case 14:
			return xors( "knife_ursus" );
        case 15:
			return xors( "knife_css" );
        case 16:
			return xors( "knife_cord" );
        case 17:
			return xors( "knife_canis" );
        case 18:
			return xors( "knife_outdoor" );
        case 19:
			return xors( "knife_skeleton" );
		}

		return nullptr;
	}

	int c_skins::get_current_weapon_id( ) {
		if( !g_ctx.m_local || !g_ctx.m_local->is_valid( ) )
			return WEAPON_KNIFE;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon )
			return WEAPON_KNIFE;

		return get_weapon_id( weapon );
	}

	int c_skins::get_weapon_id( c_base_weapon* weapon ) {
		if( weapon->is_knife( ) || weapon->is_grenade( ) )
			return WEAPON_KNIFE;

		if( weapon->m_iItemDefinitionIndex( ) == WEAPON_ZEUS )
			return WEAPON_KNIFE;

		return std::clamp< int >( weapon->m_iItemDefinitionIndex( ), 0, 64 );
	}

	// This is nasty.
	void c_skins::replace_deaths( IGameEvent* event ) {
		if( !strcmp( event->GetName( ), xors( "player_death" ) ) && g_csgo.m_engine( )->GetPlayerForUserID( event->GetInt( xors( "attacker" ) ) ) == g_csgo.m_engine( )->GetLocalPlayer( ) ) {
			if( ( !strcmp( event->GetString( xors( "weapon" ) ), xors( "knife_default_ct" ) ) || !strcmp( event->GetString( xors( "weapon" ) ), xors( "knife_t" ) ) ) ) {
				if( get_killicon_str( ) )
					event->SetString( xors( "weapon" ), get_killicon_str( ) );
			}
		}
	}

	//stop this
	void c_skins::override_weapon( c_base_weapon* wep ) {
		auto aw = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( g_ctx.m_local->m_hActiveWeapon( ) );
		auto vm = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( g_ctx.m_local->m_hViewModel( ) );
		auto wm = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( g_ctx.m_local->get_weapon( )->m_hWeaponWorldModel( ) );

		auto info = g_ctx.m_local->get_info( );
		int index = get_knife_index( );

		if( wep->is_knife( ) && index ) {
			int model = g_csgo.m_model_info( )->GetModelIndex( get_model_str( ) );

			wep->m_iItemDefinitionIndex( ) = index;
			wep->m_nModelIndex( ) = model;
			wep->m_iWorldDroppedModelIndex( ) = model + 2;
			wep->m_iWorldModelIndex( ) = model + 1;
			wep->m_iViewModelIndex( ) = model;
			wep->m_iItemIDHigh( ) = -1;
			wep->m_iEntityQuality( ) = 3;

			if( aw->m_iItemDefinitionIndex( ) == wep->m_iItemDefinitionIndex( ) ) {
				vm->m_nModelIndex( ) = model;
			}

			if( wm && wep == aw ) {
				wm->m_nModelIndex( ) = model + 1;
			}
		}

		if( m_last_index != g_settings.misc.skins.current_skin ) {
			g_settings.misc.skins.skins( )[ get_current_weapon_id( ) ] = { g_settings.misc.skins.current_skin, g_settings.misc.skins.current_seed, g_settings.misc.skins.current_stattrak };

			if( g_csgo.m_client_state->m_delta_tick != -1 ) {
				g_csgo.m_client_state->m_delta_tick = -1; // fullupdate happens
			}
			
			m_last_index = g_settings.misc.skins.current_skin;
			g_ctx.run_frame( );
		}

		if( ( wep->m_hOwner( ) & 0xfff ) == g_ctx.m_local->ce( )->GetIndex( ) ) {
			auto wep_id = get_weapon_id( wep );
			if( g_settings.misc.skins.skins( )[ wep_id ].skin <= 0 )
				return;

			if( g_settings.misc.skins.skins( )[ wep_id ].skin != wep->m_nFallbackPaintKit( ) ) {
				wep->m_iItemIDHigh( ) = -1;
				wep->m_iAccountID( ) = info.m_xuidlow;
				wep->m_nFallbackPaintKit( ) = g_settings.misc.skins.skins( )[ wep_id ].skin;
				wep->m_nFallbackSeed( ) = g_settings.misc.skins.skins( )[ wep_id ].seed;
				wep->m_flFallbackWear( ) = 0.0000001f;
				if( g_settings.misc.skins.skins( )[ wep_id ].stattrak ) {
					wep->m_nFallbackStatTrak( ) = ( int )util::get_random_float( 0, 10000 );
					wep->m_iEntityQuality( ) = 9;
				}
				else {
					wep->m_nFallbackStatTrak() = -1;
				}
			}
		}
	}

	void c_skins::override_gloves( ) {
		static int last_glove = 0;
		static int last_skin = 0;
		static ulong_t glove_handle = 0;
		auto wearables =  g_ctx.m_local->m_hMyWearables( );

		auto glove = ( c_base_weapon* )( g_csgo.m_entlist( )->GetClientEntityFromHandle( wearables[ 0 ] ) );

		if( !glove ) {
			auto last_glove = ( c_base_weapon* )( g_csgo.m_entlist( )->GetClientEntityFromHandle( glove_handle ) );

			if( last_glove ) {	
				wearables[ 0 ] = glove_handle;
				glove = last_glove;
			}
		}

		if( !g_ctx.m_local->is_alive( ) || !g_settings.misc.skins.glove ) {
			if( glove ) {
				glove->ce( )->GetClientNetworkable( )->SetDestroyedOnRecreateEntities( );
				glove->ce( )->GetClientNetworkable( )->Release( );
			}

			return;
		}

		if( !g_settings.misc.skins.glove || !g_settings.misc.skins.glove_skin )
			return;

		bool update = false;
		if( last_glove != g_settings.misc.skins.glove || last_skin != g_settings.misc.skins.glove_skin ) {
			g_csgo.m_client_state->m_delta_tick = -1;
			update = true;
		}

		last_glove = g_settings.misc.skins.glove;
		last_skin = g_settings.misc.skins.glove_skin;

		if( !glove ) {
			for( auto cc = g_csgo.m_chl( )->GetAllClasses( ); !!cc; cc = cc->m_next ) {
				if( cc->m_class_id != CEconWearable )
					continue;

				int entry = g_csgo.m_entlist( )->GetHighestEntityIndex( ) + 1;
				int serial = math::random_number( 0, 4095 );

				cc->m_create_fn( entry, serial );

				glove = g_csgo.m_entlist( )->GetClientEntity< c_base_weapon >( entry );
				wearables[ 0 ] = entry | serial << 16;
				glove_handle = wearables[ 0 ];
				update = true;

				break;
			}
		}

		if( glove ) {
			auto desired_glove = g_settings.misc.skins.glove;
			if( !desired_glove )
				return;

			glove->m_iItemDefinitionIndex( ) = desired_glove;
			glove->m_nFallbackPaintKit( ) = g_settings.misc.skins.glove_skin;
			glove->m_iEntityQuality( ) = 4;
			glove->m_nFallbackSeed( ) = math::random_number( 0, 600 );
			glove->m_iItemIDHigh( ) = -1;
			glove->m_iAccountID( ) = g_ctx.m_local->get_info( ).m_xuidlow;

			if( update ) {
				int model_index = g_csgo.m_model_info( )->GetModelIndex( get_glove_model( ) );
				glove->set_glove_model( desired_glove );
				glove->ce( )->GetClientNetworkable( )->PreDataUpdate( 0 );
			}
		}
	}


	void c_skins::operator()( ) {
		if( g_csgo.m_engine( )->IsInGame( ) && g_ctx.run_frame( ) && g_ctx.m_local->is_player( ) ) {
			override_gloves( );

			if( g_ctx.m_local->m_iHealth( ) > 0 && g_ctx.m_local->get_weapon( ) ) {
				auto weapons = ( uint32_t* )( uintptr_t( g_ctx.m_local ) + 0x2DF8 );
				for( int i{ }; weapons[ i ]; ++i ) {
					auto wep = g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( weapons[ i ] );
					if( wep && wep->m_OriginalOwnerXuidLow( ) == g_ctx.m_local->get_info( ).m_xuidlow && !wep->is_grenade( ) && wep->m_iItemDefinitionIndex( ) != WEAPON_ZEUS ) {
						override_weapon( wep );
					}
				}
			}
		}
	}
}