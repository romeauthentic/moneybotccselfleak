#pragma once

class AppSystemInfo_t;

enum InitReturnVal_t;

enum AppSystemTier_t {
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};


class IAppSystem {
public:
	virtual bool					Connect( void* factory ) = 0;
	virtual void					Disconnect( ) = 0;
	virtual void*					QueryInterface( const char* pInterfaceName ) = 0;
	virtual InitReturnVal_t			Init( ) = 0;
	virtual void					Shutdown( ) = 0;
	virtual const AppSystemInfo_t*	GetDependencies( ) = 0;
	virtual AppSystemTier_t			GetTier( ) = 0;
	virtual void					Reconnect( void* factory, const char* pInterfaceName ) = 0;
	virtual bool					IsSingleton( ) = 0;
};