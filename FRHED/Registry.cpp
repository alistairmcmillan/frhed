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
#include "Registry.h"
#include "regtools.h"

BOOL contextpresent()
{
	HKEY key1;
	LONG res = RegOpenKeyEx( HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed\\command", 0, KEY_ALL_ACCESS, &key1 );
	if (res == ERROR_SUCCESS) //succeeded check if has the required keys & data
	{
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

BOOL linkspresent()
{
	//Check if frhed\subreleaseno\links exists
	HKEY hk;
	if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3) "\\links",&hk)){
		RegCloseKey(hk);
		return TRUE;
	}

	return FALSE;
}

BOOL defaultpresent()
{
	char stringval[_MAX_PATH];
	long len = _MAX_PATH;
	LONG res = RegQueryValue(HKEY_CLASSES_ROOT, "Unknown\\shell", stringval, &len);
	return res == ERROR_SUCCESS && strcmp(stringval, "Open in frhed") == 0;
}

BOOL unknownpresent()
{
	HKEY key1;
	LONG res;
	res = RegOpenKeyEx( HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed\\command", 0, KEY_ALL_ACCESS, &key1 );
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
	char keyname[] = "Software\\frhed";
	char subkeynam[MAX_PATH + 1] = "";
	LONG res = RegOpenKeyEx( HKEY_CURRENT_USER,keyname,0,KEY_ALL_ACCESS,&hk );
	if( res == ERROR_SUCCESS ){
		for(DWORD i = 0;; i++ ){
			res = RegEnumKey(hk,i,subkeynam,MAX_PATH + 1);
			if(res==ERROR_NO_MORE_ITEMS)
				break;
			else if(0!=strcmp(subkeynam,"v"SHARPEN(FRHED_VERSION_3))){
				RegCloseKey(hk);
				return TRUE;
			}
		}
		RegCloseKey(hk);
	}
	return FALSE;
}

BOOL frhedpresent()
{
	//Check if frhed\subreleaseno exists
	HKEY hk;
	if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3) ,&hk)){
		RegCloseKey(hk);
		return TRUE;
	}

	return FALSE;
}

void registry_RemoveFrhed(HWND hwnd, bool saveIni)
{
	int res, r, r0;
	r = r0 = 0; //r&r0 used to determine if the user has removed all frhed data

	res = MessageBox(hwnd, "Are you sure you want to remove frhed ?",
			"Remove frhed", MB_YESNO);
	if (res != IDYES)
		return;

	//Can assume registry data exists
	res = linkspresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, "Remove known links ?", "Remove frhed",
				MB_YESNO);
		if (res == IDYES)
		{
			r++;
			//Remove known links & registry entries of those links
			HKEY hk;
			char valnam[_MAX_PATH + 1] = {0};
			DWORD valnamsize = _MAX_PATH + 1, typ;
			char valbuf[_MAX_PATH + 1] = {0};
			DWORD valbufsize = _MAX_PATH + 1, ret;
			if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,
					"Software\\frhed\\v"SHARPEN(FRHED_VERSION_3) "\\links",
					0, KEY_ALL_ACCESS, &hk))
			{
				for (DWORD i = 0; ; i++)
				{
					typ = 0;
					valnamsize = valbufsize = _MAX_PATH + 1;
					valbuf[0] = valnam[0] = 0;
					ret = RegEnumValue(hk, i, valnam, &valnamsize, 0,
							&typ, (BYTE*) valbuf, &valbufsize);
					if (typ == REG_SZ && valbuf[0] != 0 &&
							PathIsDirectory(valbuf))
					{
						PathAddBackslash(valbuf);
						strcat(valbuf, "frhed.lnk");
						remove(valbuf);
					}
					if (ERROR_NO_MORE_ITEMS == ret)
						break;
				}
				RegCloseKey(hk);
			}
			RegDeleteKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3) "\\links");
		}
	}
	res = contextpresent() || unknownpresent();
	if (res)
	{
		r0++;
		res = MessageBox(hwnd, "Remove 'Open in frhed' command(s) ?",
				"Remove frhed", MB_YESNO);
		if(res==IDYES)
		{
			r++;
			//Remove 'Open in frhed' command registry entries
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed\\command"); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed");
			RegDeleteKey(HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed\\command"); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed");
			char stringval[_MAX_PATH]= {0};
			long len = _MAX_PATH;
			RegQueryValue(HKEY_CLASSES_ROOT, "Unknown\\shell", stringval,&len);
			if (!strcmp(stringval, "Open in frhed"))
			{
				HKEY hk;
				if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT,
						"Unknown\\shell", &hk))
				{
					RegDeleteValue(hk, NULL);
					RegCloseKey(hk);
				}
			}
		}
	}
	HKEY tmp;
	res = RegOpenKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3), &tmp);
	if (res == ERROR_SUCCESS)
	{
		RegCloseKey(tmp);
		r0++;
		res = MessageBox(hwnd, "Remove registry entries?", "Remove frhed",
				MB_YESNO);
		if (res == IDYES)
		{
			r++;
			saveIni = FALSE;//Don't save ini data when the user quits (and hope other instances are not running now (they will write new data)
			OSVERSIONINFO ver;
			ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&ver);
			if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
				RegDeleteWinNTKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3));
			else
				RegDeleteKey(HKEY_CURRENT_USER, "Software\\frhed\\v"SHARPEN(FRHED_VERSION_3));
			res = oldpresent();
			if (res)
			{
				res = MessageBox(hwnd, 
						"Registry entries from previous versions of frhed were found\n"
						"Should they all be removed ?", "Remove frhed", MB_YESNO);
				if (res == IDYES)
				{
					if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
						RegDeleteWinNTKey(HKEY_CURRENT_USER, "Software\\frhed");
					else
						RegDeleteKey(HKEY_CURRENT_USER, "Software\\frhed");
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
			"Now all that remains to remove frhed from this computer is to:\n"
			"1. Quit all other instances of frhed(after turning off \"Save ini...\" in each one)\n"
			"2. Quit this instance of frhed\n"
			"3. Check that the registry data was removed (just in case)\n"
			"4. Delete the directory where frhed currently resides\n"
			"5. If you have an email account please take the time to\n"
			"    email the author/s and give the reason/s why you have\n"
			"    removed frhed from your computer",
			"Remove frhed", MB_OK);
	}
}