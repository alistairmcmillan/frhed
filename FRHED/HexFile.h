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
 * @file  HexFile.h
 *
 * @brief Definition of the HexFile class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _HEX_FILE_H_
#define _HEX_FILE_H_

#include "HexFileStream.h"

/**
 * @brief A class for loading hex dump files.
 * This class handles loading hex dump files from a file in the disk or from
 * memory (clipboard). There are basically two types of dumps, simple files
 * just listing bytes in the file. And Frhed display-type dump files.
 */
class HexFile
{
public:
	HexFile();
	~HexFile();

	void Open(TCHAR * buffer, int size = -1);
	void Open(FILE * file);
	int CheckType();
	int GetSize() const;
	bool ParseSimple();
	bool ParseFormatted();
	void SetHwnd(HWND hwnd);
	SimpleArray<BYTE> *GetArray();
	bool WasAutoOffsetLen() const;
	int GetMinOffset() const;
	bool GetPartialStats() const;
	int GetPartialOffset() const;
	int GetBytesPerLine() const;
	int GetAutomaticBPL() const;
	int GetCharset() const;

private:
	SimpleArray<BYTE> m_data; /**< Array where data is loaded. */
	HexStream * m_pFile; /**< Pointer to file stream from which data is read. */
	int m_type; /**< Dump file type, simple or Frhed display-formatted. */
	int m_size; /**< Size of the data to read. */
	HWND m_hwnd; /**< Handle to the window getting dialogs. */
	bool m_bAutoOffsetLen;
	int m_minOffsetLen;
	bool m_bPartialStats;
	int m_partialOffset;
	int m_bytesPerLine;
	bool m_bAutomaticBPL;
	int m_charset;
};

#endif // _HEX_FILE_H_
