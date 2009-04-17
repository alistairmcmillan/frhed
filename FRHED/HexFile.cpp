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
 * @file  HexFile.cpp
 *
 * @brief Implementation of the HexFile class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "Simparr.h"
#include "HexFile.h"
#include "HexFileStream.h"
#include "resource.h"
#include "LangString.h"

/**
 * @brief Convert hex byte to numeric value.
 * Convert hex byte (0-9,a-f) to numeric value. E.g '8' gets converted to 8
 * and 'B' gets converted to 11.
 * @param [in] by Byte to convert.
 * @return Numeric value, or ' ' if by is invalid hex char.
 * @note The function assumes the @p by is valid hex byte.
 */
static BYTE Hex2Nibble(BYTE by)
{
	assert(_istxdigit(by));

	if (_istdigit(by))
		return by - '0';
	else
	{
		if (_istlower(by))
			return by - 'a' + 10;
		else
			return by - 'A' + 10;
	}
}

/**
 * @brief Constructor.
 */
HexFile::HexFile()
: m_pFile(NULL)
, m_type(-1)
, m_size(0)
, m_bAutoOffsetLen(false)
, m_minOffsetLen(-1)
, m_bPartialStats(false)
, m_partialOffset(-1)
, m_bytesPerLine(-1)
, m_bAutomaticBPL(true)
, m_charset(-1)
{
}

/**
 * @brief Destructor.
 */
HexFile::~HexFile()
{
	delete m_pFile;
}

/**
 * @brief Open dump from memory buffer.
 * This function opens the hex dump from the memory.
 * @param [in] buffer Pointer to the buffer to hold the dump data.
 * @param [in] size Size of the buffer (if known). -1 if size is not known.
 */
void HexFile::Open(char * buffer, int size)
{
	m_pFile = new chexfile_stream(buffer);
	m_size = size;
}

/**
 * @brief Load a dump from the file.
 * This function opens the hex dump from the file in the disk.
 * @param [in] file Pointer to the file stream.
 */
void HexFile::Open(FILE * file)
{
	m_pFile = new fhexfile_stream(file);
	int cur = ftell(file);
	fseek(file, 0, SEEK_END);
	int pos = ftell(file);
	fseek(file, cur, SEEK_SET);
	m_size = pos;
}

/**
 * @brief Detemine the hex dump type (simple/formatted).
 * @return Type of the dump.
 */
int HexFile::CheckType()
{
	int typ = 0;//type of file (0=just hex digits)
	//Check the type of file - if only whitespace & hex then just hex else can be used to set line len etc
	//There is probably a better way to do this
	int temp;
	while ((temp = m_pFile->lhgetc()) != EOF)
	{
		BYTE ct = (BYTE)temp;
		if (!(_istspace(ct) || _istxdigit(ct)))
		{
			typ = 1;
			break;
		}
	}
	if (m_size == -1)
		m_size = m_pFile->lhtell();
	m_pFile->lhseek(0);
	m_type = typ;
	return typ;
}

/**
 * @brief Return dump size.
 * @return Size of the dump in bytes.
 */
int HexFile::GetSize() const
{
	return m_size;
}

/**
 * @brief Set window handle used for message boxes.
 * @param [in] hwnd Handle to the window showing message boxes.
 */
void HexFile::SetHwnd(HWND hwnd)
{
	m_hwnd = hwnd;
}

/**
 * @brief Return the array having loaded hex dump.
 * @return Array containing data parsed from the hex dump.
 */
SimpleArray<BYTE> *HexFile::GetArray()
{
	return &m_data;
}

/**
 * @brief Parse simple hex dump file.
 * This method parses simple hex dump file having just list of bytes.
 * @return true if the parsing succeeded, false if there was an error.
 */
bool HexFile::ParseSimple()
{
	int temp[4] = {0};
	bool flnd = false; // High nibble?
	int ii = 0;
	int diio = 1; // ignore invalid chars
	for (int i = 0 ; (temp[0] = m_pFile->lhgetc()) != EOF ; i++)
	{
		if (_istxdigit(temp[0]))
		{
			if (!flnd)
			{
				if (!m_data.SetSize(ii + 1))
				{
					LangString app(IDS_APPNAME);
					UINT ret = MessageBox(m_hwnd, "Not enough memory to import data.\n"
							"Cannot continue!\nDo you want to keep what has been found so far?",
							app, MB_YESNO | MB_ICONERROR);
					return IDYES == ret;
				}
				m_data.ExpandToSize();
				m_data[ii] = 0;
			}
			m_data[ii] |= Hex2Nibble((BYTE)temp[0]) ;
			if (flnd)
				ii++;
			else
				m_data[ii] <<= 4;
			flnd = !flnd;
		}
		else if (!_istspace(temp[0]) && diio)
		{
			LangString app(IDS_APPNAME);
			UINT ret = MessageBox(m_hwnd, "Illegal character found.\n"
					"Ignore further illegal characters?",
					app, MB_YESNOCANCEL | MB_ICONERROR);
			switch (ret)
			{
			case IDYES:
				diio = 0;
				break;
			case IDCANCEL:
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Parse formatted hex dump file.
 * This method parses Frhed view-style formatted hex dump file.
 * @return true if the parsing succeeded, false if there was an error.
 */
bool HexFile::ParseFormatted()
{
	LangString app(IDS_APPNAME);
	int temp[4] = {0};
	BYTE c[4] = {0};
	int i, ii = 0, ls, bpl, fo = 0, fol;
	int dim = 1, diio = 1;
	bool flnd = true;
	m_bAutoOffsetLen = true;

	do
	{
		//get the offset
		if (diio)
			ls = m_pFile->lhtell();
		int ol = 0;
		for (;;)
		{
			temp[0] = m_pFile->lhgetc();
			if (temp[0] == EOF)
				goto UnexpectedEndOfData;
			c[0] = (BYTE)temp[0];
			if (_istspace(c[0]))
				break;
			if (! _istxdigit(c[0]) && diio)
			{
				UINT ret = MessageBox(m_hwnd, "Illegal character in offset.\n"
						"Ignore further invalid offsets?", app,
						MB_YESNOCANCEL | MB_ICONERROR);
				switch (ret)
				{
				case IDYES:
					diio = 0;
					break;
				case IDCANCEL:
					return FALSE;//bad file
				}
			}
			++ol;
		}

		if (flnd)
			m_minOffsetLen = fol = ol;
		else if (fol != ol)
			m_bAutoOffsetLen = false;

		i = m_pFile->lhtell();

		if (diio)
		{
			int tmp = 0;
			m_pFile->lhseek(ls);
			m_pFile->scanf("%x", &tmp);
			if (flnd && tmp)
			{
				char msg[150];
				sprintf(msg,
					"The first offset found was 0x%x, which is greater than zero.\n"
					"Do you want to insert %d null bytes at the start of the data?", tmp, tmp);
				UINT ret = MessageBox(m_hwnd, msg, app, MB_YESNO | MB_ICONWARNING);
				if (ret == IDYES)
				{
					ii = tmp;
					if (!m_data.SetSize(ii))
						goto OutOfMemory;
					m_data.ExpandToSize();
					for (int jj = 0; jj < ii; jj++)
						m_data[jj] = 0;
				}
				else
				{
					fo = tmp;
					m_bPartialStats = true;
					m_partialOffset = tmp;
				}
			}
			else if (ii + fo != tmp)
			{
				UINT ret = MessageBox(m_hwnd, "Invalid offset found.\n"
						"Ignore further invalid offsets?", app,
						MB_YESNOCANCEL | MB_ICONWARNING);
				switch (ret)
				{
				case IDYES:
					diio = 0;
					break;
				case IDNO:
					return FALSE;//bad file
				}
			}
		}

		m_pFile->lhseek(i);

		if (m_pFile->lheatwhite() == EOF)
			goto UnexpectedEndOfData;

		ls = ii;//remember the start of the line in the DataArray

		//get data bytes
		for (bpl = 0 ;; bpl++)
		{
			//get the three chars
			for (i = 0 ; i < 3 ; i++)
			{
				temp[i] = m_pFile->lhgetc();
				if (temp[i] == EOF)
					goto UnexpectedEndOfData;
				c[i] = (BYTE)temp[i];
			}
			if (!(_istxdigit(c[0]) && _istxdigit(c[1]) && _istspace(c[2])))
				goto IllegalCharacter;
			//yes we are fine
			//store the value no matter what
			if (!m_data.SetSize(ii + 1))
				goto OutOfMemory;
			m_data.ExpandToSize();
			//do this so that we don't overwrite memory outside the DataArray
			// - because sscanf requires an int for storage
			int tmp = 0;
			sscanf((char*)c, "%x", &tmp);//save it to tmp
			m_data[ii] = (BYTE)tmp;
			ii++;//next byte

			for (i = 0 ; i < 3 ; i++)
			{
				temp[i] = m_pFile->lhgetc();
				if (temp[i] == EOF)
					goto UnexpectedEndOfData;
				c[i] = (BYTE)temp[i];
			}
			m_pFile->lhungetc(c[2]);
			m_pFile->lhungetc(c[1]);
			m_pFile->lhungetc(c[0]);
			if (c[0] == ' ' || c[0] =='_')
			{
				if (c[1] == c[0] && c[2] == ' ')
				{
					//get those back
					for (i = 0 ; i < 3 ; i++)
						if (m_pFile->lhgetc() == EOF)
							goto UnexpectedEndOfData;
					bpl++;
					for (;;bpl++)
					{
						for (i = 0 ; i < 3 ; i++)
						{
							temp[i] = m_pFile->lhgetc();
							if (temp[i] == EOF)
								return TRUE;//Assume the file is good
							c[i] = (BYTE)temp[i];
						}
						if (c[0] == '\r' && c[1] == '\n')
						{//We have missed the chars because of all the spaces
							m_pFile->lhungetc(c[2]);
							m_pFile->lhungetc(c[1]);
							m_pFile->lhungetc(c[0]);
							goto NextLine;
						}
						if (c[0] == ' ' && c[1] != ' ')
						{//We have found the start of the chars
							m_pFile->lhungetc(c[2]);
							m_pFile->lhungetc(c[1]);
							m_pFile->lhungetc(c[0]);
							break;
						}
					}
				}
				else if (c[0]=='_')
					goto IllegalCharacter;

				if (flnd)
				{
					m_bytesPerLine = bpl + 1;
					m_bAutomaticBPL = false;
				}
				break;

			}
			else if (!_istxdigit(c[0]))
			{
				goto IllegalCharacter;
			}
		}//got the data bytes

		//weak point - assumes iCharSpace is 1
		//trash the extra space
		if (m_pFile->lhgetc() == EOF)
			goto UnexpectedEndOfData;
		
		//Verify that the data read by the above loop is correct and equal to that read by this loop
		for ( ; ls < ii ; ls++)
		{
			temp[0] = m_pFile->lhgetc();
			if (temp[0] == EOF)
				goto UnexpectedEndOfData;
			c[0] = (BYTE)temp[0];
			BYTE ct = m_data[ls];
			//Get translated character - '.' for non-printables c[0] else

			c[1] = ct >= 32 && ct <= 126 || ct >= 160 && ct <= 255 || ct >= 145 && ct <= 146 ? ct : '.';
			//check if the data is the same
			if (!ct || c[1] == ct)
			{//0 (both OEM & ANSI translate) or one of those ranges where neither cset translates the character
				if (c[0] != c[1])
				{
BadData:
					if (dim)
					{
						UINT ret = MessageBox(m_hwnd, "Character data does not agree with hex data.\n"
								"Ignore further mismatched data?\nNB: Hex data will be used when ignoring.",
								app, MB_YESNOCANCEL | MB_ICONWARNING);
						switch (ret)
						{
						case IDYES:
							dim = 0;
							break;
						case IDCANCEL:
							return FALSE;//bad file
						}
					}
				}
			}
			else
			{
				if (c[0] == ct)
				{
					m_charset = OEM_FIXED_FONT;
				}
				else
				{
					if (c[0] != c[1])
						goto BadData;
					m_charset = ANSI_FIXED_FONT;
				}
			}
		}//get rest of line
NextLine:
		flnd = false;
	} while (m_pFile->lheatwhite() != EOF);
	//parsing loop
	return TRUE;

IllegalCharacter:
	//someone has been buggering with the file & the syntax is screwed up
	//the next digit is not hex ' ' or '_'
	return IDYES == MessageBox(m_hwnd, "Illegal character in hex data.\nCannot continue!\nDo you want to keep what has been found so far?",
			app, MB_YESNO | MB_ICONERROR);//bad file
UnexpectedEndOfData:
	return IDYES == MessageBox(m_hwnd, "Unexpected end of data found\nCannot continue!\nDo you want to keep what has been found so far?",
			app, MB_YESNO | MB_ICONERROR);
OutOfMemory:
	return IDYES == MessageBox(m_hwnd, "Not enough memory to import data.\nCannot continue!\nDo you want to keep what has been found so far?",
			app, MB_YESNO | MB_ICONERROR);
}

bool HexFile::WasAutoOffsetLen() const
{
	return m_bAutoOffsetLen;
}

int HexFile::GetMinOffset() const
{
	return m_minOffsetLen;
}

bool HexFile::GetPartialStats() const
{
	return m_bPartialStats;
}

int HexFile::GetPartialOffset() const
{
	return m_partialOffset;
}

int HexFile::GetBytesPerLine() const
{
	return m_bytesPerLine;
}

int HexFile::GetAutomaticBPL() const
{
	return m_bAutomaticBPL;
}

int HexFile::GetCharset() const
{
	return m_charset;
}
