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
 * @file  LangTools.h
 *
 * @brief  Utility functions for translating Frhed.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_LANG_TOOLS_H_FC9B9C7930B14218869471711D3CB16F
#define FRHED_LANG_TOOLS_H_FC9B9C7930B14218869471711D3CB16F

class LangArray;
extern LangArray langArray;

void NTAPI TranslateDialog(HWND);
INT_PTR NTAPI ShowModalDialog(UINT, HWND, DLGPROC, LPVOID);

#endif
