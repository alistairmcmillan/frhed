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
 * @file EncoderLib.cpp
 *
 * @brief Build-in encoder functions implementations.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "UnicodeString.h"
#include "EncoderLib.h"
#include "paths.h"

static const TCHAR EncoderSubFolder[] = _T("Encoders");

/**
 * @brief Find encoder dlls from the subfolder.
 * @return List of encoder dlls as a string.
 */
String enc_GetDllNames()
{
	TCHAR encodersFolder[MAX_PATH] = { 0 };
	paths_GetModulePath(GetModuleHandle(NULL), encodersFolder, MAX_PATH);
	_tcscat(encodersFolder, EncoderSubFolder);
	_tcscat(encodersFolder, _T("\\*.dll"));

	String encoders;
	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(encodersFolder, &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Add encoder dll name with subfolder name
			encoders += EncoderSubFolder;
			encoders += _T("\\");
			encoders += ff.cFileName;
			encoders += _T(";");
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
	return encoders;
}

/**
 * @brief Build-in XOR -1 encoder.
 * @param [in, out] p Data (and parameters) to encode.
 */
void WINAPI XorEncoder(MEMORY_CODING* p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) ^= -1;
}

/**
 * @brief Build-in ROT-13 encoder.
 * @param [in, out] p Data (and parameters) to encode.
 */
void WINAPI Rot13Encoder(LPMEMORY_CODING p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) = _istalpha(*q) ? (BYTE)(tolower(*q) < 'n' ? * q + 13 : *q - 13) : *q;
}
