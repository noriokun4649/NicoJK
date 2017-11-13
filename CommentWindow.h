﻿#pragma once

#include "OsdCompositor.h"

class CCommentWindow
{
public:
	static const int DEFAULT_LINE_COUNT = 14;
	static const int DEFAULT_LINE_DRAW_COUNT = 9999;
	static const int DISPLAY_DURATION = 4000;
	// 表示すべきコメントがないときウィンドウを非表示にするまでの猶予[秒]
	// (レイヤードウィンドウは被せているだけでもそれなりのコストがかかるため)
	static const int AUTOHIDE_DELAY = 10;
	static const int FONT_SMALL_RATIO = 75;
	// テクスチャ用ビットマップの幅の最小値(メモリや局所参照性の面で十分小さな値)
	static const int TEXTURE_BITMAP_WIDTH_MIN = 640;
	enum CHAT_POSITION {
		CHAT_POS_DEFAULT,
		CHAT_POS_SHITA,
		CHAT_POS_UE,
	};
	enum CHAT_SIZE {
		CHAT_SIZE_DEFAULT,
		CHAT_SIZE_SMALL,
	};
	enum CHAT_ALIGN {
		CHAT_ALIGN_CENTER,
		CHAT_ALIGN_LEFT,
		CHAT_ALIGN_RIGHT,
	};
	bool Initialize(HINSTANCE hinst, bool *pbEnableOsdCompositor, bool bSetHookOsdCompositor);
	void Finalize();
	CCommentWindow();
	~CCommentWindow();
	bool Create(HWND hwndParent);
	void Destroy();
	void SetStyle(LPCTSTR fontName, LPCTSTR fontNameMulti, bool bBold, bool bAntiAlias,
	              int fontOutline, bool bUseOsdCompositor, bool bUseTexture, bool bUseDrawingThread);
	void SetCommentSize(int size, int sizeMin, int sizeMax, int lineMargin);
	void SetDrawLineCount(int lineDrawCount);
	int GetDisplayDuration() const { return displayDuration_; }
	void SetDisplayDuration(int duration);
	BYTE GetOpacity() const { return opacity_; }
	void SetOpacity(BYTE opacity);
	void SetDebugFlags(int debugFlags);
	void OnParentMove();
	void OnParentSize();
	void AddChat(LPCTSTR text, COLORREF color, CHAT_POSITION position, CHAT_SIZE size = CHAT_SIZE_DEFAULT,
	             CHAT_ALIGN align = CHAT_ALIGN_CENTER, bool bInsertLast = false, BYTE backOpacity = 0, int delay = 0);
	void ScatterLatestChats(int duration);
	void ClearChat();
	void Forward(int duration);
	void Update();
	bool IsCreated() const { return hwnd_ != nullptr; }
	void OnFilterGraphInitialized(IGraphBuilder *pGraphBuilder) { osdCompositor_.OnFilterGraphInitialized(pGraphBuilder); }
	void OnFilterGraphFinalize(IGraphBuilder *pGraphBuilder) { osdCompositor_.OnFilterGraphFinalize(pGraphBuilder); }
private:
	struct CHAT {
		DWORD pts;
		DWORD count;
		int line;
		Gdiplus::ARGB color;
		CHAT_POSITION position;
		bool bSmall;
		BYTE alignFactor;
		bool bInsertLast;
		bool bMultiLine;
		bool bDrew;
		int currentDrawWidth;
		int currentDrawHeight;
		std::wstring text;
	};
	struct TEXTURE {
		bool bUsed;
		bool bSmall;
		RECT rc;
		Gdiplus::ARGB color;
		std::wstring text;
		bool IsMatch(const CHAT &c) const { return c.color == color && c.bSmall == bSmall && c.text == text; }
	};
	bool AllocateWorkBitmap(int width, int height, bool *pbRealloc);
	void UpdateLayeredWindow();
	static unsigned int __stdcall DrawingThread(void *pParam);
	bool WaitForIdleDrawingThread();
	void UpdateChat();
	BOOL UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,
	                         COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags, RECT *prcDirty);
	static BOOL CALLBACK UpdateCallback(void *pBits, const RECT *pSurfaceRect, int pitch, void *pClientData);
	void DrawChat(Gdiplus::Graphics &g, int width, int height, RECT *prcUnused, RECT *prcUnusedWoShita, bool *pbHasFirstDrawShita);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE hinst_;
	ULONG_PTR gdiplusToken_;
	BOOL (WINAPI *pfnUpdateLayeredWindowIndirect_)(HWND hWnd, const UPDATELAYEREDWINDOWINFO *pULWInfo);
	bool bSse2Available_;
	HWND hwnd_;
	HWND hwndParent_;
	HBITMAP hbmWork_;
	void *pBits_;
	HDC hdcWork_;
	HANDLE hDrawingThread_;
	HANDLE hDrawingEvent_;
	HANDLE hDrawingIdleEvent_;
	volatile bool bQuitDrawingThread_;
	int commentSizeMin_;
	int commentSizeMax_;
	int lineCount_;
	double lineDrawCount_;
	double fontScale_;
	double fontSmallScale_;
	TCHAR fontName_[LF_FACESIZE];
	TCHAR fontNameMulti_[LF_FACESIZE];
	int fontStyle_;
	bool bAntiAlias_;
	BYTE opacity_;
	int fontOutline_;
	int displayDuration_;
	DWORD rts_;
	DWORD chatCount_;
	int currentWindowWidth_;
	std::list<CHAT> chatList_;
	std::list<CHAT> chatPoolList_;
	// chatList_(リスト構造のみ),chatPoolList_を保護
	CCriticalLock chatLock_;
	int autoHideCount_;
	int parentSizedCount_;
	RECT rcParent_;
	COsdCompositor osdCompositor_;
	bool bUseOsd_;
	bool bShowOsd_;
	bool bUseTexture_;
	bool bUseDrawingThread_;
	std::unique_ptr<Gdiplus::Bitmap> pTextureBitmap_;
	std::unique_ptr<Gdiplus::Graphics> pgTexture_;
	int currentTextureHeight_;
	std::list<TEXTURE> textureList_;
	RECT rcUnused_;
	RECT rcUnusedWoShita_;
	RECT rcUnusedIntersect_;
	RECT rcDirty_;
	bool bForceRefreshDirty_;
	int debugFlags_;
};
