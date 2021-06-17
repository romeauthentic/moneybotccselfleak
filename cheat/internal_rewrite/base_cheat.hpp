#pragma once
#include "util.hpp"
#include "movement.hpp"
#include "prediction.hpp"
#include "legitbot.hpp"
#include "chams.hpp"
#include "autowall.hpp"
#include "ragebot.hpp"
#include "identity.hpp"
#include "lag_mgr.hpp"
#include "visual.hpp"
#include "extra.hpp"
#include "movement_recorder.hpp"
#include "player_manager.hpp"
#include "skins.hpp"

NAMESPACE_REGION( features )

class c_base_cheat {
public:
	//have your cheat classes in here
	c_movement		 m_movement;
	c_prediction	 m_prediction;
	c_legitbot		 m_legitbot;
	c_autowall		 m_autowall;
	c_ragebot		 m_ragebot;
	c_identity		 m_identity;
	c_chams			 m_chams;
	c_lagmgr		 m_lagmgr;
	c_extra			 m_extra;
	c_skins			 m_skins;
	c_visuals		 m_visuals;
	c_move_recorder  m_move_rec;
	c_player_manager m_player_mgr;
};

END_REGION

extern features::c_base_cheat g_cheat;