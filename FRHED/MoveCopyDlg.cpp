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
 * @file  MoveCopyDlg.cpp
 *
 * @brief Implementation of the data moving/copying dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"
#include "LangString.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL MoveCopyDlg::OnInitDialog(HWND hw)
{
	char buf[30] = {0};
	int iMove1stEnd = iGetStartOfSelection();
	int iMove2ndEndorLen = iGetEndOfSelection();
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", iMove1stEnd);
	SetDlgItemText (hw, IDC_1STOFFSET, buf);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", iMove2ndEndorLen);
	SetDlgItemText (hw, IDC_2NDDELIM, buf);
	CheckDlgButton (hw, IDC_OTHEREND, BST_CHECKED);
	_snprintf(buf, RTL_NUMBER_OF(buf) - 1, "x%x", iMovePos);
	SetDlgItemText(hw, IDC_MOVEMENT, buf);
	CheckDlgButton(hw, IDC_FPOS, BST_CHECKED);
	if (iMoveOpTyp == OPTYP_MOVE)
		CheckDlgButton(hw, IDC_MOVE, BST_CHECKED);
	else if (iMoveOpTyp == OPTYP_COPY)
		CheckDlgButton(hw, IDC_COPY, BST_CHECKED);
	return TRUE;
}

BOOL MoveCopyDlg::Apply(HWND hw)
{
	char buf[30] = {0};
	const int dlgitems[3] = { IDC_1STOFFSET, IDC_2NDDELIM, IDC_MOVEMENT };
	const int check[3] = { 0, IDC_LEN, IDC_FORWARD };
	int vals[3];
	for (int n = 0 ; n < 3; n++)
	{
		HWND cntrl = GetDlgItem(hw, dlgitems[n]);
		GetWindowText(cntrl, buf, RTL_NUMBER_OF(buf));
		int i = 0;
		if (n && buf[i] == '-')
		{
			if (!IsDlgButtonChecked(hw, check[n]))
			{
				LangString negativeOffset(IDS_CM_NEGATIVE_OFFSET);
				MessageBox(hw, negativeOffset, MB_ICONERROR);
				return FALSE;
			}
			// Relative jump. Read offset from next character on.
			i++;
		}
		if (sscanf(&buf[i], "x%x", &vals[n]) == 0 &&
			sscanf(&buf[i], "%d", &vals[n]) == 0)
		{
			// No fields assigned: badly formed number.
			char msg[80] = {0};
			_snprintf(msg, RTL_NUMBER_OF(msg) - 1, GetLangString(IDS_CM_INVALID_DATA), n + 1);
			MessageBox(hw, msg, MB_ICONERROR);
			return FALSE;
		}
		if (buf[0] == '-')
			vals[n] = -vals[n]; //Negate
	}
	int clen = DataArray.GetLength();
	int iMove1stEnd = vals[0];
	int iMove2ndEndorLen = vals[1];
	if (!IsDlgButtonChecked(hw, IDC_OTHEREND))
	{
		if (iMove2ndEndorLen == 0)
		{
			LangString zeroLen(IDS_CM_ZERO_LEN);
			MessageBox(hw, zeroLen, MB_OK | MB_ICONERROR);
			return 0;
		}
		if (iMove2ndEndorLen > 0)
			iMove2ndEndorLen--;
		else
			iMove2ndEndorLen++;
		iMove2ndEndorLen += iMove1stEnd;
	}

	if (iMove1stEnd < 0 ||
		iMove1stEnd >= clen ||
		iMove2ndEndorLen < 0 ||
		iMove2ndEndorLen >= clen)
	{
		LangString invalidBlock(IDS_CM_INVALID_BLOCK);
		MessageBox(hw, invalidBlock, MB_ICONERROR);
		return FALSE;
	}

	if (iMove1stEnd > iMove2ndEndorLen)
		swap(iMove1stEnd, iMove2ndEndorLen);

	if (IsDlgButtonChecked(hw, IDC_FPOS))
		iMovePos = vals[2];
	else if(IsDlgButtonChecked(hw,IDC_LPOS))
		iMovePos = iMove1stEnd + vals[2] - iMove2ndEndorLen;
	else
		iMovePos = iMove1stEnd + vals[2];

	iMoveOpTyp = IsDlgButtonChecked(hw, IDC_MOVE) ? OPTYP_MOVE : OPTYP_COPY;

	if (iMovePos == iMove1stEnd && iMoveOpTyp == OPTYP_MOVE)
	{
		LangString notMoved(IDS_CM_NOT_MOVED);
		MessageBox(hw, notMoved, MB_ICONEXCLAMATION);
		EndDialog(hw, 0);
		return FALSE;
	}

	if (iMovePos < 0 || (iMoveOpTyp == OPTYP_MOVE ?
		iMovePos + iMove2ndEndorLen - iMove1stEnd >= clen :
		iMovePos > clen))
	{
		LangString outsideData(IDS_CM_OUTSIDE_DATA);
		MessageBox(hw, outsideData,	MB_OK | MB_ICONERROR);
		return FALSE;
	}
	CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, true);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL MoveCopyDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
		if (Apply(hDlg))
		{
		case IDCANCEL:
			EndDialog(hDlg, wParam);
		}
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
INT_PTR MoveCopyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
