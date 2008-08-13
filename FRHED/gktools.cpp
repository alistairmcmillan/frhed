#include "precomp.h"
#include <imagehlp.h>
#include <ctype.h>
#include "gktools.h"
#include "resource.h"
#include "simparr.h"
#include "Physicaldrive.h"

extern IPhysicalDrive* Drive;
extern INT64 CurrentSectorNumber;
PartitionInfo* SelectedPartitionInfo = NULL;
static PList PartitionInfoList;

typedef BOOL (__stdcall* LPFNUnMapAndLoad)( PLOADED_IMAGE LoadedImage );
typedef PVOID (__stdcall* LPFNImageRvaToVa)(
	IN PIMAGE_NT_HEADERS NtHeaders,
	IN PVOID Base,
	IN ULONG Rva,
	IN OUT PIMAGE_SECTION_HEADER *LastRvaSection
);

typedef BOOL (__stdcall* LPFNMapAndLoad)(
	PSTR ImageName,
	PSTR DllPath,
	PLOADED_IMAGE LoadedImage,
	BOOL DotDll,
	BOOL ReadOnly
);

LPFNMapAndLoad fMapAndLoad = 0;
LPFNImageRvaToVa fImageRvaToVa = 0;
LPFNUnMapAndLoad fUnMapAndLoad = 0;

BOOL CanUseImagehelpDll()
{
	if( fUnMapAndLoad && fMapAndLoad && fImageRvaToVa )
		return TRUE;

	HMODULE hModule = LoadLibrary( "IMAGEHLP.DLL" );
	if( hModule )
	{
		fMapAndLoad = (LPFNMapAndLoad) GetProcAddress( hModule, "MapAndLoad" );
		fImageRvaToVa = (LPFNImageRvaToVa) GetProcAddress( hModule, "ImageRvaToVa" );
		fUnMapAndLoad = (LPFNUnMapAndLoad) GetProcAddress( hModule, "UnMapAndLoad" );

		return fUnMapAndLoad && fMapAndLoad && fImageRvaToVa;
	}
	return FALSE;
}

#define IRTV(x) fImageRvaToVa( li.FileHeader, li.MappedAddress, (DWORD)x, 0 )

BOOL WINAPI GetDllExportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	if( !CanUseImagehelpDll() )
		return FALSE;

	LOADED_IMAGE li;
	if( !fMapAndLoad( (LPSTR) pszFilename, NULL, &li, TRUE, TRUE ) )
		return FALSE;

	PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)(li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if( !pExpDir )
		return FALSE;

	pExpDir = (PIMAGE_EXPORT_DIRECTORY)IRTV(pExpDir);
	if( !pExpDir->NumberOfNames )
		return FALSE;

	PDWORD* pExpNames = (LPDWORD*) pExpDir->AddressOfNames;
	pExpNames = (LPDWORD*)IRTV(pExpNames);
	ULONG ulStart = (ULONG) IRTV(*pExpNames);
	*lpulOffset = ulStart - (ULONG) li.MappedAddress;
	pExpNames += pExpDir->NumberOfNames-1;
	ULONG ulStop = (ULONG) IRTV(*pExpNames);
	*lpulSize = ulStop - ulStart + strlen((LPCSTR)ulStop);	// hihi

	fUnMapAndLoad( &li );
	return TRUE;
}

// structures are undocumented
typedef struct
{
	// Addr +0 = start of import declaration
	// Addr +1,+2 are -1 always
	// Addr +3 = virtual name-of-dll
	// Addr +4 = ???
	ULONG Addr[5];
} IMPS0;

BOOL WINAPI GetDllImportNames( LPCSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize )
{
	if( !CanUseImagehelpDll() )
		return FALSE;

	LOADED_IMAGE li;
	if( !fMapAndLoad( (LPSTR) pszFilename, NULL, &li, TRUE, TRUE ) )
		return FALSE;

	PVOID pExpDir = (LPVOID)(li.FileHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if( !pExpDir )
		return FALSE;

	IMPS0* p = (IMPS0*)IRTV(pExpDir);
	pExpDir = (PVOID)IRTV(p->Addr[0]);
	pExpDir = (PVOID)IRTV(*(ULONG*)pExpDir);
	*lpulOffset = (ULONG)pExpDir - (ULONG) li.MappedAddress;

	LPBYTE lpbEnd = (LPBYTE) pExpDir;
	while(!( !lpbEnd[0] && !lpbEnd[1] && !lpbEnd[2] && !lpbEnd[3] ))
	{
		lpbEnd++;
	}
	*lpulSize = (ULONG)lpbEnd - (ULONG) pExpDir;

	fUnMapAndLoad( &li );
	return TRUE;
}

void WINAPI XorEncoder( MEMORY_CODING* p )
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q+p->dwSize;
	while(q<qMax)
		*(q++)^=-1;
}

void WINAPI Rot13Encoder( LPMEMORY_CODING p )
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q+p->dwSize;
	while(q<qMax)
		*(q++)=isalpha(*q)?(BYTE)(tolower(*q)<'n'?*q+13:*q-13):*q;
}

MEMORY_CODING_DESCRIPTION BuiltinEncoders[] =
{
	{ "ROT-13", Rot13Encoder },
	{ "XOR -1", XorEncoder },
	{ 0, 0 }
};

typedef struct
{
	HMODULE hLibrary;
	LPFNGetMemoryCodings Callback;
} ENCODE_DLL;

#define MAX_ENCODE_DLL 32

static ENCODE_DLL EncodeDlls[MAX_ENCODE_DLL];

void AddEncoders(HWND hListbox,LPMEMORY_CODING_DESCRIPTION lpEncoders)
{
	for( ULONG ulIndex = 0; lpEncoders[ulIndex].lpszDescription; ulIndex++ )
	{
		SendMessage(hListbox,LB_SETITEMDATA,
			SendMessage(hListbox,LB_ADDSTRING,0,(LPARAM)lpEncoders[ulIndex].lpszDescription),
			(LPARAM)lpEncoders[ulIndex].fpEncodeFunc);
	}
}

static LPMEMORY_CODING theCoding = 0;

static BOOL CALLBACK EncodeDecodeDialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER( lParam );

	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);
			AddEncoders(hListbox,BuiltinEncoders);
			for( ULONG ulIndex = 0; EncodeDlls[ulIndex].Callback; ulIndex++ )
				AddEncoders(hListbox,EncodeDlls[ulIndex].Callback());

			SendMessage(hListbox,LB_SETCURSEL,0,0);
			CheckDlgButton(hDlg,IDC_RADIO1,BST_CHECKED);
		}
		return FALSE;

	case WM_COMMAND:
		if( HIWORD(wParam) == BN_CLICKED )
		{
			if( LOWORD (wParam) == IDOK )
			{
				static CHAR szBuffer[1024];
				GetDlgItemText(hDlg,IDC_EDIT1,szBuffer,sizeof(szBuffer));
				theCoding->bEncode = (IsDlgButtonChecked( hDlg, IDC_RADIO1 ) == BST_CHECKED);
				theCoding->lpszArguments = szBuffer;
				HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);
				int nCurSel = SendMessage(hListbox,LB_GETCURSEL,0,0);
				if( nCurSel>=0)
				{
					theCoding->fpEncodeFunc = (LPFNEncodeMemoryFunction) SendMessage(hListbox,LB_GETITEMDATA,nCurSel,0);
					EndDialog( hDlg, IDOK);
					return TRUE;
				}
			}
			else if( LOWORD (wParam) == IDCANCEL )
			{
				EndDialog(hDlg, IDCANCEL );
				return FALSE;
			}
		}
		break;
	}
	return FALSE;
}

BOOL WINAPI GetMemoryCoding( HINSTANCE hInstance, HWND hParent, LPMEMORY_CODING p, LPCSTR lpszDlls )
{
	theCoding = p;

	static BOOL bDllsLoaded = FALSE;
	if( !bDllsLoaded )
	{
		SimpleString buffer((LPSTR)lpszDlls);
		LPCSTR lpszToken = strtok(buffer,";");
		ULONG ulIndex = 0;
		while( lpszToken )
		{
			if( ulIndex == MAX_ENCODE_DLL )
				break;

			EncodeDlls[ulIndex].hLibrary = LoadLibrary(lpszToken);
			if( EncodeDlls[ulIndex].hLibrary )
			{
				EncodeDlls[ulIndex].Callback = (LPFNGetMemoryCodings) GetProcAddress(EncodeDlls[ulIndex].hLibrary,"GetMemoryCodings");
				if( EncodeDlls[ulIndex].Callback )
				{
					ulIndex++;
				}
				else EncodeDlls[ulIndex].hLibrary = 0;
			}

			lpszToken = strtok(0,";");
		}
	}
	return DialogBox( hInstance, MAKEINTRESOURCE(IDD_ENCODE_DECODE_DIALOG), hParent, (DLGPROC) EncodeDecodeDialogProc ) == IDOK;
}


static BOOL CALLBACK OpenDriveDialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER( lParam );

	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);

			SelectedPartitionInfo = NULL;
			if( PartitionInfoList.IsEmpty() )
			{
				Drive->GetPartitionInfo(&PartitionInfoList);
			}
			ENUMERATE(&PartitionInfoList, PartitionInfo, pi)
			{
				int iIndex = SendMessage(hListbox,LB_ADDSTRING,0,(LPARAM) (LPCSTR) pi->GetNameAsString());
				SendMessage(hListbox,LB_SETITEMDATA,iIndex,(LPARAM)pi);
			}
			SendMessage(hListbox,LB_SETCURSEL,0,0);
		}
		return FALSE;

	case WM_COMMAND:
		if( HIWORD(wParam) == BN_CLICKED )
		{
			if( LOWORD (wParam) == IDOK )
			{
				HWND hListbox = GetDlgItem(hDlg,IDC_LIST1);
				int nCurSel = SendMessage(hListbox,LB_GETCURSEL,0,0);
				if( nCurSel>=0)
				{
					SelectedPartitionInfo = (PartitionInfo*)SendMessage(hListbox,LB_GETITEMDATA,nCurSel,0);
					EndDialog( hDlg, IDOK );
				}
			}
			else if( LOWORD (wParam) == IDCANCEL )
			{
				EndDialog( hDlg, IDCANCEL );
				return FALSE;
			}
		}
		break;
	}
	return FALSE;
}


BOOL WINAPI GetDriveNameDialog( HINSTANCE hInstance, HWND hParent )
{
	return ( DialogBox( hInstance, MAKEINTRESOURCE(IDD_OPEN_DRIVE_DIALOG), hParent, (DLGPROC) OpenDriveDialogProc ) == IDOK );
}

static BOOL CALLBACK GotoTrackDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER( lParam );

	switch (iMsg)
	{
	case WM_INITDIALOG:
		{
			CHAR szTempBuffer[10240];

			sprintf( szTempBuffer, "%I64d", CurrentSectorNumber );
			SetDlgItemText( hDlg, IDC_EDIT1, szTempBuffer );

			DISK_GEOMETRY dg;
			Drive->GetDriveGeometry(&dg);

			INT64 TotalSizeInBytes = dg.SectorsPerTrack;
			TotalSizeInBytes *= dg.BytesPerSector;
			TotalSizeInBytes *= dg.TracksPerCylinder;
			TotalSizeInBytes *= dg.Cylinders.QuadPart;

			sprintf( szTempBuffer,
				"Cylinders = %I64d\r\n"
				"Sectors = %I64d\r\n"
				"TracksPerCylinder = %ld\r\n"
				"SectorsPerTrack = %ld\r\n"
				"BytesPerSector = %ld\r\n"
				"TotalSizeInBytes = %I64d\r\n",
				dg.Cylinders.QuadPart,
				SelectedPartitionInfo->m_NumberOfSectors,
				dg.TracksPerCylinder,
				dg.SectorsPerTrack,
				dg.BytesPerSector,
				TotalSizeInBytes );

			SetDlgItemText(hDlg,IDC_EDIT3,szTempBuffer);

		}
		return FALSE;

	case WM_COMMAND:
		if( HIWORD(wParam) == BN_CLICKED)
		{
			switch (LOWORD (wParam))
			{
			case IDOK:
				{
					CHAR szBuffer[256];
					GetDlgItemText(hDlg,IDC_EDIT1, szBuffer, sizeof(szBuffer) );

					INT64 TempCurrentSectorNumber = 0;
					sscanf( szBuffer, "%I64d", &TempCurrentSectorNumber );
					if( (TempCurrentSectorNumber < SelectedPartitionInfo->m_NumberOfSectors) && (TempCurrentSectorNumber >= 0) )
					{
						CurrentSectorNumber = TempCurrentSectorNumber;
						EndDialog( hDlg, IDOK );
					}
				}
				return TRUE;

			case IDCANCEL:
				EndDialog( hDlg, IDCANCEL );
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

BOOL WINAPI GotoTrackDialog( HINSTANCE hInstance, HWND hParent )
{
	return DialogBox( hInstance, MAKEINTRESOURCE(IDD_GOTO_TRACK_DIALOG), hParent, (DLGPROC) GotoTrackDialogProc ) == IDOK;
}