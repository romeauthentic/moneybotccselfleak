#pragma once
#include "aim.h"
#include "movement.h"
#include "prediction.h"
#include "visuals.hpp"
#include "chams.h"
#include "identity.h"
#include "extra.h"
namespace tf2 {
	class c_base_cheat {
	public:
		features::c_legitbot aim;
		features::c_movement movement;
		features::c_prediction prediction;
		features::c_visuals visuals;
		features::c_chams chams;
		features::c_identity identity;
		features::c_extra extra;
	};
}

static tf2::c_base_cheat g_cheat;