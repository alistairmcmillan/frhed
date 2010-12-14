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
 * @file gktools.h
 *
 * @brief Dialogs and functions for working with disks and dlls.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_GKTOOLS_H_1BBCB83800EC460BAB1983F8B08477A0
#define FRHED_GKTOOLS_H_1BBCB83800EC460BAB1983F8B08477A0

BOOL WINAPI GetDllExportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);
BOOL WINAPI GetDllImportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);

extern PartitionInfo* SelectedPartitionInfo;

/**
 * @brief A dialog for opening disk drives.
 */
class OpenDriveDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_DRIVE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * @brief A dialog for moving in disk.
 */
class GotoTrackDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTO_TRACK_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};


#endif
