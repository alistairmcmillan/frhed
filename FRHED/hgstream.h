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
 * @file  hgstream.h
 *
 * @brief Declaration of HGlobalStream class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_HG_STREAM_H_0ED333C227A34E8180EDFA5FAECE8BEE
#define FRHED_HG_STREAM_H_0ED333C227A34E8180EDFA5FAECE8BEE

/**
 * @brief a streaming class using global memory.
 * Returns a handle to a global memory pointer Caller is responsible for
 * passing the buck or GlobalFree'ing the return value
 */
class HGlobalStream
{
public:

	HGlobalStream(DWORD blcksz = 1024); //This block size was picked at random
	~HGlobalStream();
	HGlobalStream& operator << (LPCTSTR);
	HGlobalStream& operator << (BYTE);
	HGlobalStream& operator << (DWORD);
	HGlobalStream& operator << (int);
	void Realloc(DWORD len, void* src);
	void filter(LPCTSTR src);
	HGLOBAL Relinquish();
	void Reset();
	inline HGlobalStream& HGlobalStream::operator<<(HGlobalStream&(*_f)(HGlobalStream&))
	{
		(*_f)(*this);
		return *this;
	}
	//Manipulators
	inline friend HGlobalStream& hex(HGlobalStream& s)
	{
		s._hex = 1;
		return s;
	}
	inline friend HGlobalStream& nbsp(HGlobalStream& s)
	{
		s._nbsp = 1;
		return s;
	}
	inline friend HGlobalStream& escapefilter(HGlobalStream& s)
	{
		s._escfilt = 1;
		return s;
	}
	DWORD precision;
	DWORD m_dwLen;
	DWORD m_dwSize;
	DWORD m_dwBlockSize;
	HGLOBAL m_hGlobal;

private:
	TCHAR *Extend(DWORD len);
	unsigned _hex:1;
	unsigned _nbsp:1;
	unsigned _escfilt:1;
};

#endif
