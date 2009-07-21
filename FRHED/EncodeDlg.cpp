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

static void AddEncoders(HWND hListbox, LPMEMORY_CODING_DESCRIPTION lpEncoders)
{
	for ( ; lpEncoders->lpszDescription ; ++lpEncoders)
	{
		int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)lpEncoders->lpszDescription);
		SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)lpEncoders->fpEncodeFunc);
	}
}

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL EncodeDecodeDialog::OnInitDialog(HWND hDlg)
{
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
		prev_ind = ind;
		ind = buffer.find_first_of(';', prev_ind + 1);
	}
	SendMessage(hListbox, LB_SETCURSEL, 0, 0);
	CheckDlgButton(hDlg, IDC_ENCODE_ENC, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Handle dialog closing with OK button (and applying the changes).
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL EncodeDecodeDialog::OnOK(HWND hDlg, WPARAM wParam, LPARAM lParam)
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
	EndDialog(hDlg, wParam);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL EncodeDecodeDialog::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
		return OnOK(hDlg, wParam, lParam);

	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR EncodeDecodeDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);

	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}
