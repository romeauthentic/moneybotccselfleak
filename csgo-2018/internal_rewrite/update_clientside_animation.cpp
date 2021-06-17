#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"

void __fastcall hooks::update_clientside_animation( void* ecx_, void* edx_ ) {
	static bool first_update = true;

	auto old_func = g_csgo.m_local->get_old_function< decltype( &hooks::update_clientside_animation ) >( 218 );
	auto ent = ( c_base_player* )( ecx_ );

	static ent_animdata_t prev_anims;
	static float last_choke;
	static float last_update;

	if( !g_settings.rage.anti_aim || !g_ctx.m_local->is_valid( ) || std::abs( last_update - g_csgo.m_globals->m_curtime ) > TICKS_TO_TIME( 20 ) ) {
		last_update = g_csgo.m_globals->m_curtime;
		first_update = true;
		return old_func( ecx_, edx_ );
	}

	*( byte* )( uintptr_t( ent ) + 0x270 ) = 1;
	if( !g_cheat.m_lagmgr.get_choked( ) || g_cheat.m_lagmgr.get_sent( ) > 1 || first_update ) {
		vec3_t backup;
		g_csgo.m_prediction( )->GetLocalViewAngles( backup );
		
		float backup_curtime = g_csgo.m_globals->m_curtime;
		float backup_frametime = g_csgo.m_globals->m_frametime;

		g_csgo.m_globals->m_curtime = g_ctx.pred_time( );
		g_csgo.m_globals->m_frametime = TICK_INTERVAL( );

		if( g_csgo.m_globals->m_curtime != ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime ) {
			last_update = backup_curtime;

			last_choke = std::max( g_csgo.m_globals->m_curtime - ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime, TICK_INTERVAL( ) );

			vec3_t real_angle = g_ctx.m_thirdperson_angle;

			float min_pitch = g_ctx.m_local->get_animstate( )->m_flHeadHeightOrOffsetFromHittingGroundAnimation * 25.f;
			if( ( ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) && !g_ctx.m_local->get_animstate( )->m_bOnGround ) || g_ctx.m_local->get_animstate( )->m_bInHitGroundAnimation ) {
				if( real_angle.x > min_pitch ) {
					real_angle.x = -min_pitch;
				}
			}

			g_csgo.m_prediction( )->SetViewAngles( real_angle );
			ent->get_animstate( )->update( real_angle.y, real_angle.x );
			old_func( ecx_, edx_ );

			memcpy( &prev_anims, &ent->get_animdata( ), sizeof( prev_anims ) );
			ent->cache_anim_data( );
		}

		g_csgo.m_prediction( )->SetLocalViewAngles( backup );
		g_csgo.m_globals->m_curtime = backup_curtime;
		g_csgo.m_globals->m_frametime = backup_frametime;
	}

	ent->restore_anim_data( true );
	float yaw = ent->get_animstate( )->m_flGoalFeetYaw;

	//interpolate EVERYTHING
	if( !first_update && !( ent->get_animstate( )->m_bInHitGroundAnimation && ent->get_animstate( )->m_bOnGround ) ) {
		for( size_t i{ }; i < 18; ++i ) {
			float lerp = std::min( last_choke, TICK_INTERVAL( ) * 2.f );
			float update_delta = last_choke;
			float update_lerp = std::clamp( update_delta - lerp, 0.f, 1.f );
			if( update_delta > 0.f )
				lerp = std::clamp( lerp, 0.f, update_delta );

			float lerp_progress = ( last_update + lerp - g_csgo.m_globals->m_curtime ) / lerp;

			float old_param = prev_anims.m_poseparams.at( i );
			float param = ent->get_animdata( ).m_poseparams.at( i );

			if( i == BODY_YAW )
				continue;

			if( !isfinite( old_param ) || !isfinite( param ) )
				continue;

			float jmp_param = math::lerp( old_param, param, update_lerp / update_delta );
			float final_param = math::lerp( param, jmp_param, std::clamp( lerp_progress, 0.f, 1.f ) );

			ent->m_flPoseParameter( )[ i ] = final_param;
		}

		float lerp = std::min( last_choke, TICK_INTERVAL( ) * 2.f );
		float update_delta = last_choke;
		float update_lerp = std::clamp( update_delta - lerp, 0.f, 1.f );
		if( update_delta > 0.f )
			lerp = std::clamp( lerp, 0.f, update_delta );

		for( size_t i{ }; i < 13; ++i ) {
			auto old_cycle = prev_anims.m_animlayers.at( i ).m_flCycle;
			auto cycle = ent->get_animdata( ).m_animlayers.at( i ).m_flCycle;

			float lerp_progress = ( last_update + lerp - g_csgo.m_globals->m_curtime ) / lerp;

			float jmp_cycle = math::lerp( old_cycle, cycle, update_lerp / update_delta );
			float final_cycle = math::lerp( cycle, jmp_cycle, std::clamp( lerp_progress, 0.f, 1.f ) );

			ent->m_AnimOverlay( ).GetElements( )[ i ].m_flCycle = final_cycle;
		}
	}

	if( !first_update ) {
		//*( byte* )( uintptr_t( ent ) + 0x270 ) = 0;
		ent->set_abs_angles( vec3_t( 0, yaw, 0 ) );
		bool backup = ent->get_animstate( )->m_bOnGround;
		ent->get_animstate( )->m_bOnGround = false;

		ent->invalidate_bone_cache( );
		ent->ce( )->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, 0.f );

		ent->get_animstate( )->m_bOnGround = backup;
	}

	first_update = false;
}