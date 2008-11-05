/** 
 * @file  Simparr_imp.h
 *
 * @brief Template class implementation file.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray()
{
	m_pT = NULL;
	m_nSize = 0;
	m_nUpperBound = ARR_EMPTY;
	m_nGrowBy = 1;
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(int nNewSize, int nGrowBy)
	: m_nGrowBy(nGrowBy), m_nUpperBound(ARR_EMPTY), m_nSize(nNewSize)
{
	m_pT = new T[m_nSize];
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(const T* ptArray, int upbound, int size)
	: m_nGrowBy(1), m_pT(ptArray), m_nUpperBound(upbound), m_nSize(size)
{
#ifdef _DEBUG
	if(m_pT == NULL || m_nUpperBound<0 || m_nSize<=0 || m_nUpperBound >= m_nSize)
	{
		m_nSize = 0;
		m_nUpperBound = ARR_EMPTY;
	}
#endif
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::SimpleArray(const SimpleArray& spaArg)
	: m_nSize(spaArg.m_nSize), m_nUpperBound(spaArg.m_nUpperBound),
	m_nGrowBy(spaArg.m_nGrowBy)
{
	m_pT = new T[m_nSize];
	int k;
	for(k = 0; k <= m_nUpperBound; k++)
		m_pT[k] = spaArg.m_pT[k];
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::~SimpleArray()
{
	if(m_pT != NULL) delete [] m_pT;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::InsertAtGrow(int nIndex, T argT, int nCount)
{
	if(nIndex<0 || nCount<1)
		return FALSE;
	int i;
	if(nIndex > m_nUpperBound)
	{
		for(i = 0; i < nCount; i++)
			SetAtGrow(nIndex + i, argT);
		return TRUE;
	}
	else
	{
		if(m_nSize < m_nUpperBound + 1 + nCount)
		{
			if (AddSpace(nCount) == FALSE)
				return FALSE;
		}
		for(i = m_nUpperBound + nCount; i > nIndex; i--)
		{
			m_pT[i] = m_pT[i-nCount];
		}
		for(i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
		return TRUE;
	}
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::InsertAtGrow(int nIndex, const T* pT, int nSrcIndex, int nCount)
{
	if(nIndex<0 || nCount<1)
		return FALSE;
	int i;
	if(nIndex > m_nUpperBound)
	{
		for(i = 0; i < nCount; i++)
		{
			if (SetAtGrowRef(nIndex + i, pT[nSrcIndex+i]) == FALSE)
				return FALSE;
		}
		return TRUE;
	}
	else
	{
		if(m_nSize < m_nUpperBound + 1 + nCount)
		{
			if (AddSpace(nCount) == FALSE)
				return FALSE;
		}
		for(i = m_nUpperBound + nCount; i > nIndex; i--)
		{
			m_pT[i] = m_pT[i-nCount];
		}
		for(i = 0; i < nCount; i++)
			m_pT[nIndex + i] = pT[nSrcIndex+i];
		m_nUpperBound += nCount;
		return TRUE;
	}
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::InsertAtGrowRef(int nIndex, const T& argT, int nCount)
{
	if(nIndex<0 || nCount<1) return;
	int i;
	if(nIndex > m_nUpperBound)
	{
		for(i = 0; i < nCount; i++)
			SetAtGrowRef(nIndex + i, argT);
		return;
	}
	else
	{
		if(m_nSize < m_nUpperBound + 1 + nCount)
			AddSpace(nCount);
		for(i = m_nUpperBound + nCount; i > nIndex; i--)
		{
			m_pT[i] = m_pT[i-nCount];
		}
		for(i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
	}
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::InsertAt(int nIndex, T argT, int nCount)
{
	// Valid index?
	if( nIndex < 0 || nIndex > m_nUpperBound )
		return FALSE;

	int i;
	// Is there enough space after m_nUpperBound for inserting?
	if( ( m_nSize - 1 ) - m_nUpperBound >= nCount )
	{
		// Yes, no need to allocate more memory.
		// Push up the elements at the current position.
		for( i = m_nUpperBound + nCount; i >= nIndex + nCount; i-- )
			m_pT[ i ] = m_pT[ i - nCount ];
		// Copy in the new data.
		for( i = 0; i < nCount; i++ )
			m_pT[ nIndex + i ] = argT;
		// Adjust m_nUpperBound to new size.
		m_nUpperBound += nCount;
	}
	else
	{
		// No, need to allocate more memory.
		for (i = m_nSize - 1; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		if (m_nSize - nIndex < nCount)
			nCount = m_nSize - nIndex;
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound = m_nSize - 1;
	}
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::InsertAtRef(int nIndex, const T& argT, int nCount)
{
	if(nIndex < 0 || nIndex > m_nUpperBound)
		return;
	int i;
	if ((m_nSize - 1) - m_nUpperBound >= nCount)
	{
		for(i = m_nUpperBound + nCount; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		for(i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound += nCount;
	}
	else
	{
		for (i = m_nSize - 1; i >= nIndex + nCount; i--)
			m_pT[i] = m_pT[i - nCount];
		if (m_nSize - nIndex < nCount)
			nCount = m_nSize - nIndex;
		for (i = 0; i < nCount; i++)
			m_pT[nIndex + i] = argT;
		m_nUpperBound = m_nSize - 1;
	}
}

//-------------------------------------------------------------------
template<class T> SimpleArray<T>::RemoveAt(int nIndex, int nCount)
{
	if(nIndex < 0 || nIndex > m_nUpperBound || nCount < 1) return FALSE;
	if(nCount > m_nUpperBound - nIndex)
	{
		m_nUpperBound = nIndex - 1;
		return TRUE;
	}
	int i;
	for(i = nIndex; i <= m_nUpperBound - nCount; i++)
	{
		m_pT[i] = m_pT[i + nCount];
	}
	m_nUpperBound -= nCount;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAtGrow(int nIndex, T argT)
{
	if(nIndex < 0) return;
	if(nIndex > m_nSize - 1)
		AddSpace(nIndex - m_nSize + 1);
	m_pT[nIndex] = argT;
	if(nIndex > m_nUpperBound) m_nUpperBound = nIndex;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::SetAtGrowRef(int nIndex, const T& argT)
{
	if(nIndex < 0)
		return FALSE;
	if(nIndex > m_nSize - 1)
	{
		if (AddSpace(nIndex - m_nSize + 1) == FALSE)
			return FALSE;
	}
	m_pT[nIndex] = argT;
	if(nIndex > m_nUpperBound)
		m_nUpperBound = nIndex;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAt(int nIndex, T argT)
{
	if(nIndex >= 0 && nIndex < m_nSize)
	{
		m_pT[nIndex] = argT;
		if(nIndex > m_nUpperBound)
			m_nUpperBound = nIndex;
	}
	else
		return;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetAtRef(int nIndex, const T& argT)
{
	if(nIndex >= 0 && nIndex < m_nSize)
	{
		m_pT[nIndex] = argT;
		if(nIndex > m_nUpperBound)
			m_nUpperBound = nIndex;
	}
	else
		return;
}

//-------------------------------------------------------------------
template<class T> T SimpleArray<T>::GetAt(int nIndex) const
{
	return m_pT[nIndex];
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::AddSpace(int nExtend)
{
	int newsize = m_nSize + (((nExtend-1) / m_nGrowBy) + 1) * m_nGrowBy;
	T* pT = new T[newsize];
	if (pT != NULL)
	{
		int i;
		for(i = 0; i < m_nSize; i++)
			pT[i] = m_pT[i];
		if(m_pT != NULL)
			delete [] m_pT;
		m_pT = pT;
		m_nSize = newsize;
		return TRUE;
	}
	else
		return FALSE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetGrowBy() const
{
	return m_nGrowBy;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetSize() const
{
	return m_nSize;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetUpperBound() const
{
	return m_nUpperBound;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::GetLength() const
{
	return m_nUpperBound+1;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::SetSize(int nNewSize, int nGrowBy )
{
	if(nNewSize < 0)
		return FALSE;
	if (nNewSize == m_nSize)
		return TRUE;
	if( nNewSize == 0 )
	{
		ClearAll();
		return TRUE;
	}

	T* pT = new T[nNewSize];
	if (pT == NULL)
		return FALSE;
	int i;
	if(m_nUpperBound < nNewSize)
	{
		for(i = 0; i <= m_nUpperBound; i++)
			pT[i] = m_pT[i];
	}
	else
	{
		for(i = 0; i < nNewSize; i++)
			pT[i] = m_pT[i];
		m_nUpperBound = nNewSize - 1;
	}

	if(m_pT != NULL)
		delete [] m_pT;
	m_pT = pT;
	m_nSize = nNewSize;
	if(nGrowBy > 0)
		m_nGrowBy = nGrowBy;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetGrowBy(int nGrowBy)
{
	if(nGrowBy > 0) m_nGrowBy = nGrowBy;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::ClearAll()
{
	if(m_pT != NULL)
		delete [] m_pT;
	m_pT = NULL;
	m_nSize = 0;
	m_nUpperBound = ARR_EMPTY;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::blContainsRef(const T& argT)
{
	int i;
	for(i = 0; i <= m_nUpperBound; i++)
		if(argT == m_pT[i])
			return TRUE;
	return FALSE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::blContains(T argT)
{
	int i;
	for(i = 0; i <= m_nUpperBound; i++)
		if(argT == m_pT[i])
			return TRUE;
	return FALSE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::nContainsAt(T argT)
{
	int i;
	for(i = 0; i <= m_nUpperBound; i++)
		if(argT == m_pT[i])
			return i;
	return ARR_EMPTY;
}

//-------------------------------------------------------------------
// Make a copy of the other SimpleArray.
template<class T> SimpleArray<T>& SimpleArray<T>::operator=(SimpleArray<T>& spa)
{
	// Can't assign to itself: "sa1 = sa1;" not allowed.
	if( &spa != this )
	{
		// If this array is not empty then delete it.
		ClearAll();
		// Allocate memory.
		int nSize = spa.m_nUpperBound + 1;
		m_pT = new T[ nSize ];
		// Copy the valid elements.
		if( m_pT != NULL )
		{
			// This array now is just large enough to contain the valid elements of spa.
			m_nUpperBound = spa.m_nUpperBound;
			m_nSize = nSize;
			// GrowBy rate is also copied.
			m_nGrowBy = spa.m_nGrowBy;
			int k;
			for(k = 0; k <= m_nUpperBound; k++)
				m_pT[k] = spa.m_pT[k];
		}
		// If no memory could be allocated, then this array remains empty.
	}
	return *this;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::AppendRef(const T& argT)
{
	SetAt(m_nUpperBound+1, argT);
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::Append(T argT)
{
	SetAtGrow(m_nUpperBound+1, argT);
}

//-------------------------------------------------------------------
template<class T> T& SimpleArray<T>::GetRefAt(int nIndex) const
{
	return m_pT[nIndex];
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::blCompare(SimpleArray<T>& spa) const
{
	if(m_nUpperBound != spa.GetUpperBound() ) return FALSE;
	int k;
	for(k = 0; k <= m_nUpperBound; k++)
	{
		if(m_pT[k] != spa[k]) return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::operator==(SimpleArray<T>& spa)
{
	return blCompare(spa);
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::operator!=(SimpleArray<T>& spa)
{
	return !blCompare(spa);
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::blIsEmpty() const
{
	return (GetUpperBound() < 0) ? TRUE : FALSE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::Exchange(int nIndex1, int nIndex2)
{
	T temp(GetRefAt(nIndex2));
	GetRefAt(nIndex2) = GetRefAt(nIndex1);
	GetRefAt(nIndex1) = temp;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::Adopt(T* ptArray, int upbound, int size)
{
#ifdef _DEBUG
	if(ptArray == NULL || upbound<0 || size<=0 || upbound >= size)
		return FALSE;
#endif
	if(m_pT!=NULL)
		delete [] m_pT;
	m_pT = ptArray;
	m_nSize = size;
	m_nUpperBound = upbound;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> void SimpleArray<T>::SetUpperBound(int upbnd)
{
	if(upbnd < m_nSize)
		m_nUpperBound = upbnd;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::AppendArray(T* pSrc, int srclen)
{
	if( srclen <= 0 )
		return FALSE;

	if( m_nUpperBound + 1 + srclen > m_nSize )
	{
		// Not enough space, so get some.
		if( !AddSpace( srclen ) )
			return FALSE;
	}
	// Enough space to append without growing. Copy the data.
	int i;
	for( i=0; i<srclen; i++ )
	{
		m_pT[ m_nUpperBound + 1 + i ] = pSrc[i];
	}
	m_nUpperBound += srclen;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::ExpandToSize()
{
	m_nUpperBound = m_nSize - 1;
	return TRUE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::CopyFrom(int index, const T* pSrc, int srclen)
{
	if( m_nSize - index >= srclen )
	{
		// Enough space to copy into.
		int i;
		for( i = 0; i < srclen; i++ )
			m_pT[ index + i ] = pSrc[ i ];
		if( index + srclen - 1 > m_nUpperBound )
			m_nUpperBound = index + srclen - 1;
		return TRUE;
	}
	else
		return FALSE;
}

//-------------------------------------------------------------------
template<class T> int SimpleArray<T>::Replace(int ToReplaceIndex, int ToReplaceLength, const T* pReplaceWith, int ReplaceWithLength)
{
	if( m_pT != NULL && ToReplaceLength > 0 )
	{
		// Number of elements from start to end of array large enough for request?
		if( m_nUpperBound - ToReplaceIndex + 1 >= ToReplaceLength )
		{
			if( ToReplaceLength < ReplaceWithLength )
			{
				int i;
				T dummy;

				// Next line might cause problems if used with non-pure-binary
				// objects.
				dummy = 0;

				InsertAtGrow( ToReplaceIndex, dummy, ReplaceWithLength - ToReplaceLength );
				for( i = 0; i < ReplaceWithLength; i++ )
				{
					m_pT[ ToReplaceIndex + i ] = pReplaceWith[ i ];
				}

				return TRUE;
			}
			else if( ToReplaceLength == ReplaceWithLength )
			{
				int i;
				for( i = 0; i < ReplaceWithLength; i++ )
				{
					m_pT[ ToReplaceIndex + i ] = pReplaceWith[ i ];
				}
				return TRUE;
			}
			else // if( ToReplaceLength > ReplaceWithLength )
			{
				int i;
				for( i = 0; i < ReplaceWithLength; i++ )
				{
					m_pT[ ToReplaceIndex + i ] = pReplaceWith[ i ];
				}

				RemoveAt( ToReplaceIndex + ReplaceWithLength, ToReplaceLength - ReplaceWithLength );
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
