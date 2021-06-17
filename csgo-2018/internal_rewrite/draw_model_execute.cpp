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
	static auto dme_o = g_csgo.m_model_render->get_old_function< decltype( &hooks::draw_model_execute ) >( 21 );

	if( g_csgo.m_panic || g_settings.misc.hide_from_obs )
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
	if (strstr(model_name, xors("models/player"))) {
		g_con->log("player mode");
	}

	if( strstr( model_name, xors( "models/player" ) ) && info.m_entity_index == local_index ) {
		if( g_settings.misc.thirdperson( ) && g_ctx.m_local && g_ctx.m_local->is_valid( )
			&& g_ctx.m_local->m_bIsScoped( ) ) {
			g_con->log("sss");
			float clr[ ] = { 1.0f, 1.0f, 1.0f };
			g_csgo.m_render_view( )->SetColorModulation( clr );
			g_csgo.m_render_view( )->SetBlend( 0.5f );
			return dme_o( ecx_, edx_, render_ctx, state, info, bone_to_world );
		}
	}
	if (strstr(model_name, xors("models/player")) && info.m_entity_index) {
		auto player = g_csgo.m_entlist()->GetClientEntity< >(info.m_entity_index);

		if (player && player->is_valid() && !player->m_bGunGameImmunity()) {
			is_player = true;
			team = player->m_iTeamNum();

			if (g_settings.rage.active && (team != local_team || g_settings.rage.friendlies) && !player->has_valid_anim())
				return;

			if (is_player) {
				bool should_draw = false;
				if (g_settings.rage.enabled() && g_settings.rage.bt_visualize() && (team != local_team || g_settings.rage.friendlies()))
					should_draw = true;
				if (g_settings.legit.backtracking() && g_settings.legit.backtracking_visualize() && (team != local_team || g_settings.legit.friendlies()))
					should_draw = true;


				if (should_draw) {
					matrix3x4 render_matrix[128];

					if (g_cheat.m_ragebot.m_lagcomp->get_render_record(info.m_entity_index, render_matrix, g_settings.legit.backtracking() && g_settings.legit.backtracking_visualize())) {
						fclr_t clr_hid = team != local_team ? g_settings.visuals.chams.color_hidden_enemy().to_fclr() : g_settings.visuals.chams.color_hidden_friendly().to_fclr();

						float backup_modulation[3]{ };
						bool  backup_ignorez = false;
						bool  wanted_ignorez = g_settings.visuals.chams.ignore_z();
						float backup_blend = 1.0f;

						backup_blend = g_csgo.m_render_view()->GetBlend();
						g_csgo.m_render_view()->GetColorModulation(backup_modulation);

						//BIG ROFL
						if (backup_modulation[0] == clr_hid.r() && backup_modulation[1] == clr_hid.g() && backup_modulation[2] == clr_hid.b()
							&& backup_blend == clr_hid.a()) {
							backup_ignorez = true;
						}

						if (backup_ignorez == wanted_ignorez || (!backup_ignorez && !g_settings.visuals.chams.clear_occlusion()) || !g_settings.visuals.chams.enabled()) {
							static auto flat_mat = g_cheat.m_chams.m_materials.m_chams_flat.m_mat;
							flat_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_settings.visuals.chams.ignore_z && g_settings.visuals.chams.enabled);

							clr_t bt_col = (g_settings.legit.backtracking() && g_settings.legit.backtracking_visualize()) ? g_settings.legit.backtracking_col() : g_settings.rage.bt_col();

							g_cheat.m_chams.m_materials.force_material(flat_mat, bt_col);

							dme_o(ecx_, edx_, render_ctx, state, info, render_matrix);
							flat_mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, backup_ignorez);
						}

						g_csgo.m_render_view()->SetBlend(backup_blend);
						g_csgo.m_render_view()->SetColorModulation(backup_modulation);
						g_csgo.m_model_render()->ForcedMaterialOverride(g_settings.visuals.chams.enabled() &&
							(team != local_team || g_settings.visuals.chams.friendlies()) ? mat : nullptr);
					}
				}
			}

			if (g_settings.visuals.chams.enabled) {
				if (g_settings.visuals.chams.ignore_z && !g_ctx.m_drawing_screneend && (team != local_team || g_settings.visuals.chams.friendlies()) && g_settings.visuals.chams.clear_occlusion)
					draw = false;
				else if ((!g_settings.visuals.chams.ignore_z || !g_settings.visuals.chams.clear_occlusion) && (team != local_team || g_settings.visuals.chams.friendlies())) {
					fclr_t clr = team != local_team ? g_settings.visuals.chams.color_visible_enemy() : g_settings.visuals.chams.color_visible_friendly();

					if (g_settings.visuals.chams.ignore_z) {
						fclr_t clr = team != local_team ? g_settings.visuals.chams.color_hidden_enemy() : g_settings.visuals.chams.color_hidden_friendly();

						mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
						g_cheat.m_chams.m_materials.force_material(mat, clr);
						dme_o(ecx_, 0, render_ctx, state, info, bone_to_world);
					}

					mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					g_cheat.m_chams.m_materials.force_material(mat, clr);
					dme_o(ecx_, 0, render_ctx, state, info, bone_to_world);

					g_csgo.m_model_render()->ForcedMaterialOverride(nullptr);
					g_csgo.m_render_view()->SetBlend(1.0f);
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

	if (draw) {
		dme_o(ecx_, 0, render_ctx, state, info, bone_to_world);
	}

	if (!g_ctx.m_drawing_screneend) {
		g_csgo.m_model_render()->ForcedMaterialOverride(nullptr);
	}
}