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
 * @file  BinTrans.h
 *
 * @brief Text to binary converter class declaration.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef BinTrans_h
#define BinTrans_h

#include "UnicodeString.h"
#include "Simparr.h"

int create_bc_translation(TCHAR** ppd, LPCTSTR src, int srclen, int charset, int binarymode);

/**
 * @brief A class translating between text export and binary data.
 */
class Text2BinTranslator
{
public:
	int bCompareBin(Text2BinTranslator& tr2, int charmode, int binmode);
	Text2BinTranslator(TCHAR* ps);
	Text2BinTranslator(LPCTSTR ps);
	static int iIsBytecode(LPCTSTR src, int len);
	static int iBytes2BytecodeDestLen(const BYTE* src, int srclen);
	static int iLengthOfTransToBin(LPCTSTR src, int srclen);
	static int iCreateBcTranslation(TCHAR* dest, LPCTSTR src, int srclen, int charmode, int binmode);
	static int iTranslateOneBytecode(TCHAR* dest, LPCTSTR src, int srclen, int binmode);
	static int iFindBytePos(LPCTSTR src, TCHAR c);
	static int iTranslateBytesToBC(TCHAR* pd, const BYTE* src, int srclen);

private:
	int GetTrans2Bin(SimpleArray<TCHAR>& sa, int charmode, int binmode);
	String m_str; /**< String for conversion. */
};

#endif // BinTrans_h
