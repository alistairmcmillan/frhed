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
 * @file EncodeDlg.h
 *
 * @brief Encode/decode dialog declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef ENCODE_DECODE_DLG_H
#define ENCODE_DECODE_DLG_H

#include "hexwnd.h"
#include "EncoderLib.h"
#include "resource.h"

BOOL WINAPI GetMemoryCoding(HINSTANCE hInstance, HWND hParent, LPMEMORY_CODING p, LPCTSTR lpszDlls);

/**
 * @brief A dialog for selecting encoder DLL.
 */
class EncodeDecodeDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_ENCODE_DECODE_DIALOG };
	INT_PTR DlgProc(HWND, UINT, WPARAM, LPARAM);
};

#endif // ENCODE_DECODE_DLG_H
