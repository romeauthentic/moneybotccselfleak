#pragma once
#include "simple_settings.h"
#include "strings.hpp"
#include "color.hpp"
#include <unordered_map>
//later i will implement a way to load all config files

namespace data
{


	extern SettingHolder holder_;


	class legitbot_settings : SettingHolder {
	public:
		using SettingHolder::SettingHolder;

		con_var< float > m_fov{ this, fnv( "fov" ), 5.f };
		con_var< float > m_speed{ this, fnv( "speed" ), 10.f };
		con_var< float > m_assist_strength{ this, fnv( "assist_strength" ), 0.f };
		con_var< int > m_hitbox{ this, fnv( "hitbox" ), 0 };
		con_var< int > m_bone{ this, fnv( "bone" ), 0 };
	};


	class c_settings {
	public:

		void load( ) {
			switch( menu.cur_setting ) {
			case 0:
				holder_.load( "", xors( "./tf/cfg/scout.cfg" ) );
				break;
			case 1:
				holder_.load( "", xors( "./tf/cfg/soldier.cfg" ) );
				break;
			case 2:
				holder_.load( "", xors( "./tf/cfg/pyro.cfg" ) );
				break;
			case 3:
				holder_.load( "", xors( "./tf/cfg/demo.cfg" ) );
				break;
			case 4:
				holder_.load( "", xors( "./tf/cfg/heavy.cfg" ) );
				break;
			case 5:
				holder_.load( "", xors( "./tf/cfg/engineer.cfg" ) );
				break;
			case 6:
				holder_.load( "", xors( "./tf/cfg/medic.cfg" ) );
				break;
			case 7:
				holder_.load( "", xors( "./tf/cfg/sniper.cfg" ) );
				break;
			case 8:
				holder_.load( "", xors( "./tf/cfg/spy.cfg" ) );
				break;
			case 9:
				holder_.load( "", xors( "./tf/cfg/money_rage1.cfg" ) );
				break;
			case 10:
				holder_.load( "", xors( "./tf/cfg/money_rage2.cfg" ) );
				break;
			}

			menu.anti_untrusted = true;
		}

		void save( ) {
			switch( menu.cur_setting ) {
			case 0:
				holder_.load( "", xors( "./tf/cfg/scout.cfg" ) );
				break;
			case 1:
				holder_.load( "", xors( "./tf/cfg/soldier.cfg" ) );
				break;
			case 2:
				holder_.load( "", xors( "./tf/cfg/pyro.cfg" ) );
				break;
			case 3:
				holder_.load( "", xors( "./tf/cfg/demo.cfg" ) );
				break;
			case 4:
				holder_.load( "", xors( "./tf/cfg/heavy.cfg" ) );
				break;
			case 5:
				holder_.load( "", xors( "./tf/cfg/engineer.cfg" ) );
				break;
			case 6:
				holder_.load( "", xors( "./tf/cfg/medic.cfg" ) );
				break;
			case 7:
				holder_.load( "", xors( "./tf/cfg/sniper.cfg" ) );
				break;
			case 8:
				holder_.load( "", xors( "./tf/cfg/spy.cfg" ) );
				break;
			case 9:
				holder_.load( "", xors( "./tf/cfg/money_rage1.cfg" ) );
				break;
			case 10:
				holder_.load( "", xors( "./tf/cfg/money_rage2.cfg" ) );
				break;
			}
		}

		void copy_to_clipboard( ) {
			holder_.save( "", xors( "./tf/cfg/money_temp.cfg" ) );
			std::vector< uint8_t > file_data;
			FILE* f = fopen( xors( "./tf/cfg/money_temp.cfg" ), "r" );
			size_t size = 0;
			fseek( f, 0, SEEK_END );
			size = ftell( f );

			char* buf = ( char* )malloc( size );
			fread( buf, 1, size, f );
			fclose( f );
			for( size_t i{ }; i < size; ++i ) {
				file_data.push_back( buf[ i ] );
			}

			free( buf );

			EmptyClipboard( );
			OpenClipboard( nullptr );

			auto g = GlobalAlloc( GMEM_FIXED, size );
			memcpy( g, file_data.data( ), size );

			SetClipboardData( CF_TEXT, g );
			CloseClipboard( );
			//DeleteFileA( xors( "./tf/cfg/money_temp.cfg" ) );
		}

		void load_from_clipboard( ) {
			FILE* f = fopen( xors( "./tf/cfg/money_temp.cfg" ), "wb" );
			auto data = GetClipboardData( CF_TEXT );

			auto size = GlobalSize( data );
			fwrite( data, 1, size, f );
			fclose( f );

			holder_.load( "", xors( "./tf/cfg/money_temp.cfg" ) );

			CloseClipboard( );
			//DeleteFileA( xors( "./tf/cfg/money_temp.cfg" ) );
		}

		struct {
			con_var< bool > enabled{ &holder_, fnv( "legit_active" ), false };
			con_var< size_t > sample_size{ &holder_, fnv( "legit_sample_size" ), 12 };
			con_var< float > smooth_factor{ &holder_, fnv( "legit_smooth_factor" ), 1.0f };
			con_var< bool > dynamic_smoothing{ &holder_, fnv( "legit_dynamic_smoothing" ), false };
			con_var< bool > assist{ &holder_, fnv( "legit_assist" ), false };
			con_var< bool > friendlies{ &holder_, fnv( "legit_friendlies" ), false };
			con_var< int > activation_type{ &holder_, fnv( "legit_activation_type" ), 1 };
			con_var< int > key{ &holder_, fnv( "legit_key" ), 0 };
			con_var< bool > backtracking{ &holder_, fnv( "legit_backtracking" ), 0 };
			con_var< float > backtracking_time{ &holder_, fnv( "legit_bt_time" ), 0.2f };
			con_var< float > backtracking_fov{ &holder_, fnv( "legit_bt_fov" ), 5.0f };
			con_var< bool > backtracking_target{ &holder_, fnv( "legit_bt_target" ), false };
			con_var< bool > backtracking_visualize{ &holder_, fnv( "legit_bt_visualize" ), 0 };
			con_var< clr_t > backtracking_col{ &holder_, fnv( "legit_bt_col" ), clr_t( 255, 255, 255, 120 ) };
			con_var< bool > triggerbot{ &holder_, fnv( "legit_triggerbot" ), false };
			con_var< int > trigger_key{ &holder_, fnv( "legit_triggerkey" ), 0 };
			con_var< float > trigger_delay{ &holder_, fnv( "legit_triggerdelay" ), 0.f };
			legitbot_settings secondary{ &holder_, "legit_primary" };
			legitbot_settings melee{ &holder_, "legit_melee" };
			legitbot_settings primary{ &holder_, "legit_secondary" };
			legitbot_settings* active = &primary;
		} legit;

		struct {
			struct {
				con_var< bool > enabled{ &holder_, fnv( "visuals_chams_enabled" ), 0 };
				con_var< bool > flat{ &holder_, fnv( "visuals_chams_flat" ), 0 };
				con_var< bool > ignore_z{ &holder_, fnv( "visuals_chams_ignore_z" ), 0 };
				con_var< float > reflectivity{ &holder_, fnv( "visuals_chams_reflectivity" ), 1.0f };
				con_var< float > shine{ &holder_, fnv( "visuals_chams_shine" ), 1.0f };
				con_var< bool > friendlies{ &holder_, fnv( "visuals_chams_friendlies" ), 0 };
				con_var< clr_t > color_visible_friendly{ &holder_, fnv( "visuals_chams_color_friendly" ), clr_t( 0, 255, 255 ) };
				con_var< clr_t > color_visible_enemy{ &holder_, fnv( "visuals_chams_color_enemy" ), clr_t( 255, 255, 0 ) };
				con_var< clr_t > color_hidden_friendly{ &holder_, fnv( "visuals_chams_color_h_friendly" ), clr_t( 0, 0, 255 ) };
				con_var< clr_t > color_hidden_enemy{ &holder_, fnv( "visuals_chams_color_h_enemy" ), clr_t( 255, 0, 0 ) };
			} chams;
			
			con_var< int > activation_type{ &holder_, fnv( "visuals_activation" ), false };
			//players
			con_var< bool > box{ &holder_, fnv( "visuals_box" ), false };
			con_var< bool > dormant{ &holder_, fnv( "visuals_dormant" ), false };
			con_var< bool > health{ &holder_, fnv( "visuals_health" ), false };
			con_var< bool > weapon{ &holder_, fnv( "visuals_weapon" ), false };
			con_var< bool > name{ &holder_, fnv( "visuals_name" ), false };
			con_var< bool > out_of_pov{ &holder_, fnv( "visuals_out_of_pov" ), false };
			con_var< float > out_of_pov_radius{ &holder_, fnv( "visuals_out_of_pov_radius" ), 0.75f };
			con_var< int >  out_of_pov_size{ &holder_, fnv( "visuals_out_of_pov_size" ), 25 };
			con_var< bool > friendlies{ &holder_, fnv( "visuals_friendlies" ), false };
			con_var< bool > skeleton{ &holder_, fnv( "visuals_skeleton" ) };
			con_var< bool > glow{ &holder_, fnv( "visuals_glow" ), false };
			con_var< bool > conditions{ &holder_, fnv( "visuals_conditions" ), false }; //conditions
			con_var< bool > resolver_indicator{ &holder_, fnv( "visuals_resolver_indicator" ), false };
			con_var< bool > ammo{ &holder_, fnv( "visuals_ammo" ), false };
			con_var< clr_t > ammo_bar_clr{ &holder_, fnv( "visuals_ammo_bar_clr" ), clr_t( 0, 255, 255 ) };
			con_var< bool > player_class{ &holder_, fnv( "visuals_player_class" ) };
			con_var< bool > floating_ragdolls{ &holder_, fnv( "visuals_floating_ragdolls" ) }; //dont remember
			con_var< bool > sound{ &holder_, fnv( "visuals_sound" ), false };
			con_var< clr_t > box_enemy{ &holder_, fnv( "visuals_clr_enemy" ), clr_t( 0, 255, 255 ) };
			con_var< clr_t > box_friendly{ &holder_, fnv( "visuals_clr_friendly" ), clr_t( 255, 0, 255 ) };
			con_var< clr_t > glow_enemy{ &holder_, fnv( "visuals_glow_enemy" ), clr_t( 255, 0, 255, 150 ) };
			con_var< clr_t > glow_friendly{ &holder_, fnv( "visuals_glow_friendly" ), clr_t( 255, 120, 0, 150 ) };
			con_var< clr_t > skeleton_enemy{ &holder_, fnv( "visuals_skeleton_enemy" ), clr_t( 255, 255, 255 ) };
			con_var< clr_t > skeleton_friendly{ &holder_, fnv( "visuals_skeleton_friendly" ), clr_t( 255, 255, 255 ) };
			con_var< int > bullet_tracers{ &holder_, fnv( "visuals_bullet_tracers" ), 0 };
			con_var< int > sound_range{ &holder_, fnv( "visuals_sound_range" ), 360 };
			con_var< int > key{ &holder_, fnv( "visuals_key" ), 0 };
			con_var< bool > hitmarkers{ &holder_, fnv( "visuals_hitmarkers" ), false };

			
			//world
			con_var< bool > world_modulate{ &holder_, fnv( "visuals_world_modulate" ), false };
			con_var< bool > night_mode{ &holder_, fnv( "visuals_night_mode" ), false };
			con_var< bool > transparent_props{ &holder_, fnv( "visuals_transparent_props" ), false };
			con_var< int > projectile_esp{ &holder_, fnv( "visuals_projectile_esp" ), 0 }; //projectile_esp
			con_var< int > building_esp{ &holder_, fnv( "visuals_building_esp" ), 0 };//building_esp
			con_var< bool > building_owner{ &holder_, fnv( "visuals_building_owner" ), false }; //building_owner
			con_var< clr_t > projectile_esp_clr{ &holder_, fnv( "visuals_projectile_esp_clr" ), clr_t( 255, 255, 255, 180 ) };
			con_var< clr_t > building_esp_clr{ &holder_, fnv( "visuals_building_esp_clr" ), clr_t( 255, 255, 255, 180 ) };
			con_var< bool > fullbright{ &holder_, fnv( "visuals_fullbright" ), false };
			con_var< bool > objective_info{ &holder_, fnv( "visuals_objective_info" ), false }; //objective_info
			
			//local
			con_var< bool > draw_spread{ &holder_, fnv( "misc_draw_spread" ), false };
			con_var< bool > grenade_prediction{ &holder_, fnv( "visuals_grenade_prediction" ), false };
			con_var< bool > spec_list{ &holder_, fnv( "visuals_spec_list" ), false };

			bool active = true;
		} visuals;

		struct {
			con_var< bool > watermark{ &holder_, fnv( "misc_watermark" ), false };
			con_var< bool > bunny_hop{ &holder_, fnv( "misc_bhop" ), false };
			con_var< bool > auto_strafe{ &holder_, fnv( "misc_autostrafe" ), false };
			con_var< int > viewmodel_fov{ &holder_, fnv( "misc_viewmodel_fov" ), 0 };
			con_var< int > fov_changer{ &holder_, fnv( "fov_changer" ), 90 };
			con_var< bool > thirdperson{ &holder_, fnv( "misc_thirdperson" ), false };
			con_var< int > thirdperson_key{ &holder_, fnv( "misc_tp_key" ), 0 };

			con_var< bool > no_recoil{ &holder_, fnv( "misc_no_recoil" ), false };
			con_var< bool > no_sound{ &holder_, fnv( "misc_no_sound" ), false };
			con_var< bool > no_scope{ &holder_, fnv( "misc_no_scope" ), false };
			con_var< bool > transparent_vm{ &holder_, fnv( "misc_transparent_vm" ), false };
			con_var< bool > hide_from_obs{ &holder_, fnv( "misc_hide_from_obs" ), false };
			con_var< bool > disable_post_process{ &holder_, fnv( "misc_disable_post_process" ), false };

			bool recorder_enable = false;
		} misc;

		struct {
			con_var< uint8_t > menu_color_1r{ &holder_, fnv( "menu_color_1r" ), 231 };
			con_var< uint8_t > menu_color_1g{ &holder_, fnv( "menu_color_1g" ), 105 };
			con_var< uint8_t > menu_color_1b{ &holder_, fnv( "menu_color_1b" ), 105 };

			con_var< uint8_t > menu_color_2r{ &holder_, fnv( "menu_color_2r" ), 0xf4 };
			con_var< uint8_t > menu_color_2g{ &holder_, fnv( "menu_color_2g" ), 0x7c };
			con_var< uint8_t > menu_color_2b{ &holder_, fnv( "menu_color_2b" ), 0xa8 };

			bool auto_class_settings = false;
			bool anti_untrusted = true;
			bool open = false;
			bool logs_enable = false;
			int cur_setting = 0;
		} menu;
	};
}

extern data::c_settings g_settings;