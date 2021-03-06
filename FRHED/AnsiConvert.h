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

#ifndef FRHED_ANSI_CONVERT_H_04C718D4726843F4AFFCDE8B07EFAEEA
#define FRHED_ANSI_CONVERT_H_04C718D4726843F4AFFCDE8B07EFAEEA

/**
 * @brief Convert string from wide char to ANSI.
 */
class MakeAnsi
{
private:
	BSTR m_bstr;
public:
	MakeAnsi(PCWSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeAnsi(): m_bstr(0) { }
	~MakeAnsi() { SysFreeString(m_bstr); }
	operator PCSTR() { return (PCSTR)m_bstr; }
	UINT GetLength()
	{
		return SysStringByteLen(m_bstr);
	}
	PSTR GetBufferSetLength(UINT len)
	{
		assert((len & 1) == 0);
		return SysReAllocStringLen(&m_bstr, 0, len / 2) ? PSTR(m_bstr) : 0;
	}
};

/**
 * @brief Convert string from ANSI to wide char.
 */
class MakeWide
{
private:
	BSTR m_bstr;
public:
	MakeWide(PCSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeWide(): m_bstr(0) { }
	~MakeWide() { SysFreeString(m_bstr); }
	operator PCWSTR() { return m_bstr; }
	UINT GetLength()
	{
		return SysStringLen(m_bstr);
	}
	PWSTR GetBufferSetLength(UINT len)
	{
		return SysReAllocStringLen(&m_bstr, 0, len) ? m_bstr : 0;
	}
};

typedef MakeWide A2W;
typedef MakeAnsi W2A;
typedef PCWSTR T2W, W2T;
typedef MakeAnsi T2A;
typedef MakeWide A2T;

#endif
