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
 * @file  Template.cpp
 *
 * @brief Implementation of the Template class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "resource.h"
#include "Template.h"
#include "UnicodeString.h"
#include "StringTable.h"
#include "LangString.h"

/**
 * @brief Constructor.
 */
Template::Template()
: m_filehandle(-1)
, m_tmplBuf(NULL)
, m_filelen(0)
, m_pDataArray(NULL)
{
}

/**
 * @brief Destructor.
 * Releases file resources and buffers after use.
 */
Template::~Template()
{
	delete [] m_tmplBuf;
	if (m_filehandle != -1)
		_close(m_filehandle);
}

/**
 * @brief Sets original data array.
 * Sets the data array for which the template will be applied. The original
 * data will not be modified.
 * @param [in] arr Original data.
 */
void Template::SetDataArray(const SimpleArray<BYTE> *arr)
{
	m_pDataArray = const_cast<SimpleArray<BYTE> *>(arr);
}

/**
 * @brief Sets the filename of the file in the editor.
 * This function sets name of the file into whose data the template is been
 * applied.
 * @param [in] filename Name of the file in the editor.
 */
void Template::SetOriginalFilename(LPCTSTR filename)
{
	_tcscpy(m_origFilename, filename);
}

/**
 * @brief Open template file.
 * @param [in] filename Filename of the template file to open.
 * @return true if opening succeeds, false otherwise.
 */
bool Template::OpenTemplate(LPCTSTR filename)
{
	m_filehandle = _topen(filename, _O_RDONLY | _O_BINARY);
	if (m_filehandle == -1)
		return false;
	else
	{
		_tcscpy(m_filename, filename);
		return true;
	}
}

/**
 * @brief Load template data from template file.
 * @return true if reading succeeds, false otherwise.
 */
bool Template::LoadTemplateData()
{
	if (m_filehandle == -1)
		return false;

	m_filelen = _filelength(m_filehandle);
	if (m_filelen == 0)
		return false;

	m_tmplBuf = new TCHAR[m_filelen + 1];
	if (m_tmplBuf)
	{
		memset(m_tmplBuf, 0, (m_filelen + 1) * sizeof(TCHAR));
		int ret = _read(m_filehandle, m_tmplBuf, m_filelen);
		if (ret == -1)
		{
			delete [] m_tmplBuf;
			return false;
		}
	}
	return true;
}

/**
 * @brief Create template info array.
 * Format template info text.
 * @param [in] curByte Current byte index in the open hex file.
 */
void Template::CreateTemplateArray(int curByte)
{
	// Print filename and current offset to output.
	m_resultString += GetLangString(IDS_TPL_FILENAME);
	m_resultString += _T(" ");
	m_resultString += m_origFilename;
	m_resultString += _T("\r\n");
	m_resultString += GetLangString(IDS_TPL_TEMPLATE_FILE);
	m_resultString += _T(" ");
	m_resultString += m_filename;
	m_resultString += _T("\r\n");
	m_resultString += GetLangString(IDS_TPL_APPLIED_AT);
	m_resultString += _T(" ");
	TCHAR buf[16];
	_stprintf(buf, _T("%d\r\n\r\n"), curByte);
	m_resultString += buf;
}

//-------------------------------------------------------------------
// Applies the template code in pcTpl of length tpl_len on the current file
// from the current offset and outputs the result to the ResultArray.
void Template::ApplyTemplate(HexEditorWindow::BYTE_ENDIAN binaryMode, int curByte)
{
	// Use source code in pcTpl to decipher data in file.
	int index = 0;
	int fpos = curByte;
	// While there is still code left...
	while (index < m_filelen)
	{
		// Read in the var type.
		if (FindNextCode(m_tmplBuf, m_filelen, index))
		{
			// index now points to first code character.
			// Get var type.
			TCHAR cmd[TPL_TYPE_MAXLEN]; // This holds the variable type, like byte or word.
			if (ReadToken(m_tmplBuf, m_filelen, index, cmd))
			{
				// cmd holds 0-terminated var type, index set to position of first space-
				// character after the type. Now test if valid type was given.
				//---- type BYTE ---------------------------------
				if (_tcscmp(cmd, _T("BYTE")) == 0 || _tcscmp(cmd, _T("char")) == 0)
				{
					// This is a byte/char.
					if (FindNextCode(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a byte?
						if (m_pDataArray->GetLength() - fpos >= 1)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							// index is set to a non-space character by last call to ignore_non_code.
							// Therefore the variable name can be read into buffer name.
							ReadToken(m_tmplBuf, m_filelen, index, name);
							// Write variable type and name to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							m_resultString += name;
							// Write value to output.
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							if ((*m_pDataArray)[fpos] != 0)
							{
								LangString fmt(IDS_TPL_FMT_BYTE_0);
								_stprintf(buf, fmt,
									(int) (signed char) (*m_pDataArray)[fpos], (*m_pDataArray)[fpos],
									(*m_pDataArray)[fpos], (*m_pDataArray)[fpos]);
							}
							else
							{
								LangString fmt(IDS_TPL_FMT_BYTE);
								_stprintf(buf, fmt,
									(int) (signed char) (*m_pDataArray)[fpos], (*m_pDataArray)[fpos],
									(*m_pDataArray)[fpos]);
							}
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							// Increase pointer for next variable.
							fpos += 1;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_BYTE);
							return;
						}
					}
					else
					{
						// No non-spaces after variable type up to end of array, so
						// no space for variable name.
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return;
					}
				}
				else if (_tcscmp(cmd, _T("WORD")) == 0 || _tcscmp(cmd, _T("short")) == 0)
				{
					// This is a word.
					if (FindNextCode(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a word?
						if (m_pDataArray->GetLength() - fpos >= 2)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							ReadToken(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							WORD wd = 0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								wd = *((WORD*)&((*m_pDataArray)[fpos]));
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 2; i++)
								{
									((TCHAR*)&wd)[i] = (*m_pDataArray)[fpos + 1 - i];
								}
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							LangString fmt(IDS_TPL_FMT_WORD);
							_stprintf(buf, fmt,
									(int) (signed short) wd, wd, wd );
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 2;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_WORD);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("DWORD")) == 0 || _tcscmp(cmd, _T("int")) == 0 ||
					_tcscmp(cmd, _T("long")) == 0 || _tcscmp(cmd, _T("LONG")) == 0 )
				{
					// This is a longword.
					if (FindNextCode(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a longword?
						if (m_pDataArray->GetLength() - fpos >= 4)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							ReadToken(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							DWORD dw = 0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								dw = *((DWORD*)&((*m_pDataArray)[fpos]));
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 4; i++)
									((char*)&dw)[i] = (*m_pDataArray)[fpos + 3 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							LangString fmt(IDS_TPL_FTM_DWORD);
							_stprintf(buf, fmt,	(signed long) dw, (unsigned long) dw, dw);
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 4;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_DWORD);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("float")) == 0)
				{
					// This is a float.
					if (FindNextCode(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a float?
						if (m_pDataArray->GetLength() - fpos >= 4)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							ReadToken(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							float f = 0.0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								f = *((float*)&((*m_pDataArray)[fpos]));
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 4; i++)
									((char*)&f)[i] = (*m_pDataArray)[fpos + 3 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							LangString fmt(IDS_TPL_FMT_FLOAT);
							_stprintf(buf, fmt, f, (unsigned long) *((int*) &f));
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 4;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_FLOAT);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else if (_tcscmp(cmd, _T("double")) == 0)
				{
					// This is a double.
					if (FindNextCode(m_tmplBuf, m_filelen, index))
					{
						// Enough space for a double?
						if (m_pDataArray->GetLength() - fpos >= 8)
						{
							// Read var name.
							TCHAR name[TPL_NAME_MAXLEN];
							ReadToken(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultString += cmd;
							m_resultString += _T(" ");
							// Write variable name to output.
							m_resultString += name;
							double d = 0.0;
							// Get value depending on binary mode.
							if (binaryMode == HexEditorWindow::ENDIAN_LITTLE)
							{
								d = *((double*)&((*m_pDataArray)[fpos]));
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for (i = 0; i < 8; i++)
									((char*)&d)[i] = (*m_pDataArray)[fpos + 7 - i];
							}
							TCHAR buf[TPL_NAME_MAXLEN + 200];
							LangString fmt(IDS_TPL_FMT_DOUBLE);
							_stprintf(buf, fmt, d);
							m_resultString += _T(" ");
							m_resultString += buf;
							m_resultString += _T("\r\n");
							fpos += 8;
						}
						else
						{
							m_resultString += GetLangString(IDS_TPL_ERR_NOSPC_DOUBLE);
							return;
						}
					}
					else
					{
						m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
						return; // No more code: missing name.
					}
				}
				else
				{
					m_resultString += GetLangString(IDS_TPL_ERR_UNK_TYPE);
					m_resultString += _T(" \"");
					m_resultString += cmd;
					m_resultString += _T("\"");
					return;
				}
			}
			else
			{
				// After the type there is only the array end. Therefore
				// no space for a variable name.
				m_resultString += GetLangString(IDS_TPL_ERR_NO_VAR);
				return;
			}
		}
		else
		{
			// No non-spaces up to the end of the array.
			break;
		}
	}
	// No more code left in m_tmplBuf.
	TCHAR buf[128];
	LangString strLen(IDS_TPL_LENGTH);
	_stprintf(buf, strLen, fpos - curByte);
	m_resultString += _T("\r\n");
	m_resultString += buf;
	m_resultString += _T("\r\n");
}

LPCTSTR Template::GetResult()
{
	return m_resultString.c_str();
}

/**
 * @brief Find next non-space code.
 * This will set index to the position of the next non-space-character.
 * @return true if found, false if there are no non-spaces left up to the end
 * of the array.
 */
bool Template::FindNextCode(TCHAR* pcTpl, int tpl_len, int& index)
{
	while (index < tpl_len)
	{
		// If code found, return.
		if (!_istspace(pcTpl[index]))
			return true;
		index++;
	}
	return false;
}

/**
 * @brief Write valid codes to @p dest.
 * Write all non-space characters from @p index to @p dest and close @p dest
 * with a zero-byte. @p index is set to position of the first space-
 * character.
 * @return false if there is only the array end after the
 * keyword. In that case @p index is set to tpl_len.
 */
bool Template::ReadToken(TCHAR* pcTpl, int tpl_len, int& index, TCHAR* dest)
{
	int i = 0;
	while (index + i < tpl_len)
	{
		if (_istspace(pcTpl[index + i]))
		{
			dest[i] = '\0';
			index += i;
			return true;
		}
		else
			dest[i] = pcTpl[index + i];
		i++;
	}
	dest[i] = '\0';
	index += i;
	return false;
}
