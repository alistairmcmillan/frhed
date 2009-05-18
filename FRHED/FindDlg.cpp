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
 * @file  FindDlg.cpp
 *
 * @brief Implementation of the Find dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "FindUtil.h"
#include "LangString.h"
#include "FindCtxt.h"

INT_PTR FindDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		// If there is selected data then make it the data to find.
		if (bSelected)
		{
			// Get start offset and length (is at least =1) of selection.
			int sel_start = iGetStartOfSelection();
			int select_len = iGetEndOfSelection() - sel_start + 1;
			// Get the length of the bytecode representation of the selection (including zero-byte at end).
			int findlen = Text2BinTranslator::iBytes2BytecodeDestLen((TCHAR *)&DataArray[sel_start], select_len);
			if (findlen > FindCtxt::MAX_TEXT_LEN)
			{
				LangString largeSel(IDS_FIND_SEL_TOO_LARGE);
				MessageBox(hDlg, largeSel, MB_ICONERROR);
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
			// Translate the selection into bytecode and write it into the find text buffer.
			int destLen = Text2BinTranslator::iBytes2BytecodeDestLen((TCHAR *)&DataArray[sel_start], select_len);
			TCHAR * tmpBuf = new TCHAR[destLen + 1];
			ZeroMemory(tmpBuf, (destLen + 1) * sizeof(TCHAR));
			Text2BinTranslator::iTranslateBytesToBC(tmpBuf, &DataArray[sel_start], select_len);
			m_pFindCtxt->SetText(tmpBuf);
			delete [] tmpBuf;
		}
		// Put inside block to allow using local variables to handle dialog data
		{
			SendDlgItemMessage(hDlg, IDC_FIND_TEXT, EM_SETLIMITTEXT, FindCtxt::MAX_TEXT_LEN, 0);
			SetDlgItemText(hDlg, IDC_FIND_TEXT, m_pFindCtxt->GetText());
			//GK16AUG2K
			CheckDlgButton(hDlg, m_pFindCtxt->m_iDirection == -1 ? IDC_FIND_UP : IDC_FIND_DOWN, BST_CHECKED);
			const UINT matchCase = m_pFindCtxt->m_bMatchCase ? BST_CHECKED : BST_UNCHECKED;
			CheckDlgButton(hDlg, IDC_FIND_MATCHCASE, matchCase);
			const UINT findUnicode = m_pFindCtxt->m_bUnicode ? BST_CHECKED : BST_UNCHECKED;
			CheckDlgButton(hDlg, IDC_FIND_UNICODE, findUnicode);
		}
		return TRUE;

	case WM_COMMAND:
		//GK16AUG2K
		switch (wParam)
		{
		case IDOK:
		{
			TCHAR * tmpBuf = new TCHAR[FindCtxt::MAX_TEXT_LEN + 1];
			ZeroMemory(tmpBuf, (FindCtxt::MAX_TEXT_LEN + 1) * sizeof(TCHAR));
			int srclen = GetDlgItemText(hDlg, IDC_FIND_TEXT, tmpBuf, FindCtxt::MAX_TEXT_LEN);
			m_pFindCtxt->SetText(tmpBuf);
			delete [] tmpBuf;
			if (srclen)
			{
				UINT matchCase = IsDlgButtonChecked(hDlg, IDC_FIND_MATCHCASE);
				m_pFindCtxt->m_bMatchCase = matchCase == BST_CHECKED ? true : false;
				UINT findUnicode = IsDlgButtonChecked(hDlg, IDC_FIND_UNICODE);
				m_pFindCtxt->m_bUnicode = findUnicode == BST_CHECKED ? true : false;
				m_pFindCtxt->m_iDirection = IsDlgButtonChecked(hDlg, IDC_FIND_UP) ? -1 : 1;
				// Copy text in Edit-Control. Return the number of characters
				// in the Edit-control minus the zero byte at the end.
				TCHAR *pcFindstring = 0;
				int destlen;
				if (m_pFindCtxt->m_bUnicode)
				{
					pcFindstring = new TCHAR[srclen * 2];
					destlen = MultiByteToWideChar(CP_ACP, 0, m_pFindCtxt->GetText(),
							srclen, (WCHAR *)pcFindstring, srclen) * 2;
				}
				else
				{
					// Create findstring.
					destlen = create_bc_translation(&pcFindstring,
							m_pFindCtxt->GetText(), srclen, iCharacterSet,
							iBinaryMode);
				}
				if (destlen)
				{
					int i;
					SetCursor(LoadCursor(NULL, IDC_WAIT));
					// Find forward.
					if (m_pFindCtxt->m_iDirection == 1)
					{
						i = findutils_FindBytes((TCHAR *)&DataArray[iCurByte + 1],
								DataArray.GetLength() - iCurByte - 1,
								pcFindstring, destlen, 1, m_pFindCtxt->m_bMatchCase);
						if (i != -1)
							iCurByte += i + 1;
					}
					// Find backward.
					else
					{
						i = findutils_FindBytes((TCHAR *)&DataArray[0],
							min(iCurByte + (destlen - 1), DataArray.GetLength()),
							pcFindstring, destlen, -1, m_pFindCtxt->m_bMatchCase);
						if (i != -1)
							iCurByte = i;
					}
					SetCursor(LoadCursor(NULL, IDC_ARROW));

					if (i != -1)
					{
						// Select found interval.
						bSelected = true;
						iStartOfSelection = iCurByte;
						iEndOfSelection = iCurByte + destlen - 1;
						adjust_view_for_selection();
						repaint();
					}
					else
					{
						LangString msg(IDS_FIND_CANNOT_FIND);
						MessageBox(hDlg, msg, MB_ICONWARNING);
					}
					//GK16AUG2K
				}
				else
				{
					LangString msg(IDS_FIND_EMPTY_STRING);
					MessageBox(hDlg, msg, MB_ICONERROR);
				}
				delete[] pcFindstring;
			}
			else
			{
				LangString msg(IDS_FIND_EMPTY_STRING);
				MessageBox(hDlg, msg, MB_ICONERROR);
			}
		}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
