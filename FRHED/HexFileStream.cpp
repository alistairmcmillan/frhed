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
 * @file  HexFileStream.cpp
 *
 * @brief Implementation of the HexFileStream class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "HexFileStream.h"

int HexStream::lheatwhite()
{
	int c;
	do
	{
		c = lhgetc();
	} while (_istspace(c));

	lhungetc(c);
	return c;
}

/**
 * Get next character from the buffer.
 */
int HexBufferStream::lhgetc()
{
	int c = (int)(BYTE)m_pBuf[m_offset];
	if (c)
		++m_offset;
	else
		c = EOF;
	return c;
}
