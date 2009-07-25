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
 * @file AddOne.cpp
 *
 * @brief Example encoder.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "windows.h"
#include "../../frhed/EncoderLib.h"

/**
 * @brief Example Add one -encoder.
 * This is example encoder which simply adds one to every byte value.
 * @param [in, out] p Data (and parameters) to encode.
 */
void WINAPI AddOne(MEMORY_CODING* p)
{
	LPBYTE q = p->lpbMemory;
	LPBYTE qMax = q + p->dwSize;
	while (q < qMax)
		*(q++) += 1;
}

MEMORY_CODING_DESCRIPTION MemoryCodings[] =
{
    { "Add One", AddOne },
    { 0, 0 }
};

EXTERN_C LPMEMORY_CODING_DESCRIPTION WINAPI GetMemoryCodings()
{
    return MemoryCodings;
}
