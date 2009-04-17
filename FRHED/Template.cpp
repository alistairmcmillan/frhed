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
#include "Template.h"

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
	strcpy(m_origFilename, filename);
}

/**
 * @brief Open template file.
 * @param [in] filename Filename of the template file to open.
 * @return true if opening succeeds, false otherwise.
 */
bool Template::OpenTemplate(LPCTSTR filename)
{
	m_filehandle = _open(filename, _O_RDONLY|_O_BINARY);
	if (m_filehandle == -1)
		return false;
	else
	{
		strcpy(m_filename, filename);
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

	m_tmplBuf = new char[m_filelen + 1];
	if (m_tmplBuf)
	{
		memset(m_tmplBuf, 0, m_filelen + 1);
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
	m_resultArray.SetSize(1, 100);
	// Print filename and current offset to output.
	m_resultArray.AppendArray("File: ", 6);
	m_resultArray.AppendArray(m_origFilename, strlen(m_origFilename));
	m_resultArray.AppendArray("\r\n", 2);
	m_resultArray.AppendArray("Template file: ", 15);
	m_resultArray.AppendArray(m_filename, strlen(m_filename));
	m_resultArray.AppendArray("\r\n", 2);
	m_resultArray.AppendArray("Applied at offset: ", 19);
	char buf[16];
	sprintf(buf, "%d\r\n\r\n", curByte);
	m_resultArray.AppendArray(buf, strlen(buf));
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
		if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
		{
			// index now points to first code character.
			// Get var type.
			char cmd[TPL_TYPE_MAXLEN]; // This holds the variable type, like byte or word.
			if (read_tpl_token(m_tmplBuf, m_filelen, index, cmd) == TRUE)
			{
				// cmd holds 0-terminated var type, index set to position of first space-
				// character after the type. Now test if valid type was given.
				//---- type BYTE ---------------------------------
				if (strcmp(cmd, "BYTE") == 0 || strcmp(cmd, "char") == 0)
				{
					// This is a byte/char.
					if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
					{
						// Enough space for a byte?
						if (m_pDataArray->GetLength() - fpos >= 1)
						{
							// Read var name.
							char name[TPL_NAME_MAXLEN];
							// index is set to a non-space character by last call to ignore_non_code.
							// Therefore the variable name can be read into buffer name.
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type and name to output.
							m_resultArray.AppendArray(cmd, strlen(cmd));
							m_resultArray.Append(' ');
							m_resultArray.AppendArray(name, strlen(name));
							// Write value to output.
							char buf[TPL_NAME_MAXLEN + 200];
							if ((*m_pDataArray)[fpos] != 0)
							{
								sprintf(buf, " = %d (signed) = %u (unsigned) = 0x%x = \'%c\'\r\n",
									(int) (signed char) (*m_pDataArray)[fpos], (*m_pDataArray)[fpos],
									(*m_pDataArray)[fpos], (*m_pDataArray)[fpos]);
							}
							else
							{
								sprintf(buf, " = %d (signed) = %u (unsigned) = 0x%x\r\n",
									(int) (signed char) (*m_pDataArray)[fpos], (*m_pDataArray)[fpos],
									(*m_pDataArray)[fpos]);
							}
							m_resultArray.AppendArray(buf, strlen(buf));
							// Increase pointer for next variable.
							fpos += 1;
						}
						else
						{
							m_resultArray.AppendArray("ERROR: not enough space for byte-size datum.", 45);
							return;
						}
					}
					else
					{
						// No non-spaces after variable type up to end of array, so
						// no space for variable name.
						m_resultArray.AppendArray("ERROR: missing variable name.", 29);
						return;
					}
				}
				else if (strcmp(cmd, "WORD") == 0 || strcmp(cmd, "short") == 0)
				{
					// This is a word.
					if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
					{
						// Enough space for a word?
						if (m_pDataArray->GetLength() - fpos >= 2)
						{
							// Read var name.
							char name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultArray.AppendArray(cmd, strlen(cmd));
							m_resultArray.Append(' ');
							// Write variable name to output.
							m_resultArray.AppendArray(name, strlen(name));
							WORD wd;
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
									((char*)&wd)[i] = (*m_pDataArray)[fpos + 1 - i];
								}
							}
							char buf[TPL_NAME_MAXLEN + 200];
							sprintf(buf, " = %d (signed) = %u (unsigned) = 0x%x\r\n",
									(int) (signed short) wd, wd, wd );
							m_resultArray.AppendArray(buf, strlen(buf));
							fpos += 2;
						}
						else
						{
							m_resultArray.AppendArray("ERROR: not enough space for WORD.", 34);
							return;
						}
					}
					else
					{
						m_resultArray.AppendArray("ERROR: missing variable name.", 29);
						return; // No more code: missing name.
					}
				}
				else if (strcmp(cmd, "DWORD") == 0 || strcmp(cmd, "int") == 0 ||
					strcmp(cmd, "long") == 0 || strcmp(cmd, "LONG") == 0 )
				{
					// This is a longword.
					if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
					{
						// Enough space for a longword?
						if (m_pDataArray->GetLength() - fpos >= 4)
						{
							// Read var name.
							char name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultArray.AppendArray(cmd, strlen(cmd));
							m_resultArray.Append(' ');
							// Write variable name to output.
							m_resultArray.AppendArray(name, strlen(name));
							DWORD dw;
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
							char buf[TPL_NAME_MAXLEN + 200];
							sprintf(buf, " = %d (signed) = %u (unsigned) = 0x%x\r\n",
										(signed long) dw, (unsigned long) dw, dw);
							m_resultArray.AppendArray( buf, strlen(buf) );
							fpos += 4;
						}
						else
						{
							m_resultArray.AppendArray("ERROR: not enough space for DWORD.", 34);
							return;
						}
					}
					else
					{
						m_resultArray.AppendArray("ERROR: missing variable name.", 29);
						return; // No more code: missing name.
					}
				}
				else if (strcmp(cmd, "float") == 0)
				{
					// This is a float.
					if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
					{
						// Enough space for a float?
						if (m_pDataArray->GetLength() - fpos >= 4)
						{
							// Read var name.
							char name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultArray.AppendArray(cmd, strlen(cmd));
							m_resultArray.Append(' ');
							// Write variable name to output.
							m_resultArray.AppendArray(name, strlen(name));
							float f;
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
							char buf[TPL_NAME_MAXLEN + 200];
							sprintf(buf, " = %f = 0x%x\r\n", f, (unsigned long) *((int*) &f));
							m_resultArray.AppendArray(buf, strlen(buf));
							fpos += 4;
						}
						else
						{
							m_resultArray.AppendArray("ERROR: not enough space for float.", 34);
							return;
						}
					}
					else
					{
						m_resultArray.AppendArray("ERROR: missing variable name.", 29);
						return; // No more code: missing name.
					}
				}
				else if (strcmp(cmd, "double") == 0)
				{
					// This is a double.
					if (ignore_non_code(m_tmplBuf, m_filelen, index) == TRUE)
					{
						// Enough space for a double?
						if (m_pDataArray->GetLength() - fpos >= 8)
						{
							// Read var name.
							char name[TPL_NAME_MAXLEN];
							read_tpl_token(m_tmplBuf, m_filelen, index, name);
							// Write variable type to output.
							m_resultArray.AppendArray(cmd, strlen(cmd));
							m_resultArray.Append(' ');
							// Write variable name to output.
							m_resultArray.AppendArray(name, strlen(name));
							double d;
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
							char buf[TPL_NAME_MAXLEN + 200];
							sprintf(buf, " = %g\r\n", d);
							m_resultArray.AppendArray(buf, strlen(buf));
							fpos += 8;
						}
						else
						{
							m_resultArray.AppendArray("ERROR: not enough space for double.", 35);
							return;
						}
					}
					else
					{
						m_resultArray.AppendArray("ERROR: missing variable name.", 29);
						return; // No more code: missing name.
					}
				}
				else
				{
					m_resultArray.AppendArray("ERROR: Unknown variable type \"", 30);
					m_resultArray.AppendArray(cmd, strlen(cmd));
					m_resultArray.Append('\"');
					return;
				}
			}
			else
			{
				// After the type there is only the array end. Therefore
				// no space for a variable name.
				m_resultArray.AppendArray("ERROR: Missing variable name.", 29);
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
	char buf[128];
	sprintf(buf, "\r\n-> Length of template = %d bytes.\r\n", fpos - curByte);
	m_resultArray.AppendArray(buf, strlen(buf));
	m_resultArray.Append('\0');
}

char * Template::GetResult()
{
	return m_resultArray;
}

//-------------------------------------------------------------------
// This will set index to the position of the next non-space-character.
// Return is FALSE if there are no non-spaces left up to the end of the array.
int Template::ignore_non_code(char* pcTpl, int tpl_len, int& index)
{
	while (index < tpl_len)
	{
		// If code found, return.
		switch(pcTpl[index])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		default:
			return TRUE;
		}
		index++;
	}
	return FALSE;
}

//-------------------------------------------------------------------
// Writes all non-space characters from index to dest and closes dest
// with a zero-byte. index is set to position of the first space-
// character. Return is false if there is only the array end after the
// keyword. In that case index is set to tpl_len.
int Template::read_tpl_token(char* pcTpl, int tpl_len, int& index, char* dest)
{
	int i = 0;
	while (index + i < tpl_len)
	{
		switch (pcTpl[index + i])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			dest[i] = '\0';
			index += i;
			return TRUE;

		default:
			dest[i] = pcTpl[index + i];
		}
		i++;
	}
	dest[i] = '\0';
	index += i;
	return FALSE;
}
