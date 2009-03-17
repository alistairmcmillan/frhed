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
 * @file  hexwdlg.h
 *
 * @brief Dialog class declarations.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _HEXW_DLG_H_
#define _HEXW_DLG_H_

#include "Simparr.h"
#include "Resource.h"

void GetWindowText(HWND, SimpleString &);
void GetDlgItemText(HWND, int, SimpleString &);
BOOL EnableDlgItem(HWND, int, BOOL);
BOOL IsDlgItemEnabled(HWND, int);

/**
 * @brief About-dialog for Frhed application.
 * This dialog shows program name, version and contributor info.
 */
class AboutDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_ABOUTDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
protected:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

class DragDropOptionsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_DRAG_DROP_OPTIONS };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * @brief View settings dialog class.
 */
class ViewSettingsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_VIEWSETTINGSDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	static HWND hCbLang;
	static BOOL CALLBACK EnumLocalesProc(LPTSTR);
	static int FormatLangId(LPWSTR, LANGID, int verbose = 0);
	void OnDrawitemLangId(DRAWITEMSTRUCT *);
	INT_PTR OnCompareitemLangId(COMPAREITEMSTRUCT *);
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
	void SelectEditor(HWND hDlg);
};

class CharacterSetDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHARACTERSETDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class BinaryModeDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_BINARYMODEDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class FillWithDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_FILL_WITH };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	enum { FW_MAX = 1024 }; //max bytes to fill with
	static char pcFWText[FW_MAX];//hex representation of bytes to fill with
	static char buf[FW_MAX];//bytes to fill with
	static int buflen;//number of bytes to fill with
	static char szFWFileName[_MAX_PATH];//fill with file name
	static int FWFile,FWFilelen;//fill with file and len
	static LONG_PTR oldproc;//old hex box proc
	static HFONT hfon;//needed so possible to display infinity char in fill with dlg box
	static char curtyp;//filling with input-0 or file-1
	static char asstyp;
	void inittxt(HWND);
	static unsigned char input(int);
	static unsigned char file(int);
	static void hexstring2charstring();
	static void deletenonhex(HWND);
	static LRESULT CALLBACK HexProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * A dialog for reversing bytes in the selected area.
 */
class ReverseDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REVERSE_BYTES_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class ShortcutsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_SHORTCUTS };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	BOOL OnNotify(HWND, WPARAM, LPARAM);
	static int CALLBACK BrowseCallbackProc(HWND, UINT, LPARAM, LPARAM);
	static int CALLBACK SearchCallbackProc(HWND, UINT, LPARAM, LPARAM);
};

class ChangeInstDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHANGEINST };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

class UpgradeDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_UPGRADE };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	BOOL OnNotify(HWND, WPARAM, LPARAM);
	BOOL OnDrawitem(HWND, WPARAM, LPARAM);
	void ChangeSelVer(HWND hw, char* text);
	void ChangeSelInst(HWND hw, char* text);
};

class FindDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_FINDDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static int iFindDlgMatchCase;
	static int iFindDlgDirection;
	static int iFindDlgUnicode;
	static int iFindDlgBufLen;
	static char *pcFindDlgBuffer;
};

class ReplaceDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REPLACEDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	int transl_text_to_binary(SimpleArray<char> &out);
	static int transl_binary_to_text(char *, int);
	int find_and_select_data(int finddir, char (*cmp)(char));
	int replace_selected_data(HWND);
	void find_directed(HWND, int finddir, LPCTSTR title);
	void replace_directed(HWND, int finddir, LPCTSTR title);
	static SimpleString strToReplaceData;
	static SimpleString strReplaceWithData;
};

class ChooseDiffDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHOOSEDIFFDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	void add_diff(HWND hwndList, int diff, int lower, int upper);
	int get_diffs(HWND hwndList, char *ps, int sl, char *pd, int dl);
};

class CopyHexdumpDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_HEXDUMPDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static int iCopyHexdumpMode;
	static int iCopyHexdumpType;
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

class AddBmkDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_ADDBMK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

class RemoveBmkDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REMOVEBMK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

/**
 * @brief A dialog for selecting a block of bytes.
 */
class SelectBlockDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_SELECT_BLOCK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

/**
 * @brief A dialog for pasting bytes to editor.
 * This version of Paste-dialog shows the data in the clipboard and allows
 * the user to edit data before pasting it.
 */
class PasteDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_PASTEDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	BOOL Apply(HWND);
};

/**
 * @brief A dialog for normal pasting of bytes to editor.
 * This is the default paste-dialog (opened e.g. from Ctrl+V).
 */
class FastPasteDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_FASTPASTE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	BOOL Apply(HWND);
	void RefreshClipboardFormats(HWND);
};

class MoveCopyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_MOVE_COPY };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
	BOOL Apply(HWND);
};

class EnterDecimalValueDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_DECIMALDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static int iDecValDlgSize;
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

/**
 * @brief A Goto-dialog.
 * This dialog allows user to jump to certain position in the file. The jump
 * can be absolute or relative. An absolute jump goes to the position counted
 * from the begin of the file. The relative jump goes given amount backwards
 * or forward in the file.
 */
class GoToDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTODIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static char buffer[];  /**< Buffer for offset value. */
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

/**
 * @brief A dialog for Cut- and Delete-features.
 * This dialog handles both Cut- and Delete-features. The dialog shows
 * selection (if any) start- and end-offesets. Also the selection
 * (if any) length in bytes is shown. User can edit these values before
 * the operation.
 */
class CutDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CUTDIALOG };

	/** @brief Cut modes. */
	enum CUT_MODE
	{
		CUT_CUT, /**< Cut text to clipboard. */
		CUT_DELETE, /**< Delete text without copying to clipboard. */
	};

	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static CUT_MODE iCutMode; /**< Selected cut-mode. */
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

/**
 * A dialog for copying the bytes.
 */
class CopyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_COPYDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

class AppendDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_APPENDDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

class BitManipDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_MANIPBITSDIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND, WPARAM);
};

/**
 * @brief Dialog for opening file partially.
 * This dialog allows user to select range of bytes in file to open.
 * Only the selected range is opened into the hex editor.
 */
class OpenPartiallyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_PARTIAL_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	static int filehandle;
	static bool bShowFileStatsPL;
private:
	BOOL OnInitDialog(HWND);
	BOOL Apply(HWND);
};

#endif // _HEXW_DLG_H_
