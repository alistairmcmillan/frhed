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
 * @file  AnsiConvert.h
 *
 * @brief  Class for converting strings from wide char to ANSI.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _ANSI_CONVERT_H_
#define _ANSI_CONVERT_H_

/**
 * @brief Convert string from wide char to ANSI.
 */
class AnsiConvert
{
private:
	BSTR m_bstr;
public:
	AnsiConvert(PCWSTR text, UINT codepage = CP_ACP);
	~AnsiConvert() { SysFreeString(m_bstr); }
	operator PCSTR() { return (PCSTR)m_bstr; }
};

#endif // _ANSI_CONVERT_H_
