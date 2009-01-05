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
 * @file  OSTools.cpp
 *
 * @brief Implementation OS tool functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "OSTools.h"

/**
 * @brief Check if current user has Administrator privileges.
 * @note This check applies only for NT-based Windows. For W9X Windows this
 *   function always returns TRUE.
 * @return TRUE if user has Administrator privileges, FALSE otherwise.
 */
BOOL ostools_HaveAdminAccess()
{
	OSVERSIONINFO ver = {0};
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// if this fails, we want to default to being enabled
	if (!(GetVersionEx(&ver) && ver.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		return TRUE;
	}

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	// Initialize SID.
	if (!AllocateAndInitializeSid(&NtAuthority,	2,SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup))
	{
		// Initializing SID Failed.
		return false;
	}
	// Check whether the token is present in admin group.
	BOOL IsInAdminGroup = FALSE;
	if (!CheckTokenMembership(NULL,	AdministratorsGroup, &IsInAdminGroup))
	{
		// Error occurred.
		IsInAdminGroup = FALSE;
	}
	// Free SID and return.
	FreeSid(AdministratorsGroup);
	return IsInAdminGroup;
}
