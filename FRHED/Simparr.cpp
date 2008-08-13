/** 
 * @file  Simparr.cpp
 *
 * @brief Implementation file for SimpleArray and SimpleString classes.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: $

#include "precomp.h"
#include "simparr.h"

//-------------------------------------------------------------------
int SimpleString::AppendString( char* ps )
{
	if( m_nUpperBound == -1 )
		return SetToString( ps );
	else
	{
		InsertAtGrow( m_nUpperBound, ps, 0, strlen( ps ) );
	}
	return TRUE;
}

//-------------------------------------------------------------------
int SimpleString::SetToString( char* ps )
{
	Clear();
	return AppendString( ps );
}

//-------------------------------------------------------------------
char* SimpleString::operator=( char* ps )
{
	SetToString( ps );
	return ps;
}

//-------------------------------------------------------------------
SimpleString& SimpleString::operator=( SimpleString str )
{
	SetToString( &str[0] );
	return *this;
}

//-------------------------------------------------------------------
char* SimpleString::operator+=( char* ps )
{
	if( m_nUpperBound == -1 )
		SetToString( ps );
	else
	{
		InsertAtGrow( m_nUpperBound, ps, 0, strlen( ps ) );
	}
	return ps;
}

//-------------------------------------------------------------------
int SimpleString::StrLen()
{
	if( m_pT != NULL )
		return strlen( m_pT );
	else
		return 0;
}

//-------------------------------------------------------------------
SimpleString::SimpleString()
{
	// Create a string containing only a zero-byte.
	m_nGrowBy = 64;
	Clear();
}

//-------------------------------------------------------------------
SimpleString::SimpleString( char* ps )
{
	// Create a SimpleString from a normal char array-string.
	m_nGrowBy = 64;
	Clear();
	SetToString( ps );
}

//-------------------------------------------------------------------
void SimpleString::Clear()
{
	ClearAll();
	Append( '\0' );
}

//-------------------------------------------------------------------
SimpleString SimpleString::operator+( SimpleString& str1 )
{
	SimpleString t1;
	t1.SetToString( m_pT );
	t1 += str1;
	return SimpleString( &t1[0] );
}

//-------------------------------------------------------------------
int SimpleString::IsEmpty()
{
	return !StrLen();
}

//-------------------------------------------------------------------
SimpleString operator+( SimpleString ps1, char* ps2 )
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

//-------------------------------------------------------------------
SimpleString operator+( char* ps1, SimpleString ps2 )
{
	SimpleString s1;
	s1 += ps1;
	s1 += ps2;
	return SimpleString(s1);
}

//-------------------------------------------------------------------
