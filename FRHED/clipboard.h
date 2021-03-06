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
 * @file  clipboard.h
 *
 * @brief Clipboard helper functions declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_CLIPBOARD_H_1C40192F862A4A9FB62DF59616630B9B
#define FRHED_CLIPBOARD_H_1C40192F862A4A9FB62DF59616630B9B

void TextToClipboard(HWND hwnd, TCHAR *text);
void TextToClipboard(HWND hwnd, TCHAR *text, int len);
void MessageCopyBox(HWND hwnd, LPTSTR text, UINT type);

#endif
