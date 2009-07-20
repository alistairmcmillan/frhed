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
 * @file EncodeDlg.cpp
 *
 * @brief Encode/decode dialog implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "EncodeDlg.h"
#include "EncoderLib.h"
#include "UnicodeString.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

MEMORY_CODING_DESCRIPTION BuiltinEncoders[] =
{
	{ _T("ROT-13"), Rot13Encoder },
	{ _T("XOR -1"), XorEncoder },
	{ 0, 0 }
};

void AddEncoders(HWND hListbox, LPMEMORY_CODING_DESCRIPTION lpEncoders)
{
	for ( ; lpEncoders->lpszDescription ; ++lpEncoders)
	{
		int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)lpEncoders->lpszDescription);
		SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)lpEncoders->fpEncodeFunc);
	}
}

INT_PTR EncodeDecodeDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			//SimpleString buffer((LPTSTR)(LPCTSTR)EncodeDlls);
			//LPCSTR lpszToken = strtok(buffer, ";");
			HWND hListbox = GetDlgItem(hDlg, IDC_ENCODE_LIST);
			AddEncoders(hListbox, BuiltinEncoders);

			String buffer(EncodeDlls);
			size_t prev_ind = 0;
			size_t ind = buffer.find_first_of(';');
			while (prev_ind != buffer.npos && ind != buffer.npos)
			{
				String dll = buffer.substr(prev_ind, ind - prev_ind);
				HMODULE hLibrary = GetModuleHandle(dll.c_str());
				if (hLibrary == 0)
					hLibrary = LoadLibrary(dll.c_str());
				if (hLibrary)
				{
					if (LPFNGetMemoryCodings callback = (LPFNGetMemoryCodings)
						GetProcAddress(hLibrary, _T("GetMemoryCodings")))
					{
						AddEncoders(hListbox, callback());
					}
				}
				//lpszToken = strtok(0, ";");
				prev_ind = ind;
				ind = buffer.find_first_of(';', prev_ind + 1);
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			CheckDlgButton(hDlg, IDC_ENCODE_ENC, BST_CHECKED);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				MEMORY_CODING mc;
				TCHAR szBuffer[1024];
				GetDlgItemText(hDlg, IDC_ENCODE_ARGS, szBuffer, sizeof(szBuffer));
				mc.bEncode = IsDlgButtonChecked(hDlg, IDC_ENCODE_ENC);
				mc.lpszArguments = szBuffer;
				HWND hListbox = GetDlgItem(hDlg, IDC_ENCODE_LIST);
				int nCurSel = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				if (nCurSel < 0)
					return TRUE;
				mc.fpEncodeFunc = (LPFNEncodeMemoryFunction) SendMessage(hListbox, LB_GETITEMDATA, nCurSel, 0);
				int lower = 0;
				int upper = m_dataArray.GetUpperBound();
				if (bSelected)
				{
					lower = iGetStartOfSelection();
					upper = iGetEndOfSelection();
				}
				mc.lpbMemory = &m_dataArray[lower];
				mc.dwSize = upper - lower + 1;
				mc.fpEncodeFunc(&mc);
				iFileChanged = TRUE;
				bFilestatusChanged = true;
				repaint();
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
