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
 * @file  HexFileStream.h
 *
 * @brief Definition of the HexFileStream class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _HEXFILE_STREAM_H_
#define _HEXFILE_STREAM_H_

/**
 * @brief Hex streaming interface.
 */
class HexStream
{
public:
	// Methods
	virtual int lhgetc() = 0;
	virtual int lhungetc(int) = 0;
	virtual long lhtell() = 0;
	virtual int lhseek(long) = 0;
	virtual int scanf(LPCTSTR, int *) = 0;
	int lheatwhite();
};

/**
 * @brief Hex streaming class reading from file.
 * This class allows reading a hex stream from a file.
 */
class HexFileStream : public HexStream
{
private:
	FILE *m_hexfile; /**< File pointer to the hex file. */

public:
	/** 
	 * Constructor.
	 * @param [in] hexfile file pointer to open file.
	 */
	HexFileStream(FILE *hexfile) : m_hexfile(hexfile) { }

	/**
	 * Get next character from the file.
	 */
	virtual int lhgetc()
	{
		return fgetc(m_hexfile);
	}
	
	/**
	 * Return character to the file.
	 * @param [in] c Character to put back to the file.
	 */
	virtual int lhungetc(int c)
	{
		return ungetc(c, m_hexfile);
	}

	/**
	 * Gets current position in the file.
	 * @return Current position.
	 */
	virtual long lhtell()
	{
		return ftell(m_hexfile);
	}

	/**
	 * Move to given position in the file.
	 * @param [in] p Position to move to.
	 * @return 0 if success, != 0 otherwise.
	 */
	virtual int lhseek(long p)
	{
		return fseek(m_hexfile, p, SEEK_SET);
	}

	/**
	 * Read formatted integer from file.
	 * @param [in] fmt Format string.
	 * @param [in] p Pointer where to put the read integer.
	 * @return Count of read integers.
	 */
	virtual int scanf(LPCTSTR fmt, int *p)
	{
		return _ftscanf(m_hexfile, fmt, p);
	}
};

/**
 * @brief Hex streaming class reading from memory buffer.
 * This class allows reading a hex stream from a memory buffer.
 */
class HexBufferStream : public HexStream
{
private:
	TCHAR *m_pBuf; /**< Start address of the hex data buffer. */
	long m_offset; /**< Offset in the buffer. */

public:
	/** 
	 * Constructor.
	 * @param [in] pBuffer Pointer to begin of read buffer.
	 */
	HexBufferStream(TCHAR *pBuffer): m_pBuf(pBuffer), m_offset(0) { }

	virtual int lhgetc();

	/**
	 * Return character to the buffer.
	 * @param [in] c Character to put back to the buffer.
	 */
	virtual int lhungetc(int c)
	{
		return BYTE(m_pBuf[--m_offset] = (BYTE)c);
	}

	/**
	 * Gets current position in the buffer.
	 * @return Current position.
	 */
	virtual long lhtell()
	{
		return m_offset;
	}

	/**
	 * Move to given position in the buffer.
	 * @param [in] p Position to move to.
	 * @return 0 if success, != 0 otherwise.
	 */
	virtual int lhseek(long p)
	{
		return m_offset = p;
	}

	/**
	 * Read formatted integer from buffer.
	 * @param [in] fmt Format string.
	 * @param [in] p Pointer where to put the read integer.
	 * @return Count of read integers.
	 */
	virtual int scanf(LPCTSTR fmt, int *p)
	{
		return _stscanf(m_pBuf + m_offset, fmt, p);
	}
};

#endif // _HEXFILE_STREAM_H_
