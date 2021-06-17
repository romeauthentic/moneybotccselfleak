#include "hooks.hpp"
#include "renderer.hpp"
#include "context.hpp"
#include "console.hpp"
#include "base_cheat.hpp"
#include "math.hpp"
#include "settings.hpp"

void __fastcall hooks::paint_traverse( void* ecx_, void* edx_, unsigned int panel, bool force_repaint, bool allowforce ) {
	static auto paint_traverse_o = g_csgo.m_panel->get_old_function< decltype( &hooks::paint_traverse ) >( 41 );
	if( g_csgo.m_panic ) return paint_traverse_o( ecx_, 0, panel, force_repaint, allowforce );

	g_csgo.m_main_thread = std::this_thread::get_id( );

	if( g_settings.legit.enabled ) {
		g_settings.rage.enabled( ) = false;
	}

	static uint32_t mat_system_top_panel{ };
	if( !mat_system_top_panel ) {
		auto panel_name = g_csgo.m_panel( )->GetName( panel );
		auto panel_hash = hash::fnv1a( panel_name );
		if( fnv( "MatSystemTopPanel" ) == panel_hash ) {
			mat_system_top_panel = panel;
		}
	}

	static uint32_t hud_zoom{ };
	if( !hud_zoom ) {
		auto panel_name = g_csgo.m_panel( )->GetName( panel );
		auto panel_hash = hash::fnv1a( panel_name );
		if( fnv( "HudZoom" ) == panel_hash ) {
			hud_zoom = panel;
		}
	}

	if( panel && panel == hud_zoom && g_settings.misc.no_scope 
		&& g_ctx.m_local && g_ctx.m_local->is_valid( ) ) {
		return;
	}

	if( panel && panel == mat_system_top_panel ) {
		//render your epic 2008 hake esp here
		g_renderer.draw_box( 0, 0, 1, 1, clr_t( 0, 0, 0, 1 ) );
		g_cheat.m_visuals( );
		
		/*auto nci = g_csgo.m_engine( )->GetNetChannelInfo( );
		if( nci ) {
			float desired_in_latency = g_settings.misc.net_fakeping_amount * 0.001f;
			float latency = nci->GetLatency( 1 );

			float delta = latency - desired_in_latency;

			g_renderer.draw_string( g_fonts.f_12, 20, 400, clr_t( 255, 255, 255 ), "latency: %f", latency );
			//g_renderer.draw_string( g_fonts.f_12, 20,  )
			g_renderer.draw_string( g_fonts.f_12, 20, 410, clr_t( 255, 255, 255 ), "delta: %f", delta );
		}*/
	}

	g_csgo.m_prediction( )->SetLocalViewAngles( g_ctx.m_last_fakeangle );
	return paint_traverse_o( ecx_, 0, panel, force_repaint, allowforce );
}