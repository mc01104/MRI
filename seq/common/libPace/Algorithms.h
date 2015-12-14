//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\Algorithms.h
//	 Version:
//	  Author: stemal8q
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Definition of various template routines.
//
//	---------------------------------------------------------

#ifndef __ALGORITHM_H
#define __ALGORITHM_H 1

//  Definition of class Numeric.
//#include "Types.h"

//  Assigns all elements in the range from itBegin (inclusive)
//  up to itEnd (exclusive) the value rVal
template<class ITERATOR_TYPE, class TYPE>
void fill(ITERATOR_TYPE itBegin, ITERATOR_TYPE itEnd, const TYPE& rVal)
{
    while(itBegin != itEnd) *itBegin++ = rVal;
}

// The first template function evaluates *(x + N) = *(first + N)) once for each N
// in the range [0, pLast - pFirst), for strictly increasing values of N
// beginning with the lowest value. It then returns x + N.
// If x and first designate regions of storage,
// x must not be in the range [first, last).

template<class OutIt, class InIt>
OutIt copy(OutIt pDst, InIt pFirst, InIt pLast)
{
    while(pFirst != pLast)
    {
        *pDst++ = *pFirst++;
    }
    return pDst;
}

template<class OutIt, class InIt, class SCALE_TYPE>
OutIt copy(OutIt pDst, InIt pFirst, InIt pLast, SCALE_TYPE flScale)
{
    while(pFirst != pLast)
    {
        *pDst++ = flScale * *pFirst++;
    }
    return pDst;
}

//  The template function determines the lowest position that
//  contains the largest value in the sequence [pFirst,pLast[.
template<class FwdIt>
FwdIt max_element(FwdIt pFirst, FwdIt pLast)
{
    FwdIt pMax = pFirst;
    ++pFirst;
    while(pFirst != pLast)
    {
        if(*pFirst > *pMax)
        {
            pMax = pFirst;
        }
        ++pFirst;
    }
    return pMax;
}

//  The template function determines the lowest position that
//  contains the lowest value in the sequence [pFirst,pLast[.
template<class FwdIt>
FwdIt min_element(FwdIt pFirst, FwdIt pLast)
{
    FwdIt pMin = pFirst;
    ++pFirst;
    while(pFirst != pLast)
    {
        if(*pFirst < *pMin)
        {
            pMin = pFirst;
        }
        ++pFirst;
    }
    return pMin;
}

//  The template function repeatedly replaces val with val + *I, for each value of the InIt iterator I in the interval [first, last). It then returns val.


template<class FwdIt, class DEREF_TYPE>
DEREF_TYPE accumulate(FwdIt pFirst, FwdIt pLast,DEREF_TYPE  tVal)
{
    while(pFirst != pLast)
    {
        tVal += *pFirst;
        ++pFirst;
    }
    return tVal;
}




/*
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

//  Traverses the sequence from __begin up to __end.
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
*/
//  -----------------------------------------------------------------
//  Functions neeed for filtering
//  -----------------------------------------------------------------
#ifndef VXWORKS
//  The template function calculates the convolution
//  *itDst = sum_h *(itSrc+h) * itKernel[h],
//  where h = -lMaskWidthCol/2, ..., lMaskWidthCol/2
//        itKernel[h] = *((itKernelEnd+itKernelBegin)/2+h)
//        lMaskWidth  = itKernelEnd+itKernelBegin
//  The range [itSrc-lMaskWidth/2,itSrc+lMaskWidth/2] must be valid.
template <class DST_ITERATOR_TYPE, class SRC_ITERATOR_TYPE, class KERNEL_ITERATOR_TYPE>
static void convolve(DST_ITERATOR_TYPE itDst, SRC_ITERATOR_TYPE itSrc, KERNEL_ITERATOR_TYPE itKernelBegin, KERNEL_ITERATOR_TYPE itKernelEnd)
{
    itSrc                             -= (itKernelEnd-itKernelBegin)/2; 
    *itDst = 0;
    while(itKernelBegin != itKernelEnd)
    {
        *itDst += *itSrc * *itKernelBegin++;
        ++itSrc;
    }
}

//  Builds floation point gaussian kernel
//  Kernel width is (itEnd-itBegin) and must be odd.
template <class FLOAT_TYPE>
bool buildGaussKernel(FLOAT_TYPE* itBegin, FLOAT_TYPE* itEnd)
{
    int iKernelWidth = itEnd-itBegin;
    if(iKernelWidth < 1 || iKernelWidth%2 == 0) return false;

    //  Build kernel
    FLOAT_TYPE flConst = static_cast<FLOAT_TYPE>(iKernelWidth)/5;
    flConst         = 2*flConst*flConst;
    int iArg        = iKernelWidth/2;
    FLOAT_TYPE* pflLeft  = itBegin;
    FLOAT_TYPE* pflRight = itEnd-1;
    FLOAT_TYPE  flSum    = 0;
    while(pflLeft != pflRight)
    {
        flSum += 2 * ( *pflLeft++ = *pflRight-- = Numeric<FLOAT_TYPE>::exp(-(iArg*iArg)/flConst) );
        --iArg;
    }
    flSum += (*pflLeft = 1.);

    //  Normalization
    while(itBegin != itEnd)
    {
        *itBegin++ /= flSum;
    }
    return true;
}

//  Builds strict increasing gaussian floating point kernel
//  Kernel width is (itEnd-itBegin) and must be odd.
template <class FLOAT_TYPE>
bool buildHalfGaussKernel(FLOAT_TYPE* itBegin, FLOAT_TYPE* itEnd)
{
    int iKernelWidth = itEnd-itBegin;
    if(iKernelWidth < 1 || iKernelWidth%2 == 0) return false;

    //  Build kernel
    FLOAT_TYPE flConst = static_cast<FLOAT_TYPE>(2*iKernelWidth-1)/5;
    flConst         = 2*flConst*flConst;
    int iArg        = iKernelWidth-1;
    FLOAT_TYPE* pflLeft        = itBegin;
    const FLOAT_TYPE* pflRight = itEnd-1;
    FLOAT_TYPE  flSum          = 0;
    while(pflLeft != pflRight)
    {
        flSum += ( *pflLeft++ = Numeric<FLOAT_TYPE>::exp(-(iArg*iArg)/flConst) );
        --iArg;
    }
    flSum += (*pflLeft = 1.);

    //  Normalization
    while(itBegin != itEnd)
    {
        *itBegin++ /= flSum;
    }
    return true;
}
//  Builds floation point mean kernel
//  Kernel width is (itEnd-itBegin) and must be odd.
template <class FLOAT_TYPE>
bool buildMeanKernel(FLOAT_TYPE* itBegin, FLOAT_TYPE* itEnd)
{
    int iKernelWidth = itEnd-itBegin;
    if(iKernelWidth < 1 || iKernelWidth%2 == 0) return false;
    fill(itBegin,itEnd,1./iKernelWidth);
    return true;
}

//  Prints for each element in the range from itBegin (inclusive)
//  up to itEnd (exclusive) one line to file pszFName.
//  Line format is:
//  (it-itBegin)*rXScale+rXOffset  *it*rYScale+rYOffset 
//  
template<class ITERATOR_TYPE, class XTYPE, class YTYPE>
bool gnuplot(ITERATOR_TYPE itBegin, ITERATOR_TYPE itEnd, const XTYPE& rXScale, const XTYPE& rXOffset, const YTYPE& rYScale, const YTYPE& rYOffset, const char* const pszFName)
{
    try
    {
        if( pszFName == NULL )
        {
			//LOCATE_ERROR(__FILE__,__LINE__);
			return false;
        }
        ofstream sStream(pszFName);
        if( !sStream.is_open() )
		{
			//cout << E_BEGIN(__FILE__,__LINE__) << "Failed to open '" << pszFName << '\'' << E_END;
			return false;
		}
        ITERATOR_TYPE it = itBegin;
        while(it != itEnd)
        {
            sStream << (it-itBegin)*rXScale+rXOffset << ' ' << (*it)*rYScale+rYOffset << '\n';
            ++it;
        }
        sStream.close();
        return true;
    }
    catch(...)
    {
        CAUGHT_EXCEPT(__FILE__,__LINE__);
        return false;
    }
}

template<class IT_TYPE, class TYPE>
bool var(TYPE& rtVar, TYPE tMean, IT_TYPE it, IT_TYPE itEnd) 
{
	//  First pass to get the mean
	size_t uN = itEnd-it;

	if( uN < 2L  )
	{
		return false;
	}
	rtVar = 0;
	TYPE flErr = 0;
	TYPE flDev = 0;
	for(; it != itEnd; ++it )
	{
		flDev  = *it;
		flDev -= tMean;
		flErr += flDev;
		flDev *= flDev;
		rtVar += flDev;
	}
	flErr *= flErr;
	flErr /= uN;
	rtVar -= flErr;
	rtVar /= (uN-1);
	return true;
}

#endif
#endif