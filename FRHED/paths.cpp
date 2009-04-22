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
 * @file  paths.cpp
 *
 * @brief Implementation of the path functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include <tchar.h>
#include "paths.h"
#include <sys/types.h>
#include <sys/stat.h>

/**
 * @brief Check if the given file exists.
 * @param [in] file Full path to the file to check.
 * @return true if the file exists, false if file is not found.
 */
bool paths_DoesFileExist(LPCTSTR file)
{
	struct _stat st;
	if (_tstat(file, &st) == 0)
		return true;
	else
		return false;
}

/**
 * @brief Get the full path for relative (to Frhed exe) path.
 * @param [in] path Relative path to Frhed executable.
 * @param [in, out] fullpath Buffer where the full path is copied.
 * @param [in] size Size of the buffer pointed by fullpath parameter.
 */
void paths_GetFullPath(LPCTSTR path, LPTSTR fullpath, int size)
{
	TCHAR modpath[MAX_PATH] = {0};
	paths_GetModulePath(GetModuleHandle(NULL), modpath, MAX_PATH);
	const int len = _tcslen(modpath);
	_tcsncpy(fullpath, modpath, size - 1);
	_tcsncat(fullpath, path, size - len - 1);
}

/**
 * @brief Get module's path component (without filename).
 * @param [in] hModule Module's handle.
 * @param [in,out] path Char table where path is copied.
 * @param [in] size Size of the char table.
 * @return Module's path.
 */
void paths_GetModulePath(HMODULE hModule, LPTSTR path, int size)
{
	TCHAR temp[MAX_PATH + 1] = {0};
	if (GetModuleFileName(hModule, temp, MAX_PATH))
	{
		TCHAR drive[_MAX_DRIVE] = {0};
		TCHAR dir[_MAX_DIR] = {0};
		_tsplitpath(temp, drive, dir, NULL, NULL);
		const int drive_len = _tcslen(drive);
		const int dir_len = _tcslen(dir);
		if (drive_len + dir_len < size)
		{
			_tcsncpy(path, drive, drive_len);
			_tcsncat(path, dir, size - drive_len);
		}
	}
}

/**
 * @brief Open text file to text editor.
 * @param [in] hwnd Parent window for the editor process (Frhed main window?).
 * @param [in] file Relative (to Frhed executable) path the the file to open.
 */
void paths_OpenFile(HWND hwnd, LPCTSTR file)
{
	TCHAR filepath[MAX_PATH] = {0};
	paths_GetModulePath(GetModuleHandle(NULL), filepath, MAX_PATH);
	const int len = _tcslen(filepath);
	_tcsncat(filepath, file, MAX_PATH - len);
	ShellExecute(hwnd, _T("open"), _T("notepad.exe"), filepath, NULL, SW_SHOWNORMAL);
}
