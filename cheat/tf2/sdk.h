#pragma once

#define CLIENT_DLL

#include <Windows.h>
#include <map>
#include <algorithm>

#include "vector.hpp"

#include "color.hpp"
#include "IClientMode.h"
#include "CHLClient.h"
#include "IEngineVGui.h"
#include "IClientEntityList.h"
#include "IEngineTrace.h"
#include "CGlobalVarsBase.h"
#include "ICvar.h"
#include "IVDebugOverlay.h"
#include "IVEngineClient.h"
#include "IPlayerInfoManager.h"
#include "CBasePlayer.h"
#include "IPanel.h"
#include "ISurface.h"
#include "GlowObject.h"
#include "IInputSystem.h"
#include "IVRenderView.h"
#include "CBaseWeapon.h"
#include "IVModelInfo.h"
#include "IGameMovement.h"
#include "IMaterialSystem.hpp"
#include "IGameEventManager.h"
#include "CInput.hpp"
#include "KeyValues.h"