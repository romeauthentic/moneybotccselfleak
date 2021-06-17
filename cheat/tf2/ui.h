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
//#include "ui_itemlist.h"
//#include "ui_itemlist_def.h"

#include "settings.h"
#include "base_cheat.h"

namespace ui
{
	int current_category = 0;

	static std::string date_str = std::string( "lol no rage tab lol this sucks | " ) + __DATE__;
	auto menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "quebot" ),
		__DATE__ );

	static void render( ) {
		static bool was_setup = false;
		if( !was_setup ) {
			std::transform( date_str.begin( ), date_str.end( ), date_str.begin( ),
				[ ]( char c ) { return ::tolower( c ); }
			);

			menu.reset( );
			menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "moneybot - tf2" ),
				date_str.c_str( ) );
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

						triggerbot_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 0.2f, xors( "trigger delay" ), &g_settings.legit.trigger_delay, "s" )
						)->set_cond( [ ] { return g_settings.legit.triggerbot( ); } );
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
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.primary.m_fov ) );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.primary.m_speed ) );
						general_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "bone - dropdown" ), &g_settings.legit.primary.m_bone, &ui::dropdowns::aimbot_bone ) );
						general_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.primary.m_assist_strength ) );
					}

					auto snipers_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "snipers" ) ); {
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.melee.m_fov ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.melee.m_speed ) );
						snipers_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "bone - dropdown" ), &g_settings.legit.melee.m_bone, &ui::dropdowns::aimbot_bone ) );
						snipers_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.melee.m_assist_strength ) );
					}

					auto rifles_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "rifles" ) ); {
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 40.f, xors( "fov" ), &g_settings.legit.secondary.m_fov ) );
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 50.f, xors( "speed" ), &g_settings.legit.secondary.m_speed ) );
						rifles_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "bone - dropdown" ), &g_settings.legit.secondary.m_bone, &ui::dropdowns::aimbot_bone ) );
						rifles_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 140, 0.f, 1.f, xors( "assist strength" ), &g_settings.legit.secondary.m_assist_strength ) );
					}



					left_column->add_item( general_form );
					left_column->add_item( snipers_form );
					right_column->add_item( rifles_form );

					subtab_weapons->add_item( left_column );
					subtab_weapons->add_item( right_column );
				}

				subtab_sheet->add_item( subtab_weapons );
				subtab_sheet->add_item( subtab_main );

				tab_legit->add_item( subtab_sheet );
			}


			auto tab_rage = std::make_shared< ui::c_tab_sheet >( xors( "rage" ), &icons::sprite_rage ); {


				
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
						auto box_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "top bar (health, name)" ), &g_settings.visuals.box ) );

						auto glow_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "glow" ), &g_settings.visuals.glow ) );
						glow_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 2, 15, &g_settings.visuals.glow_enemy ) );
						glow_checkbox->add_item( std::make_shared< ui::c_color_picker >( 180, -11, 15, &g_settings.visuals.glow_friendly ) )->set_cond(
							[ ]( ) { return g_settings.visuals.friendlies || g_settings.visuals.chams.friendlies; } );
						glow_checkbox->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs; } );

						auto skeleton_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "skeleton" ), &g_settings.visuals.skeleton ) );
						skeleton_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 2, 15, &g_settings.visuals.skeleton_enemy ) );
						skeleton_checkbox->add_item( std::make_shared< ui::c_color_picker >( 180, -11, 15, &g_settings.visuals.skeleton_friendly ) )->set_cond(
							[ ]( ) { return g_settings.visuals.friendlies; } );

				
						//i will skull fuck you kid
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "bottom bar (weapon, ammo)" ), &g_settings.visuals.weapon ) )->add_item(
							std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.visuals.ammo_bar_clr( ) ) );
						player_form->add_item(std::make_shared< ui::c_checkbox >(0, 0, xors("dormant"), &g_settings.visuals.dormant));
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "conditions - close" ), &g_settings.visuals.conditions ) );
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "class" ), &g_settings.visuals.player_class ) );
						//player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "resolver indicator - when i add hvh" ), &g_settings.visuals.resolver_indicator ) ); 
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "sound" ), &g_settings.visuals.sound ) )->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs; } );
						player_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 220, 500, &g_settings.visuals.sound_range ) )->set_cond( [ ]( ) { return g_settings.visuals.sound && !g_settings.misc.hide_from_obs; } );
					}

					auto hud_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "hud" ) ); {
						auto pov = hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "out of pov" ), &g_settings.visuals.out_of_pov ) );
						pov->add_item( std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.visuals.box_enemy( ) ) );
						pov->add_item( std::make_shared< ui::c_color_picker >( 180, -9, 15, &g_settings.visuals.box_friendly( ) ) )->set_cond(
							[ ]( ) { return g_settings.visuals.friendlies; } );

						hud_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.25f, 1.f, &g_settings.visuals.out_of_pov_radius )
						)->set_cond( [ ]( ) { return g_settings.visuals.out_of_pov( ); } );

						hud_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 15, 40, &g_settings.visuals.out_of_pov_size, xors( "px" ) )
						)->set_cond( [ ]( ) { return g_settings.visuals.out_of_pov( ); } );
						//hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "hitmarkers - not done" ), &g_settings.visuals.hitmarkers ) ); no need
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spectator list" ), &g_settings.visuals.spec_list ) );
					}

					auto other_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "other" ) ); {
						//other_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "bullet tracers - not done" ), &g_settings.visuals.bullet_tracers, &dropdowns::tracer_type ) ); no bullet_impact event or any equivalent
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "floating ragdolls - hopefully" ), &g_settings.visuals.floating_ragdolls ) );
					}

					auto chams_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "chams - works now (thanks nave)" ), 170 ); {
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.visuals.chams.enabled( ) ) );
						chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "through walls" ), &g_settings.visuals.chams.ignore_z( ) ) );
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
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "world modulate - not done" ), &g_settings.visuals.world_modulate )
						)->set_cond( [ ]( ) { return !g_settings.misc.hide_from_obs( ); } );
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "night mode - not done" ), &g_settings.visuals.night_mode )
						)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate && !g_settings.misc.hide_from_obs( ); } );
						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "transparent props - maybe" ), &g_settings.visuals.transparent_props ) //maybe?
						)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate && !g_settings.misc.hide_from_obs( ); } );

						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "fullbright - not done" ), &g_settings.visuals.fullbright ) );
						world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "projectiles" ), &g_settings.visuals.projectile_esp, &dropdowns::world_esp_type )
						)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.building_esp_clr( ) ) );

						world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "buildings" ), &g_settings.visuals.building_esp, &dropdowns::world_esp_type )
						)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.building_esp_clr( ) ) );

						world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "building owner - not done" ), &g_settings.visuals.building_owner )
						)->set_cond( [ ]( ) { return g_settings.visuals.building_esp == 1 || g_settings.visuals.building_esp == 3; } );
					}

					//auto local_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "local player" ) ); {
					//	local_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "projectile prediction - hopefully" ), &g_settings.visuals.grenade_prediction( ) ) );
					//}

					auto view_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "view" ) ); {
						view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "watermark" ), &g_settings.misc.watermark ) );
						view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "thirdperson - crash" ), &g_settings.misc.thirdperson( ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.thirdperson_key )
						);
						view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 40, xors( "viewmodel fov" ), &g_settings.misc.viewmodel_fov( ) ) );
						view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 90, 130, xors( "view fov - broken" ), &g_settings.misc.fov_changer( ) ) );
					}

					auto removals_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "removals" ) ); {
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no scope - not done" ), &g_settings.misc.no_scope ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "transparent viewmodel - hopefully" ), &g_settings.misc.transparent_vm ) );
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no post processing - not done" ), &g_settings.misc.disable_post_process ) );
					}

					left_column->add_item( world_form );
					//left_column->add_item( local_form );

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
					auto column_left = subtab_general->add_item( std::make_shared< ui::base_item >( 0, 0, 0, 0 ) );
					auto column_right = subtab_general->add_item( std::make_shared< ui::base_item >( 220, -5, 0, 0 ) );

					auto movement_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "movement" ) ); {
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto strafe" ), &g_settings.misc.auto_strafe ) );
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "bhop" ), &g_settings.misc.bunny_hop( ) ) );
					}

					auto identity_form = std::make_shared< ui::c_form >( 0, 1, 215, 0, xors( "timeshift - not done" ) ); {
						//enabled (key)
						//on attack
						//value
					}


					auto helpers_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "helpers - not done" ) ); {
						//no push
						//auto backstab
						//
					}

					auto other_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "other" ) ); {
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "remove gun sounds" ), &g_settings.misc.no_sound ) );
						other_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "hide from obs" ), &g_settings.misc.hide_from_obs ) );
						//other_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "unlock hidden cvars" ), [ ]( ) { g_cheat.extra.unlock_hidden_cvars( ); } ) );
					}

					column_left->add_item( movement_form );
					column_right->add_item( identity_form );
					column_right->add_item( helpers_form );
					column_left->add_item( other_form );

				}
				subtab_sheet->add_item( subtab_general );

				tab_misc->add_item( subtab_sheet );
			}


			auto tab_config = std::make_shared< ui::c_tab_sheet >( xors( "config" ), &icons::sprite_config ); {
				auto cfg_form = std::make_shared< ui::c_form >( 0, 10, 200, 93, xors( "config" ) );
				cfg_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 180, xors( "setting" ), &g_settings.menu.cur_setting, &dropdowns::configs ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 0, 0, 85, 20, xors( "save" ), [ ]( ) { g_settings.save( ); } ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 95, -25, 85, 20, xors( "load" ), [ ]( ) { g_settings.load( ); } ) );
				//cfg_form->add_item( std::make_shared< ui::c_button >( 0, -25, 85, 20, xors( "to clipboard" ), [ ]( ) { g_settings.copy_to_clipboard( ); } ) );
				//cfg_form->add_item( std::make_shared< ui::c_button >( 95, -50, 85, 20, xors( "from clipboard" ), [ ]( ) { g_settings.load_from_clipboard( ); } ) );
				cfg_form->add_item( std::make_shared< ui::c_checkbox >( 0, -25, xors( "anti-untrusted" ), &g_settings.menu.anti_untrusted ) );

				tab_config->add_item( cfg_form );
			}

			tab_sheet->add_item( tab_config );
			tab_sheet->add_item( tab_misc );
			tab_sheet->add_item( tab_visuals );
			//tab_sheet->add_item( tab_rage );
			tab_sheet->add_item( tab_legit );

			menu->add_item( tab_sheet );

			was_setup = true;
		}

			render_item( menu.get( ) );

			//reset the menu if home is pressed for debugging purposes
			if( g_input.is_key_pressed( KEYS_END ) ) {
				menu.reset( );
				menu = std::make_shared< c_menu >( 10, 10, 580, 470, xors( "golden cheat" ) );
				was_setup = false;
			}
		
	}
}