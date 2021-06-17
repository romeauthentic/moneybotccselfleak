#include "base_cheat.hpp"
#include "context.hpp"
#include "hooks.hpp"
#include "settings.hpp"
void __fastcall hooks::override_view( void* ecx_, void* edx_, CViewSetup* setup ) {
	if( g_csgo.m_panic ) {
		return override_view_o( ecx_, nullptr, setup );
	}

	float fov = setup->m_flFov;

	g_ctx.m_fov = setup->m_flFov;

	g_cheat.m_extra.thirdperson( );

	if( g_ctx.run_frame( ) && g_ctx.m_local->is_valid( ) ) {
		if( g_settings.misc.no_zoom && !g_settings.misc.zoom_on_double_scope ) {
			setup->m_flFov = g_settings.misc.fov_changer( );
		}
		else if( g_settings.misc.no_zoom && g_settings.misc.zoom_on_double_scope ) {
			auto weapon = g_ctx.m_local->get_weapon( );
            if( weapon && weapon->m_zoomLevel( ) == 2 ) {
				setup->m_flFov = g_settings.misc.fov_changer( ) * ( fov / 90.f );
            }
            else {
				setup->m_flFov = g_settings.misc.fov_changer( );
			}
		}
		else {
			setup->m_flFov = g_settings.misc.fov_changer( ) * ( fov / 90.f );
		}

		if( g_settings.misc.fake_duck && g_input.is_key_pressed( g_settings.misc.fake_duck_key ) && g_csgo.m_input( )->m_fCameraInThirdPerson )
			setup->m_vecOrigin.z = g_ctx.m_local->ce( )->GetRenderOrigin( ).z + 64.f;
	}

	if( g_settings.misc.no_scope && g_ctx.run_frame( ) && g_ctx.m_local->is_valid( ) && g_ctx.m_local->m_bIsScoped( ) ) {
		int backup = setup->m_iEdgeBlur;
		
		setup->m_iEdgeBlur = 0;
		
		override_view_o( ecx_, edx_, setup );

		setup->m_iEdgeBlur = backup;

		return;
	}

	override_view_o( ecx_, nullptr, setup );
}