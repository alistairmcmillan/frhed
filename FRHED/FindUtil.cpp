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
 * @file FindUtil.cpp
 *
 * @brief Find functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "FindUtil.h"

static TCHAR equal(TCHAR c);
static TCHAR lower_case(TCHAR c);
static int find_bytes(TCHAR* ps, int ls, TCHAR* pb, int lb, int mode, TCHAR (*cmp)(TCHAR));

/**
 * @brief Find bytes in buffer.
 * @param [in] ps Start position.
 * @param [in] ls Length of src array.
 * @param [in] pb Start of searchstring.
 * @param [in] lb Length searchstring.
 * @param [in] mode Search mode:
 *   -1 : backwards search
 *    1 : forward search
 * @param [in] case_sensitive If true, performs case-sensitive search.
 * @return Position of found string or -1 if not there.
*/
int findutils_FindBytes(TCHAR* ps, int ls, TCHAR* pb, int lb, int mode, bool case_sensitive)
{
	TCHAR (*cmp)(TCHAR) = case_sensitive ? equal : lower_case;
	return find_bytes(ps, ls, pb, lb, mode, cmp);
}

//--------------------------------------------------------------------------------------------
// Required for the find function.
TCHAR equal(TCHAR c)
{
	return c;
}

TCHAR lower_case(TCHAR c)
{
	if (c >= 'A' && c <= 'Z')
		c = (TCHAR)('a' + c - 'A');
	return c;
}

/**
 * @brief Find bytes in buffer.
 * @param [in] ps Start position.
 * @param [in] ls Length of src array.
 * @param [in] pb Start of searchstring.
 * @param [in] lb Length searchstring.
 * @param [in] mode Search mode:
 *   -1 : backwards search
 *    1 : forward search
 * @param [in] cmp pointer to function that is applied to data before comparing.
 * @return Position of found string or -1 if not there.
*/
int find_bytes(TCHAR* ps, int ls, TCHAR* pb, int lb, int mode, TCHAR (*cmp)(TCHAR))
{
	int start, inc, end, i;
	if (mode == 1)
	{
		start = 0;
		inc = 1;
		end = ls - lb + 1;
	}
	else
	{
		start = ls - lb;
		inc = -1;
		end = 1;
	}

	for (; mode * start < end; start += inc)
	{
		for (i = start; i < start + lb; i++)
		{
			if (cmp (ps[i]) != cmp (pb[i - start]))
				break;
		}
		if (i == start + lb)
			return start;
	}

	return -1;
}
