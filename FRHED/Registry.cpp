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
#include "shtools.h"
#include "regtools.h"

/**
 * @brief Check if Frhed settings key already exist.
 */
BOOL frhedpresent()
{
	//Check if frhed\subreleaseno exists
	HKEY hk;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, OptionsRegistrySettingsPath, 0, KEY_READ, &hk))
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
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_READ, &hk))
		return FALSE;
	RegCloseKey(hk);
	return TRUE;
}
