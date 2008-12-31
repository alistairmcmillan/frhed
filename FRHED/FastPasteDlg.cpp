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
 * @file  FastPasteDlg.cpp
 *
 * @brief Implementation of the Fast Paste dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

static HWND hwndNextViewer = NULL;

BOOL FastPasteDlg::OnInitDialog(HWND hDlg)
{
	SendMessage(hDlg, WM_COMMAND, IDC_FPASTE_REFRESH, 0);
	hwndNextViewer = SetClipboardViewer(hDlg);
	SetDlgItemInt(hDlg, IDC_FPASTE_PASTETIMES, iPasteTimes, TRUE);
	SetDlgItemInt(hDlg, IDC_FPASTE_SKIPS, iPasteSkip, TRUE);
	// Depending on INS or OVR mode, set the radio button.
	if (bSelected) // iPasteMode = 0
	{
		EnableWindow(GetDlgItem(hDlg, IDC_FPASTE_OWERWRITE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_FPASTE_INSERT), FALSE);
	}
	else if (iInsertMode) // iPasteMode = 2
	{
		CheckDlgButton(hDlg, IDC_FPASTE_INSERT, BST_CHECKED);
	}
	else // iPasteMode = 1
	{
		CheckDlgButton(hDlg, IDC_FPASTE_OWERWRITE, BST_CHECKED);
	}
	CheckDlgButton(hDlg, IDC_CHECK1, iPasteAsText);
	return TRUE;
}

BOOL FastPasteDlg::Apply(HWND hDlg)
{
	iPasteAsText = IsDlgButtonChecked(hDlg, IDC_CHECK1);
	iPasteTimes = GetDlgItemInt(hDlg, IDC_FPASTE_PASTETIMES, 0, TRUE);
	if (iPasteTimes <= 0)
	{
		MessageBox(hDlg, "Number of times to paste must be at least 1.", "Paste", MB_ICONERROR);
		return FALSE;
	}
	iPasteSkip = GetDlgItemInt(hDlg, IDC_FPASTE_SKIPS, 0, TRUE);
	HWND list = GetDlgItem(hDlg, IDC_FPASTE_CFORMAT);
	int i = SendMessage(list, LB_GETCURSEL, 0, 0);
	if (i == LB_ERR)
	{
		MessageBox(hDlg, "You need to select a clipboard format to use.", "Paste", MB_ICONERROR);
		return FALSE;
	}
	UINT uFormat = SendMessage(list, LB_GETITEMDATA, i, 0);
	char *pcPastestring = 0;
	int destlen = 0;
	BOOL bPasteBinary = FALSE;
	BOOL bPasteUnicode = FALSE;
	if (OpenClipboard(NULL))
	{
		if (HGLOBAL hClipMemory = GetClipboardData(uFormat))
		{
			int gsize = GlobalSize(hClipMemory);
			if (gsize)
			{
				char *pClipMemory = (char *)GlobalLock(hClipMemory);
				pcPastestring = new char[gsize];
				memcpy (pcPastestring, pClipMemory, gsize);
				GlobalUnlock(hClipMemory);
			}
			switch (uFormat)
			{
			case CF_UNICODETEXT:
				bPasteUnicode = TRUE;
			case CF_TEXT:
			case CF_DSPTEXT:
			case CF_OEMTEXT:
				break;
			default:
				destlen = gsize;//CRAP f**king M$ Windoze
				bPasteBinary = TRUE;
				break;
			}
		}
		CloseClipboard();
	}
	if (pcPastestring && !bPasteBinary)
	{
		if (iPasteAsText)
		{
			destlen = bPasteUnicode ? 2 * wcslen((WCHAR *)pcPastestring) : strlen(pcPastestring);
		}
		else
		{
			char *pc = 0;
			destlen = create_bc_translation(&pc, pcPastestring, strlen(pcPastestring));
			delete [] pcPastestring;
			pcPastestring = pc;
		}
	}
	if (destlen == 0)
	{
		MessageBox(hDlg, "Tried to paste zero-length array.", "Paste", MB_ICONERROR);
		delete [] pcPastestring;
		return FALSE;
	}
	WaitCursor wc1;
	if (bSelected || IsDlgButtonChecked(hDlg, IDC_FPASTE_INSERT))
	{
		// Insert at iCurByte. Bytes there will be pushed up.
		if (bSelected)
		{
			iCurByte = iGetStartOfSelection();
			int iEndByte = iGetEndOfSelection();
			DataArray.RemoveAt(iCurByte, iEndByte - iCurByte + 1);//Remove extraneous data
			bSelected = FALSE;//Deselect
		}
		int i = iCurByte;
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			if (!DataArray.InsertAtGrow(i, (unsigned char*)pcPastestring, 0, destlen))
			{
				MessageBox(hDlg, "Not enough memory for inserting.", "Paste", MB_ICONERROR);
				break;
			}
			i += destlen + iPasteSkip;
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		resize_window();
	}
	else
	{
		// Overwrite.
		// Enough space for writing?
		if (DataArray.GetLength() - iCurByte < (iPasteSkip + destlen) * iPasteTimes)
		{
			MessageBox(hDlg, "Not enough space for overwriting.", "Paste", MB_ICONERROR);
			delete [] pcPastestring;
			return TRUE;
		}
		// Overwrite data.
		for (int k = 0 ; k < iPasteTimes ; k++)
		{
			for (int i = 0 ; i < destlen ; i++)
			{
				DataArray[iCurByte + k * (iPasteSkip + destlen) + i] = pcPastestring[i];
			}
		}
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		repaint();
	}
	delete [] pcPastestring;
	return TRUE;
}

BOOL FastPasteDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM)
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
	case IDC_FPASTE_REFRESH:
		{
			//Get all the Clipboard formats
			HWND list = GetDlgItem(hDlg, IDC_FPASTE_CFORMAT);
			SendMessage( list, LB_RESETCONTENT, 0, 0 );
			if (CountClipboardFormats() && OpenClipboard(NULL))
			{
				UINT uFormat;
				int i;
				char szFormatName[100], SetSel = 0;
				LPCSTR lpFormatName;

				uFormat = EnumClipboardFormats(0);
				while (uFormat){
					lpFormatName = NULL;

					// For registered formats, get the registered name.
					if (GetClipboardFormatName(uFormat, szFormatName, sizeof(szFormatName)))
						lpFormatName = szFormatName;
					else{
						//Get the name of the standard clipboard format.
						switch(uFormat){
							#define CASE(a,b) case a: lpFormatName = #a; SetSel = b; break;
								CASE(CF_TEXT,1)
							#undef CASE
							#define CASE(a) case a: lpFormatName = #a; break;
								CASE(CF_BITMAP) CASE(CF_METAFILEPICT) CASE(CF_SYLK)
								CASE(CF_DIF) CASE(CF_TIFF) CASE(CF_OEMTEXT)
								CASE(CF_DIB) CASE(CF_PALETTE) CASE(CF_PENDATA)
								CASE(CF_RIFF) CASE(CF_WAVE) CASE(CF_UNICODETEXT)
								CASE(CF_ENHMETAFILE) CASE(CF_HDROP) CASE(CF_LOCALE)
								CASE(CF_MAX) CASE(CF_OWNERDISPLAY) CASE(CF_DSPTEXT)
								CASE(CF_DSPBITMAP) CASE(CF_DSPMETAFILEPICT)
								CASE(CF_DSPENHMETAFILE) CASE(CF_PRIVATEFIRST)
								CASE(CF_PRIVATELAST) CASE(CF_GDIOBJFIRST)
								CASE(CF_GDIOBJLAST) CASE(CF_DIBV5)
							#undef CASE
							default:
								if(i!=i);
								#define CASE(a) else if(uFormat>a##FIRST&&uFormat<a##LAST) sprintf(szFormatName,#a "%d",uFormat-a##FIRST);
									CASE(CF_PRIVATE) CASE(CF_GDIOBJ)
								#undef CASE
								/*Format ideas for future: hex number, system/msdn constant, registered format, WM_ASKFORMATNAME, tickbox for delay rendered or not*/
								/*else if(uFormat>0xC000&&uFormat<0xFFFF){
									sprintf(szFormatName,"CF_REGISTERED%d",uFormat-0xC000);
								}*/
								else break;
								lpFormatName = szFormatName;
							break;
						}
					}

					if (lpFormatName == NULL){
						sprintf(szFormatName,"0x%.8x",uFormat);
						lpFormatName = szFormatName;
					}

					//Insert into the list
					if(lpFormatName){
						i = SendMessage(list, LB_ADDSTRING, 0, (LPARAM) lpFormatName);
						if(SetSel == 1){SetSel = 2; SendMessage(list, LB_SETCURSEL, i, 0);}
						SendMessage(list, LB_SETITEMDATA, i, uFormat);
					}

					uFormat = EnumClipboardFormats(uFormat);
				}
				CloseClipboard();
				if (!SetSel)
					SendMessage(list, LB_SETCURSEL, 0, 0);
			}
		}
		return TRUE;
	case MAKEWPARAM(IDC_FPASTE_CFORMAT, LBN_SELCHANGE):
		{
			HWND list = GetDlgItem(hDlg, IDC_FPASTE_CFORMAT);
			int i = SendMessage(list, LB_GETCURSEL, 0, 0);
			UINT f = SendMessage(list, LB_GETITEMDATA, i, 0);
			if (f == CF_UNICODETEXT)
				CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
		}
		return TRUE;
	}
	return FALSE;
}

INT_PTR FastPasteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	case WM_CHANGECBCHAIN:
		// If the next window is closing, repair the chain.
		if ((HWND) wParam == hwndNextViewer)
			hwndNextViewer = (HWND) lParam;
		// Otherwise, pass the message to the next link.
		else if (hwndNextViewer != NULL)
			SendMessage(hwndNextViewer, iMsg, wParam, lParam);
		break;
	case WM_DRAWCLIPBOARD:  // clipboard contents changed.
		// Update the window by using Auto clipboard format.
		SendMessage(hDlg, WM_COMMAND, IDC_FPASTE_REFRESH, 0);
		// Pass the message to the next window in clipboard viewer chain.
		SendMessage(hwndNextViewer, iMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		ChangeClipboardChain(hDlg, hwndNextViewer);
		hwndNextViewer = NULL;
		break;
	}
	return FALSE;
}