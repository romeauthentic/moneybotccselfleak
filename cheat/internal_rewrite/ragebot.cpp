#include "ragebot.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "math.hpp"
#include "base_cheat.hpp"
#include "input_system.hpp"

namespace features
{
	c_ragebot::c_ragebot( ) :
		m_antiaim( std::make_shared< c_antiaim >( ) ),
		m_lagcomp( std::make_shared< c_lagcomp >( ) ),
		m_resolver( std::make_shared< c_resolver >( ) ) { }

	void c_ragebot::operator()( user_cmd_t* cmd ) {
		if( !cmd ) return;
		m_cmd = cmd;

		m_antiaim->run( cmd );
		run( );
		m_antiaim->fix_movement( );
	}

	vec3_t head_scale( c_base_player* ent, vec3_t pos ) {

		// broke, crashes the hack
		//not what it is
		//return pos;

		vec3_t eye_pos = g_ctx.m_local->get_eye_pos( );
		CTraceFilterOneEntity filter;
		Ray_t ray;
		CGameTrace trace;

		filter.ent = ent;

		float height{ };
		float highest_dmg{ };
		float max_height{ };

		const float max = 6.5f * g_settings.rage.active->m_hitbox_scale;

		for( float i = 0.f; i < max; i += 0.25f ) {
			vec3_t end = pos;
			end.z += i;

			ray.Init( eye_pos, end );
			g_csgo.m_trace( )->TraceRay( ray, 0x46004003, &filter, &trace );

			if( trace.m_pEnt == ent->ce( ) ) {
				max_height = i;
				if( trace.hitgroup == HITGROUP_HEAD && !height ) {
					height = i;
				}
			}
		}

		if( height < 0.5f ) {
			pos.z += height;
			return pos;
		}

		pos.z += ( height + max_height ) * 0.5f;
		return pos;
	}

	int c_ragebot::get_min_dmg( c_base_player* ent ) {
		int hp = ent->m_iHealth( );
		int dmg = g_settings.rage.active->m_damage( );
		int scale = g_settings.rage.active->m_damage_scale( );
		return std::min< int >( ( int )( hp * scale * 0.01f ) + dmg, 100 );
	}

	vec3_t c_ragebot::multipoint( c_base_player* ent, int hitbox, float* out_dmg ) {
		auto should_multipoint = [ ]( int hitbox, bool moving ) -> bool {
			if( !g_settings.rage.multipoint_enable )
				return false;

			auto& setting = g_settings.rage.multipoint;

			switch( hitbox ) {
			case HITBOX_HEAD:
				return setting.head;
			case HITBOX_PELVIS:
			case HITBOX_BODY:
				return setting.stomach;
				//case HITBOX_CHEST:
			case HITBOX_UPPER_CHEST:
			case HITBOX_THORAX:
				return setting.chest;
			case HITBOX_RIGHT_THIGH:
			case HITBOX_LEFT_THIGH:
				if( moving && g_settings.rage.ignore_limbs_moving )
					return false;

				if( g_settings.rage.preserve_fps && util::is_low_fps( ) )
					return false;

				return setting.thighs;

			case HITBOX_LEFT_CALF:
			case HITBOX_RIGHT_CALF:
				if( moving && g_settings.rage.ignore_limbs_moving )
					return false;

				if( g_settings.rage.preserve_fps && util::is_low_fps( ) )
					return false;

				return setting.calves;
			default:
				return false;
			}
		};

		bool moving = ent->m_vecVelocity( ).length2d( ) > 0.1f && !ent->is_fakewalking( );

		vec3_t pos = ent->get_hitbox_pos( hitbox );
		if( hitbox == HITBOX_HEAD )
			pos = head_scale( ent, pos );

		float dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, pos );
		if( dmg > get_min_dmg( ent ) ) {
			*out_dmg = dmg;
			return pos;
		}

		if( should_multipoint( hitbox, moving ) ) {
			const auto model = ent->ce( )->GetModel( );
			if( !model ) return vec3_t{ };

			auto hdr = g_csgo.m_model_info( )->GetStudiomodel( model );
			if( !hdr ) return vec3_t{ };

			matrix3x4 bone_matrix[ 128 ];
			if( !ent->ce( )->SetupBones( bone_matrix, 128, 0x100, g_csgo.m_globals->m_curtime ) ) {
				return vec3_t{ };
			}

			auto set = hdr->pHitboxSet( ent->m_nHitboxSet( ) );
			if( !set ) return vec3_t{ };

			//literally 20000 iq, the best multipoint
			//im an actual fucking retard jesus christ
			auto box = set->pHitbox( hitbox );
			if( !box ) return vec3_t{ };

			vec3_t center = ( box->bbmax + box->bbmin ) * 0.5f;

			float dist = box->m_flRadius;

			if( box->m_flRadius == -1.f )
				dist = center.dist_to( box->bbmin ) * 0.85f;
			vec3_t min_dir = math::angle_vectors( math::vector_angles( center, box->bbmin ) );
			vec3_t min = center + min_dir * dist * g_settings.rage.active->m_hitbox_scale * 1.1f;

			if( box->m_flRadius == -1.f )
				dist = center.dist_to( box->bbmax ) * 0.85f;
			vec3_t max_dir = math::angle_vectors( math::vector_angles( center, box->bbmax ) );
			vec3_t max = center + max_dir * dist * g_settings.rage.active->m_hitbox_scale * 1.1f;

			static int point_index[ 65 ][ HITBOX_MAX ];

			std::vector< vec3_t > points;
			
			if( g_settings.rage.multipoint_enable( ) == 1 ||
				hitbox == HITBOX_LEFT_CALF || hitbox == HITBOX_RIGHT_CALF ||
				hitbox == HITBOX_LEFT_THIGH || hitbox == HITBOX_RIGHT_THIGH ) {
				points.push_back( vec3_t{ min.x, min.y, center.z } );
				points.push_back( vec3_t{ max.x, min.y, center.z } );
				points.push_back( vec3_t{ min.x, max.y, center.z } );
				points.push_back( vec3_t{ max.x, max.y, center.z } );
			}
			else if( g_settings.rage.multipoint_enable( ) == 2 ) {
				points.push_back( vec3_t{ max.x, max.y, max.z } );
				points.push_back( vec3_t{ min.x, max.y, max.z } );
				points.push_back( vec3_t{ max.x, min.y, max.z } );
				points.push_back( vec3_t{ min.x, min.y, max.z } );

				points.push_back( vec3_t{ max.x, max.y, min.z } );
				points.push_back( vec3_t{ min.x, max.y, min.z } );
				points.push_back( vec3_t{ max.x, min.y, min.z } );
				points.push_back( vec3_t{ min.x, min.y, min.z } );
			}
			else if( g_settings.rage.multipoint_enable( ) == 3 ) {
				points.push_back( vec3_t{ min.x, min.y, center.z } );
				points.push_back( vec3_t{ max.x, min.y, center.z } );
				points.push_back( vec3_t{ min.x, max.y, center.z } );
				points.push_back( vec3_t{ max.x, max.y, center.z } );

				points.push_back( vec3_t{ max.x, max.y, max.z } );
				points.push_back( vec3_t{ min.x, max.y, max.z } );
				points.push_back( vec3_t{ max.x, min.y, max.z } );
				points.push_back( vec3_t{ min.x, min.y, max.z } );

				points.push_back( vec3_t{ max.x, max.y, min.z } );
				points.push_back( vec3_t{ min.x, max.y, min.z } );
				points.push_back( vec3_t{ max.x, min.y, min.z } );
				points.push_back( vec3_t{ min.x, min.y, min.z } );
			}

			float min_dmg = get_min_dmg( ent );
			auto& point_idx = point_index[ ent->ce( )->GetIndex( ) ][ hitbox ];

			float percentage = g_settings.rage.multipoint_scale( ) * 0.01f;
			int mul = ( int )( g_csgo.m_globals->m_frametime / TICK_INTERVAL( ) );

			int count = math::min< int >( math::max< int >( mul * points.size( ) * percentage,
				points.size( ) * percentage, 
				1 ), points.size( ) );

			for( int i{ }; i < count; ++i ) {
				point_idx %= points.size( );

				vec3_t point = points[ point_idx ];

				vec3_t trans_point = math::vector_transform( point, bone_matrix[ box->bone ] );
				CTraceFilterOneEntity filter;
				CGameTrace trace;
				Ray_t ray;

				filter.ent = ent;
				ray.Init( g_ctx.m_local->get_eye_pos( ), trans_point );
				g_csgo.m_trace( )->TraceRay( ray, MASK_SHOT, &filter, &trace );

				if( trace.m_pEnt == ent->ce( ) ) {
					float dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, trans_point );
					if( dmg > min_dmg ) {
						*out_dmg = dmg;
						return trans_point;
					}
					else {
						point_idx++;
					}
				}
				else {
					point_idx++;
				}
			}
		}

		return ent->get_hitbox_pos( hitbox );
	}

	int c_ragebot::hitscan( c_base_player* ent, bool baim, vec3_t& out_pos, float& out_dmg ) {
		auto should_histcan = []( int hitbox, bool moving ) -> bool {
			switch ( hitbox ) {
			case HITBOX_HEAD:
			case HITBOX_NECK:
				return g_settings.rage.hitscan.head;
			case HITBOX_PELVIS:
			case HITBOX_BODY:
				return g_settings.rage.hitscan.stomach;
			case HITBOX_CHEST:
			case HITBOX_UPPER_CHEST:
			case HITBOX_THORAX:
				return g_settings.rage.hitscan.chest;
			case HITBOX_RIGHT_HAND:
			case HITBOX_LEFT_HAND:
				if( moving && g_settings.rage.ignore_limbs_moving )
					return false;

				if( g_settings.rage.preserve_fps && util::is_low_fps( ) )
					return false;

				return g_settings.rage.hitscan.arms;
			case HITBOX_RIGHT_THIGH:
			case HITBOX_LEFT_THIGH:
			case HITBOX_LEFT_CALF:
			case HITBOX_RIGHT_CALF:
				if( moving && g_settings.rage.ignore_limbs_moving )
					return false;

				if( g_settings.rage.preserve_fps && util::is_low_fps( ) )
					return false;

				return g_settings.rage.hitscan.legs;

			case HITBOX_RIGHT_FOOT:
			case HITBOX_LEFT_FOOT:
				if( moving )
					return false;

				if( g_settings.rage.preserve_fps && util::is_low_fps( ) )
					return false;
				
				return g_settings.rage.hitscan.feet;
			default:
				return false;
			}
		};

		constexpr std::array< std::pair< PlayerHitboxes_t, int >, 15 > hitbox_order = {
			std::make_pair( HITBOX_HEAD, 3 ),
			
			std::make_pair( HITBOX_PELVIS, 3 ),
			std::make_pair( HITBOX_BODY, 3 ),
			std::make_pair( HITBOX_THORAX, 3 ),
			std::make_pair( HITBOX_CHEST, 3 ),
			std::make_pair( HITBOX_UPPER_CHEST, 3 ),

			std::make_pair( HITBOX_NECK, 2 ),

			std::make_pair( HITBOX_RIGHT_HAND, 2 ),
			std::make_pair( HITBOX_LEFT_HAND, 2 ),

			std::make_pair( HITBOX_RIGHT_THIGH, 2 ),
			std::make_pair( HITBOX_LEFT_THIGH, 2 ),

			std::make_pair( HITBOX_RIGHT_CALF, 1 ),
			std::make_pair( HITBOX_LEFT_CALF , 1 ),

			std::make_pair( HITBOX_RIGHT_FOOT, 0 ),
			std::make_pair( HITBOX_LEFT_FOOT, 0 ),
		};

		bool moving = ent->m_vecVelocity( ).length( ) > 0.1f && !ent->is_fakewalking( );
		float min_dmg = get_min_dmg( ent );

		if( g_settings.rage.hitbox != -1 ) {
			float dmg{ };
			vec3_t pos = multipoint( ent, g_settings.rage.hitbox, &dmg );

			if( dmg > min_dmg && pos ) {
				out_dmg = dmg;
				out_pos = pos;
				return g_settings.rage.hitbox;
			}

			return -1;
		}

		struct hitbox_t {
			float dmg;
			int hitbox;
			int priority;
			vec3_t position;
		};

		std::vector< hitbox_t > hitboxes;

		for( size_t i{ }; i < hitbox_order.size( ); ++i ) {
			auto hitbox = hitbox_order[ i ];
			if( !should_histcan( hitbox.first, moving ) ) {
				continue;
			}

			float dmg{ };
			vec3_t pos = multipoint( ent, hitbox.first, &dmg ); 

			if( pos )
				hitboxes.push_back( { dmg, hitbox.first, hitbox.second, pos } );
		}

		std::sort( hitboxes.begin( ), hitboxes.end( ), [ &ent, &baim ]( hitbox_t& a, hitbox_t& b ) -> bool {
			int hp = ent->m_iHealth( );

			bool a_head = !( a.hitbox == HITBOX_HEAD || a.hitbox == HITBOX_NECK );
			bool b_head = !( b.hitbox == HITBOX_HEAD || b.hitbox == HITBOX_NECK );

			if( a.dmg != b.dmg ) {
				if( a.dmg > ( float )( hp + 10 ) && ( float )( b.dmg > hp + 10 ) ) {
					if( a_head != b_head ) {
						return a_head > b_head;
					}
				}
			}

			if( a_head != b_head && baim )
				return a_head > b_head;

			if( a.priority != b.priority ) {
				return a.priority > b.priority;
			}

			if( a.dmg != b.dmg ) {
				return a.dmg > b.dmg;
			}

			return a.hitbox > b.hitbox;
		} );

		for( auto& it : hitboxes ) {
			if( it.dmg > min_dmg ) {
				out_dmg = it.dmg;
				out_pos = it.position;

				return it.hitbox;
			}
		}

		return -1;
	}

	void c_ragebot::update_settings( ) {
		auto weapon = g_ctx.m_local->get_weapon( );
		if( !weapon ) return;
		int  weapon_id = weapon->m_iItemDefinitionIndex( );

		switch ( weapon_id ) {
		case WEAPON_DEAGLE:
		case WEAPON_REVOLVER:
			g_settings.rage.active = &g_settings.rage.heavy_pistols;
			break;
		case WEAPON_AWP:
		case WEAPON_SSG08:
			g_settings.rage.active = &g_settings.rage.snipers;
			break;
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
			g_settings.rage.active = &g_settings.rage.auto_snipers;
			break;
		default:
			g_settings.rage.active = &g_settings.rage.general;
			break;
		}
	}

	bool c_ragebot::should_baim_entity( c_base_player* ent, bool backtracked ) {
		int index = ent->ce( )->GetIndex( );

		auto weap = g_ctx.m_local->get_weapon( );
		if( weap->m_iItemDefinitionIndex( ) == WEAPON_ZEUS )
			return true;

		if( !g_cheat.m_player_mgr.is_cheater( index ) )
			return false;

		int min_hp = g_settings.rage.active->m_baim_health;
		int min_shots = g_settings.rage.active->m_baim_shots;

		if( m_resolver->get_shots( index ) >= min_shots )
			return true;

		if( ent->m_iHealth( ) <= min_hp )
			return true;

		if( !( ent->m_fFlags( ) & FL_ONGROUND ) && g_settings.rage.active->m_baim_air )
			return true;

		if( ent->m_vecVelocity( ).length2d( ) > 0.1f && g_settings.rage.active->m_baim_fake )
			return true;

		return false;
	}

	void quick_stop( user_cmd_t* cmd ) {
		if( g_cheat.m_ragebot.m_antiaim->is_fakewalking( ) )
			return;

		if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
			return;

		vec3_t vel = g_ctx.m_local->m_vecVelocity( );
		float speed = vel.length2d( );

		if( g_settings.rage.active->m_auto_stop == 1 || g_settings.rage.active->m_auto_stop == 4 ) {
			auto wep = g_ctx.m_local->get_weapon( );
			auto wep_info = wep->get_wpn_info( );

			bool scoped = g_ctx.m_local->m_bIsScoped( );
			float max_vel = scoped ? wep_info->max_speed_alt : wep_info->max_speed;

			if( speed < max_vel / 3 )
				return;
		}

		if( speed < 0.1f ) {
			g_ctx.get_last_cmd( )->m_forwardmove = 0.f;
			g_ctx.get_last_cmd( )->m_sidemove = 0.f;
			return;
		}

		static auto sv_accelerate = g_csgo.m_cvar( )->FindVar( xors( "sv_accelerate" ) );
		float accel = sv_accelerate->get_float( );
		float max_speed = g_ctx.m_local->get_weapon( )->get_wpn_info( )->max_speed;
		if( g_ctx.m_local->get_weapon( )->is_sniper( ) && g_ctx.m_local->m_bIsScoped( ) ) {
			max_speed = g_ctx.m_local->get_weapon( )->get_wpn_info( )->max_speed_alt;
		}

		// thx ida
		max_speed = std::min< float >( max_speed, 250.f );

		if( g_ctx.m_local->m_fFlags( ) & FL_DUCKING ) {
		//	max_speed /= 3.f;
		//	accel /= 3.f;
		}

		float surf_friction = 1.f;
		float max_accelspeed = accel * g_csgo.m_globals->m_interval_per_tick * max_speed * surf_friction;

		float wishspeed{ };

		if( speed - max_accelspeed <= -1.f ) {
			wishspeed = max_accelspeed / ( speed / ( accel * g_csgo.m_globals->m_interval_per_tick ) );
		}
		else {
			wishspeed = max_accelspeed;
		}

		vec3_t ndir = math::vector_angles( vel * -1.f );
		ndir.y = g_csgo.m_engine( )->GetViewAngles( ).y - ndir.y;
		ndir = math::angle_vectors( ndir );

		g_ctx.get_last_cmd( )->m_forwardmove = ndir.x * wishspeed;
		g_ctx.get_last_cmd( )->m_sidemove = ndir.y * wishspeed;
	}

	std::array< int, 65 > c_ragebot::get_sorted_targets( ) {
		typedef struct {
			int index;
			float dist;
			float fov;
			bool moving;
			int hp;
			float simtime;
		} target;

		std::array< target, 65 > targets{ };
		std::array< int, 65 > ret{ };

		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );
		vec3_t cur_ang, dir_vec;
		g_csgo.m_engine( )->GetViewAngles( cur_ang );

		dir_vec = math::angle_vectors( cur_ang );

		for( size_t i{ }; i < 65; ++i ) {
			auto& cur_target = targets.at( i );
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );
			cur_target.index = i;

			if( !ent || ent == g_ctx.m_local || !ent->is_valid( ) ) 
				continue;

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) &&
				!g_settings.rage.friendlies( ) )
				continue;

			if( !ent->has_valid_anim( ) ) 
				continue;

			auto pos = ent->m_vecOrigin( );
			pos.z += 30.f; //eh

			auto dir = ( pos - local_pos );
			dir.normalize_vector( );

			float fov = dir_vec.fov_to( dir ) * M_RADPI; // epic fov fix by friendly....
			
			cur_target.fov = fov;
			cur_target.dist = pos.dist_to( local_pos );
			cur_target.hp = ent->m_iHealth( );
			cur_target.moving = ent->m_vecVelocity( ).length2d( ) > 0.1f
				&& !ent->is_fakewalking( )
				&& ( ent->m_fFlags( ) & FL_ONGROUND );
			cur_target.simtime = ent->m_flSimulationTime( );
		}

		auto sort_fn = [ ]( const target& a, const target& b ) {
			if( g_settings.rage.prefer_low_hp ) {
				if( a.hp != b.hp )
					return a.hp < b.hp;
			}

			if( g_settings.rage.prefer_moving ) {
				if( a.moving != b.moving )
					return a.moving > b.moving;
			}

			if( g_settings.rage.selection_type == 0 ) {
				if( a.fov != b.fov )
					return a.fov < b.fov;
			}
			else if( g_settings.rage.selection_type == 1 ) {
				if( a.dist != b.dist )
					return a.dist < b.dist;
			}
			else if( g_settings.rage.selection_type == 2 ) {
				if( a.simtime != b.simtime )
					return a.simtime > b.simtime;
			}

			return a.index > b.index;
		};

		std::sort( targets.begin( ), targets.end( ), sort_fn );
		for( size_t i{ }; i < 65; ++i ) {
			if( targets[ i ].fov < g_settings.rage.fov ) {
				ret[ i ] = targets[ i ].index;
			}
		}

		return ret;
	}

	bool c_ragebot::can_hit_without_ping( int entity ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity( entity );

		if( !ent )
			return false;

		int min_dmg = get_min_dmg( ent );
		auto check_records = [ &ent, &min_dmg ]( std::deque< lag_record_t >* records ) {
			auto& front = records->front( );
			vec3_t pred_pos = front.m_vecPelvisPos;

			float dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, pred_pos, false );
			if( dmg > min_dmg )
				return true;

			pred_pos = front.m_vecHeadPos;
			dmg = g_cheat.m_autowall.run( g_ctx.m_local, ent, pred_pos, false );
			if( dmg > min_dmg )
				return true;

			return false;
		};

		auto lby_records = m_lagcomp->get_records( entity, RECORD_LBY );
		auto sim_records = m_lagcomp->get_records( entity, RECORD_NORMAL );

		if( lby_records->size( ) ) {
			if( check_records( lby_records ) )
				return true;
		}

		if( sim_records->size( ) ) {
			if( check_records( sim_records ) )
				return true;
		}

		return false;
	}

	int c_ragebot::knifebot( ) {
		auto aim_pos = g_ctx.m_local->get_eye_pos( );

		float dist = FLT_MAX;
		int ideal_ent = 0;
		int ideal_tick = 0;
		int hp = 0;
		vec3_t pos;

		for( size_t i{ }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity( i );

			if( !ent || !ent->is_valid( ) || ent == g_ctx.m_local || !ent->has_valid_anim( ) || ent->m_bGunGameImmunity( ) )
				continue;

			auto newest_record = m_lagcomp->get_newest_valid_record( i );
			if( !newest_record )
				continue;

			if( newest_record->m_vecPelvisPos.dist_to( aim_pos ) < dist ) {
				hp = ent->m_iHealth( );
				ideal_ent = i;
				ideal_tick = newest_record->m_tickcount;
				pos = newest_record->m_vecPelvisPos;

				dist = pos.dist_to( aim_pos );
			}
		}

		if( ideal_ent && dist < 80.f ) {
			int attack = 0;

			if( dist < 65.f )
				attack = IN_ATTACK2;
			else if( hp < 35 )
				attack = IN_ATTACK;
			else
				return -1;

			vec3_t ang = math::vector_angles( aim_pos, pos );
			float fov = ang.dist_to( g_csgo.m_engine( )->GetViewAngles( ) );

			if( fov < g_settings.rage.fov ) {
				m_cmd->m_viewangles = math::vector_angles( aim_pos, pos );
				m_cmd->m_tick_count = ideal_tick;
				m_cmd->m_buttons |= attack;

				return ideal_ent;
			}
			else {
				return -1;
			}
		}

		return -1;
	}

	bool c_ragebot::can_hit_target( int it, aim_target_t* ret, bool zeus, bool hitchance ) {
		vec3_t aim_ang;
		vec3_t aim_pos;
		vec3_t local_pos = g_ctx.m_local->get_eye_pos( );

		auto entity = g_csgo.m_entlist( )->GetClientEntity< >( it );
		if( !entity || !entity->is_valid( ) || entity == g_ctx.m_local ) 
			return false;
		if( entity->m_bGunGameImmunity( ) ) 
			return false;
		if( !entity->has_valid_anim( ) ) 
			return false;
		if( entity->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( ) && !g_settings.rage.friendlies )
			return false;

		bool baim = false;
		bool should_baim = true;
		bool backtracked = false;
		bool restore = false;
		float hit_damage = 0.f;
		vec3_t extrapolate{ };

		lag_record_t   original_record( entity );
		lag_record_t*  backtracked_record{ };

		int tick_count = m_cmd->m_tick_count;
		float lerp = util::get_lerptime( );
		auto record_type = m_lagcomp->can_backtrack_entity( it );
		if( !g_settings.rage.resolver( ) ) {
			if( !util::is_tick_valid( entity->m_flSimulationTime( ) + lerp ) ) {
				int new_tickcount = m_lagcomp->backtrack_entity( it,
					record_type, &backtracked_record );

				if( new_tickcount != -1 ) {
					restore = true;
					tick_count = new_tickcount;
				}
			}
		}
		else {
			int new_tickcount = m_lagcomp->backtrack_entity( it, record_type, &backtracked_record );
			if( new_tickcount != -1 ) {
				tick_count = new_tickcount;
				should_baim = record_type != RECORD_LBY;
				backtracked = record_type == RECORD_LBY;
				restore = true;
			}
			else {

				lag_record_t* last_simtime_record = m_lagcomp->get_newest_valid_record( it );
				if( last_simtime_record ) {
					//ENSURE THE MOST ACCURATE DATA
					entity->m_vecVelocity( ) = entity->get_animdata( ).m_last_velocity;

					auto velocity = entity->m_vecVelocity( );
					auto min_vel = 64 / TICK_INTERVAL( ) / 14;

					//IS HE A SLOW BOI OR A FAST NIGGER
					if( g_settings.rage.fakelag_resolver( ) && velocity.length2d( ) > min_vel - 10.f ) {
						auto new_record = *last_simtime_record;

						//FUCK FAKE PINGERS WE IN THIS BITCH
						auto nci = g_csgo.m_engine( )->GetNetChannelInfo( ); //GOD FUCKING KNOWS WHY
						float latency = nci->GetLatency( 0 ) + g_csgo.m_globals->m_frametime;

						auto pred_choke = g_cheat.m_prediction.get_predicted_choke( it );
						auto latency_ticks = TIME_TO_TICKS( latency );

						float time_since_update = g_ctx.pred_time( ) + latency - entity->m_flSimulationTime( );

						//NIGGA UPDATED ALREADY FUCK ADAPTIVE
						if( TIME_TO_TICKS( time_since_update ) > pred_choke && pred_choke > 0 ) {
							if( g_settings.rage.fakelag_resolver( ) == 1 )
								return false;

							else if( latency_ticks > 0 ) {
								int update_count = TIME_TO_TICKS( time_since_update ) / pred_choke;

								vec3_t extrapolated_origin = g_cheat.m_prediction.full_walk_move( entity, update_count * pred_choke );
								new_record.m_vecRenderOrigin = extrapolated_origin;
								new_record.m_tickcount += update_count * pred_choke;
							}
						}

						//if( !new_record.is_valid( ) )
						//	continue;

						int shots = m_resolver->get_shots( it );
						bool recalc = shots != last_simtime_record->m_shots;

						new_record.restore( true, recalc );
						tick_count = new_record.m_tickcount;
						backtracked_record = last_simtime_record;
						restore = true;
					}
					else {
						int shots = m_resolver->get_shots( it );
						bool recalc = shots != last_simtime_record->m_shots;

						last_simtime_record->restore( recalc, recalc );
						tick_count = last_simtime_record->m_tickcount;
						backtracked_record = last_simtime_record;
						restore = true;
					}
				}
				else {
					//fake ping, just wait for an update
					return false;
				}
			}
		}

		baim = should_baim_entity( entity, !should_baim );

		auto restore_player = [ &original_record, &restore, &entity, this ]( ) {
			original_record.restore( true );
			//entity->m_flLastBoneSetupTime( ) = std::numeric_limits< float >::quiet_NaN( );
		};

		int hitbox = hitscan( entity, baim, aim_pos, hit_damage );
		if( hitbox == -1 ) {
			restore_player( );
			return false;
		}

		if( aim_pos.dist_to( local_pos ) > 150.f && zeus ) {
			restore_player( );
			return false;
		}

		aim_ang = math::vector_angles( local_pos, aim_pos ).clamp( );

		if( g_settings.rage.active->m_auto_stop( ) == 2 )
			quick_stop( m_cmd );
		else if( g_settings.rage.active->m_auto_stop( ) == 3 ) {
			m_antiaim->run_fakewalk( );
		}

		if( g_settings.rage.active->m_auto_stop( ) == 5 ) {
			m_cmd->m_buttons |= IN_DUCK;
		}

		if( hitchance && !util::hitchance( it, aim_ang,
			g_settings.rage.active->m_hitchance, hitbox )  ) {
			if( g_settings.rage.active->m_auto_stop( ) == 1 || g_settings.rage.active->m_auto_stop( ) == 4 )
				quick_stop( m_cmd );

			if( g_settings.rage.active->m_auto_stop( ) == 5 ) {
				if( g_ctx.m_local->m_flDuckAmount( ) < 1.f )
					quick_stop( m_cmd );
			}

			if( g_settings.rage.active->m_auto_stop( ) == 4 && g_cheat.m_lagmgr.get_choked( ) < 10 && !m_antiaim->is_fakewalking( ) )
				g_cheat.m_lagmgr.set_state( false );

			if( g_settings.rage.auto_scope( ) && g_ctx.m_local->get_weapon( )->is_sniper( ) ) {
				if( !g_ctx.m_local->m_bIsScoped( ) ) {
					m_cmd->m_buttons |= IN_ATTACK2;
				}
			}

			restore_player( );
			return false;
		}

		ret->m_ent_index = it;
		ret->m_position = aim_pos;
		ret->m_tick_count = tick_count;
		ret->m_hitbox = hitbox;
		ret->m_backtracked = backtracked;
		ret->m_min = entity->get_hitbox_mins( hitbox );
		ret->m_max = entity->get_hitbox_maxs( hitbox );
		ret->m_radius = entity->get_hitbox_radius( hitbox );

		memcpy( m_shot_matrix, entity->m_CachedBoneData( ).GetElements( ), sizeof( matrix3x4 ) * entity->m_CachedBoneData( ).GetSize( ) );

		m_shot_target = it;

		restore_player( );

		return true;
	}

	c_ragebot::aim_target_t c_ragebot::find_best_target( ) {
		aim_target_t ret{ -1, m_cmd->m_tick_count, -1, vec3_t{ }, vec3_t{ }, vec3_t{ }, 0.f, 0 };
		vec3_t	aim_angle{ };
		vec3_t	aim_pos{ };
		auto	weapon_info = g_ctx.m_local->get_weapon( )->get_wpn_info( );
		bool	is_zeus = g_ctx.m_local->get_weapon( )->m_iItemDefinitionIndex( ) == WEAPON_ZEUS;
		int		damage = weapon_info->damage;

		auto players = get_sorted_targets( );
		auto data = players.data( );

		for( size_t i{ }; i < players.size( ); ++i ) {
			if( can_hit_target( data[ i ], &ret, is_zeus ) )
				break;
		}

		return ret;
	}

	void c_ragebot::set_angles( const vec3_t& angles ) {
		auto weapon = g_ctx.m_local->get_weapon( );

		// TODO: Implement the get_spread_direction here I guess.
		if( g_settings.rage.compensate_spread ) {
			int seed = m_cmd->m_random_seed;

			float inaccuracy = g_ctx.m_weapon_inaccuracy;
			float spread = g_ctx.m_weapon_spread;

			util::set_random_seed( ( seed & 0xff ) + 1 );

			float rand_a = util::get_random_float( 0.0f, 1.0f );
			float pi_rand_a = util::get_random_float( 0.f, 2.f * M_PI );
			float rand_b = util::get_random_float( 0.f, 1.0f );
			float pi_rand_b = util::get_random_float( 0.f, 2.f * M_PI );

			float spread_x = cos( pi_rand_a ) * ( rand_a * inaccuracy ) + cos( pi_rand_b ) * ( rand_b * spread );
			float spread_y = sin( pi_rand_a ) * ( rand_a * inaccuracy ) + sin( pi_rand_b ) * ( rand_b * spread );

			vec3_t forward, right, up;
			math::angle_vectors( angles, &forward, &right, &up );

			vec3_t spread_dir = forward + ( right * -spread_x ) + ( up * -spread_y );
			spread_dir.normalize_vector( );

			vec3_t comp = math::vector_angles( vec3_t( ), spread_dir );
			
			m_cmd->m_viewangles = comp;
		}
		else {
			m_cmd->m_viewangles = angles;
		}

		//if( g_settings.rage.silent == 2 ) {
		//	if( !m_antiaim->is_fakewalking( ) )
		//		g_cheat.m_lagmgr.set_state( g_cheat.m_lagmgr.get_choked( ) );
		//}
		
		if( g_settings.rage.silent == 1 && !m_antiaim->is_fakewalking( ) ) {
			//if( weapon->m_iItemDefinitionIndex( ) != WEAPON_R8REVOLVER )
				g_cheat.m_lagmgr.set_state( !g_cheat.m_lagmgr.get_sent( ) );
		}

		if( !g_settings.rage.silent( ) ) {
			g_csgo.m_engine( )->SetViewAngles( m_cmd->m_viewangles );
		}
	}

	void c_ragebot::aim_at_target( const aim_target_t& target ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( target.m_ent_index );
		if( !ent )	
			return;

		if( g_ctx.m_local->get_weapon( )->m_iItemDefinitionIndex( ) != WEAPON_REVOLVER ) {
			if( g_settings.rage.active->m_delay_type == 2 ) {
				static auto time_to_fire = 0.f;
				const float delay = float( g_settings.rage.active->m_delay( ) ) * 0.001f;
				const float time = g_ctx.pred_time( );

				float delta = time_to_fire - time;

				if( delta > delay ) {
					time_to_fire = time + delay;
				}

				if( delta < 0.f ) {
					time_to_fire = time + delay;
					delta = delay;
				}

				if( delta > g_csgo.m_globals->m_interval_per_tick ) {
					return;
				}
			}
			if( g_settings.rage.active->m_delay_type == 3 ) {
				static float time_to_fire = 0.f;
				const float time = g_ctx.pred_time( );

				float delta = time_to_fire - time;

				vec3_t velocity = ent->m_vecVelocity( );
				float speed = velocity.length2d( );
				if( speed > g_ctx.m_local->m_vecVelocity( ).length2d( ) && speed > 100.f ) {
					vec3_t head_pos = ent->get_hitbox_pos( 0 );
					vec3_t origin = ent->m_vecOrigin( );
					origin.z = head_pos.z = 0.f;

					float dist = origin.dist_to( head_pos ); //1 / 64
					auto delay = dist / speed + 0.015625f;

					if( delta > delay )
						time_to_fire = time + delay;

					if( delta < 0.f ) {
						time_to_fire = time + delay;
						delta = delay;
					}

					if( delta > g_csgo.m_globals->m_interval_per_tick )
						return;
				}
			}
		}

		if( g_settings.rage.active->m_auto_stop && !m_antiaim->is_fakewalking( ) && !g_settings.rage.compensate_spread )
			m_cmd->m_forwardmove = m_cmd->m_sidemove = 0.f;

		m_cmd->m_buttons |= IN_ATTACK;
		m_cmd->m_buttons &= ~IN_USE;

		vec3_t position = target.m_position;
		m_cmd->m_tick_count = target.m_tick_count;
	
		auto angle = math::vector_angles(
			g_ctx.m_local->get_eye_pos( ),
			target.m_position );

		static auto weapon_recoil_scale = g_csgo.m_cvar( )->FindVar( xors( "weapon_recoil_scale" ) );
		angle -= g_ctx.m_local->m_aimPunchAngle( ) * weapon_recoil_scale->get_float( );

		g_cheat.m_visuals.store_shot( target.m_position );

		if( !target.m_backtracked && g_cheat.m_player_mgr.is_cheater( target.m_ent_index ) ) {
			m_resolver->aimbot( 
				target.m_ent_index, 
				target.m_hitbox, angle, 
				target.m_position,
				target.m_min,
				target.m_max,
				target.m_radius );
		}
		else {
			context::shot_data_t new_shot{ };
			new_shot.m_angle = angle;
			new_shot.m_enemy_index = target.m_ent_index;
			
			new_shot.m_hitbox.max = target.m_min;
			new_shot.m_hitbox.min = target.m_max;
			new_shot.m_hitbox.radius = target.m_radius;
			new_shot.m_hitgroup = target.m_hitbox;

			new_shot.m_enemy_pos = target.m_position;

			g_ctx.m_last_shot++;
			g_ctx.m_last_shot %= 128;

			g_ctx.m_shot_data[ g_ctx.m_last_shot ] = new_shot;
			g_ctx.m_has_incremented_shots = true;
		}

		memcpy( g_ctx.m_shot_data[ g_ctx.m_last_shot ].m_matrix,
			m_shot_matrix,
			sizeof( matrix3x4 ) * ent->m_CachedBoneData( ).GetSize( ) );

		set_angles( angle.clamp( ) );

		if( g_settings.rage.active->m_auto_stop && !g_settings.rage.compensate_spread( ) )
			quick_stop( m_cmd );
	}

	void c_ragebot::run( ) {
		if( !g_settings.rage.enabled( ) || !g_ctx.run_frame( ) || !g_ctx.m_local->is_valid( ) ) {
			m_target = -1;
			return;
		}

		update_settings( );

		if( m_send_next ) {
			if( !g_cheat.m_lagmgr.get_sent( ) && !m_antiaim->is_fakewalking( ) )
				g_cheat.m_lagmgr.set_state( true );

			m_send_next = false;
		}

		auto wep = g_ctx.m_local->get_weapon( );
		if( !wep ) {
			m_target = -1;
			return;
		}

		bool is_revolver = wep->m_iItemDefinitionIndex( ) == WEAPON_REVOLVER;

		bool in_attack = false;

		m_can_fakeping = true;

		if ( g_settings.rage.activation_type == 0 ) in_attack = true;
		else in_attack = g_input.is_key_pressed(
			( VirtualKeys_t )g_settings.rage.aimkey( ) );

		if( !in_attack ) {
			m_target = -1;
			return;
		}

		if( wep->is_knife( ) ) {
			m_target = knifebot( );
			return;
		}

		if( !wep->m_iClip1( ) || wep->is_grenade( ) ) {
			m_target = -1;
			return;
		}

		matrix3x4 backup_bones[ 128 ];
		memcpy( backup_bones, g_ctx.m_local->m_CachedBoneData( ).GetElements( ),
			g_ctx.m_local->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );

		if( g_settings.rage.anti_aim ) {
			float pitch = 0.f;
			if( g_ctx.m_local->get_animstate( )->m_bInHitGroundAnimation && ( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) ) {
				bool dist = g_settings.rage.enabled( ) && g_settings.rage.selection_type( ) == 1;
				int target = util::get_closest_player( dist );

				if( target != -1 ) {
					auto t = g_csgo.m_entlist( )->GetClientEntity( target );

					pitch = math::vector_angles( g_ctx.m_local->get_eye_pos( ), t->get_hitbox_pos( 0 ) ).x;
				}

				g_ctx.m_local->invalidate_bone_cache( );

				g_ctx.m_local->restore_anim_data( true );
				float pose_backup = g_ctx.m_local->m_flPoseParameter( )[ BODY_PITCH ];

				g_ctx.m_local->m_flPoseParameter( )[ BODY_PITCH ] = ( pitch + 90.f ) / 180.f;

				byte backup = g_ctx.m_local->get< byte >( 0x274 );
				g_ctx.m_local->get< byte >( 0x274 ) = 0;
				g_ctx.m_local->ce( )->SetupBones( nullptr, 128, BONE_USED_BY_ANYTHING, 0.f );
				g_ctx.m_local->get< byte >( 0x274 ) = backup;

				g_ctx.m_local->m_flPoseParameter( )[ BODY_PITCH ] = pose_backup;
			}
		}

		if( !g_ctx.m_local->can_attack( ) ) {
			aim_target_t t{ };
			bool is_zeus = wep->m_iItemDefinitionIndex( ) == WEAPON_ZEUS;

			if( m_target != -1 && !m_antiaim->is_fakewalking( ) ) {
				bool can_hit = can_hit_target( m_target, &t, is_zeus, false );
				
				if( !can_hit )
					m_target = -1;

				else if( g_settings.rage.active->m_auto_stop == 3 && can_hit ) {
					m_antiaim->run_fakewalk( );
					//if( !g_cheat.m_lagmgr.get_choked( ) || g_cheat.m_lagmgr.get_state( ) )
					m_cmd->m_viewangles.y = m_antiaim->get_yaw( );
				}
			}
			return;
		}

		// do not
		if( g_settings.rage.silent == 2 && !g_cheat.m_lagmgr.get_choked( ) && !is_revolver ) {
			if( !m_antiaim->is_fakewalking( ) )
				g_cheat.m_lagmgr.set_state( false );
			return;
		}

		auto target = find_best_target( );
		if ( target.m_ent_index != -1 ) {
			aim_at_target( target );
		}

		memcpy( g_ctx.m_local->m_CachedBoneData( ).GetElements( ), backup_bones,
			g_ctx.m_local->m_CachedBoneData( ).GetSize( ) * sizeof( matrix3x4 ) );
		g_ctx.m_local->invalidate_bone_cache( );
		m_target = target.m_ent_index;
	}
}