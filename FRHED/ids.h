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
 * @file  ids.h
 *
 * @brief Declaration of (Drag &) Drop data source class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_IDS_H_770C0C0CE6514332B68A5FCA9B823320
#define FRHED_IDS_H_770C0C0CE6514332B68A5FCA9B823320

class CDropSource: public IDropSource
{
private:
	ULONG m_cRefCount;

public:
	//Members
	CDropSource();
	~CDropSource();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IDataObject members
	STDMETHODIMP GiveFeedback(DWORD dwEffect);
	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
};

#endif