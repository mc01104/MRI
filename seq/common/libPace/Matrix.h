//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\Matrix.h@@\main\2
//          Version: \main\branch\5
//
//         Language: C
//
//      Description: Definition of template class Matrix
//          Structs: 
//
//  -----------------------------------------------------------------
#ifndef __MATRIX_H
#define __MATRIX_H

//  Definition of template class Array
#include "Array.h"


//  -----------------------------------------------------------------
//  Defintion of template class Matrix
//  -----------------------------------------------------------------

template<class TYPE>
class Matrix
{
public:
    typedef Matrix<TYPE> MYTYPE;

    //  Initialization of the class. Must be called before any other
    //  member function is invoked.
    //  P a r a m e t e r s:
    //  lInnerMin
    //    The minimum index of the inner dimension.
    //  lInnerSize
    //    lInnerMin+lInnerSize-1 is the maximum index of the inner dimension.
    //  lOuterMin
    //    The maximum index of the outer dimension.
    //  lOuterSize
    //    lOuterMin+lOuterSize-1 is the maximum index of the outer dimension.
    //
    bool init(long lInnerMin, long lInnerSize, long lOuterMin, long lOuterSize);

    //  The member function returns a reference to the element of the
    //  controlled sequence at position pos. If that position is invalid,
    //  the function returns false.
    TYPE* elementAt(long lInner, long lOuter);
    const TYPE* elementAt(long lInner, long lOuter) const;

    //  Default constructor
    Matrix();
protected:
    //  Class attributes
    TArray<TYPE> m_aData;
    long        m_lInnerMin;
    long        m_lInnerSize;
    long        m_lOuterMin;
};

template<class TYPE>
Matrix<TYPE>::Matrix()
: m_lInnerMin(0L)
, m_lInnerSize(0L)
, m_lOuterMin(0L)
{}


template<class TYPE>
bool Matrix<TYPE>::init(long lInnerMin, long lInnerSize, long lOuterMin, long lOuterSize)
{
    if( lInnerSize < 1 || lOuterSize < 1 || !m_aData.resize(lInnerSize*lOuterSize) )
    {
        return false;
    }
    m_lInnerMin  = lInnerMin;
    m_lInnerSize = lInnerSize;
    m_lOuterMin  = lOuterMin;
    return true;
}

template<class TYPE>
TYPE* Matrix<TYPE>::elementAt(long lInner, long lOuter)
{
    long lPos = (lOuter-m_lOuterMin)*m_lInnerSize+(lInner-m_lInnerMin);
    if( lInner < m_lInnerMin || lInner >= m_lInnerMin+m_lInnerSize || lOuter < m_lOuterMin || lPos >= m_aData.size() )
    {
        return NULL;
    }
    return &m_aData[lPos];
}

template<class TYPE>
const TYPE* Matrix<TYPE>::elementAt(long lInner, long lOuter) const
{
    long lPos = (lOuter-m_lOuterMin)*m_lInnerSize+(lInner-m_lInnerMin);
    if( lInner < m_lInnerMin || lInner >= m_lInnerMin+m_lInnerSize || lOuter < m_lOuterMin || lPos >= m_aData.size() )
    {
        return NULL;
    }
    return &m_aData[lPos];
}

#endif //  __MATRIX_H