//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\Array.h
//          Version: \main\branch\5
//
//         Language: C
//
//      Description: Definition of template class TArray
//          Structs: 
//
//  -----------------------------------------------------------------
#ifndef __ARRAY_H
#define __ARRAY_H

//  Definition of NULL
#ifdef VXWORKS
#include <vxworks.h>
#else
#include <stddef.h>
#endif
//  -----------------------------------------------------------------
//  Defintion of template class Pair
//  Similar to STL-pair.
//  Note: The STL can't be used on the ALPHA since the preprocessor
//        directives __STL_BEGIN_NAMESPACE, __STL_END_NAMESPACE have
//        not been defined in the ALPHA environment.
//  -----------------------------------------------------------------
/*
template <class TYPE1, class TYPE2>
struct Pair
{
    typedef TYPE1 first_type;
    typedef TYPE2 second_type;

    //  Attributes
    TYPE1 m_first;
    TYPE2 m_second;

    //  Constructors.
    Pair()
    {}

    Pair(const TYPE1& __a, const TYPE2& __b) 
        : m_first(__a)
        , m_second(__b)
    {}

    //  Copy constructor
    Pair(const Pair<TYPE1,TYPE2>& rRHS)
        : m_first(rRHS.m_first)
        , m_second(rRHS.m_second)
    {}
};

//  Equal to operator.
template <class TYPE1, class TYPE2>
inline bool operator==(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{ 
    return rLHS.m_first == rRHS.m_first && rLHS.m_second == rRHS.m_second; 
}

//  Less than operator
//  Similar to STL-Imoplemantation.
//  Check carefully before usage.
template <class TYPE1, class TYPE2>
inline bool operator<(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{ 
    return rLHS.m_first < rRHS.m_first || 
         (!(rRHS.m_first < rLHS.m_first) && rLHS.m_second < rRHS.m_second); 
}

//  Unequal to operator.
template <class TYPE1, class TYPE2>
inline bool operator!=(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{
    return !(rLHS == rRHS);
}

//  Greater than operator.
template <class TYPE1, class TYPE2>
inline bool operator>(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{
    return rRHS < rLHS;
}

//  Less than or equal to opeartor.
template <class TYPE1, class TYPE2>
inline bool operator<=(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{
    return !(rRHS < rLHS);
}

//  Greater than or equal to operator.
template <class TYPE1, class TYPE2>
inline bool operator>=(const Pair<TYPE1, TYPE2>& rLHS, const Pair<TYPE1, TYPE2>& rRHS)
{
    return !(rLHS < rRHS);
}

//  -----------------------------------------------------------------
//  Defintion of template class Triple
//  Similar to Pair but with three members.
//  -----------------------------------------------------------------

template <class TYPE1, class TYPE2, class TYPE3>
struct Triple : public Pair<TYPE1,TYPE2>
{
    typedef Triple<TYPE1,TYPE2,TYPE3> MYTYPE;
    typedef Pair<TYPE1,TYPE2>         BASE_TYPE;
    typedef Pair<TYPE1,TYPE2>::first_type     first_type;
    typedef Pair<TYPE1,TYPE2>::second_type    second_type;
    typedef TYPE3                     third_type;

    //  Attributes
    TYPE3 m_third;

    //  Constructors.
    Triple()
    {}

    Triple(const TYPE1& __a, const TYPE2& __b, const TYPE3& __c) 
        : BASE_TYPE(__a,__b)
        , m_third(__c)
    {}

    //  Copy constructor
    Triple(const MYTYPE& rRHS)
        : BASE_TYPE(rRHS)
        , m_third(rRHS.m_third)
    {}
};

//  Equal to operator.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator==(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{ 
    return rLHS.m_first == rRHS.m_first && rLHS.m_second == rRHS.m_second && rLHS.m_third == rRHS.m_third; 
}

//  Less than operator
//  Similar to STL-Imoplemantation.
//  Check carefully before usage.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator<(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{ 
    return rLHS.m_first < rRHS.m_first
        || (!(rRHS.m_first < rLHS.m_first) && rLHS.m_second < rRHS.m_second)
        || (!(rRHS.m_first < rLHS.m_first) && !(rLHS.m_second < rRHS.m_second) && rLHS.m_third < rRHS.m_third); 
}

//  Unequal to operator.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator!=(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{
    return !(rLHS == rRHS);
}

//  Greater than operator.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator>(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{
    return rRHS < rLHS;
}

//  Less than or equal to opeartor.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator<=(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{
    return !(rRHS < rLHS);
}

//  Greater than or equal to operator.
template <class TYPE1, class TYPE2, class TYPE3>
inline bool operator>=(const Triple<TYPE1, TYPE2, TYPE3>& rLHS, const Triple<TYPE1, TYPE2, TYPE3>& rRHS)
{
    return !(rLHS < rRHS);
}


//  -----------------------------------------------------------------
//  Defintion of template class Quad
//  Similar to Pair but with four members.
//  -----------------------------------------------------------------

template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
struct Quad : public Triple<TYPE1,TYPE2,TYPE3>
{
    typedef Quad <TYPE1,TYPE2,TYPE3,TYPE4>          MYTYPE;
    typedef Triple<TYPE1,TYPE2,TYPE3>               BASE_TYPE;
    typedef Triple<TYPE1,TYPE2,TYPE3>::first_type   first_type;
    typedef Triple<TYPE1,TYPE2,TYPE3>::second_type  second_type;
    typedef Triple<TYPE1,TYPE2,TYPE3>::third_type   third_type;
    typedef TYPE4                                   fourth_type;

    //  Attributes
    TYPE4 m_fourth;

    //  Constructors.
    Quad()
    {}

    Quad(const TYPE1& __a, const TYPE2& __b, const TYPE3& __c, const TYPE4& __d) 
        : BASE_TYPE(__a,__b,__c)
        , m_fourth(__d)
    {}

    //  Copy constructor
    Quad(const MYTYPE& rRHS)
        : BASE_TYPE(rRHS)
        , m_fourth(rRHS.m_fourth)
    {}
};

//  Equal to operator.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator==(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{ 
    return rLHS.m_first == rRHS.m_first && rLHS.m_second == rRHS.m_second && rLHS.m_third == rRHS.m_third && rLHS.m_fourth == rRHS.m_fourth; 
}

//  Less than operator
//  Similar to STL-Imoplemantation.
//  Check carefully before usage.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator<(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{ 
    return rLHS.m_first < rRHS.m_first
        || (!(rRHS.m_first < rLHS.m_first) && rLHS.m_second < rRHS.m_second)
        || (!(rRHS.m_first < rLHS.m_first) && !(rLHS.m_second < rRHS.m_second) && rLHS.m_third < rRHS.m_third)
        || (!(rRHS.m_first < rLHS.m_first) && !(rLHS.m_second < rRHS.m_second && !(rLHS.m_third < rRHS.m_third)) && rLHS.m_fourth < rRHS.m_fourth); 
}

//  Unequal to operator.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator!=(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{
    return !(rLHS == rRHS);
}

//  Greater than operator.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator>(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{
    return rRHS < rLHS;
}

//  Less than or equal to opeartor.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator<=(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{
    return !(rRHS < rLHS);
}

//  Greater than or equal to operator.
template <class TYPE1, class TYPE2, class TYPE3, class TYPE4>
inline bool operator>=(const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rLHS, const Quad<TYPE1, TYPE2, TYPE3, TYPE4>& rRHS)
{
    return !(rLHS < rRHS);
}
*/

//  -----------------------------------------------------------------
//  Defintion of template class TArray
//  To avoid usage of STL-vector.
//  -----------------------------------------------------------------
namespace PACE
{
template <class TYPE>
class TArray
{
public:

	//  The type describes an object that can serve as a random-access
	//  iterator for the controlled sequence. 
	typedef TYPE* iterator;

	//  The type describes an object that can serve as a constant
	//  random-access iterator for the controlled sequence. 
	typedef const TYPE* const_iterator;


    //  Actual size of array.
    long m_lSize;

    //  Length of the array m_pElm points to.
    long m_lAllocSize;

    //  Pointer to array of elements of type TYPE.
    TYPE* m_pElm;

    //  If the boolean flag is true the object is the owner
    //  of the data addressed by m_pElm and the destructur
    //  will deallocate them. Otherwise the client is
    //  responsible for data construction/destruction and any
    //  function call that desires reallocation will fail.
    bool  m_bOwner;

    //  Default constructor.
    TArray();

    //  Initialization with preallocated data. The object will
    //  not get the owner of the data. The client is responsible
    //  for deallocation of the data passed. Resize, reserve will
    //  fail if the desired new size is greater than the initial size
    //  lAllocSize passed.
    //  The initial size of the array is equal to lAllocSize.
    TArray(TYPE* pData, long lAllocSize);

    //  Copy constructor
    TArray(const TArray& rRHS);

    //  Destructor.
    ~TArray();

    //  The member function returns the length of the controlled sequence.
    long size() const;

    //  The member function returns the storage currently allocated
    //  to hold the controlled sequence, a value at least as large
    //  as size().
    long capacity() const;

    //  Set size to lSize. Growth the array if necessary.
    //  Previous array content is lost.
    bool resize(long lSize);

    //  The member function ensures that size() henceforth returns
    //  lSize. If it must lengthen the controlled sequence,
    //  it appends elements with value rDef.
    bool resize(long lSize, const TYPE& rDef);

    //  Ensures that the storage currently allocated is at least as
    //  large as lAllocSize. Does not change the actual size of the
    //  arry. May be used to prevent multiple allocation.
    bool reserve(long lAllocSize);

    //  Subscript operator
    TYPE& operator[](long lPos);
    const TYPE& operator[](long lPos) const;

    //  The member function inserts an element with value rNewVal at
    //  the end of the controlled sequence.
    bool push_back(const TYPE& rNewVal);

    //  The member function replaces the sequence controlled by *this
    //  with the sequence [first, last).
    bool assign(const_iterator itFirst, const_iterator itLast)
    {
        if( !resize(itLast-itFirst) )
        {
            return false;
        }
        iterator it = begin();
        while(itFirst != itLast)
        {
            *it++ = *itFirst++;
        }
        return true;
    }
    //  Assignment operator
    TArray<TYPE>& operator=(const TArray<TYPE>& rRHS);

	//  The member function returns a random-access iterator that points
	//  at the first element of the sequence (or just beyond the end of
	//  an empty sequence).
	const_iterator begin() const
    {
        return m_pElm;
    }
	iterator begin()
    {
        return m_pElm;
    }

	//  The member function returns a random-access iterator that points
	//  just beyond the end of the sequence.
	const_iterator end() const
    {
        return m_pElm+m_lSize;
    }

	iterator end()
    {
        return m_pElm+m_lSize;
    }

	//  The member function returns a reference to the first element of
	//  the controlled sequence, which must be non-empty.
	TYPE& front();
    const TYPE& front() const;

	//  The member function returns a reference to the last element of
	//  the controlled sequence, which must be non-empty.
	TYPE& back();
	const TYPE& back() const;

    //  The member function returns true for an empty controlled
    //  sequence.
    bool empty() const;

	//  Similar to resize(0). No error status since it is always
	//  possible to reduce array size.
	void clear();

};

//  Inserts a new element and returns the position of the new element.
//  If insert fails (due to an allocation error) size() is returned.
//  Insert point is the first element found for which (CMP)(rArray[lPos-1],rNewElm)
//  returns true. The search always starts at the end of the array.
//  If no such element exists the new element is inserted at the beginning of
//  the Array.
//  The type STRICT_WEAK_ORDERING must implement the operator
//  bool operator()(const TYPE&, const TYPE&) const

template<class TYPE, class STRICT_WEAK_ORDERING>
long insert(TArray<TYPE>& rArray, const TYPE& rNewElm, const STRICT_WEAK_ORDERING& rCMP)
{
    if( !rArray.resize(rArray.m_lSize+1) )
    {
        return rArray.m_lSize;
    }
    long lPos = rArray.m_lSize-1;
    rArray[lPos] = rNewElm;
    while( lPos > 0 && !(rCMP)(rArray[lPos-1],rNewElm) )
    {
        //  Swap positions
        rArray[lPos]   = rArray[lPos-1];
        rArray[--lPos] = rNewElm;
    }
    return lPos;
}

//  -----------------------------------------------------------------
//  Defintion of template class Ring
//  -----------------------------------------------------------------

template <class TYPE>
class Ring
{
    //  The type realizes a ring buffer.

protected:
    TArray<TYPE> m_sBuffer;
    long        m_lPosLatest;
    long        m_lPosRightMost;

public:

    //  The type describes an object that can serve as a constant
	//  random-access iterator for the controlled sequence.
    //  It can be used to iterate through the elements in non-
    //  chronlogical order.
    typedef const TYPE* const_iterator;

    class chron_iterator
    {
        friend class Ring<TYPE>;
        Ring* m_pRing;
        long  m_lPos;

    public:

        //  Default constructor.
        //  Creates an invalid iterator.
        chron_iterator()
            : m_pRing(NULL)
            , m_lPos(0)
        {}

        //  Copy constructor
        chron_iterator(const chron_iterator& rRHS)
            : m_pRing(rRHS.m_pRing)
            , m_lPos(rRHS.m_lPos)
        {}

        //  Asignment operator
        chron_iterator& operator=(const chron_iterator& rRHS)
        {
            if(&rRHS != this)
            {
                m_pRing = rRHS.m_pRing;
                m_lPos  = rRHS.m_lPos;
            }
            return *this;
        }

        //  If the member function returns false the
        //  iterator must not be dereferenced.
        bool valid() const
        {
            return m_pRing != NULL;
        }

        //	Postfix increment operator.
        chron_iterator& operator++()
        {
            m_lPos = (m_lPos+1)%(m_pRing->m_lPosRightMost+1);
            return *this;
        }

        //  Prefix increment operator
        const chron_iterator operator ++ (int )
        {
            chron_iterator sTmp(*this);
            ++*this;
            return sTmp;
        }

        //	Postfix decrement operator.
        chron_iterator& operator--()
        {
            if(--m_lPos < 0L)
            {
                m_lPos = m_lPosRightMost;
            }
            return *this;
        }

        //	Prefix decrement operator.
        const chron_iterator operator -- (int )
        {
            chron_iterator sTmp(*this);
            --*this;
            return sTmp;
        }

        //  Equality operator.
        bool operator==(const chron_iterator& rRHS) const
        {
            return (m_lPos == rRHS.m_lPos && m_pRing == rRHS.m_pRing);
        }

        //  Unequal to operator.
        bool operator!=(const chron_iterator& rRHS) const
        {
            return !(*this == rRHS);
        }

        //	Dereference operator.
        const TYPE& operator*() const
        {
            #ifndef VXWORKS
                if( !valid() ) throw;
            #endif
            return m_pRing->m_sBuffer[m_lPos];
        }

    };

    friend class chron_iterator;

    //  Default constructor.
    //  Creates an empty buffer.
    Ring()
        : m_lPosLatest(-1L)
        , m_lPosRightMost(-1L)
    {}

    //  Sets the size of the ring buffer.
    //  Invalidate previous content and any previously returned
    //  iterator.
    bool reserve(long lSize)
    {
        m_lPosRightMost = m_lPosLatest = -1L;
        return m_sBuffer.resize(lSize);
    }

    //  Returns number of stored elements.
    long size() const
    {
        return 1L+m_lPosRightMost;
    }

    //  The member function returns true for an empty controlled
    //  sequence.
    bool empty() const
    {
        return m_lPosRightMost == -1L;
    }


    //  Adds a new to the ring buffer. Overwrites earliest element
    //  if necessary.
    bool add(const TYPE& rNewElm)
    {
        if( m_sBuffer.size() < 1 )
        {
            return false;
        }
        m_lPosLatest = (m_lPosLatest+1L)%m_sBuffer.size();
        m_sBuffer[m_lPosLatest] = rNewElm;
        if(m_lPosRightMost < m_lPosLatest)
        {
            m_lPosRightMost = m_lPosLatest;
        }
        return true;
    }


	//  The member function returns a random-access iterator that points
	//  at the first (not the earliest) element of the sequence (or just beyond the end of
	//  an empty sequence).
	const_iterator begin() const
    {
        return m_sBuffer.begin();
    }

	//  The member function returns a random-access iterator that points
	//  just beyond the end of the sequence.
	const_iterator end() const
    {
        return begin()+m_lPosRightMost+1;
    }

    //  Returns iterator that points to earliest element that has
    //  been added but not overwritten.
    //  Fails for an empty ring.
    bool earliest(chron_iterator& rDst)
    {
        if( empty() ) return false;
        rDst.m_pRing = this;
        rDst.m_lPos  = (m_lPosLatest+1)%(m_lPosRightMost+1);
        return true;
    }

    //  Returns iterator that points to the most recently added
    //  element.
    //  Fails for an empty ring.
    bool latest(chron_iterator& rDst)
    {
        if( empty() ) return false;
        rDst.m_pRing = this;
        rDst.m_lPos  = m_lPosLatest;
        return true;
    }
};


//  The follwing template classes fullfill the STRICT_WEAK_ORDERING
//  interface if the associated binary operator (e.g. less than poerator < for LESS)
//  has been implemented for the type.

template <class TYPE>
struct EQUAL_TO
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x == __y;
	}
};

template <class TYPE>
struct NOT_EQUAL_TO 
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x != __y;
	}
};

template <class TYPE>
struct GREATER
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x > __y;
	}
};

template <class TYPE>
struct LESS
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x < __y;
	}
};

template <class TYPE>
struct GREATER_EQUAL
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x >= __y;
	}
};

template <class TYPE>
struct LESS_EQUAL
{
	bool operator()(const TYPE& __x, const TYPE& __y) const
	{
		return __x <= __y;
	}
};



//  -----------------------------------------------------------------
//  Defintion of template class TArrayEx
//  Adds an iterator to template class Array that can be used to
//  transverse the Array with an variable step size between
//  [0,size()/2]. If the step size of the array is set to n, n
//  sweeps through the sequence are required to pass each element.   
//  -----------------------------------------------------------------

template <class TYPE>
class TArrayEx : public TArray<TYPE>
{
public:

    class Iterator
    {
        //  Start address of designated array
        TArrayEx<TYPE>* m_pArray;
        //  Current position
        long           m_lPos;
        //  Current increment
        long           m_lIncr;
        //  Current sweep, within [0, lIncr-1]
        long           m_lSweep;

    public:

        //  Default Constructur
        //  Creates iterator that must be initialized before
        //  it can be used.
        Iterator()
            : m_pArray(0)
            , m_lPos(0L)
            , m_lIncr(1L)
            , m_lSweep(0L)
        {}

        //  Copy constructor
        Iterator(const Iterator& rRHS)
            : m_pArray(rRHS.m_pArray)
            , m_lPos  (rRHS.m_lPos)
            , m_lIncr (rRHS.m_lIncr)
            , m_lSweep(rRHS.m_lSweep)
        {}

        //  Asignment operator
        Iterator& operator=(const Iterator& rRHS)
        {
            if(&rRHS != this)
            {
                m_pArray = rRHS.m_pArray;
                m_lPos   = rRHS.m_lPos;
                m_lIncr  = rRHS.m_lIncr;
                m_lSweep = rRHS.m_lSweep;
            }
            return *this;
        }


        //  If the member function returns false the
        //  iterator must not be dereferenced.
        bool valid() const
        {
            return m_pArray != 0;
        }

        //  Returns element position in controlled sequence.
        long pos() const
        {
            return m_lPos;
        }

        //  Sets the value by which the anatomical position is incremented
        //  by each call to the increment operator. This also determines
        //  the number of sweeps through the array required to pass each
        //  slice. lVal must be greater than one and equal to or less than
        //  N/2, where N is the number of elements (m_lSize).
        bool incr(long lVal)
        {
            if(lVal == 1L)
            {
                m_lIncr  = 1L;
                m_lSweep = 0L;
                return true;
            }
            if(lVal < 1L || lVal > m_pArray->m_lSize/2L)
            {
                return false;
            }
            m_lIncr = lVal;
            m_lSweep = lVal-m_lPos%lVal-1L;
            return true;
        }

        //  Initailizes position and increment
        bool init(TArrayEx<TYPE>* pArray, long lPos, long lIncr)
        {
            if(pArray == 0 || lPos < 0L || lPos >= pArray->m_lSize) return false;
            m_pArray = pArray;
            m_lPos = lPos;
            return incr(lIncr);
        }

        //	Postfix increment operator.
        Iterator& operator++()
        {
            m_lPos += m_lIncr;
            if(m_lPos >= m_pArray->m_lSize)
            {
                m_lSweep = ++m_lSweep%m_lIncr;
                m_lPos = m_lIncr-m_lSweep-1L;
            }
            return *this;
        }

        //  Prefix increment operator
        const Iterator operator ++ (int )
        {
            Iterator sTmp(*this);
            ++*this;
            return sTmp;
        }

        //	Postfix decrement operator.
        Iterator& operator--()
        {
            m_lPos -= m_lIncr;
            if(m_lPos < 0L)
            {
                m_lSweep = (--m_lSweep+m_lIncr)%m_lIncr;
                m_lPos = m_lIncr-m_lSweep-1L+(m_pArray->m_lSize/m_lIncr)*m_lIncr;
                while(m_lPos >= m_pArray->m_lSize) m_lPos -= m_lIncr;
            }
            return *this;
        }

        //	Prefix decrement operator.
        const Iterator operator -- (int )
        {
            Iterator sTmp(*this);
            --*this;
            return sTmp;
        }

        //  Equality operator.
        bool operator==(const Iterator& rRHS) const
        {
            return (m_lPos == rRHS.m_lPos && m_pArray == rRHS.m_pArray);
        }

        //  Unequal to operator.
        bool operator!=(const Iterator& rRHS) const
        {
            return !(*this == rRHS);
        }

        //	Dereference operator.
        const TYPE& operator*() const
        {
            return m_pArray->operator[](m_lPos);
        }

        TYPE& operator*()
        {
            return m_pArray->operator[](m_lPos);
        }

        //  Member access operator
        const TYPE* operator->() const
        {
            return &m_pArray->operator[](m_lPos);
        }

        TYPE* operator->()
        {
            return &m_pArray->operator[](m_lPos);
        }

    };

    //  If the member function succeeds the bidirectional iterator
    //  rIt points to will get an iterator with initial step width
    //  lStepWidth that points at the first element of the array.
    //  Note that the actual element designated by the iterator
    //  depends on the step-width passed.
    //  The step width must be within [0,size()/2)
    bool first(Iterator& rIt, long lStepWidth)
    {
        return rIt.init(this,lStepWidth-1,lStepWidth);
    }

    //  If the member function succeeds the bidirectional iterator
    //  rIt points to will get an iterator with initial step width
    //  lStepWidth that points at the last element of the array.
    //  Note that the actual element designated by the iterator
    //  depends on the step-width passed.
    //  The step width must be within [0,size()/2)
    bool last(Iterator& rIt, long lStepWidth)
    {
        if( !first(rIt,lStepWidth) ) return false;
        --rIt;
        return true;
    }

};

//  -----------------------------------------------------------------
//  Implementation of TArray member functions.
//  -----------------------------------------------------------------

template <class TYPE>
TYPE& TArray<TYPE>::operator[](long lPos)
{
#ifndef VXWORKS
    if( lPos < 0L || lPos >= m_lSize || m_pElm == NULL ) throw;
#endif
    return *(m_pElm+lPos);
}


template <class TYPE>
const TYPE& TArray<TYPE>::operator[](long lPos) const
{
#ifndef VXWORKS
    if( lPos < 0L || lPos >= m_lSize ) throw;
#endif
    return *(m_pElm+lPos);
}


template <class TYPE>
TYPE& TArray<TYPE>::front()
{
#ifndef VXWORKS
    if( m_lSize < 1 ) throw;
#endif
    return *m_pElm;
}

template <class TYPE>
const TYPE& TArray<TYPE>::front() const
{
#ifndef VXWORKS
    if( m_lSize < 1 ) throw;
#endif
    return *m_pElm;
}

template <class TYPE>
TYPE& TArray<TYPE>::back()
{
#ifndef VXWORKS
    if( m_lSize < 1 ) throw;
#endif
    return *(m_pElm+m_lSize-1);
}

template <class TYPE>
const TYPE& TArray<TYPE>::back() const
{
#ifndef VXWORKS
    if( m_lSize < 1 ) throw;
#endif
    return *(m_pElm+m_lSize-1);
}


template <class TYPE>
TArray<TYPE>::TArray()
: m_lSize(0)
, m_lAllocSize(0)
, m_pElm(0)
, m_bOwner(true)
{}

template <class TYPE>
TArray<TYPE>::TArray(TYPE* pData, long lAllocSize)
: m_lSize(lAllocSize)
, m_lAllocSize(lAllocSize)
, m_pElm(pData)
, m_bOwner(false)
{}

template <class TYPE>
TArray<TYPE>::~TArray()
{
    if(m_bOwner && m_pElm) delete [] m_pElm;
}

template <class TYPE>
bool TArray<TYPE>::reserve(long lAllocSize)
{
    if(lAllocSize > m_lAllocSize)
    {
        if( !m_bOwner ) return false;
        TYPE* pDoomed = m_pElm;
        m_pElm = new TYPE[lAllocSize];
        if(!m_pElm)
        {
            m_pElm = pDoomed;
            return false;
        }
        if(pDoomed)
        {
            long lCntr = 0;
            while(lCntr < m_lSize)
            {
                *(m_pElm+lCntr) = *(pDoomed+lCntr);
                ++lCntr;
            }
            delete [] pDoomed;
        }
        m_lAllocSize = lAllocSize;
    }
    return true;
}

template <class TYPE>
long TArray<TYPE>::size() const
{
    return m_lSize;
}

template <class TYPE>
long TArray<TYPE>::capacity() const
{
    return m_lAllocSize;
}

template <class TYPE>
bool TArray<TYPE>::empty() const
{
    return m_lSize == 0L;
}

template <class TYPE>
bool TArray<TYPE>::resize(long lSize)
{
    if( !reserve(lSize) )
    {
        return false;
    }
    m_lSize = lSize;
    return true;
}

template <class TYPE>
bool TArray<TYPE>::resize(long lSize, const TYPE& rDef)
{
    if( !reserve(lSize) )
    {
        return false;
    }
    iterator it = end();
    m_lSize = lSize;
    iterator itEnd = end();
    while(it < itEnd)
    {
        *it = rDef;
        ++it;
    }
    return true;
}


template <class TYPE>
void TArray<TYPE>::clear()
{
    m_lSize = 0L;
}



template <class TYPE>
TArray<TYPE>& TArray<TYPE>::operator=(const TArray<TYPE>& rRHS)
{
    if(this != &rRHS)
    {
#ifdef VXWORKS
        reserve(rRHS.size());
        m_lSize = min(rRHS.size(),m_lAllocSize);
#else
        if( !reserve(rRHS.size()) ) throw;
        m_lSize = rRHS.size();
#endif
        iterator it = begin(), itEnd = end();
        const_iterator itSrc = rRHS.begin();
        while( it != itEnd )
        {
            *it++ = *itSrc++;
        }
    }
    return *this;
}

template <class TYPE>
TArray<TYPE>::TArray(const TArray<TYPE>& rRHS)
: m_lSize(0)
, m_lAllocSize(0)
, m_pElm(0)
, m_bOwner(true)
{
    *this = rRHS;
}

template <class TYPE>
bool TArray<TYPE>::push_back(const TYPE& rNewVal)
{
    if( !resize(m_lSize+1L) )
    {
        return false;
    }
    back() = rNewVal;
    return true;
}

//  -----------------------------------------------------------------
//  Evaluates mean and variance of a distribution
//  -----------------------------------------------------------------

//  Input value are two random access iterators. The first itBegin
//  must point to the begin of the distribution that should be
//  evaluated. The second itEnd must point just beyond the end
//  of the distribution. 
//  Return value is the mean of the distribution and the variance
//  of the distribution which has been calculated using
//  the corrected two-pass algorithm (see Bevington P.R. 1969,
//  Data Reduction and Error Analysis for the Physical Sciences
//  (New York: McGraw-Hill), Chapter 2).
//  pMean must be different from zero and the distribution must be
//  non-empty. pVar can be NULL. In this case the variance is
//  not calculated. If pVar is different from zero the number
//  of element of the distribution must be at least two.
template<class TYPE, class IT_TYPE1, class IT_TYPE2>
bool moment(TYPE* pMean, TYPE* pVar, IT_TYPE1 itBegin, IT_TYPE2 itEnd) 
{
	//  First pass to get the mean
	size_t uN = itEnd-itBegin;
	if( uN < 1 || pMean == NULL )
	{
        return false;
	}
	IT_TYPE1 it = itBegin;
	*pMean = 0;
	while( it != itEnd )
	{
		*pMean += *it;
		++it;
	}
	*pMean /= uN;
	if( pVar != NULL )
	{
		if( uN < 2L || pVar == NULL )
		{
			return false;
		}
		*pVar = 0;
		TYPE flErr = 0;
		TYPE flDev = 0;
		for( it = itBegin; it != itEnd; ++it )
		{
			flDev  = *it;
			flDev -= *pMean;
			flErr += flDev;
			flDev *= flDev;
			*pVar += flDev;
		}
		flErr *= flErr;
		flErr /= uN;
		*pVar -= flErr;
		*pVar /= (uN-1);
	}
	return true;
}

//  The first template function repeatedly replaces rInOut with 
//  rInOut + (*I1 * *I2), for each value of the IT_TYPE1 iterator I1
//  in the interval [first1, last2). In each case, the IT_TYPE2
//  iterator I2 equals first2 + (I1 - first1).
//  The function returns true.
template < class TYPE, class IT_TYPE1, class IT_TYPE2>
bool inner_product(TYPE& rInOut, IT_TYPE1 __first1, IT_TYPE1 __last1, IT_TYPE2 __first2)
{
    while(__first1 != __last1)
    {
        rInOut += (*__first1 * *__first2);
        ++__first1;
        ++__first2;
    }
    return true;
}

//  The template function repeatedly replaces rVal with 
//  rVal + (*I1-rRef) * (*I1-rRef), for each value of the IT_TYPE
//  iterator I1 in the interval [__begin, __end[.
//  Initial value of rVal is set to zero.
template < class TYPE, class IT_TYPE>
bool sum_squared_differences(TYPE& rVal, IT_TYPE __begin, IT_TYPE __end, const TYPE& rRef)
{
    TYPE _tmp;
    rVal = 0;
    while(__begin != __end)
    {
        _tmp  = (*__begin-rRef);
        _tmp *= _tmp;
        rVal += _tmp;
        ++__begin;
    }
    return true;
}

//  The template function determines the lowest value of N in the
//  range [0, last - first) such that, for each M in the range
//  [0, last - first), the predicate *(first + N) < *(first + M) is false.
//  It then returns first + N. Thus, the function determines the lowest
//  position that contains the largest value in the sequence.
template<class IT_TYPE_OUT, class IT_TYPE_IN>
bool max_element(IT_TYPE_OUT& ritMax, IT_TYPE_IN itBegin, IT_TYPE_IN itEnd)
{
    if(itBegin == itEnd) return false;
    ritMax = *itBegin;
    ++itBegin;
    while(itBegin != itEnd)
    {
        if( *itBegin > ritMax )
        {
            ritMax = *itBegin;
        }
        ++itBegin;
    }
    return true;
}

//  The template function determines the lowest value of N in the
//  range [0, last - first) such that, for each M in the range
//  [0, last - first), the predicate *(first + M) < *(first + N) is false.
//  It then returns first + N. Thus, the function determines the
//  lowest position that contains the smallest value in the sequence.
template<class IT_TYPE_OUT, class IT_TYPE_IN>
bool min_element(IT_TYPE_OUT& ritMin, IT_TYPE_IN itBegin, IT_TYPE_IN itEnd)
{
    if(itBegin == itEnd) return false;
    ritMin = *itBegin;
    ++itBegin;
    while(itBegin != itEnd)
    {
        if( *itBegin < ritMin )
        {
            ritMin = *itBegin;
        }
        ++itBegin;
    }
    return true;
}




//  Traverses the sequence from itBegin up to __end.
//  If the function returns success ritMin will refer to
//  smallest element found and ritMax will refer to the
//  largest element found.
//  Returns false, if __begin equals __end
template < class IT_TYPE_OUT, class IT_TYPE_IN>
bool extreme_element(IT_TYPE_OUT& ritMin, IT_TYPE_OUT& ritMax, IT_TYPE_IN __begin, IT_TYPE_IN __end)
{
    if(__begin == __end) return false;
    ritMin = ritMax = __begin;
    ++__begin;
    while(__begin != __end)
    {
        if( *__begin < *ritMin )
        {
            ritMin = __begin;
        }
        else if( *ritMax  < *__begin )
        {
            ritMax = __begin;
        }
        ++__begin;
    }
    return true;
}

template<class IT_TYPE, class TYPE>
inline void __swap(IT_TYPE it1, IT_TYPE it2, TYPE& rTmp)
{
	rTmp = *it1;
	*it1 = *it2;
	*it2 = rTmp;
}

template <class RANDOM_ACCESS_ITER, class TYPE>
bool __nth_largest(RANDOM_ACCESS_ITER __left, RANDOM_ACCESS_ITER __nth, RANDOM_ACCESS_ITER __right, TYPE&)
{
	TYPE __tmp, __a;
	RANDOM_ACCESS_ITER __mid, __i, __j;
	--__right;
	while(true)
	{
		if(__right <= __left+1)
		{
			//  Active partition contains one or two elements
			if( __right == __left+1 && *__right < *__left )
			{
				//  Case 2 elements
				__swap(__left,__right,__tmp);
			}
			return true;
		}
		else
		{
			__mid = __left+(__right-__left)/2;
			__swap(__mid,__left+1,__tmp);
			if(*(__left+1) > *__right)
			{
				__swap(__left+1,__right,__tmp);
			}
			if(*__left > *__right)
			{
				__swap(__left,__right,__tmp);
			}
			if(*(__left+1) > *__left)
			{
				__swap(__left+1,__left,__tmp);
			}
			//  Initialize pointers for partitioning
			__i = __left+1;
			__j = __right;
			__a = *__left;
			while(true)
			{
				//  Scan up to find element > __a
				do ++__i; while(*__i < __a);
				//  Scan down to find element < __a
				do --__j; while(*__j > __a);
				if(__j < __i)
				{
					//  Pointers crossed. Partitioning complete
					break;
				}
				//  
				__swap(__i,__j,__tmp);
			}
			*__left = *__j;
			*__j    = __a;
			//  Keep active the partition that contains the nth element
			if(__j >= __nth)
			{
				__right = __j-1;
			}
			if(__j <= __nth)
			{
				__left = __i;
			}
		}
	}
	return true;
}


//  Places the nth element in the range from left up to right in the
//  position it would lie if the range had been sorted. It also ensures
//  that all of the elements prior to the final location of the nth
//  element are less than or equal to all of the elements that are after
//  the final position of the nth element.
//  nth must be an iterator in the range [__left,__right[. 
template <class RANDOM_ACCESS_ITER>
bool nth_largest(RANDOM_ACCESS_ITER __left, RANDOM_ACCESS_ITER __nth, RANDOM_ACCESS_ITER __right)
{
	if(__nth < __left || __right <= __nth) return false;
	return __nth_largest(__left,__nth,__right,*__left);
}

//  Calculates the median of the distribution [__left,__right[.
//  Note:  The computional weight doubles, if the number of 
//         elements (__right-__left) is even.
template <class RANDOM_ACCESS_ITER,class TYPE>
bool median(TYPE& rMedian, RANDOM_ACCESS_ITER __left, RANDOM_ACCESS_ITER __right)
{
	if(__right <= __left) return false;
	RANDOM_ACCESS_ITER itMedian = __left+(__right-__left)/2;
	if( !nth_largest(__left,itMedian,__right) )
	{
		return false;
	}
	rMedian = *itMedian;
	if( (__right-__left)%2 == 0 )
	{
		if( !nth_largest(__left,--itMedian,__right) )
		{
			return false;
		}
		rMedian += *itMedian;
		rMedian /= 2;
	}
	return true;
}

} //  namespace PACE

#endif
