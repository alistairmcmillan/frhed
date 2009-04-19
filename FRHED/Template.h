/** 
 * @file  Template.h
 *
 * @brief Declaration file for Template class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include "Simparr.h"
#include "UnicodeString.h"
#include "hexwnd.h"  // For HexEditorWindow::BYTE_ENDIAN

/** @brief Maximum length of the type in template. */
const int TPL_TYPE_MAXLEN = 16;
/** @brief Maximum length of the variable name in template. */
const int TPL_NAME_MAXLEN = 128;

/**
 * @brief A class for applying template for the binary data.
 */
class Template
{
public:
	Template();
	~Template();

	void SetDataArray(const SimpleArray<BYTE> *arr);
	void SetOriginalFilename(LPCTSTR filename);
	bool OpenTemplate(LPCTSTR filename);
	bool LoadTemplateData();
	void CreateTemplateArray(int curByte);
	void ApplyTemplate(HexEditorWindow::BYTE_ENDIAN binaryMode, int curByte);
	LPCTSTR GetResult();

	void apply_template_on_memory(TCHAR* pcTpl, int tpl_len, SimpleArray<TCHAR>& ResultArray);

protected:
	int ignore_non_code(TCHAR* pcTpl, int tpl_len, int& index);
	int read_tpl_token(TCHAR* pcTpl, int tpl_len, int& index, TCHAR* name);

private:
	int m_filehandle; /**< File handle to template file. */
	TCHAR m_origFilename[MAX_PATH]; /**< Filename of the file in the editor. */
	TCHAR m_filename[MAX_PATH]; /**< Template file name. */
	int m_filelen; /**< Template file size. */
	TCHAR * m_tmplBuf; /**< Template file data buffer (read from file). */
	String m_resultString; /**< Resulting string for applied template. */
	SimpleArray<BYTE> *m_pDataArray; /**< Original data. */
};

#endif // _TEMPLATE_H_
