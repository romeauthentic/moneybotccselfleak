#include "c_base_player.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "interface.hpp"
#include "math.hpp"
#include "mem.hpp"

#undef max

ent_animdata_t c_base_player::sm_animdata[ 65 ];

void IKContext::Init(
    void* hdr, vec3_t& angles, vec3_t& origin, float curtime, int framecount, int bone_mask ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D 8F" ) );

    auto fn = ( void( __thiscall* )( void*, void*, vec3_t&, vec3_t&, float, int, int ) )( fn_ptr );
    fn( this, hdr, angles, origin, curtime, framecount, bone_mask );
}

void IKContext::UpdateTargets( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F0 81 EC 00 00 00 00 33 D2" ) );

    auto fn = ( void( __thiscall* )( void*, vec3_t*, void*, matrix3x4*, char* ) )( fn_ptr );
    fn( this, pos, q, bone_array, computed );
}

void IKContext::SolveDependencies( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F0 81 EC 00 00 00 00 8B 81" ) );

    auto fn = ( void( __thiscall* )( void*, vec3_t*, void*, matrix3x4*, char* ) )( fn_ptr );
    fn( this, pos, q, bone_array, computed );
}

bool& c_base_player::s_bInvalidateBoneCache( ) {
#ifdef HEADER_MODULE
    static auto fn_ptr = g_header.patterns.animstate_update;
#else
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" ) );
#endif
    static auto var = *( bool** )( fn_ptr + 0xda + 2 );

    return *var;
}

void CCSGOPlayerAnimState::update( float yaw, float pitch ) {
#ifdef HEADER_MODULE
    static auto fn_ptr = g_header.patterns.animstate_update;
#else
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" ) );
#endif

    uintptr_t alloc = ( uintptr_t ) this;
    if( !alloc )
        return;

    if( !this->pBaseEntity )
        return;

    // bool* b = &c_base_player::s_bInvalidateBoneCache( );
    // char backup = *b;

    // vectorcall is GAY
    if( fn_ptr ) {
        __asm {
			push 0
			mov ecx, alloc

			movss xmm1, dword ptr[ yaw ]
			movss xmm2, dword ptr[ pitch ]

			call fn_ptr
        }
    }

    // c_base_player::s_bInvalidateBoneCache( ) = backup;
}

float CCSGOPlayerAnimState::desync_delta( ) {
    auto state = this;

    float duck_amt = state->m_fDuckAmount;
    float speed_fraction = std::clamp( state->m_flFeetSpeedForwardsOrSideWays, 0.f, 1.f );
    float speed_factor = std::clamp( state->m_flFeetSpeedUnknownForwardOrSideways, 0.f, 1.f );

    float unk =
        ( ( state->m_flStopToFullRunningFraction * -0.30000001f ) - 0.19999999f ) * speed_fraction;
    float unk2 = unk + 1.f;
    if( duck_amt > 0.f )
        unk2 += ( ( duck_amt * speed_factor ) * ( 0.5f - unk2 ) );

    float max_delta = *( float* )( ( uintptr_t )state + 0x334 ) * unk2;

    return max_delta;
}

void CCSGOPlayerAnimState::setup_velocity( float a2 ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D" ) );
    if( fn_ptr ) {
        auto ptr_ = ( uintptr_t ) this;

        __asm {
			mov ecx, ptr_

			fld a2
			call fn_ptr
        }
    }
}

bool CCSGOPlayerAnimState::cache_sequences( ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 34 53 56 8B F1 57 8B 46 60" ) );
    if( fn_ptr ) {
        auto fn = ( bool( __thiscall* )( void* ) )( fn_ptr );
        return fn( this );
    }

    return false;
}

void CCSGOPlayerAnimState::update_ex( float yaw, float pitch ) {
    /*	static auto unk_func_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "8B 0D ? ? ?
     ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 75 14 8B 0D ? ? ? ? 8B 01" ) );
     auto unk_fn = ( bool( *)( ) )( unk_func_ptr );

     auto player = ( c_base_player* )this->pBaseEntity;

     if( player && cache_sequences( ) ) {
      float v7 = *( float* )( uintptr_t( player ) + 0x2e0d ) + pitch;
      v7 = std::remainderf( v7, 360.f );

      float last_update = m_flLastClientSideAnimationUpdateTime;
      float cur_time = g_csgo.m_globals->m_curtime;

      if( last_update != cur_time ) {
       this->m_flLastUpdateDelta = math::max( cur_time - last_update, 0.f );
       this->m_flEyeYaw = yaw;
       this->m_flPitch = pitch;

       this->m_vOrigin = player->m_vecOrigin( );
       this->pActiveWeapon = player->get_weapon( );
       if( pActiveWeapon != pLastActiveWeapon ) {
        for( size_t i{ }; i < 13; ++i ) {
         C_AnimationLayer* layer = &player->m_AnimOverlay( ).GetElements( )[ i ];

         layer->m_unk1 = 0;
         layer->m_unk2 = layer->m_unk3 = -1;
        }
       }

       bool using_duck_offset = !unk_fn( );

       if( using_duck_offset ) {
        float total_duck = *( float* )( uintptr_t( player ) + 0xbe7 ) +
     m_fLandingDuckAdditiveSomething;

        total_duck = std::clamp( total_duck, 0.f, 1.f );

        float duck_amt = m_fDuckAmount;

        float v28 = this->m_flLastUpdateDelta * 6.0f;
        float v29 = 0.f;

        if( total_duck - duck_amt > v28 ) {
         if( v28 <= total_duck - duck_amt )
          v29 = total_duck;
         else
          v29 = duck_amt - v28;
        }
        else {
         v29 = duck_amt + v28;
        }

        m_fDuckAmount = std::clamp( v29, 0.f, 1.f );
       }
       else {
        float duck_amt = m_fDuckAmount;

        float v19 = player->m_fFlags( ) & FL_WATERJUMP ? 1.0 : m_fLandingDuckAdditiveSomething;
        float v21 = 0.f;
        if( v19 <= duck_amt )
         v21 = 6.0f;
        else
         v21 = 3.1f;

        float v22 = m_flLastUpdateDelta * v21;
        float new_duck = 0.f;
        if( v19 - duck_amt <= v22 ) {
         if( v22 <= v19 - duck_amt )
          new_duck = v19;
         else
          new_duck = duck_amt - v22;
        }
        else {
         new_duck = duck_amt = v22;
        }

        m_fDuckAmount = std::clamp( new_duck, 0.f, 1.f );
       }

       *( uintptr_t* )( uintptr_t( player ) + 0x286 ) = 0;
       player->invalidate_physics_recursive( 8 );
       setup_velocity( v7 );
       setup_aim_matrix( );
       setup_weapon_action( );
       setup_movement( );
       setup_alive_loop( );

      }
     }*/
}

void CCSGOPlayerAnimState::reset( ) {
    static auto fn_ptr =
        pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "56 6A 01 68 ? ? ? ? 8B F1" ) );
    if( fn_ptr ) {
        auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
        fn( this );
    }
}

void CCSGOPlayerAnimState::setup_aim_matrix( ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D" ) );
    if( fn_ptr ) {
        auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
        fn( this );
    }
}

void c_base_player::invalidate_physics_recursive( int flags ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3" ) );
    if( fn_ptr ) {
        auto fn = ( void( __thiscall* )( void*, int ) )( fn_ptr );
        fn( this, flags );
    }
}

void c_base_player::update_clientside_animation( ) {
    bool backup = m_bClientSideAnimation( );
    m_bClientSideAnimation( ) = true;

    using fn = void( __fastcall* )( void* );
    pattern::first_code_match< fn >( g_csgo.m_chl.dll( ),
                                     xors( "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36" ) )( this );

    m_bClientSideAnimation( ) = backup;
}

uintptr_t c_base_player::get_player_resource( ) {
    for( int i{1}; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
        auto ent = g_csgo.m_entlist( )->GetClientEntity< IClientEntity >( i );
        if( !ent )
            continue;
        auto class_ = ent->GetClientClass( );
        if( class_ ) {
            if( class_->m_class_id == CCSPlayerResource ) {
                return uintptr_t( ent );
            }
        }
    }

    return 0;
}

uintptr_t c_base_player::get_game_rules( ) {
    static uintptr_t** game_rules = pattern::first_code_match< uintptr_t** >(
        g_csgo.m_chl.dll( ),
        xors( "83 3D ? ? ? ? 0 74 ? A1 ? ? ? ? B9 ? ? ? ? FF 50 ? 85 C0 75 ? 8B 0D ? ? ? ?" ),
        0x2 );
    if( !game_rules ) {
        game_rules = pattern::first_code_match< uintptr_t** >(
            g_csgo.m_chl.dll( ),
            xors( "83 3D ? ? ? ? 0 74 ? A1 ? ? ? ? B9 ? ? ? ? FF 50 ? 85 C0 75 ? 8B 0D ? ? ? ?" ),
			0x2 );
    }

    return **game_rules;
}

int& c_base_player::g_iModelBoneCounter( ) {
    static auto offset =
        pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "A1 ? ? ? ? 48 C7 81" ), 0x1 );
    return **( int** )( offset );
}

// use is_zero( ) on the vector to figure out if it returns a valid value
vec3_t c_base_player::get_hitbox_pos( int hitbox ) {
    auto ce = this->ce( );

    const auto model = ce->GetModel( );
    if( !model )
        return vec3_t{};

    auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
    if( !hdr )
        return vec3_t{};

    matrix3x4 bone_matrix[ 128 ];
    memcpy( bone_matrix,
            m_CachedBoneData( ).GetElements( ),
            m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

    auto set = hdr->pHitboxSet( m_nHitboxSet( ) );
    if( !set )
        return vec3_t{};

    auto box = set->pHitbox( hitbox );
    if( !box )
        return vec3_t{};

    vec3_t center = ( box->bbmin + box->bbmax ) * 0.5f;

    return math::vector_transform( center, bone_matrix[ box->bone ] );
}

c_base_weapon* c_base_player::get_weapon( ) {
    auto weap_handle = m_hActiveWeapon( );
    if( weap_handle )
        return g_csgo.m_entlist( )->GetClientEntityFromHandle< c_base_weapon >( weap_handle );

    return nullptr;
}

void c_base_player::set_needs_interpolate( bool interpolate ) {
    auto varmap = get< VarMapping_t >( 0x24 );
    auto elements = varmap.m_Entries.GetElements( );

    for( size_t i{}; i < varmap.m_nInterpolatedEntries; ++i ) {
        auto& e = elements[ i ];
        e.m_bNeedsToInterpolate = interpolate;
    }
}

bool c_base_player::is_visible( int hitbox ) {
    vec3_t start = g_ctx.m_local->get_eye_pos( );
    vec3_t end = get_hitbox_pos( hitbox );

    if( !end )
        return false;
    return util::trace_ray( start, end, g_ctx.m_local->ce( ), this->ce( ) );
}

float c_base_player::get_hitbox_radius( int hitbox ) {
    auto model = ce( )->GetModel( );
    auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
    auto set = hdr->pHitboxSet( m_nHitboxSet( ) );

    if( !model || !hdr || !set )
        return 0.f;

    auto box = set->pHitbox( hitbox );

    if( box ) {
        if( box->m_flRadius == -1 )
            return box->bbmin.dist_to( box->bbmax );
        return box->m_flRadius;
    }

    return 0.f;
}

vec3_t c_base_player::get_hitbox_mins( int hitbox ) {
    auto model = ce( )->GetModel( );
    auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
    auto set = hdr->pHitboxSet( m_nHitboxSet( ) );

    if( !model || !hdr || !set )
        return vec3_t( );

    matrix3x4 bone_matrix[ 128 ];
    memcpy( bone_matrix,
            m_CachedBoneData( ).GetElements( ),
            m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

    auto box = set->pHitbox( hitbox );

    return math::vector_transform( box->bbmin, bone_matrix[ box->bone ] );
}

vec3_t c_base_player::get_hitbox_maxs( int hitbox ) {
    auto model = ce( )->GetModel( );
    auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
    auto set = hdr->pHitboxSet( m_nHitboxSet( ) );

    if( !model || !hdr || !set )
        return vec3_t( );

    matrix3x4 bone_matrix[ 128 ];
    memcpy( bone_matrix,
            m_CachedBoneData( ).GetElements( ),
            m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

    auto box = set->pHitbox( hitbox );

    vec3_t max = math::vector_transform( box->bbmax, bone_matrix[ box->bone ] );
    return max;
}

bool c_base_player::is_fakewalking( ) {
    auto walk_layer = m_AnimOverlay( ).GetElements( )[ 6 ];
    auto some_layer = m_AnimOverlay( ).GetElements( )[ 4 ];

    auto velocity = m_vecVelocity( ).length2d( );
    bool on_ground = m_fFlags( ) & FL_ONGROUND;
    if( walk_layer.m_flWeight < 0.1f && some_layer.m_flWeight < 0.1f && velocity > 0.1f &&
        on_ground ) {
        return true;
    }

    if( velocity <= 0.1f )
        return true;

    return false;
}

int c_base_player::get_choked_ticks( ) {
    auto simtime = m_flSimulationTime( );

    return TIME_TO_TICKS( g_csgo.m_globals->m_curtime - simtime ) + 1;
}

int c_base_player::get_ping( ) {
    auto        resource = get_player_resource( );
    static auto offset = g_netvars.get_netvar( fnv( "DT_CSPlayerResource" ), fnv( "m_iPing" ) );
    int         ping = *( int* )( resource + offset + ce( )->GetIndex( ) * 4 );

    return ping;
}

int c_base_player::get_c4_carrier( ) {
	auto        resource = get_player_resource( );
	static auto offset = g_netvars.get_netvar( fnv( "DT_CSPlayerResource" ), fnv( "m_iPlayerC4" ) );

	return this->ce( )->GetIndex( ) == *( int* )( resource + offset  );
}

bool c_base_player::is_breaking_lc( ) {
    return g_cheat.m_prediction.is_breaking_lc( this->ce( )->GetIndex( ) );
}

bool c_base_player::can_attack( bool ignore_rapid ) {
    if( g_ctx.m_has_fired_this_frame )
        return false;
    if( !is_alive( ) )
        return false;

    auto weapon = get_weapon( );
    if( !weapon )
        return false;

    if( weapon->is_sniper( ) && g_cheat.m_lagmgr.has_fired( ) )
        return false;

    float time = g_ctx.pred_time( );

    float next_attack = m_flNextAttack( );
    if( next_attack > time )
        return false;

    float next_primary_attack = weapon->m_flNextPrimaryAttack( );
    if( next_primary_attack > time ) {
        if( weapon->is_knife( ) ) {
            if( weapon->m_flNextSecondaryAttack( ) > time )
                return false;
        } else {
            return false;
        }
    }

    if( weapon->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER ) {
        return g_ctx.m_revolver_shot;
    }

    return true;
}

int c_base_player::get_seq_activity( int sequence ) {
    static uintptr_t fn_offset = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" ) );
    auto fn = ( int( __thiscall* )( void*, int ) )( fn_offset );

    return fn ? fn( this, sequence ) : -2;
}

void c_base_player::set_abs_angles( vec3_t angles ) {
#ifdef HEADER_MODULE
    static uintptr_t fn_addr = g_header.patterns.set_abs_angles;
#else
    static uintptr_t fn_addr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" ) );
#endif

    auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );

    return fn( this, angles );
}

void c_base_player::set_abs_origin( vec3_t origin ) {
#ifdef HEADER_MODULE
    static uintptr_t fn_addr = g_header.patterns.set_abs_origin;
#else
    static uintptr_t fn_addr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 51 53 56 57 8B F1" ) );
#endif

    auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );

    return fn( this, origin );
}

void c_base_player::create_animstate( CCSGOPlayerAnimState* state ) {
    static auto fn_addr = pattern::first_code_match( g_csgo.m_chl.dll( ),
                                                     xors( "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" ) );
    auto fn = ( void( __thiscall* )( CCSGOPlayerAnimState*, decltype( this ) ) )( fn_addr );

    fn( state, this );
    state->pBaseEntity = this;
}

void c_base_player::set_abs_velocity( vec3_t velocity ) {
    static auto fn_addr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1" ) );
    auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );

    fn( this, velocity );
}

void c_base_player::invalidate_bone_cache( ) {
    m_flLastBoneSetupTime( ) = -FLT_MAX;
    m_iMostRecentModelBoneCounter( ) = 0;
}

void c_base_player::handle_taser_animation( ) {
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 08 56 8B F1 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 80 BE "
                                   "? ? ? ? ? 0F 84 ? ? ? ? A1" ) );
    if( fn_ptr ) {
        vec3_t eye_angles = m_angEyeAngles( );

        __asm {
			mov ecx, this
			fld eye_angles.y
			fld eye_angles.x

			call fn_ptr
        }
    }
}

vec3_t c_base_player::get_eye_pos( ) {
    vec3_t yes;
    // fuck references Lol
    util::get_vfunc< void( __thiscall* )( void*, vec3_t& ) >( this, 284 )( this, yes );

    // thanks senator!! !11
    return yes;
}

void c_base_player::calculate_duckamount( bool reset ) {
    static float last_duckamt[ 65 ]{};
    static float original_duckamt[ 65 ]{};
    int          idx = ce( )->GetIndex( );

    if( reset ) {
        original_duckamt[ idx ] = m_flDuckAmount( );
        last_duckamt[ idx ] = m_flDuckAmount( );
        return;
    }

    float cur_duck = m_flDuckAmount( );
    float delta = m_flOldSimulationTime( ) - m_flSimulationTime( ) + TICK_INTERVAL( );

    float old_duck = last_duckamt[ idx ];
    float rate = ( cur_duck - old_duck ) * delta;

    m_flDuckAmount( ) = old_duck + rate;

    if( cur_duck != original_duckamt[ idx ] )
        last_duckamt[ idx ] = cur_duck;
    original_duckamt[ idx ] = cur_duck;
}

void c_base_player::cache_anim_data( bool layers ) {
    int   idx = ce( )->GetIndex( );
    auto& animdata = sm_animdata[ idx ];

    memcpy( &animdata.m_poseparams, m_flPoseParameter( ), sizeof( float ) * 24 );

    if( layers ) {
        memcpy( &animdata.m_animlayers,
                m_AnimOverlay( ).GetElements( ),
                sizeof( C_AnimationLayer ) * 13 );
    }

    memcpy( animdata.m_bones,
            m_CachedBoneData( ).GetElements( ),
            sizeof( matrix3x4 ) * m_CachedBoneData( ).GetSize( ) );
}

void c_base_player::restore_anim_data( bool layers ) {
    int   idx = ce( )->GetIndex( );
    auto& animdata = sm_animdata[ idx ];

    memcpy( m_flPoseParameter( ), &animdata.m_poseparams, sizeof( float ) * 24 );

    if( layers ) {
        memcpy( m_AnimOverlay( ).GetElements( ),
                &animdata.m_animlayers,
                sizeof( C_AnimationLayer ) * 13 );
    }

    memcpy( animdata.m_bones,
            m_CachedBoneData( ).GetElements( ),
            sizeof( matrix3x4 ) * m_CachedBoneData( ).GetSize( ) );
}

void c_base_player::do_ent_interpolation( bool reset ) {
    auto  idx = ce( )->GetIndex( );
    auto* anim_data = &sm_animdata[ idx ];

    vec3_t origin = m_vecOrigin( );

    if( reset ) {
        anim_data->m_last_interp_origin = anim_data->m_interp_origin = origin;
        anim_data->m_simtime = anim_data->m_last_simtime = g_csgo.m_globals->m_curtime;
        return;
    }

    if( !anim_data->m_valid )
        return;

    if( origin.dist_to_sqr( anim_data->m_interp_origin ) > FLT_EPSILON ) {
        anim_data->m_last_interp_origin = anim_data->m_interp_origin;
        anim_data->m_interp_origin = origin;
        anim_data->m_last_simtime = anim_data->m_simtime;
        anim_data->m_simtime = g_csgo.m_globals->m_curtime;
    }

    float lerp = util::get_lerptime( );
    if( !lerp )
        return;

    // yes
    if( anim_data->m_last_interp_origin.dist_to( anim_data->m_interp_origin ) > 128.f )
        return;

    for( size_t i{}; i < 3; ++i ) {
        if( !std::isfinite( anim_data->m_last_interp_origin[ i ] ) ||
            !std::isfinite( anim_data->m_interp_origin[ i ] ) ) {
            char error_msg[ 128 ];
            sprintf_s< 128 >( error_msg,
                              "player origin was NaN\nreport this bug\ndetails:\n %f %f",
                              g_csgo.m_globals->m_curtime,
                              anim_data->m_simtime );

            MessageBoxA( nullptr, error_msg, "error", MB_OK );
            exit( 0 );
        }
    }

    float update_delta = anim_data->m_simtime - anim_data->m_last_simtime;
    float update_lerp = std::clamp( update_delta - lerp, 0.f, 1.f );

    if( update_delta < TICK_INTERVAL( ) )
        return;

    lerp = std::clamp( lerp, 0.f, update_delta );

    float lerp_progress = ( anim_data->m_simtime + lerp - g_csgo.m_globals->m_curtime ) / lerp;

    vec3_t vec_lerp = math::lerp(
        origin, anim_data->m_last_interp_origin, std::clamp( lerp_progress, 0.f, 1.f ) );

    for( size_t i{}; i < 3; ++i )
        if( !isfinite( vec_lerp[ i ] ) )
            return;

    byte backup = *( byte* )( uintptr_t( this ) + 0x274 );
    *( byte* )( uintptr_t( this ) + 0x274 ) = 0;

    // restore_anim_data( true );

    // aaaaAAAAAAAA
    // calc_abs_velocity( );
    set_abs_origin( vec_lerp );
    invalidate_bone_cache( );

    ce( )->SetupBones( nullptr, -1, BONE_USED_BY_ANYTHING, g_csgo.m_globals->m_curtime );
    this->m_iMostRecentModelBoneCounter( )++;

    *( byte* )( uintptr_t( this ) + 0x274 ) = backup;
}

void c_base_player::validate_animation_layers( ) {
    for( size_t i{}; i < m_AnimOverlay( ).GetSize( ); ++i ) {
        auto& layer = m_AnimOverlay( ).GetElements( )[ i ];
        layer.m_flCycle = std::clamp( layer.m_flCycle, 0.f, 1.f );
        // layer.m_flWeight = std::clamp( layer.m_flWeight, 0.f, 1.f );

        layer.m_player = this;
    }

    for( size_t i{}; i < 24; ++i ) {
        auto& param = m_flPoseParameter( )[ i ];
        if( !isfinite( param ) )
            param = 0.f;
    }
}

void c_base_player::compute_move_cycle( bool reset, bool moving ) {
    if( !moving || reset )
        return;

    float eye_yaw = m_angEyeAngles( ).y;

    float blend_ang = math::approach_angle(
        eye_yaw, get_animstate( )->m_flCurrentFeetYaw, get_animstate( )->m_flFeetYawRate * 100.f );

    // get_animstate( )->m_flCurrentFeetYaw = blend_ang + 360.f;
    m_flPoseParameter( )[ LEAN_YAW ] = ( blend_ang + 180.f ) / 360.f;
    m_flPoseParameter( )[ BODY_YAW ] = ( blend_ang + 180.f ) / 360.f;
}

void c_base_player::calc_anim_velocity( bool reset ) {
    int idx = ce( )->GetIndex( );

    auto accelerate = [&]( vec3_t velocity, vec3_t direction, float speed, float accel ) {
        float addspeed, accelspeed, currentspeed;

        velocity.z = 0.f;
        currentspeed = velocity.dot( direction );

        addspeed = speed - currentspeed;

        if( addspeed <= 0.f ) {
            return velocity;
        }

        // guess how many fucks i give, this works
        accelspeed = std::min( accel * 10.f * TICK_INTERVAL( ) * speed, currentspeed );

        for( size_t i{}; i < 3; ++i ) {
            velocity[ i ] += accelspeed * direction[ i ];
        }

        return velocity;
    };

    auto friction = [&]( vec3_t velocity ) {
        static auto sv_friction = g_csgo.m_cvar( )->FindVar( xors( "sv_friction" ) );
        static auto sv_stopspeed = g_csgo.m_cvar( )->FindVar( xors( "sv_stopspeed" ) );

        float speed = velocity.length2d( );
        if( speed < 0.1f )
            return vec3_t{};

        float friction = sv_friction->get_float( );
        float control = ( speed < sv_stopspeed->get_float( ) ) ? sv_stopspeed->get_float( ) : speed;
        float drop = control * friction * TICK_INTERVAL( );

        float newspeed = speed - drop;
        if( newspeed < 0.f )
            newspeed = 0.f;

        if( newspeed != speed ) {
            newspeed /= speed;
            velocity *= newspeed;
        }

        return velocity;
    };

    if( true ) {
        vec3_t velocity = m_vecVelocity( );
        sm_animdata[ idx ].m_last_origin = m_vecOrigin( );
        sm_animdata[ idx ].m_last_velocity = velocity;
        sm_animdata[ idx ].m_anim_velocity = velocity;
    } else {
        static auto sv_accelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_accelerate" ) );

        float delta = m_flSimulationTime( ) - m_flOldSimulationTime( );
        delta = std::max( delta, TICK_INTERVAL( ) );

        bool on_ground = m_fFlags( ) & FL_ONGROUND;

        vec3_t origin = m_vecOrigin( );
        vec3_t origin_delta = origin - sm_animdata[ idx ].m_last_origin;

        vec3_t velocity = origin_delta / delta;
        vec3_t last_velocity = sm_animdata[ idx ].m_last_velocity;

        vec3_t anim_vel;

        if( on_ground && !( m_fFlags( ) & FL_ONGROUND ) ) {
            vec3_t ang = math::vector_angles( vec3_t( ), velocity );

            float move_yaw = math::vector_angles( velocity ).y;
            float move_delta = math::vector_angles( last_velocity ).y;
            move_delta -= move_yaw;
            move_delta = std::remainderf( move_delta, 360.f );

            vec3_t move_dir = math::angle_vectors( vec3_t( 0.f, move_delta, 0.f ) ) * 450.f;

            vec3_t forward, right, up;
            math::angle_vectors( ang, &forward, &right, &up );

            vec3_t wishdir;
            for( size_t i{}; i < 2; ++i )
                wishdir[ i ] = forward[ i ] * move_dir.x + right[ i ] * move_dir.y;

            anim_vel = friction( last_velocity );
            if( anim_vel.length2d( ) < 1.f )
                anim_vel = vec3_t( );

            int    ticks = TIME_TO_TICKS( delta );
            vec3_t est_tick_vel = last_velocity;

            for( int i{}; i < ticks; i++ ) {
                est_tick_vel = friction( est_tick_vel );

                if( i <= ticks * 0.75f ) {
                    accelerate( anim_vel, wishdir, 250.f, sv_accelerate->get_float( ) );
                }
            }

            if( velocity.length2d( ) > last_velocity.length2d( ) )
                anim_vel = accelerate( anim_vel, wishdir, 250.f, sv_accelerate->get_float( ) );

            // assume fakewalk
            if( anim_vel.length2d( ) >= sm_animdata[ idx ].m_anim_velocity.length2d( ) &&
                est_tick_vel.length2d( ) < 1.f && delta > TICK_INTERVAL( ) )
                anim_vel = vec3_t( );
        } else {
            anim_vel = math::lerp( last_velocity, velocity, TICK_INTERVAL( ) / delta );
        }

        sm_animdata[ idx ].m_anim_velocity = anim_vel;
        sm_animdata[ idx ].m_last_velocity = velocity;
        sm_animdata[ idx ].m_last_origin = origin;
    }
}

void c_base_player::calc_abs_velocity( ) {
#ifdef HEADER_MODULE
    static auto fn_ptr = g_header.patterns.calc_abs_velocity;
#else
    static auto fn_ptr = pattern::first_code_match(
        g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7 87" ), 0 );
#endif

    if( fn_ptr ) {
        auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
        fn( this );
    }
}

void c_base_player::fix_jump_fall( bool reset ) {
    if( reset )
        return;

    int prev_flags = get_animdata( ).m_prev_flags;
    int flags = m_fFlags( );

    if( ( prev_flags & FL_ONGROUND ) && ( flags & FL_ONGROUND ) ) {
        flags |= FL_ONGROUND;
    } else {
        auto layer_weight = m_AnimOverlay( ).GetElements( )[ 4 ].m_flWeight;
        auto last_weight = get_animdata( ).m_last_layers.at( 4 ).m_flWeight;

        if( layer_weight != 1.f && last_weight == 1.f &&
            m_AnimOverlay( ).GetElements( )[ 5 ].m_flWeight != 0.f )
            flags |= FL_ONGROUND;

        if( ( flags & FL_ONGROUND ) && !( prev_flags & FL_ONGROUND ) )
            flags &= ~FL_ONGROUND;
    }

    m_fFlags( ) = flags;
}

void c_base_player::fix_animations( bool reset, bool resolver_change ) {
    // todo: legs dont match up when fakelagging <- not anymore
    int idx = ce( )->GetIndex( );

    bool was_valid = sm_animdata[ idx ].m_valid;
    sm_animdata[ idx ].m_valid = false;

    if( !get_animstate( ) )
        return;

    m_angEyeAngles( ).y = std::remainder( m_angEyeAngles( ).y, 360.f );

    int  flags = m_fFlags( );
    auto eye_angles = m_angEyeAngles( );
    auto original_duck = m_flDuckAmount( );

    float delta = m_flSimulationTime( ) - m_flOldSimulationTime( );

    // enable animations - time to update
    m_bClientSideAnimation( ) = true;
    set_needs_interpolate( false );

    // we need the player data at THIS EXACT moment, somehow
    float anim_time = m_flSimulationTime( );

    auto backup_curtime = g_csgo.m_globals->m_curtime;
    auto backup_frametime = g_csgo.m_globals->m_frametime;

    g_csgo.m_globals->m_curtime = m_flOldSimulationTime( ) + TICK_INTERVAL( );
    g_csgo.m_globals->m_frametime = TICK_INTERVAL( );

    /*if( resolver_change && m_fFlags( ) & FL_ONGROUND && !reset ) {
     float lby_delta = m_flLowerBodyYawTarget( ) - eye_angles.y;
     lby_delta = std::remainderf( lby_delta, 360.f );
     lby_delta = std::clamp( lby_delta, -60.f, 60.f );

     float feet_yaw = std::remainderf( eye_angles.y + lby_delta, 360.f );
     if( feet_yaw < 0.f )
      feet_yaw += 360.f;

     get_animstate( )->m_flGoalFeetYaw = get_animstate( )->m_flCurrentFeetYaw = feet_yaw;
    }*/

    get_animstate( )->m_flFeetYawRate = 0.f;

    // why?
    // because this calls pAttachmentHelper->CalcAbsVelocity
    // aswell as removes EFL_DIRTY_ABSVELOCITY
    // which fixes attachments etc
    // normally this would be called by animstate->update
    // but we prevent that
    calc_abs_velocity( );

    // calc_anim_velocity( reset );

    set_abs_origin( m_vecOrigin( ) );
    // networked duck amount comes from the last simulation tick instead of the current one
    // calculate_duckamount( reset );
    // fix_jump_fall( reset );
    validate_animation_layers( );

    float old_cycle = m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle;
    get_animstate( )->m_iLastClientSideAnimationUpdateFramecount -= 1;

    vec3_t velocity = m_vecVelocity( );
    // int backup_eflags = get< int >( 0xe4 );
    // int backup_byte = get< byte >( 0x35f8 );

    if( !reset ) {
        // m_vecVelocity( ) = sm_animdata[ idx ].m_anim_velocity;
        // get< vec3_t >( 0x94 ) = sm_animdata[ idx ].m_anim_velocity;
        // get< byte >( 0x35f8 ) = 1;
        // get< int >( 0xe4 ) &= ~0x1000;

        // get_animstate( )->m_flUnknownFraction = sm_animdata[ idx ].m_fraction;
    }

    sm_animdata[ idx ].m_last_duck = m_flDuckAmount( );

    float prev_cycle = m_AnimOverlay( ).GetElements( )[ 6 ].m_flPrevCycle;
    float prev_rate = m_AnimOverlay( ).GetElements( )[ 6 ].m_flPlaybackRate;

    /*if( !reset ) {
     auto fraction = get_animstate( )->m_flStopToFullRunningFraction;
     bool stopping = sm_animdata[ idx ].m_is_stopping;

     if( fraction > 0.1f && fraction < 1.f ) {
      float delta_2x = get_animstate( )->m_flLastUpdateDelta * 2.f;

      if( stopping )
       fraction -= delta_2x;
      else
       fraction += delta_2x;

      fraction = std::clamp( fraction, 0.f, 1.f );
     }

     auto speed = get_anim_velocity( ).length2d( );

     if( speed > 135.2f && stopping ) {
      sm_animdata[ idx ].m_is_stopping = false;
      fraction = std::max( fraction, 0.0099999998f );
     }
     if( speed < 135.2f && !stopping ) {
      sm_animdata[ idx ].m_is_stopping = true;
      fraction = std::min( fraction, 0.99000001f );
     }

     get_animstate( )->m_flStopToFullRunningFraction = fraction;
    }*/

    C_AnimationLayer prev_layers[ 13 ];
    memcpy( prev_layers, m_AnimOverlay( ).GetElements( ), sizeof( prev_layers ) );

    sm_animdata[ idx ].m_last_animtime = get_animstate( )->m_flLastClientSideAnimationUpdateTime;
    // m_AnimOverlay( ).GetElements( )[ 6 ].m_flPrevCycle = m_AnimOverlay( ).GetElements( )[ 6
    // ].m_flCycle;

    g_csgo.m_globals->m_curtime += delta;
    for( int i = 0; i < TIME_TO_TICKS( delta ); ++i ) {
        get_animstate( )->m_flFeetYawRate = 0.f;
        get_animstate( )->update( eye_angles.y, eye_angles.x );

        // ADVANCE THE FRAME
        g_csgo.m_globals->m_curtime += TICK_INTERVAL( );
    }

#ifdef _DEBUG
    static con_var< bool > dbg_desync{&data::holder_, fnv( "dbg_desync" ), false};

    if( dbg_desync( ) ) {
        g_con->m_mutex.lock( );
        g_con->m_logs.clear( );
        g_con->m_mutex.unlock( );
        auto& layer = m_AnimOverlay( ).GetElements( )[ 3 ];
        auto& prev = prev_layers[ 3 ];

        g_con->log( "delta:		  c: %0.2f w: %0.2f r: %0.2f",
                    layer.m_flCycle - prev.m_flCycle,
                    layer.m_flWeight - prev.m_flWeight,
                    layer.m_flPlaybackRate - prev.m_flPlaybackRate );
        g_con->log( "to:          c: %0.2f w: %0.2f r: %0.2f",
                    layer.m_flCycle,
                    layer.m_flWeight,
                    layer.m_flPlaybackRate );
        g_con->log( "from:        c: %0.2f w: %0.2f r: %0.2f",
                    prev.m_flCycle,
                    prev.m_flWeight,
                    prev.m_flPlaybackRate );
    }
#endif

    if( was_valid && !reset ) {
        float lerp_rate =
            math::lerp( prev_rate,
                        m_AnimOverlay( ).GetElements( )[ 6 ].m_flPlaybackRate,
                        TICK_INTERVAL( ) / ( m_flSimulationTime( ) - m_flOldSimulationTime( ) ) );

        // m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = prev_cycle + lerp_rate;
    } else if( !reset ) {
        float cycle = m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle;
        float rate = m_AnimOverlay( ).GetElements( )[ 6 ].m_flPlaybackRate;

        float est_cycle = cycle - rate +
            ( rate * TICK_INTERVAL( ) / ( m_flSimulationTime( ) - m_flOldSimulationTime( ) ) );

        // m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = est_cycle;
    }

    // m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = std::fmod( m_AnimOverlay( ).GetElements( )[
    // 6 ].m_flCycle, 1.0f );

    get_animdata( ).m_prev_flags = flags;
    memcpy( get_animdata( ).m_last_layers.data( ),
            m_AnimOverlay( ).GetElements( ),
            sizeof( C_AnimationLayer ) * 13 );

    sm_animdata[ idx ].m_fraction = get_animstate( )->m_flUnknownFraction;

    // m_vecVelocity( ) = velocity;
    // get< vec3_t >( 0x94 ) = velocity;
    // get< int >( 0xe4 ) = backup_eflags;
    // get< byte >( 0x35f8 ) = backup_byte;

    // m_AnimOverlay( ).GetElements( )[ 6 ].m_flPrevCycle = old_cycle;

    // check for any possible mistakes
    validate_animation_layers( );

    // sm_animdata[ idx ].m_adjust_rate = m_AnimOverlay( ).GetElements( )[ 3 ].m_flPlaybackRate;
    // sm_animdata[ idx ].m_adjust_cycle = m_AnimOverlay( ).GetElements( )[ 3 ].m_flCycle;

    // if( m_flSimulationTime( ) - m_flOldSimulationTime( ) > TICK_INTERVAL( ) ) {
    // auto activity = get_seq_activity( m_AnimOverlay( ).GetElements( )[ 3 ].m_nSequence );

    // if( activity == 979 ) {
    //	m_AnimOverlay( ).GetElements( )[ 3 ].m_flWeight = 0.f;
    //	m_AnimOverlay( ).GetElements( )[ 3 ].m_flCycle = 0.f;
    //}
    //}

    // bool moving = sm_animdata[ idx ].m_anim_velocity.length( ) > 0.1f;
    // compute_move_cycle( reset, moving );

    get_animdata( ).m_anim_flags = m_fFlags( );

    // if( !moving )
    //	m_AnimOverlay( ).GetElements( )[ 6 ].m_flWeight = 0.f;

    ce( )->GetRenderAngles( ).y = std::remainderf( ce( )->GetRenderAngles( ).y, 360.f );

    m_flDuckAmount( ) = original_duck;
    m_fFlags( ) = flags;
    m_angEyeAngles( ) = eye_angles;
    m_flDuckAmount( ) = original_duck;

    // get_animstate( )->m_flUnknownFraction = 0.f;

    // clear occlusion for setupbones (pvs fix)
    *( int* )( uintptr_t( this ) + 0xa30 ) = 0;

    byte backup = get< byte >( 0x274 );
    get< byte >( 0x274 ) = 0;

    invalidate_bone_cache( );
    ce( )->SetupBones( nullptr, -1, BONE_USED_BY_ANYTHING, anim_time );
    cache_anim_data( );

    g_csgo.m_globals->m_curtime = backup_curtime;
    g_csgo.m_globals->m_frametime = backup_frametime;

    if( !reset ) {
        sm_animdata[ idx ].m_valid = true;
    }

    // disable animations again
    // m_bClientSideAnimation( ) = false;
    get< byte >( 0x274 ) = backup;
}

player_info_t c_base_player::get_info( ) {
    player_info_t info;
    g_csgo.m_engine( )->GetPlayerInfo( ce( )->GetIndex( ), &info );

    return info;
}

bool c_base_player::is_flashed( ) {
    return m_flFlashTime( ) - g_csgo.m_globals->m_curtime > m_flFlashDuration( ) * 0.5f;
}

float c_base_player::get_flashed_alpha( ) {
	static auto offset = g_netvars.get_netvar( fnv( "DT_CSPlayer" ), fnv( "m_flFlashDuration" ) );
	return *reinterpret_cast< float * >( uintptr_t( this ) + ( offset - 12 ) );
}

bool c_base_player::is_reloading( ) {
    auto  gun_layer = m_AnimOverlay( ).GetElements( )[ 1 ];
    float playback_rate = gun_layer.m_flPlaybackRate;
    if( playback_rate < 0.55f ) {
        return gun_layer.m_flCycle < 0.99f;
    }

    return false;
}

void c_base_player::get_name_safe( char* buf ) {
    player_info_t info;
    if( g_csgo.m_engine( )->GetPlayerInfo( this->ce( )->GetIndex( ), &info ) ) {
        for( size_t i{}; i < 32; ++i ) {
            switch( info.name[ i ] ) {
            case '"':
            case '\\':
            case ';':
            case '\n': buf[ i ] = ' '; break;
            default: buf[ i ] = info.name[ i ]; break;
            }
        }

        buf[ 31 ] = 0;
    }
}

void c_base_player::set_local_view_angles( vec3_t* angle ) {
    using fn = void( __thiscall* )( void*, vec3_t* );
    util::get_vfunc< fn >( this, 363 )( this, angle );
}

bool c_base_player::run_physics_think( int unk01 ) {
    static auto impl =
        reinterpret_cast< bool( __thiscall* )( void*, int ) >( pattern::first_code_match(
            g_csgo.m_chl.dll( ),
            xors( "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87 ? ? ? ? C1 E8 16" ) ) );

    return impl( this, unk01 );
}

void c_base_player::pre_think( ) {
    using fn = void( __thiscall* )( void* );
    util::get_vfunc< fn >( this, 309 )( this );
}

void c_base_player::think( ) {
    using fn = void( __thiscall* )( void* );
    util::get_vfunc< fn >( this, 137 )( this );
}

void c_base_player::setup_bones_ex( matrix3x4* array_out, int bone_mask, float curtime ) {
    /*	auto hdr = *( studiohdr_t** )( uintptr_t( ce( )->GetClientRenderable( ) ) + 0x2938 );
     auto bone_array = ( matrix3x4* )( m_dwBoneMatrix( ) );

     auto readable_bones = *( int* )( ( uintptr_t )ce( )->GetClientRenderable( ) + 0x2694 );
     bone_mask |= readable_bones;

     //backup necessary data
     matrix3x4	backup_bone_array[ 128 ];
     float		backup_pose_params[ 24 ];

     memcpy( backup_bone_array,
      ( void* )( m_dwBoneMatrix( ) ),
      sizeof( backup_bone_array ) );

     memcpy( backup_pose_params,
      m_flPoseParameter( ),
      sizeof( backup_pose_params ) );

     vec3_t backup_origin = ce( )->GetRenderOrigin( );
     vec3_t backup_angles = ce( )->GetRenderAngles( );

     //setup a transform matrix
     matrix3x4 transform{ };
     math::angle_imatrix( backup_angles, transform );
     math::set_matrix_position( backup_origin, transform );

     //set render flags
     *( int* )( ( uintptr_t )ce( )->GetClientRenderable( ) + 0xE0 ) |= 8;

     update_ik_locks( m_flSimulationTime( ) );
     IKContext* ik = m_IKContext( );

     if( ik ) {
      ik->ClearTargets( );
      ik->Init( hdr, backup_angles,
       backup_origin,
       curtime,
       g_csgo.m_globals->m_framecount,
       bone_mask );
     }

     vec3_t pos[ 128 ];
     float  q[ 4 ][ 128 ];

     standard_blending_rules( hdr, pos, q, curtime, bone_mask );

     //idk but this is in ida
     char computed[ 0x100 ]{ };

     if( ik ) {
      ik->UpdateTargets( pos, q, bone_array, computed );
      calculate_ik_locks( curtime );
      ik->SolveDependencies( pos, q, bone_array, computed );
     }

     build_transformations( hdr, pos, q, transform, bone_mask, computed );

     //clear render flags
     *( int* )( ( uintptr_t )ce( )->GetClientRenderable( ) + 0xE0 ) &= ~8;

     //copy new bone array into the matrix
     memcpy( array_out, bone_array, 128 * sizeof( matrix3x4 ) );

     /*
     //restore data
     memcpy( bone_array,
      backup_bone_array,
      sizeof( matrix3x4 ) );

     memcpy( m_flPoseParameter( ),
      backup_pose_params,
      sizeof( backup_pose_params ) );

     set_abs_angles( backup_angles );
     set_abs_origin( backup_origin );
     */
}
