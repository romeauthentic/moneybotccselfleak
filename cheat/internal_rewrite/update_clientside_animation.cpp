#include "hooks.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "mem.hpp"
#include "util.hpp"

void __fastcall hooks::update_clientside_animation( void* ecx_, void* edx_ ) {
	static bool first_update = true;
	auto ent = ( c_base_player* )( ecx_ );

	auto old_func = update_clientside_animation_o;

	if( ent != g_ctx.m_local ) {
		if( ent && ent->is_valid( ) && g_ctx.m_local && g_ctx.m_local->is_valid( ) && g_settings.rage.active && g_settings.rage.resolver ) {
			if( ent->m_iTeamNum( ) != g_ctx.m_local->m_iTeamNum( ) || g_settings.rage.friendlies )
				return;
		}

		return old_func( ecx_, edx_ );
	}


	static float last_update;
	static float last_spawntime = 0.f;

	// Arbitrary number much.
	if( !g_settings.rage.anti_aim || !g_ctx.m_local->is_valid( ) || std::abs( last_update - g_csgo.m_globals->m_curtime ) > TICKS_TO_TIME( 20 ) || last_spawntime != ent->m_flSpawnTime( ) ) {
		last_update = g_csgo.m_globals->m_curtime;
		first_update = true;

		if( last_spawntime != ent->m_flSpawnTime( ) )
			last_spawntime = ent->m_flSpawnTime( );

		return old_func( ecx_, edx_ );
	}

	last_update = g_csgo.m_globals->m_curtime;

	//ent->get_animstate( )->m_iLastClientSideAnimationUpdateFramecount = g_csgo.m_globals->m_framecount;

	//float backup_time = ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime;

	//this doesn't work anymore
	//ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime = g_csgo.m_globals->m_curtime;
	
	ent->get_animstate( )->pBaseEntity = nullptr;
	
	old_func( ecx_, edx_ );

	ent->get_animstate( )->pBaseEntity = ent;
	//ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime = backup_time;


	if( !first_update ) {
		g_ctx.m_local->restore_anim_data( true );

		ent->set_abs_angles( vec3_t( 0, g_ctx.m_absyaw, 0 ) );

		ent->invalidate_bone_cache( );

		auto model = ent->ce( )->GetModel( );
		auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
		auto set = hdr->pHitboxSet( ent->m_nHitboxSet( ) );

		if( hdr && set ) {
			for( size_t i{ }; i < hdr->numbones; i++ ) {
				auto bone = hdr->GetBone( i );

				bone->proctype &= ~5;
			}
		}

		bool backup = ent->get_animstate( )->m_bOnGround;
		ent->get_animstate( )->m_bOnGround = false;
		ent->ce( )->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, 0.f );
		ent->get_animstate( )->m_bOnGround = backup;
	}

	first_update = false;
}