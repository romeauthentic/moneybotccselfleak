#pragma once
#include "IAppSystem.hpp"
#include "vector.hpp"
#include "color.hpp"

enum FontDrawType_t {
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags_t {
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum ImageFormat {
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
								IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
								IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

								NUM_IMAGE_FORMATS
};

struct vertex_t {
	vertex_t( ) {}

	vertex_t( const vec2_t& pos, const vec2_t& coord = vec2_t( 0, 0 ) ) {
		m_position = pos;
		m_tex_coord = coord;
	}

	void init( float x, float y, const vec2_t& coord = vec2_t( 0, 0 ) ) {
		m_position = vec2_t( x, y );
		m_tex_coord = coord;
	}

	vec2_t m_position;
	vec2_t m_tex_coord;
};

using HFont = unsigned long;
using VPANEL = unsigned long;
class IntRect;
class IHTMLEvents;
class IHTML;
using HCursor = unsigned long;
using HTexture = unsigned long;
class CharRenderInfo;
class IVguiMatInfo;
class IImage;
class DrawTexturedRectParms_t;
class IHTMLChromeController;

class ISurface : public IAppSystem {
public:
	// frame
	virtual void RunFrame( ) = 0;

	// hierarchy root
	virtual VPANEL GetEmbeddedPanel( ) = 0;
	virtual void SetEmbeddedPanel( VPANEL pPanel ) = 0;

	// drawing context
	virtual void PushMakeCurrent( VPANEL panel, bool useInsets ) = 0;
	virtual void PopMakeCurrent( VPANEL panel ) = 0; //13

													 // rendering functions
	virtual void DrawSetColor( ) = 0; //15
	virtual void DrawSetColor( clr_t col ) = 0; //14

	virtual void DrawFilledRect( int x0, int y0, int x1, int y1 ) = 0; //16
	virtual void DrawFilledRectArray( IntRect *pRects, int numRects ) = 0;
	virtual void DrawOutlinedRect( int x0, int y0, int x1, int y1 ) = 0;

	virtual void DrawLine( int x0, int y0, int x1, int y1 ) = 0;
	virtual void DrawPolyLine( int *px, int *py, int numPoints ) = 0;

	virtual void DrawSetApparentDepth( float flDepth ) = 0;
	virtual void DrawClearApparentDepth( ) = 0;

	virtual void DrawSetTextFont( HFont font ) = 0;
	virtual void DrawSetTextColor( ) = 0; //25
	virtual void DrawSetTextColor( clr_t col ) = 0; //24
	virtual void DrawSetTextPos( int x, int y ) = 0; //26
	virtual void DrawGetTextPos( int& x, int& y ) = 0;
	virtual void DrawPrintText( const wchar_t *text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void DrawUnicodeChar( wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;

	virtual void DrawFlushText( ) = 0;		// flushes any buffered text (for rendering optimizations)
	virtual IHTML *CreateHTMLWindow( IHTMLEvents *events, VPANEL context ) = 0;
	virtual void PaintHTMLWindow( IHTML *htmlwin ) = 0;
	virtual void DeleteHTMLWindow( IHTML *htmlwin ) = 0;

	enum ETextureFormat
	{
		eTextureFormat_RGBA,
		eTextureFormat_BGRA,
		eTextureFormat_BGRA_Opaque, // bgra format but alpha is always 255, CEF does this, we can use this fact for better perf on win32 gdi
	};

	virtual int	 DrawGetTextureId( char const *filename ) = 0;
	virtual bool DrawGetTextureFile( int id, char *filename, int maxlen ) = 0;
	virtual void DrawSetTextureFile( int id, const char *filename, int hardwareFilter, bool forceReload ) = 0;
	virtual void DrawSetTextureRGBA( int id, const unsigned char *rgba, int wide, int tall ) = 0;
	virtual void DrawSetTexture( int id ) = 0; //38
	virtual void DrawGetTextureSize( int id, int &wide, int &tall ) = 0;
	virtual void DrawTexturedRect( int x0, int y0, int x1, int y1 ) = 0;
	virtual bool IsTextureIDValid( int id ) = 0;
	virtual bool DeleteTextureByID( int id ) = 0;

	virtual int CreateNewTextureID( bool procedural = false ) = 0; //43

	virtual void GetScreenSize( int &wide, int &tall ) = 0;
	virtual void SetAsTopMost( VPANEL panel, bool state ) = 0;
	virtual void BringToFront( VPANEL panel ) = 0;
	virtual void SetForegroundWindow( VPANEL panel ) = 0;
	virtual void SetPanelVisible( VPANEL panel, bool state ) = 0;
	virtual void SetMinimized( VPANEL panel, bool state ) = 0;
	virtual bool IsMinimized( VPANEL panel ) = 0;
	virtual void FlashWindow( VPANEL panel, bool state ) = 0;
	virtual void SetTitle( VPANEL panel, const wchar_t *title ) = 0;
	virtual void SetAsToolBar( VPANEL panel, bool state ) = 0;		// removes the window's task bar entry (for context menu's, etc.)

																	// windows stuff
	virtual void CreatePopup( VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true ) = 0;
	virtual void SwapBuffers( VPANEL panel ) = 0;
	virtual void Invalidate( VPANEL panel ) = 0;
	virtual void SetCursor( HCursor cursor ) = 0;
	virtual bool IsCursorVisible( ) = 0;
	virtual void ApplyChanges( ) = 0;
	virtual bool IsWithin( int x, int y ) = 0;
	virtual bool HasFocus( ) = 0;

	// returns true if the surface supports minimize & maximize capabilities
	enum SurfaceFeature_e
	{
		ANTIALIASED_FONTS = 1,
		DROPSHADOW_FONTS = 2,
		ESCAPE_KEY = 3,
		OPENING_NEW_HTML_WINDOWS = 4,
		FRAME_MINIMIZE_MAXIMIZE = 5,
		OUTLINE_FONTS = 6,
		DIRECT_HWND_RENDER = 7,
	};
	virtual bool SupportsFeature( SurfaceFeature_e feature ) = 0;

	// restricts what gets drawn to one panel and it's children
	// currently only works in the game
	virtual void RestrictPaintToSinglePanel( VPANEL panel ) = 0;

	// these two functions obselete, use IInput::SetAppModalSurface() instead
	virtual void SetModalPanel( VPANEL ) = 0;
	virtual VPANEL GetModalPanel( ) = 0;

	virtual void UnlockCursor( ) = 0;
	virtual void LockCursor( ) = 0;
	virtual void SetTranslateExtendedKeys( bool state ) = 0;
	virtual VPANEL GetTopmostPopup( ) = 0;

	// engine-only focus handling (replacing WM_FOCUS windows handling)
	virtual void SetTopLevelFocus( VPANEL panel ) = 0;

	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
	virtual HFont CreateFont( ) = 0; //71

									 // adds to the font
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
		FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
	};

	virtual bool SetFontGlyphSet( HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0 ) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile( const char *fontName, const char *fontFileName ) = 0;

	// returns the details about the font
	virtual int GetFontTall( HFont font ) = 0;
	virtual int GetFontAscent( HFont font, wchar_t wch ) = 0;
	virtual bool IsFontAdditive( HFont font ) = 0;
	virtual void GetCharABCwide( HFont font, int ch, int &a, int &b, int &c ) = 0;
	virtual int GetCharacterWidth( HFont font, int ch ) = 0;
	virtual void GetTextSize( HFont font, const wchar_t *text, int &wide, int &tall ) = 0;

	// notify icons?!?
	virtual VPANEL GetNotifyPanel( ) = 0;
	virtual void SetNotifyIcon( VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text ) = 0;

	// plays a sound
	virtual void PlaySound( const char *fileName ) = 0; //82

														//!! these functions should not be accessed directly, but only through other vgui items
														//!! need to move these to seperate interface
	virtual int GetPopupCount( ) = 0;
	virtual VPANEL GetPopup( int index ) = 0;
	virtual bool ShouldPaintChildPanel( VPANEL childPanel ) = 0;
	virtual bool RecreateContext( VPANEL panel ) = 0;
	virtual void AddPanel( VPANEL panel ) = 0;
	virtual void ReleasePanel( VPANEL panel ) = 0;
	virtual void MovePopupToFront( VPANEL panel ) = 0;
	virtual void MovePopupToBack( VPANEL panel ) = 0;

	virtual void SolveTraverse( VPANEL panel, bool forceApplySchemeSettings = false ) = 0;
	virtual void PaintTraverse( VPANEL panel ) = 0;

	virtual void EnableMouseCapture( VPANEL panel, bool state ) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds( int &x, int &y, int &wide, int &tall ) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds( int &x, int &y, int &wide, int &tall ) = 0;

	// gets the base resolution used in proportional mode
	virtual void GetProportionalBase( int &width, int &height ) = 0;

	virtual void CalculateMouseVisible( ) = 0;
	virtual bool NeedKBInput( ) = 0;

	virtual bool HasCursorPosFunctions( ) = 0;
	virtual void SurfaceGetCursorPos( int &x, int &y ) = 0;
	virtual void SurfaceSetCursorPos( int x, int y ) = 0;

	// SRC only functions!!!
	virtual void DrawTexturedLine( const vertex_t &a, const vertex_t &b ) = 0;
	virtual void DrawOutlinedCircle( int x, int y, int radius, int segments ) = 0;
	virtual void DrawTexturedPolyLine( const vertex_t *p, int n ) = 0; // (Note: this connects the first and last points).
	virtual void DrawTexturedSubRect( int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1 ) = 0;
	virtual void DrawTexturedPolygon( int n, vertex_t *pVertice, bool bClipVertices = true ) = 0; //106
	virtual const wchar_t* GetTitle( VPANEL panel ) = 0;
	virtual bool IsCursorLocked( ) const = 0;
	virtual void SetWorkspaceInsets( int left, int top, int right, int bottom ) = 0;

	// squarish comic book word bubble with pointer, rect params specify the space inside the bubble
	virtual void DrawWordBubble( int x0, int y0, int x1, int y1, int nBorderThickness, clr_t rgbaBackground, clr_t rgbaBorder,
		bool bPointer = false, int nPointerX = 0, int nPointerY = 0, int nPointerBaseThickness = 16 ) = 0;


	// Lower level char drawing code, call DrawGet then pass in info to DrawRender
	virtual bool DrawGetUnicodeCharRenderInfo( wchar_t ch, CharRenderInfo& info ) = 0;
	virtual void DrawRenderCharFromInfo( const CharRenderInfo& info ) = 0;

	// global alpha setting functions
	// affect all subsequent draw calls - shouldn't normally be used directly, only in Panel::PaintTraverse()
	virtual void DrawSetAlphaMultiplier( float alpha /* [0..1] */ ) = 0;
	virtual float DrawGetAlphaMultiplier( ) = 0;

	// web browser
	virtual void SetAllowHTMLJavaScript( bool state ) = 0;

	// video mode changing
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;

	virtual HCursor CreateCursorFromFile( char const *curOrAniFile, char const *pPathID = nullptr ) = 0;

	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual IVguiMatInfo* DrawGetTextureMatInfoFactory( int id ) = 0;

	virtual void PaintTraverseEx( VPANEL panel, bool paintPopups = false ) = 0;

	virtual float GetZPos( ) const = 0;

	// From the Xbox
	virtual void SetPanelForInput( VPANEL vpanel ) = 0;
	virtual void DrawFilledRectFastFade( int x0, int y0, int x1, int y1, int fadeStartPt, int fadeEndPt, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0;
	virtual void DrawFilledRectFade( int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal ) = 0; //123

	virtual void DrawSetTextureRGBAEx( int id, const unsigned char *rgba, int wide, int tall, ImageFormat imageFormat ) = 0;
	virtual void DrawSetTextScale( float sx, float sy ) = 0;
	virtual bool SetBitmapFontGlyphSet( HFont font, const char *windowsFontName, float scalex, float scaley, int flags ) = 0;
	// adds a bitmap font file
	virtual bool AddBitmapFontFile( const char *fontFileName ) = 0;
	// sets a symbol for the bitmap font
	virtual void SetBitmapFontName( const char *pName, const char *pFontFilename ) = 0;
	// gets the bitmap font filename
	virtual const char* GetBitmapFontName( const char *pName ) = 0;
	virtual void ClearTemporaryFontCache( ) = 0;

	virtual IImage* GetIconImageForFullPath( char const *pFullPath ) = 0;
	virtual void DrawUnicodeString( const wchar_t *pwString, FontDrawType_t drawType = FONT_DRAW_DEFAULT ) = 0;
	virtual void PrecacheFontCharacters( HFont font, const wchar_t *pCharacters ) = 0;
	// Console-only.  Get the string to use for the current video mode for layout files.
	//virtual const char *GetResolutionKey( void ) const = 0;

	virtual const char *GetFontName( HFont font ) = 0;
	//virtual const char *GetFontFamilyName( HFont font ) = 0;

	virtual bool ForceScreenSizeOverride( bool bState, int wide, int tall ) = 0;
	// LocalToScreen, ParentLocalToScreen fixups for explicit PaintTraverse calls on Panels not at 0, 0 position
	virtual bool ForceScreenPosOffset( bool bState, int x, int y ) = 0;
	virtual void OffsetAbsPos( int &x, int &y ) = 0;

	virtual void SetAbsPosForContext( int, int, int ) = 0;
	virtual void GetAbsPosForContext( int, int &, int & ) = 0;

	// Causes fonts to get reloaded, etc.
	virtual void ResetFontCaches( ) = 0;

	virtual bool IsScreenSizeOverrideActive( ) = 0;
	virtual bool IsScreenPosOverrideActive( ) = 0;

	virtual void DestroyTextureID( int id ) = 0;

	virtual int GetTextureNumFrames( int id ) = 0;
	virtual void DrawSetTextureFrame( int id, int nFrame, unsigned int *pFrameCache ) = 0;

	virtual void GetClipRect( int &x0, int &y0, int &x1, int &y1 ) = 0;
	virtual void SetClipRect( int x0, int y0, int x1, int y1 ) = 0;

	virtual void DrawTexturedRectEx( DrawTexturedRectParms_t *pDrawParms ) = 0;

	virtual void GetKernedCharWidth( HFont font, wchar_t ch, wchar_t chBefore, wchar_t chAfter, float &wide, float &abcA ) = 0;

	virtual void DrawUpdateRegionTextureRGBA( int nTextureID, int x, int y, const unsigned char *pchData, int wide, int tall, ImageFormat imageFormat ) = 0;
	virtual bool BHTMLWindowNeedsPaint( IHTML *htmlwin ) = 0;

	virtual void DrawSetTextureRGBALinear( int, unsigned char const*, int, int ) = 0;

	virtual const char *GetWebkitHTMLUserAgentString( ) = 0;

	virtual IHTMLChromeController *Deprecated_AccessChromeHTMLController( ) = 0;

	/*
	154	CMatSystemSurface::SetAppDrivesInput(bool)
	155	CMatSystemSurface::EnableWindowsMessages(bool)
	156	CMatSystemSurface::Begin3DPaint(int,int,int,int,bool)
	157	CMatSystemSurface::End3DPaint(bool)
	158	CMatSystemSurface::DisableClipping(bool)
	159	CMatSystemSurface::SetMouseCallbacks(void (*)(int &,int &),void (*)(int,int))
	160	CMatSystemSurface::InstallPlaySoundFunc(void (*)(char const*))
	161	CMatSystemSurface::DrawColoredCircle(int,int,float,int,int,int,int)
	162	CMatSystemSurface::DrawColoredText(unsigned long,int,int,int,int,int,int,char const*,...)
	163	CMatSystemSurface::DrawColoredTextRect(unsigned long,int,int,int,int,int,int,int,int,char const*,...)
	164	CMatSystemSurface::DrawTextHeight(unsigned long,int,int &,char *,...)
	165	CMatSystemSurface::DrawTextLen(unsigned long,char const*,...)
	166	CMatSystemSurface::DrawPanelIn3DSpace(unsigned long long,VMatrix const&,int,int,float,float)
	167	CMatSystemSurface::DrawSetTextureMaterial(int,IMaterial *)
	168	CMatSystemSurface::HandleInputEvent(InputEvent_t const&)
	169	CMatSystemSurface::Set3DPaintTempRenderTarget(char const*)
	170	CMatSystemSurface::Reset3DPaintTempRenderTarget(void)
	171	CMatSystemSurface::DrawGetTextureMaterial(int)
	172	CMatSystemSurface::SetInputContext(InputContextHandle_t__ *)
	173	CMatSystemSurface::DrawSetSubTextureRGBA(int,int,int,unsigned char const*,int,int)
	174	CMatSystemSurface::DrawTexturedSubRectGradient(int,int,int,int,float,float,float,float,Color,Color,bool)
	175	CMatSystemSurface::~CMatSystemSurface()
	176	CMatSystemSurface::~CMatSystemSurface()
	177	CMatSystemSurface::SupportsFontFeature(FontFeature_t)
	178	CMatSystemSurface::InEngine(void)
	179	CMatSystemSurface::IsInThink(unsigned long long)
	180	CMatSystemSurface::SetLanguage(char const*)
	181	CMatSystemSurface::GetLanguage(void)
	182	CMatSystemSurface::ComputeTextWidth(wchar_t const*)
	*/
};