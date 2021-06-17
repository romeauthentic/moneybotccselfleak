#pragma once
#include "simple_settings.hpp"
#include "strings.hpp"
#include "color.hpp"

//later i will implement a way to load all config files

namespace data
{
	extern SettingHolder holder_;

	class c_settings {
	public:
		void load( ) {
			switch( menu.cur_setting ) {
			case 0:
				holder_.load( "", xors( "./garrysmod/cfg/money_legit.cfg" ) );
				break;
			case 1:
				holder_.load( "", xors( "./garrysmod/cfg/money_closet.cfg" ) );
				break;
			case 2:
				holder_.load( "", xors( "./garrysmod/cfg/money_rage1.cfg" ) );
				break;
			case 3:
				holder_.load( "", xors( "./garrysmod/cfg/money_rage2.cfg" ) );
				break;
			}
		}

		void save( ) {
			switch( menu.cur_setting ) {
			case 0:
				holder_.save( "", xors( "./garrysmod/cfg/money_legit.cfg" ) );
				break;
			case 1:
				holder_.save( "", xors( "./garrysmod/cfg/money_closet.cfg" ) );
				break;
			case 2:
				holder_.save( "", xors( "./garrysmod/cfg/money_rage1.cfg" ) );
				break;
			case 3:
				holder_.save( "", xors( "./garrysmod/cfg/money_rage2.cfg" ) );
				break;
			}
		}

		struct {
			con_var< bool > enabled{ &holder_, fnv( "rage_enabled" ), false };
			con_var< int > activation_type{ &holder_, fnv( "rage_activation_type" ), 0 };
			con_var< float > fov{ &holder_, fnv( "rage_fov" ), 0.f };
			con_var< int > hitbox{ &holder_, fnv( "rage_hitbox" ), false };
			con_var< bool > auto_fire{ &holder_, fnv( "rage_auto_fire" ), false };

			struct {
				con_var< bool > head{ &holder_, fnv( "rage_hitscan_head" ), true };
				con_var< bool > body{ &holder_, fnv( "rage_hitscan_body" ), true };
				con_var< bool > arms{ &holder_, fnv( "rage_hitscan_arms" ), false };
				con_var< bool > legs{ &holder_, fnv( "rage_hitscan_legs" ), false };
			} hitscan;

			con_var< bool > nospread{ &holder_, fnv( "rage_no_spread" ), false };
			con_var< bool > norecoil{ &holder_, fnv( "rage_no_recoil" ), false };
			con_var< bool > ignore_limbs_moving{ &holder_, fnv( "rage_ignore_limbs_moving" ), false };
			con_var< int > silent{ &holder_, fnv( "rage_silent" ), false };
			con_var< int > aimkey{ &holder_, fnv( "rage_key" ), 0 };
			con_var< bool > auto_scope{ &holder_, fnv( "rage_auto_scope" ), false };
			con_var< bool > auto_stop{ &holder_, fnv( "rage_auto_stop" ), false };
			con_var< bool > spread_limit{ &holder_, fnv( "rage_spread_limit" ), false };
			con_var< float > min_spread{ &holder_, fnv( "rage_min_spread" ), 100.f };
			con_var< bool > resolver{ &holder_, fnv( "rage_resolver" ), false };
			con_var< bool > resolver_override{ &holder_, fnv( "rage_resolver_override" ), false };
			con_var< int > resolver_override_key{ &holder_, fnv( "override_key" ), 0 };
			con_var< bool > bt_scan_all{ &holder_, fnv( "rage_scan_all_records" ), false };
			con_var< bool > bt_visualize{ &holder_, fnv( "rage_visualize_records" ), false };
			con_var< clr_t > bt_col{ &holder_, fnv( "rage_bt_color" ), clr_t( 150, 150, 150, 20 ) };
			con_var< bool > preserve_fps{ &holder_, fnv( "rage_preserve_fps" ), false };
			con_var< int > multipoint{ &holder_, fnv( "rage_multipoint" ), false };
			con_var< float > point_scale{ &holder_, fnv( "rage_point_scale" ), 1.0f };

			con_var< int > selection_type{ &holder_, fnv( "rage_selection_type" ) };
			con_var< bool > prefer_moving{ &holder_, fnv( "rage_prefer_moving" ) };
			con_var< bool > prefer_low_hp{ &holder_, fnv( "rage_low_hp" ) };

			con_var< bool > ignore_team{ &holder_, fnv( "rage_ignoreteam" ), false };
			con_var< bool > ignore_teamcolor{ &holder_, fnv( "rage_ignoreteamcolor" ), false };
			con_var< bool > ignore_spawnprot{ &holder_, fnv( "rage_ignorespawnprot" ), false };
			con_var< bool > ignore_noclip{ &holder_, fnv( "rage_ignorenoclip" ), false };



			struct {
				con_var< int > mode{ &holder_, fnv( "fakelag_mode" ), 0 };
				con_var< int > ticks{ &holder_, fnv( "fakelag_ticks" ), 0 };
				con_var< int > fluctuate{ &holder_, fnv( "fakelag_fluctuate" ), 0 };
				con_var< bool > in_air{ &holder_, fnv( "fakelag_in_air" ), false };
				con_var< bool > on_peek{ &holder_, fnv( "fakelag_on_peek" ), false };
				con_var< bool > in_move{ &holder_, fnv( "fakelag_in_move" ), false };
				con_var< bool > in_attack{ &holder_, fnv( "fakelag_in_attack" ), false };
				con_var< bool > avoid_ground{ &holder_, fnv( "fakelag_avoid_ground" ), false };
			} fakelag;

			con_var< bool > anti_aim{ &holder_, fnv( "rage_anti_aim" ), false };
			con_var< bool > dormant_check{ &holder_, fnv( "rage_dormant_check" ), false };
			con_var< bool > at_players{ &holder_, fnv( "rage_at_players" ), false };
			con_var< int > pitch{ &holder_, fnv( "rage_pitch" ), 0 };
			con_var< int > pitch_offset{ &holder_, fnv( "rage_pitch_offset" ), 0 };

			con_var< int > real_yaw{ &holder_, fnv( "rage_real_yaw" ), 0 };
			con_var< int > real_yaw_add{ &holder_, fnv( "rage_real_yaw_add" ), 0 };
			con_var< int > real_yaw_jitter{ &holder_, fnv( "rage_real_jitter" ), 0 };

			con_var< int > fake_yaw{ &holder_, fnv( "rage_fake_yaw" ), 0 };
			con_var< int > fake_yaw_add{ &holder_, fnv( "rage_fake_yaw_add" ), 0 };
			con_var< int > fake_yaw_jitter{ &holder_, fnv( "rage_fake_jitter" ), 0 };

			con_var< int > real_yaw_moving{ &holder_, fnv( "rage_real_yaw_moving" ), 0 };
			con_var< int > real_yaw_moving_add{ &holder_, fnv( "rage_real_yaw_moving_add" ), 0 };
			con_var< int > real_moving_jitter{ &holder_, fnv( "rage_real_moving_jitter" ), 0 };

			con_var< int > fake_yaw_moving{ &holder_, fnv( "rage_fake_yaw_moving" ), 0 };
			con_var< int > fake_yaw_moving_add{ &holder_, fnv( "rage_fake_yaw_moving_add" ), 0 };
			con_var< int > fake_moving_jitter{ &holder_, fnv( "rage_fake_moving_jitter" ), 0 };

			con_var< int > edge_detection{ &holder_, fnv( "rage_edge_dtc" ), 0 };
			con_var< int > edge_dtc_jitter{ &holder_, fnv( "rage_edge_dtc_jitter" ), 0 };

			con_var< int > edge_dtc_normal{ &holder_, fnv( "rage_edge_dtc_normal" ), 0 };
			con_var< int > edge_dtc_type{ &holder_, fnv( "rage_edge_dtc_type" ), 0 };
			con_var< int > edge_dtc_standing{ &holder_, fnv( "rage_edge_dtc_standing" ), 100 };
			con_var< int > edge_dtc_moving{ &holder_, fnv( "rage_edge_dtc_moving" ), 100 };
			con_var< int > edge_dtc_priority{ &holder_, fnv( "rage_edge_dtc_priority" ), 0 };

			con_var< bool > break_lby{ &holder_, fnv( "rage_break_lby" ), 0 };
			con_var< int > lby_delta{ &holder_, fnv( "rage_lby_delta" ), 0 };

			con_var< bool > fakewalk{ &holder_, fnv( "rage_fakewalk" ), 0 };
			con_var< int > fakewalk_key{ &holder_, fnv( "rage_fakewalk_key" ), 0 };
			con_var< int > fakewalk_ticks{ &holder_, fnv( "rage_fakewalk_ticks" ), 7 };

			con_var< bool > draw_angles{ &holder_, fnv( "rage_draw_angles" ), false };
			con_var< clr_t > real_color{ &holder_, fnv( "rage_real_color" ), clr_t( 255, 255, 255, 150 ) };
			con_var< clr_t > lby_color{ &holder_, fnv( "rage_lby_color" ), clr_t( 231, 105, 105, 150 ) };

			con_var< bool > angle_step{ &holder_, fnv( "rage_angle_step" ), false };
			con_var< int > angle_step_degrees{ &holder_, fnv( "range_angle_step_degrees" ), false };
		} rage;

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
			con_var< bool > box{ &holder_, fnv( "visuals_box" ), false };
			con_var< bool > dormant{ &holder_, fnv( "visuals_dormant" ), false };
			con_var< bool > health{ &holder_, fnv( "visuals_health" ), false };
			con_var< bool > weapon{ &holder_, fnv( "visuals_weapon" ), false };
			con_var< bool > name{ &holder_, fnv( "visuals_name" ), false };
			con_var< bool > rank{ &holder_, fnv( "visuals_rank" ), false };
			con_var< bool > out_of_pov{ &holder_, fnv( "visuals_out_of_pov" ), false };
			con_var< bool > ignore_team{ &holder_, fnv( "visuals_ignoreteam" ), false };
			con_var< bool > ignore_teamcolor{ &holder_, fnv( "visuals_ignoreteamcolor" ), false };

			con_var< bool > skeleton{ &holder_, fnv( "visuals_skeleton" ) };
			con_var< bool > glow{ &holder_, fnv( "visuals_glow" ), false };
			con_var< bool > flags{ &holder_, fnv( "visuals_flags" ), false };
			con_var< bool > resolver_indicator{ &holder_, fnv( "visuals_resolver_indicator" ), false };
			con_var< bool > ammo{ &holder_, fnv( "visuals_ammo" ), false };
			con_var< bool > money{ &holder_, fnv( "visuals_money" ) };
			con_var< bool > ping{ &holder_, fnv( "visuals_ping" ) };
			con_var< bool > floating_ragdolls{ &holder_, fnv( "visuals_floating_ragdolls" ) };
			con_var< clr_t > box_enemy{ &holder_, fnv( "visuals_clr_enemy" ), clr_t( 0, 255, 255 ) };
			con_var< clr_t > box_friendly{ &holder_, fnv( "visuals_clr_friendly" ), clr_t( 255, 0, 255 ) };
			
			con_var< clr_t > glow_enemy{ &holder_, fnv( "visuals_glow_enemy" ), clr_t( 255, 0, 255, 150 ) };
			con_var< clr_t > glow_friendly{ &holder_, fnv( "visuals_glow_friendly" ), clr_t( 255, 120, 0, 150 ) };
			con_var< clr_t > skeleton_enemy{ &holder_, fnv( "visuals_skeleton_enemy" ), clr_t( 255, 255, 255 ) };
			con_var< clr_t > skeleton_friendly{ &holder_, fnv( "visuals_skeleton_friendly" ), clr_t( 255, 255, 255 ) };

			con_var< bool > bullet_tracers{ &holder_, fnv( "visuals_bullet_tracers" ), false };

			con_var< int > key{ &holder_, fnv( "visuals_key" ), 0 };
			con_var< bool > hitmarkers{ &holder_, fnv( "visuals_hitmarkers" ), false };
			bool active = true;

			con_var< bool > world_modulate{ &holder_, fnv( "visuals_world_modulate" ), false };
			con_var< bool > night_mode{ &holder_, fnv( "visuals_night_mode" ), false };
			con_var< bool > transparent_props{ &holder_, fnv( "visuals_transparent_props" ), false };

			con_var< bool > recoil_crosshair{ &holder_, fnv( "misc_recoil_crosshair" ), false };
			con_var< bool > autowall_crosshair{ &holder_, fnv( "visual_recoil_crosshair" ), false };
			con_var< bool > draw_spread{ &holder_, fnv( "visual_draw_spread" ), false };
			con_var< bool > snipers_crosshair{ &holder_, fnv( "visuals_sniper_crosshair" ), false };
			con_var< bool > grenade_prediction{ &holder_, fnv( "visuals_grenade_prediction" ), false };

			con_var< int > weapon_esp{ &holder_, fnv( "visuals_weapon_esp" ), 0 };
			con_var< int > grenade_esp{ &holder_, fnv( "visuals_weapon_esp" ), 0 };

			con_var< clr_t > weapon_esp_clr{ &holder_, fnv( "visuals_weapon_esp_clr" ), clr_t( 255, 255, 255, 180 ) };
			con_var< clr_t > grenade_esp_clr{ &holder_, fnv( "visuals_grenade_esp_clr" ), clr_t( 255, 255, 255, 180 ) };

			con_var< bool > spec_list{ &holder_, fnv( "visuals_spec_list" ), false };
			con_var< bool > radar{ &holder_, fnv( "visuals_radar" ), false };
		} visuals;

		struct {
			con_var< bool > watermark{ &holder_, fnv( "misc_watermark" ), false };
			con_var< bool > bunny_hop{ &holder_, fnv( "misc_bhop" ), false };
			con_var< bool > auto_strafe{ &holder_, fnv( "misc_autostrafe" ), false };
			con_var< bool > circle_strafe{ &holder_, fnv( "misc_circlestrafe" ), false };
			con_var< int > circle_strafe_key{ &holder_, fnv( "misc_circle_key" ), 0 };
			con_var< bool > edge_jump{ &holder_, fnv( "misc_edgejump" ), false };
			con_var< int > edge_jump_key{ &holder_, fnv( "misc_edge_jump_key" ), false };
			con_var< bool > auto_jumpbug{ &holder_, fnv( "misc_auto_jumpbug" ), false };
			con_var< int > auto_jumpbug_key{ &holder_, fnv( "misc_auto_jumpbug_key" ), 0 };
			con_var< bool > show_jump_stats{ &holder_, fnv( "misc_show_jump_stats" ), 0 };
			con_var< int > viewmodel_fov{ &holder_, fnv( "misc_viewmodel_fov" ), 0 };
			con_var< int > fov_changer{ &holder_, fnv( "fov_changer" ), 90 };
			con_var< bool > clantag_changer{ &holder_, fnv( "misc_clantag" ), false };
			con_var< bool > log_hits{ &holder_, fnv( "misc_log_hits" ) };
			con_var< int > name_changer{ &holder_, fnv( "misc_namechanger" ), 0 };
			con_var< bool > thirdperson{ &holder_, fnv( "misc_thirdperson" ), false };
			con_var< int > thirdperson_key{ &holder_, fnv( "misc_tp_key" ), 0 };
			con_var< bool > auto_accept{ &holder_, fnv( "misc_autoaccept" ), false };
			con_var< int > auto_accept_delay{ &holder_, fnv( "misc_autoaccept_delay" ), 0 };
			con_var< int > net_fakelag{ &holder_, fnv( "misc_net_fakelag" ), false };
			con_var< int > net_fakeping_amount{ &holder_, fnv( "net_fakeping_amount" ), 0 };
			con_var< int > net_fakeping_key{ &holder_, fnv( "net_fakeping_key" ), 0 };
			bool net_fakeping_active = false;

			con_var< bool > rank_reveal{ &holder_, fnv( "misc_rank_reveal" ), 0 };
			con_var< bool > unlock_inventory{ &holder_, fnv( "misc_unlock_inventory" ), 0 };
			con_var< bool > auto_revolver{ &holder_, fnv( "misc_auto_revolver" ), 0 };
			con_var< bool > no_scope{ &holder_, fnv( "misc_no_scope" ), false };
			con_var< bool > no_recoil{ &holder_, fnv( "misc_no_recoil" ), false };
			con_var< bool > no_smoke{ &holder_, fnv( "misc_no_smoke" ), false };
			con_var< bool > no_flash{ &holder_, fnv( "misc_no_flash" ), false };
			con_var< bool > transparent_vm{ &holder_, fnv( "misc_transparent_vm" ), false };
			con_var< bool > disable_post_process{ &holder_, fnv( "misc_disable_post_process" ), false };
			con_var< int > recording_start_key{ &holder_, fnv( "recording_start_key" ), 0 };
			con_var< int > recording_stop_key{ &holder_, fnv( "recording_stop_key" ), 0 };
			con_var< int > recording_show_angles{ &holder_, fnv( "recording_show_angles" ), 0 };

			con_var< bool > use_spam{ &holder_, fnv( "misc_use_spam" ), false };
			con_var< int > use_spam_key{ &holder_, fnv( "misc_use_spam_key" ), 0 };

			bool recorder_enable = false;
		} misc;

		struct {
			con_var< uint8_t > menu_color_1r{ &holder_, fnv( "menu_color_1r" ), 231 };
			con_var< uint8_t > menu_color_1g{ &holder_, fnv( "menu_color_1g" ), 105 };
			con_var< uint8_t > menu_color_1b{ &holder_, fnv( "menu_color_1b" ), 105 };

			con_var< uint8_t > menu_color_2r{ &holder_, fnv( "menu_color_2r" ), 0xf4 };
			con_var< uint8_t > menu_color_2g{ &holder_, fnv( "menu_color_2g" ), 0x7c };
			con_var< uint8_t > menu_color_2b{ &holder_, fnv( "menu_color_2b" ), 0xa8 };

			bool open = false;
			bool logs_enable = false;
			int cur_setting = 0;
		} menu;
	};
}

extern data::c_settings g_settings;