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
