//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\RNavScout.h
//  Version:
//   Author: CC_SEQUENCES CC_FMRI STEMAL8Q LANDWIPD
//     Date: n.a.
//
//     Lang: C++
//
//  Descrip: Definition of class RNavScout.
//           The class RNavScout realizes the abstract interface
//           define by the classes SBBScoutScan and INavScout.
//           Clients of SBB know only the interface class. This technology
//           decouples interfaces from implementations, thereby reducing
//           compilation dependencies between files.
//
//  -----------------------------------------------------------------

#ifndef __RNavScout_H
#define __RNavScout_H

//  -----------------------------------------------------------------
//  Used interfaces
//  -----------------------------------------------------------------

//  Base class definition
#include "MrServers/MrImaging/seq/common/libPace/INavScout.h"

//  Definition of class SeqBuildBlock2DExcitation
#include "MrServers/MrImaging/seq/common/libPace/SBB2DExcitation.h"

//  Definition of class C2DRectProfile
#include "MrServers/MrImaging/seq/common/libPace/SBB2DExcitationProfiles.h"

//  Definition of class sREADOUT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"

//  Definition of class sFREQ_PHASE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"

// Definition of sROT_MATRIX
#include "MrServers/MrMeasSrv/SeqIF/libRT/sROT_MATRIX.h"

//  Definition of class sSYNC_WAKEUP
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"

//  Definition of class SeqBuildBlock2DTrajBlippedEPI
#include "MrServers/MrImaging/seq/common/libPace/SBB2DExcitationTrajectories.h"

namespace PACE
{
    //  -----------------------------------------------------------------
    //  Definition of class SBB2DExcitEx
    //  -----------------------------------------------------------------

    class SBB2DExcitEx : public SeqBuildBlock2DExcitation
    {
        typedef SeqBuildBlock2DExcitation BASE_TYPE;
        typedef SBB2DExcitEx              MYTYPE;
    public:

#ifdef RTFB_ONLINE_MUTUAL_EXCLUSIVE
        //  Readout object for dummy ADC
        sREADOUT      m_sOnlineADC;

        bool          m_bInvokeOnline;
#endif

        SBB2DExcitEx(SBBList* pSBBList, char* ptIdent)
            : SeqBuildBlock2DExcitation(pSBBList, ptIdent)
#ifdef RTFB_ONLINE_MUTUAL_EXCLUSIVE
            , m_bInvokeOnline(false)
#endif
        {}

        //  Similar to SeqBuildBlock2DExcitation::run with the exception that it
        //  is possible to specify an offset time that is added to the start time
        //  of the SBB.
        //  Note:  The offset time is not included in the duration of the SBB.
        bool runEx(MrProt* pMrProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, sSLICE_POS* pSLC, long lOffsetTime_us);

        //  Note: Although since SeqBuildBlock2DExcitation::prep is not declared
        //        virtual explicitely it is virtual since SeqBuildBlock::prep
        //        is virtual.
        virtual bool prep (MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, Navigator* pNavigator);

    };


    //  -----------------------------------------------------------------
    //  Definition of SBB2DTrajBlippedEPIEx
    //  -----------------------------------------------------------------

    class SBB2DTrajBlippedEPIEx : public SeqBuildBlock2DTrajBlippedEPI
    {
    public:
        //  Constructor
        SBB2DTrajBlippedEPIEx(SBBList* pSBBList)
            : SeqBuildBlock2DTrajBlippedEPI(pSBBList)
        {}

        //	Runs trajectory's gradients.
        //  Similar to SeqBuildBlock2DTrajBlippedEPI::run with the exception that it
        //  is possible to specify an offset time.
        bool runEx (MrProt* pMrProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, sSLICE_POS* pSLC, long lOffsetTime_us);

        //  Similar to SeqBuildBlock2DTrajBlippedEPI::runTrajectory with the exception that it
        //  is possible to specify an offset time.
        bool runTrajectoryEx (MrProt* pMrProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, sSLICE_POS* pSLC, long lOffsetTime_us);

        //  Similar to SeqBuildBlock2DGradients::runRefocussingGradients with the exception that it
        //  is possible to specify an offset time.
        //	Inserts the refocussing gradients' events in the timing table
        bool runRefocussingGradientsEx (MrProt* pMrProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, sSLICE_POS* pSLC, long lOffsetTime_us);

    };

    //  -----------------------------------------------------------------
    //  Definition of class RNavScout
    //  -----------------------------------------------------------------

    class RNavScout : public INavScout
    {
    public:

        //	----------------------------------------------------
        //  Members that may be also inherited by base class SeqBuildBlock
        //  Start


        //  NLS-status code of last error.
        NLS_STATUS              m_lNLSStatus;

        //	Time needed for one execution of the SBB-Run function [us].
        //  long                    m_lSBBDurationPerRequest_us;

        //	Energy applied during one execution of the run-function [Ws].
        double                  m_dEnergyPerRequest_Ws;

        //	Number of requests of the run- function during one measurement.
        long                    m_lRequestsPerMeasurement;

        //  Sets NLS-Status code.
        inline void setNLSStatus (NLS_STATUS lStatus);

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

        //  Number of samples of readout event.
        long   m_lSamples;

        //  Real dwell time [ns]
        long   m_lRealDwellTime_ns;

        //  Flip angle of 2d excitation [deg].
        double m_dFlipAngle_deg;

        //  Current gradient mode
        SEQ::Gradients m_eGradMode;

        //  Maximum gradient performance used for the 2DRF-pulse.
        SEQ::Gradients m_eMaxGradPerf2DRF;

        //  Maximum gradient amplitude [mT/m].
        double m_dGradMaxAmpl_mT_m;

        //  Minimum gradient rise time [us/(mT/m)].
        double m_dGradMinRiseTime_usm_mT;

        //  The navigator itself.
        Navigator  m_sNav;

        //  Index of assigned navigator object in protocol.
        long       m_lPosNav;

        //  Slice data/Swap flag calculated during initialization
        //  (exception reprep) and passed to the MDH at run time.
        sSliceData  m_sSlcData;
        bool        m_bSwapped;

        //  Pointer to SeqLim and SeqExpo structure
        //  (Stored during prepPre and passed to SBB2DExcit during run)
        SeqLim*    m_pSeqLim;
        SeqExpo*   m_pSeqExpo;


        //	----------------------------------------------------
        //  Additional attributes
        //	----------------------------------------------------


        //  2D Excitation
        // Members for 2DRF excitation
        //C2DRectProfile                m_sRectProfile;
        C2DCircProfile                m_sCircProfile;
        SBB2DTrajBlippedEPIEx         m_sTrajectory;
        mutable SBB2DExcitEx          m_s2DExcit;

        //  Rotation matrix used for event block.
        //  The rotation matrix maps the logical coordinates (read, phase, slice)
        //  to the physical system of x-,y-,z-gradient axis.
        sROT_MATRIX                   m_sRotMatrix;

        //  Readout object
        sREADOUT                      m_sADC;

        //  Frequency/phase of the ADC
        sFREQ_PHASE                   m_sRXNCOSet;
        sFREQ_PHASE                   m_sRXNCONeg;


        //  Refocusing gradient (phase)
        sGRAD_PULSE                   m_sGradRefPh;

        //  Refocusing gradient (read)
        sGRAD_PULSE                   m_sGradRefRd;

        //  Extra trapezoidal gradient lope, needed if
        //  velocity compensation along the readout
        //  direction is used.
        sGRAD_PULSE                   m_sGradVC;

        //  Prephase gradient
        sGRAD_PULSE                   m_sGradPrephase;

        //  Readout gradient
        sGRAD_PULSE                   m_sGradRO;

        //  Wakeup event
        mutable sSYNC_WAKEUP          m_sWakeUp;

        //  Sidelope distance of the 2D-excitation
        long   m_l2DRFSidelopeDist_mm;

        //  Resolution in read direction of 2D-excitation
        double m_d2DRFResPhase_mm;

        //  Resolution in phase direction of 2D-excitation
        double m_d2DRFResRead_mm;

        //  Delay time between RF and gradients
        long   m_lRFDelay_us;

        //  Boolean flag indicating wether a velocity compensated
        //  readout gradient is used.
        bool   m_bVelComp;

        //  Readout OS-Factor (can be one or two)
        static long   s_lREADOUT_OS_FACTOR;


        //	----------------------------------------------------
        //  Member functions inherited by base class SBBScoutScan.
        //	----------------------------------------------------

        virtual void destroy();

        virtual bool init(MrProt* pProt, SeqLim* pSeqLim, int32_t lPosNav, int32_t lPosImaWndBegin, int32_t lPosImaWndEnd, int32_t lPosSearchWndBegin, const char* tIdent);

        virtual bool prep (MrProt* pExtProt, SeqLim* pExtSeqLim, SeqExpo* pExtSeqExpo);

        virtual bool reprep(const VectorPat<double>& rNewPos);

        virtual bool check(MrProt* pExtProt, SeqLim* pExtSeqLim, SeqExpo* pExtSeqExpo, SEQCheckMode*, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lPollInterv_us);

        virtual bool run(MrProt* pProt, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual bool first(int32_t& rlFeed_us, int32_t& rlRemain_us, MrProt* pProt, uint32_t ulCntrl, unsigned short aushFBPara[], size_t uFBParaSize, unsigned short aushOnlinePara[], size_t uOnlineParaSize, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us);

        virtual bool next(int32_t& rlFeed_us, int32_t& rlRemain_us);

        virtual bool maxSummedSuccessorDuration(int32_t& rlFirst_us, int32_t& rlSecond_us, int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual NLS_STATUS getNLSStatus() const;

        virtual double getEnergyPerRequest_Ws(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lDelay_us) const;

        virtual int32_t getDurationPerRequest_us(int32_t lOffsetTime_us, int32_t lSleepInterv_us, int32_t lPollInterv_us) const;

        virtual int32_t getMinOffsetForOnline_us() const;


        //	----------------------------------------------------
        //  Member functions inherited by base class INavScout
        //	----------------------------------------------------

        //	----------------------------------------------------
        //  Additional member functions
        //	----------------------------------------------------

        //  Constructor
        RNavScout();

        //  Additional entry point invoked at the beginning of prep, that copies the WIP_MEM_BLOCK parameters.
        //  Destinaded to hold the rest free from any WIP_MEM_BLOCK stuff.
        virtual bool prepPre(MrProt* pExtProt, SeqLim* pExtSeqLim, SeqExpo* pExtSeqExpo);

        //  Checks whether a given ALPHA, T pair can be realized.
        //  Additional input variables is the zeroth moment of the RO gradient
        //  (from the start of the readout gradient to the echo time) and the two
        //  gradient moments along the axis orthogonal to the readout axis that must
        //  be rewinded.
        bool checkVC(double dM0Ref1_usmT_m, double dM0Ref2_usmT_m, double dM0RO_usmT_m, double dALPHA, int32_t lT_us);

        //  Given the ramp-up time of the readout gradient and the ADC duration the member
        //  function calculates attributes of velocity compensation and prephase gradient
        //  such that the zeroth and first gradient moment vanishes at TE.
        bool calcVC(sGRAD_PULSE& rsGradFC, sGRAD_PULSE& rsGradPP, double dM0Ref1_usmT_m, double dM0Ref2_usmT_m, double dROAmpl_mT_m, int32_t lRORampUpTime_us, int32_t lADCDuratin_us);

        //  Executes a single event block that starts with a synchronization
        //  object. The duration of the event block is lDuration.
        bool runSync(int32_t lDuration_us);

    };

    //  -----------------------------------------------------------------
    //  Implementation of RNavScout inline members
    //  -----------------------------------------------------------------

    inline void RNavScout::setNLSStatus (NLS_STATUS lStatus)
    {
        m_lNLSStatus = lStatus;
    }
}  //  namespace PACE
#endif  //  __RNavScout_H
