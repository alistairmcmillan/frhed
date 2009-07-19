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
#include "offset.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL MoveCopyDlg::OnInitDialog(HWND hw)
{
	TCHAR buf[30] = {0};
	int iMove1stEnd = iGetStartOfSelection();
	int iMove2ndEndorLen = iGetEndOfSelection();
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iMove1stEnd);
	SetDlgItemText (hw, IDC_1STOFFSET, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iMove2ndEndorLen);
	SetDlgItemText (hw, IDC_2NDDELIM, buf);
	CheckDlgButton (hw, IDC_OTHEREND, BST_CHECKED);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("x%x"), iMovePos);
	SetDlgItemText(hw, IDC_MOVEMENT, buf);
	CheckDlgButton(hw, IDC_FPOS, BST_CHECKED);
	if (iMoveOpTyp == OPTYP_MOVE)
		CheckDlgButton(hw, IDC_MOVE, BST_CHECKED);
	else if (iMoveOpTyp == OPTYP_COPY)
		CheckDlgButton(hw, IDC_COPY, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Read start offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadStartOffset(HWND hDlg, int & value)
{
	TCHAR buf[30] = {0};
	HWND cntrl = GetDlgItem(hDlg, IDC_1STOFFSET);
	GetWindowText(cntrl, buf, RTL_NUMBER_OF(buf));

	if (buf[0] == '-')
	{
		LangString negativeOffset(IDS_CM_NEGATIVE_OFFSET);
		MessageBox(hDlg, negativeOffset, MB_ICONERROR);
		return false;
	}

	if (!offset_parse(&buf[0], value))
	{
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, MB_ICONERROR);
		return false;
	}
	return true;
}

/**
 * @brief Read end offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadEndOffset(HWND hDlg, int & value)
{
	TCHAR buf[30] = {0};
	HWND cntrl = GetDlgItem(hDlg, IDC_2NDDELIM);
	GetWindowText(cntrl, buf, RTL_NUMBER_OF(buf));
	int i = 0;
	if (buf[i] == '-')
	{
		if (!IsDlgButtonChecked(hDlg, IDC_LEN))
		{
			LangString negativeOffset(IDS_CM_NEGATIVE_OFFSET);
			MessageBox(hDlg, negativeOffset, MB_ICONERROR);
			return false;
		}
		// Relative jump. Read offset from next character on.
		i++;
	}
	
	// Skip the '-' if present
	if (!offset_parse(&buf[i], value))
	{
		LangString endOffsetErr(IDS_OFFSET_END_ERROR);
		MessageBox(hDlg, endOffsetErr, MB_ICONERROR);
		return false;
	}
	if (buf[0] == '-')
		value = -value; // Negate
	return true;
}

/**
 * @brief Read target offset value from the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @param [out] value Value read from the dialog.
 * @return true if the value was read, false if value could not be read.
 */
bool MoveCopyDlg::ReadTargetOffset(HWND hDlg, int & value)
{
	TCHAR buf[30] = {0};
	HWND cntrl = GetDlgItem(hDlg, IDC_MOVEMENT);
	GetWindowText(cntrl, buf, RTL_NUMBER_OF(buf));
	int i = 0;
	if (buf[i] == '-')
	{
		if (!IsDlgButtonChecked(hDlg, IDC_FORWARD))
		{
			LangString negativeOffset(IDS_CM_NEGATIVE_OFFSET);
			MessageBox(hDlg, negativeOffset, MB_ICONERROR);
			return false;
		}
		// Relative jump. Read offset from next character on.
		i++;
	}
	
	// Skip the '-' if present
	if (!offset_parse(&buf[i], value))
	{
		LangString endOffsetErr(IDS_CM_INVALID_TARGET);
		MessageBox(hDlg, endOffsetErr, MB_ICONERROR);
		return false;
	}
	if (buf[0] == '-')
		value = -value; // Negate
	return true;
}

/**
 * @brief Apply the copy/move of the data.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if operation succeeded, FALSE otherwise.
 */
BOOL MoveCopyDlg::Apply(HWND hw)
{
	int vals[3];
	if (!ReadStartOffset(hw, vals[0]))
		return FALSE;
	if (!ReadEndOffset(hw, vals[1]))
		return FALSE;
	if (!ReadTargetOffset(hw, vals[2]))
		return FALSE;
	
	int clen = m_dataArray.GetLength();
	int iMove1stEnd = vals[0];
	int iMove2ndEndorLen = vals[1];
	if (!IsDlgButtonChecked(hw, IDC_OTHEREND))
	{
		if (iMove2ndEndorLen == 0)
		{
			LangString zeroLen(IDS_CM_ZERO_LEN);
			MessageBox(hw, zeroLen, MB_OK | MB_ICONERROR);
			return FALSE;
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
