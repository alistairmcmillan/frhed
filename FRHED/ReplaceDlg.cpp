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
 * @file  ReplaceDlg.cpp
 *
 * @brief Implementation of the replace dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "Simparr.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "FindUtil.h"
#include "LangString.h"

// String containing data to replace.
SimpleString ReplaceDlg::strToReplaceData;
// String containing data to replace with.
SimpleString ReplaceDlg::strReplaceWithData;

//-------------------------------------------------------------------
// Translate the text in the string to binary data and store in the array.
int ReplaceDlg::transl_text_to_binary(SimpleArray<TCHAR> &out)
{
	TCHAR *pcOut;
	int destlen = create_bc_translation(&pcOut,
		(TCHAR *)strReplaceWithData, strReplaceWithData.StrLen(),
		iCharacterSet, iBinaryMode);
	if (destlen)
		out.Adopt(pcOut, destlen - 1, destlen);
	return destlen;
}

//-------------------------------------------------------------------
// Create a text representation of an array of bytes and save it in
// a SimpleString object.
int	ReplaceDlg::transl_binary_to_text(TCHAR *src, int len)
{
	// How long will the text representation of array of bytes be?
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen(src, len);
	strToReplaceData.SetSize(destlen);
	strToReplaceData.ExpandToSize();
	if (TCHAR *pd = strToReplaceData)
	{
		Text2BinTranslator::iTranslateBytesToBC(pd, (BYTE*) src, len);
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
int ReplaceDlg::find_and_select_data(int finddir, bool case_sensitive)
{
	TCHAR *tofind;
	// Create a translation from bytecode to char array of finddata.
	int destlen = create_bc_translation(&tofind, strToReplaceData,
		strToReplaceData.StrLen(), iCharacterSet, iBinaryMode);
	int i = iGetStartOfSelection();
	int n = iGetEndOfSelection() - i + 1;
	int j;
	if (finddir >= 0)
	{
		i += finddir * n;
		// Find forward.
		j = findutils_FindBytes((TCHAR *)&DataArray[i],
			DataArray.GetLength() - i - 1,
			tofind,	destlen, 1, case_sensitive);
		if (j != -1)
			i += j;
	}
	else
	{
		// Find backward.
		j = findutils_FindBytes((TCHAR *)&DataArray[0],
			min(iCurByte + (destlen - 1), DataArray.GetLength()),
			tofind, destlen, -1, case_sensitive);
		if (j != -1)
			i = j;
	}
	int done = 0;
	if (j != -1)
	{
		// NEW: Select found interval.
		bSelected = TRUE;
		iStartOfSelection = iCurByte = i;
		iEndOfSelection = iStartOfSelection + destlen - 1;
		done = 1;
	}
	delete [] tofind;
	return done;
}

//-------------------------------------------------------------------
// SimpleString replacedata contains data to replace with.
int ReplaceDlg::replace_selected_data(HWND hDlg)
{
	if (!bSelected)
	{
		LangString noData(IDS_REPL_NO_DATA);
		MessageBox(hDlg, noData, MB_ICONERROR);
		return FALSE;
	}
	int i = iGetStartOfSelection();
	int n = iGetEndOfSelection() - i + 1;
	if (strReplaceWithData.IsEmpty())
	{
		// Selected data is to be deleted, since replace-with data is empty string.
		if (!DataArray.Replace(i, n, 0, 0))
		{
			LangString couldNotDelete(IDS_REPL_CANT_DELETE);
			MessageBox(hDlg, couldNotDelete, MB_ICONERROR);
			return FALSE;
		}
		bSelected = FALSE;
		iCurByte = iStartOfSelection;
	}
	else if (iPasteAsText)
	{
		// Replace with non-zero-length data.
		if (!DataArray.Replace(i, n, (BYTE*)(TCHAR *)strReplaceWithData, strReplaceWithData.StrLen()))
		{
			LangString failed(IDS_REPL_FAILED);
			MessageBox(hDlg, failed, MB_ICONERROR);
			return FALSE;
		}
		iEndOfSelection = iStartOfSelection + strReplaceWithData.StrLen() - 1;
	}
	else
	{
		// Input string contains special-syntax-coded binary data.
		SimpleArray<TCHAR> out;
		if (!transl_text_to_binary(out))
		{
			LangString cannotConvert(IDS_REPL_CANNOT_CONVERT);
			MessageBox(hDlg, cannotConvert, MB_ICONERROR);
			return FALSE;
		}
		if (!DataArray.Replace(i, n, (BYTE*)(TCHAR*)out, out.GetLength()))
		{
			LangString failed(IDS_REPL_FAILED);
			MessageBox(hDlg, failed, MB_ICONERROR);
			return FALSE;
		}
		iEndOfSelection = iStartOfSelection + out.GetLength() - 1;
	}
	bFilestatusChanged = true;
	iFileChanged = TRUE;
	return TRUE;
}

void ReplaceDlg::find_directed(HWND hDlg, int finddir)
{
	GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
	bool case_sensitive = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) == BST_CHECKED;
	if (find_and_select_data(finddir, case_sensitive))
	{
		adjust_view_for_selection();
		repaint();
		synch_sibling();
		EnableDlgItem(hDlg, IDC_REPLACE_BUTTON, TRUE);
	}
	else
	{
		LangString notFound(IDS_FIND_CANNOT_FIND);
		MessageBox(hDlg, notFound, MB_ICONWARNING);
	}
}

void ReplaceDlg::replace_directed(HWND hDlg, int finddir, bool showCount)
{
	bool case_sensitive = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) == BST_CHECKED;
	GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
	GetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, strReplaceWithData);
	iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
	//------------------
	// Don't do anything if to-replace and replace-with data are same.
	Text2BinTranslator tr_find(strToReplaceData), tr_replace(strReplaceWithData);
	if (tr_find.bCompareBin(tr_replace, iCharacterSet, iBinaryMode))
	{
		LangString sameStrings(IDS_REPL_SAME_STRS);
		MessageBox(hDlg, sameStrings, MB_ICONERROR);
		return;
	}
	WaitCursor wc;
	int occ_num = 0;
	HWND hwndFocus = GetFocus();
	if (EnableDlgItem(hDlg, IDC_REPLACE_BUTTON, FALSE) == FALSE)
	{
		// Don't lose focus.
		if (!IsWindowEnabled(hwndFocus))
			SetFocus(hDlg);
		occ_num++;
		replace_selected_data(hDlg);
	}
	if (finddir)
	{
		while (find_and_select_data(finddir, case_sensitive))
		{
			occ_num++;
			replace_selected_data(hDlg);
		};
	}
	if (occ_num)
	{
		set_wnd_title();
		adjust_view_for_selection();
		resize_window();
		synch_sibling();

		if (showCount)
		{
			TCHAR tbuf[100];
			_stprintf(tbuf, GetLangString(IDS_REPL_COUNT), occ_num);
			MessageBox(hDlg, tbuf, MB_ICONINFORMATION);
		}
	}
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR ReplaceDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		// If there is selected data then make it the data to find.
		if (bSelected)
		{
			int sel_start = iGetStartOfSelection();
			int select_len = iGetEndOfSelection() - sel_start + 1;
			if (!transl_binary_to_text((TCHAR *)&DataArray[sel_start], select_len))
			{
				LangString badSelect(IDS_REPL_BAD_SELECT);
				MessageBox(hDlg, badSelect, MB_OK);
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
		}
		CheckDlgButton(hDlg, IDC_USETRANSLATION_CHECK, !iPasteAsText);
		if (TCHAR *pstr = strToReplaceData)
			SetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, pstr);
		if (TCHAR *pstr = strReplaceWithData)
			SetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, pstr);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDCANCEL:
			iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
			EndDialog(hDlg, wParam);
			return TRUE;

		case IDC_FINDNEXT_BUTTON:
			find_directed(hDlg, 1);
			break;

		case IDC_FINDPREVIOUS_BUTTON:
			find_directed(hDlg, -1);
			break;

		case IDC_FOLLOCC_BUTTON:
			replace_directed(hDlg, 1, true);
			break;

		case IDC_PREVOCC_BUTTON:
			replace_directed(hDlg, -1, true);
			break;

		case IDC_REPLACE_BUTTON:
			replace_directed(hDlg, 0, false);
			break;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
