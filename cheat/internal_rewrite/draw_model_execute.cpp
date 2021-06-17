#include "hooks.hpp"
#include "base_cheat.hpp"
#include "context.hpp"
#include "util.hpp"
#include "settings.hpp"
//this is so ugly dont even look at it

//gets called once per frame for every entity
void __fastcall hooks::draw_model_execute( IVModelRender* ecx_, void* edx_,
	void* render_ctx, const DrawModelState_t& state,
	const ModelRenderInfo_t& info, matrix3x4* bone_to_world ) {
	static auto dme_o = draw_model_execute_o;

	if( g_csgo.m_panic || g_ctx.m_drawing_postscreenspace || g_settings.misc.hide_from_obs )
		return dme_o( ecx_, edx_, render_ctx, state, info, bone_to_world );

	bool draw = true;
	IMaterial* mat = g_settings.visuals.chams.flat( ) ? g_cheat.m_chams.m_materials.m_chams_flat : g_cheat.m_chams.m_materials.m_chams;
	auto model_name = g_csgo.m_model_info( )->GetModelName( info.m_model );
	auto local_index = g_csgo.m_engine( )->GetLocalPlayer( );
	int local_team = 0;
	if( g_ctx.m_local )
		local_team = g_ctx.m_local->m_iTeamNum( );
	int team = 0;
	bool is_player = false;

	//printf( "model: %s\n", model_name );

	if( strstr( model_name, xors( "models/player" ) ) && info.m_entity_index == local_index ) {
		if( g_settings.misc.thirdperson( ) && g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
			if( g_settings.rage.anti_aim ) {
				static bool was_peeking = false;
				static float peek_time = 0.f;
				bool is_peeking = g_cheat.m_lagmgr.m_is_peeking;

				if( is_peeking ) {
					if( !was_peeking ) {
						peek_time = g_csgo.m_globals->m_curtime;
					}

					auto matrix = g_cheat.m_lagmgr.m_peek_matrix;
					clr_t col( 225, 225, 225, g_ctx.m_local->m_bIsScoped( ) ? 9 : 25 );

					float totaltime = TICKS_TO_TIME( g_settings.rage.fakelag.ticks - 1 );

					float end_time = peek_time + totaltime;
					float delta = ( end_time - g_csgo.m_globals->m_curtime ) / totaltime;
					delta = std::clamp( delta, 0.f, 1.f );

					float progress = math::ease_in( 0.f, 1.f, delta );

					float alpha = 0.f;
					if( progress < 0.5f ) {
						alpha = progress * 2.f;
					} else {
						alpha = 1.f - ( 0.5f - progress ) * 2.f;
					}

					col.a( ) *= alpha;

					float backup_blend;
					float backup_modulation[ 3 ];

					if( g_ctx.m_local->m_bIsScoped( ) ) {
						backup_blend = 0.3f;
						backup_modulation[ 0 ] = backup_modulation[ 1 ] = backup_modulation[ 2 ] == 1.f;
					}

					backup_blend = g_csgo.m_render_view( )->GetBlend( );
					g_csgo.m_render_view( )->GetColorModulation( backup_modulation );

					bool wanted_ignorez = g_settings.visuals.chams.ignore_z( ) && g_settings.visuals.chams.friendlies( );
					bool backup = false;

					fclr_t clr_hid = g_settings.visuals.chams.color_hidden_friendly( ).to_fclr( );
					if( backup_modulation[ 0 ] == clr_hid.r( ) && backup_modulation[ 1 ] == clr_hid.g( ) && backup_modulation[ 2 ] == clr_hid.b( )
						&& backup_blend == clr_hid.a( ) ) {
						backup = true;
					}

					if( backup == wanted_ignorez || ( !backup && !g_settings.visuals.chams.clear_occlusion( ) ) || !g_settings.visuals.chams.enabled( )
						|| g_ctx.m_local->m_bIsScoped( ) ) {
						g_cheat.m_chams.m_materials.m_chams_flat.m_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
						g_cheat.m_chams.m_materials.force_material( g_cheat.m_chams.m_materials.m_chams_flat, col.to_fclr( ) );
						dme_o( ecx_, edx_, render_ctx, state, info, matrix );
						g_cheat.m_chams.m_materials.m_chams_flat.m_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, backup );

						g_csgo.m_render_view( )->SetBlend( backup_blend );
						g_csgo.m_render_view( )->SetColorModulation( backup_modulation );
						g_csgo.m_model_render( )->ForcedMaterialOverride( g_settings.visuals.chams.enabled( ) &&
							g_settings.visuals.chams.friendlies( ) && !g_ctx.m_local->m_bIsScoped( ) ? mat : nullptr );
					}
				}
				else {
					bool is_desyncing = g_settings.rage.anti_aim && g_settings.rage.fake_yaw;
					if( !( g_ctx.m_local->m_fFlags( ) & FL_ONGROUND ) )
						is_desyncing = false;

					if( is_desyncing ) {
						auto animstate = g_ctx.m_local->get_animstate( );
						
						auto max_desync = *( float* )( uintptr_t( animstate ) + 0x334 );
						auto desync = g_cheat.m_ragebot.m_antiaim->desync_delta( );

						float percentage = math::ease_in( 0.f, 1.f, desync / ( max_desync ) );

						float clr_a = g_ctx.m_local->m_bIsScoped( ) ? 0.025 : 0.07;

						clr_t col = g_settings.visuals.desync_clr;
						col.a( ) *= clr_a;
						col.a( ) *= percentage;

						float abs_yaw = animstate->m_flGoalFeetYaw;
						float desync_yaw = abs_yaw - desync;

						matrix3x4 matrix[ 128 ];
						memcpy( matrix, g_ctx.m_fake_matrix, sizeof( matrix ) );

						float sign = g_cheat.m_ragebot.m_antiaim->get_direction( ) ? -1.f : 1.f;
						for( size_t i{ }; i < 128; ++i ) {
							vec3_t pos = math::get_matrix_position( matrix[ i ] );
							vec3_t delta = pos - g_ctx.m_last_origin;

							math::set_matrix_position( info.m_origin + delta, matrix[ i ] );

							math::rotate_matrix( vec3_t( 0, g_ctx.m_fake_absyaw, 0 ), info.m_origin, -desync * sign, matrix[ i ] );
						}

						float backup_blend;
						float backup_modulation[ 3 ];

						if( g_ctx.m_local->m_bIsScoped( ) ) {
							backup_blend = 0.3f;
							backup_modulation[ 0 ] = backup_modulation[ 1 ] = backup_modulation[ 2 ] == 1.f;
						}

						backup_blend = g_csgo.m_render_view( )->GetBlend( );
						g_csgo.m_render_view( )->GetColorModulation( backup_modulation );

						bool wanted_ignorez = g_settings.visuals.chams.ignore_z( ) && g_settings.visuals.chams.friendlies( );
						bool backup = false;

						fclr_t clr_hid = g_settings.visuals.chams.color_hidden_friendly( ).to_fclr( );
						if( backup_modulation[ 0 ] == clr_hid.r( ) && backup_modulation[ 1 ] == clr_hid.g( ) && backup_modulation[ 2 ] == clr_hid.b( )
							&& backup_blend == clr_hid.a( ) ) {
							backup = true;
						}

						if( backup == wanted_ignorez || ( !backup && !g_settings.visuals.chams.clear_occlusion( ) ) || !g_settings.visuals.chams.enabled( )
							|| g_ctx.m_local->m_bIsScoped( ) ) {
							g_cheat.m_chams.m_materials.m_chams_flat.m_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
							g_cheat.m_chams.m_materials.force_material( g_cheat.m_chams.m_materials.m_chams_flat, col.to_fclr( ) );
							dme_o( ecx_, edx_, render_ctx, state, info, matrix );
							g_cheat.m_chams.m_materials.m_chams_flat.m_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, backup );

							g_csgo.m_render_view( )->SetBlend( backup_blend );
							g_csgo.m_render_view( )->SetColorModulation( backup_modulation );
							g_csgo.m_model_render( )->ForcedMaterialOverride( g_settings.visuals.chams.enabled( ) &&
								g_settings.visuals.chams.friendlies( ) && !g_ctx.m_local->m_bIsScoped( ) ? mat : nullptr );
						}
					}
				}

				was_peeking = is_peeking;
			}

			if( g_ctx.m_local->m_bIsScoped( ) ) {
				float clr[ ] = { 1.0f, 1.0f, 1.0f };
				g_csgo.m_render_view( )->SetColorModulation( clr );
				g_csgo.m_render_view( )->SetBlend( 0.3f );
				return dme_o( ecx_, edx_, render_ctx, state, info, bone_to_world );
			}
		}
	}

	if( strstr( model_name, xors( "models/player" ) ) && info.m_entity_index ) {
		auto player = g_csgo.m_entlist( )->GetClientEntity< >( info.m_entity_index );

		if( player && player->is_valid( ) && !player->m_bGunGameImmunity( ) ) {
			is_player = true;
			team = player->m_iTeamNum( );

			if( g_settings.rage.active && ( team != local_team || g_settings.rage.friendlies ) && !player->has_valid_anim( ) )
				return;

			if( is_player ) {
				bool should_draw = false;
				if( g_settings.rage.enabled( ) && g_settings.rage.bt_visualize( ) && ( team != local_team || g_settings.rage.friendlies( ) ) )
					should_draw = true;
				if( g_settings.legit.backtracking( ) && g_settings.legit.backtracking_visualize( ) && ( team != local_team || g_settings.legit.friendlies( ) ) )
					should_draw = true;


				if( should_draw ) {
					matrix3x4 render_matrix[ 128 ];

					if( g_cheat.m_ragebot.m_lagcomp->get_render_record( info.m_entity_index, render_matrix, g_settings.legit.backtracking( ) && g_settings.legit.backtracking_visualize( ) ) ) {
						fclr_t clr_hid = team != local_team ? g_settings.visuals.chams.color_hidden_enemy( ).to_fclr( ) : g_settings.visuals.chams.color_hidden_friendly( ).to_fclr( );

						float backup_modulation[ 3 ]{ };
						bool  backup_ignorez = false;
						bool  wanted_ignorez = g_settings.visuals.chams.ignore_z( );
						float backup_blend = 1.0f;

						backup_blend = g_csgo.m_render_view( )->GetBlend( );
						g_csgo.m_render_view( )->GetColorModulation( backup_modulation );

						//BIG ROFL
						if( backup_modulation[ 0 ] == clr_hid.r( ) && backup_modulation[ 1 ] == clr_hid.g( ) && backup_modulation[ 2 ] == clr_hid.b( )
							&& backup_blend == clr_hid.a( ) ) {
							backup_ignorez = true;
						}

						if( backup_ignorez == wanted_ignorez || ( !backup_ignorez && !g_settings.visuals.chams.clear_occlusion( ) ) || !g_settings.visuals.chams.enabled( ) ) {
							static auto flat_mat = g_cheat.m_chams.m_materials.m_chams_flat.m_mat;
							flat_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, g_settings.visuals.chams.ignore_z && g_settings.visuals.chams.enabled );

							clr_t bt_col = (g_settings.legit.backtracking( ) && g_settings.legit.backtracking_visualize( )) ? g_settings.legit.backtracking_col( ) : g_settings.rage.bt_col( );

							g_cheat.m_chams.m_materials.force_material( flat_mat, bt_col );

							dme_o( ecx_, edx_, render_ctx, state, info, render_matrix );
							flat_mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, backup_ignorez );
						}

						g_csgo.m_render_view( )->SetBlend( backup_blend );
						g_csgo.m_render_view( )->SetColorModulation( backup_modulation );
						g_csgo.m_model_render( )->ForcedMaterialOverride( g_settings.visuals.chams.enabled( ) &&
							( team != local_team || g_settings.visuals.chams.friendlies( ) ) ? mat : nullptr );
					}
				}
			}

			if( g_settings.visuals.chams.enabled ) {
				if( g_settings.visuals.chams.ignore_z && !g_ctx.m_drawing_sceneend && ( team != local_team || g_settings.visuals.chams.friendlies( ) ) && g_settings.visuals.chams.clear_occlusion )
					draw = false;
				else if( ( !g_settings.visuals.chams.ignore_z || !g_settings.visuals.chams.clear_occlusion ) && ( team != local_team || g_settings.visuals.chams.friendlies( ) ) ) {
					fclr_t clr = team != local_team ? g_settings.visuals.chams.color_visible_enemy( ) : g_settings.visuals.chams.color_visible_friendly( );

					if( g_settings.visuals.chams.ignore_z ) {
						fclr_t clr = team != local_team ? g_settings.visuals.chams.color_hidden_enemy( ) : g_settings.visuals.chams.color_hidden_friendly( );

						mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
						g_cheat.m_chams.m_materials.force_material( mat, clr );
						dme_o( ecx_, 0, render_ctx, state, info, bone_to_world );
					}

					mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
					g_cheat.m_chams.m_materials.force_material( mat, clr );
					dme_o( ecx_, 0, render_ctx, state, info, bone_to_world );

					g_csgo.m_model_render( )->ForcedMaterialOverride( nullptr );
					g_csgo.m_render_view( )->SetBlend( 1.0f );
					draw = false;
				}
			}

		}
	}

	if( g_settings.misc.transparent_vm && strstr( model_name, xors( "weapon" ) ) ) {
		if( strstr( model_name, xors( "arms" ) ) ) {
			g_csgo.m_render_view( )->SetBlend( 0.6f );
		}
	}

	if( draw ) {
		dme_o( ecx_, 0, render_ctx, state, info, bone_to_world );
	}

	if( !g_ctx.m_drawing_sceneend ) {
		g_csgo.m_model_render( )->ForcedMaterialOverride( nullptr );
	}
}