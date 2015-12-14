//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\RGREScout.h
//  Version: \main\24
//   Author: CC_SEQUENCES CC_FMRI STEMAL8Q LANDWIPD
//     Date: 2011-09-14 16:28:27 +02:00
//
//     Lang: C++
//
//  Descrip: Definition of class RGREScout.
//           The class RGREScout realizes the abstract interface
//           define by the classes SBBScoutScan and IGREScout.
//           Clients of SBB know only the interface class. This technology
//           decouples interfaces from implementations, thereby reducing
//           compilation dependencies between files.
//
//  -----------------------------------------------------------------

#ifndef __RGREScout_H
#define __RGREScout_H

//  -----------------------------------------------------------------
//  Used interfaces
//  -----------------------------------------------------------------

//  Base class definition
#include "MrServers/MrImaging/seq/common/libPace/IGREScout.h"

//  Definition of MDH_SIZE_ONLINE_PARA
#include "MrServers/MrImaging/seq/common/libPace/FB.h"

//  Definition of class sREADOUT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"

//  Definition of class sFREQ_PHASE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"

//  Definition of class sRF_PULSE_SINC
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"

//  Definition of class sGRAD_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"

//  Definition of class sSLICE_POS
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSLICE_POS.h"

//  Definition of class sSYNC_WAKEUP
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"

namespace PACE
{
    //  -----------------------------------------------------------------
    //  Definition of class RGREScout
    //  -----------------------------------------------------------------
#ifdef IMPLEMENT_GRE_SCOUT
    class RGREScout : public IGREScout
    {
    public:
#ifdef _VB_LINE
        sRF_PULSE* m_pRF_alpha;
#else
        IRF_PULSE* m_pRF_alpha;
#endif

        Slice      m_sNav;

        double m_dPhOS;

        //	----------------------------------------------------     
        //  Members that may be also inherited by base class SeqBuildBlock
        //  Start

        //	Time needed for one execution of the SBB-Run function [us].
        //  long                    m_lSBBDurationPerRequest_us;

        //	Energy applied during one execution of the run-function [Ws].
        double                  m_dEnergyPerRequest_Ws;


        //  End
        //  Members that may be also inherited by base class SeqBuildBlock
        //	----------------------------------------------------
    
        //	----------------------------------------------------
        //  Protocol parameters initialized by prepPre
        //	----------------------------------------------------

        //  Time interval between end of ADC event and execution of wakeup event.
        //long m_lSleepInterv_us;

        //  Time interval between wakeup event and end of event block.
        //long m_lPollInterv_us;


        //  The PE Moment increment
        double m_dPEMomInc_usmT_m;



        //  Number of measured lines
        uint32_t  m_u32CLin;
        uint32_t  m_u32NLin;

        //  Parameters stored by first and used by next
        int32_t  m_i32OffsetTime_us;
        int32_t  m_i32SleepInterv_us;
        int32_t  m_i32PollInterv_us;
        uint32_t m_u32Cntrl;

        unsigned short m_aushOnlinePara[MDH_SIZE_ONLINE_PARA];
        unsigned short m_aushFBPara[MDH_SIZE_SCOUT_PARA];

        //  Slice data/Swap flag calculated during initialization
        //  (exception reprep) and passed to the MDH at run time.
        bool        m_bSwapped;
        sSliceData  m_sSlcData;

        //	----------------------------------------------------
        //  Additional attributes
        //	----------------------------------------------------

        //  Rotation matrix used for event block.
        //  The rotation matrix maps the logical coordinates (read, phase, slice)
        //  to the physical system of x-,y-,z-gradient axis.
        sSLICE_POS                    m_sSlcPos;

        //  Object used to set frequency and phase for RF pulse
        sFREQ_PHASE                   m_sRFSet;   
        //  Object used to reset synthesizer
        sFREQ_PHASE                   m_sRFNeg;

        //  Slice selection gradient
        sGRAD_PULSE                   m_sGSS;
    
        //  First/second lope of the flow compensated slice refocusing gradient
        sGRAD_PULSE                   m_sGSR;

        //  Spoil gradient along slice
        sGRAD_PULSE                   m_sGSSpoil;

        //  (P)hase-(E)ncoding/(P)hase-(R)ewinding gradient
        sGRAD_PULSE                   m_sGPE;
        sGRAD_PULSE                   m_sGPR;

        //  Readout object
        sREADOUT                      m_sADC;

        //  Frequency/phase of the ADC
        sFREQ_PHASE                   m_sRXNCOSet;
        sFREQ_PHASE                   m_sRXNCONeg;

        //  Prephase gradient
        sGRAD_PULSE                   m_sGradROPre;

        //  Readout gradient
        sGRAD_PULSE                   m_sGradRO;

        //  Wakeup event
        sSYNC_WAKEUP                  m_sWakeUp;

        //  Readout OS-Factor (can be one or two)
        static const int32_t          s_i32READOUT_OS_FACTOR;

        //  Prolonging of flat-top of slice refocusing gradient
        static const int32_t          s_i32ProlongGSSFlatTop_us;


        //	----------------------------------------------------
        //  Member functions inherited by base class SBBScoutScan.
        //	----------------------------------------------------

        virtual void destroy();

        virtual NLS_STATUS prep(const IGREScout::PARA* pPara);

        //virtual bool init(MrProt* pProt, SeqLim* pSeqLim, int32_t lPosNav, int32_t lPosImaWndBegin, int32_t lPosImaWndEnd, int32_t lPosSearchWndBegin, const char* tIdent);

        //virtual bool prep (MrProt* pExtProt, SeqLim* pExtSeqLim, SeqExpo* pExtSeqExpo);

        virtual NLS_STATUS reprep(const VectorPat<double>& rPos, const VectorPat<double>& rShift);

        //virtual bool check(MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, SEQCheckMode*, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS run(uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS first(int32_t& rlFeed_us, int32_t& rlRemain_us, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS next(int32_t& rlFeed_us, int32_t& rlRemain_us);
    
        virtual NLS_STATUS maxSummedSuccessorDuration(int32_t& rlFirst_us, int32_t& rlSecond_us, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;


        virtual double getEnergyPerRequest_Ws(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual int32_t getDurationPerRequest_us(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual int32_t getMinOffsetForOnline_us() const;


        //	----------------------------------------------------
        //  Member functions inherited by base class IGREScout
        //	----------------------------------------------------

        //	----------------------------------------------------
        //  Additional member functions
        //	----------------------------------------------------

        //  Constructor
        RGREScout();

        //  Additional entry point invoked at the beginning of prep, that copies the WIP_MEM_BLOCK parameters.
        //  Destinaded to hold the rest free from any WIP_MEM_BLOCK stuff.  
        //virtual bool prepPre(MrProt* pExtProt, SeqLim* pExtSeqLim, SeqExpo* pExtSeqExpo);

        //  Checks whether a given ALPHA, T pair can be realized.
        //  Additional input variables is the zeroth moment of the RO gradient
        //  (from the start of the readout gradient to the echo time) and the two
        //  gradient moments along the axis orthogonal to the readout axis that must
        //  be rewinded.
        //bool checkVC(double dM0Ref1_usmT_m, double dM0Ref2_usmT_m, double dM0RO_usmT_m, double dALPHA, long lT_us);

        //  Given the ramp-up time of the readout gradient and the ADC duration the member
        //  function calculates attributes of velocity compensation and prephase gradient
        //  such that the zeroth and first gradient moment vanishes at TE.
        //bool calcVC(sGRAD_PULSE& rsGradFC, sGRAD_PULSE& rsGradPP, double dM0Ref1_usmT_m, double dM0Ref2_usmT_m, double dROAmpl_mT_m, long lRORampUpTime_us, long lADCDuratin_us);

        //  Executes a single event block that starts with a synchronization
        //  object. The duration of the event block is lDuration.
        NLS_STATUS runSync(long lDuration_us);

        //  Acquires a single line. Line index is m_lCLin-m_lNLin/2
        //virtual bool run_kernel(MrProt* pProt, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

    };
#endif  //  IMPLEMENT_GRE_SCOUT
}
#endif  //  __RGREScout_H
