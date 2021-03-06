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

#ifndef FRHED_DRAG_DROP_DLG_H_F19ADC1A93094D95A38A19D358920D6D
#define FRHED_DRAG_DROP_DLG_H_F19ADC1A93094D95A38A19D358920D6D

#include "resource.h"

/**
 * @brief A dialog for Drag & Drop.
 * This dialog is shown when user d&d data (not file) from another application
 * to Frhed view. User needs to select the format of the d&d'ed data.
 */
class DragDropDlg
{
public:
	enum { IDD = IDD_DRAG_DROP };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);

	DWORD allowable_effects; /**< Allowed operations bitmask. */
	bool effect; /**< Copy (true) or move (false) the data from other app? */
	UINT numformatetcs;
	FORMATETC *formatetcs;
	UINT numformats;
	UINT *formats;

protected:
	BOOL OnInitDialog(HWND);
	BOOL OnCommand(HWND, WPARAM, LPARAM);
};

#endif
