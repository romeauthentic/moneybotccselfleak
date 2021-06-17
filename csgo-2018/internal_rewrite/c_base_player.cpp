#include "c_base_player.hpp"
#include "interface.hpp"
#include "math.hpp"
#include "mem.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

#undef max

ent_animdata_t c_base_player::sm_animdata[ 65 ];
CCSGOPlayerAnimState* c_base_player::sm_player_animstate[65];

void IKContext::Init( void* hdr, vec3_t& angles, vec3_t& origin, float curtime, int framecount, int bone_mask ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D 8F" ) );

	auto fn = ( void( __thiscall* )( void*, void*, vec3_t&, vec3_t&, float, int, int ) )( fn_ptr );
	fn( this, hdr, angles, origin, curtime, framecount, bone_mask );
}

void IKContext::UpdateTargets( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F0 81 EC 00 00 00 00 33 D2" ) );

	auto fn = ( void( __thiscall* )( void*, vec3_t*, void*, matrix3x4*, char* ) )( fn_ptr );
	fn( this, pos, q, bone_array, computed );
}

void IKContext::SolveDependencies( vec3_t* pos, void* q, matrix3x4* bone_array, char* computed ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F0 81 EC 00 00 00 00 8B 81" ) );

	auto fn = ( void( __thiscall* )( void*, vec3_t*, void*, matrix3x4*, char* ) )( fn_ptr );
	fn( this, pos, q, bone_array, computed );
}

bool& c_base_player::s_bInvalidateBoneCache( ) {
	static auto fn_ptr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
	static auto var = *(bool**)(fn_ptr + 0xda + 2);

	return *var;
}

void CCSGOPlayerAnimState::update( float yaw, float pitch ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" ) );

	uintptr_t alloc = ( uintptr_t )this;
	if( !alloc )
		return;

	//bool* b = &c_base_player::s_bInvalidateBoneCache( );
	//char backup = *b;

	//vectorcall is GAY
	if( fn_ptr ) {
		__asm {
			push 0
			mov ecx, alloc

			movss xmm1, dword ptr[ yaw ]
			movss xmm2, dword ptr[ pitch ]

			call fn_ptr
		}
	}

	//c_base_player::s_bInvalidateBoneCache( ) = backup;
}

void CCSGOPlayerAnimState::setup_velocity( float a2 ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D" ) );
	if( fn_ptr ) {
		auto ptr_ = ( uintptr_t )this;

		__asm {
			mov ecx, ptr_

			fld a2
			call fn_ptr
		}
	}
}

bool CCSGOPlayerAnimState::cache_sequences( ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 34 53 56 8B F1 57 8B 46 60" ) );
	if( fn_ptr ) {
		auto fn = ( bool( __thiscall* )( void* ) )( fn_ptr );
		return fn( this );
	}

	return false;
}

void CCSGOPlayerAnimState::update_ex( float yaw, float pitch ) {
/*	static auto unk_func_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 75 14 8B 0D ? ? ? ? 8B 01" ) );
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
				float total_duck = *( float* )( uintptr_t( player ) + 0xbe7 ) + m_fLandingDuckAdditiveSomething;

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
	static auto fn_ptr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("56 6A 01 68 ? ? ? ? 8B F1"));
	if (fn_ptr) {
		auto fn = (void(__thiscall*)(void*))(fn_ptr);
		fn(this);
	}
}

void CCSGOPlayerAnimState::setup_aim_matrix( ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D" ) );
	if( fn_ptr ) {
		auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
		fn( this );
	}
}

void c_base_player::invalidate_physics_recursive( int flags ) {
	static auto fn_ptr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3"));
	if (fn_ptr) {
		auto fn = (void(__thiscall*)(void*, int))(fn_ptr);
		fn(this, flags);
	}
}

uintptr_t c_base_player::get_player_resource( ) {
	for( int i{ 1 }; i < g_csgo.m_entlist( )->GetHighestEntityIndex( ); ++i ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< IClientEntity >( i );
		if( !ent ) continue;
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
	static uintptr_t** game_rules = pattern::first_code_match< uintptr_t** >(g_csgo.m_chl.dll(), xors("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05"), 0x1);
	return **game_rules;
}

int& c_base_player::g_iModelBoneCounter( ) {
	static auto offset = pattern::first_code_match(g_csgo.m_chl.dll(), xors("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"));
	return **(int**)(offset + 0xa);
}

//use is_zero( ) on the vector to figure out if it returns a valid value
vec3_t c_base_player::get_hitbox_pos( int hitbox ) {
	auto ce = this->ce();

	const auto model = ce->GetModel();
	if (!model) return vec3_t{ };

	auto hdr = g_csgo.m_model_info()->GetStudiomodel(model);
	if (!hdr) return vec3_t{ };

	matrix3x4 bone_matrix[128];
	if (!ce->SetupBones(bone_matrix, 128, 0x100, g_csgo.m_globals->m_curtime)) {
		return vec3_t{ };
	}

	auto set = hdr->pHitboxSet(m_nHitboxSet());
	if (!set) return vec3_t{ };

	auto box = set->pHitbox(hitbox);
	if (!box) return vec3_t{ };

	vec3_t center = (box->bbmin + box->bbmax) * 0.5f;

	return math::vector_transform(center, bone_matrix[box->bone]);
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

	for( size_t i{ }; i < varmap.m_nInterpolatedEntries; ++i ) {
		auto& e = elements[ i ];
		e.m_bNeedsToInterpolate = interpolate;
	}
}

bool c_base_player::is_visible( int hitbox ) {
	vec3_t start = g_ctx.m_local->get_eye_pos( );
	vec3_t end = get_hitbox_pos( hitbox );

	if( !end ) return false;
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
		if( box->m_flRadius == -1 ) return box->bbmin.dist_to( box->bbmax );
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
	auto other_layer = m_AnimOverlay( ).GetElements( )[ 5 ];

	auto velocity = m_vecVelocity( ).length2d( );
	bool on_ground = m_fFlags( ) & FL_ONGROUND;
	if( walk_layer.m_flWeight < 0.3f 
		&& some_layer.m_flWeight < 0.2f
		&& velocity > 0.1f && on_ground ) {
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
	auto resource = get_player_resource( );
	static auto offset = g_netvars.get_netvar( fnv( "DT_CSPlayerResource" ), fnv( "m_iPing" ) );
	int ping = *( int* )( resource + offset + ce( )->GetIndex( ) * 4 );

	return ping;
}

int c_base_player::get_c4_carrier() {
	auto        resource = get_player_resource();
	static auto offset = g_netvars.get_netvar(fnv("DT_CSPlayerResource"), fnv("m_iPlayerC4"));

	return this->ce()->GetIndex() == *(int*)(resource + offset);
}

bool c_base_player::is_breaking_lc( ) {
	return g_cheat.m_prediction.is_breaking_lc( this->ce( )->GetIndex( ) );
}

bool c_base_player::can_attack( bool ignore_rapid ) {
	if (g_ctx.m_has_fired_this_frame) return false;
	if (!is_alive()) return false;

	auto weapon = get_weapon();
	if (!weapon) return false;

	float time = g_ctx.pred_time();

	float next_attack = m_flNextAttack();
	if (next_attack > time) return false;

	float next_primary_attack = weapon->m_flNextPrimaryAttack();
	if (next_primary_attack > time) return false;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_R8REVOLVER) {
		float ready_time = weapon->m_flPostponeFireReadyTime();
		if (ready_time == FLT_MAX) return false;

		float time_to_shoot = ready_time - time;
		return TIME_TO_TICKS(time_to_shoot) < 1;
	}

	return true;
}

int c_base_player::get_seq_activity(int sequence) {
	static uintptr_t fn_offset = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 83 7D 08 FF 56 8B F1 74 3D"));
	auto fn = (int(__thiscall*)(void*, int))(fn_offset);

	return fn ? fn(this, sequence) : -2;
}

void c_base_player::set_abs_angles(vec3_t angles) {
	static uintptr_t fn_addr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	auto fn = (void(__thiscall*)(void*, vec3_t&))(fn_addr);

	return fn(this, angles);
}

void c_base_player::set_abs_origin(vec3_t origin) {
	static uintptr_t fn_addr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	auto fn = (void(__thiscall*)(void*, vec3_t&))(fn_addr);

	return fn(this, origin);
}

void c_base_player::create_animstate(CCSGOPlayerAnimState* state) {
	static auto fn_addr = pattern::first_code_match(g_csgo.m_chl.dll(), xors("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));
	auto fn = (void(__thiscall*)(CCSGOPlayerAnimState*, decltype(this)))(fn_addr);

	fn(state, this);
}


void c_base_player::set_abs_velocity( vec3_t velocity ) {
	static auto fn_addr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 0C 53 56 57 8B 7D 08 8B F1" ) );
	auto fn = ( void( __thiscall* )( void*, vec3_t& ) )( fn_addr );

	fn( this, velocity );
}

void c_base_player::enforce_animations() {
	int idx = this->ce()->GetIndex();
	auto animstate = sm_player_animstate[idx];

	if (animstate && animstate->pBaseEntity == this) {
		memcpy(get_animstate(), animstate, sizeof(CCSGOPlayerAnimState));
	}
}

void c_base_player::invalidate_bone_cache( ) {
	static auto off =
		m_flLastBoneSetupTime() = -FLT_MAX;
	m_iMostRecentModelBoneCounter() -= 1;
}

void c_base_player::handle_taser_animation( ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 EC 08 56 8B F1 80 BE ? ? ? ? ? 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 0F 84 ? ? ? ? A1" ) );
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

void c_base_player::calculate_duckamount( bool reset ) {
	static float last_duckamt[ 65 ]{ };
	static float original_duckamt[ 65 ]{ };
	int			 idx = ce( )->GetIndex( );

	if( reset ) {
		original_duckamt[ idx ] = m_flDuckAmount( );
		last_duckamt[ idx ]		= m_flDuckAmount( );
		return;
	}

	float cur_duck = m_flDuckAmount( );
	float delta = m_flOldSimulationTime( ) - m_flSimulationTime( ) + TICK_INTERVAL( );

	float old_duck = last_duckamt[ idx ];
	float rate	   = ( cur_duck - old_duck ) * delta;

	m_flDuckAmount( ) = old_duck + rate;

	if( cur_duck != original_duckamt[ idx ] )
		last_duckamt[ idx ] = cur_duck;
	original_duckamt[ idx ] = cur_duck;
}

void c_base_player::cache_anim_data( ) {
	int idx = ce( )->GetIndex( );
	auto& animdata = sm_animdata[ idx ];

	util::memcpy( animdata.m_poseparams.data( ),
		m_flPoseParameter( ),
		sizeof( float ) * 24 );

	util::memcpy( animdata.m_animlayers.data( ),
		m_AnimOverlay( ).GetElements( ),
		sizeof( C_AnimationLayer ) * 13 );

	util::memcpy( animdata.m_bones,
		m_CachedBoneData( ).GetElements( ),
		sizeof( matrix3x4 ) * m_CachedBoneData( ).GetSize( ) );
}

void c_base_player::restore_anim_data( bool layers ) {
	int idx = ce( )->GetIndex( );
	auto& animdata = sm_animdata[ idx ];

	util::memcpy( m_flPoseParameter( ),
		animdata.m_poseparams.data( ),
		sizeof( float ) * 24 );

	if( layers ) {
		util::memcpy( m_AnimOverlay( ).GetElements( ),
			animdata.m_animlayers.data( ),
			sizeof( C_AnimationLayer ) * 13 );
	}

	util::memcpy( animdata.m_bones,
		m_CachedBoneData( ).GetElements( ),
		sizeof( matrix3x4 ) * m_CachedBoneData( ).GetSize( ) );
}

void c_base_player::do_ent_interpolation( bool reset ) {
//	return; //cuck

	
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

	float lerp = util::get_lerptime( ) * 2.f;
	if( !lerp )
		return;

	for( size_t i{ }; i < 3; ++i ) {
		if( !std::isfinite( anim_data->m_last_interp_origin[ i ] ) || !std::isfinite( anim_data->m_interp_origin[ i ] ) ) {
			char error_msg[ 128 ];
			sprintf_s< 128 >( error_msg, "player origin was NaN\nreport this bug\ndetails:\n %f %f",
				g_csgo.m_globals->m_curtime,
				anim_data->m_simtime );

			MessageBoxA( nullptr, error_msg, "error", MB_OK );
			exit( 0 );
		}
	}

	float update_delta = anim_data->m_simtime - anim_data->m_last_simtime;
	float update_lerp = std::clamp( update_delta - lerp, 0.f, 1.f );

	if( !update_delta )
		return;

	if( update_delta > 0.f )
		lerp = std::clamp( lerp, 0.f, update_delta );

	vec3_t cur_vec = math::lerp( anim_data->m_last_interp_origin, origin, update_lerp / update_delta );
	float lerp_progress = ( anim_data->m_simtime + lerp - g_csgo.m_globals->m_curtime ) / lerp;
	if( lerp_progress < 0.f && update_delta == TICK_INTERVAL( ) )
		lerp_progress = 1.f + lerp_progress;

	vec3_t vec_lerp = math::lerp( origin, cur_vec, std::clamp( lerp_progress, 0.f, 1.f ) );

	for( size_t i{ }; i < 3; ++i )
		if( !isfinite( vec_lerp[ i ] ) )
			return;

	byte backup = *( byte* )( uintptr_t( this ) + 0x270 );
	*( byte* )( uintptr_t( this ) + 0x270 ) = 0;

	restore_anim_data( true );
	
	//aaaaAAAAAAAA
	//calc_abs_velocity( );
	set_abs_origin( vec_lerp );
	invalidate_bone_cache( );
	
	ce( )->SetupBones( anim_data->m_bones, 128, BONE_USED_BY_ANYTHING, g_csgo.m_globals->m_curtime );
	this->m_iMostRecentModelBoneCounter( )++;

	*( byte* )( uintptr_t( this ) + 0x270 ) = backup;
}

void c_base_player::validate_animation_layers( ) {
	for( size_t i{ }; i < m_AnimOverlay( ).GetSize( ); ++i ) {
		auto& layer = m_AnimOverlay( ).GetElements( )[ i ];
		layer.m_flCycle = std::clamp( layer.m_flCycle, 0.f, 1.f );
		layer.m_flWeight = std::clamp( layer.m_flWeight, 0.f, 1.f );

		layer.m_player = this;
	}

	for( size_t i{ }; i < 24; ++i ) {
		auto& param = m_flPoseParameter( )[ i ];
		if( !isfinite( param ) )
			param = 0.f;
	}
}

void c_base_player::compute_move_cycle( bool reset, bool moving ) {
	if( !moving || reset )
		return;

	float eye_yaw = m_angEyeAngles( ).y;

	float blend_ang = math::approach_angle( eye_yaw,
		get_animstate( )->m_flCurrentFeetYaw,
		get_animstate( )->m_flFeetYawRate * 100.f );

	//get_animstate( )->m_flCurrentFeetYaw = blend_ang + 360.f;
	m_flPoseParameter( )[ LEAN_YAW ] = ( blend_ang + 180.f ) / 360.f;
	m_flPoseParameter( )[ BODY_YAW ] = ( blend_ang + 180.f ) / 360.f;
}

void c_base_player::estimate_layers( bool reset ) {
	auto  idx = ce( )->GetIndex( );
	auto& anim_data	   = sm_animdata[ idx ];
	auto  anim_overlay = m_AnimOverlay( ).GetElements( );

	if( reset ) {
		memcpy( anim_data.m_last_layers.data( ), 
			anim_overlay, 
			sizeof( C_AnimationLayer ) * 13 );

		for( size_t i{ }; i < 13; ++i ) {
			auto& layer = anim_overlay[ i ];
			
			float rate = layer.m_flPlaybackRate;
			float rate_1tick = rate * TICK_INTERVAL( );

			float old_cycle = layer.m_flCycle - rate + rate_1tick;
			
			if( old_cycle < 0.f )
				old_cycle += 1.f;
			if( old_cycle > 1.f )
				old_cycle -= 1.f;

			layer.m_flCycle = old_cycle;
		}

		return;
	}

	float cycle[ 13 ]{ };

	for( size_t i{ }; i < 13; ++i ) {
		auto& layer = anim_overlay[ i ];
		auto& last_layer = anim_data.m_last_layers.at( i );

		cycle[ i ] = layer.m_flCycle;

		float last_rate = last_layer.m_flPlaybackRate;
		float last_cycle = last_layer.m_flCycle;

		layer.m_flCycle = last_cycle + last_rate;

		if( layer.m_flCycle < 0.f )
			layer.m_flCycle += 1.f;
		if( layer.m_flCycle > 1.f )
			layer.m_flCycle -= 1.f;
	}

	memcpy( anim_data.m_last_layers.data( ),
		anim_overlay,
		sizeof( C_AnimationLayer ) * 13 );

	for( size_t i{ }; i < 13; ++i )
		anim_data.m_last_layers[ i ].m_flCycle = cycle[ i ];
}

void c_base_player::calc_anim_velocity( bool reset ) {
	int idx = ce( )->GetIndex( );

	if( reset ) {
		vec3_t velocity = m_vecVelocity( );
		sm_animdata[ idx ].m_last_origin = m_vecOrigin( );
		sm_animdata[ idx ].m_last_velocity = velocity;
		sm_animdata[ idx ].m_anim_velocity = velocity;
	}
	else {
		float delta = m_flSimulationTime( ) - m_flOldSimulationTime( );
		delta = std::max( delta, TICK_INTERVAL( ) );

		vec3_t origin = m_vecOrigin( );

		vec3_t origin_delta = origin - sm_animdata[ idx ].m_last_origin;

		vec3_t velocity = origin_delta / delta;
		vec3_t last_velocity = sm_animdata[ idx ].m_last_velocity;

		vec3_t anim_velocity = math::lerp(
			last_velocity,
			velocity,
			TICK_INTERVAL( ) / delta );

		if( anim_velocity.length2d( ) < 3.f ) {
			anim_velocity.x = anim_velocity.y = 0.f;
			velocity = anim_velocity;
		}

		sm_animdata[ idx ].m_anim_velocity = anim_velocity;
		sm_animdata[ idx ].m_last_velocity = velocity;
		sm_animdata[ idx ].m_last_origin = origin;
	}
}

void c_base_player::calc_abs_velocity( ) {
	static auto fn_ptr = pattern::first_code_match( g_csgo.m_chl.dll( ), xors( "55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7 87" ), 0 );
	if( fn_ptr ) {
		auto fn = ( void( __thiscall* )( void* ) )( fn_ptr );
		fn( this );
	}
}

void c_base_player::fix_animations( bool reset, bool resolver_change ) {
	int idx = ce()->GetIndex();

	bool was_valid = sm_animdata[idx].m_valid;
	sm_animdata[idx].m_valid = false;

	if (!get_animstate())
		return;

	m_angEyeAngles().y = std::remainder(m_angEyeAngles().y, 360.f);

	int  flags = m_fFlags();
	auto eye_angles = m_angEyeAngles();
	auto original_duck = m_flDuckAmount();

	float delta = m_flSimulationTime() - m_flOldSimulationTime();

	// enable animations - time to update
	m_bClientSideAnimation() = true;
	set_needs_interpolate(false);

	// we need the player data at THIS EXACT moment, somehow
	float anim_time = m_flSimulationTime();

	auto backup_curtime = g_csgo.m_globals->m_curtime;
	auto backup_frametime = g_csgo.m_globals->m_frametime;

	g_csgo.m_globals->m_curtime = m_flOldSimulationTime() + TICK_INTERVAL();
	g_csgo.m_globals->m_frametime = TICK_INTERVAL();

	/*if( resolver_change && m_fFlags( ) & FL_ONGROUND && !reset ) {
	 float lby_delta = m_flLowerBodyYawTarget( ) - eye_angles.y;
	 lby_delta = std::remainderf( lby_delta, 360.f );
	 lby_delta = std::clamp( lby_delta, -60.f, 60.f );

	 float feet_yaw = std::remainderf( eye_angles.y + lby_delta, 360.f );
	 if( feet_yaw < 0.f )
	  feet_yaw += 360.f;

	 get_animstate( )->m_flGoalFeetYaw = get_animstate( )->m_flCurrentFeetYaw = feet_yaw;
	}*/

	get_animstate()->m_flFeetYawRate = 0.f;

	// why?
	// because this calls pAttachmentHelper->CalcAbsVelocity
	// aswell as removes EFL_DIRTY_ABSVELOCITY
	// which fixes attachments etc
	// normally this would be called by animstate->update
	// but we prevent that
	calc_abs_velocity();

	// calc_anim_velocity( reset );

	set_abs_origin(m_vecOrigin());
	// networked duck amount comes from the last simulation tick instead of the current one
	// calculate_duckamount( reset );
	// fix_jump_fall( reset );
	validate_animation_layers();

	float old_cycle = m_AnimOverlay().GetElements()[6].m_flCycle;
	get_animstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;

	vec3_t velocity = m_vecVelocity();
	// int backup_eflags = get< int >( 0xe4 );
	// int backup_byte = get< byte >( 0x35f8 );

	if (!reset) {
		// m_vecVelocity( ) = sm_animdata[ idx ].m_anim_velocity;
		// get< vec3_t >( 0x94 ) = sm_animdata[ idx ].m_anim_velocity;
		// get< byte >( 0x35f8 ) = 1;
		// get< int >( 0xe4 ) &= ~0x1000;

		// get_animstate( )->m_flUnknownFraction = sm_animdata[ idx ].m_fraction;
	}

	sm_animdata[idx].m_last_duck = m_flDuckAmount();

	float prev_cycle = m_AnimOverlay().GetElements()[6].m_flPrevCycle;
	float prev_rate = m_AnimOverlay().GetElements()[6].m_flPlaybackRate;

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

	C_AnimationLayer prev_layers[13];
	memcpy(prev_layers, m_AnimOverlay().GetElements(), sizeof(prev_layers));

	sm_animdata[idx].m_last_animtime = get_animstate()->m_flLastClientSideAnimationUpdateTime;
	// m_AnimOverlay( ).GetElements( )[ 6 ].m_flPrevCycle = m_AnimOverlay( ).GetElements( )[ 6
	// ].m_flCycle;

	g_csgo.m_globals->m_curtime += delta;
	for (int i = 0; i < TIME_TO_TICKS(delta); ++i) {
		get_animstate()->m_flFeetYawRate = 0.f;
		get_animstate()->update(eye_angles.y, eye_angles.x);

		// ADVANCE THE FRAME
		g_csgo.m_globals->m_curtime += TICK_INTERVAL();
	}

	if (was_valid && !reset) {
		float lerp_rate =
			math::lerp(prev_rate,
				m_AnimOverlay().GetElements()[6].m_flPlaybackRate,
				TICK_INTERVAL() / (m_flSimulationTime() - m_flOldSimulationTime()));

		// m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = prev_cycle + lerp_rate;
	}
	else if (!reset) {
		float cycle = m_AnimOverlay().GetElements()[6].m_flCycle;
		float rate = m_AnimOverlay().GetElements()[6].m_flPlaybackRate;

		float est_cycle = cycle - rate +
			(rate * TICK_INTERVAL() / (m_flSimulationTime() - m_flOldSimulationTime()));

		// m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = est_cycle;
	}

	// m_AnimOverlay( ).GetElements( )[ 6 ].m_flCycle = std::fmod( m_AnimOverlay( ).GetElements( )[
	// 6 ].m_flCycle, 1.0f );

	get_animdata().m_prev_flags = flags;
	memcpy(get_animdata().m_last_layers.data(),
		m_AnimOverlay().GetElements(),
		sizeof(C_AnimationLayer) * 13);

	sm_animdata[idx].m_fraction = get_animstate()->m_flUnknownFraction;

	// m_vecVelocity( ) = velocity;
	// get< vec3_t >( 0x94 ) = velocity;
	// get< int >( 0xe4 ) = backup_eflags;
	// get< byte >( 0x35f8 ) = backup_byte;

	// m_AnimOverlay( ).GetElements( )[ 6 ].m_flPrevCycle = old_cycle;

	// check for any possible mistakes
	validate_animation_layers();

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

	get_animdata().m_anim_flags = m_fFlags();

	// if( !moving )
	//	m_AnimOverlay( ).GetElements( )[ 6 ].m_flWeight = 0.f;

	ce()->GetRenderAngles().y = std::remainderf(ce()->GetRenderAngles().y, 360.f);

	m_flDuckAmount() = original_duck;
	m_fFlags() = flags;
	m_angEyeAngles() = eye_angles;
	m_flDuckAmount() = original_duck;

	// get_animstate( )->m_flUnknownFraction = 0.f;

	// clear occlusion for setupbones (pvs fix)
	*(int*)(uintptr_t(this) + 0xa30) = 0;

	byte backup = get< byte >(0x274);
	get< byte >(0x274) = 0;

	invalidate_bone_cache();
	ce()->SetupBones(nullptr, -1, BONE_USED_BY_ANYTHING, anim_time);
	cache_anim_data();

	g_csgo.m_globals->m_curtime = backup_curtime;
	g_csgo.m_globals->m_frametime = backup_frametime;

	if (!reset) {
		sm_animdata[idx].m_valid = true;
	}

	// disable animations again
	// m_bClientSideAnimation( ) = false;
	get< byte >(0x274) = backup;
}

player_info_t c_base_player::get_info( ) {
	player_info_t info;
	g_csgo.m_engine( )->GetPlayerInfo( ce( )->GetIndex( ), &info );

	return info;
}

bool c_base_player::is_flashed( ) {
	return m_flFlashTime( ) - g_csgo.m_globals->m_curtime > m_flFlashDuration( ) * 0.5f;
}

bool c_base_player::is_reloading( ) {
	auto gun_layer = m_AnimOverlay( ).GetElements( )[ 1 ];
	float playback_rate = gun_layer.m_flPlaybackRate;
	if( playback_rate < 0.55f ) {
		return gun_layer.m_flCycle < 0.99f;
	}

	return false;
}

void c_base_player::get_name_safe( char* buf ) {
	player_info_t info;
	if( g_csgo.m_engine( )->GetPlayerInfo( this->ce( )->GetIndex( ), &info ) ) {
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