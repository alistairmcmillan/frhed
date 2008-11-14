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
 * @file  version.h
 *
 * @brief Version number (macros) for Frhed.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FRHED_VERSION_H_
#define _FRHED_VERSION_H_

// These set Frhed release version number
#define FRHED_MAJOR_VERSION 1
#define FRHED_MINOR_VERSION 2
#define FRHED_SUB_RELEASE_NO 1
#define FRHED_BUILD_NO 1

// Utility macros for formatting version number to string.
#define SHARPEN_A(X) #X
#define SHARPEN_W(X) L#X
#define SHARPEN(T,X) SHARPEN_##T(X)

#define CURRENT_VERSION SHARPEN(A,FRHED_MAJOR_VERSION) "." SHARPEN(A,FRHED_MINOR_VERSION)
#define SUB_RELEASE_NO SHARPEN(A,FRHED_SUB_RELEASE_NO)
#define BUILD_NO SHARPEN(A,FRHED_BUILD_NO)

#endif // _FRHED_VERSION_H_
