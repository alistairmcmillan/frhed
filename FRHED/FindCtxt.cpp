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
 * @file  FindCtxt.cpp
 *
 * @brief Implementation of the hex window.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "FindCtxt.h"

/**
 * @brief Constructor. 
 */
FindCtxt::FindCtxt()
: m_pText(NULL)
, m_bMatchCase(false)
, m_iDirection(0)
, m_bUnicode(false)
{
}

/**
 * @brief Destructor.
 */
FindCtxt::~FindCtxt()
{
	delete [] m_pText;
}

/**
 * @brief Clear the search text.
 */
void FindCtxt::ClearText()
{
	if (m_pText)
		delete [] m_pText;
	m_pText = NULL;
}

/**
 * @brief Set the search text.
 * @param [in] text New search text.
 */
void FindCtxt::SetText(LPCTSTR text)
{
	ClearText();
	int len = _tcslen(text);
	m_pText = new TCHAR[len + 1];
	ZeroMemory(m_pText, (len + 1) * sizeof(TCHAR));
	_tcsncpy(m_pText, text, len);
}
