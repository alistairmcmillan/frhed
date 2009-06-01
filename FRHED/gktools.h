#ifndef gktools_h
#define gktools_h

#include "EncoderLib.h"

BOOL WINAPI GetDllExportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);
BOOL WINAPI GetDllImportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);

extern PartitionInfo* SelectedPartitionInfo;

BOOL WINAPI GetMemoryCoding(HINSTANCE hInstance, HWND hParent, LPMEMORY_CODING p, LPCTSTR lpszDlls);

/**
 * @brief A dialog for selecting encoder DLL.
 */
class EncodeDecodeDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_ENCODE_DECODE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * @brief A dialog for opening disk drives.
 */
class OpenDriveDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_DRIVE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * @brief A dialog for moving in disk.
 */
class GotoTrackDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTO_TRACK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};


#endif // gktools_h
