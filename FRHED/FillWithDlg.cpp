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
 * @file  FillWithDlg.cpp
 *
 * @brief Implementation of the Fill dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangString.h"

C_ASSERT(sizeof(FillWithDialog) == sizeof(HexEditorWindow)); // disallow instance members

//Pabs changed - line insert
TCHAR FillWithDialog::pcFWText[FW_MAX];//hex representation of bytes to fill with
TCHAR FillWithDialog::buf[FW_MAX];//bytes to fill with
int FillWithDialog::buflen;//number of bytes to fill with
TCHAR FillWithDialog::szFWFileName[_MAX_PATH];//fill with file name
int FillWithDialog::FWFile;//fill with file
int FillWithDialog::FWFilelen;//fill with file len
LONG_PTR FillWithDialog::oldproc;//old hex box proc
//LONG cmdoldproc;//old command box proc
HFONT FillWithDialog::hfon;//needed so possible to display infinity char in fill with dlg box
TCHAR FillWithDialog::curtyp;//filling with input-0 or file-1
TCHAR FillWithDialog::asstyp;
//see CMD_fw below

void FillWithDialog::inittxt(HWND hDlg)
{
	int iStartOfSelSetting;
	int iEndOfSelSetting;
	if (bSelected)
	{
		iStartOfSelSetting = iStartOfSelection;
		iEndOfSelSetting = iEndOfSelection;
	}
	else
	{
		iStartOfSelSetting = 0;
		iEndOfSelSetting = m_dataArray.GetUpperBound();
	}
	//init all the readonly boxes down below
	TCHAR bufff[250] = {0};
	int tteemmpp = 1 + abs(iStartOfSelSetting - iEndOfSelSetting);
	_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"), iStartOfSelSetting, iStartOfSelSetting);
	SetDlgItemText(hDlg, IDC_STS,bufff);
	_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"), iEndOfSelSetting, iEndOfSelSetting);
	SetDlgItemText(hDlg, IDC_ES,bufff);
	_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"),tteemmpp,tteemmpp);
	SetDlgItemText(hDlg, IDC_SS,bufff);
	if (curtyp)
	{//1-file
		SetDlgItemText(hDlg,IDC_SI, _T("???"));
		SetDlgItemText(hDlg,IDC_IFS, _T("???"));
		SetDlgItemText(hDlg,IDC_R, _T("???"));
	}
	else
	{//0-input
		_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"), buflen, buflen);
		SetDlgItemText(hDlg,IDC_SI, bufff);
		if (buflen)
		{
			int d = tteemmpp / buflen;
			int m = tteemmpp % buflen;
			_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"), d, d);
			SetDlgItemText(hDlg,IDC_IFS, bufff);
			HFONT hfdef = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
			SendDlgItemMessage(hDlg,IDC_IFS,WM_SETFONT,(WPARAM) hfdef, MAKELPARAM(TRUE, 0));
			_sntprintf(bufff, RTL_NUMBER_OF(bufff) - 1, _T("%d=0x%x"), m, m);
			SetDlgItemText(hDlg,IDC_R, bufff);
		}
		else
		{
			SetDlgItemText(hDlg,IDC_IFS, _T("\xa5"));//set to infinity symbol
			SendDlgItemMessage(hDlg,IDC_IFS,WM_SETFONT,(WPARAM) hfon,MAKELPARAM(TRUE, 0));
			SetDlgItemText(hDlg,IDC_R, _T("0=0x0"));
		}
	}
}

//-------------------------------------------------------------------
// Following code by Pabs.

BYTE FillWithDialog::input(int index)
{
	return buf[index];
}

//see CMD_fw below
BYTE FillWithDialog::file(int index)
{
	BYTE x;
	_lseek(FWFile, index, SEEK_SET);
	_read(FWFile,&x,1);
	return x;
}

//convert a string of hex digits to a string of chars
void FillWithDialog::hexstring2charstring()
{
	// RK: removed definition of variable "a".
	int i, ii =_tcslen(pcFWText);
	if (ii % 2)//if number of hex digits is odd
	{
		//concatenate them
		for (i = 0 ; i < ii ; i++)
			pcFWText[ii + i] = pcFWText[i];
		pcFWText[ii * 2] = 0;
	}
	for (i = ii = 0 ; pcFWText[i] != '\0' ; i += 2)
	{
		// RK: next two lines changed, would crash when compiled with VC++ 4.0.
		/*
		sscanf(pcTemp,"%2x",&a);//get byte from the hexstring
		buf[ii]=a;//store it
		*/
		// Replaced with this line:
		_stscanf(pcFWText + i, _T("%2x"), buf + ii);//get byte from the hexstring
		ii++;
	}//for
	buflen = ii;//store length
}//func

//used to delete non-hex chars after the user pastes into the hexbox
void FillWithDialog::deletenonhex(HWND hEd)
{
	GetWindowText(hEd, pcFWText, FW_MAX);
	int ii = 0;
	for (int i = 0; pcFWText[i] != '\0'; i++)
	{
		if (_istxdigit(pcFWText[i]))
		{
			pcFWText[ii] = pcFWText[i];
			ii++;
		}
	}
	pcFWText[ii] = '\0';
	SetWindowText(hEd, pcFWText);
}

//hex box msg handler
LRESULT CALLBACK FillWithDialog::HexProc(HWND hEdit, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CHAR:
		// only enter chars if they are hex digits or backspace
		if (!_istxdigit((TBYTE)wParam) && wParam != VK_BACK)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			return 0;
		}
		break;
	case WM_PASTE:
		CallWindowProc((WNDPROC)oldproc, hEdit, iMsg, wParam, lParam);//paste as usual
		deletenonhex(hEdit);//but delete non-hex chars
		return 0;
	}
	return CallWindowProc((WNDPROC)oldproc, hEdit, iMsg, wParam, lParam);
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR FillWithDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hEditt = GetDlgItem(hDlg, IDC_HEX);//get the handle to the hex edit box
			SendMessage(hEditt, EM_SETLIMITTEXT, (WPARAM)FW_MAX, 0);//limit the amount of text the user can enter
			SetWindowText(hEditt, pcFWText);//init hex text
			SetFocus(hEditt);//give the hex box focus
			EnableWindow(hEditt, !curtyp);
			oldproc = (LONG_PTR) SetWindowLongPtr(hEditt, GWLP_WNDPROC, (LONG_PTR)HexProc);//override the old proc to be HexProc
			EnableWindow(GetDlgItem(hDlg, IDC_HEXSTAT),!curtyp);

			HWND typ = GetDlgItem(hDlg, IDC_TYPE);
			SendMessage(typ, CB_ADDSTRING ,0, (LPARAM)_T("Input"));
			SendMessage(typ, CB_ADDSTRING ,0, (LPARAM)_T("File"));
			SendMessage(typ, CB_SETCURSEL, (WPARAM)curtyp,0);//set cursel to previous

			//en/disable filename box and browse button
			HWND fn = GetDlgItem(hDlg, IDC_FN);
			SetWindowText(fn, szFWFileName);
			EnableWindow(fn, curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_BROWSE), curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_FILESTAT), curtyp);

			hfon = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0,
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Symbol"));
			inittxt(hDlg);
			switch (asstyp)
			{
			case 0: 
				CheckDlgButton(hDlg, IDC_EQ, BST_CHECKED);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_OR, BST_CHECKED);
				break;
			case 2:
				CheckDlgButton(hDlg, IDC_AND, BST_CHECKED);
				break;
			case 3:
				CheckDlgButton(hDlg, IDC_XOR, BST_CHECKED);
				break;
			}
			return 0;//stop the system from setting focus to the control handle in (HWND) wParam because we already set focus above
		}
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK: //ok pressed
			{
				if (curtyp)
				{//1-file
					GetDlgItemText(hDlg, IDC_FN, szFWFileName, _MAX_PATH);//get file name
					FWFile = _topen(szFWFileName, _O_RDONLY | _O_BINARY);
					if (FWFile == -1)
					{//if there is error opening
						LangString errOpen(IDS_ERR_OPENING_FILE);
						MessageBox(hDlg, errOpen, MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					FWFilelen = _filelength(FWFile);
					if (FWFilelen == 0)
					{//if filelen is zero
						LangString zeroSize(IDS_FILL_ZERO_SIZE_FILE);
						MessageBox(hDlg, zeroSize, MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//if
					else if (FWFilelen == -1)
					{//error returned by _filelength
						LangString errOpen(IDS_ERR_OPENING_FILE);
						MessageBox(hDlg, errOpen, MB_ICONERROR);//tell user but don't close dlgbox
						_close(FWFile);//close file
						return 1;//didn't process this message
					}//elseif
				}
				else
				{//0-input
					if (!buflen)
					{//no hex input
						LangString zeroSize(IDS_FILL_ZERO_SIZE_STR);
						MessageBox(hDlg, zeroSize, MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					int i = GetDlgItemText(hDlg, IDC_HEX, pcFWText, FW_MAX);
					if (i == 0 || i == FW_MAX - 1)
					{//error
						LangString tooManyBytes(IDS_FILL_TOO_MANY_BYTES);
						MessageBox(hDlg, tooManyBytes, MB_ICONERROR);//tell user but don't close dlgbox
						return 1;//didn't process this message
					}//if
					hexstring2charstring();//just in case
					//pcFWText[(aa?buflen:buflen*2)]='\0';//access violation if i do it in the above function

				}
				if (IsDlgButtonChecked(hDlg,IDC_EQ))
					asstyp = 0;
				else if (IsDlgButtonChecked(hDlg,IDC_OR))
					asstyp = 1;
				else if (IsDlgButtonChecked(hDlg,IDC_AND))
					asstyp = 2;
				else if (IsDlgButtonChecked(hDlg,IDC_XOR))
					asstyp = 3;

				// go ahead
				SetCursor(LoadCursor(NULL,IDC_WAIT));
				BYTE (*fnc)(int);
				int iStartOfSelSetting;
				int iEndOfSelSetting;
				int iimax;
				if (curtyp)
				{//1-file
					fnc = file;
					iimax = FWFilelen;
				}//if
				else
				{//0-input
					fnc = input;
					iimax = buflen;
				}//else

				if (bSelected)
				{
					iStartOfSelSetting = iGetStartOfSelection();
					iEndOfSelSetting = iGetEndOfSelection();
				}
				else
				{
					iStartOfSelSetting = 0;
					iEndOfSelSetting = m_dataArray.GetUpperBound();
				}

				int i = iStartOfSelSetting;
				int ii = 0;
				switch (asstyp)
				{// use switch instead of pointers to funcs that just call an operator as its faster
				case 0:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] = fnc(ii++);
						ii %= iimax;
					}
					break;
				case 1:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] |= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 2:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] &= fnc(ii++);
						ii %= iimax;
					}
					break;
				case 3:
					while (i <= iEndOfSelSetting)
					{
						m_dataArray[i++] ^= fnc(ii++);
						ii %= iimax;
					}
					break;
				}
				if (curtyp)
					_close(FWFile);//close file
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				iFileChanged = TRUE;//mark as changed
				bFilestatusChanged = true;
				repaint();//you tell me
			}
			// fall through
		case IDCANCEL: //cancel pressed
			DeleteObject(hfon);// won't need till next time
			EndDialog(hDlg, wParam);//tell CMD_fw not to carry out the fill with operation
			return 1;//did process this message
		case MAKEWPARAM(IDC_TYPE, CBN_SELCHANGE):
			//thing to fill selection with changes
			curtyp = (char)SendMessage(GetDlgItem(hDlg, IDC_TYPE), CB_GETCURSEL, 0, 0);//get cursel
			EnableWindow(GetDlgItem(hDlg, IDC_FN), curtyp);//en/disable fnamebox and browse button
			EnableWindow(GetDlgItem(hDlg, IDC_BROWSE), curtyp);
			EnableWindow(GetDlgItem(hDlg, IDC_FILESTAT), curtyp);
			curtyp = !curtyp;//flip it for the others
			EnableWindow(GetDlgItem(hDlg, IDC_HEX), curtyp);//en/disable hexboxand relateds
			EnableWindow(GetDlgItem(hDlg, IDC_HEXSTAT), curtyp);
			curtyp = !curtyp;//restore original value -not for below -accurate value needed elsewhere
			//set text in boxes down below
			inittxt(hDlg);
			break;
		case IDC_BROWSE:
			{
				//prepare OPENFILENAME for the file open common dlg box
				LangString allFiles(IDS_OPEN_ALL_FILES);
				szFWFileName[0] = '\0';
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof ofn);
				ofn.lStructSize = sizeof ofn;
				ofn.hwndOwner = hDlg;
				ofn.lpstrFilter = allFiles;
				ofn.lpstrFile = szFWFileName;
				ofn.nMaxFile = _MAX_PATH;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
				//show open dlgbox and if file good save name & path in edit box
				if (GetOpenFileName(&ofn))
					SetDlgItemText(hDlg, IDC_FN, ofn.lpstrFile);
			}
			return TRUE;
		case MAKEWPARAM(IDC_HEX, EN_UPDATE): //hexedit updated
			GetWindowText(GetDlgItem(hDlg, IDC_HEX), pcFWText, FW_MAX);//gettext
			hexstring2charstring();//convert to char string
			//set text in boxes down below
			inittxt(hDlg);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
