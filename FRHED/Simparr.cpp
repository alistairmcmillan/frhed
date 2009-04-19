/** 
 * @file  Simparr.cpp
 *
 * @brief Implementation file for SimpleString class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "Simparr.h"

/**
 * @brief Append string.
 * @param [in] ps String to append.
 * @return TRUE
 */
int SimpleString::AppendString(LPCTSTR ps)
{
	if (m_nUpperBound == ARR_EMPTY)
		return SetToString(ps);
	else
		InsertAtGrow(m_nUpperBound, ps, 0, _tcslen(ps));
	return TRUE;
}

/**
 * @brief Replace string content with given string.
 * @param [in] ps String to replace current content.
 * @return TRUE.
 */
int SimpleString::SetToString(LPCTSTR ps)
{
	Clear();
	return AppendString(ps);
}

/**
 * @brief Replace string content with given string.
 * @param [in] ps String to replace current content.
 * @return TRUE.
 */
SimpleString& SimpleString::operator=(LPCTSTR ps)
{
	SetToString(ps);
	return *this;
}

/**
 * @brief Replace string content with given string.
 * @param [in] str String to replace current content.
 * @return TRUE.
 */
SimpleString& SimpleString::operator=(const SimpleString &str)
{
	SetToString(&str[0]);
	return *this;
}

/**
 * @brief Append string to current string.
 * @param [in] ps String to append.
 * @return New string (given string appended).
 */
SimpleString& SimpleString::operator+=(LPCTSTR ps)
{
	if (m_nUpperBound == ARR_EMPTY)
		SetToString(ps);
	else
		InsertAtGrow(m_nUpperBound, ps, 0, _tcslen(ps));
	return *this;
}

/**
 * @brief Get length of the string.
 * @return String length.
 */
int SimpleString::StrLen() const
{
	if (m_pT != NULL)
		return _tcslen(m_pT);
	else
		return 0;
}

/**
 * @brief Constructor.
 * Create a string containing only a zero-byte.
 */
SimpleString::SimpleString()
{
	m_nGrowBy = 64;
	Clear();
}

/**
 * @brief Constructor.
 * Create a SimpleString from a normal char array-string.
 * @param [in] ps String to use as initial value.
 */
SimpleString::SimpleString(LPCTSTR ps)
{
	m_nGrowBy = 64;
	SetToString(ps);
}

/**
 * @brief Clear string contents.
 */
void SimpleString::Clear()
{
	ClearAll();
	Append('\0');
}

/**
 * @brief Append a string.
 * @param [in] str1 String to append.
 * @return String with given string appended.
 */
SimpleString SimpleString::operator+(const SimpleString& str1)
{
	SimpleString t1;
	t1.SetToString(m_pT);
	t1 += str1;
	return SimpleString(&t1[0]);
}

/**
 * @brief Check if string is empty.
 * @return true if the string is empty, false otherwise.
 */
bool SimpleString::IsEmpty() const
{
	return !StrLen();
}

/**
 * @brief Concat two strings.
 * @param [in] ps1 First string to concat.
 * @param [in] ps2 Second string to concat.
 * @return String with given strings combined.
 */
SimpleString operator+(const SimpleString &ps1, LPCTSTR ps2)
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

/**
 * @brief Concat two strings.
 * @param [in] ps1 First string to concat.
 * @param [in] ps2 Second string to concat.
 * @return String with given strings combined.
 */
SimpleString operator+(LPCTSTR ps1, const SimpleString &ps2)
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}
