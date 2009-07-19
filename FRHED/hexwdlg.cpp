#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "UnicodeString.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

/**
 * @brief Get window text.
 * @param [in] hwnd Handle to window.
 * @param [out] str Window's text.
 */
void GetWindowText(HWND hwnd, String &str)
{
	const int len = GetWindowTextLength(hwnd) + 1;
	str.resize(len);
	TCHAR *temp = new TCHAR[len + 1];
	ZeroMemory(temp, len + 1);
	GetWindowText(hwnd, temp, len);
	str = temp;
	delete [] temp;
}

/**
 * @brief Get control's text.
 * @param [in] hwnd Handle to window.
 * @param [in] id Control's id.
 * @param [out] str Control's text.
 */
void GetDlgItemText(HWND hwnd, int id, String &str)
{
	hwnd = GetDlgItem(hwnd, id);
	GetWindowText(hwnd, str);
}

BOOL EnableDlgItem(HWND hwnd, int id, BOOL enable)
{
	hwnd = GetDlgItem(hwnd, id);
	return EnableWindow(hwnd, enable);
}

BOOL IsDlgItemEnabled(HWND hwnd, int id)
{
	hwnd = GetDlgItem(hwnd, id);
	return IsWindowEnabled(hwnd);
}

int MessageBox(HWND hwnd, LPCTSTR text, UINT type)
{
	LPCTSTR app = GetLangString(IDS_APPNAME);
	return MessageBox(hwnd, text, app, type);
}

int CheckHResult(HWND hwnd, HRESULT hr, UINT type)
{
	int response = 0;
	if (FAILED(hr))
	{
		TCHAR text[1024];
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, text, 1024, 0);
		response = MessageBox(hwnd, text, type);
	}
	return response;
}

C_ASSERT(sizeof(DragDropOptionsDlg) == sizeof(HexEditorWindow)); // disallow instance members

INT_PTR DragDropOptionsDlg::DlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	UINT checked = 0;
	switch (m)
	{
	case WM_INITDIALOG:
		checked = enable_drag ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_ENABLE_DRAG, checked);
		checked = enable_drop ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_ENABLE_DROP, checked);
		checked = enable_scroll_delay_dd ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_EN_SD_DD, checked);
		checked = enable_scroll_delay_sel ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_EN_SD_SEL, checked);
		checked = always_pick_move_copy ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_ALWAYS_CHOOSE, checked);
		checked = prefer_CF_HDROP ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DROP_CF_HDROP, checked);
		checked = prefer_CF_BINARYDATA ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DROP_BIN_DATA, checked);
		checked = prefer_CF_TEXT ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DROP_CF_TEXT, checked);
		checked = output_CF_BINARYDATA ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DRAG_BIN_DATA, checked);
		checked = output_CF_TEXT ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DRAG_CF_TEXT, checked);
		CheckDlgButton(h, output_text_special ? IDC_TEXT_SPECIAL : IDC_TEXT_HEXDUMP, BST_CHECKED);
		checked = output_text_hexdump_display ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_TEXT_DISPLAY, checked);
		checked = output_CF_RTF ? BST_CHECKED : BST_UNCHECKED;
		CheckDlgButton(h, IDC_DRAG_RTF, checked);
		return TRUE;
	case WM_COMMAND:
		switch (w)
		{
		case IDOK:
			enable_drag = IsDlgButtonChecked(h, IDC_ENABLE_DRAG);
			enable_drop = IsDlgButtonChecked(h, IDC_ENABLE_DROP);
			enable_scroll_delay_dd = IsDlgButtonChecked(h, IDC_EN_SD_DD);
			enable_scroll_delay_sel = IsDlgButtonChecked(h, IDC_EN_SD_SEL);
			always_pick_move_copy = IsDlgButtonChecked(h, IDC_ALWAYS_CHOOSE);
			prefer_CF_HDROP = IsDlgButtonChecked(h, IDC_DROP_CF_HDROP);
			prefer_CF_BINARYDATA = IsDlgButtonChecked(h, IDC_DROP_BIN_DATA);
			prefer_CF_TEXT = IsDlgButtonChecked(h, IDC_DROP_CF_TEXT);
			output_CF_BINARYDATA = IsDlgButtonChecked( h, IDC_DRAG_BIN_DATA);
			output_CF_TEXT = IsDlgButtonChecked(h, IDC_DRAG_CF_TEXT);
			output_text_special = IsDlgButtonChecked(h, IDC_TEXT_SPECIAL);
			output_text_hexdump_display = IsDlgButtonChecked(h, IDC_TEXT_DISPLAY);
			output_CF_RTF = IsDlgButtonChecked(h, IDC_DRAG_RTF);

			DragAcceptFiles(hwnd, !target || !enable_drop || enable_drop && prefer_CF_HDROP);

			if (target && enable_drop)
				RegisterDragDrop(hwnd, target);
			else
				RevokeDragDrop(hwnd);

			save_ini_data();
			// fall through
		case IDCANCEL:
			EndDialog(h, w);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(h);
		break;
	}
	return FALSE;
}

INT_PTR ChangeInstDlg::DlgProc(HWND hw, UINT m, WPARAM w, LPARAM l)
{
	//God damn spinners make life easy
	switch (m)
	{
	case WM_INITDIALOG:
		{
			int i = 0;
			HKEY hk;
			int iLoadInst = -1;
			int iSaveInst = 0;
			for (;;)
			{
				int i = iLoadInst + 1;
				const int num_size = 64;
				TCHAR num[num_size] = {0};
				_sntprintf(num, num_size - 1, _T("%s\\%d"), OptionsRegistrySettingsPath, i);
				if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, num, 0, KEY_EXECUTE, &hk))
					break;
				RegCloseKey(hk);//Close the key - just testing if it exists
				iLoadInst = i;
			}
			if (iLoadInst == -1)
			{
				LangString app(IDS_APPNAME);
				MessageBox(hwnd, _T("No instance data present"), app, MB_OK);
				EndDialog(hw, IDCANCEL);
				return TRUE;
			}
			//for both the spinners
			//iLoadInst is the max
			//iSaveInst is the min
			//iInstCount is the start pos
			LONG range = MAKELONG(iLoadInst, iSaveInst);
			LONG pos = MAKELONG(iInstCount, 0);
			HWND hWndUpDown = GetDlgItem(hw, IDC_SINST);
			SendMessage(hWndUpDown, UDM_SETRANGE, 0L, range);
			SendMessage(hWndUpDown, UDM_SETPOS, 0L, pos);
			hWndUpDown = GetDlgItem(hw, IDC_LINST);
			SendMessage(hWndUpDown, UDM_SETRANGE, 0L, range);
			SendMessage(hWndUpDown, UDM_SETPOS, 0L, pos);
			return TRUE;
		}
	case WM_COMMAND:
		switch (w)
		{
		case IDOK:
			iInstCount = SendDlgItemMessage(hw, IDC_LINST, UDM_GETPOS, 0, 0);
			read_ini_data();
			iInstCount = SendDlgItemMessage(hw, IDC_SINST, UDM_GETPOS, 0, 0);
			resize_window();
			// fall through
		case IDCANCEL:
			EndDialog(hw, w);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hw);
		break;
	}
	return FALSE;
}
