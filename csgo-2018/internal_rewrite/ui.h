#pragma once
#include "ui_base_item.h"
#include "ui_menu.h"
#include "ui_form.h"
#include "ui_render.h"
#include "ui_checkbox.h"
#include "ui_tab_manager.h"
#include "ui_slider.h"
#include "ui_dropdown.h"
#include "ui_key_picker.h"
#include "ui_button.h"
#include "ui_color_picker.h"
#include "ui_label.h"
#include "ui_itemlist.h"
#include "ui_itemlist_def.h"

#include "settings.hpp"
#include "base_cheat.hpp"

namespace ui
{
	int current_category = 0;

	static std::string date_str = __DATE__;
	auto menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "quebot" ),
		__DATE__ );

	static void render( ) {
		static bool was_setup = false;
		if ( !was_setup ) {
			std::transform( date_str.begin( ), date_str.end( ), date_str.begin( ),
				[ ]( char c ) { return ::tolower( c ); }
			);

			menu.reset( );
#ifndef _DEBUG
			menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "moneybot" ),
				date_str.c_str( ) );
#else
			menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "wisdom teeth hake" ),
				date_str.c_str( ) );
#endif
			auto tab_sheet = std::make_shared< ui::c_tab_manager >( );

			//todo: replace with icons?
			auto tab_legit = std::make_shared< ui::c_tab_sheet >( xors( "legit" ), &icons::sprite_legit ); {
				auto subtab_sheet = std::make_shared< ui::c_subtab_manager >( );

				auto subtab_main = std::make_shared< ui::c_tab_sheet >( xors( "main" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ) ); {
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.legit.enabled ) );
						auto activation_dropdown = std::make_shared< ui::c_dropdown< > >( 
							0, 0, 120, xors( "activation type" ), 
							&g_settings.legit.activation_type,
							&ui::dropdowns::activation_types_aimbot );
						activation_dropdown->add_item( std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.legit.key ) )->set_cond( 
							[ ]( ) { return g_settings.legit.activation_type == 1; } );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "friendlies" ), &g_settings.legit.friendlies ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "aim assist" ), &g_settings.legit.assist ) );

						main_form->add_item( activation_dropdown );
					}

					auto smooth_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "smoothing" ) ); {
						smooth_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "dynamic smoothing" ), &g_settings.legit.dynamic_smoothing ) );
						smooth_form->add_item( std::make_shared< ui::c_slider< size_t > >( 0, 0, 120, 2, 28, xors( "sample size" ), &g_settings.legit.sample_size ) );
						smooth_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.1f, 2.0f, xors( "multiplier" ), &g_settings.legit.smooth_factor ) );
					}

					auto backtracking_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "backtracking" ) ); {
						backtracking_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.legit.backtracking ) );
						backtracking_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "target" ), &g_settings.legit.backtracking_target ) );
						auto visualize_checkbox = backtracking_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "visualize" ), &g_settings.legit.backtracking_visualize ) );
						visualize_checkbox->add_item( std::make_shared< ui::c_color_picker >( 175, 2, 15, &g_settings.legit.backtracking_col ) );
						visualize_checkbox->set_cond( [ ] { return !g_settings.misc.hide_from_obs( ); } );

						backtracking_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 50.f, xors( "max fov" ), &g_settings.legit.backtracking_fov ) );
						backtracking_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 0.2f, xors( "max time" ), &g_settings.legit.backtracking_time, "s" ) );
					}

					auto triggerbot_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "triggerbot" ) ); {
						triggerbot_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.legit.triggerbot ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.legit.trigger_key ) );

						triggerbot_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "magnet" ), &g_settings.legit.trigger_magnet ) );

						triggerbot_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 0.2f, xors( "trigger delay" ), &g_settings.legit.trigger_delay, "s" )
						)->set_cond( [] { return g_settings.legit.triggerbot( ); } );
					}

					left_column->add_item( main_form );
					left_column->add_item( smooth_form );

					right_column->add_item( backtracking_form );
					right_column->add_item( triggerbot_form );

					subtab_main->add_item( left_column );
					subtab_main->add_item( right_column );
				}

				//make a subtab for each weapon type?
				auto subtab_weapons = std::make_shared< ui::c_tab_sheet >( xors( "weapons" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto general_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "general" ) ); {
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.general.m_fov ) );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.general.m_speed ) );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 100.f, xors( "rcs" ), &g_settings.legit.general.m_rcs ) );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.general.m_assist_strength ) );
					}

					auto snipers_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "snipers" ) ); {
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.snipers.m_fov ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.snipers.m_speed ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 100.f, xors( "rcs" ), &g_settings.legit.snipers.m_rcs ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.snipers.m_assist_strength ) );
						snipers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "awp body aim" ), &g_settings.legit.awp_baim ) );
					}

					auto rifles_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "rifles" ) ); {
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.rifles.m_fov ) );
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.rifles.m_speed ) );
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 100.f, xors( "rcs" ), &g_settings.legit.rifles.m_rcs ) );
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.rifles.m_assist_strength ) );
					}

					auto pistols_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "pistols" ) ); {
						pistols_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.pistols.m_fov ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.pistols.m_speed ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 100.f, xors( "rcs" ), &g_settings.legit.pistols.m_rcs ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.pistols.m_assist_strength ) );
					}

					left_column->add_item( general_form );
					left_column->add_item( snipers_form );
					right_column->add_item( rifles_form );
					right_column->add_item( pistols_form );

					subtab_weapons->add_item( left_column );
					subtab_weapons->add_item( right_column );
				}

				subtab_sheet->add_item( subtab_weapons );
				subtab_sheet->add_item( subtab_main );

				tab_legit->add_item( subtab_sheet );
			}


			auto tab_rage = std::make_shared< ui::c_tab_sheet >( xors( "rage" ), &icons::sprite_rage ); {
				auto subtab_sheet = std::make_shared< ui::c_subtab_manager >( );

				auto subtab_aimbot = std::make_shared< ui::c_tab_sheet >( xors( "aimbot" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 235, xors( "main" ), 235 ); {
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.rage.enabled ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "friendlies" ), &g_settings.rage.friendlies ) );
						main_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 180.f, xors( "max fov" ), &g_settings.rage.fov ) );


						main_form->add_item( std::make_shared< ui::c_dropdown< int > >(
							0, 0, 120, xors( "activation type" ),
							&g_settings.rage.activation_type,
							&dropdowns::activation_types_aimbot ) )->add_item(
								std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.rage.aimkey ) )->set_cond(
									[ ]( ) { return g_settings.rage.activation_type == 1; } );

						main_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "silent" ),
							&g_settings.rage.silent, &dropdowns::silent_aimbot_types ) );

						
						main_form->add_item( std::make_shared< ui::c_dropdown< int > >( 0, 0, 120, xors( "hitbox" ),
							&g_settings.rage.hitbox, &dropdowns::hitboxes ) );

						auto hitscan_form = main_form->add_item( std::make_shared< ui::c_form >( 0, 5, 119, 0, xors( "hitscan" ), 0, true ) );
						hitscan_form->set_cond( [ ]( ) { return g_settings.rage.hitbox == -1; } );

						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "head" ), &g_settings.rage.hitscan.head ) );
						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "chest" ), &g_settings.rage.hitscan.chest ) );
						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "stomach" ), &g_settings.rage.hitscan.stomach ) );
						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "arms" ), &g_settings.rage.hitscan.arms ) );
						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "legs" ), &g_settings.rage.hitscan.legs ) );
						hitscan_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "feet" ), &g_settings.rage.hitscan.feet ) );

						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore limbs when moving" ), &g_settings.rage.ignore_limbs_moving )
							)->set_cond( [ ]( ) { return g_settings.rage.hitbox == -1; } );
						main_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "multipoint" ), &g_settings.rage.multipoint, &dropdowns::multipoint_types ) );

						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "preserve fps" ), &g_settings.rage.preserve_fps ) );
					}

					auto accuracy_form = std::make_shared< ui::c_form >( 0, 0, 215, 157, xors( "accuracy" ), 157 ); {
						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto scope" ), &g_settings.rage.auto_scope ) );
						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "anti-aim correction" ), &g_settings.rage.resolver ) );
						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "pitch correction" ), &g_settings.rage.pitch_resolver ) 
						)->set_cond( [ ]( ) { return g_settings.rage.resolver; } );

	

						accuracy_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "fake lag correction" ), &g_settings.rage.fakelag_resolver, &dropdowns::fakelag_fix_types )
						)->set_cond( [ ]( ) { return g_settings.rage.resolver; } );

						auto override_checkbox = accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "resolver override" ), &g_settings.rage.resolver_override ) );
						override_checkbox->add_item( std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.rage.resolver_override_key ) );
						override_checkbox->set_cond( [ ]( ) { return g_settings.rage.resolver; } );
						auto visualize_checkbox = accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "visualize lag compensation" ), &g_settings.rage.bt_visualize ) );
						visualize_checkbox->set_cond( [ ]( ) { return g_settings.rage.resolver( ) && !g_settings.misc.hide_from_obs( );; } );
						visualize_checkbox->add_item( std::make_shared< ui::c_color_picker >( 175, 2, 15, &g_settings.rage.bt_col ) );

						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "compensate spread" ), &g_settings.rage.compensate_spread ) );
					}

					auto selection_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "selection" ) ); {
						selection_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "type" ), &g_settings.rage.selection_type, &dropdowns::selection_type ) );
						selection_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "prefer moving" ), &g_settings.rage.prefer_moving ) );
						selection_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "prefer low hp" ), &g_settings.rage.prefer_low_hp ) );
					}



					auto general_form = std::make_shared< ui::c_form >( 0, 0, 215, 306, xors( "weapon specific" ), 306 ); {
						general_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "category" ), &current_category, &dropdowns::weapon_configs ) );

						general_form->add_item(std::make_shared< ui::c_dropdown< > >(0, 0, 120, xors("auto stop"), &g_settings.rage.general.m_auto_stop, &dropdowns::auto_stop));
						general_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spread limit" ), &g_settings.rage.general.m_spread_limit )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );

						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.general.m_spread_limit_min )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread && g_settings.rage.general.m_spread_limit; } );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "hitbox scale" ), &g_settings.rage.general.m_hitbox_scale ) )->set_cond(
							[ ]( ) { return g_settings.rage.multipoint( ); } );

						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "hitchance" ), &g_settings.rage.general.m_hitchance )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );
						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "min damage" ), &g_settings.rage.general.m_damage ) );
						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.general.m_damage_scale, xors( "%%hp" ) ) );
						general_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "delay shot" ), &g_settings.rage.general.m_delay_type, &dropdowns::delay_type ) );
						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 300, &g_settings.rage.general.m_delay, xors( "ms" ) )
						)->set_cond( [ ]( ) { return g_settings.rage.general.m_delay_type == 2; } );

						general_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head in air" ), &g_settings.rage.general.m_baim_air ) );
						general_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head on fake angles" ), &g_settings.rage.general.m_baim_fake ) );
						general_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "body aim if lethal" ), &g_settings.rage.general.m_baim_lethal ) );
						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 20, xors( "body aim after" ), &g_settings.rage.general.m_baim_shots, xors( "shots" ) ) );
						general_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "body aim under" ), &g_settings.rage.general.m_baim_health, xors( "hp" ) ) );
					}

					general_form->set_cond( [ ]( ) { return current_category == 0; } );

					auto pistols_form = std::make_shared< ui::c_form >( 0, 0, 215, 306, xors( "weapon specific" ), 306 ); {
						pistols_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "category" ), &current_category, &dropdowns::weapon_configs ) );

						pistols_form->add_item(std::make_shared< ui::c_dropdown< > >(0, 0, 120, xors("auto stop"), &g_settings.rage.heavy_pistols.m_auto_stop, &dropdowns::auto_stop));
						pistols_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spread limit" ), &g_settings.rage.heavy_pistols.m_spread_limit )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );

						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.heavy_pistols.m_spread_limit_min )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread && g_settings.rage.heavy_pistols.m_spread_limit; } );
						pistols_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "hitbox scale" ), &g_settings.rage.heavy_pistols.m_hitbox_scale ) )->set_cond(
							[ ]( ) { return g_settings.rage.multipoint( ); } );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "hitchance" ), &g_settings.rage.heavy_pistols.m_hitchance )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "min damage" ), &g_settings.rage.heavy_pistols.m_damage ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.heavy_pistols.m_damage_scale, xors( "%%hp" ) ) );
						pistols_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "delay shot" ), &g_settings.rage.heavy_pistols.m_delay_type, &dropdowns::delay_type ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 300, &g_settings.rage.heavy_pistols.m_delay, xors( "ms" ) )
						)->set_cond( [ ]( ) { return g_settings.rage.heavy_pistols.m_delay_type == 2; } );

						pistols_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head in air" ), &g_settings.rage.heavy_pistols.m_baim_air ) );
						pistols_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head on fake angles" ), &g_settings.rage.heavy_pistols.m_baim_fake ) );
						pistols_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "body aim if lethal" ), &g_settings.rage.heavy_pistols.m_baim_lethal ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 20, xors( "body aim after" ), &g_settings.rage.heavy_pistols.m_baim_shots, xors( "shots" ) ) );
						pistols_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "body aim under" ), &g_settings.rage.heavy_pistols.m_baim_health, xors( "hp" ) ) );
					}

					pistols_form->set_cond( [ ]( ) { return current_category == 1; } );

					auto snipers_form = std::make_shared< ui::c_form >( 0, 0, 215, 306, xors( "weapon specific" ), 306 ); {
						snipers_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "category" ), &current_category, &dropdowns::weapon_configs ) );

						snipers_form->add_item(std::make_shared< ui::c_dropdown< > >(0, 0, 120, xors("auto stop"), &g_settings.rage.snipers.m_auto_stop, &dropdowns::auto_stop));
						snipers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spread limit" ), &g_settings.rage.snipers.m_spread_limit )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );

						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.snipers.m_spread_limit_min )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread && g_settings.rage.snipers.m_spread_limit; } );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "hitbox scale" ), &g_settings.rage.snipers.m_hitbox_scale ) )->set_cond(
							[ ]( ) { return g_settings.rage.multipoint( ); } );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "hitchance" ), &g_settings.rage.snipers.m_hitchance )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "min damage" ), &g_settings.rage.snipers.m_damage ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.snipers.m_damage_scale, xors( "%%hp" ) ) );
						snipers_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "delay shot" ), &g_settings.rage.snipers.m_delay_type, &dropdowns::delay_type ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 300, &g_settings.rage.snipers.m_delay, xors( "ms" ) )
						)->set_cond( [ ]( ) { return g_settings.rage.snipers.m_delay_type == 2; } );

						snipers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head in air" ), &g_settings.rage.snipers.m_baim_air ) );
						snipers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head on fake angles" ), &g_settings.rage.snipers.m_baim_fake ) );
						snipers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "body aim if lethal" ), &g_settings.rage.snipers.m_baim_lethal ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 20, xors( "body aim after" ), &g_settings.rage.snipers.m_baim_shots, xors( "shots" ) ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "body aim under" ), &g_settings.rage.snipers.m_baim_health, xors( "hp" ) ) );
					}

					snipers_form->set_cond( [ ]( ) { return current_category == 2; } );

					auto auto_form = std::make_shared< ui::c_form >( 0, 0, 215, 306, xors( "weapon specific" ), 306 ); {
						auto_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "category" ), &current_category, &dropdowns::weapon_configs ) );

						auto_form->add_item(std::make_shared< ui::c_dropdown< > >(0, 0, 120, xors("auto stop"), &g_settings.rage.auto_snipers.m_auto_stop, &dropdowns::auto_stop));
						auto_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spread limit" ), &g_settings.rage.auto_snipers.m_spread_limit )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );

						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.auto_snipers.m_spread_limit_min )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread && g_settings.rage.auto_snipers.m_spread_limit; } );
						auto_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "hitbox scale" ), &g_settings.rage.auto_snipers.m_hitbox_scale ) )->set_cond(
							[ ]( ) { return g_settings.rage.multipoint( ); } );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "hitchance" ), &g_settings.rage.auto_snipers.m_hitchance )
						)->set_cond( [ ]( ) { return !g_settings.rage.compensate_spread; } );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "min damage" ), &g_settings.rage.auto_snipers.m_damage ) );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, &g_settings.rage.auto_snipers.m_damage_scale, xors( "%%hp" ) ) );
						auto_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "delay shot" ), &g_settings.rage.auto_snipers.m_delay_type, &dropdowns::delay_type ) );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 300, &g_settings.rage.auto_snipers.m_delay, xors( "ms" ) )
						)->set_cond( [ ]( ) { return g_settings.rage.auto_snipers.m_delay_type == 2; } );

						auto_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head in air" ), &g_settings.rage.auto_snipers.m_baim_air ) );
						auto_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid head on fake angles" ), &g_settings.rage.auto_snipers.m_baim_fake ) );
						auto_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "body aim if lethal" ), &g_settings.rage.auto_snipers.m_baim_lethal ) );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 20, xors( "body aim after" ), &g_settings.rage.auto_snipers.m_baim_shots, xors( "shots" ) ) );
						auto_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 100, xors( "body aim under" ), &g_settings.rage.auto_snipers.m_baim_health, xors( "hp" ) ) );
					}

					auto_form->set_cond( [ ]( ) { return current_category == 3; } );

					left_column->add_item( main_form );
					left_column->add_item( accuracy_form );
					right_column->add_item( selection_form );
					right_column->add_item( general_form );
					right_column->add_item( pistols_form );
					right_column->add_item( snipers_form );
					right_column->add_item( auto_form );


					subtab_aimbot->add_item( left_column );
					subtab_aimbot->add_item( right_column );
				}

				auto subtab_antiaim = std::make_shared< ui::c_tab_sheet >( xors( "antiaim" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 110, xors( "main" ), 110 ); {
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.rage.anti_aim ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "at players" ), &g_settings.rage.at_players ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "dormant check" ), &g_settings.rage.dormant_check ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "angle step" ), &g_settings.rage.angle_step ) );
						main_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 45, &g_settings.rage.angle_step_degrees ) )->set_cond(
							[ ]( ) { return g_settings.rage.angle_step( ); } );
					}

					auto pitch_form = std::make_shared< ui::c_form >( 0, 0, 215, 60, xors( "pitch" ) ); {
						pitch_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "pitch" ), &g_settings.rage.pitch, &dropdowns::antiaim_pitch ) );
						pitch_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -89, 89, &g_settings.rage.pitch_offset ) )->set_cond(
							[ ]( ) { return g_settings.rage.pitch == 1 || g_settings.rage.pitch == 4 || g_settings.rage.pitch == 5; } );
					}

					auto yaw_form = std::make_shared< ui::c_form >( 0, 0, 215, 211, xors( "yaw" ), 211 ); {
						yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "real yaw" ), &g_settings.rage.real_yaw, &dropdowns::antiaim_yaw ) );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -180, 180, &g_settings.rage.real_yaw_add( ) ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "real yaw jitter" ), &g_settings.rage.real_yaw_jitter ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "real yaw moving" ), &g_settings.rage.real_yaw_moving, &dropdowns::antiaim_yaw ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -180, 180, &g_settings.rage.real_yaw_moving_add( ) ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw_moving( ) && !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "real yaw moving jitter" ), &g_settings.rage.real_moving_jitter ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw_moving( ) && !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "in-air yaw" ), &g_settings.rage.air_yaw, &dropdowns::antiaim_yaw ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -180, 180, &g_settings.rage.air_yaw_add( ) ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.air_yaw( ) && !!g_settings.rage.real_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "in-air yaw jitter" ), &g_settings.rage.air_yaw_jitter ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.air_yaw( ) && !!g_settings.rage.real_yaw( ); } );


						yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "fake yaw" ), &g_settings.rage.fake_yaw, &dropdowns::antiaim_yaw ) );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -180, 180, &g_settings.rage.fake_yaw_add( ) ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.fake_yaw( ); } );
						yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "fake yaw jitter" ), &g_settings.rage.fake_yaw_jitter ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.fake_yaw( ); } );
					}
					
					auto edge_form = std::make_shared< ui::c_form >( 0, 0, 215, 110, xors( "edge detection" ), 110 ); {
						edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "edge" ), &g_settings.rage.edge_dtc_normal,
							&dropdowns::antiaim_edge ) );

						edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 70, 300, &g_settings.rage.edge_dtc_standing ) )->set_cond(
							[ ]( ) { return g_settings.rage.edge_dtc_normal( ) == 2; } );

						edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 70, 300, &g_settings.rage.edge_dtc_moving ) )->set_cond(
							[ ]( ) { return !!g_settings.rage.edge_dtc_normal( ); } );

						edge_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "freestanding" ), 
							&g_settings.rage.edge_detection ) );

						edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 50, 200, &g_settings.rage.freestanding_distance )
						)->set_cond( [ ]( ) { return g_settings.rage.edge_detection( ); } );

						edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "priority" ),
							&g_settings.rage.edge_dtc_priority, &dropdowns::edge_priority )
						)->set_cond( [ ]( ) { return g_settings.rage.edge_dtc_normal( ) && g_settings.rage.edge_detection( ); } );

						edge_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "break lby when edging" ), &g_settings.rage.break_lby_edge( ) ) 
						)->set_cond( [ ]( ) { return g_settings.rage.break_lby; } );

						edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "fake jitter" ), &g_settings.rage.edge_dtc_jitter ) );
					}

					auto lby_form = std::make_shared< ui::c_form >( 0, 0, 215, 126, xors( "lby breaker" ), 126 ); {
						lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "break lby" ), &g_settings.rage.break_lby ) );
						lby_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -120, 120, &g_settings.rage.lby_delta ) )->set_cond(
							[ ]( ) { return g_settings.rage.break_lby( ); } );

						lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid updates" ), &g_settings.rage.lby_avoid_updates ) );
						auto angles_box = lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "draw angles" ), &g_settings.rage.draw_angles ) );
						angles_box->set_cond( [ ]( ) { return g_settings.rage.break_lby( ); } );
						angles_box->add_item( std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.rage.lby_color ) );
						angles_box->add_item( std::make_shared< ui::c_color_picker >( 180, -9, 15, &g_settings.rage.real_color ) );

						lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "fakewalk" ), &g_settings.rage.fakewalk ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.rage.fakewalk_key )
						);
						lby_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 7, 14, &g_settings.rage.fakewalk_ticks ) )->set_cond(
							[ ]( ) { return g_settings.rage.fakewalk; }	);
					}

					auto fakelag_form = std::make_shared< ui::c_form >( 0, 0, 215, 145, xors( "fakelag" ), 145 ); {
						fakelag_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "mode" ), &g_settings.rage.fakelag.mode, &dropdowns::fakelag_types ) );
						fakelag_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 14, &g_settings.rage.fakelag.ticks ) );
						fakelag_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 50, xors( "variance" ), &g_settings.rage.fakelag.fluctuate ) );

						auto flags_form = fakelag_form->add_item( std::make_shared< ui::c_form >( 0, 5, 139, 0, xors( "flags" ), 0, true ) );

						flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "in air" ), &g_settings.rage.fakelag.in_air ) );
						flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "on move" ), &g_settings.rage.fakelag.in_move ) );
						flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "on peek" ), &g_settings.rage.fakelag.on_peek ) );
						flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "while shooting" ), &g_settings.rage.fakelag.in_attack ) );
						flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid ground" ), &g_settings.rage.fakelag.avoid_ground ) );
					}

					left_column->add_item( main_form );
					left_column->add_item( pitch_form );
					left_column->add_item( yaw_form );

					right_column->add_item( edge_form );
					right_column->add_item( lby_form );
					right_column->add_item( fakelag_form );

					subtab_antiaim->add_item( left_column );
					subtab_antiaim->add_item( right_column );
				}

				subtab_sheet->add_item( subtab_antiaim );
				subtab_sheet->add_item( subtab_aimbot );

				tab_rage->add_item( subtab_sheet );
			}


			auto tab_visuals = std::make_shared< ui::c_tab_sheet >( xors( "visuals" ), &icons::sprite_visuals ); {
				auto subtab_sheet = std::make_shared< ui::c_subtab_manager >( );

				auto subtab_players = std::make_shared< ui::c_tab_sheet >( xors( "players" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ) ); {
						main_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "activation type" ), &g_settings.visuals.activation_type, &dropdowns::activation_type_visuals ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.visuals.key ) )->set_cond( [ ]( ) { return g_settings.visuals.activation_type > 1; } );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "friendlies" ), &g_settings.visuals.friendlies ) );
					}

					auto player_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "players" ) ); {
						auto box_checkbox = player_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("top bar (health, name)"), &g_settings.visuals.box));

						box_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.visuals.box_enemy( ) ) );
						box_checkbox->add_item( std::make_shared< ui::c_color_picker >( 180, -9, 15, &g_settings.visuals.box_friendly( ) ) )->set_cond( 
							[ ]( ) { return g_settings.visuals.friendlies; } );

						auto glow_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "glow" ), &g_settings.visuals.glow ) );
						glow_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 2, 15, &g_settings.visuals.glow_enemy ) );
						glow_checkbox->add_item( std::make_shared< ui::c_color_picker >( 180, -11, 15, &g_settings.visuals.glow_friendly ) )->set_cond(
							[ ]( ) { return g_settings.visuals.friendlies || g_settings.visuals.chams.friendlies; } );
						glow_checkbox->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs; } );

						auto skeleton_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "skeleton" ), &g_settings.visuals.skeleton ) );
						skeleton_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 2, 15, &g_settings.visuals.skeleton_enemy ) );
						skeleton_checkbox->add_item( std::make_shared< ui::c_color_picker >( 180, -11, 15, &g_settings.visuals.skeleton_friendly ) )->set_cond(
							[ ]( ) { return g_settings.visuals.friendlies; } );

						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "dormant" ), &g_settings.visuals.dormant ) );
						

						player_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("bottom bar (weapon, ammo)"), &g_settings.visuals.weapon))->add_item(
							std::make_shared< ui::c_color_picker >(160, 4, 16, &g_settings.visuals.ammo_bar_clr()));;

						auto flag_form = player_form->add_item(std::make_shared< ui::c_form >(0, 5, 135, 0, xors("flags"), 0, true));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("money"), &g_settings.visuals.money));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("scoped"), &g_settings.visuals.scoped));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("c4 carrier"), &g_settings.visuals.c4_carrier));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("flashed"), &g_settings.visuals.flashed));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("1-way potential"), &g_settings.visuals.one_way));
						flag_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("high ping warning"), &g_settings.visuals.ping));

						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "sound" ), &g_settings.visuals.sound ) )->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs; } );
						player_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 220, 500, &g_settings.visuals.sound_range ) )->set_cond( [ ]( ) { return g_settings.visuals.sound && !g_settings.misc.hide_from_obs; } );
					}

					auto hud_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "hud" ) ); {
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "out of pov" ), &g_settings.visuals.out_of_pov ) );
						hud_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.25f, 1.f, &g_settings.visuals.out_of_pov_radius )
						)->set_cond( [ ]( ) { return g_settings.visuals.out_of_pov( ); } );

						hud_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 15, 40, &g_settings.visuals.out_of_pov_size, xors( "px" ) )
						)->set_cond( [ ]( ) { return g_settings.visuals.out_of_pov( ); } );
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "hitmarkers" ), &g_settings.visuals.hitmarkers ) );
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spectator list" ), &g_settings.visuals.spec_list ) );
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "radar" ), &g_settings.visuals.radar ) );
					}

					auto other_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "other" ), 100 ); {
						other_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "bullet tracers" ), &g_settings.visuals.bullet_tracers, &dropdowns::tracer_type ) );
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "floating ragdolls" ), &g_settings.visuals.floating_ragdolls ) );
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "rank revealer" ), &g_settings.misc.rank_reveal ) );
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "target" ), &g_settings.visuals.target ) );
						other_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 1, 7, &g_settings.visuals.target_time, "s" )
						)->set_cond( [ ]( ) { return g_settings.visuals.target; } );
					}

					auto chams_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "chams" ), 165 ); {
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.visuals.chams.enabled( ) ) );
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "through walls" ), &g_settings.visuals.chams.ignore_z( ) ) );
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "clear occlusion" ), &g_settings.visuals.chams.clear_occlusion ) );
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "friendlies" ), &g_settings.visuals.chams.friendlies( ) ) );
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "flat" ), &g_settings.visuals.chams.flat( ) ) )->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs( ); } );
						chams_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "reflectivity" ), &g_settings.visuals.chams.reflectivity ) 
							)->set_cond( [ ]( ) { return !g_settings.visuals.chams.flat && !g_settings.misc.hide_from_obs; } );

						chams_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "shine" ), &g_settings.visuals.chams.shine )
						)->set_cond( [ ]( ) { return !g_settings.visuals.chams.flat && !g_settings.misc.hide_from_obs; } );

						chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "friendlies visible" ), &g_settings.visuals.chams.color_visible_friendly( ) ) )->set_cond(
							[ ]( ) { return g_settings.visuals.chams.friendlies( ); } );
						chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "friendlies hidden" ), &g_settings.visuals.chams.color_hidden_friendly( ) ) )->set_cond(
							[ ]( ) { return g_settings.visuals.chams.friendlies( ) && g_settings.visuals.chams.ignore_z( ); } );
						chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "enemies visible" ), &g_settings.visuals.chams.color_visible_enemy( ) ) );
						chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "enemies hidden" ), &g_settings.visuals.chams.color_hidden_enemy( ) ) )->set_cond(
							[ ]( ) { return g_settings.visuals.chams.ignore_z( ); } );
					}

					chams_form->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs( ); } );
					
					left_column->add_item( main_form );
					left_column->add_item( player_form );

					right_column->add_item( hud_form );
					right_column->add_item( other_form );
					right_column->add_item( chams_form );

					subtab_players->add_item( left_column );
					subtab_players->add_item( right_column );
				}

				auto subtab_other = std::make_shared< ui::c_tab_sheet >( xors( "other" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto world_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "world" ) ); {
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "world modulate" ), &g_settings.visuals.world_modulate ) 
						)->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs( ); } );
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "night mode" ), &g_settings.visuals.night_mode ) 
						)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate && !g_settings.misc.hide_from_obs( ); } );
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "transparent props" ), &g_settings.visuals.transparent_props )
						)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate && !g_settings.misc.hide_from_obs( ); } );

						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "fullbright" ), &g_settings.visuals.fullbright ) );
						world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "dropped weapons" ), &g_settings.visuals.weapon_esp, &dropdowns::world_esp_type )
						)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.weapon_esp_clr( ) ) );

						world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "grenades" ), &g_settings.visuals.grenade_esp, &dropdowns::world_esp_type )
						)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.grenade_esp_clr( ) ) );

						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "grenade owner" ), &g_settings.visuals.grenade_owner )
						)->set_cond( [ ]( ) { return g_settings.visuals.grenade_esp == 1 || g_settings.visuals.grenade_esp == 3; } );

						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "bomb information" ), &g_settings.visuals.bomb_info ) );
					}

					auto local_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "local player" ) ); {
						local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "recoil crosshair" ), &g_settings.visuals.recoil_crosshair( ) ) );
						local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "draw spread" ), &g_settings.visuals.draw_spread ) );
						local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "autowall crosshair" ), &g_settings.visuals.autowall_crosshair( ) ) );
						local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "grenade prediction" ), &g_settings.visuals.grenade_prediction( ) ) );
						local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "snipers crosshair" ), &g_settings.visuals.snipers_crosshair( ) ) );
					}

					auto view_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "view" ) ); {
						view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "watermark" ), &g_settings.misc.watermark ) );
						view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "thirdperson" ), &g_settings.misc.thirdperson( ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.thirdperson_key )
						);
						view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 40, xors( "viewmodel fov" ), &g_settings.misc.viewmodel_fov( ) ) );
						view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 90, 130, xors( "view fov" ), &g_settings.misc.fov_changer( ) ) );
					}

					auto removals_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "removals" ) ); {
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no scope" ), &g_settings.misc.no_scope ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no zoom" ), &g_settings.misc.no_zoom ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no recoil" ), &g_settings.misc.no_recoil ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no flash" ), &g_settings.misc.no_flash ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no smoke" ), &g_settings.misc.no_smoke ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "transparent viewmodel" ), &g_settings.misc.transparent_vm ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no post processing" ), &g_settings.misc.disable_post_process ) );
					}

					left_column->add_item( world_form );
					left_column->add_item( local_form );

					right_column->add_item( view_form );
					right_column->add_item( removals_form );

					subtab_other->add_item( left_column );
					subtab_other->add_item( right_column );
				}

				subtab_sheet->add_item( subtab_other );
				subtab_sheet->add_item( subtab_players );

				tab_visuals->add_item( subtab_sheet );
			}


			auto tab_misc = std::make_shared< ui::c_tab_sheet >( xors( "miscellaneous" ), &icons::sprite_misc ); {
				auto subtab_sheet = std::make_shared< ui::c_subtab_manager >( );

				auto subtab_general = std::make_shared< ui::c_tab_sheet >( xors( "general" ) ); {
					auto column_left  = subtab_general->add_item( std::make_shared< ui::base_item >( 0, 0, 0, 0 ) );
					auto column_right = subtab_general->add_item( std::make_shared< ui::base_item >( 220, -5, 0, 0 ) );

					auto movement_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "movement" ), 145 ); {
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto strafe" ), &g_settings.misc.auto_strafe ) );
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "bhop" ), &g_settings.misc.bunny_hop( ) ) );
						auto circle_box = movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "circle strafe" ), &g_settings.misc.circle_strafe ) );
						circle_box->add_item( std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.circle_strafe_key ) );
						circle_box->set_cond( [ ]( ) { return g_settings.misc.bunny_hop && g_settings.misc.auto_strafe; } );

						movement_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("no duck cooldown"), &g_settings.misc.no_crouch_cooldown));

						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "air duck" ), &g_settings.misc.air_duck ) );

						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "edge jump" ), &g_settings.misc.edge_jump( ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.edge_jump_key( ) )
						);
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto jumpbug" ), &g_settings.misc.auto_jumpbug( ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.auto_jumpbug_key( ) )
						);
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "show jump stats" ), &g_settings.misc.show_jump_stats ) );

						movement_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("fake duck"), &g_settings.misc.fake_duck()))->add_item(
							std::make_shared< ui::c_key_picker_small >(195, 1, &g_settings.misc.fake_duck_key())
						);
					}

					auto identity_form = std::make_shared< ui::c_form >( 0, 1, 215, 0, xors( "identity" ) ); {
						identity_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "clantag changer" ), &g_settings.misc.clantag_changer( ) ) );
						identity_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "name changer" ), &g_settings.misc.name_changer,
							&dropdowns::name_changers ) );
						identity_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "rich presence" ), &g_settings.misc.rich_presence( ) ) );
						identity_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("money talk"), &g_settings.misc.money_talk));
					}

					auto fake_ping = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "fake latency" ) ); {
						fake_ping->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "enabled" ), &g_settings.misc.net_fakelag,
							&dropdowns::fake_ping_activation ) )->add_item( std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.net_fakeping_key )
							)->set_cond( [ ]( ) { return g_settings.misc.net_fakelag == 1 || g_settings.misc.net_fakelag == 2; } );
						fake_ping->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 800, xors( "amount" ), &g_settings.misc.net_fakeping_amount( ) ) 
						)->set_cond( [ ]( ) { return g_settings.misc.net_fakelag != 4; } );
					}

					auto helpers_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "helpers" ) ); {
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto revolver" ), &g_settings.misc.auto_revolver ) );
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto match accept" ), &g_settings.misc.auto_accept ) );
						helpers_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 10, xors( "accept delay" ), &g_settings.misc.auto_accept_delay ) 
						)->set_cond( [ ]( ) { return g_settings.misc.auto_accept( ); } );
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "server information" ), &g_settings.misc.server_information ) );
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "unlock inventory" ), &g_settings.misc.unlock_inventory ) );
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "log hits" ), &g_settings.misc.log_hits ) );
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "show team damage" ), &g_settings.misc.team_dmg ) );
					}

					auto autobuy_form = std::make_shared< ui::c_form>( 0, 0, 215, 0, xors( "autobuy" ), 165 ); {
						autobuy_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "primary" ), &g_settings.misc.autobuy.main_weapon,
						 &dropdowns::autobuy_main_wep ) );
						autobuy_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "secondary" ), &g_settings.misc.autobuy.secondary_weapon,
						 &dropdowns::autobuy_second_wep ) );
						autobuy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "armor" ), &g_settings.misc.autobuy.armor ) );
						autobuy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "defuser" ), &g_settings.misc.autobuy.defuser ) );
						autobuy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "zeus" ), &g_settings.misc.autobuy.zeus ) );

						auto grenades_form = autobuy_form->add_item( std::make_shared< ui::c_form >( 0, 5, 139, 0, xors( "grenades" ), 0, true ) );

						grenades_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "molotov/incendiary" ), &g_settings.misc.autobuy.molly ) );
						grenades_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "grenade" ), &g_settings.misc.autobuy.grenade ) );
						grenades_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "smoke" ), &g_settings.misc.autobuy.smoke ) );
						grenades_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "flash" ), &g_settings.misc.autobuy.flash ) );
						grenades_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "decoy" ), &g_settings.misc.autobuy.decoy ) );
					}

					auto other_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "other" ) ); {
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "remove gun sounds" ), &g_settings.misc.no_sound ) );
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "hide from obs" ), &g_settings.misc.hide_from_obs ) );
						other_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "unlock hidden cvars" ), []( ) { g_cheat.m_extra.unlock_hidden_cvars( ); } ) );
					}

					column_left->add_item( movement_form );
					column_left->add_item( identity_form );
					column_left->add_item( fake_ping );
					column_left->add_item( other_form ); 

					column_right->add_item( helpers_form );
					column_right->add_item( autobuy_form );
				}

				auto subtab_skins = std::make_shared< ui::c_tab_sheet >( xors( "money compensator" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto knife_form = left_column->add_item( std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "knife" ) ) ); {
						knife_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "override knife" ), &g_settings.misc.skins.knife, &dropdowns::skin_knives ) );
					}

					auto skin_form = left_column->add_item( std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "skin" ), 325 ) ); {
						skin_form->add_item( std::make_shared< ui::c_itemlist< int, 20 > >( 0, 0, 180, xors( "override skin" ), &dropdowns::skins, &g_settings.misc.skins.current_skin ) );					
					}

					auto gloves_form = right_column->add_item( std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "glove" ) ) ); {
						gloves_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "override glove" ), &g_settings.misc.skins.glove, &dropdowns::gloves ) );
					}

					auto gloves_skin_form = right_column->add_item( std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "glove skin" ), 325 ) ); {
						gloves_skin_form->add_item( std::make_shared< ui::c_itemlist< int, 20 > >( 0, 0, 180, xors( "override skin" ), &dropdowns::glove_skins, &g_settings.misc.skins.glove_skin ) );
					}

					subtab_skins->add_item( left_column );
					subtab_skins->add_item( right_column );
				}

				auto subtab_movement_recorder = std::make_shared< ui::c_tab_sheet >( xors( "movement recorder" ) ); {
					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ) ); {
						main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "idle" ) ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.misc.recorder_enable ) );
						main_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "show angles" ), &g_settings.misc.recording_show_angles, &dropdowns::recorder_angles ) );
						main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "recording start key" ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.recording_start_key )
						);
						main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "recording stop key" ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.recording_stop_key )
						);

						main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "play" ), [ ]( ) { g_cheat.m_move_rec.play_back( ); } ) );
						main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "stop playing" ), [ ]( ) { g_cheat.m_move_rec.stop_playback( ); } ) );
						main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "clear recording" ), [ ]( ) { g_cheat.m_move_rec.clear_recording( ); } ) );
					}

					subtab_movement_recorder->add_item( main_form );
				}

				subtab_sheet->add_item( subtab_movement_recorder );
				subtab_sheet->add_item( subtab_skins );
				subtab_sheet->add_item( subtab_general );

				tab_misc->add_item( subtab_sheet );
			}


			auto tab_config = std::make_shared< ui::c_tab_sheet >( xors( "config" ), &icons::sprite_config ); {
				auto cfg_form = std::make_shared< ui::c_form >( 0, 10, 200, 93, xors( "config" ) );
				cfg_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 180, xors( "setting" ), &g_settings.menu.cur_setting, &dropdowns::configs ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 0, 0, 85, 20, xors( "save" ), []( ) { g_settings.save( ); } ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 95, -25, 85, 20, xors( "load" ), []( ) { g_settings.load( ); } ) );
				//cfg_form->add_item( std::make_shared< ui::c_button >( 0, -25, 85, 20, xors( "to clipboard" ), [ ]( ) { g_settings.copy_to_clipboard( ); } ) );
				//cfg_form->add_item( std::make_shared< ui::c_button >( 95, -50, 85, 20, xors( "from clipboard" ), [ ]( ) { g_settings.load_from_clipboard( ); } ) );
				cfg_form->add_item( std::make_shared< ui::c_checkbox >( 0, -25, xors( "anti-untrusted" ), &g_settings.menu.anti_untrusted ) );

				tab_config->add_item( cfg_form );
			}

			tab_sheet->add_item( tab_config );
			tab_sheet->add_item( tab_misc );
			tab_sheet->add_item( tab_visuals );
			tab_sheet->add_item( tab_rage );
			tab_sheet->add_item( tab_legit );

			menu->add_item( tab_sheet );

			was_setup = true;
		}
		else {
			static auto movement_state = menu->find_item(
				xors( "movement recorder" ) )->find_item(
					xors( "main" ) )->find_item(
						xors( "idle" )
					);

			if( !g_settings.misc.recorder_enable )
				movement_state->set_text( xors( "idle" ) );
			else if( g_cheat.m_move_rec.is_playing( ) ) {
				movement_state->set_text( xors( "playing" ) );
			}
			else if( g_cheat.m_move_rec.is_recording( ) ) {
				movement_state->set_text( xors( "recording" ) );
			}
			else {
				movement_state->set_text( xors( "idle" ) );
			}

			static auto jitter_real = menu->find_item( xors( "real yaw jitter" ) );
			jitter_real->set_text( g_settings.rage.real_yaw == 2 ? xors( "spin range" ) : xors( "real yaw jitter" ) );

			static auto jitter_real_moving = menu->find_item( xors( "real yaw moving jitter" ) );
			jitter_real_moving->set_text( g_settings.rage.real_yaw_moving == 2 ? xors( "spin range" ) : xors( "real yaw moving jitter" ) );

			static auto jitter_fake = menu->find_item( xors( "fake yaw jitter" ) );
			jitter_fake->set_text( g_settings.rage.fake_yaw == 2 ? xors( "spin range" ) : xors( "fake yaw jitter" ) );

			static auto jitter_air = menu->find_item( xors( "in-air yaw jitter" ) );
			jitter_air->set_text( g_settings.rage.air_yaw == 2 ? xors( "spin range" ) : xors( "in-air yaw jitter" ) );

			render_item( menu.get( ) );

			//reset the menu if home is pressed for debugging purposes
			if ( g_input.is_key_pressed( KEYS_END ) ) {
				menu.reset( );
				menu = std::make_shared< c_menu >( 10, 10, 580, 470, xors( "golden cheat" ) );
				was_setup = false;
			}
		}
	}
}