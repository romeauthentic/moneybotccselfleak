#pragma once

#include "CLuaInterface.hpp"

class CLuaShared
{
public:
	virtual ~CLuaShared( );
	virtual void Init( );
	virtual void ShutDown( );
	virtual void DumpStats( );
	virtual CLuaInterface* CreateLuaInterface( STATE_TYPE, bool );
	virtual void CloseLuaInterface( CLuaInterface* );
	virtual CLuaInterface* GetLuaInterface( STATE_TYPE );
};