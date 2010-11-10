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

static const TCHAR szMainClass[] = _T("frhed wndclass");
static const TCHAR szHexClass[] = _T("heksedit");
static const TCHAR szHexBinary[] = _T("heksedit.dll");

HINSTANCE hMainInstance;

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
static unsigned long ProcessSwitch(LPWSTR szFirstArgument, LPWSTR szNextArgument, bool* bSkipNextArg);

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

/**
 * @brief The application starting point.
 */
int WINAPI WinMain(HINSTANCE hIconInstance, HINSTANCE, LPSTR, int)
{
	OleInitialize(NULL);
	InitCommonControls();

	LPWSTR szCmdLine = GetCommandLineW();

	// Load the heksedit component.
	hMainInstance = LoadLibrary(szHexBinary);
	if (hMainInstance == NULL)
	{
		TCHAR complain[100];
		wsprintf(complain, _T("Unable to load the %s"), szHexBinary);
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

	//Parse the command line
	// (frhed.exe) [/s[offset]] [/l[offset]] [/e[another]] [filename]
	int iNumberOfArgs;
	LPWSTR* szArguments = CommandLineToArgvW(szCmdLine, &iNumberOfArgs);

	if(iNumberOfArgs > 1)
	{
		//There are command line parameters.
		DWORD dwStart = 0, dwLength = 0, dwEnd = 0;
		LPWSTR szFileToLoad = NULL;
		LPWSTR szNextArgument = NULL;
		bool bSkipNextArg = false;

		for (int i = 1; i < iNumberOfArgs; i++)
		{
			LPWSTR szArgUnderTest = szArguments[i];

			if (i + 1 < iNumberOfArgs)
				szNextArgument = szArguments[i+1];
			else
				szNextArgument = NULL;

			if (szArgUnderTest[0] == '/')
			{
				switch (szArgUnderTest[1])
				{
				case 'S': // Start offset
				case 's':

					dwStart = ProcessSwitch(szArgUnderTest, szNextArgument, &bSkipNextArg);
					break;
				case 'L': // Length of selection
				case 'l':
					dwLength = ProcessSwitch(szArgUnderTest, szNextArgument, &bSkipNextArg);
					break;
				case 'E': // End of selection
				case 'e':
					dwEnd = ProcessSwitch(szArgUnderTest, szNextArgument, &bSkipNextArg);
					break;
				}

				if (bSkipNextArg)
				{
					//The ProcessSwitch function pulled the number out of the next command line
					//parameter. Don't attempt to use it for anything else!
					i++;
				}
			}
			else
			{
				szFileToLoad = szArgUnderTest;
			}
		}

		// If a user only selects switches, there's no file to load.
		if (szFileToLoad != NULL)
		{
			pHexWnd->open_file(szFileToLoad);
			
			if (dwLength)
				dwEnd = dwStart + dwLength - 1;
			if (dwEnd)
				pHexWnd->CMD_setselection(dwStart, dwEnd);
		}
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!pHexWnd || !pHexWnd->translate_accelerator(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	LocalFree(szArguments);

#ifdef _DEBUG
	// The System32 build cannot safely FreeLibrary(hMainInstance) because
	// frhed.exe and heksedit.dll share the same atexit list.
	::FreeLibrary(hMainInstance);
	_CrtDumpMemoryLeaks();
#endif
	OleUninitialize();
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
		hwndHex = CreateWindowEx(WS_EX_CLIENTEDGE, szHexClass, 0,
			WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			10, 10, 100, 100, hwnd, 0, hMainInstance, 0);
		pHexWnd = (HexEditorWindow *)GetWindowLongPtr(hwndHex, GWL_USERDATA);
		if (!pHexWnd)
			return -1;
		hwndToolBar = CreateTBar(hwndHex, hMainInstance);
		SetParent(hwndToolBar, hwnd);
		hwndStatusBar = CreateStatusWindow(
			CCS_BOTTOM | WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, hwndHex, 2);
		SetParent(hwndStatusBar, hwnd);
		pHexWnd->hwndMain = hwnd;
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
			pHexWnd = 0;
			PostQuitMessage(0);
		}
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam );
}

// Deals with command line switches. (/s, /e, /l) If the number is immediately
// after the switch, with no space, it is returned from szFirstArgument.
// Otherwise it is looked for in the next argument.
static unsigned long ProcessSwitch(LPWSTR szFirstArgument, LPWSTR szNextArgument, bool* bSkipNextArg)
{
	*bSkipNextArg = false;

	if (wcslen(szFirstArgument) < 3)
	{
		//The number associated with this switch could be in the next
		//item
		if (szNextArgument == NULL)
		{
			//Nope, it can't
			return 0;
		}
		else
		{
			// This flag indicates to the calling code that we used
			// the next command line argument to find the number
			// we wanted. Don't try to use it for, for example, a
			// filename!
			*bSkipNextArg = true;
			
			//We're reasonably well covered by the wcstoul function - if the input is unrecognised,
			//we'll get zero, and the switch is ignored.
			return wcstoul(szNextArgument, NULL, 0);
		}
	}
	else
	{
		return wcstoul(szFirstArgument + 2, NULL, 0);
	}

	//Make some compilers happy.
	return 0;
}
