#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

void __cdecl hooks::lby_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	lby_proxy.get_old_function( )( proxy_data_const, entity, output );

	if( !g_csgo.m_panic ) {
		auto player = ( c_base_player* )( entity );
		if( player && player == g_ctx.m_local ) {
			g_cheat.m_ragebot.m_antiaim->on_lby_proxy( );
		}
	}
}

void __cdecl hooks::last_shot_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	if( !proxy_data_const->m_Value.m_Int ) {
		return;
	}

	last_shot_proxy.get_old_function( )( proxy_data_const, entity, output );
	if( !g_csgo.m_panic && proxy_data_const && g_settings.rage.enabled( ) && g_settings.rage.resolver( ) ) {
		auto wep = ( c_base_weapon* )( entity );
		if( wep && !wep->is_knife( ) && !wep->is_grenade( ) ) {
			auto owner = g_csgo.m_entlist( )->GetClientEntityFromHandle( wep->m_hOwner( ) );
			if( owner && owner->is_valid( ) && owner != g_ctx.m_local && g_ctx.m_local->is_valid( ) && owner->has_valid_anim( ) ) {
				if( owner->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
					return;

				static float last_time = 0.f;
				float time = wep->m_fLastShotTime( );

				if( !time )
					return;

				float last_anim = owner->m_flOldSimulationTime( );
				float anim_time = owner->m_flSimulationTime( );
				auto record = g_cheat.m_ragebot.m_lagcomp->get_newest_record( owner->ce( )->GetIndex( ) );
				float& last_update = g_cheat.m_ragebot.m_lagcomp->get_last_updated_simtime( owner->ce( )->GetIndex( ) );

				if( time > last_anim && time <= anim_time && owner->m_flSimulationTime( ) != last_update ) {
					last_update = owner->m_flSimulationTime( );
					owner->fix_animations( );

					features::c_ragebot::lag_record_t record( owner );
					record.m_shot = true;
					
					g_cheat.m_ragebot.m_lagcomp->store_record( owner->ce( )->GetIndex( ), RECORD_LBY, record );
					last_time = time;
				}
			}
		}
	}
}

void __cdecl hooks::simtime_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	auto old_fn = simtime_proxy.get_old_function( );

	auto ent = ( c_base_player* )( entity );
	if( ent && ent->is_valid( ) && ent->has_valid_anim( ) && ( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || g_settings.rage.friendlies( ) ) && ent != g_ctx.m_local ) {
		if( !proxy_data_const->m_Value.m_Int ) {
			return;
		}
	}

	old_fn( proxy_data_const, entity, output );
}

#define SEQUENCE_DEFAULT_DRAW						0
#define SEQUENCE_DEFAULT_IDLE1						1
#define SEQUENCE_DEFAULT_IDLE2						2
#define SEQUENCE_DEFAULT_LIGHT_MISS1				3
#define SEQUENCE_DEFAULT_LIGHT_MISS2				4
#define SEQUENCE_DEFAULT_HEAVY_MISS1				9
#define SEQUENCE_DEFAULT_HEAVY_HIT1					10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB				11
#define SEQUENCE_DEFAULT_LOOKAT01					12

#define SEQUENCE_BUTTERFLY_DRAW						0
#define SEQUENCE_BUTTERFLY_DRAW2					1
#define SEQUENCE_BUTTERFLY_LOOKAT01					13
#define SEQUENCE_BUTTERFLY_LOOKAT03					15

#define SEQUENCE_FALCHION_IDLE1						1
#define SEQUENCE_FALCHION_HEAVY_MISS1				8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP		9
#define SEQUENCE_FALCHION_LOOKAT01					12
#define SEQUENCE_FALCHION_LOOKAT02					13

#define SEQUENCE_DAGGERS_IDLE1						1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 				2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 				6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 				11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 				12

#define SEQUENCE_BOWIE_IDLE1 						1

void __cdecl hooks::set_viewmodel_sequence_proxy_fn( const CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	auto ent = ( c_base_weapon* )entity;

  if( ent ) {
    auto owner = ent->m_hOwnerViewModel( ) & 0xfff;
    if( owner && owner == g_ctx.m_local->ce( )->GetIndex( ) ) {


      const auto data = const_cast< CRecvProxyData* >( proxy_data_const );
      int sequence = data->m_Value.m_Int;

      const std::string model_name = g_csgo.m_model_info( )->GetModelName( g_csgo.m_model_info( )->GetModel( ent->m_nModelIndex( ) ) );

      if( model_name == xors( "models/weapons/v_knife_butterfly.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03 );
          break;
        default:
          sequence++; break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_falchion_advanced.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_IDLE2:
          sequence = SEQUENCE_FALCHION_IDLE1; break;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
          sequence = util::get_random_int( SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02 );
          break;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
          break;
        default:
          sequence--; break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_survival_bowie.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
          break;
        case SEQUENCE_DEFAULT_IDLE2:
          sequence = SEQUENCE_DEFAULT_IDLE1;
          break;
        default:
          sequence--; break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_push.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_IDLE2:
          sequence = 1;
          break;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
          sequence = util::get_random_int( SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5 );
          break;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
          sequence = util::get_random_int( SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1 );
          break;
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence += 3;
          break;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
          break;
        default:
          sequence += 2; break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_survival_bowie.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
          break;
        case SEQUENCE_DEFAULT_IDLE2:
          sequence = SEQUENCE_BOWIE_IDLE1;
          break;
        default:
          sequence--;
        }
      } else if( model_name == xors( "models/weapons/v_knife_ursus.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
          break;
        default:
          sequence++;
        }
      } else if( model_name == xors( "models/weapons/v_knife_stiletto.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( 12, 13 );
          break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_widowmaker.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( 14, 15 );
          break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_css.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_IDLE2:
          sequence = 1;
          break;
        }
      } else if( model_name == xors( "models/weapons/v_knife_cord.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
          break;
        default:
          sequence++;
        }
      } else if( model_name == xors( "models/weapons/v_knife_canis.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
          break;
        default:
          sequence++;
        }
      } else if( model_name == xors( "models/weapons/v_knife_outdoor.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
          break;
        default:
          sequence++;
        }
      } else if( model_name == xors( "models/weapons/v_knife_skeleton.mdl" ) ) {
        switch( sequence ) {
        case SEQUENCE_DEFAULT_DRAW:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
          break;
        case SEQUENCE_DEFAULT_LOOKAT01:
          sequence = util::get_random_int( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
          break;
        default:
          sequence++;
        }
      }

      data->m_Value.m_Int = sequence;
    }
  }

	set_viewmodel_sequence_proxy.get_old_function( )( proxy_data_const, entity, output );
}
