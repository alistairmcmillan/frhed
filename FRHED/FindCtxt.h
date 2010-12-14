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
 * @file  FindCtxt.h
 *
 * @brief Declaration of the Find context class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_FIND_CTXT_H_C37B7BA9F3D740C19A93B2FFC7856804
#define FRHED_FIND_CTXT_H_C37B7BA9F3D740C19A93B2FFC7856804

class FindCtxt
{
public:
	enum { MAX_TEXT_LEN = 32 * 1024 };

	FindCtxt();
	~FindCtxt();
	
	void SetText(LPCTSTR text);
	bool HasText() const { return m_pText != NULL; }
	void ClearText();
	LPCTSTR GetText() const { return m_pText; }

	bool m_bMatchCase; /**< Do we match case? */
	int m_iDirection; /**< Direction of the find? */
	int m_bUnicode; /**< Find Unicode data? */

private:
	TCHAR *m_pText; /**< Text to find. */
};

#endif
