//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\THist.h@@\main\3
//          Version: \main\branch\5
//
//         Language: C
//
//      Description: Definition of template class THist
//          Structs: 
//
//  -----------------------------------------------------------------
#ifndef __THIST_H
#define __THIST_H

//  Definition of template class TArray
#include "Array.h"

//  Definition of fill,copy,...
#include "Algorithms.h"

#include <iostream.h>

//  -----------------------------------------------------------------
//  Defintion of template class TFLHist
//  -----------------------------------------------------------------

struct THistHeader
{
    //  The index of the first channel with number of
    //  entries greater than zero.
    long  m_lPosBegin;

    //  The index just right of the last channel with
    //  number of entries greater than zero.
    long  m_lPosEnd;

    //  The resolution of the floating point values.
    //  2^(lResExp).
    //  2^(-lResExp) is equal to the displacement difference 
    //  between adjacent channels
    long  m_lResExp;

    //  Discretized value of the channel at position
    //  m_lPosBegin
    long  m_lDiscrValBegin;

};

template <class FLOAT_TYPE, class CHA_TYPE>
class TFLHist
{
    //  The type realizes a histogram for floating point values.
    //  CHA_TYPE must be an integer.
    typedef TFLHist<FLOAT_TYPE,CHA_TYPE>  MYTYPE;
protected:
    friend class const_iterator;

    //  Header data
    THistHeader*    m_psHeader;

    //  Channel data
    TArray<CHA_TYPE> m_aiChannel;

public:

    //  Returns discretized value of argument
    static long discretize(long lResExp, const FLOAT_TYPE& rflVal)
    {
        return IMULT(rflVal*(lResExp < 0 ? raise2To(-lResExp) : 1./raise2To(lResExp)),1L);
    }

    //  Retrieves external value from channel number.
    static FLOAT_TYPE undiscretize(long lResExp, long lDiscrVal)
    {
        return lResExp < 0 
            ? static_cast<FLOAT_TYPE>(lDiscrVal)/raise2To(-lResExp)
            : static_cast<FLOAT_TYPE>(lDiscrVal)*raise2To(lResExp);
    }


    class const_iterator
    {
        //  The type describes an object that can be used to
        //  iterate through the channels of the histogram
    protected:
        friend class MYTYPE;
        const MYTYPE*  m_pHist;     
        long  m_lPos;

        //  Preserved constructor
        const_iterator(const MYTYPE* pHist, long lPos)
            : m_pHist(pHist)
            , m_lPos(lPos)
        {}

    public:

        //  Default constructor.
        //  Creates an invalid iterator.
        const_iterator()
            : m_pHist(NULL)
            , m_lPos(0)
        {}

        //  Copy constructor
        const_iterator(const const_iterator& rRHS)
            : m_pHist(rRHS.m_pHist)
            , m_lPos(rRHS.m_lPos)
        {}

        //  Asignment operator
        const_iterator& operator=(const const_iterator& rRHS)
        {
            if(&rRHS != this)
            {
                m_pHist = rRHS.m_pHist;
                m_lPos  = rRHS.m_lPos;
            }
            return *this;
        }

        //  If the member function returns false the
        //  iterator must not be dereferenced.
        bool valid() const
        {
            if(m_pHist == NULL || m_pHist->m_psHeader == NULL)
            {
                return false;
            }
            if( m_pHist->m_psHeader->m_lPosBegin <= m_pHist->m_psHeader->m_lPosEnd )
            {
                //  +----+----------+----+
                //  |    XXXXXXXXXXX     |
                //  +----+----------+----+
                //     Begin       End 
                return m_lPos >= m_pHist->m_psHeader->m_lPosBegin && m_lPos < m_pHist->m_psHeader->m_lPosEnd;
            }
            else
            {
                //  +---+-----+----------+
                //  |XXX      XXXXXXXXXXX|
                //  +---+-----+----------+
                //     End  Begin
                return (m_lPos >= 0 && m_lPos < m_pHist->m_psHeader->m_lPosEnd) 
                    || (m_lPos >= m_pHist->m_psHeader->m_lPosBegin && m_lPos < m_pHist->m_aiChannel.size());
            }
        }

        //	Postfix increment operator.
        const_iterator& operator++()
        {
            if( m_pHist && m_pHist->m_psHeader )
            {
                if( m_pHist->m_psHeader->m_lPosBegin <= m_pHist->m_psHeader->m_lPosEnd )
                {
                    ++m_lPos;
                }
                else
                {
                    if(++m_lPos == m_pHist->m_aiChannel.size())
                    {
                        m_lPos = 0L;
                    }
                }
            }
            return *this;
        }

        //  Prefix increment operator
        const const_iterator operator ++ (int )
        {
            const_iterator sTmp(*this);
            ++*this;
            return sTmp;
        }

        //	Postfix decrement operator.
        const_iterator& operator--()
        {
            if( m_pHist && m_pHist->m_psHeader )
            {
                if( m_pHist->m_psHeader->m_lPosBegin <= m_pHist->m_psHeader->m_lPosEnd )
                {
                    --m_lPos;
                }
                else
                {
                    if(m_lPos == 0L)
                    {
                        m_lPos = m_pHist->m_aiChannel.size();
                    }
                    --m_lPos;
                }
            }
            return *this;
        }

        //	Prefix decrement operator.
        const const_iterator operator -- (int )
        {
            const_iterator sTmp(*this);
            --*this;
            return sTmp;
        }

        //  Equality operator.
        bool operator==(const const_iterator& rRHS) const
        {
            return (m_lPos == rRHS.m_lPos && m_pHist == rRHS.m_pHist);
        }

        //  Unequal to operator.
        bool operator!=(const const_iterator& rRHS) const
        {
            return !(*this == rRHS);
        }

        //	Dereference operator.
        //  Returns number of entries of current channel
        const CHA_TYPE& operator*() const
        {
            if( !valid() )
            {
                LOCATE_ERROR(__FILE__,__LINE__);
                throw;
            }
            return m_pHist->m_aiChannel[m_lPos];
        }

        //  Returns discretized value of the channel
        //  the iterator points to
        long discrValue() const
        {
            if( !valid() ) return 0L;
            if( m_pHist->m_psHeader->m_lPosBegin <= m_pHist->m_psHeader->m_lPosEnd || m_lPos >= m_pHist->m_psHeader->m_lPosBegin )
            {
                return m_pHist->m_psHeader->m_lDiscrValBegin+(m_lPos-m_pHist->m_psHeader->m_lPosBegin);
            }
            else
            {
                //  +---+-----+----------+
                //  |XXX      XXXXXXXXXXX|
                //  +---+-----+----------+
                //     End  Begin
                return m_pHist->m_psHeader->m_lDiscrValBegin+(m_lPos+m_pHist->m_aiChannel.size()-m_pHist->m_psHeader->m_lPosBegin);
            }
        }

        long resExp() const
        {
            return valid() ? m_pHist->m_psHeader->m_lResExp : 0L;
        }


        //  Returns floating point value of the channel
        //  the iterator points to.
        //  Note: The iterater must be valid!!
        FLOAT_TYPE value() const
        {
            return m_pHist->undiscretize(valid() ? m_pHist->m_psHeader->m_lResExp : 0L,discrValue()); 
        }

    
        //  Difference operator
        int operator-(const const_iterator& rRHS)
        {
            if( m_pHist != rRHS.m_pHist )
            {
                //  uncomparable
                return INT_MAX;
            }
            if( m_pHist->m_psHeader->m_lPosBegin <= m_pHist->m_psHeader->m_lPosEnd )
            {
                return m_lPos-rRHS.m_lPos;
            }
            else
            {
                //  +---+-----+----------+
                //  |XXX      XXXXXXXXXXX|
                //  +---+-----+----------+
                //     End  Begin
                return (m_lPos >= m_pHist->m_psHeader->m_lPosBegin 
                    ? m_lPos-m_pHist->m_psHeader->m_lPosBegin 
                    : m_lPos+m_pHist->m_aiChannel.size()-m_pHist->m_psHeader->m_lPosBegin
                    )
                    -
                    (rRHS.m_lPos >= rRHS.m_pHist->m_psHeader->m_lPosBegin 
                    ? rRHS.m_lPos-m_pHist->m_psHeader->m_lPosBegin 
                    : rRHS.m_lPos+rRHS.m_pHist->m_aiChannel.size()-rRHS.m_pHist->m_psHeader->m_lPosBegin
                    );
            }
        }

    };

    friend class const_iterator;

    //  Default constructor
    TFLHist();

    //  Initialization with preallocated data. The object will
    //  not get the owner of the data. The client is responsible
    //  for deallocation of the data passed. Resize, reserve will
    //  fail if the desired new size is greater than the initial size
    //  lAllocSize passed.
    //  The initial size of the array is equal to lAllocSize.
    TFLHist(THistHeader* pHeader, CHA_TYPE* pData, long lAllocSize);

    //  Destructor
    ~TFLHist();

    //  The member function replaces the histogram by a copy
    //  off rRHS.
    bool assign(const_iterator itBegin, const_iterator itEnd);
    bool assign(const_iterator itBegin, const_iterator itEnd, long lDstResExp);


    //  Sets the size of the internal buffer.
    //  Invalidate previous content and any previously returned
    //  iterator.
    //  flMin, flMax are the minimum/maximum value that will be 
    //  passed to add.
    bool reserve(long lCha, long lResExp);
    bool reserve(FLOAT_TYPE flMin, FLOAT_TYPE flMax, long lResExp);

    //  The member function returns true for an empty controlled
    //  sequence.
    bool empty() const;

    //  Returns the maximum number of channels that can be stored.
    long capacity() const;


    //  The meber function calculates the total number of entries.
    CHA_TYPE entries() const;


	//  The member function returns an iterator that points
	//  at the first non-empty channel of the histogram
    //  (or just beyond the end of an empty sequence).
	const_iterator begin() const
    {
        return const_iterator(this,m_psHeader ? m_psHeader->m_lPosBegin : 0L);
    }

	//  The member function returns an iterator that points
	//  just beyond the first empty channel beyond the range
    //  OF VALID CHANNELS.
	const_iterator end() const
    {
        return const_iterator(this,m_psHeader ? m_psHeader->m_lPosEnd : 0L);
    }

    //  The member function calculates the median from
    //  the stored values.
    bool median(FLOAT_TYPE& rflMedian) const;
    bool median(long& rlDiscrMedian) const;

    //  Adds a new entry to the histogram.
    bool add(const FLOAT_TYPE& rflVal, CHA_TYPE iCount = 1);

    //  Removes an entry added in a preceding call to add.
    bool remove(const FLOAT_TYPE& rflVal);

    //  Restores state after reserve. 
    void clear();

    //  Plots the histogram between [itBegin, itEnd[ in gnuplot compatiple format
    bool plot(ostream& rDst, const_iterator itBegin, const_iterator itEnd) const;
};

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::add(const FLOAT_TYPE& rflVal, CHA_TYPE iCount)
{
    if(m_psHeader == NULL) return false;
    const long lDiscrVal = discretize(m_psHeader->m_lResExp,rflVal);
    if( m_psHeader->m_lPosBegin == m_psHeader->m_lPosEnd )
    {
        //  Empty histogram
        //  +--------------------+
        //  |                    |
        //  +--------------------+
        //                     Begin
        //                      End
        m_psHeader->m_lPosBegin              = m_aiChannel.size()/2L;
        m_psHeader->m_lPosEnd                = m_psHeader->m_lPosBegin+1;
        m_psHeader->m_lDiscrValBegin         = lDiscrVal; 
        m_aiChannel[m_psHeader->m_lPosBegin] = iCount;
        return true;
    }
    //  Assign position to new val
    long lPos   = (lDiscrVal-m_psHeader->m_lDiscrValBegin)+m_psHeader->m_lPosBegin;
    if( m_psHeader->m_lPosBegin > m_psHeader->m_lPosEnd )
    {
        //  +---+-----+----------+
        //  |XXX      XXXXXXXXXXX|
        //  +---+-----+----------+
        //     End  Begin

        if( lPos <= m_psHeader->m_lPosEnd )
        {
            //  Spanning
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }
        if( lPos >= m_aiChannel.size() )
        {
            lPos -= m_aiChannel.size();
            if( lPos >= m_psHeader->m_lPosBegin-1L )
            {
                //  Spanning
                LOCATE_ERROR(__FILE__,__LINE__);
                return false;
            }
            m_aiChannel[lPos] += iCount;
            if( lPos >= m_psHeader->m_lPosEnd )
            {
                m_psHeader->m_lPosEnd = lPos+1L;
            }
            return true;
        }
        m_aiChannel[lPos] += iCount;
        if(lPos < m_psHeader->m_lPosBegin)
        {
            m_psHeader->m_lPosBegin      = lPos;
            m_psHeader->m_lDiscrValBegin = lDiscrVal; 
        }
        return true;
    }
    
    //  +----+----------+----+
    //  |    XXXXXXXXXXX     |
    //  +----+----------+----+
    //     Begin       End 
    if( lPos < 0 )
    {
        lPos += m_aiChannel.size();
        if( lPos <= m_psHeader->m_lPosEnd )
        {
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }
        m_aiChannel[lPos] += iCount;
        m_psHeader->m_lPosBegin      = lPos;
        m_psHeader->m_lDiscrValBegin = lDiscrVal;
        return true;
    }
    if( lPos >= m_aiChannel.size() )
    {
        lPos -= m_aiChannel.size();
        if( lPos >= m_psHeader->m_lPosBegin-1L )
        {
            //  Spanning
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }
        m_aiChannel[lPos] += iCount;
        m_psHeader->m_lPosEnd = lPos+1L;
        return true;
    }
    m_aiChannel[lPos] += iCount;
    if( lPos < m_psHeader->m_lPosBegin )
    {
        m_psHeader->m_lPosBegin      = lPos;
        m_psHeader->m_lDiscrValBegin = lDiscrVal;
    }
    else if( lPos >= m_psHeader->m_lPosEnd )
    {
        m_psHeader->m_lPosEnd = lPos+1;
    }
    return true;
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::remove(const FLOAT_TYPE& rflVal)
{
    //  Discretization
    if( m_psHeader == NULL || m_psHeader->m_lPosBegin == m_psHeader->m_lPosEnd )
    {
        //  Cannont remove a value from an empty histogram
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    const long lDiscrVal = discretize(m_psHeader->m_lResExp,rflVal);
    long lPos   = (lDiscrVal-m_psHeader->m_lDiscrValBegin)+m_psHeader->m_lPosBegin;

    if( m_psHeader->m_lPosBegin >= m_psHeader->m_lPosEnd )
    {
        //  +---+-----+----------+
        //  |XXX      XXXXXXXXXXX|
        //  +---+-----+----------+
        //     End  Begin

        if( lPos >= m_aiChannel.size() )
        {
            lPos -= m_aiChannel.size();
        }
        if( lPos < 0L || lPos >= m_aiChannel.size() || (lPos >= m_psHeader->m_lPosEnd && lPos < m_psHeader->m_lPosBegin) )
        {
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }

        if( --m_aiChannel[lPos] > 0 )
        {
            return true;
        }
        if( lPos == m_psHeader->m_lPosBegin )
        {
            while( m_psHeader->m_lPosBegin != m_psHeader->m_lPosEnd && m_aiChannel[m_psHeader->m_lPosBegin] == 0 )
            {
                if(++m_psHeader->m_lPosBegin == m_aiChannel.size())
                {
                    m_psHeader->m_lPosBegin = 0L;
                }
                ++m_psHeader->m_lDiscrValBegin;
            }
            return true;
        }
        if( lPos == m_psHeader->m_lPosEnd-1L )
        {
            while( m_psHeader->m_lPosBegin != m_psHeader->m_lPosEnd && m_aiChannel[m_psHeader->m_lPosEnd-1L] == 0 )
            {
                if(--m_psHeader->m_lPosEnd == 0L)
                {
                    m_psHeader->m_lPosEnd = m_aiChannel.size();
                }
            }
            return true;
        }
        return true;
    }
    
    //  +----+----------+----+
    //  |    XXXXXXXXXXX     |
    //  +----+----------+----+
    //     Begin       End 
    if( lPos < m_psHeader->m_lPosBegin || lPos >= m_psHeader->m_lPosEnd )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    if( --m_aiChannel[lPos] > 0 )
    {
        return true;
    }
    if( lPos == m_psHeader->m_lPosBegin )
    {
        while( m_psHeader->m_lPosBegin != m_psHeader->m_lPosEnd && m_aiChannel[m_psHeader->m_lPosBegin] == 0 )
        {
            ++m_psHeader->m_lPosBegin;
            ++m_psHeader->m_lDiscrValBegin;
        }
        return true;
    }
    if( lPos == m_psHeader->m_lPosEnd-1L )
    {
        while( m_psHeader->m_lPosBegin != m_psHeader->m_lPosEnd && m_aiChannel[m_psHeader->m_lPosEnd-1L] == 0 )
        {
            --m_psHeader->m_lPosEnd;
        }
        return true;
    }
    return true;
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::empty() const
{
    return m_psHeader == NULL || m_psHeader->m_lPosEnd == m_psHeader->m_lPosBegin;
}

template <class FLOAT_TYPE, class CHA_TYPE>
long TFLHist<FLOAT_TYPE,CHA_TYPE>::capacity() const
{
    return m_aiChannel.size()-1L;
}


template <class FLOAT_TYPE, class CHA_TYPE>
void TFLHist<FLOAT_TYPE,CHA_TYPE>::clear()
{
    if(m_psHeader != 0)
    {
        m_psHeader->m_lPosEnd = m_psHeader->m_lPosBegin = m_aiChannel.size();
        m_psHeader->m_lDiscrValBegin = 0L;
    }
    fill(m_aiChannel.begin(),m_aiChannel.end(),0);
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::reserve(long lCha, long lResExp)
{
    if( lCha < 1L || lResExp < -31 || lResExp > 31 )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    if( m_psHeader == NULL )
    {
        m_psHeader = new THistHeader;
        if( m_psHeader == NULL )
        {
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }
    }
    m_psHeader->m_lResExp  = lResExp;
    if( !m_aiChannel.resize(lCha+1L) )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    clear();
    return true;
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::reserve(FLOAT_TYPE flMin, FLOAT_TYPE flMax, long lResExp)
{
    if( flMin > flMax )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    return reserve(1L+discretize(lResExp,flMax)-discretize(lResExp,flMin),lResExp);
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::assign(const_iterator itBegin, const_iterator itEnd)
{
    if( itBegin != itEnd )
    {
        if( !reserve(static_cast<long>(itEnd-itBegin),itBegin.resExp()) )
        {
            LOCATE_ERROR(__FILE__,__LINE__);
            return false;
        }
        copy(m_aiChannel.begin(),itBegin,itEnd);
        m_psHeader->m_lPosBegin      = 0;
        m_psHeader->m_lPosEnd        = itEnd-itBegin;
        m_psHeader->m_lResExp        = itBegin.resExp();
        m_psHeader->m_lDiscrValBegin = itBegin.discrValue();
    }
    else
    {
        clear();
    }
    return true;
}


template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::assign(const_iterator itBegin, const_iterator itEnd, long lDstResExp)
{
    if( itBegin != itEnd )
    {
        if( lDstResExp != itBegin.resExp() )
        {
            const_iterator itBack = itEnd;
            --itBack;
            if( !reserve(itBegin.value(),itBack.value(),lDstResExp) )
            {
                LOCATE_ERROR(__FILE__,__LINE__);
                return false;
            }
            while(itBegin != itEnd)
            {
                if( *itBegin > 0 && !add(itBegin.value(),*itBegin) )
                {
                    LOCATE_ERROR(__FILE__,__LINE__);
                    return false;
                }
                ++itBegin;
            }
        }
        else
        {
            if( !reserve(static_cast<long>(itEnd-itBegin),itBegin.resExp()) )
            {
                LOCATE_ERROR(__FILE__,__LINE__);
                return false;
            }
            copy(m_aiChannel.begin(),itBegin,itEnd);
            m_psHeader->m_lPosBegin      = 0;
            m_psHeader->m_lPosEnd        = m_aiChannel.size();
            m_psHeader->m_lResExp        = itBegin.resExp();
            m_psHeader->m_lDiscrValBegin = itBegin.discrValue();
        }
    }
    else
    {
        clear();
        if(m_psHeader) m_psHeader->m_lResExp = lDstResExp;
    }
    return true;
}



template <class FLOAT_TYPE, class CHA_TYPE>
CHA_TYPE TFLHist<FLOAT_TYPE,CHA_TYPE>::entries() const
{
    //  The following implemenation is in general faster than
    // return accumlate(begin(),end());
    if( empty() ) 
    {
        return 0;
    }
    //  Note: If m_psHeader is NULL empty returns true
    if( m_psHeader->m_lPosBegin < m_psHeader->m_lPosEnd )
    {
        return accumulate(m_aiChannel.begin()+m_psHeader->m_lPosBegin,m_aiChannel.begin()+m_psHeader->m_lPosEnd,0);
    }
    return accumulate(m_aiChannel.begin()            ,m_aiChannel.begin()+m_psHeader->m_lPosEnd,CHA_TYPE(0))
        +  accumulate(m_aiChannel.begin()+m_psHeader->m_lPosBegin,m_aiChannel.end()            ,CHA_TYPE(0));
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::median(FLOAT_TYPE& rflMedian) const
{
    if( empty() )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    //  Note: If m_psHeader is NULL empty returns true
    const_iterator itLeft = begin();
    const_iterator itEnd  = end();
    int iNRight        = entries() - *itLeft;
    int iNLeft         = *itLeft;
    long lDiscrValLeft = m_psHeader->m_lDiscrValBegin;

    while(iNLeft < iNRight)
    {
        ++itLeft;
        ++lDiscrValLeft;
        iNLeft  += *itLeft;
        iNRight -= *itLeft;
    }
    if( iNLeft == iNRight )
    {
        const_iterator itRight = itLeft;
        ++itRight;
        long lDiscrValRight = lDiscrValLeft+1L;
        while(*itRight == 0)
        {
            ++lDiscrValRight;
            ++itRight;
        }
        //rflMedian = (*itLeft * undiscretize(m_psHeader->m_lResExp,lDiscrValLeft) + *itRight * undiscretize(m_psHeader->m_lResExp,lDiscrValRight))/(*itLeft + *itRight);
        //  Conventional definition (see "Numerical Recipes" page 614)
        rflMedian = (undiscretize(m_psHeader->m_lResExp,lDiscrValLeft) + undiscretize(m_psHeader->m_lResExp,lDiscrValRight))/2;
    }
    else
    {
        rflMedian = undiscretize(m_psHeader->m_lResExp,lDiscrValLeft);
    }
    return true;
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::median(long& rlDiscrMedian) const
{
    FLOAT_TYPE flMedian;
    if( !median(flMedian) )
    {
        LOCATE_ERROR(__FILE__,__LINE__);
        return false;
    }
    rlDiscrMedian = discretize(m_psHeader->m_lResExp,flMedian);
    return true;
}

template <class FLOAT_TYPE, class CHA_TYPE>
TFLHist<FLOAT_TYPE,CHA_TYPE>::TFLHist()
: m_psHeader(NULL)
{}

template <class FLOAT_TYPE, class CHA_TYPE>
TFLHist<FLOAT_TYPE,CHA_TYPE>::TFLHist(THistHeader* psHeader, CHA_TYPE* pData, long lAllocSize)
: m_psHeader(psHeader)
, m_aiChannel(pData,lAllocSize)
{}

template <class FLOAT_TYPE, class CHA_TYPE>
TFLHist<FLOAT_TYPE,CHA_TYPE>::~TFLHist()
{
    if(m_aiChannel.m_bOwner && m_psHeader != NULL)
    {
        delete m_psHeader;
    }
}

template <class FLOAT_TYPE, class CHA_TYPE>
bool TFLHist<FLOAT_TYPE,CHA_TYPE>::plot(ostream& rDst, const_iterator itBegin, const_iterator itEnd) const
{
    if( m_psHeader != NULL )
    {
        FLOAT_TYPE flDelta = undiscretize(m_psHeader->m_lResExp,1L)/2., flVal;
        while( itBegin != itEnd )
        {
            flVal = itBegin.value();
            rDst << flVal-flDelta << ' ' << 0        << '\n';
            rDst << flVal-flDelta << ' ' << *itBegin << '\n';
            rDst << flVal+flDelta << ' ' << *itBegin << '\n';
            rDst << flVal+flDelta << ' ' << 0        << '\n';
            ++itBegin;
        }
    }
    return true;
}

#endif