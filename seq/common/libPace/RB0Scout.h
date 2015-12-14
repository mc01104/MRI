//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\RB0Scout.h
//  Version: \main\9
//   Author: CC_SEQUENCES CC_FMRI STEMAL8Q LANDWIPD
//     Date: 2011-11-17 14:17:54 +01:00
//
//     Lang: C++
//
//  Descrip: Navigator which is used to detect physiologically induced
//           B0-fluctuations.
//
//  -----------------------------------------------------------------

#ifndef __RB0Scout_H
#define __RB0Scout_H

//  -----------------------------------------------------------------
//  Used interfaces
//  -----------------------------------------------------------------


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

//  Definition of class Slice
#include "MrServers/MrProtSrv/MrProt/MrSlice.h"

//  FLASH-Kernel
#include "MrServers/MrImaging/seq/common/MODULE/MODULE_IGREKernel.h"
//  IFillMDH Interface
#include "MrServers/MrImaging/seq/common/MODULE/MODULE_IFillMDH.h"

#ifdef USE_STL_VECTOR
#include <vector>
#else
#include "MrServers/MrImaging/seq/common/libPace/Array.h"
#endif

template <class TYPE>
class VectorPat;

namespace PACE
{
    //  -----------------------------------------------------------------
    //  Definition of class RB0Scout
    //  -----------------------------------------------------------------

    template<class __BASE_TYPE, class __PARA_TYPE>
        class RB0Scout : public __BASE_TYPE, public MODULE::IFillMDH
    {
    public:
        typedef __BASE_TYPE               BASE_TYPE;
        typedef __PARA_TYPE               PARA_TYPE;

        MODULE::IGREKernel*               m_pIKernel;

        //  -----------------------------------------------
        //  Parameters needed for reprep/simulation
        //  -----------------------------------------------

        //  Describes position and orientation of the slice
        Slice      m_sNav;

        //  Phase-oversampling
        double     m_dPhOS;

        //  Actual first TE and DeltaTE (second TE - first TE) from Kernel exports
        int32_t    m_i32TE1_us;
        int32_t    m_i32DeltaTE_us;

        //  -----------------------------------------------
        //  Parameters needed for run
        //  -----------------------------------------------

        //  Index of Coil select to be used
        uint32_t  m_u32CoilSelectIndex;
        
        //  Number of acquired phase encoding lines
        int32_t    m_i32NPELin;

        //  Phase encoding counter (needed for first/next mechanism)
        int32_t    m_i32CPELin;

        //  Parameters stored by first/run and used by next/operator()
        int32_t  m_i32OffsetTime_us;
        int32_t  m_i32SleepInterv_us;
        int32_t  m_i32PollInterv_us;
        uint32_t m_u32Cntrl;
        //  Desired simulated phase drift
        double   m_dDeltaPhs_deg;

        size_t   m_uFBParaSize;
        size_t   m_uOnlineParaSize;
        unsigned short m_aushOnlinePara[MDH_SIZE_ONLINE_PARA];
        unsigned short m_aushFBPara[MDH_SIZE_SCOUT_PARA];

        //  Slice data/Swap flag calculated during initialization
        //  (exception reprep) and passed to the MDH at run time.
        sSliceData           m_sSlcData;
        bool                 m_bSwapped;

        //  Run time variables to realize quadratic phase evolution:
        bool                 m_bRFSpoiling;
        //  Starting phase used for RF Spoiling
        //  Phase evolution Phi_0 = ePhi0, Phi_j = Phi_j-1 + j*Phi0, j = 1, 2, 3 . . . .
        double               m_dRFSpoilPhasePhi0_rad;
        //  Second argment for Phi_j = fmodf( Phi_j-1 + j*Phi0,dRFSpoilPhaseMax_rad)
        double               m_dRFSpoilPhaseMax_rad;
        //  RF-spoil phase of previous call
        double               m_dRFSpoilPhsPrev_rad;
        //  RF-spoil phase to be added to previous RF-Spoil phase to get the actual RF-spoil phase.
        double               m_dRFSpoilPhsInc_rad;
        //	----------------------------------------------------
        //  Additional attributes
        //	----------------------------------------------------

        //  Rotation matrix used for event block.
        //  The rotation matrix maps the logical coordinates (read, phase, slice)
        //  to the physical system of x-,y-,z-gradient axis.
        sSLICE_POS                    m_sSlcPos;

        //  Wakeup event
        sSYNC_WAKEUP                  m_sWakeUp;

        //	----------------------------------------------------
        //  Member functions inherited by base class SBBScoutScan.
        //	----------------------------------------------------

        virtual void destroy();

        virtual NLS_STATUS prep(const PARA_TYPE* pPara);

        virtual NLS_STATUS reprep(const VectorPat<double>& rPos, const VectorPat<double>& rShift);

        //virtual bool check(MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, SEQCheckMode*, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS run(uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS first(int32_t& rlFeed_us, int32_t& rlRemain_us, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual NLS_STATUS next(int32_t& rlFeed_us, int32_t& rlRemain_us);
        
        virtual NLS_STATUS maxSummedSuccessorDuration(int32_t& ri32First_us, int32_t& ri32Second_us, int32_t i32OffsetTime_us, int32_t i32SleepInterv_us, int32_t i32Delay_us) const;

        virtual double getEnergyPerRequest_Ws(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual int32_t getDurationPerRequest_us(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual int32_t getMinOffsetForOnline_us() const;

        //	----------------------------------------------------
        //  MODULE::IFillMDH interface
        //	----------------------------------------------------
        virtual NLS_STATUS operator()(MdhProxy& rDst, bool& rbADCEnable, uint32_t u32CEco_intra, uint32_t u32NEco_intra, uint32_t u32CEco_inter, uint32_t u32NEco_inter);


        //	----------------------------------------------------
        //  Member functions inherited by base class IB0Scout
        //	----------------------------------------------------

        //	----------------------------------------------------
        //  Additional member functions
        //	----------------------------------------------------

        //  Constructor
        RB0Scout();

        //  Just to avoid warning
        virtual ~RB0Scout();

        //  Executes a single event block that starts with a synchronization
        //  object. The duration of the event block is lDuration.
        NLS_STATUS runSync(int32_t i32Duration_us);
    };
}
#endif  //  __RB0Scout_H
