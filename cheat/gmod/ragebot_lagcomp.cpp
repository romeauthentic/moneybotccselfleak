#include "ragebot.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "context.hpp"
#include "base_cheat.hpp"
#include "mem.hpp"

namespace features
{
	c_ragebot::lag_record_t::lag_record_t( c_base_player* ent ) : m_ent( ent ) {
		if( !m_ent || !m_ent->is_valid( ) ) return;
		m_valid = true;

		m_flSimulationTime = m_ent->m_flSimulationTime( );
		m_tickcount = TIME_TO_TICKS( m_flSimulationTime + util::get_lerptime( ) );
		m_choked = m_ent->get_choked_ticks( );

		m_flLowerBodyYaw = m_ent->m_flLowerBodyYawTarget( );
		m_vecVelocity = m_ent->m_vecVelocity( );
		m_fFlags = m_ent->m_fFlags( );
		m_flCycle = m_ent->m_flCycle( );
		m_nSequence = m_ent->m_nSequence( );

		m_vecAngles = m_ent->m_angEyeAngles( );
		m_vecOrigin = m_ent->m_vecOrigin( );

		m_vecRenderAngles = m_ent->ce( )->GetRenderAngles( );
		m_vecAbsAngles = *( vec3_t* )( uintptr_t( m_ent ) + 0xc4 );

		m_vecAbsOrigin = *( vec3_t* )( uintptr_t( m_ent ) + 0xa0 );
		m_vecRenderOrigin = m_ent->ce( )->GetRenderOrigin( );

		m_vecHeadPos = ent->get_hitbox_pos( 0 );
		m_vecPelvisPos = ent->get_hitbox_pos( HITBOX_PELVIS );
		m_can_render = ent->ce( )->SetupBones( m_matrix, 128, 0x100, 0.f );

		m_vecMins = *( vec3_t* )( uintptr_t( m_ent ) + 0x318 + 0x8 );
		m_vecMaxs = *( vec3_t* )( uintptr_t( m_ent ) + 0x318 + 0x14 );

		float* pose_params = m_ent->m_flPoseParameter( );
		memcpy( m_PoseParameters,
			pose_params,
			sizeof( float ) * 24 );

		size_t count = std::min< size_t >( m_ent->m_AnimOverlay( ).GetSize( ), 13 );
		memcpy( m_AnimLayers,
			m_ent->m_AnimOverlay( ).GetElements( ),
			sizeof( C_AnimationLayer ) * count );

		memcpy( &m_state,
			m_ent->get_animstate( ),
			sizeof( CCSGOPlayerAnimState ) );
	}

	//this shit doesnt work and im going to lose my fucking mind
	bool c_ragebot::lag_record_t::restore( bool recalculate ) {
		if( !m_ent || !m_ent->is_valid( ) ) return false;

		m_ent->m_flLowerBodyYawTarget( ) = m_flLowerBodyYaw;

		m_ent->m_vecVelocity( ) = m_vecVelocity;
		m_ent->m_flCycle( ) = m_flCycle;
		m_ent->m_nSequence( ) = m_nSequence;

		m_ent->m_angEyeAngles( ) = m_vecAngles;
		m_ent->m_vecOrigin( ) = m_vecOrigin;
		m_ent->m_fFlags( ) = m_fFlags;

		m_ent->ce( )->GetRenderAngles( ) = m_vecRenderAngles;
		m_ent->ce( )->GetRenderOrigin( ) = m_vecRenderOrigin;

		m_ent->set_abs_angles( m_vecAbsAngles );
		m_ent->set_abs_origin( m_vecAbsOrigin );

		*( vec3_t* )( uintptr_t( m_ent ) + 0x318 + 0x8 ) = m_vecMins;
		*( vec3_t* )( uintptr_t( m_ent ) + 0x318 + 0x14 ) = m_vecMaxs;

		memcpy( m_ent->m_flPoseParameter( ),
			m_PoseParameters,
			sizeof( float ) * 24 );
			
		size_t count = std::min< int >( m_ent->m_AnimOverlay( ).GetSize( ), 13 );
		memcpy( m_ent->m_AnimOverlay( ).GetElements( ),
			m_AnimLayers,
			sizeof( C_AnimationLayer ) * count );

		if( recalculate ) {
			m_ent->fix_animations( nullptr );
			//m_ent->invalidate_bone_cache( );
			return m_ent->ce( )->SetupBones( nullptr, -1, 0x100, 0.f );
		}
		else {
			m_ent->invalidate_bone_cache( );
			return true;
		}

		return false;
	}

	bool c_ragebot::lag_record_t::is_valid( ) {
		if( !m_valid ) return false;
		if( m_balanceadjust && m_shot ) return false;

		return util::is_tick_valid( m_tickcount );
	}

	void c_ragebot::c_lagcomp::restore_animations( ) {
		if( g_csgo.m_engine( )->IsInGame( ) ) {
			if( g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
				for( int i{ 1 }; i < 65; ++i ) {
					auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

					if( ent && ent->is_player( ) && ent != g_ctx.m_local ) {
						ent->m_flLastBoneSetupTime( ) = g_csgo.m_globals->m_curtime;
						ent->m_iMostRecentModelBoneCounter( ) = g_csgo.m_globals->m_framecount;
						ent->m_bClientSideAnimation( ) = true;
					}
				}
			}
		}
	}

	void c_ragebot::c_lagcomp::store_visuals( ) {
		if( g_csgo.m_engine( )->IsInGame( ) && g_settings.rage.resolver( ) ) {
			static float last_simtime[ 65 ]{ };

			if( g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
				for( int i{ }; i < 65; ++i ) {
					auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );
					auto& data = m_data_render[ i ];
					
					if( ent && ent->is_valid( ) && !ent->m_bGunGameImmunity( ) && 
						!!std::abs( last_simtime[ i ] - ent->m_flSimulationTime( ) ) ) {
						render_record_t new_record;
						new_record.m_simtime = ent->m_flSimulationTime( ) + util::get_lerptime( );
						new_record.m_origin = ent->m_vecOrigin( );
						if( ent->ce( )->SetupBones( new_record.m_matrix, 128, 0x100, 0.f ) ) {
							data.push_front( new_record );
						}

						last_simtime[ i ] = ent->m_flSimulationTime( );
					}

					while( !data.empty( ) && 
						data.size( ) > TIME_TO_TICKS( 1.f ) ) {
						data.pop_back( );
					}
 				}
			}
		}
	}

	c_ragebot::render_record_t* c_ragebot::c_lagcomp::get_render_record( int ent_index ) {
		auto& data = m_data_render[ ent_index ];
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		for( auto& it : util::reverse_iterator( data ) ) {
			if( util::is_tick_valid( TIME_TO_TICKS( it.m_simtime ) ) ) {
				return it.m_origin.dist_to( ent->m_vecOrigin( ) ) > 5.f ? &it : nullptr;
			}
		}

		return nullptr;
	}

	void c_ragebot::c_lagcomp::fsn_net_update_start( ) {
		static bool restored{ };

		if( !g_settings.rage.enabled( ) ) {
			if( !restored ) {
				restore_animations( );
				restored = true;
			}

			return;
		}

		restored = false;
		if( g_csgo.m_engine( )->IsInGame( ) ) {
			if( g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
				static float last_simtime[ 65 ]{ };
				for( int i{ 1 }; i < 65; ++i ) {
					auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

					if( ent && ent->is_valid( ) && ent != g_ctx.m_local ) {
						ent->disable_interpolaton( );
						if( ent != g_ctx.m_local ) {
							ent->m_bClientSideAnimation( ) = false;
						}

						float simtime = ent->m_flSimulationTime( );

						last_simtime[ i ] = simtime;
					}
				}
			}
		}
	}

	void c_ragebot::c_lagcomp::fsn_render_start( ) {
		if( !g_settings.rage.enabled( ) )
			return;

		if( !g_ctx.m_local )
			return;

		static float stored_lby[ 65 ];

		for( int i{ 1 }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if( !ent || !ent->is_player( ) ) {
				//invalidate_animstate( i );
				continue;
			}

			if( ent == g_ctx.m_local && ent->is_alive( ) ) {
				ent->m_bClientSideAnimation( ) = true;
				//fuck ya

				auto breaker_animstate = g_cheat.m_ragebot.m_antiaim->get_animstate( );
				if( breaker_animstate && g_settings.rage.anti_aim( ) ) {
					if( breaker_animstate->pBaseEntity == ent ) {
						memcpy( g_ctx.m_local->get_animstate( ),
							breaker_animstate,
							sizeof( CCSGOPlayerAnimState ) );
						ent->get_animstate( )->m_flLastClientSideAnimationUpdateTime = FLT_MAX;
						ent->get_animstate( )->m_iLastClientSideAnimationUpdateFramecount = INT_MAX;
					}
				}

				continue;
			}

			//return;

			if( !ent->is_alive( ) ) {
				ent->m_bClientSideAnimation( ) = true;
				//invalidate_animstate( i );
				continue;
			}

			if( ent->ce( )->IsDormant( ) ) {
				ent->invalidate_bone_cache( );
				continue;
			}

			if( ent->m_bGunGameImmunity( ) ) {
				ent->fix_animations( );
				ent->enforce_animations( );
				continue;
			}

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( )
				&& !g_settings.rage.friendlies ) {
				ent->fix_animations( );
				ent->enforce_animations( );
				continue;
			}

			if( m_data_lby[ i ].size( ) &&
				m_data_lby[ i ].front( ).m_ent != ent ) {
				m_data_lby[ i ].clear( );
			}

			if( m_data_normal[ i ].size( ) &&
				m_data_normal[ i ].front( ).m_ent != ent ) {
				m_data_normal[ i ].clear( );
			}

			auto update_anims = [ this, &ent ]( int e ) {
				ent->fix_animations( );
			};

			bool is_moving = ent->m_vecVelocity( ).length2d( ) > 0.1f && !ent->is_fakewalking( )
				&& ( ent->m_fFlags( ) & FL_ONGROUND );

			float lby = ent->m_flLowerBodyYawTarget( );

			float last_simtime = m_last_simtime[ i ];
			float simtime = ent->m_flSimulationTime( );

			if( !!std::abs( simtime - last_simtime ) ) {
				if( is_moving && g_settings.rage.resolver ) {
					ent->m_angEyeAngles( ).y = lby;
					//ent->ce( )->GetRenderAngles( ).y = lby;

					//vec3_t abs_ang = ent->get< vec3_t >( 0xc4 );
					//abs_ang.y = lby;
					//ent->set_abs_angles( abs_ang );

					update_anims( i );
					lag_record_t new_record( ent );
					//new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y
					//	= new_record.m_vecRenderAngles.y = lby;

					m_data_lby[ i ].push_front( new_record );
				}

				else if( ( lby != stored_lby[ i ] ) && g_settings.rage.resolver ) {
					stored_lby[ i ] = lby;
					ent->m_angEyeAngles( ).y = lby;
					//ent->ce( )->GetRenderAngles( ).y = lby;

					//vec3_t abs_ang = ent->get< vec3_t >( 0xc4 );
					//abs_ang.y = lby;
					//ent->set_abs_angles( abs_ang );

					update_anims( i );
					lag_record_t new_record( ent );
					//new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y
					//	= new_record.m_vecRenderAngles.y = lby;

					m_data_lby[ i ].push_front( new_record );
				}

				else if( g_settings.rage.resolver ) {
					static C_AnimationLayer last_layer[ 65 ]{ };
					auto layer = m_anim_backup[ i ][ 3 ];
					auto ent_layer = &ent->m_AnimOverlay( ).GetElements( )[ 3 ];
					if( g_cheat.m_ragebot.m_resolver->get_breaking_state( i ) == BREAKING_ADJUST &&
						layer.m_flCycle == 0.f && layer.m_flWeight == 0.f && ent_layer &&
						last_layer[ i ].m_flCycle > layer.m_flCycle ) {
						ent->m_angEyeAngles( ).y = lby;
						//ent->ce( )->GetRenderAngles( ).y = lby;

						//vec3_t abs_ang = ent->get< vec3_t >( 0xc4 );
						//abs_ang.y = lby;
						//ent->set_abs_angles( abs_ang );

						update_anims( i );

						auto anim_time = ent->m_flOldSimulationTime( ) + TICK_INTERVAL( );

						int flick_tick = TIME_TO_TICKS( anim_time + util::get_lerptime( ) );
						lag_record_t new_record( ent );
						//new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y 
						//	= new_record.m_vecRenderAngles.y = lby;
						new_record.m_flSimulationTime = anim_time;
						new_record.m_tickcount = flick_tick;

						new_record.m_balanceadjust = true;
						m_data_lby[ i ].push_front( new_record );
					}
					else {
						update_anims( i );
						m_data_normal[ i ].push_front( lag_record_t( ent ) );
					}

					last_layer[ i ] = layer;
				}
				else {
					update_anims( i );
					m_data_normal[ i ].push_front( lag_record_t( ent ) );
				}
			}

			m_last_simtime[ i ] = simtime;
			ent->enforce_animations( );

			while( !m_data_lby[ i ].empty( ) &&
				m_data_lby[ i ].size( ) > TIME_TO_TICKS( 1.0f ) ) {
				m_data_lby[ i ].pop_back( );
			}

			while( !m_data_normal[ i ].empty( ) &&
				m_data_normal[ i ].size( ) > TIME_TO_TICKS( 1.0f ) ) {
				m_data_normal[ i ].pop_back( );
			}
		}
	}

	void c_ragebot::c_lagcomp::fsn_postdataupdate_start( ) {
		if( !g_settings.rage.enabled( ) || !g_settings.rage.resolver( ) )
			return;

		static float stored_lby[ 65 ];

		for( int i{ 1 }; i < 65; ++i ) {
			auto ent = g_csgo.m_entlist( )->GetClientEntity< >( i );

			if( !ent || !ent->is_player( ) ) {
				//invalidate_animstate( i );
				continue;
			}

			if( ent == g_ctx.m_local ) {
				continue;
			}

			if( !ent->is_alive( ) ) {
				continue;
			}

			if( ent->ce( )->IsDormant( ) ) {
				continue;
			}

			memcpy( m_anim_backup.at( i ),
				ent->m_AnimOverlay( ).GetElements( ),
				sizeof( C_AnimationLayer ) * 13 );

			if( ent->m_bGunGameImmunity( ) ) {
				continue;
			}

			if( ent->m_iTeamNum( ) == g_ctx.m_local->m_iTeamNum( )
				&& !g_settings.rage.friendlies )
				continue;

			bool is_moving = ent->m_vecVelocity( ).length2d( ) > 0.1f && !ent->is_fakewalking( )
				&& ( ent->m_fFlags( ) & FL_ONGROUND );

			float lby = ent->m_flLowerBodyYawTarget( );

			float last_simtime = m_last_simtime[ i ];
			float simtime = ent->m_flSimulationTime( );

			if( !!std::abs( simtime - last_simtime ) ) {
				memcpy( m_anim_backup.at( i ),
					ent->m_AnimOverlay( ).GetElements( ),
					sizeof( C_AnimationLayer ) * 13 );

				if( is_moving && g_settings.rage.resolver ) {
					ent->m_angEyeAngles( ).y = lby;
					//ent->ce( )->GetRenderAngles( ).y = lby;

					m_last_simtime[ i ] = simtime;
					ent->fix_animations( );

					lag_record_t new_record( ent );
					//new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y
					//	= new_record.m_vecRenderAngles.y = lby;

					m_data_lby[ i ].push_front( new_record );
				}

				else if( ( lby != stored_lby[ i ] ) && g_settings.rage.resolver ) {
					stored_lby[ i ] = lby;
					ent->m_angEyeAngles( ).y = lby;
					//ent->ce( )->GetRenderAngles( ).y = lby;

					m_last_simtime[ i ] = simtime;
					ent->fix_animations( );

					auto anim_time = ent->m_flSimulationTime( ) + TICK_INTERVAL( );
					auto anim_tick = TIME_TO_TICKS( anim_time + util::get_lerptime( ) );

					lag_record_t new_record( ent );
					new_record.m_tickcount = anim_tick;
					//new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y
					//	= new_record.m_vecRenderAngles.y = lby;

					m_data_lby[ i ].push_front( new_record );
				}

				else if( g_settings.rage.resolver ) {
					static C_AnimationLayer last_layer[ 65 ]{ };
					auto layer = m_anim_backup[ i ][ 3 ];
					auto ent_layer = &ent->m_AnimOverlay( ).GetElements( )[ 3 ];
					if( g_cheat.m_ragebot.m_resolver->get_breaking_state( i ) == BREAKING_ADJUST &&
						layer.m_flCycle == 0.f && layer.m_flWeight == 0.f && ent_layer &&
						last_layer[ i ].m_flCycle > layer.m_flCycle ) {
						ent->m_angEyeAngles( ).y = lby;
						//ent->ce( )->GetRenderAngles( ).y = lby;

						ent->fix_animations( );
						m_last_simtime[ i ] = simtime;
						float anim_time = ent->m_flOldSimulationTime( ) + TICK_INTERVAL( );

						int flick_tick = TIME_TO_TICKS( anim_time + util::get_lerptime( ) );
						lag_record_t new_record( ent );
						new_record.m_vecAngles.y = new_record.m_vecAbsAngles.y
							= new_record.m_vecRenderAngles.y = lby;
						new_record.m_flSimulationTime = anim_time;
						new_record.m_tickcount = flick_tick;

						new_record.m_balanceadjust = true;
						m_data_lby[ i ].push_front( new_record );
					}

					last_layer[ i ] = layer;
				}
			}

			ent->enforce_animations( );
		}
	}

	RecordType_t c_ragebot::c_lagcomp::can_backtrack_entity( int ent_index ) {
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );
		if( ent->is_breaking_lc( ) ) {
			return RECORD_NONE;
		}

		auto data_lby = &m_data_lby[ ent_index ];
		auto data_normal = &m_data_normal[ ent_index ];

		if( !data_lby->empty( ) ) {
			for( auto& it : *data_lby ) {
				if( it.is_valid( ) ) return RECORD_LBY;
			}
		}

		if( !data_normal->empty( ) ) {
			for( auto& it : *data_normal ) {
				if( it.is_valid( ) ) return RECORD_NORMAL;
			}
		}

		return RECORD_NONE;
	}

	int c_ragebot::c_lagcomp::backtrack_entity( int ent_index, RecordType_t type, lag_record_t** out_record ) {
		if( type == RECORD_NONE ) return -1;

		auto& data = *get_records( ent_index, type );
		auto ent = g_csgo.m_entlist( )->GetClientEntity< >( ent_index );

		auto check_record = [ &ent, &out_record ]( lag_record_t* record, bool check_pelvis ) {
			if( record->m_balanceadjust && record->m_shot ) return -1;

			vec3_t head_pos = record->m_vecHeadPos;
			vec3_t pelvis_pos = record->m_vecPelvisPos;

			float damage = g_cheat.m_autowall.run( g_ctx.m_local, ent, head_pos, false ) * 4.f;
			float min_damage = std::min< float >( ent->m_iHealth( ),
				g_settings.rage.active->m_damage );

			//g_con->log( xors( "record: %d damage: %f" ), counter++, damage );

			if( damage > min_damage ) {
				if( !record->restore( ) )
					return -1;

				if( out_record ) {
					*out_record = record;
				}
				return record->m_tickcount;
			}

			if( check_pelvis ) {
				float pelvis_damage = g_cheat.m_autowall.run( g_ctx.m_local, ent, pelvis_pos, false );
				if( pelvis_damage > min_damage ) {
					if( !record->restore( ) )
						return -1;

					if( out_record ) {
						*out_record = record;
					}
					return record->m_tickcount;
				}
			}

			return -1;
		};

		if( data.empty( ) )
			return -1;

		auto* back = &data.back( );
		auto* front = &data.front( );

		for( auto& it : data ) {
			if( !it.is_valid( ) ) continue;
			front = &it;
			break;
		}

		for( auto& it : util::reverse_iterator( data ) ) {
			if( !it.is_valid( ) ) continue;
			back = &it;
			break;
		}

		int front_result = check_record( front, false );
		if( front_result != -1 ) return front_result;

		int back_result = check_record( back, true );
		if( back_result != -1 ) return back_result;

		if( g_settings.rage.bt_scan_all && !( g_settings.rage.preserve_fps && util::is_low_fps( ) ) ) {
			for( auto& it : data ) {
				if( &it == front ) continue;
				if( &it == back ) break;
				if( !it.is_valid( ) ) continue;

				auto result = check_record( &it, false );
				if( result != -1 ) return result;
			}
		}

		return -1;
	}
}