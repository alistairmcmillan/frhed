#include "precomp.h"
#include "toolbar.h"
#include "resource.h"

#define IMAGEWIDTH 16
#define IMAGEHEIGHT 17
#define BUTTONWIDTH 0
#define BUTTONHEIGHT 0

TBBUTTON tbButtons[] = // Array defining the toolbar buttons
{
	{  0, IDM_NEW,                   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  1, IDM_OPEN,                  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  2, IDM_SAVE,                  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  0, 0,                         TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0 },
	{  3, IDM_EDIT_CUT,              TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  4, IDM_EDIT_COPY,             TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  5, IDM_EDIT_PASTE,            TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  0, 0,                         TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0 },
	{  6, IDM_FIND,                  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  7, IDM_REPLACE,               TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  0, 0,                         TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0 },
	{  8, IDM_HELP_TOPICS,           TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0 },
	{  0, 0,                         TBSTATE_ENABLED, TBSTYLE_SEP,    0, 0 },
	{  9, ID_DISK_GOTOFIRSTTRACK,    0,               TBSTYLE_BUTTON, 0, 0 },
	{ 10, ID_DISK_GOTOPREVIOUSTRACK, 0,               TBSTYLE_BUTTON, 0, 0 },
	{ 11, ID_DISK_GOTONEXTTRACK,     0,               TBSTYLE_BUTTON, 0, 0 },
	{ 12, ID_DISK_GOTOLASTTRACK,     0,               TBSTYLE_BUTTON, 0, 0 }
};

LRESULT SetTooltipText(HINSTANCE hInst, HWND hWnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER( hWnd );
	LPTOOLTIPTEXT lpToolTipText;
	static char szBuffer[64];

	lpToolTipText = (LPTOOLTIPTEXT)lParam;
	if ((lpToolTipText->hdr.code == TTN_NEEDTEXTA) ||
		(lpToolTipText->hdr.code == TTN_NEEDTEXTW))
	{
		LoadString(hInst,
		lpToolTipText->hdr.idFrom, // string ID == command ID
		szBuffer,
		sizeof(szBuffer));

		lpToolTipText->lpszText = szBuffer;
	}
	return 0;
}

void EnableDriveButtons(HWND hWndToolBar, BOOL bEnable)
{
	int IDS[] = { ID_DISK_GOTOFIRSTTRACK, ID_DISK_GOTONEXTTRACK, ID_DISK_GOTOPREVIOUSTRACK, ID_DISK_GOTOLASTTRACK, -1 };

	for( int i = 0; IDS[i] != -1; i++ )
	{
		int iCurrentFlags = (int)SendMessage(hWndToolBar, TB_GETSTATE, IDS[i], 0L);

		if( bEnable )
			iCurrentFlags |= TBSTATE_ENABLED;
		else
			iCurrentFlags &= ~TBSTATE_ENABLED;

		SendMessage(hWndToolBar, TB_SETSTATE, IDS[i], MAKELPARAM(iCurrentFlags, 0));
	}
}

HWND CreateTBar(HINSTANCE hInst, HWND hWnd, UINT idBitmap)
{
	HWND hWndToolBar = CreateToolbarEx(hWnd,
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_WRAPABLE,
		0,
		sizeof(tbButtons)/sizeof(TBBUTTON),
		hInst,
		idBitmap,
		tbButtons,
		sizeof(tbButtons)/sizeof(TBBUTTON),
		BUTTONWIDTH,
		BUTTONHEIGHT,
		IMAGEWIDTH,
		IMAGEHEIGHT,
		sizeof(TBBUTTON));

	return hWndToolBar;
}
