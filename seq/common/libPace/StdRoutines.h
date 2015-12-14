
//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\StdRoutines.h
//          Version: \main\branch\5
//
//         Language: C
//
//      Description: Definition of class MrUILinkBase
//          Structs: 
//
//  -----------------------------------------------------------------
#ifndef __MC_STDROUTINES_H
#define __MC_STDROUTINES_H

//  Definition of struct sSliceData
#include "MrServers/MrMeasSrv/SeqIF/MDH/mdh.h"

//  Definition of NLS_STATUS
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"


//  Definition of ceil, floor
#include <math.h>



//  The following macros can be used to avoid the warning:
//  unused parameter '...', if an argument is need only on
//  one platform.

#ifdef WIN32
#define X86_ARGUMENT(_type,_name) _type _name
#else
#define X86_ARGUMENT(_type,_name) _type
#endif

#ifndef WIN32
#define VXWORKS_ARGUMENT(_type,_name) _type _name
#else
#define VXWORKS_ARGUMENT(_type,_name) _type
#endif

#ifdef WIN32
#define X86_TRY try
#define X86_CATCH(_except) catch(_except)
#else
#define X86_TRY
#define X86_CATCH(_except) if(false)
#endif


//  The class is defined in file "Measurement\Sequence\Prot\MrVector.h"
template <class TYPE>
class VectorPat;

#ifdef _VB_LINE
class sSYNC;
class RFASProxy;
class sRF_PULSE;
#else
class ISYNC;
class ISLICE_POS;
class IRF_PULSE;
#endif
class Slice;
class sREADOUT;
class MdhProxy;
class sROT_MATRIX;
class sFREQ_PHASE;
class sSLICE_POS;
class NLSStatus; 

namespace PACE
{
    //   for VB/VD compatibility
    const MdhProxy& MDH(const sREADOUT& rADC);
    MdhProxy& MDH(sREADOUT& rADC);

#ifdef _VB_LINE
    typedef NLS_STATUS MrResult;
    //NLSStatus fRTEBInit(sROT_MATRIX &rsROT_MATRIX);
    bool MrFailed(NLS_STATUS i32Status);

    RFASProxy& theRFASProxy();
#endif

#ifdef _VB_LINE
    void call_prepSet(sFREQ_PHASE& rFREQ_PHASE,sSLICE_POS& rSlicePos, sRF_PULSE* pRFPulse);
    void call_prepNeg(sFREQ_PHASE& rFREQ_PHASE,sSLICE_POS& rSlicePos, sRF_PULSE* pRFPulse);
#else
    void call_prepSet(sFREQ_PHASE& rFREQ_PHASE,ISLICE_POS* pSlicePos, IRF_PULSE *pRFPulse);
    void call_prepNeg(sFREQ_PHASE& rFREQ_PHASE,ISLICE_POS* pSlicePos, IRF_PULSE *pRFPulse);
#endif

    //  Returns integer multiple of lIncr which is equal to or
    //  greater than lIn/dIn. Thereby lIncr must be greater than
    //  zero.
    template<class INT_TYPE>
    static inline int8_t IMULT_CEIL(int8_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr) : (lIn/lIncr+1));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int16_t IMULT_CEIL(int16_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr) : (lIn/lIncr+1));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int32_t IMULT_CEIL(int32_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr) : (lIn/lIncr+1));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int64_t IMULT_CEIL(int64_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr) : (lIn/lIncr+1));
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint8_t IMULT_CEIL(uint8_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr+1);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint16_t IMULT_CEIL(uint16_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr+1);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint32_t IMULT_CEIL(uint32_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr+1);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint64_t IMULT_CEIL(uint64_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr+1);
        return lIn;
    }
    template<class INT_TYPE>
    static inline int32_t IMULT_CEIL(double dIn, INT_TYPE i32Incr)
    {
        int32_t i32In = static_cast<int32_t>(ceil(dIn));
        if(i32In%i32Incr) i32In = i32Incr*(i32In < 0 ? (i32In/i32Incr) : (i32In/i32Incr+1));
        return i32In;
    }

    //  Returns integer multiple of lIncr which is equal to or
    //  less than lIn. Thereby lIncr must be greater than
    //  zero.
    template<class INT_TYPE>
    static inline int8_t IMULT_FLOOR(int8_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr-1) : (lIn/lIncr));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int16_t IMULT_FLOOR(int16_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr-1) : (lIn/lIncr));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int32_t IMULT_FLOOR(int32_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr-1) : (lIn/lIncr));
        return lIn;
    }
    template<class INT_TYPE>
    static inline int64_t IMULT_FLOOR(int64_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr-1) : (lIn/lIncr));
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint8_t IMULT_FLOOR(uint8_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint16_t IMULT_FLOOR(uint16_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint32_t IMULT_FLOOR(uint32_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr);
        return lIn;
    }
    template<class INT_TYPE>
    static inline uint64_t IMULT_FLOOR(uint64_t lIn, INT_TYPE lIncr)
    {
        if(lIn%lIncr) lIn = lIncr*(lIn/lIncr);
        return lIn;
    }
    static inline int32_t IMULT_FLOOR(double dIn, int32_t lIncr)
    {
        int32_t lIn = static_cast<int32_t>(floor(dIn));
        if(lIn%lIncr) lIn = lIncr*(lIn < 0 ? (lIn/lIncr-1) : (lIn/lIncr));
        return lIn;
    }

    //  Returns nearest integer to dIn which is an integer multiple of lIncr.
    //  lIncr must be greater than zero.
    static inline int32_t IMULT(double dIn, int32_t lIncr)
    {
        return dIn < 0
            ? lIncr*(static_cast<int32_t>(dIn-lIncr/2.)/lIncr)
            : lIncr*(static_cast<int32_t>(dIn+lIncr/2.)/lIncr);
    }


    static inline long IMULT_CEIL_TIME(long lIn_us, long lIncr_us)
    {
        if(lIn_us < 10000L)
        {
            return IMULT_CEIL(lIn_us,lIncr_us);
        }
        else if(lIn_us < 1000000L)
        {
            return IMULT_CEIL(lIn_us,10*lIncr_us);
        }
        else if(lIn_us < 100000000L)
        {
            return IMULT_CEIL(lIn_us,100*lIncr_us);
        }
        else
        {
             return IMULT_CEIL(lIn_us,1000*lIncr_us);
        }
    }



    //  Similar to STL's min
    template <class _Tp>
    inline const _Tp& SMALLER(const _Tp& __a, const _Tp& __b) {
      return __b < __a ? __b : __a;
    }

    //  Similar to STL's max
    template <class _Tp>
    inline const _Tp& LARGER(const _Tp& __a, const _Tp& __b) {
      return  __a < __b ? __b : __a;
    }


    //  Returns 2^uExp
    static inline unsigned raise2To(unsigned uExp)
    {
        return 0x1 << uExp;
    }

    //  Returns 2^i, where i is the smallest integer which satifies:
    //               2^i >= nIn
    static inline unsigned BASE2_CEIL(unsigned nIn)
    {
        if( nIn == 0 )
        {
            //  1 == 2^0 > 0
            return 1;
        }
        unsigned nExp = 0;
        while(nExp < 8*sizeof(unsigned))
        {
            if( (nIn >> nExp) == 0x1 )
            {
                return nIn == static_cast<unsigned>(0x1 << nExp) ? nIn : (0x1 << (nExp+1));
            }
            ++nExp;
        }
	    return 1;
    }

    //  Returns number of one bits
    template<class INT_TYPE>
    static inline int BIT_COUNT(INT_TYPE iIn)
    {
        int iRet = 0;
        for(;iIn != 0; iIn >>= 1)
        {
            if(iIn & 0x1) ++iRet;
        }
        return iRet;
    }

    //  Similar to strcpy.
    //  Returns pointer to terminating '\0' character instead of pszDest.
    static inline char* strcpyEx(register char* pszDest, register const char* pszSrc)
    {
        while(*pszSrc) *pszDest++ = *pszSrc++;
        *pszDest = '\0';
        return pszDest;
    }



    //  -----------------------------------------------------------------
    //  Calculates the sSliceData object (needed for the MDH) from the position
    //  vector 'rPosVect' [mm], the normal vector 'rNormVect', and the inplane
    //  rotation angle 'rdInPlaneRot_rad' [rad]. The result is written to the
    //  sSliceData objectet referenced by 'rDst'. If the function fails,
    //  the return value is false and long varaible 'rlNLSStatus' points to
    //  receives an appropriate NLS-status code. Otherwise true is returned.
    //  -----------------------------------------------------------------

    bool setSlcData(NLS_STATUS& rlNLSStatus, sSliceData& rDst, bool& rbSwapped, const VectorPat<double>& rPosVect, const VectorPat<double>& rNormVect, const double& rdInPlaneRot_rad);

    //  Similar to fSBBFillTimeRun, which can't be used to avoid dependencies to libSBB
    NLS_STATUS fRunFillTime_us(int32_t i32FillTime_us);

#ifdef _VB_LINE
    //  Similar to fSBBECGFillTimeRun, which reversed second and third argument.
    NLS_STATUS fWaitOnCardiacTrigger(sSYNC *psHALT,long lDelayPriorToTrigger_us,long lDelayAfterTrigger_us);
    NLS_STATUS fSliceMatPosOffc(sSLICE_POS& rSlcPos, double* pdSliceOffCenterPE, double* pdSliceOffCenterRO, double * pdSliceShift, int iPatDirection, int iPatPosition, const Slice &rSlice);
    NLS_STATUS fPrepISLICE_POS(sSLICE_POS& rSlcPos, const Slice &rSlice, double dPhOS, double dSlOS);
#else
    //  Similar to fSBBECGFillTimeRun, which reversed second and third argument.
    NLS_STATUS fWaitOnCardiacTrigger(ISYNC *psHALT,long lDelayPriorToTrigger_us,long lDelayAfterTrigger_us);
    NLS_STATUS fSliceMatPosOffc(ISLICE_POS* psp, double* pdSliceOffCenterPE, double* pdSliceOffCenterRO, double * pdSliceShift, int iPatDirection, int iPatPosition, const Slice &rSlice);
    NLS_STATUS fPrepISLICE_POS(ISLICE_POS* psp, const Slice &rSlice, double dPhOS, double dSlOS);
#endif

                           
    //                RT
    //              <--->                   RT
    //  RO:         <-DT1->               <--->
    //          Ampl1   +-+   <----DT2---->          Zero
    //                 /   \                  +----------+ Ampl3
    //                /     \                /       
    //               / M0_FC \             /      
    //  ------------+---+-+---+-----------+---+----------+----------------------
    //                     \                 /
    //                      \ M0_PP         /
    //                        \            /
    //                         +----------+  -Ampl2
    //                                    <-----TE3------>


    //  Calculates amplitude and duration of the first and second gradient, such that the first
    //  and the second moment is zero at Tim TE3 after the start of the third gradient.
    //  Additional input parameters are the amplitude of the third gradient dAmpl3_mT_m,
    //  the common ramp time of all gradients lRT_us and the maximum gradient amplitude dGradMaxAmpl_mT_m
    NLS_STATUS fCalcVC(long& rlDT1_us, double& rdAmpl1_mT_m, long &rlDT2_us, double& rdAmpl2_mT_m, long lTE3_us, double dAmpl3_mT_m, long lRT_us, double dGradMaxAmpl_mT_m);
}  //  namesppace PACE

#endif  // __MC_STDROUTINES_H
