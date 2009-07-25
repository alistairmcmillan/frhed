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
 * @file  EncoderLib.h
 *
 * @brief Definition of encode/decode plugin interface.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _ENCODER_LIB_H_
#define _ENCODER_LIB_H_

#include "UnicodeString.h"

String enc_GetDllNames();

struct s_MEMORY_ENCODING;

typedef void (WINAPI* LPFNEncodeMemoryFunction)( s_MEMORY_ENCODING* p );

/**
 * @brief A struct defining encoded/decoded and operation.
 */
typedef struct s_MEMORY_ENCODING
{
	LPBYTE lpbMemory; /**< Pointer to data buffer. */
	DWORD dwSize; /**< Size of the data. */
	LPCTSTR lpszArguments; /**< Arguments for the function. */
	BOOL bEncode; /**< If TRUE encode, if false decode. */
	LPFNEncodeMemoryFunction fpEncodeFunc; /**< Name of the selected function. */
} MEMORY_CODING, *LPMEMORY_CODING;

/**
 * @brief Encoder description.
 */
typedef struct s_MEMORY_CODING_DESCRIPTION
{
	LPCTSTR lpszDescription; /**< Name of the encoder. */
	LPFNEncodeMemoryFunction fpEncodeFunc; /**< Pointer to the encoder function. */
} MEMORY_CODING_DESCRIPTION, *LPMEMORY_CODING_DESCRIPTION;

EXTERN_C LPMEMORY_CODING_DESCRIPTION WINAPI GetMemoryCodings();
typedef LPMEMORY_CODING_DESCRIPTION (WINAPI* LPFNGetMemoryCodings)();

void WINAPI XorEncoder(MEMORY_CODING* p);
void WINAPI Rot13Encoder(LPMEMORY_CODING p);

#endif // _ENCODER_LIB_H_
