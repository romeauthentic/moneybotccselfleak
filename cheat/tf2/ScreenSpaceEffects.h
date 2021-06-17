#pragma once
class KeyValues;

class IScreenSpaceEffect {
public:

	virtual void Init( ) = 0;
	virtual void Shutdown( ) = 0;

	virtual void SetParameters( KeyValues *params ) = 0;

	virtual void Render( int x, int y, int w, int h ) = 0;

	virtual void Enable( bool bEnable ) = 0;
	virtual bool IsEnabled( ) = 0;
};

class IScreenSpaceEffectManager {
public:

	virtual void InitScreenSpaceEffects( ) = 0;
	virtual void ShutdownScreenSpaceEffects( ) = 0;

	virtual IScreenSpaceEffect *GetScreenSpaceEffect( const char *pEffectName ) = 0;

	virtual void SetScreenSpaceEffectParams( const char *pEffectName, KeyValues *params ) = 0;
	virtual void SetScreenSpaceEffectParams( IScreenSpaceEffect *pEffect, KeyValues *params ) = 0;

	virtual void EnableScreenSpaceEffect( const char *pEffectName ) = 0;
	virtual void EnableScreenSpaceEffect( IScreenSpaceEffect *pEffect ) = 0;

	virtual void DisableScreenSpaceEffect( const char *pEffectName ) = 0;
	virtual void DisableScreenSpaceEffect( IScreenSpaceEffect *pEffect ) = 0;

	virtual void DisableAllScreenSpaceEffects( ) = 0;

	virtual void RenderEffects( int x, int y, int w, int h ) = 0;
};

class CScreenSpaceEffectRegistration {
public:
	CScreenSpaceEffectRegistration( const char *pName, IScreenSpaceEffect *pEffect );

	const char			*m_pEffectName;
	IScreenSpaceEffect	*m_pEffect;

	CScreenSpaceEffectRegistration *m_pNext;

	static CScreenSpaceEffectRegistration *s_pHead;
};