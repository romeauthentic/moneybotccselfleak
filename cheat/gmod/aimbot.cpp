
#include "aimbot.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "math.hpp"
#include "base_cheat.hpp"
#include "input_system.hpp"

namespace features
{
	void c_aimbot::operator()( user_cmd_t* cmd ) {
		m_cmd = cmd;
		run( );
	}

	c_aimbot::aim_target_t c_aimbot::find_best_target( ) const {
		aim_target_t ret{ -1, vec3_t{ } };
		vec3_t angle{ };

		g_gmod.m_engine( )->GetViewAngles( angle );

		vec3_t angle_dir  = math::angle_vectors( angle );
		vec3_t eye_pos	  = g_ctx.m_local->get_eye_pos( );
		float  closest_to = std::numeric_limits< float >::max( );

		for( int i{ 1 }; i <= g_gmod.m_globals->m_maxclients; ++i ) {
			auto ent = g_gmod.m_entlist( )->GetClientEntity( i );
			if( !ent || ent == g_ctx.m_local || !ent->is_valid( ) )
				continue;

			if( g_settings.rage.ignore_team &&
				g_ctx.m_local->m_iTeamNum( ) == ent->m_iTeamNum( ) )
				continue;

			if( g_settings.rage.ignore_teamcolor &&
				g_ctx.m_local->get_team_color( ) == ent->get_team_color( ) )
				continue;

			if( g_settings.rage.ignore_spawnprot &&
				ent->m_clrRender( ).a( ) < 250 )
				continue;

			player_info_t info;
			if( !g_gmod.m_engine( )->GetPlayerInfo( i, &info ) )
				continue;

			// whitelisted.
			if( g_cheat.m_playerlist.is_friend( info.m_steamid ) )
				continue;
	
			const auto ent_pos = get_entity_position( ent );
			if( ent_pos.is_zero( ) )
				continue;

			auto dir = ( ent_pos - eye_pos );
			dir.normalize_vector( );

			const float fov = angle_dir.fov_to( dir ) * M_RADPI;
			if( fov > g_settings.rage.fov )
				continue;

			if( g_settings.rage.selection_type( ) == 0 ) {
				if( fov < closest_to ) {
					closest_to = fov;
					ret.m_ent_index = i;
					ret.m_position = ent_pos;
				}
			}
			else if( g_settings.rage.selection_type( ) == 1 ) {
				float dist = eye_pos.dist_to( ent_pos );
				if( dist < closest_to ) {
					closest_to = dist;
					ret.m_ent_index = i;
					ret.m_position = ent_pos;
				}
			}
		}
		return ret;
	}

	vec3_t c_aimbot::get_entity_position( c_base_player* ent ) const {
		matrix3x4 matrix[ 256 ]{ };
		bool found_valid_hitbox{ };
		// the idea behind this is that some players can have really weird models and our aimbot will not work on them
		// so if they don't have a valid hitbox, we then get the OOB center of their model and use that instead.
		// fuck gmod.
		// const const const const
		bool hitscan = g_settings.rage.hitbox == -1;
		const char* hitbox_selection[ ] = {
			"Head",
			"Neck",
			"Pelvis",
			"Chest"
		};


		if( !ent->ce( )->SetupBones( matrix, 256, 0x100, g_gmod.m_globals->m_curtime ) )
			return vec3_t{ };

		const auto model = ent->ce( )->GetModel( );
		if( !model )
			return vec3_t{ };

		const auto hdr = g_gmod.m_model_info( )->GetStudiomodel( model );
		if( !hdr )
			return vec3_t{ };

		const auto set = hdr->get_hitbox_set( ent->m_nHitboxSet( ) );
		if( !set )
			return vec3_t{ };

		for( int i{ }; i < set->numhitboxes; ++i ) {
			const auto hitbox = set->get_hitbox( i );
			if( !hitbox )
				continue;

			const auto bone = hdr->get_bone( hitbox->bone );
			if( !bone )
				continue;

			const std::string name = bone->get_name( );
			if( name.empty( ) )
				continue;

			if( hitscan ) {
				if( name.find( "Head" ) == std::string::npos )
					continue;
			}
			else {
				if( name.find( hitbox_selection[ g_settings.rage.hitbox ] ) == std::string::npos ) 
					continue;
			}

			const auto pos = math::vector_transform( ( hitbox->min + hitbox->max ) * 0.5f, matrix[ hitbox->bone ] );
			if( pos.is_zero( ) )
				continue;

			found_valid_hitbox = true;
			if( ent->is_visible( pos ) )
				return pos;
		}

		if( hitscan ) { // ghetto but it works fine.
			for( int i{ }; i < set->numhitboxes; ++i ) {
				const auto hitbox = set->get_hitbox( i );
				if( !hitbox )
					continue;

				const auto pos = math::vector_transform( ( hitbox->min + hitbox->max ) * 0.5f, matrix[ hitbox->bone ] );
				if( pos.is_zero( ) )
					continue;

				if( ent->is_visible( pos ) )
					return pos;
			}
		}

		if( !found_valid_hitbox ) {
			auto pos = ent->ce( )->GetRenderOrigin( );
			pos += ( ent->m_vecMins( ) + ent->m_vecMaxs( ) ) * 0.5f;
			if( ent->is_visible( pos ) )
				return pos;
		}
		
		return vec3_t{ };
	}

	void c_aimbot::fix_accuracy( vec3_t& angle ) {
		auto srand = []( double seed ) -> void {
			if( !g_ctx.m_lua )
				return;

			const auto lua = g_ctx.m_lua;

			lua->GetField( LUA_GLOBALSINDEX, xors( "math" ) );
			if( !lua->IsType( -1, LUA_TYPE::TYPE_TABLE ) ) {
				lua->Pop( 1 );
				return;
			}

			lua->GetField( -1, xors( "randomseed" ) );
			if( !lua->IsType( -1, LUA_TYPE::TYPE_FUNCTION ) ) {
				lua->Pop( 2 );
				return;
			}

			lua->PushNumber( seed );
			lua->Call( 1, 0 );

			lua->Pop( 1 );
		};

		auto rand = []( const double low, const double high ) -> double {
			if( !g_ctx.m_lua )
				return 0.0;

			const auto lua = g_ctx.m_lua;

			lua->GetField( LUA_GLOBALSINDEX, xors( "math" ) );
			if( !lua->IsType( -1, LUA_TYPE::TYPE_TABLE ) ) {
				lua->Pop( 1 );
				return 0.0;
			}

			lua->GetField( -1, xors( "Rand" ) );
			if( !lua->IsType( -1, LUA_TYPE::TYPE_FUNCTION ) ) {
				lua->Pop( 2 );
				return 0.0;
			}

			lua->PushNumber( low );
			lua->PushNumber( high );

			lua->Call( 2, 1 );
			if( !lua->IsType( -1, LUA_TYPE::TYPE_NUMBER ) ) {
				lua->Pop( 2 );
				return 0.0;
			}

			double rand = lua->GetNumber( -1 );
			
			lua->Pop( 2 );

			return rand;
		};	
		

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon )
			return;

		bool is_cw20 = weapon->is_cw20( ); // own spread handling weapon packs (big).
		bool is_fas2 = weapon->is_fas2( );
		bool is_custom = is_cw20 || is_fas2;

		auto weapon_name = weapon->get_class_name( );
		if( !is_custom && !weapon_name )
			return;

		auto cone = m_spread[ weapon_name ];
		if( !is_custom && cone.is_zero( ) )
			return;

		float cur_cone = weapon->get_custom_cone( );
		if( is_custom || cur_cone ) { // may be custom weapon similar to FAS2/CW20
			if ( g_settings.rage.norecoil )
				angle -= g_ctx.m_local->m_vecPunchAngle( );

			if( !g_settings.rage.nospread )
				return;

			if( is_cw20 ) {
				srand( m_cmd->m_cmd_nr );

				if( g_ctx.m_local->m_fFlags( ) & FL_DUCKING )
					cur_cone *= 0.85f;
			}
			else if ( is_fas2 ) {
				srand( g_ctx.m_local->m_nTickBase( ) * g_gmod.m_globals->m_interval_per_tick ); // FPS has to be over tickrate otherwise this fucks up, do not know how to fix.
			}
			else {
				srand( g_ctx.m_local->m_nTickBase( ) * g_gmod.m_globals->m_interval_per_tick ); // some other custom ones based on FAS2 or CW20 use this.
			}
			
			angle.x -= ( float )( rand( -cur_cone, cur_cone ) * 25.0 );
			angle.y -= ( float )( rand( -cur_cone, cur_cone ) * 25.0 );
		}
		else {
			// if hl2
			// no recoil... figure this out.

			if( !g_settings.rage.nospread )
				return;

			util::set_random_seed( m_cmd->m_random_seed & 0xff );

			float rand_a = util::get_random_float( -0.5f, 0.5f ) + util::get_random_float( -0.5f, 0.5f );
			float rand_b = util::get_random_float( -0.5f, 0.5f ) + util::get_random_float( -0.5f, 0.5f );

			float spread_x = cone.x * rand_a;
			float spread_y = cone.y * rand_b;

			vec3_t forward, right, up;
			math::angle_vectors( angle, &forward, &right, &up );

			vec3_t spread_dir = forward + ( right * -spread_x ) + ( up * -spread_y );
			spread_dir.normalize_vector( );

			angle = math::vector_angles( vec3_t{ }, spread_dir );
		}
	}

	void c_aimbot::aim_at_target( const c_aimbot::aim_target_t& target ) {
		// we're not gonna be checking for m_flNextPrimaryFire due to server side anticheats checking if IN_ATTACK is sent when you can fire, you will get banned.
		// I'm gonna do some ghetto autopistol if we find that the weapon is not automatic (we can find this easily using a lua member of the weapon).
		vec3_t eye_pos		   = g_ctx.m_local->get_eye_pos( );
		vec3_t angle_to_target = math::vector_angles( eye_pos, target.m_position );
		auto weapon			   = g_ctx.m_local->get_weapon( );
		float yaw			   = m_cmd->m_viewangles.y;
		
		fix_accuracy( angle_to_target );

		if( g_settings.rage.silent != 2 || weapon->is_fas2( ) || weapon->is_cw20( ) || weapon->get_custom_cone( ) ) { // These custom weapons cannot do pSilent as they handle shot direction themselves... should probably warn users about this.
			m_cmd->m_viewangles = angle_to_target.clamp( );

			vec3_t delta = m_last_viewangles - ( m_cmd->m_viewangles - m_last_viewangles );
			m_cmd->m_mousedx = -( short )( delta.x / 0.022f ); // serverside anticheats detect this stuff.
			m_cmd->m_mousedy = ( short )( delta.y / 0.022f );

			if( g_settings.rage.silent == 1 ) {
				vec3_t move = vec3_t( m_cmd->m_forwardmove, m_cmd->m_sidemove, m_cmd->m_upmove );
				float len = move.length( );

				if( len ) {
					move = math::angle_vectors( ( math::vector_angles( vec3_t{ }, move ) + vec3_t( 0, m_cmd->m_viewangles.y - yaw, 0 ) ) ) * len;
					m_cmd->m_forwardmove = move.x;
					m_cmd->m_sidemove = move.y;
				}
			}
		}
		else {
			m_shot_dir = math::angle_vectors( angle_to_target.clamp( ) ); // save for later. this is really inaccurate and i do not know why.
			m_shot_dir.normalize_vector( );
		}

		if( g_settings.rage.silent( ) == 0 )
			g_gmod.m_engine( )->SetViewAngles( m_cmd->m_viewangles );

		if( g_settings.rage.auto_fire )
			m_cmd->m_buttons |= IN_ATTACK;
	}

	void c_aimbot::log_shot( c_base_player* ent, fire_bullets_info* info ) {
		// we log shot spread cones due to not having a proper function to give us weapon spread...
		if( !ent || !info ) 
			return;

		if( info->spread.is_zero( ) )
			return;

		auto weapon = ent->get_weapon( );
		if( !weapon )
			return;

		auto weapon_name = weapon->get_class_name( );
		if( !weapon_name )
			return;

		m_spread[ weapon_name ] = info->spread;
	}

	void c_aimbot::silent( ) {
		// the idea behind this was using the context menu used in sandbox and darkrp ( C menu )
		// you can fire anywhere in the menu but it doesn't change your view angles
		// worldclick stuff gets set in C_GMOD_Player::CreateMove so we must set it after the call to it.
		// however this method has some problems, the weapon back has to be using the lua call player:GetAimVector() for src direction in FireBullets
		// otherwise it will not work, 2 of the biggest weapon packs do not use this function so its kinda bork
		// another problem is that if you do not aim within a 30 degree cone of your target or are close to them, lag compensation will not take effect ( thanks garry )
		// - kolo

		if( !m_cmd )
			return;

		m_last_viewangles = m_cmd->m_viewangles; // for mousedx garbage.

		if( !g_settings.rage.enabled )
			return;

		if( g_settings.rage.silent != 2 )
			return;

		if( m_shot_dir.is_zero( ) )
			return;

		m_cmd->m_world_clicking = true;
		m_cmd->m_world_click_direction = m_shot_dir;

		m_shot_dir = vec3_t{ }; // useless now.
	}

	void c_aimbot::run( ) {
		if( !g_settings.rage.enabled )
			return;

		if( g_settings.rage.activation_type != 0 && 
			!g_input.is_key_pressed( ( VirtualKeys_t )g_settings.rage.aimkey( ) ) )
			return;

		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon )
			return;

		//auto wpn_data = weapon->get_wpn_data( ); // only works for hl2 weapons..
		//if( !wpn_data )						   // however you can get lua weapon data, cast to C_WeaponSWEP and call correct functions.
		//	return;

		auto target = find_best_target( );
		if( target.m_ent_index != -1 ) {
			aim_at_target( target );
		}
	}
}

