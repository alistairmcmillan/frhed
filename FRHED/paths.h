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
 * @file paths.h
 *
 * @brief Path handling functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_PATHS_H_AEFBD4A1A5CE4F4D9AC111A880C6B127
#define FRHED_PATHS_H_AEFBD4A1A5CE4F4D9AC111A880C6B127

bool paths_DoesFileExist(LPCTSTR file);
void paths_GetFullPath(LPCTSTR path, LPTSTR fullpath, int size);
void paths_GetModulePath(HMODULE hModule, LPTSTR path, int size);
void paths_OpenFile(HWND hwnd, LPCTSTR file);

#endif
