#pragma once
#include "CGlobalVarsBase.h"
class IPlayerInfoManager {
public:
	virtual void pad_000( ) = 0;
	virtual CGlobalVarsBase* GetGlobalVars( ) = 0;
};