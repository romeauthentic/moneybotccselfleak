#include "c_base_player.hpp"
#include "interface.hpp"
#include "math.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

#undef max

//CCSGOPlayerAnimState* c_base_player::sm_player_animstate[ 65 ];
//
//void CCSGOPlayerAnimState::update( float yaw, float pitch ) {
//	static auto fn_ptr = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" ) );
//	uintptr_t alloc = ( uintptr_t )this;
//
//
//	//vectorcall is GAY
//	if( fn_ptr ) {
//		__asm {
//			push 0
//			mov ecx, alloc
//
//			movss xmm1, dword ptr[ yaw ]
//			movss xmm2, dword ptr[ pitch ]
//
//			call fn_ptr
//		}
//	}
//}
//
//void CCSGOPlayerAnimState::reset( ) {
//	static auto fn_ptr = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "56 6A 01 68 ? ? ? ? 8B F1" ) );
//	if( fn_ptr ) {
//		auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
//		fn( this );
//	}
//}
//
//uintptr_t c_base_player::get_player_resource( ) {
//	for( int i{ 1 }; i < g_gmod.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
//		auto ent = g_gmod.m_entlist( )->GetClientEntity< IClientEntity >( i );
//		if( !ent ) continue;
//		auto class_ = ent->GetClientClass( );
//		if( class_ ) {
//			if( class_->m_class_id == CCSPlayerResource ) {
//				return uintptr_t( ent );
//			}
//		}
//	}
//
//	return 0;
//}
//
////use is_zero( ) on the vector to figure out if it returns a valid value
//vec3_t c_base_player::get_hitbox_pos( int hitbox ) {
//	auto ce = this->ce( );
//
//	const auto model = ce->GetModel( );
//	if( !model ) return vec3_t{ };
//
//	auto hdr = g_gmod.m_model_info( )->GetStudiomodel( model );
//	if( !hdr ) return vec3_t{ };
//
//	matrix3x4 bone_matrix[ 128 ];
//	if( !ce->SetupBones( bone_matrix, 128, 0x100, g_gmod.m_globals->m_curtime ) ) {
//		return vec3_t{ };
//	}
//
//	auto set = hdr->pHitboxSet( m_nHitboxSet( ) );
//	if( !set ) return vec3_t{ };
//
//	auto box = set->pHitbox( hitbox );
//	if( !box ) return vec3_t{ };
//
//	vec3_t center = ( box->bbmin + box->bbmax ) * 0.5f;
//
//	return math::vector_transform( center, bone_matrix[ box->bone ] );
//}
//

c_base_player* c_base_player::get_observer_target( ) {
	return g_gmod.m_entlist( )->GetClientEntityFromHandle( m_hObserverTarget( ) );
}


const char* c_base_player::get_rank( ) {
	if( !g_ctx.m_lua )
		return "";

	const auto lua = g_ctx.m_lua;

	this->ce( )->PushEntity( );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_ENTITY ) ) {
		lua->Pop( 1 );
		return "";
	}

	lua->GetField( -1, "GetUserGroup" );
	if( !lua->IsType( -1, LUA_TYPE::TYPE_FUNCTION ) ) {
		lua->Pop( 2 );
		return "";
	}

	lua->Push( -2 ); // have to push the current entity.

	lua->Call( 1, 1 );
	if( !lua->IsType( -1, LUA_TYPE::_TYPE_STRING ) ) {
		lua->Pop( 2 );
		return "";
	}

	const char* rank = lua->GetString( -1 );

	lua->Pop( 2 );

	return rank;
}

clr_t c_base_player::get_team_color( ) {
	if( !g_ctx.m_lua )
		return clr_t{ };

	const auto lua = g_ctx.m_lua;

	clr_t ret{ };

	lua->GetField( LUA_GLOBALSINDEX, xors( "team" ) ); // team
	if( !lua->IsType( -1, LUA_TYPE::TYPE_TABLE ) ) {
		lua->Pop( 1 );
		return clr_t{ };
	}

	lua->GetField( -1, xors( "GetColor" ) ); // team, GetColor
	if( !lua->IsType( -1, LUA_TYPE::TYPE_FUNCTION ) ) {
		lua->Pop( 2 );
		return clr_t{ };
	}

	lua->PushNumber( ( double )m_iTeamNum( ) ); // team, GetColor, team_number
	lua->Call( 1, 1 ); // team, color_table
	if( !lua->IsType( -1, LUA_TYPE::TYPE_TABLE ) ) {
		lua->Pop( 2 );
		return clr_t{ };
	}

	lua->PushString( xors( "r" ) ); // team, color_table, "r"
	lua->GetTable( -2 ); // team, color_table, r_value
	if( !lua->IsType( -1, LUA_TYPE::TYPE_NUMBER ) ) {
		lua->Pop( 3 );
		return clr_t{ };
	}

	ret.r( ) = lua->GetNumber( -1 );
	lua->Pop( 1 );

	lua->PushString( xors( "g" ) ); // team, color_table, "g"
	lua->GetTable( -2 ); // team, color_table, g_value
	if( !lua->IsType( -1, LUA_TYPE::TYPE_NUMBER ) ) {
		lua->Pop( 3 );
		return clr_t{ };
	}

	ret.g( ) = lua->GetNumber( -1 );
	lua->Pop( 1 );

	lua->PushString( xors( "b" ) ); // team, color_table, "b"
	lua->GetTable( -2 ); // team, color_table, b_value
	if( !lua->IsType( -1, LUA_TYPE::TYPE_NUMBER ) ) {
		lua->Pop( 3 );
		return clr_t{ };
	}

	ret.b( ) = lua->GetNumber( -1 );
	lua->Pop( 3 ); // stack cleaned up.

	return ret;
}

c_base_weapon* c_base_player::get_weapon( ) {
	return g_gmod.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( m_hActiveWeapon( ) );
}
bool c_base_player::is_visible( const vec3_t& pos ) {
	vec3_t start = g_ctx.m_local->get_eye_pos( );
	return util::trace_ray( start, pos, g_ctx.m_local->ce( ), this->ce( ) );
}

//
//bool c_base_player::is_visible( int hitbox ) {
//	vec3_t start = g_ctx.m_local->get_eye_pos( );
//	vec3_t end = get_hitbox_pos( hitbox );
//
//	if( !end ) return false;
//	return util::trace_ray( start, end, g_ctx.m_local->ce( ), this->ce( ) );
//}
//
//float c_base_player::get_hitbox_radius( int hitbox ) {
//	auto model = ce( )->GetModel( );
//	auto hdr = g_gmod.m_model_info( )->GetStudiomodel( model );
//	auto set = hdr->pHitboxSet( m_nHitboxSet( ) );
//
//	if( !model || !hdr || !set )
//		return 0.f;
//
//	auto box = set->pHitbox( hitbox );
//
//	if( box ) {
//		auto dist = box->bbmin.dist_to( box->bbmax );
//		return dist + box->m_flRadius > 0 ? box->m_flRadius : 0.f;
//	}
//
//	return 0.f;
//}
//
//bool c_base_player::is_fakewalking( ) {
//	auto walk_layer = m_AnimOverlay( ).GetElements( )[ 6 ];
//	auto velocity = m_vecVelocity( ).length2d( );
//	bool on_ground = m_fFlags( ) & FL_ONGROUND;
//	if( walk_layer.m_flWeight < 0.03f && velocity > 0.1f && on_ground ) {
//		return true;
//	}
//
//	return false;
//}
//
//int c_base_player::get_choked_ticks( ) {
//	auto simtime = m_flSimulationTime( );
//
//	return TIME_TO_TICKS( g_gmod.m_globals->m_curtime - simtime ) + 1;
//}
//
//bool c_base_player::is_breaking_lc( ) {
//	auto choked = get_choked_ticks( );
//	auto velocity = m_vecVelocity( );
//	auto ipt = g_gmod.m_globals->m_interval_per_tick;
//
//	vec3_t travel_origin = m_vecOrigin( ) + velocity * ( ipt * choked );
//	return travel_origin.dist_to_sqr( m_vecOrigin( ) ) > 4096.f;
//}
//
//bool c_base_player::can_attack( bool ignore_rapid ) {
//	if( g_ctx.m_has_fired_this_frame ) return false;
//	if( !is_alive( ) ) return false;
//
//	auto weapon = get_weapon( );
//	if( !weapon ) return false;
//
//	float time = g_ctx.pred_time( );
//
//	float next_attack = m_flNextAttack( );
//	if( next_attack > time ) return false;
//
//	float next_primary_attack = weapon->m_flNextPrimaryAttack( );
//	if( next_primary_attack > time ) return false;
//
//	if( weapon->m_iItemDefinitionIndex( ) == WEAPON_R8REVOLVER ) {
//		float ready_time = weapon->m_flPostponeFireReadyTime( );
//		if( ready_time == FLT_MAX ) return false;
//
//		float time_to_shoot = ready_time - time;
//		return TIME_TO_TICKS( time_to_shoot ) < 1;
//	}
//
//	return true;
//}
//
//int c_base_player::get_seq_activity( int sequence ) {
//	static uintptr_t fn_offset = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" ) );
//	auto fn = ( int( __thiscall* )( void*, int ) )( fn_offset );
//
//	return fn ? fn( this, sequence ) : -2;
//}
//
//void c_base_player::set_abs_angles( vec3_t angles ) {
//	static uintptr_t fn_addr = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );
//	auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );
//
//	return fn( this, angles );
//}
//
//void c_base_player::set_abs_origin( vec3_t origin ) {
//	static uintptr_t fn_addr = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 51 53 56 57 8B F1" ) );
//	auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );
//
//	return fn( this, origin );
//}
//
//void c_base_player::create_animstate( CCSGOPlayerAnimState* state ) {
//	static auto fn_addr = pattern::first_code_match( g_gmod.m_chl.dll( ), xors( "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" ) );
//	auto fn = ( void( __thiscall* )( CCSGOPlayerAnimState*, decltype( this ) ) )( fn_addr );
//
//	fn( state, this );
//}
//
//void c_base_player::enforce_animations( ) {
//	int idx = this->ce( )->GetIndex( );
//	auto animstate = sm_player_animstate[ idx ];
//
//	if( animstate && animstate->pBaseEntity == this ) {
//		memcpy( get_animstate( ), animstate, sizeof( CCSGOPlayerAnimState ) );
//	}
//
//	//get< bool >( 0x38CD ) = false;
//	//get< bool >( 0x35F8 ) = false;
//	//m_bClientSideAnimation( ) = false;
//}
//
//void c_base_player::fix_animations( CCSGOPlayerAnimState* saved_state ) {
//	bool valid = false;
//	int idx = this->ce( )->GetIndex( );
//
//	C_AnimationLayer anim_backup[ 13 ]{ };
//	float pose_param_backup[ 24 ]{ };
//
//	memcpy( anim_backup,
//		m_AnimOverlay( ).GetElements( ),
//		sizeof( anim_backup ) );
//
//	memcpy( pose_param_backup,
//		m_flPoseParameter( ),
//		sizeof( pose_param_backup ) );
//
//	auto sequence_backup = m_nSequence( );
//	auto flags_backup	 = m_fFlags( );
//	auto cycle_backup	 = m_flCycle( );
//	auto eye_angles		 = m_angEyeAngles( );
//	auto render_angles	 = ce( )->GetRenderAngles( );
//	auto origin			 = m_vecOrigin( );
//
//	//these change but idk what they are
//	m_bClientSideAnimation( ) = true;
//	this->invalidate_bone_cache( );
//
//	auto backup_curtime = g_gmod.m_globals->m_curtime;
//	auto backup_frametime = g_gmod.m_globals->m_frametime;
//	auto backup_interp = g_gmod.m_globals->m_interpolation_amount;
//
//	g_gmod.m_globals->m_curtime = m_flSimulationTime( ) - util::get_lerptime( );
//	g_gmod.m_globals->m_frametime = TICK_INTERVAL( );
//
//	int framecount = g_gmod.m_globals->m_framecount;
//
//	auto csgo_animstate = get_animstate( );
//	if( csgo_animstate ) {
//		if( !saved_state ) {
//			if( !sm_player_animstate[ idx ] ) {
//				sm_player_animstate[ idx ] = ( CCSGOPlayerAnimState* )g_gmod.m_alloc->Alloc( sizeof( CCSGOPlayerAnimState ) );
//				this->create_animstate( sm_player_animstate[ idx ] );
//			}
//			else if( sm_player_animstate[ idx ]->pBaseEntity != this ) {
//				sm_player_animstate[ idx ]->reset( );
//				this->create_animstate( sm_player_animstate[ idx ] );
//				sm_player_animstate[ idx ]->pBaseEntity = this;
//			}
//			else {
//				valid = true;
//				sm_player_animstate[ idx ]->m_iLastClientSideAnimationUpdateFramecount = 0;
//				sm_player_animstate[ idx ]->m_flLastClientSideAnimationUpdateTime = 0.f;
//				sm_player_animstate[ idx ]->update( eye_angles.y, eye_angles.x );
//			}
//		}
//	}
//	else {
//		g_con->log( xors( "couldnt update animstate for player %d!" ), ce( )->GetIndex( ) );
//		return;
//	}
//
//	//call updateclientsideanimation
//	util::get_vfunc< 218, void >( this );
//
//	if( saved_state )
//		memcpy( csgo_animstate, saved_state, sizeof( CCSGOPlayerAnimState ) );
//	else if( valid )
//		memcpy( csgo_animstate, sm_player_animstate[ idx ], sizeof( CCSGOPlayerAnimState ) );
//
//	this->ce( )->GetRenderAngles( ).y = std::remainderf( ce( )->GetRenderAngles( ).y, 360.f );
//	this->ce( )->GetRenderOrigin( ) = m_vecOrigin( );
//	set_abs_angles( this->ce( )->GetRenderAngles( ) );
//	set_abs_origin( m_vecOrigin( ) );
//	m_fFlags( ) = flags_backup;
//	m_flCycle( ) = cycle_backup;
//	m_nSequence( ) = sequence_backup;
//
//	//*( int* )( uintptr_t( this ) + 0xa30 ) = 0;
//	//m_nWriteableBones( ) = 0;
//
//	memcpy( m_AnimOverlay( ).GetElements( ),
//		anim_backup,
//		sizeof( anim_backup ) );
//
//	memcpy( m_flPoseParameter( ),
//		pose_param_backup,
//		sizeof( pose_param_backup ) );
//
//	m_flLastBoneSetupTime( ) = 0.f;
//	m_iMostRecentModelBoneCounter( ) = 0;
//
//	ce( )->SetupBones( nullptr, -1, 0x100, m_flSimulationTime( ) );
//
//	m_flLastBoneSetupTime( ) = std::numeric_limits< float >::quiet_NaN( );
//	m_iMostRecentModelBoneCounter( ) = 0;
//
//	g_gmod.m_globals->m_curtime = backup_curtime;
//	g_gmod.m_globals->m_frametime = backup_frametime;
//
//	m_bClientSideAnimation( ) = false;
//}
//
player_info_t c_base_player::get_info( ) {
	player_info_t info;
	g_gmod.m_engine( )->GetPlayerInfo( ce( )->GetIndex( ), &info );

	return info;
}

void c_base_player::get_name_safe( char* buf ) {
	player_info_t info;
	if( g_gmod.m_engine( )->GetPlayerInfo( this->ce( )->GetIndex( ), &info ) ) {
		for( size_t i{ }; i < 32; ++i ) {
			switch( info.name[ i ] ) {
			case '"':
			case '\\':
			case ';':
			case '\n':
				buf[ i ] = ' ';
				break;
			default:
				buf[ i ] = info.name[ i ];
				break;
			}
		}

		buf[ 31 ] = 0;
	}
}