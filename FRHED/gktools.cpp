#include "precomp.h"
#include "DllProxies.h"
#include <imagehlp.h>
#include <ctype.h>
#include "UnicodeString.h"
#include "resource.h"
#include "hexwnd.h"
#include "gktools.h"
#include "simparr.h"
#include "OSTools.h"
#include "LangString.h"
#include "StringTable.h"

static PList PartitionInfoList;

BOOL WINAPI GetDllExportNames(LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize)
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad((LPSTR)pszFilename, NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
	{
		PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		if (pExpDir)
		{
			DWORD firstName = pExpDir->Name;
			DWORD lastName = pExpDir->Name;
			if (pExpDir->NumberOfNames)
			{
				DWORD *pExpNames = (DWORD *)
					IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pExpDir->AddressOfNames, 0);
				if (pExpNames)
				{
					pExpNames += pExpDir->NumberOfNames - 1;
					lastName = *pExpNames;
				}
			}
			*lpulOffset = firstName;
			TCHAR *name = (TCHAR *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, lastName, 0);
			if (name)
				*lpulSize = lastName + _tcslen(name) + 1 - firstName;
		}
		bDone = TRUE;
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

BOOL WINAPI GetDllImportNames(LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize)
{
	struct IMAGEHLP *IMAGEHLP = ::IMAGEHLP;
	if (IMAGEHLP == 0)
		return FALSE;

	LOADED_IMAGE li;
	if (!IMAGEHLP->MapAndLoad((LPSTR)pszFilename, NULL, &li, TRUE, TRUE))
		return FALSE;

	BOOL bDone = FALSE;
	if (DWORD dw = li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
	{
		IMAGE_IMPORT_DESCRIPTOR *pDescriptor = (IMAGE_IMPORT_DESCRIPTOR *)
			IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, dw, 0);
		DWORD lower = ULONG_MAX;
		DWORD upper = 0;
		while (pDescriptor->FirstThunk)
		{
			if (lower > pDescriptor->Name) //OriginalFirstThunk)
				lower = pDescriptor->Name; //OriginalFirstThunk;
			TCHAR *name = (TCHAR *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->Name, 0);
			DWORD end = 0;
			if (name)
			{
				end = pDescriptor->Name + _tcslen(name) + 1;
				if (upper < end)
					upper = end;
			}
			DWORD *pEntry = (DWORD *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->FirstThunk, 0);
			IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)
				IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, pDescriptor->OriginalFirstThunk, 0);

			if (!name || !pEntry || !pThunk)
			{
				++pDescriptor;
				continue;
			}

			while (pThunk->u1.Function)
			{
				if ((*pEntry & 0x80000000) == 0)
				{
					IMAGE_IMPORT_BY_NAME *pImport = (IMAGE_IMPORT_BY_NAME *)
						IMAGEHLP->ImageRvaToVa(li.FileHeader, li.MappedAddress, *pEntry, 0);
					if (pImport)
					{
						DWORD end = *pEntry + sizeof(IMAGE_IMPORT_BY_NAME) + _tcslen((TCHAR *)pImport->Name);
						if (lower > *pEntry)
							lower = *pEntry;
						if (upper < end)
							upper = end;
					}
				}
				++pThunk;
				++pEntry;
			}
			++pDescriptor;
		}
		if (lower < upper)
		{
			*lpulOffset = lower;
			*lpulSize = upper - lower;
			bDone = TRUE;
		}
	}
	IMAGEHLP->UnMapAndLoad(&li);
	return bDone;
}

void WINAPI XorEncoder(MEMORY_CODING* p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) ^= -1;
}

void WINAPI Rot13Encoder(LPMEMORY_CODING p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) = _istalpha(*q) ? (BYTE)(tolower(*q) < 'n' ? * q + 13 : *q - 13) : *q;
}

MEMORY_CODING_DESCRIPTION BuiltinEncoders[] =
{
	{ _T("ROT-13"), Rot13Encoder },
	{ _T("XOR -1"), XorEncoder },
	{ 0, 0 }
};

void AddEncoders(HWND hListbox, LPMEMORY_CODING_DESCRIPTION lpEncoders)
{
	for ( ; lpEncoders->lpszDescription ; ++lpEncoders)
	{
		int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)lpEncoders->lpszDescription);
		SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)lpEncoders->fpEncodeFunc);
	}
}

INT_PTR EncodeDecodeDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			//SimpleString buffer((LPTSTR)(LPCTSTR)EncodeDlls);
			//LPCSTR lpszToken = strtok(buffer, ";");
			HWND hListbox = GetDlgItem(hDlg, IDC_ENCODE_LIST);
			AddEncoders(hListbox, BuiltinEncoders);

			String buffer(EncodeDlls);
			size_t prev_ind = 0;
			size_t ind = buffer.find_first_of(';');
			while (prev_ind != buffer.npos && ind != buffer.npos)
			{
				String dll = buffer.substr(prev_ind, ind - prev_ind);
				HMODULE hLibrary = GetModuleHandle(dll.c_str());
				if (hLibrary == 0)
					hLibrary = LoadLibrary(dll.c_str());
				if (hLibrary)
				{
					if (LPFNGetMemoryCodings callback = (LPFNGetMemoryCodings)
						GetProcAddress(hLibrary, _T("GetMemoryCodings")))
					{
						AddEncoders(hListbox, callback());
					}
				}
				//lpszToken = strtok(0, ";");
				prev_ind = ind;
				ind = buffer.find_first_of(';', prev_ind + 1);
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
			CheckDlgButton(hDlg, IDC_ENCODE_ENC, BST_CHECKED);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				MEMORY_CODING mc;
				TCHAR szBuffer[1024];
				GetDlgItemText(hDlg, IDC_ENCODE_ARGS, szBuffer, sizeof(szBuffer));
				mc.bEncode = IsDlgButtonChecked(hDlg, IDC_ENCODE_ENC);
				mc.lpszArguments = szBuffer;
				HWND hListbox = GetDlgItem(hDlg, IDC_ENCODE_LIST);
				int nCurSel = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				if (nCurSel < 0)
					return TRUE;
				mc.fpEncodeFunc = (LPFNEncodeMemoryFunction) SendMessage(hListbox, LB_GETITEMDATA, nCurSel, 0);
				int lower = 0;
				int upper = DataArray.GetUpperBound();
				if (bSelected)
				{
					lower = iGetStartOfSelection();
					upper = iGetEndOfSelection();
				}
				mc.lpbMemory = &DataArray[lower];
				mc.dwSize = upper - lower + 1;
				mc.fpEncodeFunc(&mc);
				iFileChanged = TRUE;
				bFilestatusChanged = true;
				repaint();
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR OpenDriveDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			BOOL admin = ostools_HaveAdminAccess();
			if (!admin)
			{
				LangString app(IDS_APPNAME);
				LangString needAdmin(IDS_DRIVES_NEED_ADMIN);
				MessageBox(hDlg, needAdmin, app, MB_OK | MB_ICONSTOP);
				return TRUE;
			}
			ShowWindow(hDlg, SW_SHOW);
			if (PartitionInfoList.IsEmpty())
			{
				if (IPhysicalDrive *Drive = CreatePhysicalDriveInstance())
				{
					Drive->GetPartitionInfo(&PartitionInfoList);
					delete Drive;
				}
			}
			HWND hListbox = GetDlgItem(hDlg, IDC_DRIVE_LIST);
			LIST_ENTRY *Flink = &PartitionInfoList;
			while ((Flink = Flink->Flink) != &PartitionInfoList)
			{
				PartitionInfo *pi = static_cast<PartitionInfo *>(Flink);
				int i = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)pi->GetNameAsString());
				SendMessage(hListbox, LB_SETITEMDATA, i, (LPARAM)pi);
			}
			SendMessage(hListbox, LB_SETCURSEL, 0, 0);
		}
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			{
				HWND hListbox = GetDlgItem(hDlg, IDC_DRIVE_LIST);
				int nCurSel = SendMessage(hListbox, LB_GETCURSEL, 0, 0);
				if (nCurSel < 0)
					return TRUE;
				PartitionInfo *SelectedPartitionInfo = (PartitionInfo *)SendMessage(hListbox, LB_GETITEMDATA, nCurSel, 0);

				IPhysicalDrive *Drive = CreatePhysicalDriveInstance();
				if (Drive == 0 || !Drive->Open(SelectedPartitionInfo->m_dwDrive))
				{
					LangString app(IDS_APPNAME);
					LangString err(IDS_DRIVES_ERR_OPEN);
					MessageBox(hwnd, err, app, MB_ICONERROR);
					delete Drive;
					return TRUE;
				}

				delete HexEditorWindow::Drive;
				HexEditorWindow::Drive = Drive;
				HexEditorWindow::SelectedPartitionInfo = SelectedPartitionInfo;
				EnableDriveButtons(TRUE);
				if (Track.GetObjectSize() != SelectedPartitionInfo->m_dwBytesPerSector)
					Track.Create(SelectedPartitionInfo->m_dwBytesPerSector);
				CurrentSectorNumber = 0;
				RefreshCurrentTrack();
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

INT_PTR GotoTrackDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			TCHAR szTempBuffer[10240];

			_stprintf(szTempBuffer, _T("%I64d"), CurrentSectorNumber);
			SetDlgItemText(hDlg, IDC_DRIVE_TRACK, szTempBuffer);

			DISK_GEOMETRY dg;
			Drive->GetDriveGeometry(&dg);

			INT64 TotalSizeInBytes = dg.SectorsPerTrack;
			TotalSizeInBytes *= dg.BytesPerSector;
			TotalSizeInBytes *= dg.TracksPerCylinder;
			TotalSizeInBytes *= dg.Cylinders.QuadPart;

			_stprintf(szTempBuffer,
				_T("%s = %I64d\r\n")
				_T("%s = %I64d\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %ld\r\n")
				_T("%s = %I64d\r\n"),
				GetLangString(IDS_DRIVES_CYLINDERS),
				dg.Cylinders.QuadPart,
				GetLangString(IDS_DRIVES_SECTORS),
				SelectedPartitionInfo->m_NumberOfSectors,
				GetLangString(IDS_DRIVES_TRACSPERCYL),
				dg.TracksPerCylinder,
				GetLangString(IDS_DRIVES_SECTPERTRACK),
				dg.SectorsPerTrack,
				GetLangString(IDS_DRIVES_BYTESPERSECT),
				dg.BytesPerSector,
				GetLangString(IDS_DRIVES_TOTALBYTES),
				TotalSizeInBytes);

			SetDlgItemText(hDlg, IDC_DRIVE_INFO, szTempBuffer);
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
			{
				TCHAR szBuffer[256];
				GetDlgItemText(hDlg, IDC_DRIVE_TRACK, szBuffer, RTL_NUMBER_OF(szBuffer));

				INT64 TempCurrentSectorNumber = 0;
				_stscanf(szBuffer, _T("%I64d"), &TempCurrentSectorNumber);
				if (TempCurrentSectorNumber < 0 ||
					TempCurrentSectorNumber >= SelectedPartitionInfo->m_NumberOfSectors)
				{
					return TRUE;
				}
				CurrentSectorNumber = TempCurrentSectorNumber;
				RefreshCurrentTrack();
			}
			// fall through
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
