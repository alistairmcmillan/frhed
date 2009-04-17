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
 * @file  HexDump.h
 *
 * @brief Hex dump class declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _HEX_DUMP_H_
#define _HEX_DUMP_H_

#include "simparr.h"

class HexDump
{
public:
	HexDump();
	~HexDump();

	void Settings(int bytesPerLine, int charsPerLine,
			bool partialStats, __int64 partialOffset,
			int byteSpace, int charSpace, int charset);
	void SetOffsets(int minLen, int maxLen);
	void SetArray(const SimpleArray<unsigned char> *dataArray);
	void CreateBuffer(unsigned size);
	char * GetBuffer() const;
	void Write(unsigned startInd, unsigned endInd);

private:
	SimpleArray<BYTE> *m_pData;
	char *m_pBuffer;
	int m_bytesPerLine;
	int m_charsPerLine;
	int m_offsetMaxLen;
	int m_offsetMinLen;
	bool m_partialStats;
	__int64 m_partialOffset;
	int m_byteSpace;
	int m_charSpace;
	int m_charset;
};


#endif // _HEX_DUMP_H_
