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
	// make sure this is NT first
	OSVERSIONINFO ver = { 0 };
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// if this fails, we want to default to being enabled
	if (!(GetVersionEx(&ver) && ver.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		return TRUE;
	}

	// now check with the security manager
	HANDLE hHandleToken;

	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_READ, &hHandleToken))
		return FALSE;

	UCHAR TokenInformation[1024];
	DWORD dwTokenInformationSize;

	BOOL bSuccess = ::GetTokenInformation(hHandleToken, TokenGroups,
			TokenInformation, sizeof(TokenInformation), &dwTokenInformationSize);

	::CloseHandle(hHandleToken);

	if (!bSuccess)
		return FALSE;

	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
	PSID psidAdministrators;

	if (!::AllocateAndInitializeSid(&siaNtAuthority, 2,
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
			&psidAdministrators))
		return FALSE;

	// assume that we don't find the admin SID.
	bSuccess = FALSE;

	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS)TokenInformation;

	for (UINT x = 0; x < ptgGroups->GroupCount; x++)
	{
		if (::EqualSid(psidAdministrators, ptgGroups->Groups[x].Sid))
		{
			bSuccess = TRUE;
			break;
		}
	}

	::FreeSid(psidAdministrators);
	return bSuccess;
}
