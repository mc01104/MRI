//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\IScout.h
//          Version: \main\32
//
//         Language: C++
//
//      Description: Definition of protocol class IScout.
//          Structs: 
//
//  -----------------------------------------------------------------

//  -----------------------------------------------------------------
//  Used Interfaces
//
//  Definition of class MdhProxy
#include "MrServers/MrMeasSrv/SeqIF/MDH/MdhProxy.h"

#ifdef USE_XPROTOCOL
//  Definition of class XProtocol
#include "MrServers/MrVista/include/Parc/XNode.h"

#include "ProtBasic/Interfaces/NavigatorDefines.h"
#ifndef BUILD_PLATFORM_LINUX
#include "MrServers/MrProtSrv/MrProt/MrSlice.h" 
#endif
#else  //  USE_XPROTOCOL not defined
//  Definition of class MrProt
#include "MrServers/MrProtSrv/MrProt/MrProt.h"
#include "MrServers/MrProtSrv/MrProt/MrProtArray.h"
#endif //  USE_XPROTOCOL

//  Definition of struct VectPat
#include "MrServers/MrImaging/seq/common/libPace/VectPat.h" 

//  Definition of class Navigator
#include "MrServers/MrProtSrv/MrProt/MrSlice.h"


//  Definition of ICE_PROGRAM_PARA_LEN
#ifdef _VB_LINE 
#include "MrServers/MrMeasSrv/SeqIF/SeqBuffer/MeasAqcDefs.h"
#else
#include "ProtBasic/Interfaces/MeasAqcDefs.h"
#endif

//  Definition of min, max
#include <algorithm>

//  Definition of ostream
#include <iostream>

//  Definition of NLS_STATUS
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"

//  -----------------------------------------------------------------
//  Forward Declararions
//  -----------------------------------------------------------------
//class XProtocol;
//class SeqLim;
//class MrProt;
//class SEQCheckMode;
class MdhProxy;
class Navigator;
class SEQData;

template <class TYPE>
class VectorPat;

#undef min
#undef max

#ifndef __IScout_H
#define __IScout_H

namespace PACE
{
    //  -----------------------------------------------------------------
    //  Definition of class IScout
    //  -----------------------------------------------------------------
    
    class IScout
    {
        
    public:
        
        enum
        {            
            //  Flags for the cntrl argument of the run-method:
            
            //  Indicates to pass MDH_ONLINE to eval info mask and pass the OnlineParameter to MDH
            SCOUT_INVOKE_ONLINE                   = 0x00000001
            //  Flag one ADC as 'RelevantForMeasTime' 
        ,   SCOUT_RELEVANT_FOR_MEAS_TIME          = 0x00000002
            //  If RF-spoiling is used: Reset phase cycling scheme
        ,   SCOUT_RESET_RF_PHASE                  = 0x00000004
            //  Execute scout as noise-adjust scan (i.e. skip RF)
        ,   SCOUT_NOISE_ADJ_SCAN                  = 0x00010000
            //  Start of a series of navigator sequences with time interval ScoutTR
        ,   SCOUT_STEADY_STATE_PREP               = 0x00020000
            //  No feedback expected
        ,   SCOUT_NO_FEEDBACK                     = 0x00040000
            //  Do not insert RTEvent of type sSYNC_WAKEUP
        ,   SCOUT_NO_WAKEUP                       = 0x00080000                
        };    

    
    //  Packs a series of signal points into MDH.
    //  Returns number of packed signal point to int32_t variable
    //  rlPacked points to. The returned value may be less than
    //  (pEnd-pEarliest), if the series cannot be stored in a single
    //  MDH. Additional input variables are the time stamp of
    //  the earliest signal point ulTimeEarliest and the temporal
    //  spacing between adjacent signal points ulSpacing.
    //static inline bool packSignal(MdhProxy& rMDH, int32_t& rlPacked, const unsigned short* pEarliest, const unsigned short* pEnd, uint32_t ulTimeEarliest, uint32_t ulSpacing);
    
    //  Retrieves the earliest signal point and the associated
    //  time stamp from the MDH.
    //static inline bool earliestSignal(unsigned short& rushSignal, uint32_t& rulTime, const MdhProxy& rMDH);
    
    //  Retrieves the latest signal point and the associated
    //  time stamp from the MDH.
    //static inline bool latestSignal(unsigned short& rushSignal, uint32_t& rulTime, const MdhProxy& rMDH);
    
    //  Retrieves the next signal point and the associated
    //  time stamp from the MDH. On input the uint32_t
    //  variable rulTime points to must be equal to the time
    //  value returned by the previous call to latestSignal.
    //  For the first call to nextSignal rulTime must
    //  be equal to the time value returned by earliestSignal.
    //  nextSignal fails, if the end of the series is reached.
    //static inline bool nextSignal(unsigned short& rushSignal, uint32_t& rulTime, const MdhProxy& rMDH);
    
    //  -------------------------------------------------------------
    //  IScout member functions
    //  -------------------------------------------------------------
    
    //  Virtual destructor
    virtual void destroy() = 0;
    
    //  Initialization of the SBB.
    //  Should be invoked once prior to preparation.
    //  Parameters:
    //  lPosNav:
    //    Index of the navigator object in the protocol structure
    //    that difines the navigator excitation.
    //  [lPosImaWndBegin,lPosImawndEnd[
    //    Indeces of the navigator object in the protocol structure
    //    that are assigned to the scout. (Needed only for initialization)
    //  [lPosSearchWndBegin,lPosSearchWndBegin+lPosImawndEnd-lPosImaWndBegin[
    //    Indeces that must be passed to maxTraCol/Lin_pix(.,.) to retrieve
    //    the search window assigned to a particular window.
    //  tIdent
    //    Prefix of the ident used for the excitation RF-pulse.
    //virtual bool init(MrProt* pProt, SeqLim* pSeqLim, int32_t lPosNav, int32_t lPosImaWndBegin, int32_t lPosImaWndEnd, int32_t lPosSearchWndBegin, const char* tIdent) = 0;
    
    //  Prepares the SBB.
    //	Note: the SBB must have been initialized.
    //virtual bool prep (MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo) = 0;
    
    //  If the member function is called after the SBB has been prepared the navigator
    //  image is measured at position sNewPos that is different from the position
    //  specified in the protocol passed to the foregoing prep call.
    virtual NLS_STATUS reprep(const VectorPat<double>& rPos, const VectorPat<double>& rShift) = 0;
    
    //	Gradient overflow check.
    //	Note: the SBB must have been initialized and prepared.
    //virtual bool check(MrProt* pMrProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, SEQCheckMode*, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) = 0;
    
    //	SBB runtime execution.
    //  Parameters:
    //  pProt
    //    Pointer to protocol buffer.
    //  ulCntrl
    //    Combination of one of the following bit flags:
    //    SCOUT_INVOKE_ONLINE
    //       Switch an extra ADC to invoke the online method of the
    //       ice program. In this case the duration of the ADC is
    //       included in the offset-time which must be long enough.
    //    SCOUT_RELEVANT_FOR_MEAS_TIME
    //       Flag Readout of the scout as relevant for measurement
    //       time.
    //  aushFBPara
    //    Array of uFBParaSize unsigned shorts which are written to
    //    the free MDH parameters of the readout event of the scout.
    //  uFBParaSize
    //    Number of array elements to copy. Must be equal to or less
    //    than IScout::MDH_SIZE_SCOUT_PARA.
    //  aushOnlinePara
    //    Free header parameters passed to the MDH of the extra ADC,
    //    if the cntrl-flag SCOUT_INVOKE_ONLINE is set.
    //  uOnlineParaSize
    //    Number of array elements to copy. Must be equal to or less
    //    than IScout::MDH_SIZE_ONLINE_PARA.
    //  lOffsetTime_us
    //    Extra offset time within first event block of SBB.
    //    This time is not included in the duration of the SBB.
    //  lSleepInterv_us
    //  lPollInterv_us
    //    After the last ADC a synchronization event followed by a pause
    //    interval is inserted. The duration of the pause is lDelay_us.
    //    The time interval between the last ADC and the synchronization
    //    event is lSleepInterv_us.
    //	Note: the SBB must have been initialized, prepared and checked.
    virtual NLS_STATUS run(uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) = 0;
    
    //  Similar to run except that the call is split into several event
    //  blocks. At the beginning of each event block a synchronization
    //  object with code SYNCODE_WAKEUP is inserted. The caller triggers
    //  the start of the first event block by calling first.
    //  First writes the duration of the first event block to rlFeed_us
    //  and the duration of the remaining event blocks (that have not yet
    //  been feed) to rlRemain_us. The caller waits until the GCU runs
    //  on the synchronization object and then starts the next event block
    //  via next. When the synchronization object of the second event
    //  block is seen by the DSP's the caller starts the next event block
    //  via next until the scaout call is finished (i.e. rlRemain == 0).
    virtual NLS_STATUS first(int32_t& rlFeed_us, int32_t& rlRemain_us, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) = 0;
    virtual NLS_STATUS next(int32_t& rlFeed_us, int32_t& rlRemain_us) = 0;
    
    //  Returns the duration of the two successive event blocks that
    //  maximize rlFirst_us+rlSecond_us in first/next calling mode.
    virtual NLS_STATUS maxSummedSuccessorDuration(int32_t& ri32First_us, int32_t& ri32Second_us, int32_t i32OffsetTime_us, int32_t i32SleepInterv_us, int32_t i32Delay_us) const = 0;
    
    
    //  Default Constructor.
    //IScout();
    
    //  Destructor.
    //~IScout();
    
    //  Returns the last-error code value, if one of the member function fails.
    //virtual NLS_STATUS getNLSStatus() const = 0;
    
    //  Returns pointer to the SeqLim buffer of the navigator scan.
    //  After the SBB has been initialized the buffer containes the parameter limits.
    //  Otherwise the return value is NULL.
    //  const SeqLim* getSeqLim() const;
    
    //  Returns pointer to the SeqExpo buffer of the navigator scan.
    //  After the SBB has been prepared the buffer contains data exported from the SBB.
    //  If the SBB hasn't been initialized the return value is NULL.
    //  If the SBB has been initialized but wasn't prepared the pointer is valid , but
    //  the content of the buffer the returned pointer points to is undefined.
    //  const SeqExpo* getSeqExpo() const;
    
    //	Returns the energy [Ws] of one execution of the SBB-run function.
    //	Note: The SBB must have been prepared. Otherwise the return value is undefined.
    virtual double getEnergyPerRequest_Ws(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const = 0;
    
    //	Returns the duration [us] of one execution of the SBB-run function.
    //	Note: The SBB must have been prepared. Otherwise the return value is undefined.
    virtual int32_t getDurationPerRequest_us(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const = 0;
    
    //  Returns the minimum offset time, that must be specified if the
    //  'Online'-flag MDH_MASK_ONLINE is specified. The return value
    //  is an integer multiple of the GRAD_RASTER_TIME.
    //	Note: The SBB must have been prepared. Otherwise the return value is undefined.
    virtual int32_t getMinOffsetForOnline_us() const = 0;
    
    //  Returns the minimum possible TR value [us].
    //  Note: The SBB must have been prepared. Otherwise the return value is undefined.
    //  int32_t getTRMin_us() const;
    
    };
    /*
#ifndef USE_XPROTOCOL
    //  Sets the 8 bits in rlDst starting at position iStart.
    //  Note:  Defined in global scope since the VXWORKS compiler does not
    //         support nested template functions.
    template <class TYPE>
        static inline TYPE setBits(MrProt* pProt, int iPos, int iStart, TYPE tIn)
    {
        int32_t& rlDst = pProt->NavigatorParam().data()->alFree[iPos];
        rlDst &= ~(0xff << iStart);
        rlDst ^= (static_cast<uint32_t>(tIn) << iStart);
        return tIn;
    }
#endif
    
#ifdef USE_XPROTOCOL
    static inline int16_t getShort(const XProtocol* pProt, int iPos, int iStart)
    {
        const int32_t i32Mask = (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(iPos);
        return static_cast<int16_t>((i32Mask >> iStart)&0xffff);
    }
#else
    static inline short getShort(const MrProt* pProt, int iPos, int iStart)
    {
        return static_cast<short>((pProt->NavigatorParam().data()->alFree[iPos] >> iStart)&0xffff);
    }
    static inline short setShort(MrProt* pProt, int iPos, int iStart, short shVal)
    {
        int32_t& rlDst = pProt->NavigatorParam().data()->alFree[iPos];
        rlDst &= ~(0xffff << iStart);
        rlDst ^= (static_cast<int32_t>(shVal) << iStart);
        return shVal;
    }
#endif



    //  -----------------------------------------------------------------
    //  Implementation of IScout inline members
    //  -----------------------------------------------------------------
    
    inline int32_t IScout::findFTLen(double dFTLen)
    {
        int32_t lFTLen = 2048;
        if(dFTLen < 32+1)
        {
            lFTLen = 2*(static_cast<int32_t>(dFTLen+1)/2); 
        }
        else if(dFTLen < 64+2)
        {
            lFTLen = 4*(static_cast<int32_t>(dFTLen+2)/4);
        }
        else if(dFTLen < 128+4)
        {
            lFTLen = 8*(static_cast<int32_t>(dFTLen+4)/8);
        }
        else if(dFTLen < 256+8)
        {
            lFTLen = 16*(static_cast<int32_t>(dFTLen+8)/16);
        }
        else if(dFTLen < 512+16)
        {
            lFTLen = 32*(static_cast<int32_t>(dFTLen+16)/32);
        }
        else if(dFTLen < 1024+32)
        {
            lFTLen = 64*(static_cast<int32_t>(dFTLen+32)/64);
        }
        else if(dFTLen < 2048+64)
        {
            lFTLen = 128*(static_cast<int32_t>(dFTLen+64)/128);
        }
        return lFTLen;
    }
    
    inline int32_t IScout::multipleOf(double dIn, int32_t nBase)
    {
        return dIn < 0
            ? nBase*(static_cast<int32_t>(dIn-nBase/2.)/nBase)
            : nBase*(static_cast<int32_t>(dIn+nBase/2.)/nBase);
    }
    
   
    
    inline bool IScout::setOnlinePara(MdhProxy& rMDH, const unsigned short aushPara[], size_t uSize)
    {
        if( uSize > MDH_NUMBEROFICEPROGRAMPARA )
        {
            return false;
        }
        for( unsigned short uCntr = 0; uCntr < uSize; ++uCntr )
        {
            rMDH.setIceProgramPara(uCntr,aushPara[uCntr]);
        }
        return true;
    }
    
    inline bool IScout::getOnlinePara(unsigned short aushPara[], const MdhProxy& rMDH, size_t uSize)
    {
        if( uSize > MDH_NUMBEROFICEPROGRAMPARA )
        {
            return false;
        }
        for( unsigned short uCntr = 0; uCntr < uSize; ++uCntr )
        {
            aushPara[uCntr] = const_cast<MdhProxy*>(&rMDH)->getIceProgramPara(uCntr);
        }
        return true;
    }
    
    inline bool IScout::setFBPara(MdhProxy& rMDH, const unsigned short aushPara[], size_t uSize)
    {
        size_t uEnd = std::min(uSize,size_t(MDH_FREEHDRPARA));
        unsigned short uCntr;
        
        for( uCntr = 0; uCntr < uEnd; ++uCntr )
        {
            if( !rMDH.setFreeParameterByIndex(uCntr, aushPara[uCntr]) )
            {
                return false;
            }
        }
        if( uCntr < uSize )
        {
            rMDH.setCidc(aushPara[uCntr++]);
        }
        return uCntr == uSize;
    }
    
    inline bool IScout::getFBPara(unsigned short aushPara[], const MdhProxy& rMDH, size_t uSize)
    {
        size_t uEnd = std::min(uSize,size_t(MDH_FREEHDRPARA));
        unsigned short uCntr;
        for( uCntr = 0; uCntr < uEnd; ++uCntr )
        {
            if( !rMDH.getFreeParameterByIndex(uCntr, aushPara[uCntr]) )
            {
                return false;
            }
        }
        if( uCntr < uSize )
        {
            aushPara[uCntr++] = rMDH.getCidc();
        }
        return uCntr == uSize;
    }
    
    
    
    
    
    
    

    
    inline bool IScout::getValidFlag(const PROT_TYPE* pProt, int32_t i32Mask)
    {
        const int32_t i32Val =
#ifdef USE_XPROTOCOL
            (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_VALID_FLAG);
#else
        pProt->NavigatorParam().data()->alFree[FREELNG_INDX_VALID_FLAG];
#endif
        return (i32Val & i32Mask) != 0;
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::setValidFlag(MrProt* pProt, uint32_t ulMask, bool bNewVal)
    {
        int32_t& rlDst = pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_VALID_FLAG];
        if(bNewVal)
        {
            rlDst |= ulMask;
        }
        else
        {
            rlDst &= ~ulMask;
        }
        return bNewVal;
    }
#endif
    
    
    //inline uint8_t IScout::version(const PROT_TYPE* pProt)
    //{
    //    return getBits(pProt,FREELNG_INDX_VERSION,FREELNG_SBIT_VERSION);
    //}
    
    //#ifndef USE_XPROTOCOL
    //inline IScout::Version IScout::setVersion(MrProt* pProt)
    //{
    //    return setBits(pProt,FREELNG_INDX_VERSION,FREELNG_SBIT_VERSION,VERSION_VA25);
    //}
    //#endif
    
    inline uint8_t IScout::context(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_CONTEXT,FREELNG_SBIT_CONTEXT);
    }
    
#ifndef USE_XPROTOCOL
    inline uint8_t IScout::context(MrProt* pProt, uint8_t  ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_CONTEXT,FREELNG_SBIT_CONTEXT,ucNewVal);
    }
#endif
    
    
    inline bool IScout::squarePixel(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_PEFT_LEN) == 0;
#else
        return pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_PEFT_LEN] == 0;
#endif
    }
    
    
    
    
    inline bool IScout::prepScansValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_PREP_SCANS);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::prepScansValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_PREP_SCANS,bNewVal);
    }
#endif
    
    
    inline int32_t IScout::fourierLines(const PROT_TYPE* pProt)
    {
        if( excitMode(pProt) & (IScout::EXCIT_MODE_2D_PENCIL|IScout::EXCIT_MODE_EXT) )
        {
            return 1L;
        }
        //  The number of Fourier lines must be reduced in Partial Fourier is on.
        //  (See MrKSpace::linesToMeasure)
        //  Partial Fourier is currently not supported.
        return phaseEncodingLines(pProt);
    }
    
    
    
#ifdef USE_XPROTOCOL
#ifndef BUILD_PLATFORM_LINUX
    
    inline bool IScout::navPhFoVAt(double& rdPhFoV, const XProtocol* pProt, int32_t lPos)
    {
        const int32_t lSize = (*pProt)["MEAS.sNavigatorArray.lSize"].ToLong();
        if( lPos < 0L || lPos >= lSize )
        {
            return false;
        }
        char pszTag[128];
        std::strstream sPhaseFOV(pszTag,sizeof(pszTag),ios::in|ios::out);
        sPhaseFOV << "MEAS.sNavigatorArray.asElm." << lPos << ".sCuboid.dPhaseFOV" << std::ends;
        rdPhFoV = (*pProt)[pszTag].ToDouble();
        return true;
    }
    inline bool IScout::navRdFoVAt(double& rdRdFoV, const XProtocol* pProt, int32_t lPos)
    {
        const int32_t lSize = (*pProt)["MEAS.sNavigatorArray.lSize"].ToLong();
        if( lPos < 0L || lPos >= lSize )
        {
            return false;
        }
        char pszTag[128];
        std::strstream sReadoutFOV(pszTag,sizeof(pszTag),ios::in|ios::out);
        sReadoutFOV << "MEAS.sNavigatorArray.asElm." << lPos << ".sCuboid.dReadoutFOV" << std::ends;
        rdRdFoV = (*pProt)[pszTag].ToDouble();
        return true;
    }
    inline bool IScout::navThickAt(double& rdThick, const XProtocol* pProt, int32_t lPos)
    {
        const int32_t lSize = (*pProt)["MEAS.sNavigatorArray.lSize"].ToLong();
        if( lPos < 0L || lPos >= lSize )
        {
            return false;
        }
        char pszTag[128];
        std::strstream sThickness(pszTag,sizeof(pszTag),ios::in|ios::out);
        sThickness << "MEAS.sNavigatorArray.asElm." << lPos << ".sCuboid.dThickness" << std::ends;
        rdThick = (*pProt)[pszTag].ToDouble();
        return true;
    }
#endif //  BUILD_PLATFORM_LINUX
#else
    inline bool IScout::navAt(Navigator& rDst, const MrProt* pProt, int32_t lPos)
    {
        const int32_t lSize = pProt->navigatorArray().size();
        if( lPos < 0L || lPos >= lSize )
        {
            return false;
        }
        rDst = pProt->navigatorArray()[lPos];
        return true;
    }
    
#endif

    inline bool IScout::calcPixelSize(const PROT_TYPE* pProt, double& rdPixelSizeCol_mm, double& rdPixelSizeLin_mm, int32_t lPos)
    {
        const int32_t lImaCol = IScout::imageColumns(pProt);
        if( lImaCol <= 0 )
        {
            return false;
        }
        if( excitMode(pProt) == EXCIT_MODE_2D_PENCIL )
        {
            double dThick_mm, dPhsFoV_mm;
            if( !navThickAt(dThick_mm,pProt,lPos) || !navPhFoVAt(dPhsFoV_mm,pProt,lPos) )
            {
                return false;
            }
            rdPixelSizeCol_mm = dThick_mm/lImaCol;
            //  Undefined, only projection images
            rdPixelSizeLin_mm = dPhsFoV_mm;
            
            return true;
        }
        const int32_t lPEFTLen = IScout::PEFTLen(pProt);
        if( lPEFTLen <= 0 )
        {
            return false;
        }
        double dPhFoV_mm, dRdFoV_mm;
        if( !navPhFoVAt(dPhFoV_mm,pProt,lPos) || !navRdFoVAt(dRdFoV_mm,pProt,lPos) )
        {
            return false;
        }
        rdPixelSizeCol_mm = dRdFoV_mm/lImaCol;
        rdPixelSizeLin_mm = (dPhFoV_mm*(1+IScout::phaseOS(pProt)))/lPEFTLen;
        return true;
    }
#endif  // BUILD_PLATFORM_LINUX    
    
    inline bool IScout::PEFTLenValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_PEFT_LEN);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::PEFTLenValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_PEFT_LEN,bNewVal);
    }
#endif
    
    //inline int32_t IScout::fourierColumns(const MrProt* pProt)
    //{
    //    return baseRes(pProt);
    //}
    
    inline bool IScout::minimizeTE(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_MINIMIZE_TE);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::minimizeTE(MrProt* pProt, bool bNewVal)
    {
        int32_t& rlDst = pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_EXTRA_SWITCH];
        if(bNewVal)
        {
            rlDst |= IScout::EXTRA_SWITCH_MASK_MINIMIZE_TE;
        }
        else
        {
            rlDst &= ~IScout::EXTRA_SWITCH_MASK_MINIMIZE_TE;
        }
        return bNewVal;
    }
#endif
    
    inline bool IScout::minimizeTR(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_MINIMIZE_TR);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::minimizeTR(MrProt* pProt, bool bNewVal)
    {
        int32_t& rlDst = pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_EXTRA_SWITCH];
        if(bNewVal)
        {
            rlDst |= IScout::EXTRA_SWITCH_MASK_MINIMIZE_TR;
        }
        else
        {
            rlDst &= ~IScout::EXTRA_SWITCH_MASK_MINIMIZE_TR;
        }
        return bNewVal;
    }
#endif
    
    inline int32_t IScout::TR_us(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_TR);
#else
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_TR];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::TR_us(MrProt* pProt, int32_t lNewVal_us)
    {
        return pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_TR] = lNewVal_us;
    }
#endif
    
    inline int32_t IScout::TE_us(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_TE);
#else
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_TE];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::TE_us(MrProt* pProt, int32_t i32NewVal_us)
    {
        return pProt->NavigatorParam().data()->alFree[IScout::FREELNG_INDX_TE] = i32NewVal_us;
    }
#endif
    
    
    inline bool IScout::asymmetricEchoAllowed(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_ASYMMETRIC_ECHO_ALLOWED);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::asymmetricEchoAllowed(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_ASYMMETRIC_ECHO_ALLOWED,bNewVal);
    }
#endif
    
    
    inline bool IScout::flipAngleValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_FLIP_ANGLE);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::flipAngleValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_FLIP_ANGLE,bNewVal);
    }
#endif
    
//    inline uint8_t IScout::feedbackMode(const PROT_TYPE* pProt)
//    {
//        return getBits(pProt,FREELNG_INDX_FEEDBACK_MODE,FREELNG_SBIT_FEEDBACK_MODE);
//    }
    
//#ifndef USE_XPROTOCOL
//    inline unsigned char IScout::feedbackMode(MrProt* pProt, unsigned char ucNewVal)
//    {
//        return setBits(pProt,FREELNG_INDX_FEEDBACK_MODE,FREELNG_SBIT_FEEDBACK_MODE,ucNewVal);
//    }
//#endif
    
    inline uint8_t IScout::send(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_SEND, FREELNG_SBIT_SEND);
    }
    
#ifndef USE_XPROTOCOL
    inline IScout::Send IScout::send(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_SEND,FREELNG_SBIT_SEND,static_cast<Send>(ucNewVal));
    }
#endif
    
    inline uint8_t IScout::sendCha(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_SEND_CHA, FREELNG_SBIT_SEND_CHA);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::sendCha(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_SEND_CHA,FREELNG_SBIT_SEND_CHA,ucNewVal);
    }
#endif
    
    inline bool IScout::scoutMode(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.ucScoutMode"].ToBool();
#else
        return pProt->NavigatorParam().isScoutModeOn();
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::scoutMode(MrProt* pProt, bool bNewVal)
    {
        return pProt->NavigatorParam().isScoutModeOn(bNewVal);
    }
#endif
    
    
    
    inline bool IScout::updateRefIma(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_UPDATE_REF_IMA);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::updateRefIma(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_UPDATE_REF_IMA,bNewVal);
    }
#endif
    
    //inline bool IScout::showCorrSliceData(const MrProt* pProt)
    //{
    //    return getExtraFlag(pProt,EXTRA_SWITCH_MASK_SHOW_CORR_SLICE_DATA);
    //}
    
    //inline bool IScout::showCorrSliceData(MrProt* pProt, bool bNewVal)
    //{
    //    return setExtraFlag(pProt,EXTRA_SWITCH_MASK_SHOW_CORR_SLICE_DATA, bNewVal);
    //}
    
    inline bool IScout::storeNavigatorIma(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_STORE_NAVIGATOR_IMA);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::storeNavigatorIma(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_STORE_NAVIGATOR_IMA, bNewVal);
    }
#endif
    
    inline bool IScout::logFile(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_LOG_FILE);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::logFile(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_LOG_FILE,bNewVal);
    }
#endif
    
    inline bool IScout::showOuterNav(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_SHOW_OUTER_NAV);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::showOuterNav(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_SHOW_OUTER_NAV, bNewVal);
    }
#endif
    
    inline bool IScout::verbose(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_VERBOSE);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::verbose(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_VERBOSE, bNewVal);
    }
#endif
    
    inline bool IScout::clxCombine(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_CLX_COMBINE);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::clxCombine(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_CLX_COMBINE, bNewVal);
    }
#endif
    
    
    
    inline bool IScout::seriesInterlInConc(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_SERIES_INTERL_IN_CONC);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::seriesInterlInConc(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_SERIES_INTERL_IN_CONC, bNewVal);
    }
#endif
    
    inline bool IScout::seriesInterlDescending(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_SERIES_INTERL_DESCENDING);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::seriesInterlDescending(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_SERIES_INTERL_DESCENDING, bNewVal);
    }
#endif
    
    inline bool IScout::velCompRead(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_VEL_COMP_READ);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::velCompRead(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_VEL_COMP_READ, bNewVal);
    }
#endif
    
    inline int32_t IScout::realDwellTime_ns(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_DWELL_TIME);
#else
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_DWELL_TIME];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::realDwellTime_ns(MrProt* pProt, int32_t lNewVal_ns)
    {
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_DWELL_TIME] = lNewVal_ns;
    }
#endif
#ifndef BUILD_PLATFORM_LINUX    
    
    
    inline uint8_t IScout::motionDir(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_MOTION_DIR,FREELNG_SBIT_MOTION_DIR);
    }
    
#ifndef USE_XPROTOCOL
    inline IScout::MotionDir IScout::motionDir(MrProt* pProt, MotionDir eNewVal)
    {
        return setBits(pProt,FREELNG_INDX_MOTION_DIR,FREELNG_SBIT_MOTION_DIR,eNewVal);
    }
#endif
    
    inline uint8_t IScout::moDeAlgo(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_MODE_ALGO,FREELNG_SBIT_MODE_ALGO);
    }
    
#ifndef USE_XPROTOCOL
    inline IScout::MoDeAlgo IScout::moDeAlgo(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_MODE_ALGO,FREELNG_SBIT_MODE_ALGO,static_cast<MoDeAlgo>(ucNewVal));
    }
#endif
    
    
    
   
    inline uint8_t IScout::RXGainMode(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_RXGAIN_MODE,FREELNG_SBIT_RXGAIN_MODE);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::RXGainMode(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_RXGAIN_MODE,FREELNG_SBIT_RXGAIN_MODE, ucNewVal);
    }
#endif
    
    
    

    
    
    inline int32_t IScout::respComp(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.lRespComp"].ToLong();
#else
        return pProt->NavigatorParam().RespComp();
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline IScout::RespComp IScout::respComp(MrProt* pProt, long iNewVal)
    {
        //  Note: static_cast from `SEQ::RspCompMode' to `IScout::RespComp' (and vice versa)
        //        declined by tornado compiler
        return RespComp( pProt->NavigatorParam().RespComp(SEQ::RspCompMode(iNewVal)) );
    }
#endif
    

    
    inline int32_t IScout::TRecoveryBeforeScout_us(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_TRECOVERY_BEFORE_SCOUT);
#else
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_TRECOVERY_BEFORE_SCOUT];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::TRecoveryBeforeScout_us(MrProt* pProt, int32_t lNewVal_us)
    {
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_TRECOVERY_BEFORE_SCOUT] = lNewVal_us;
    }
#endif
    
    inline bool IScout::TRecoveryBeforeScoutValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_TRECOVERY_BEFORE_SCOUT);
    }
#ifndef USE_XPROTOCOL
    inline bool IScout::TRecoveryBeforeScoutValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_TRECOVERY_BEFORE_SCOUT,bNewVal);
    }
#endif
    
    inline uint8_t IScout::maxGradPerf2DRF(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_2DRF_MAX_GRAD_PERF,FREELNG_SBIT_2DRF_MAX_GRAD_PERF);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::maxGradPerf2DRF(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_2DRF_MAX_GRAD_PERF,FREELNG_SBIT_2DRF_MAX_GRAD_PERF,ucNewVal);
    }
#endif
    
    inline uint8_t IScout::reorderScheme(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_REORDER,FREELNG_SBIT_REORDER);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::reorderScheme(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_REORDER,FREELNG_SBIT_REORDER,ucNewVal);
    }
#endif
    
    inline double IScout::relAcceptPosCol(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adAcceptPosition"].ToDouble(0);
#else
        return pProt->NavigatorParam().data()->adAcceptPosition[0];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::relAcceptPosCol(MrProt* pProt, double dNewVal)
    {
        return pProt->NavigatorParam().data()->adAcceptPosition[0] = dNewVal;
    }
#endif
    
    inline double IScout::relAcceptPosLin(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adAcceptPosition"].ToDouble(1);
#else
        return pProt->NavigatorParam().data()->adAcceptPosition[1];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::relAcceptPosLin(MrProt* pProt, double dNewVal)
    {
        return pProt->NavigatorParam().data()->adAcceptPosition[1] = dNewVal;
    }
#endif
    
    inline double IScout::minRiseTime_usm_mT(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adFree"].ToDouble(FREEDBL_INDX_MIN_RISE_TIME);
#else
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_MIN_RISE_TIME];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::minRiseTime_usm_mT(MrProt* pProt, double dNewVal)
    {
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_MIN_RISE_TIME] = dNewVal;
    }
#endif
    

    
    inline double IScout::phaseRes2DRF_mm(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adFree"].ToDouble(FREEDBL_INDX_2DRF_RES_PHASE);
#else
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_RES_PHASE];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::phaseRes2DRF_mm(MrProt* pProt, double dNewVal_mm)
    {
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_RES_PHASE] = dNewVal_mm;
    }
#endif
    
    inline double IScout::readRes2DRF_mm(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adFree"].ToDouble(FREEDBL_INDX_2DRF_RES_READ);
#else
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_RES_READ];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::readRes2DRF_mm(MrProt* pProt, double dNewVal_mm)
    {
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_RES_READ] = dNewVal_mm;
    }
#endif
    
    inline double IScout::sidelopeDist2DRF_mm(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.adFree"].ToDouble(FREEDBL_INDX_2DRF_SIDELOPE_DIST);
#else
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_SIDELOPE_DIST];
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline double IScout::sidelopeDist2DRF_mm(MrProt* pProt, double dNewVal_mm)
    {
        return pProt->NavigatorParam().data()->adFree[FREEDBL_INDX_2DRF_SIDELOPE_DIST] = dNewVal_mm;
    }
#endif
    
    inline int32_t IScout::RFDelay2DRF_us(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_2DRF_RF_DELAY);
#else
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_2DRF_RF_DELAY];
#endif
    }
#ifndef USE_XPROTOCOL
    inline int32_t IScout::RFDelay2DRF_us(MrProt* pProt, int32_t i32NewVal_us)
    {
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_2DRF_RF_DELAY] = i32NewVal_us;
    }
#endif
    
    inline bool IScout::RFDelay2DRFValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_2DRF_RF_DELAY);
    }
#ifndef USE_XPROTOCOL
    inline bool IScout::RFDelay2DRFValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_2DRF_RF_DELAY,bNewVal);
    }
#endif
    
    
    
    
    inline bool IScout::statisticScansValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_STATISTIC_SCANS);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::statisticScansValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_STATISTIC_SCANS,bNewVal);
    }
#endif
    
    //inline bool IScout::separateStatPhase(const MrProt* pProt)
    //{
    //	return getExtraFlag(pProt,EXTRA_SWITCH_MASK_SEPARATE_STAT_PHASE);
    //}
    
    //inline bool IScout::separateStatPhase(MrProt* pProt, bool bNewVal)
    //{
    //	return setExtraFlag(pProt,EXTRA_SWITCH_MASK_SEPARATE_STAT_PHASE,bNewVal);
    //}
    
    
#endif
    
    
    inline uint8_t  IScout::QRSDetection(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_QRS_DETECTION,FREELNG_SBIT_QRS_DETECTION);
    }
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::QRSDetection(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_QRS_DETECTION,FREELNG_SBIT_QRS_DETECTION,ucNewVal);
    }
#endif
    
    inline uint8_t IScout::triggerAlgo(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_TRIGGER_ALGO,FREELNG_SBIT_TRIGGER_ALGO);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::triggerAlgo(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_TRIGGER_ALGO,FREELNG_SBIT_TRIGGER_ALGO,ucNewVal);
    }
#endif
    
    inline int16_t IScout::respIntrvStatSize(const PROT_TYPE* pProt)
    {
        return getShort(pProt,FREELNG_INDX_RESP_INTRV_STAT_SIZE,FREELNG_SBIT_RESP_INTRV_STAT_SIZE);
    }
    
#ifndef USE_XPROTOCOL
    inline short IScout::respIntrvStatSize(MrProt* pProt, short shNewVal)
    {
        return setShort(pProt,FREELNG_INDX_RESP_INTRV_STAT_SIZE,FREELNG_SBIT_RESP_INTRV_STAT_SIZE, shNewVal);
    }
#endif
    
    inline int32_t IScout::shotsPerRespCycle(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return std::max(int32_t(1),(*pProt)["MEAS.sNavigatorPara.alFree"].ToLong(FREELNG_INDX_SHOTS_PER_RESP_CYCLE));
#else
        return std::max(int32_t(1),pProt->NavigatorParam().data()->alFree[FREELNG_INDX_SHOTS_PER_RESP_CYCLE]);
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::shotsPerRespCycle(MrProt* pProt, int32_t lNewVal)
    {
        return pProt->NavigatorParam().data()->alFree[FREELNG_INDX_SHOTS_PER_RESP_CYCLE] = lNewVal;
    }
#endif
    

    
    inline bool IScout::PTThresholdValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_PT_THRESHOLD);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::PTThresholdValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_PT_THRESHOLD,bNewVal);
    }
#endif
    
    
    
    inline bool IScout::triggerLockTimeValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_TRIGGER_LOCK_TIME);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::triggerLockTimeValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_TRIGGER_LOCK_TIME,bNewVal);
    }
#endif
    
    
    inline bool IScout::minSendIntrvValid(const PROT_TYPE* pProt)
    {
        return getValidFlag(pProt,VALID_FLAG_MIN_SEND_INTRV);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::minSendIntrvValid(MrProt* pProt, bool bNewVal)
    {
        return setValidFlag(pProt,VALID_FLAG_MIN_SEND_INTRV,bNewVal);
    }
#endif
    
    inline int32_t IScout::breatholdMeas(const PROT_TYPE* pProt)
    {
#ifdef USE_XPROTOCOL
        return (*pProt)["MEAS.sNavigatorPara.lBreathHoldMeas"].ToLong();
#else
        return pProt->NavigatorParam().data()->lBreathHoldMeas;
#endif
    }
    
#ifndef USE_XPROTOCOL
    inline int32_t IScout::breatholdMeas(MrProt* pProt, int32_t lNewVal)
    {
        return pProt->NavigatorParam().data()->lBreathHoldMeas = lNewVal;
    }
#endif
    
    inline bool IScout::RTCntrlBHMeas(const PROT_TYPE* pProt)
    {
        return getExtraFlag(pProt,EXTRA_SWITCH_MASK_RTCNTRL_BH_MEAS);
    }
    
#ifndef USE_XPROTOCOL
    inline bool IScout::RTCntrlBHMeas(MrProt* pProt, bool bNewVal)
    {
        return setExtraFlag(pProt,EXTRA_SWITCH_MASK_RTCNTRL_BH_MEAS,bNewVal);
    }
#endif
    
    inline uint8_t IScout::gatePrioZone(const PROT_TYPE* pProt)
    {
        return getBits(pProt,FREELNG_INDX_GATE_PRIO_ZONE,FREELNG_SBIT_GATE_PRIO_ZONE);
    }
    
#ifndef USE_XPROTOCOL
    inline unsigned char IScout::gatePrioZone(MrProt* pProt, unsigned char ucNewVal)
    {
        return setBits(pProt,FREELNG_INDX_GATE_PRIO_ZONE,FREELNG_SBIT_GATE_PRIO_ZONE,ucNewVal);
    }
#endif
   */ 

}  //  namespace PACE

#endif  //  __IScout_H
