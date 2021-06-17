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

#include "settings.hpp"
#include "base_cheat.hpp"

namespace ui
{
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
			menu = std::make_shared< ui::c_menu >( 10, 10, 580, 470, xors( "moneybot" ),
				date_str.c_str( ) );
			auto tab_sheet = std::make_shared< ui::c_tab_manager >( );

			//todo: replace with icons?

			auto tab_rage = std::make_shared< ui::c_tab_sheet >( xors( "rage" ), &icons::sprite_rage ); {
				auto subtab_sheet = std::make_shared< ui::c_subtab_manager >( );

				auto subtab_aimbot = std::make_shared< ui::c_tab_sheet >( xors( "aimbot" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ), 0 ); {
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.rage.enabled ) );
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

						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "autofire" ), &g_settings.rage.auto_fire ) );
					}

					auto accuracy_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "accuracy" ) ); {
						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no spread" ), &g_settings.rage.nospread ) );
						accuracy_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no recoil" ), &g_settings.rage.norecoil ) );
					}

					auto selection_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "selection" ) ); {
						selection_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "type" ), &g_settings.rage.selection_type, &dropdowns::selection_type ) );
					}

					auto filter_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "filters" ) ); {
						filter_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore team" ), &g_settings.rage.ignore_team ) );
						filter_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore team color" ), &g_settings.rage.ignore_teamcolor ) );
						filter_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore spawn protection" ), &g_settings.rage.ignore_spawnprot ) );
					}

					left_column->add_item( main_form );
					right_column->add_item( accuracy_form );
					right_column->add_item( selection_form );
					right_column->add_item( filter_form );

					subtab_aimbot->add_item( left_column );
					subtab_aimbot->add_item( right_column );
				}

				//auto subtab_antiaim = std::make_shared< ui::c_tab_sheet >( xors( "antiaim" ) ); {
				//	auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
				//	auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );
				//
				//	auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ) ); {
				//		main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.rage.anti_aim ) );
				//		main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "at players" ), &g_settings.rage.at_players ) );
				//		main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "dormant check" ), &g_settings.rage.dormant_check ) );
				//		main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "angle step" ), &g_settings.rage.angle_step ) );
				//		main_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 45, &g_settings.rage.angle_step_degrees ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.angle_step( ); } );
				//	}
				//
				//	auto pitch_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "pitch" ) ); {
				//		pitch_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "pitch" ), &g_settings.rage.pitch, &dropdowns::antiaim_pitch ) );
				//		pitch_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -89, 89, &g_settings.rage.pitch_offset ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.pitch == 1; } );
				//	}
				//
				//	auto yaw_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "yaw" ), 200 ); {
				//		yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "real yaw" ), &g_settings.rage.real_yaw, &dropdowns::antiaim_yaw ) );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -100, 100, &g_settings.rage.real_yaw_add( ) ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "real yaw jitter" ), &g_settings.rage.real_yaw_jitter ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "real yaw moving" ), &g_settings.rage.real_yaw_moving, &dropdowns::antiaim_yaw ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.real_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -100, 100, &g_settings.rage.real_yaw_moving_add( ) ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.real_yaw_moving( ) && !!g_settings.rage.real_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "real yaw moving jitter" ), &g_settings.rage.real_moving_jitter ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.real_yaw_moving( ) && !!g_settings.rage.real_yaw( ); } );
				//
				//		yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "fake yaw" ), &g_settings.rage.fake_yaw, &dropdowns::antiaim_yaw ) );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -100, 100, &g_settings.rage.fake_yaw_add( ) ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.fake_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "fake yaw jitter" ), &g_settings.rage.fake_yaw_jitter ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.fake_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "fake yaw moving" ), &g_settings.rage.fake_yaw_moving, &dropdowns::antiaim_yaw ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.fake_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, -100, 100, &g_settings.rage.fake_yaw_moving_add( ) ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.fake_yaw_moving( ) && !!g_settings.rage.fake_yaw( ); } );
				//		yaw_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 90, xors( "fake yaw moving jitter" ), &g_settings.rage.fake_moving_jitter ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.fake_yaw_moving( ) && !!g_settings.rage.fake_yaw( ); } );
				//	}
				//	
				//	auto edge_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "edge detection" ), 110 ); {
				//		edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "edge" ), &g_settings.rage.edge_dtc_normal,
				//			&dropdowns::antiaim_edge ) );
				//
				//		edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 70, 300, &g_settings.rage.edge_dtc_standing ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.edge_dtc_normal( ) == 2; } );
				//
				//		edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 70, 300, &g_settings.rage.edge_dtc_moving ) )->set_cond(
				//			[ ]( ) { return !!g_settings.rage.edge_dtc_normal( ); } );
				//
				//		edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "edge type" ), &g_settings.rage.edge_dtc_type,
				//			&dropdowns::antiaim_edge_type ) )->set_cond( [ ]( ) { return g_settings.rage.edge_dtc_normal( ) == 2; } );
				//
				//		edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "freestanding" ), 
				//			&g_settings.rage.edge_detection, &dropdowns::antiaim_freestanding ) );
				//
				//		edge_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "priority" ),
				//			&g_settings.rage.edge_dtc_priority, &dropdowns::edge_priority )
				//		)->set_cond( [ ]( ) { return g_settings.rage.edge_dtc_normal( ) && g_settings.rage.edge_detection( ); } );
				//
				//		edge_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 45, xors( "fake jitter" ), &g_settings.rage.edge_dtc_jitter ) );
				//	}
				//
				//	auto lby_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "lby breaker" ) ); {
				//		lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "break lby" ), &g_settings.rage.break_lby ) );
				//		lby_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 105, 195, &g_settings.rage.lby_delta ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.break_lby( ); } );
				//
				//		auto angles_box = lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "draw angles" ), &g_settings.rage.draw_angles ) );
				//		angles_box->set_cond( [ ]( ) { return g_settings.rage.break_lby( ); } );
				//		angles_box->add_item( std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.rage.lby_color ) );
				//		angles_box->add_item( std::make_shared< ui::c_color_picker >( 180, -9, 15, &g_settings.rage.real_color ) );
				//
				//		lby_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "fakewalk" ), &g_settings.rage.fakewalk ) )->add_item(
				//			std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.rage.fakewalk_key )
				//		);
				//		lby_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 7, 13, &g_settings.rage.fakewalk_ticks ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.fakewalk; }	);
				//	}
				//
				//	auto fakelag_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "fakelag" ), 165 ); {
				//		fakelag_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 140, xors( "mode" ), &g_settings.rage.fakelag.mode, &dropdowns::fakelag_types ) );
				//		fakelag_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 15, xors( "max ticks" ), &g_settings.rage.fakelag.ticks ) );
				//		fakelag_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 140, 0, 50, xors( "fluctuate" ), &g_settings.rage.fakelag.fluctuate ) )->set_cond(
				//			[ ]( ) { return g_settings.rage.fakelag.mode == 2; }
				//		);
				//
				//		auto flags_form = fakelag_form->add_item( std::make_shared< ui::c_form >( 0, 5, 139, 0, xors( "flags" ), 0, true ) );
				//
				//		flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "in air" ), &g_settings.rage.fakelag.in_air ) );
				//		flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "on move" ), &g_settings.rage.fakelag.in_move ) );
				//		flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "on peek" ), &g_settings.rage.fakelag.on_peek ) );
				//		flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "while shooting" ), &g_settings.rage.fakelag.in_attack ) );
				//		flags_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "avoid ground" ), &g_settings.rage.fakelag.avoid_ground ) );
				//	}
				//
				//	left_column->add_item( main_form );
				//	left_column->add_item( pitch_form );
				//	left_column->add_item( yaw_form );
				//
				//	right_column->add_item( edge_form );
				//	right_column->add_item( lby_form );
				//	right_column->add_item( fakelag_form );
				//
				//	subtab_antiaim->add_item( left_column );
				//	subtab_antiaim->add_item( right_column );
				//}

				//subtab_sheet->add_item( subtab_antiaim );
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
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore team" ), &g_settings.visuals.ignore_team ) );
						main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "ignore team color" ), &g_settings.visuals.ignore_teamcolor ) );

					}

					auto player_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "players" ) ); {
						auto box_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "box" ), &g_settings.visuals.box ) );
						box_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 4, 15, &g_settings.visuals.box_enemy( ) ) );

						auto skeleton_checkbox = player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "skeleton" ), &g_settings.visuals.skeleton ) );
						skeleton_checkbox->add_item( std::make_shared< ui::c_color_picker >( 160, 2, 15, &g_settings.visuals.skeleton_enemy ) );
						
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "health" ), &g_settings.visuals.health ) );
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "name" ), &g_settings.visuals.name ) );
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "weapon" ), &g_settings.visuals.weapon ) );
						player_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "rank" ), &g_settings.visuals.rank ) );
					}

					auto hud_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "hud" ) ); {
						// hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "hitmarkers" ), &g_settings.visuals.hitmarkers ) ); will probably add soon.
						hud_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "spectator list" ), &g_settings.visuals.spec_list ) );
					}

					//auto chams_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "chams" ) ); { soon
					//	chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.visuals.chams.enabled( ) ) );
					//	chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "through walls" ), &g_settings.visuals.chams.ignore_z( ) ) );
					//	chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "friendlies" ), &g_settings.visuals.chams.friendlies( ) ) );
					//	chams_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "flat" ), &g_settings.visuals.chams.flat( ) ) );
					//	chams_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "reflectivity" ), &g_settings.visuals.chams.reflectivity ) 
					//		)->set_cond( [ ]( ) { return !g_settings.visuals.chams.flat; } );
					//
					//	chams_form->add_item( std::make_shared< ui::c_slider< float > >( 0, 0, 120, 0.f, 1.f, xors( "shine" ), &g_settings.visuals.chams.shine )
					//	)->set_cond( [ ]( ) { return !g_settings.visuals.chams.flat; } );
					//
					//	chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "friendlies visible" ), &g_settings.visuals.chams.color_visible_friendly( ) ) )->set_cond(
					//		[ ]( ) { return g_settings.visuals.chams.friendlies( ); } );
					//	chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "friendlies hidden" ), &g_settings.visuals.chams.color_hidden_friendly( ) ) )->set_cond(
					//		[ ]( ) { return g_settings.visuals.chams.friendlies( ) && g_settings.visuals.chams.ignore_z( ); } );
					//	chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "enemies visible" ), &g_settings.visuals.chams.color_visible_enemy( ) ) );
					//	chams_form->add_item( std::make_shared< ui::c_color_picker >( 0, 0, 120, xors( "enemies hidden" ), &g_settings.visuals.chams.color_hidden_enemy( ) ) )->set_cond(
					//		[ ]( ) { return g_settings.visuals.chams.ignore_z( ); } );
					//}
					
					left_column->add_item( main_form );
					left_column->add_item( player_form );

					right_column->add_item( hud_form );
					//right_column->add_item( chams_form );

					subtab_players->add_item( left_column );
					subtab_players->add_item( right_column );
				}

				auto subtab_other = std::make_shared< ui::c_tab_sheet >( xors( "other" ) ); {
					auto left_column = std::make_shared< ui::base_item >( 0, 0, 0, 0 );
					auto right_column = std::make_shared< ui::base_item >( 220, -5, 0, 0 );

					//auto world_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "world" ) ); {
					//	world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "world modulate" ), &g_settings.visuals.world_modulate ) );
					//	world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "night mode" ), &g_settings.visuals.night_mode ) 
					//	)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate; } );
					//	world_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "transparent props" ), &g_settings.visuals.transparent_props )
					//	)->set_cond( [ ]( ) { return g_settings.visuals.world_modulate; } );
					//
					//	world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "dropped weapons" ), &g_settings.visuals.weapon_esp, &dropdowns::world_esp_type )
					//	)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.weapon_esp_clr( ) ) );
					//
					//	world_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "grenades" ), &g_settings.visuals.grenade_esp, &dropdowns::world_esp_type )
					//	)->add_item( std::make_shared< ui::c_color_picker >( 180, 4, 15, &g_settings.visuals.grenade_esp_clr( ) ) );
					//}


					auto view_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "view" ) ); {
						view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "watermark" ), &g_settings.misc.watermark ) );
						// will add soon
						//view_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "thirdperson" ), &g_settings.misc.thirdperson( ) ) )->add_item( 
						//	std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.thirdperson_key )
						//);
						//view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 0, 40, xors( "viewmodel fov" ), &g_settings.misc.viewmodel_fov( ) ) );
						//view_form->add_item( std::make_shared< ui::c_slider< int > >( 0, 0, 120, 90, 130, xors( "view fov" ), &g_settings.misc.fov_changer( ) ) );
					}

					auto removals_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "removals" ) ); {
						removals_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "no recoil" ), &g_settings.misc.no_recoil ) );
					}

					left_column->add_item( view_form );
					left_column->add_item( removals_form );

					//right_column->add_item( view_form );
					//right_column->add_item( removals_form );

					subtab_other->add_item( left_column );
					//subtab_other->add_item( right_column );
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

					auto movement_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "movement" ) ); {
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto strafe" ), &g_settings.misc.auto_strafe ) );
						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "bhop" ), &g_settings.misc.bunny_hop( ) ) );
						//auto circle_box = movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "circle strafe" ), &g_settings.misc.circle_strafe ) );
						//circle_box->add_item( std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.circle_strafe_key ) );
						//circle_box->set_cond( [ ]( ) { return g_settings.misc.bunny_hop && g_settings.misc.auto_strafe; } );

						movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "edge jump" ), &g_settings.misc.edge_jump( ) ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.edge_jump_key( ) )
						);
						//movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "auto jumpbug" ), &g_settings.misc.auto_jumpbug( ) ) )->add_item(
						//	std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.auto_jumpbug_key( ) )
						//);
						//movement_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "show jump stats" ), &g_settings.misc.show_jump_stats ) );
					}

					//auto identity_form = std::make_shared< ui::c_form >( 0, 1, 215, 0, xors( "identity" ) ); {
					//	// need to add
					//	identity_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "name changer" ), &g_settings.misc.name_changer,
					//		&dropdowns::name_changers ) );
					//}


					auto helpers_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "helpers" ) ); {
						// e spammer
						helpers_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "use spammer" ), &g_settings.misc.use_spam ) )->add_item(
							std::make_shared< ui::c_key_picker_small >( 195, 1, &g_settings.misc.use_spam_key ) );

					}

				

					column_left->add_item( movement_form );
					//column_left->add_item( identity_form );

					column_right->add_item( helpers_form );
				}


				//auto subtab_movement_recorder = std::make_shared< ui::c_tab_sheet >( xors( "movement recorder" ) ); {
				//	auto main_form = std::make_shared< ui::c_form >( 0, 0, 215, 0, xors( "main" ) ); {
				//		main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "idle" ) ) );
				//		main_form->add_item( std::make_shared< ui::c_checkbox >( 0, 0, xors( "enabled" ), &g_settings.misc.recorder_enable ) );
				//		main_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 120, xors( "show angles" ), &g_settings.misc.recording_show_angles, &dropdowns::recorder_angles ) );
				//		main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "recording start key" ) ) )->add_item(
				//			std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.recording_start_key )
				//		);
				//		main_form->add_item( std::make_shared< ui::c_label >( 0, 0, xors( "recording stop key" ) ) )->add_item(
				//			std::make_shared< ui::c_key_picker_small >( 195, 0, &g_settings.misc.recording_stop_key )
				//		);
				//
				//		main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "play" ), [ ]( ) { g_cheat.m_move_rec.play_back( ); } ) );
				//		main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "stop playing" ), [ ]( ) { g_cheat.m_move_rec.stop_playback( ); } ) );
				//		main_form->add_item( std::make_shared< ui::c_button >( 0, 0, 120, 20, xors( "clear recording" ), [ ]( ) { g_cheat.m_move_rec.clear_recording( ); } ) );
				//	}
				//
				//	subtab_movement_recorder->add_item( main_form );
				//}
				//
				//subtab_sheet->add_item( subtab_movement_recorder );
				subtab_sheet->add_item( subtab_general );

				tab_misc->add_item( subtab_sheet );
			}


			auto tab_config = std::make_shared< ui::c_tab_sheet >( xors( "config" ), &icons::sprite_config ); {
				auto cfg_form = std::make_shared< ui::c_form >( 0, 10, 200, 75, xors( "config" ) );
				cfg_form->add_item( std::make_shared< ui::c_dropdown< > >( 0, 0, 180, xors( "setting" ), &g_settings.menu.cur_setting, &dropdowns::configs ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 0, 0, 85, 20, xors( "save" ), []( ) { g_settings.save( ); } ) );
				cfg_form->add_item( std::make_shared< ui::c_button >( 95, -25, 85, 20, xors( "load" ), []( ) { g_settings.load( ); } ) );

				tab_config->add_item( cfg_form );
			}

			tab_sheet->add_item( tab_config );
			tab_sheet->add_item( tab_misc );
			tab_sheet->add_item( tab_visuals );
			tab_sheet->add_item( tab_rage );
			//tab_sheet->add_item( tab_legit );

			menu->add_item( tab_sheet );

			was_setup = true;
		}
		else {	
			//static auto movement_state = menu->find_item(
			//	xors( "movement recorder" ) )->find_item(
			//		xors( "main" ) )->find_item(
			//			xors( "idle" )
			//		);
			//
			//if( !g_settings.misc.recorder_enable )
			//	movement_state->set_text( xors( "idle" ) );
			//else if( g_cheat.m_move_rec.is_playing( ) ) {
			//	movement_state->set_text( xors( "playing" ) );
			//}
			//else if( g_cheat.m_move_rec.is_recording( ) ) {
			//	movement_state->set_text( xors( "recording" ) );
			//}
			//else {
			//	movement_state->set_text( xors( "idle" ) );
			//}

			//static auto jitter_real = menu->find_item( xors( "real yaw jitter" ) );
			//jitter_real->set_text( g_settings.rage.real_yaw == 2 ? xors( "spin range" ) : xors( "real yaw jitter" ) );
			//
			//static auto jitter_real_moving = menu->find_item( xors( "real yaw moving jitter" ) );
			//jitter_real_moving->set_text( g_settings.rage.real_yaw_moving == 2 ? xors( "spin range" ) : xors( "real yaw moving jitter" ) );
			//
			//static auto jitter_fake = menu->find_item( xors( "fake yaw jitter" ) );
			//jitter_fake->set_text( g_settings.rage.fake_yaw == 2 ? xors( "spin range" ) : xors( "fake yaw jitter" ) );
			//
			//static auto jitter_fake_moving = menu->find_item( xors( "fake yaw moving jitter" ) );
			//jitter_fake_moving->set_text( g_settings.rage.fake_yaw_moving == 2 ? xors( "spin range" ) : xors( "fake yaw moving jitter" ) );

			render_item( menu.get( ) );

			//reset the menu if home is pressed for debugging purposes
			if ( g_input.is_key_pressed( KEYS_HOME ) ) {
				menu.reset( );
				menu = std::make_shared< c_menu >( 10, 10, 580, 470, xors( "golden cheat" ) );
				was_setup = false;
			}
		}
	}
}