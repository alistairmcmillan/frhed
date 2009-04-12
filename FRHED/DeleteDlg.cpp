#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "LangString.h"

/** Length of the offset buffer. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE
 */
BOOL DeleteDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	char buf[OffsetLen + 1] = {0};

	sprintf(buf, "x%x", iStart);
	SetDlgItemText(hDlg, IDC_DELETE_STARTOFFSET, buf);
	sprintf(buf, "x%x", iEnd);

	CheckDlgButton(hDlg, IDC_DELETE_INCLUDEOFFSET, BST_CHECKED);
	SetDlgItemText(hDlg, IDC_DELETE_ENDOFFSET, buf);
	SetDlgItemInt(hDlg, IDC_DELETE_NUMBYTES, iEnd - iStart + 1, TRUE);

	return TRUE;
}

/**
 * @brief Delete the data.
 * This function deletes the data based on values user entered to the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if the deleting succeeded, FALSE otherwise.
 */
BOOL DeleteDlg::Apply(HWND hDlg)
{
	char buf[OffsetLen + 1] = {0};
	int iOffset;
	int iNumberOfBytes;

	if (GetDlgItemText(hDlg, IDC_DELETE_STARTOFFSET, buf, OffsetLen) &&
		sscanf(buf, "x%x", &iOffset) == 0 &&
		sscanf(buf, "%d", &iOffset) == 0)
	{
		LangString app(IDS_APPNAME);
		LangString startOffsetErr(IDS_OFFSET_START_ERROR);
		MessageBox(hDlg, startOffsetErr, app, MB_ICONERROR);
		return FALSE;
	}

	if (IsDlgButtonChecked(hDlg, IDC_DELETE_INCLUDEOFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_DELETE_ENDOFFSET, buf, OffsetLen) &&
			sscanf(buf, "x%x", &iNumberOfBytes) == 0 &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			LangString app(IDS_APPNAME);
			LangString endOffsetErr(IDS_OFFSET_END_ERROR);
			MessageBox(hDlg, endOffsetErr, app, MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_DELETE_NUMBYTES, buf, OffsetLen) &&
			sscanf(buf, "%d", &iNumberOfBytes) == 0)
		{
			LangString notRecognized(IDS_BYTES_NOT_KNOWN);
			MessageBox(hDlg, notRecognized, MB_ICONERROR);
			return FALSE;
		}
	}

	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Can't delete more bytes than are present.", app, MB_ICONERROR);
		return FALSE;
	}

	// Delete data.
	if (!DataArray.RemoveAt(iOffset, iNumberOfBytes))
	{
		LangString app(IDS_APPNAME);
		MessageBox(hDlg, "Could not delete data.", app, MB_ICONERROR);
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

/**
 * @brief Process messages.
 * @param [in] hDlg Handle to dialog.
 * @param [in] iMsg Message ID.
 * @param [in] wParam First message parameter (depends on message).
 * @param [in] lParam Second message parameter (depends on message).
 * @return TRUE if message was handled, FALSE if ignored.
 */
INT_PTR DeleteDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		
		// Check for controls
		switch (LOWORD(wParam))
		{
		case IDC_DELETE_INCLUDEOFFSET:  
		case IDC_DELETE_NUMBEROFBYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_DELETE_NUMBYTES),
						IsDlgButtonChecked(hDlg, IDC_DELETE_NUMBEROFBYTES));
			EnableWindow(GetDlgItem(hDlg, IDC_DELETE_ENDOFFSET),
						IsDlgButtonChecked(hDlg, IDC_DELETE_INCLUDEOFFSET));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(hDlg);
		break;
	}
	return FALSE;
}
