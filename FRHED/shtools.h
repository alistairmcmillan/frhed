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
 * @file  shtools.h
 *
 * @brief Declaration of Shell api tool functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_SHTOOLS_H_D011422EE0B24F18859141A774CD385C
#define FRHED_SHTOOLS_H_D011422EE0B24F18859141A774CD385C

STDAPI CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink);
STDAPI CreateLinkToMe(LPCTSTR lpszPathLink);
STDAPI ResolveIt(HWND hwnd, LPCTSTR lpszLinkFile, LPTSTR lpszPath);
STDAPI PathsEqual(LPCTSTR, LPCTSTR);
STDAPI PathPointsToMe(LPCTSTR);
STDAPI CreateShellCommand(LPCTSTR);
STDAPI GetLongPathNameWin32(LPCTSTR lpszShortPath, LPTSTR lpszLongPath);

#endif
