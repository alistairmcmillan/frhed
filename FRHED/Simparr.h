/** 
 * @file  Simparr.h
 *
 * @brief Declaration file for SimpleArray and SimpleString classes.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef FRHED_SIMPLEARR_H_80F70DE188054EBFA1C3979C01099475
#define FRHED_SIMPLEARR_H_80F70DE188054EBFA1C3979C01099475

const int ARR_EMPTY = -1;

/**
 * A simple array class template.
 * This template offers basic array class methods.
 * @note This class is limited in 32-bit space for item count.
 */
template<class T> class SimpleArray
{
public:
	SimpleArray();
	SimpleArray(const T* ptArray, int upbound, int size);
	SimpleArray(int nNewSize, int nGrowBy = 1);
	SimpleArray(const SimpleArray& spaArg);
	virtual ~SimpleArray();

	operator T*()
	{
		return m_pT;
	}
	operator const T*() const
	{
		return m_pT;
	}

	bool InsertAt(int nIndex, T argT, int nCount = 1);
	void InsertAtRef(int nIndex, const T& argT, int nCount = 1);
	bool InsertAtGrow(int nIndex, T argT, int nCount = 1);
	bool InsertAtGrow (int nIndex, const T* pT, int nSrcIndex, int nCount);
	void InsertAtGrowRef(int nIndex, const T& argT, int nCount = 1);
	bool RemoveAt(int nIndex, int nCount = 1);
	void SetAtGrow(int nIndex, T argT);
	bool SetAtGrowRef(int nIndex, const T& argT);
	void SetAt(int nIndex, T argT);
	void SetAtRef(int nIndex, const T& argT);
	T GetAt(int nIndex) const;
	T& GetRefAt(int nIndex) const;
	int GetSize() const;
	int GetUpperBound() const;
	int GetLength() const;
	int GetGrowBy() const;
	bool SetSize(int nNewSize, int nGrowBy = 0);
	void SetGrowBy(int nGrowBy);
	T& operator[](int nIndex) {return m_pT[nIndex];}
	T& operator[](unsigned nIndex) {return m_pT[nIndex];}
	SimpleArray<T>& operator=(SimpleArray<T>& spa);
	void ClearAll();
	bool blContainsRef(const T& argT);
	bool blContains(T argT);
	int nContainsAt(T argT);
	bool blIsEmpty() const;
	void AppendRef(const T& argT);
	void Append(T argT);
	void Exchange(int nIndex1, int nIndex2);
	bool blCompare(SimpleArray<T>& spa) const;
	int operator==(SimpleArray<T>& spa);
	int operator!=(SimpleArray<T>& spa);
	bool Adopt(T* ptArray, int upbound, int size);
	void SetUpperBound(int upbnd);
	bool AppendArray( T* pSrc, int srclen );
	void ExpandToSize();
	bool CopyFrom(int index, const T* pSrc, int srclen);
	bool Replace(int ToReplaceIndex, int ToReplaceLength, const T* pReplaceWith, int ReplaceWithLength);

protected:
	bool AddSpace(int nExtend);
	T* m_pT;
	int m_nSize;
	int m_nUpperBound;
	int m_nGrowBy;
};

// The template implementation methods must be included to the header file.
// Otherwise there will be link errors.
#include "Simparr_imp.h"

/**
 * @brief A string class.
 */
class SimpleString : public SimpleArray<TCHAR>
{
public:
	SimpleString operator+(const SimpleString& str1);
	SimpleString();
	SimpleString(LPCTSTR ps);

	bool IsEmpty() const;
	int StrLen() const;
	int AppendString(LPCTSTR ps);
	int SetToString(LPCTSTR ps);
	void Clear();

	SimpleString& operator=(LPCTSTR ps);
	SimpleString& operator=(const SimpleString &str);
	SimpleString& operator+=(LPCTSTR ps);
};

SimpleString operator+(const SimpleString &ps1, LPCTSTR ps2);
SimpleString operator+(LPCTSTR ps1, const SimpleString &ps2);

#endif
