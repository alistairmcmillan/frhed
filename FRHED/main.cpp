/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  main.cpp
 *
 * @brief WinMain of Frhed.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

//============================================================================================
// frhed - free hex editor

#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "hexwnd.h"
#include "toolbar.h"

static const char szMainClass[] = "frhed wndclass";
static const char szHexClassA[] = "hekseditA_" SHARPEN(FRHED_VERSION_4);

HINSTANCE hMainInstance;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

static BOOL NTAPI IsNT()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof osvi);
	osvi.dwOSVersionInfoSize = sizeof osvi;
	if (!GetVersionEx(&osvi))
		osvi.dwPlatformId = 0;
	return osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

static BOOL CALLBACK WndEnumProcCountInstances(HWND hwnd, LPARAM lParam)
{
	TCHAR buf[64];
	if (GetClassName(hwnd, buf, RTL_NUMBER_OF(buf)))
		if (StrCmp(buf, szMainClass) == 0)
			++*(int *)lParam;
	return TRUE;
}

static HWND hwndMain = 0;
static HWND hwndHex = 0;
static HWND hwndToolBar = 0;
static HWND hwndStatusBar = 0;
static HexEditorWindow *pHexWnd = 0;

//--------------------------------------------------------------------------------------------
// WinMain: the starting point.
int WINAPI WinMain(HINSTANCE hIconInstance, HINSTANCE, char *szCmdLine, int)
{
	OleInitialize(NULL);
	InitCommonControls();

	// Load the heksedit component.
	static const TCHAR pe_heksedit[] = _T("heksedit.dll");
	hMainInstance = LoadLibrary(pe_heksedit);
	if (hMainInstance == NULL)
	{
		TCHAR complain[100];
		wsprintf(complain, _T("Unable to load the %s"), pe_heksedit);
		MessageBox(NULL, complain, NULL, MB_ICONSTOP);
		return 3;
	}
	// Register window class and open window.

	MSG msg;

	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof wndclass);

	//Register the main window class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = MainWndProc;
	wndclass.hIcon = LoadIcon(hIconInstance, MAKEINTRESOURCE(IDI_FRHED));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hMainInstance;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
	wndclass.lpszClassName = szMainClass;

	RegisterClass(&wndclass);

	int iInstCount = 0;
	EnumWindows(WndEnumProcCountInstances, (LPARAM)&iInstCount);

	CreateWindow(szMainClass, 0, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hMainInstance, NULL);

	if (!pHexWnd)
	{
		MessageBox(NULL, _T("Unable to create the heksedit control"), NULL, MB_ICONSTOP);
		return 3;
	}

	// Read in the last saved preferences.
	pHexWnd->iInstCount = iInstCount;
	pHexWnd->read_ini_data();

	// The if prevents the window from being resized to 0x0 it becomes just a title bar
	if (pHexWnd->iWindowX != CW_USEDEFAULT)
	{
		// Prevent window creep when Taskbar is at top or left of screen
		WINDOWPLACEMENT wp;
		wp.length = sizeof wp;
		GetWindowPlacement(hwndMain, &wp);
		wp.showCmd = pHexWnd->iWindowShowCmd;
		wp.rcNormalPosition.left = pHexWnd->iWindowX;
		wp.rcNormalPosition.top = pHexWnd->iWindowY;
		wp.rcNormalPosition.right = pHexWnd->iWindowWidth + pHexWnd->iWindowX;
		wp.rcNormalPosition.bottom = pHexWnd->iWindowHeight + pHexWnd->iWindowY;
		SetWindowPlacement(hwndMain, &wp);
	}
	ShowWindow(hwndMain, pHexWnd->iWindowShowCmd);
	UpdateWindow(hwndMain);

	if (*szCmdLine != '\0')
	{
		// Command line not empty: open a file on startup.
		char *p = szCmdLine;
		char *q = szCmdLine;
		DWORD dwStart = 0, dwLength = 0, dwEnd = 0; // MF cmd line parms
		while ((*p = *q) != '\0')
		{
			switch (*q++)
			{
			case '/': // switch coming up
				switch (*q)
				{
				case 'S': // Start offset
				case 's':
					dwStart = strtoul(++q, &q, 0);
					break;
				case 'L': // Length of selection
				case 'l':
					dwLength = strtoul(++q, &q, 0);
					break;
				case 'E': // End of selection
				case 'e':
					dwEnd = strtoul(++q, &q, 0);
					break;
				}
				// fall through
			case '"':
				break;
			default:
				++p;
				break;
			}
		}
		if (dwLength)
			dwEnd = dwStart + dwLength - 1;
		char lpszPath[MAX_PATH];
		HRESULT hres = pHexWnd->ResolveIt(szCmdLine, lpszPath);
		if (SUCCEEDED(hres))
		{
			// Trying to open a link file: decision by user required.
			int ret = MessageBox(hwndMain,
				"You are trying to open a link file.\n"
				"Click on Yes if you want to open the file linked to,\n"
				"or click on No if you want to open the link file itself.\n"
				"Choose Cancel if you want to abort opening.",
				ApplicationName, MB_YESNOCANCEL | MB_ICONQUESTION );
			switch( ret )
			{
			case IDYES:
				pHexWnd->load_file(lpszPath);
				break;
			case IDNO:
				pHexWnd->load_file(szCmdLine);
				break;
			case IDCANCEL:
				break;
			}
		}
		else
		{
			pHexWnd->load_file(szCmdLine);
		}
		if (dwEnd)
			pHexWnd->CMD_setselection(dwStart, dwEnd);
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!pHexWnd || !pHexWnd->translate_accelerator(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	FreeLibrary(hMainInstance);
	OleUninitialize();
	_CrtDumpMemoryLeaks();
	return msg.wParam;
}

//--------------------------------------------------------------------------------------------
// The main window procedure.
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CREATE:
		hwndMain = hwnd;
		DragAcceptFiles(hwnd, TRUE); // Accept files dragged into main window.
		hwndToolBar = CreateTBar(hwnd, hMainInstance);
		SendMessage(hwndToolBar, CCM_SETUNICODEFORMAT, TRUE, 0);
		hwndHex = CreateWindowEx(WS_EX_CLIENTEDGE,
			szHexClassA, 0,
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			10, 10, 100, 100, hwnd, 0, hMainInstance, 0);
		pHexWnd = (HexEditorWindow *)GetWindowLong(hwndHex, GWL_USERDATA);
		if (!pHexWnd)
			return -1;
		hwndStatusBar = CreateStatusWindow(
			CCS_BOTTOM | WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
			"Ready", hwnd, 2);
		pHexWnd->hwndMain = hwnd;
		pHexWnd->hwndToolBar = hwndToolBar;
		pHexWnd->hwndStatusBar = hwndStatusBar;
		pHexWnd->bSaveIni = TRUE;
		pHexWnd->bCenterCaret = TRUE;
		pHexWnd->load_lang((LANGID)GetThreadLocale());
		pHexWnd->set_wnd_title();
		return 0;
	case WM_COMMAND:
		// Exit command must be handled in Frhed executable,
		// not in heksedit dll.
		if (LOWORD(wParam) == IDM_EXIT)
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		else
			pHexWnd->command(LOWORD(wParam));
		break;
	case WM_DROPFILES:
		pHexWnd->dropfiles(reinterpret_cast<HDROP>(wParam));
		break;
	case WM_SETFOCUS:
		SetFocus(hwndHex);
		break;
	case WM_CLOSE: 
		if (!pHexWnd->close())
			return 0;
		break;
	case WM_INITMENUPOPUP:
		pHexWnd->initmenupopup(wParam, lParam);
		break;
	case WM_SIZE:
		{
			int cx = GET_X_LPARAM(lParam);
			int cy = GET_Y_LPARAM(lParam);
			RECT rect;
			// Adjust tool bar width so buttons will wrap accordingly.
			MoveWindow(hwndToolBar, 0, 0, cx, 0, TRUE);
			// Set tool bar height to bottom of last button plus some padding.
			if (int n = SendMessage(hwndToolBar, TB_BUTTONCOUNT, 0, 0))
			{
				SendMessage(hwndToolBar, TB_GETITEMRECT, n - 1, (LPARAM)&rect);
				MoveWindow(hwndToolBar, 0, 0, cx, rect.bottom + 2, TRUE);
			}
			GetWindowRect(hwndToolBar, &rect);
			int cyToolBar = rect.bottom - rect.top;
			// Moves status bar back to the bottom
			SendMessage(hwndStatusBar, WM_SIZE, 0, 0);
			GetWindowRect(hwndStatusBar, &rect);
			int cyStatusBar = rect.bottom - rect.top;
			//--------------------------------------------
			// Set statusbar divisions.
			// Calculate the right edge coordinate for each part
			int parts[] = { MulDiv(cx, 4, 6), MulDiv(cx, 5, 6), cx };
			SendMessage(hwndStatusBar, SB_SETPARTS, RTL_NUMBER_OF(parts), (LPARAM)parts);
			MoveWindow(hwndHex, 0, cyToolBar, cx, cy - cyToolBar - cyStatusBar, TRUE);
		}
		break;
	case WM_NOTIFY:
		{
			NMHDR *pn = (NMHDR *)lParam;
			HWND hwndFrom = pn->hwndFrom;
			UINT code = pn->code;
			if (hwndFrom == hwndStatusBar)
			{
				if (code == NM_CLICK || code == NM_RCLICK)
					pHexWnd->status_bar_click(code == NM_CLICK);
			}
			else if (hwndFrom == hwndToolBar)
			{
				if (code == TBN_GETINFOTIPW)
				{
					NMTBGETINFOTIPW *pi = (NMTBGETINFOTIPW *)lParam;
					if (BSTR text = pHexWnd->load_string(pi->iItem))
					{
						StrCpyNW(pi->pszText, text, pi->cchTextMax);
						pHexWnd->free_string(text);
					}
				}
			}
		}
		break;
	case WM_DESTROY:
		if (pHexWnd)
		{
			// Store window position for next startup.
			WINDOWPLACEMENT wndpl;
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &wndpl);
			pHexWnd->iWindowShowCmd = wndpl.showCmd;
			pHexWnd->iWindowX = wndpl.rcNormalPosition.left;
			pHexWnd->iWindowY = wndpl.rcNormalPosition.top;
			pHexWnd->iWindowWidth = wndpl.rcNormalPosition.right - pHexWnd->iWindowX;
			pHexWnd->iWindowHeight = wndpl.rcNormalPosition.bottom - pHexWnd->iWindowY;
			pHexWnd->save_ini_data();
			DragAcceptFiles(hwndMain, FALSE);
			pHexWnd = 0;
			PostQuitMessage(0);
		}
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam );
}
