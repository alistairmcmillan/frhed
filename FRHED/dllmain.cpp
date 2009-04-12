//============================================================================================
// frhed - free hex editor

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"

HINSTANCE hMainInstance;
LRESULT CALLBACK HexWndProc(HWND, UINT, WPARAM, LPARAM);

static const TCHAR szHexClass[] = _T("heksedit");

//--------------------------------------------------------------------------------------------
// WinMain: the starting point.
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		hMainInstance = hInstance;
		WNDCLASSEX wndclass;
		ZeroMemory(&wndclass, sizeof wndclass);
		wndclass.cbSize = sizeof wndclass;
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
		wndclass.lpfnWndProc = HexWndProc;
		wndclass.hInstance = hInstance;
		wndclass.hCursor = 0;
		wndclass.lpszClassName = szHexClass;
		RegisterClassEx(&wndclass);
		HexEditorWindow::LoadStringTable();
		return TRUE;
	}
	if (dwReason == DLL_PROCESS_DETACH)
	{
		HexEditorWindow::FreeStringTable();
	}
	return FALSE;
}

// The hex window procedure.
LRESULT CALLBACK HexWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if (iMsg == WM_NCCREATE)
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)new HexEditorWindow);
	HexEditorWindow *pHexWnd = (HexEditorWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LRESULT lResult = pHexWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
	if (iMsg == WM_NCDESTROY)
		delete pHexWnd;
	return lResult;
}
//============================================================================================

/**
 * @brief Export what is needed to be a COM in-process server.
 */
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12")

/**
 * @brief InProcServer32 subkey.
 */
static const TCHAR InProcServer32[] =
	_T("CLSID\\{BCA3CA6B-CC6B-4F79-A2C2-DDBE864B1C90}\\InProcServer32");

/**
 * @brief Register this DLL as an in-process server.
 */
STDAPI DllRegisterServer()
{
	TCHAR path[MAX_PATH];
	DWORD cb = GetModuleFileName(hMainInstance, path, MAX_PATH) * sizeof(TCHAR);
	LONG err = RegSetValue(HKEY_CLASSES_ROOT, InProcServer32, REG_SZ, path, cb);
	return HRESULT_FROM_WIN32(err);
}

/**
 * @brief Unregister this DLL as an in-process server.
 */
STDAPI DllUnregisterServer()
{
	LONG err = RegDeleteKey(HKEY_CLASSES_ROOT, InProcServer32);
	return HRESULT_FROM_WIN32(err);
}

/**
 * @brief Do not actually expose any COM classes.
 */
STDAPI DllGetClassObject(REFCLSID, REFIID, LPVOID *ppv)
{
	// Return an IMalloc just to return something.
	return CoGetMalloc(1, reinterpret_cast<IMalloc **>(ppv));
}
