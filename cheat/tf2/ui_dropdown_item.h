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

		static std::vector< dropdown_item_t< > > activation_types_aimbot = {
			{ xors( "always" ), 0 },
			{ xors( "on key" ), 1 },
		};

		static std::vector< dropdown_item_t< > > silent_aimbot_types = {
			{ xors( "none" ), 0 },
			{ xors( "client-side" ), 1 },
			{ xors( "server-side" ), 2 },
		};

		static std::vector< dropdown_item_t< > > hitboxes = {
			{ "head", 0 },
			{ "neck", 1 },
			{ "pelvis", 3 },
			{ "chest", 6 },
			{ "thorax", 5 },
		};

		static std::vector< dropdown_item_t< > > tracer_type = {
			{ xors( "none" ), 0 },
			{ xors( "line" ), 1 },
			{ xors( "beam" ), 2 }
		};

		static std::vector< dropdown_item_t< > > world_esp_type = {
			{ xors( "off" ), 0 },
			{ xors( "name" ), 1 },
			{ xors( "glow" ), 2 },
			{ xors( "both" ), 3 }
		};

		static std::vector< dropdown_item_t< > > weapon_esp_types = {
			{ xors( "none" ), 0 },
			{ xors( "icon" ), 1 },
			{ xors( "name" ), 2 },
		};

		static std::vector< dropdown_item_t< > > activation_type_visuals = {
			{ xors( "never" ), 0 },
			{ xors( "always" ), 1 },
			{ xors( "on hold" ), 2 },
			{ xors( "toggle" ), 3 },
		};

		static std::vector< dropdown_item_t< > > aimbot_bone = {
			{ "head", 0 },
			{ "neck", 1 },
			{ "pelvis", 3 },
			{ "chest", 6 },
			{ "thorax", 5 },
		};

		static std::vector< dropdown_item_t< > > configs = {
			{ xors( "scout" ), 0 },
			{ xors( "soldier" ), 1 },
			{ xors( "pyro" ), 2 },
			{ xors( "demo" ), 3 },
			{ xors( "heavy" ), 4 },
			{ xors( "engineer" ), 5 },
			{ xors( "medic" ), 6 },
			{ xors( "sniper" ), 7 },
			{ xors( "spy" ), 8 },
			{ xors( "rage 1" ), 9 },
			{ xors( "rage 2" ), 10 },
		};

	}
}