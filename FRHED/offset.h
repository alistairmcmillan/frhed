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
 * @file  Offset.h
 *
 * @brief Definition of the offset helper functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_OFFSET_H_B21FEFC3C1A34B72B23A1754B2DC9D95
#define FRHED_OFFSET_H_B21FEFC3C1A34B72B23A1754B2DC9D95

bool offset_parse(LPCTSTR str, int &offset);
bool offset_parse64(LPCTSTR str, INT64 &offset);

#endif
