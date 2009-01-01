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
 * @file  Registry.cpp
 *
 * @brief Implementation of Frhed registry functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "version.h"
#include "Constants.h"
#include "Registry.h"
#include "regtools.h"

BOOL contextpresent()
{
	HKEY key1;
	LONG res = RegOpenKeyEx(HKEY_CLASSES_ROOT, "*\\shell\\Open in Frhed\\command", 0, KEY_ALL_ACCESS, &key1);
	if (res == ERROR_SUCCESS) //succeeded check if has the required keys & data
	{
		char stringval[ _MAX_PATH ];
		char exepath[ _MAX_PATH ];
		long len = 0;//dummy
		strcpy(exepath, _pgmptr);
		strcat(exepath," %1");
		RegQueryValue(key1, NULL, stringval, &len);
		RegCloseKey(key1);
		if (strcmp(stringval, exepath))
			return 1;
	}
	return 0;
}

BOOL defaultpresent()
{
	char stringval[_MAX_PATH];
	long len = _MAX_PATH;
	LONG res = RegQueryValue(HKEY_CLASSES_ROOT, "Unknown\\shell", stringval, &len);
	return res == ERROR_SUCCESS && strcmp(stringval, "Open in Frhed") == 0;
}

BOOL unknownpresent()
{
	HKEY key1;
	LONG res;
	res = RegOpenKeyEx( HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in Frhed\\command", 0, KEY_ALL_ACCESS, &key1 );
	if( res == ERROR_SUCCESS ){//succeeded check if has the required keys & data
		char stringval[ _MAX_PATH ];
		char exepath[ _MAX_PATH ];
		long len = 0;//dummy
		strcpy( exepath, _pgmptr );
		strcat(exepath," %1");
		RegQueryValue(key1,NULL,stringval,&len);
		RegCloseKey(key1);
		if(strcmp(stringval, exepath))
			return 1;
	}
	return 0;
}

BOOL oldpresent()
{
	HKEY hk;
	LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, OptionsRegistryPath, 0,
			KEY_ALL_ACCESS, &hk);
	if (res == ERROR_SUCCESS)
	{
		TCHAR subkeynam[MAX_PATH] = {0};
		for (DWORD i = 0; ; i++)
		{
			res = RegEnumKey(hk ,i, subkeynam, MAX_PATH);
			if (res == ERROR_NO_MORE_ITEMS)
				break;
			else if (0 != strcmp(subkeynam, OptionsRegistrySettingsPath))
			{
				RegCloseKey(hk);
				return TRUE;
			}
		}
		RegCloseKey(hk);
	}
	return FALSE;
}

/**
 * @brief Check if Frhed settings key already exist.
 */
BOOL frhedpresent()
{
	//Check if frhed\subreleaseno exists
	HKEY hk;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, OptionsRegistrySettingsPath, &hk))
		return FALSE;
	RegCloseKey(hk);
	return TRUE;
}

/**
 * @brief Check if links key in Frhed settings already exist.
 */
BOOL linkspresent()
{
	HKEY hk;
	TCHAR keyname[64] = {0};
	_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
			OptionsRegistrySettingsPath);
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, keyname, &hk))
		return FALSE;
	RegCloseKey(hk);
	return TRUE;
}

BOOL registry_RemoveFrhed(HWND hwnd)
{
	BOOL bRemoveRegistryEntries = FALSE;
	int res, r, r0;
	r = r0 = 0; // r&r0 used to determine if the user has removed all frhed data
	res = MessageBox(hwnd, _T("Are you sure you want to remove Frhed?"), _T("Remove Frhed"), MB_YESNO);
	if (res != IDYES)
		return FALSE;
	//Can assume registry data exists
	res = linkspresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, _T("Remove known links?"), _T("Remove Frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			//Remove known links & registry entries of those links
			HKEY hk;
			TCHAR valnam[MAX_PATH];
			DWORD valnamsize, typ;
			TCHAR valbuf[MAX_PATH];
			DWORD valbufsize, ret;
			
			TCHAR keyname[64] = {0};
			_sntprintf(keyname, RTL_NUMBER_OF(keyname), _T("%s\\links"),
					OptionsRegistrySettingsPath);
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0,
					KEY_ALL_ACCESS, &hk))
			{
				for (DWORD i = 0; ; i++)
				{
					typ = 0;
					valnamsize = sizeof valnam;
					valbufsize = sizeof valbuf;
					valbuf[0] = valnam[0] = 0;
					ret = RegEnumValue(hk, i, valnam, &valnamsize, 0, &typ, (BYTE*) valbuf,
								&valbufsize);
					if (typ == REG_SZ && valbuf[0] != 0 && PathIsDirectory(valbuf))
					{
						PathAppend(valbuf, _T("Frhed.lnk"));
						_tremove(valbuf);
					}
					if (ERROR_NO_MORE_ITEMS == ret)
						break;
				}
				RegCloseKey(hk);
			}
			RegDeleteKey(HKEY_CURRENT_USER, keyname);
		}
	}
	res = contextpresent() || unknownpresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, _T("Remove 'Open in Frhed' command(s) ?"), _T("Remove Frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			//Remove 'Open in frhed' command registry entries
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in Frhed\\command")); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in Frhed"));
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in Frhed\\command")); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in Frhed"));
			TCHAR stringval[MAX_PATH];
			long len = _MAX_PATH;
			RegQueryValue(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), stringval, &len);
			if (!_tcscmp(stringval, _T("Open in Frhed")))
			{
				HKEY hk;
				if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), &hk))
				{
					RegDeleteValue(hk, NULL);
					RegCloseKey(hk);
				}
			}
		}
	}
	HKEY tmp;
	res = RegOpenKey(HKEY_CURRENT_USER, OptionsRegistrySettingsPath, &tmp);
	if (res == ERROR_SUCCESS)
	{
		RegCloseKey(tmp);
		r0++;
		res = MessageBox(hwnd, _T("Remove registry entries?"), _T("Remove Frhed"), MB_YESNO);
		if (res == IDYES)
		{
			r++;
			bRemoveRegistryEntries = TRUE;//Don't save ini data when the user quits (and hope other instances are not running now (they will write new data)
			OSVERSIONINFO ver;
			ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&ver);
			if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
				RegDeleteWinNTKey(HKEY_CURRENT_USER, OptionsRegistrySettingsPath);
			else
				RegDeleteKey(HKEY_CURRENT_USER, OptionsRegistrySettingsPath);
			res = oldpresent();
			if (res)
			{
				res = MessageBox(hwnd, _T("Registry entries from previous versions of Frhed were found\n")
					_T("Should they all be removed?"), _T("Remove Frhed"), MB_YESNO);
				if (res == IDYES)
				{
					if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
						RegDeleteWinNTKey(HKEY_CURRENT_USER, OptionsRegistryPath);
					else
						RegDeleteKey(HKEY_CURRENT_USER, OptionsRegistryPath);
				}
			}
		}
	}
#if 0
	for(;;i++){
		RegEnumKey (HKEY_USERS,i,buf,MAX_PATH+1);
		if(res==ERROR_NO_MORE_ITEMS)break;
		if(!stricmp(buf,".Default")){
			if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_USERS,buf,0,KEY_ALL_ACCESS,&hk)){
				removefrhedfromuser(hk);
				RegCloseKey(hk);
			}
		}
	}
#endif // 0
	if (r == r0)
	{
		MessageBox(hwnd,
			_T("Now all that remains to remove Frhed from this computer is to:\n")
			_T("1. Quit all other instances of Frhed(after turning off \"Save ini...\" in each one)\n")
			_T("2. Quit this instance of Frhed\n")
			_T("3. Check that the registry data was removed (just in case)\n")
			_T("4. Delete the directory where Frhed currently resides\n")
			_T("5. If you have an email account please take the time to\n")
			_T("    email the author/s and give the reason/s why you have\n")
			_T("    removed Frhed from your computer"),
			_T("Remove Frhed"), MB_OK);
	}
	return bRemoveRegistryEntries;
}
