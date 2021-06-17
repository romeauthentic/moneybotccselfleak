#pragma once

#include "ui_base_item.h"

namespace ui
{
	namespace dropdowns
	{
		template < typename t = int >
		struct dropdown_item_t {
			const char* m_name;
			t			m_value;
		};

		std::vector< dropdown_item_t< > > activation_types_aimbot = {
			{ xors( "always" ), 0 },
			{ xors( "on key" ), 1 },
		};

		std::vector< dropdown_item_t< > > silent_aimbot_types = {
			{ xors( "none" ), 0 },
			{ xors( "client-side" ), 1 },
			{ xors( "server-side" ), 2 },
		};

		std::vector< dropdown_item_t< > > hitboxes = {
			{ xors( "hitscan" ), -1 },
			{ xors( "head" ), 0 },
			{ xors( "neck" ), 1 },
			{ xors( "pelvis" ), 2 },
			{ xors( "chest" ), 3 },
		};

		std::vector< dropdown_item_t< > > antiaim_yaw = {
			{ xors( "normal" ), 0 },
			{ xors( "backwards" ), 1 },
			{ xors( "backwards spin" ), 2 },
			{ xors( "direction" ), 6 },
			{ xors( "sideways" ), 3 },
			{ xors( "golden spin" ), 4 },
			{ xors( "random" ), 5 },
		};

		std::vector< dropdown_item_t< > > delay_type = {
			{ xors( "none" ), 0 },
			{ xors( "fake lag" ), 1 },
			{ xors( "time" ), 2 },
			{ xors( "velocity" ), 3 },
		};

		std::vector< dropdown_item_t< > > antiaim_pitch = {
			{ xors( "normal" ), 0 },
			{ xors( "custom" ), 1 },
			{ xors( "flip" ), 2 },
			{ xors( "minimal" ), 3 },
			{ xors( "random" ), 4 }
		};

		std::vector< dropdown_item_t< > > antiaim_freestanding = {
			{ xors( "disabled" ), 0 },
			{ xors( "static" ), 1 },
			{ xors( "narrow angle" ), 2 },
			{ xors( "wide angle" ), 3 },
		};

		std::vector< dropdown_item_t< > > antiaim_edge = {
			{ xors( "off" ), 0 },
			{ xors( "peek" ), 1 },
			{ xors( "full" ), 2 },
		};

		std::vector< dropdown_item_t< > > antiaim_edge_type = {
			{ xors( "static" ), 0 },
			{ xors( "narrow angle" ), 1 },
			{ xors( "wide angle" ), 2 },
		};

		std::vector< dropdown_item_t< > > fakelag_types = {
			{ xors( "disabled" ), 0 },
			{ xors( "adaptive" ), 1 },
			{ xors( "fluctuate" ), 2 },
			{ xors( "static" ), 3 },
		};

		std::vector< dropdown_item_t< > > activation_type_visuals = {
			{ xors( "never" ), 0 },
			{ xors( "always" ), 1 },
			{ xors( "on hold" ), 2 },
			{ xors( "toggle" ), 3 },
		};

		std::vector< dropdown_item_t< > > configs = {
			{ xors( "legit" ), 0 },
			{ xors( "closet" ), 1 },
			{ xors( "rage 1" ), 2 },
			{ xors( "rage 2" ), 3 },
		};

		std::vector< dropdown_item_t< > > recorder_angles = {
			{ xors( "none" ), 0 },
			{ xors( "shot" ), 1 },
			{ xors( "all" ), 2 }
		};

		std::vector< dropdown_item_t< > > baim_types = {
			{ xors( "off" ), 0 },
			{ xors( "basic" ), 1 },
			{ xors( "smart" ), 2 },
			{ xors( "aggressive" ), 3 },
			{ xors( "force" ), 4 },
		};

		std::vector< dropdown_item_t< > > name_changers = {
			{ xors( "off" ), 0 },
			{ xors( "dollar" ), 1 },
			{ xors( "moneybot.cc" ), 2 },
		};

		std::vector< dropdown_item_t< > > weapon_esp_types = {
			{ xors( "none" ), 0 },
			{ xors( "icon" ), 1 },
			{ xors( "name" ), 2 },
		};

		std::vector< dropdown_item_t< > > fake_ping_activation = {
			{ xors( "off" ), 0 },
			{ xors( "on hold" ), 1 },
			{ xors( "toggle" ), 2 },
			{ xors( "always" ), 3 },
			{ xors( "auto" ), 4 }
		};

		std::vector< dropdown_item_t< > > world_esp_type = {
			{ xors( "off" ), 0 },
			{ xors( "name" ), 1 },
			{ xors( "glow" ), 2 },
			{ xors( "both" ), 3 }
		};

		std::vector< dropdown_item_t< > > multipoint_types = {
			{ xors( "off" ), 0 },
			{ xors( "minimal" ), 1 },
			{ xors( "vitals" ), 2 },
			{ xors( "full" ), 3 }
		};

		std::vector< dropdown_item_t< > > edge_priority = {
			{ xors( "freestanding" ), 0 },
			{ xors( "edge" ), 1 }
		};

		std::vector< dropdown_item_t< > > selection_type = {
			{ xors( "fov" ), 0 },
			{ xors( "distance" ), 1 }
		};

		std::vector< dropdown_item_t< > > autobuy_main_wep = {
			{ xors( "none" ), 0 },
			{ xors( "autosniper" ), 1 },
			{ xors( "scout" ), 2 },
			{ xors( "sniper" ), 3 },
			{ xors( "ak/m4" ), 4 },
			{ xors( "aug/sg" ), 5 },
			{ xors( "mac10/mp9" ), 6 },
		};

		std::vector< dropdown_item_t< > > autobuy_second_wep = {
			{ xors( "none" ), 0 },
			{ xors( "deagle/r8" ), 1 },
			{ xors( "dualies" ), 2 },
		};

		std::vector< dropdown_item_t< > > skin_knives = {
			{ xors( "none" ), 0 },
			{ xors( "bayonet" ), 1 },
			{ xors( "flip knife" ), 2 },
			{ xors( "gut knife" ), 3 },
			{ xors( "karambit" ), 4 },
			{ xors( "m9 bayonet" ), 5 },
			{ xors( "huntsman" ), 6 },
		};
	}
}