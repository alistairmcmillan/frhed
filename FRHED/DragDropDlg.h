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
 * @file  DragDropDlg.h
 *
 * @brief Drag and drop -dialog declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _DRAG_DROP_DLG_H_
#define _DRAG_DROP_DLG_H_

#include "resource.h"

/**
 * @brief A dialog for Drag & Drop.
 */
class DragDropDlg
{
public:
	enum { IDD = IDD_DRAGDROP };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
	DWORD allowable_effects;
	bool effect;
	UINT numformatetcs;
	FORMATETC *formatetcs;
	UINT numformats;
	UINT *formats;
};

#endif // _DRAG_DROP_DLG_H_
