#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

/** Length of the offset buffer. */
static const int OffsetLen = 16;

int CutDlg::iCutMode = 1;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE
 */
BOOL CutDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	char buf[OffsetLen + 1] = {0};

	sprintf(buf, "x%x", iStart);
	SetDlgItemText(hDlg, IDC_CUT_STARTOFFSET, buf);
	sprintf(buf, "x%x", iEnd);
	SetDlgItemText(hDlg, IDC_CUT_NUMBYTES, buf);
	SetDlgItemInt(hDlg, IDC_CUT_NUMBYTES2, iEnd - iStart + 1, TRUE);
	CheckDlgButton(hDlg, IDC_CUT_CLIPBOARD, iCutMode);
	CheckDlgButton(hDlg, IDC_CUT_INCLUDEOFFSET, BST_CHECKED);
	return TRUE;
}

/**
 * @brief Cut the data.
 * This function cuts the data based on values user entered to the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if the cutting succeeded, FALSE otherwise.
 */
BOOL CutDlg::Apply(HWND hDlg)
{
	char buf[OffsetLen + 1] = {0};
	int iOffset;
	int iNumberOfBytes;

	if (GetDlgItemText(hDlg, IDC_CUT_STARTOFFSET, buf, OffsetLen) &&
		sscanf(buf, "x%x", &iOffset) == 0 &&
		sscanf(buf, "%d", &iOffset) == 0)
	{
		MessageBox(hDlg, "Start offset not recognized.", "Cut", MB_ICONERROR);
		return FALSE;
	}
	if (IsDlgButtonChecked(hDlg, IDC_CUT_INCLUDEOFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_CUT_NUMBYTES, buf, OffsetLen) &&
			sscanf(buf, "x%x", &iNumberOfBytes) == 0 &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, "End offset not recognized.", "Cut", MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_CUT_NUMBYTES2, buf, OffsetLen) &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, "Number of bytes not recognized.", "Cut", MB_ICONERROR);
			return FALSE;
		}
	}
	iCutMode = IsDlgButtonChecked(hDlg, IDC_CUT_CLIPBOARD);
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		MessageBox(hDlg, "Can't cut more bytes than are present.", "Cut", MB_ICONERROR);
		return FALSE;
	}
	// OK
	//int newlen = DataArray.GetLength () - iCutNumberOfBytes;
	// Cut to clipboard?
	if (iCutMode)
	{
		// Transfer to cipboard.
		int destlen = Text2BinTranslator::iBytes2BytecodeDestLen((char*) &DataArray[iOffset], iNumberOfBytes);
		HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
		if (hGlobal == 0)
		{
			// Not enough memory for clipboard.
			MessageBox(hDlg, "Not enough memory for cutting to clipboard.", "Cut", MB_ICONERROR);
			return FALSE;
		}
		WaitCursor wc;
		char *pd = (char *)GlobalLock(hGlobal);
		Text2BinTranslator::iTranslateBytesToBC(pd, &DataArray[iOffset], iNumberOfBytes);
		GlobalUnlock(hGlobal);
		OpenClipboard(hwnd);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hGlobal);
		CloseClipboard();
	}
	// Cut data.
	if (!DataArray.RemoveAt(iOffset, iNumberOfBytes))
	{
		MessageBox(hDlg, "Could not cut data.", "Cut", MB_ICONERROR);
		return FALSE;
	}
	iCurByte = iOffset;
	if (iCurByte > DataArray.GetUpperBound())
		iCurByte = DataArray.GetUpperBound();
	if (iCurByte < 0)
		iCurByte = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	bSelected = FALSE;
	resize_window();
	return TRUE;
}

INT_PTR CutDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(hDlg))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		case IDC_CUT_INCLUDEOFFSET:
		case IDC_CUT_NUMBYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_CUT_NUMBYTES), IsDlgButtonChecked(hDlg, IDC_CUT_INCLUDEOFFSET));
			EnableWindow(GetDlgItem(hDlg, IDC_CUT_NUMBYTES2), IsDlgButtonChecked(hDlg, IDC_CUT_NUMBYTES));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
