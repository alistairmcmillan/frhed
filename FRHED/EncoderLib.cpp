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
#include "EncoderLib.h"

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
