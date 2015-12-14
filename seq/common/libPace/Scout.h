//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\Scout.h
//          Version: \main\23
//
//         Language: C++
//
//      Description: Definition of class Scout.
//          Structs: 
//
//  -----------------------------------------------------------------

//  -----------------------------------------------------------------
//  Used interfaces
//  -----------------------------------------------------------------

//  Definition of LOCATE_ERROR, ...
#include "MrServers/MrImaging/seq/common/libPace/StdRoutines.h"

//  Definition of class Array
//#include "MrServers/MrImaging/seq/common/libPace/Array.h"

//  Definition of MDH_...
#include "MrServers/MrImaging/seq/common/libPace/FB.h"

//  Definition of class IScout
#include "MrServers/MrImaging/seq/common/libPace/IScout.h"

//  Definition of SIM_MODE_OFF
#include "MrServers/MrImaging/seq/common/libPace/PACE_PROT.h"

//  Definition of class refSignal
#include "MrServers/MrImaging/seq/common/libPace/RefSignal.h"

//  Definition of INavScout (needed for expression pScout == m_pHandle->m_asScout[u32Scout].m_pINavScout)
//#include "MrServers/MrImaging/seq/common/libPace/INavScout.h"

//  Disable WARNING warning C4018: '>=' : signed/unsigned mismatch
#ifdef WIN32 
#pragma warning( disable : 4018 )
#endif

//  Definition of class sSYNC_HALT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"

//  Definition of class sREADOUT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"

//  Definition of class SEQSemaphore
#include "MrServers/MrMeasSrv/SeqIF/libRT/SEQSemaphore.h"

//  Definition of SEQU_NORMAL, ...
#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"

//  Definition of fRTGetUnityMatrix/sROT_MATRIXUnity
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRT.h"

// Definition of SEQData
#include "MrServers/MrMeasSrv/SeqIF/libMES/SEQData.h"

// Definition of TRACE_PUT macros
#include "MrCommon/MrNFramework/MrTrace/MPCUTrace/MPCUTrace.h"

//  Definition of mSEQTest
#include "MrServers/MrImaging/ut/libsequt.h"

#include "MrServers/MrMeasSrv/SeqIF/libRT/RTController.h"

//  Definition of template class string
#include <string>

//  Definition oftemplate class vector
#include <vector>

#ifdef WIN32
#pragma warning( default : 4018 )
#endif

#include <time.h>
#ifdef WIN32
#include <sys/timeb.h>
#endif



#ifndef __SCOUT_H
#define __SCOUT_H

#ifdef _VB_LINE
class SeqExpo;
namespace MrProtocolData
{
    typedef ::SeqExpo SeqExpo;
}
#else
namespace MrProtocolData
{
    class SeqExpo;
}
#endif
class SEQData;

#ifndef	M_PI
#define M_PI    3.14159265358979323846
#endif

namespace PACE
{
    class CHA;
    class SBBGREKernelEx;
    class IGREScout;
    class INavScout;
    class IB0Scout;
    struct GRESCOUT_PARA;
    struct B0SCOUT_PARA;
    struct NAVSCOUT_PARA;
    
    
    //  -----------------------------------------------------------------
    //  Definition of struct NAVSPEC
    //  -----------------------------------------------------------------
    /*
    struct NAVSPEC
    {
        typedef NAVSPEC MYTYPE;
        
        //  Index of the navigator object in the protocol structure
        //  that specifies the navigator 'slice'/pencil volume.
        uint32_t m_u32ExcitVol;
        
        //  [m_lImaWndBegin,m_lImaWndEnd[ are the indeces of those
        //  navigator objects that specify the image windows needed
        //  for evaluation.
        uint32_t m_u32ImaWndBegin;
        uint32_t m_u32ImaWndEnd;
        
        //  [m_lSearchWndBegin,m_lSearchWndBegin+m_lImaWndEnd-m_lImaWndBegin[
        //  are the search windows assigned to [m_lImaWndBegin,m_lImaWndEnd[
        uint32_t m_u32SearchWndBegin;
        
        //  Ident of the related excitation pulse.
        std::string m_pszRFExcitIdent;
        
        //  Default constructor
        NAVSPEC()
            : m_u32ExcitVol(0L)
            , m_u32ImaWndBegin(1L)
            , m_u32ImaWndEnd(2L)
            , m_u32SearchWndBegin(0L)
        {}
        
        //  Another constructor
        NAVSPEC(uint32_t u32ExcitVol, uint32_t u32ImaWndBegin, uint32_t u32ImaWndEnd, uint32_t u32SearchWndBegin, const char* pszRFExcitIdent)
            : m_u32ExcitVol(u32ExcitVol)
            , m_u32ImaWndBegin(u32ImaWndBegin)
            , m_u32ImaWndEnd(u32ImaWndEnd)
            , m_u32SearchWndBegin(u32SearchWndBegin)
        {
            m_pszRFExcitIdent = pszRFExcitIdent;
        }
        
        
        //  Copy constructor
        NAVSPEC(const MYTYPE& rRHS)
            : m_u32ExcitVol(rRHS.m_u32ExcitVol)
            , m_u32ImaWndBegin(rRHS.m_u32ImaWndBegin)
            , m_u32ImaWndEnd(rRHS.m_u32ImaWndEnd)
            , m_u32SearchWndBegin(m_u32SearchWndBegin)
        {
             m_pszRFExcitIdent = rRHS.m_pszRFExcitIdent;
        }
        
        //  Assignment operator
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(&rRHS != this)
            {
                m_u32ExcitVol    = rRHS.m_u32ExcitVol;
                m_u32ImaWndBegin = rRHS.m_u32ImaWndBegin;
                m_u32ImaWndEnd   = rRHS.m_u32ImaWndEnd;
                m_u32SearchWndBegin = rRHS.m_u32SearchWndBegin;
                m_pszRFExcitIdent = rRHS.m_pszRFExcitIdent;
            }
            return *this;
        }
        
    };
    */
   
    //  -----------------------------------------------------------------
    //  Definition of struct SCOUT
    //  -----------------------------------------------------------------
    
    struct SCOUT
    {
        
        //  Address of the scout object created during prepScoutSync.
        IScout*           m_pIScout;
        
        //  Used to avoid multiple realocations during UI-context.
        IGREScout*        m_pIGREScout;
        INavScout*        m_pINavScout;
        IB0Scout*         m_pIB0Scout;

        //  Vector in direction of the simulated motion
        //  Magnitude is equal to the maximum simulated motion.
        VectorPat<double> m_sShiftVect;

        //  Position vector of the navigator (needed for simulated motion)
        VectorPat<double> m_sPosVect;
        
        //  Specifies indices of scout and image window in the 
        //  navigator array of the protocol. Passed to the MDH.
        unsigned short    m_ushImaWnd;
        
        //  Constructor.
        SCOUT();
        
        //  Destructor.
        ~SCOUT();
        
       
        //  Initialization.
        //NLS_STATUS prep(MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo, const NAVSPEC& rasNavSpec);
        NLS_STATUS prep(const GRESCOUT_PARA* pGREScoutPara, unsigned short ushImaWnd, double dMaxSimTra_mm);
        NLS_STATUS prep(const B0SCOUT_PARA* pB0ScoutPara,   unsigned short ushImaWnd, double dMaxSimTra_mm);
#ifdef SUPPORT_NAV_SCOUT
        NLS_STATUS prep(const NAVSCOUT_PARA* pNavScoutPara, unsigned short ushImaWnd, double dMaxSimTra_mm, MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo);
#endif
    };

    //  -----------------------------------------------------------------
    //  Definition of class Scout
    //  -----------------------------------------------------------------

    struct SCOUT_PARA
    {
        //  See PACE::sleepIntervAfterScout_us
        //      PACE::pollIntervAfterScout_us
        //  for a description (Initialized by prep)
        int32_t              m_i32SleepIntervAfterScout_us;
        int32_t              m_i32PollIntervAfterScout_us;
        
        //  For simulation mode
        double            m_dMaxSimTra_mm;
        SEQ::PhysioSignal m_iRefSignal;
        unsigned char     m_ucSimMode;

        SCOUT_PARA()
            : m_i32SleepIntervAfterScout_us(10000)
            , m_i32PollIntervAfterScout_us (10000)
            , m_dMaxSimTra_mm(0)
            , m_iRefSignal(SEQ::SIGNAL_NONE)
            , m_ucSimMode(PACE::SIM_MODE_OFF)
        {}
    };

    
    class Scout : public SCOUT_PARA
    {
    public:
        
        enum
        {
            //  Low order bits: (passed one to one to IScout::run):
            INVOKE_ONLINE          = IScout::SCOUT_INVOKE_ONLINE,
                
                //  High order bits (reserved for future use
        };

        typedef SCOUT_PARA PARA;

                        
        //  The scout 'SBB' themselves.
        //  Note: m_asScout is NOT owner of its data.
        //        This avoids reallocation reallocation that will fail since
        //        the class SCOUT does not have an assignment operator.
        std::vector< SCOUT > m_asScout;
                
        //  Semaphore used to synchronize run and receive.
        //  The semaphore is acquired as int32_t as the feedback from the image
        //  reconstruction is pending.
        SEQSemaphore      m_sSyncIce;
        
        //  Readout object for dummy ADC
        sREADOUT          m_sOnlineADC;
        
        //  Synchronizes the feeding of timing instructions to the DSPs with the real-time clock.
        //  sSyncHalt creates a sSync event with code = SYNCCODE_WAKEUP, and duration zero.
        //  The ident is specified in the constructor.
        sSYNC_WAKEUP      m_sWakeUp;
        
        
        //  See IScout::sleepIntervAfterScout_us
        //      IScout::pollIntervAfterScout_us
        //  for a description (Initialized by prep)
        //int32_t              m_i32SleepIntervAfterScout_us;
        //int32_t              m_i32PollIntervAfterScout_us;
        
        //  Statistic data
        //  Minimum/maximum/total delay between end of the navigator scan and start of the imaging scan.
        //  (without extra ADC to send navigator images).
        //  (Set at run time)
        int32_t              m_i32DelayMin_us;
        int32_t              m_i32DelayMax_us;
        int32_t              m_i32DelayTotal_us;

        //  In the simulation mode the navigator image is acquired at a position that is shifted
        //  compared with the position stored in the protocol. This simulates motion.
        //uint8_t              m_u8SimMode;

        
        
        //  Reference Time
//#ifndef WIN32
//        struct timespec     m_sRefTime;
//#else
//        struct _timeb       m_sRefTime;
//#endif
        double m_dRefTime_ms;

        RefSignal  m_sRefSignal;

        // Maximum, minimum signal so far
        short      m_shRefSignalMin;
        short      m_shRefSignalMax;
        
        
        public:
            
            //   Default Constructor
            Scout();
                        
            
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            //  Preparartion
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            //  Protocol independent preparation
            NLS_STATUS prep(const PARA* pPARA, const std::vector< std::pair<GRESCOUT_PARA,unsigned short> >& rNavSpec);
            NLS_STATUS prep(const PARA* pPARA, const std::vector< std::pair<B0SCOUT_PARA,unsigned short> >&  rNavSpec);
            NLS_STATUS prep(const PARA* pPARA, const std::vector< std::pair<NAVSCOUT_PARA,unsigned short> >& rNavSpec, MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo);
            
            //  Must be called once before exeSync is invoked.
            //  Argument 4 of the second function is the address of an array that
            //  specifies the location of the geometrical scout data in the protocol
            //  structure (passed as argument 2).
            //  The first member of each array element is the index of the navigator
            //  object that specifies the navigator 'slice'/pencil volume. 
            //  [m_lImaWndBegin,m_lImaWndEnd[ are the indeces of those navigator
            //  objects that specify the image windows needed for evaluation.
            //  m_pszRFExcitIdent is the ident of the related excitation pulse.
            //  The size of the array is equal to the number of different scout object
            //  that can be used after preparation.
            //  The first function is equal to the second when an array with a single
            //  element (0,1,2,"PACE") is passed.
            //NLS_STATUS prep(MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo);
            //NLS_STATUS prep(MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo, const std::vector< NAVSPEC >& rasNavSpec);
            
            //  Must be called once before invokeOnline is invoked
            NLS_STATUS prepOnline();
            
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            //  Members that can be invoked after preparation
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            //  Writes the duration of one call to EXE_SYNC<.,.,.>::operator()
            //  to the int32_t variable ri32Duration_us points to, if the offset/delay
            //  time returned by PFCT_SET_FB_PARA is equal to lOffsetBeforeScout_us
            //  and lDelayAfterScout_us respectively and scout # u32Scout is used.
            NLS_STATUS  exeSyncDuration(int32_t& ri32Duration_us, uint32_t u32Scout, int32_t i32OffsetBeforeScout_us, int32_t i32DelayAfterScout_us) const;
            
            //	Writes the energy [Ws] exposured by one execution of
            //  EXE_SYNC<.,.,.>::operator()(.,.,u32Scout,...) to the double variable
            //  rdEnergy_Ws points to.
            NLS_STATUS  exeSyncEnergy(double& rdEnergy_Ws, uint32_t u32Scout, int32_t lOffsetBeforeScout_us, int32_t lDelayAfterScout_us) const;
             
            //  Writes the duration of one call to EXE_ASYNC<.,.,.>::operator()
            //  to the int32_t variable EXE_ASYNC points to, if the offset/delay
            //  time returned by PFCT_SET_FB_PARA is equal to i32OffsetBeforeScout_us
            //  and i32DelayAfterScout_us respectively and scout # u32Scout is used.
            NLS_STATUS  exeAsyncDuration(int32_t& ri32Duration_us, uint32_t u32Scout, int32_t i32OffsetBeforeScout_us, int32_t i32DelayAfterScout_us) const;
            
            //	Writes the energy [Ws] exposured by one execution of
            //  EXE_SYNC<.,.,.>::operator()(.,.,u32Scout,...) to the double variable
            //  rdEnergy_Ws points to.
            NLS_STATUS  exeAsyncEnergy(double& rdEnergy_Ws, uint32_t u32Scout, int32_t lOffsetBeforeScout_us, int32_t lDelayAfterScout_us) const;

            
            //  Writes the duration of one call to InvokeScout<.,.,.>::invokeOnline
            //  to the int32_t variable rlDuration_us points to, if lDuration_us
            //  is passed to the last argument of invokeOnline.
            NLS_STATUS  invokeOnlineDuration(int32_t& rlDuration_us, int32_t lDuration_us) const;
            
            //  Writes the minimum offset time needed, if the online method 
            //  of the ice program is used to rlOffset_us.
            NLS_STATUS  exeSyncMinOffsetToInvokeOnline(int32_t& rlOffset_us, uint32_t u32Scout) const;
            
            //  Returns the duration of the two successive event blocks that
            //  maximize rlFirst_us+rlSecond_us in EXE_CRUSH calling mode.
            NLS_STATUS  exeCrushMaxSummedSuccessorDuration(int32_t& rlFirst_us, int32_t& rlSecond_us, uint32_t u32Scout, int32_t lOffsetBeforeScout_us, int32_t lDelayAfterScout_us) const;
            
            
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            //  Members invoked at run-time
            //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            //  Traces feedback statistic to stdout
            void traceFBStat() const;
            
            //  Resets those members modified at run-time (e.g. statistic data,
            //  reference time)
            void reset();
            
            //  Creates a new event-block. The duration of the event-block is equal
            //  to lDuration_us [us]. The event block starts with a synchronization
            //  event with code SYNCCODE_WAKEUP and duration zero.
            //  Therefore the following boundary conditions must be fullfilled:
            //  i)  The duration must not be too small to ensure a sufficient delay
            //      till the end of the event block.
            //  ii) To each call to runWakeUpEvent there must be a related call to
            //      fRTWaitForWakeup.
            NLS_STATUS  runWakeUpEvent(int32_t lDuration_us);
            
    };

    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class SIMFB_PARA_TYPE>
        struct EXE_SYNC
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' exeSync is defined as template
        //  function object EXE_SYNC.
        typedef Scout HANDLE_TYPE;
        
        //    Prototype of a function that specifies the parameters passed
        //    to exeSync.
        //    pFBPara
        //      Arbitrary input parameters to the function.
        //    ru32FBCntrFlags
        //      The last 16 high oder bits are passed to the 32-bit control word of IScout::run.
        //    aushFBFlags
        //      Parameter passed to MDH of the READOUT event of the scout.
        //      Contains information for the HP/RT-feedback method of the ice program.
        //      The length ot the array is MDH_SIZE_SCOUT_PARA.
        //    lOffsetTime_us
        //      Additional time offset of the first event block of the SBB.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        //    lDelayAfterScout_us
        //      The last event block of the SBB is lengthen by this time interval.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        typedef NLS_STATUS (*PFCT_SET_FB_PARA)(FB_PARA_TYPE* const pFBPara, uint32_t& ru32FBCntrFlags, unsigned short aushFBFlags[], int32_t& rlOffsetBeforeScout_us, int32_t& rlDelayAfterScout_us);
        
        //    Prototype of a function that specifies the parameters passed to an
        //    extra ADC object. 
        //    pOnlinePara
        //      Arbitrary input parameters to the function.
        //    ru32CntrFlags
        //        You can specify a combination of the following bit flags.
        //        EXESCOUTSYNC_INVOKE_ONLINE
        //         Execute extra ADC to invoke online method of the
        //         ice program. If the flag is set the offset time must be
        //         at least IScout::getMinOffsetForOnline_us
        //    aushOnlineFlags
        //      Flags passed to MDH of the 'extra' ADC, if control-flag
        //      EXESCOUTSYNC_INVOKE_ONLINE is set.
        //      Contains information for the Online method of the ice program.
        //      The length ot the array is MDH_SIZE_ONLINE_PARA.
        typedef NLS_STATUS (*PFCT_SET_ONLINE_PARA)(ONLINE_PARA_TYPE* const pOnlinePara, uint32_t& ru32CntrFlags, unsigned short aushOnlineFlags[]);
        
        //   Prototype of a function that is used to simulate feedback when the
        //   sequence runs on the host.
        typedef NLS_STATUS (*PFCT_SIMFB)(SIMFB_PARA_TYPE* const pFBPara, const unsigned short aushFBFlags[]);
        
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        EXE_SYNC(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  Executes scout and waits on feedback.
        //  Parameters:
        //  u32Scout
        //    Index of the scout to use.
        //    Must be within [0, rasNavSpec.size()], where rasNavSpecis the last
        //    argument passed to prep.
        //  pFctSetFBPara
        //    See comment before definition of PFCT_SET_FB_PARA. 
        //  psFBPara
        //    Passed as first parameter to the function pFctSetFBPara points to.
        //  pFctSetOnlinePara
        //    See comment before definition of PFCT_SET_ONLINE_PARA.
        //  psOnlinePara
        //    Passed as first parameter to the function pFctSetOnlinePara points to.
        //    Both parameters may be NULL, if the parameters are not needed.   
        NLS_STATUS operator()(uint32_t u32Scout, PFCT_SET_FB_PARA pFctSetFBPara, FB_PARA_TYPE* psFBPara, PFCT_SET_ONLINE_PARA pFctSetOnlinePara, ONLINE_PARA_TYPE* psOnlinePara, PFCT_SIMFB pFctSimFB, SIMFB_PARA_TYPE* psSimFBPara);
    };


    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class SIMFB_PARA_TYPE>
        struct EXE_ASYNC
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' exeSync is defined as template
        //  function object EXE_SYNC.
        typedef Scout HANDLE_TYPE;
        
        //    Prototype of a function that specifies the parameters passed
        //    to exeSync.
        //    pFBPara
        //      Arbitrary input parameters to the function.
        //    ru32FBCntrFlags
        //      The last 16 high oder bits are passed to the 32-bit control word of IScout::run.
        //    aushFBFlags
        //      Parameter passed to MDH of the READOUT event of the scout.
        //      Contains information for the HP/RT-feedback method of the ice program.
        //      The length ot the array is MDH_SIZE_SCOUT_PARA.
        //    lOffsetTime_us
        //      Additional time offset of the first event block of the SBB.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        //    lDelayAfterScout_us
        //      The last event block of the SBB is lengthen by this time interval.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        typedef NLS_STATUS (*PFCT_SET_FB_PARA)(FB_PARA_TYPE* const pFBPara, uint32_t& ru32FBCntrFlags, unsigned short aushFBFlags[], int32_t& rlOffsetBeforeScout_us, int32_t& rlDelayAfterScout_us);
        
        //    Prototype of a function that specifies the parameters passed to an
        //    extra ADC object. 
        //    pOnlinePara
        //      Arbitrary input parameters to the function.
        //    ru32CntrFlags
        //        You can specify a combination of the following bit flags.
        //        EXESCOUTSYNC_INVOKE_ONLINE
        //         Execute extra ADC to invoke online method of the
        //         ice program. If the flag is set the offset time must be
        //         at least IScout::getMinOffsetForOnline_us
        //    aushOnlineFlags
        //      Flags passed to MDH of the 'extra' ADC, if control-flag
        //      EXESCOUTSYNC_INVOKE_ONLINE is set.
        //      Contains information for the Online method of the ice program.
        //      The length ot the array is MDH_SIZE_ONLINE_PARA.
        typedef NLS_STATUS (*PFCT_SET_ONLINE_PARA)(ONLINE_PARA_TYPE* const pOnlinePara, uint32_t& ru32CntrFlags, unsigned short aushOnlineFlags[]);
        
        //   Prototype of a function that is used to simulate feedback when the
        //   sequence runs on the host.
        typedef NLS_STATUS (*PFCT_SIMFB)(SIMFB_PARA_TYPE* const pFBPara, const unsigned short aushFBFlags[]);
        
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        EXE_ASYNC(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  Executes scout and waits on feedback.
        //  Parameters:
        //  u32Scout
        //    Index of the scout to use.
        //    Must be within [0, rasNavSpec.size()], where rasNavSpecis the last
        //    argument passed to prep.
        //  pFctSetFBPara
        //    See comment before definition of PFCT_SET_FB_PARA. 
        //  psFBPara
        //    Passed as first parameter to the function pFctSetFBPara points to.
        //  pFctSetOnlinePara
        //    See comment before definition of PFCT_SET_ONLINE_PARA.
        //  psOnlinePara
        //    Passed as first parameter to the function pFctSetOnlinePara points to.
        //    Both parameters may be NULL, if the parameters are not needed.   
        NLS_STATUS operator()(uint32_t u32Scout, PFCT_SET_FB_PARA pFctSetFBPara, FB_PARA_TYPE* psFBPara, PFCT_SET_ONLINE_PARA pFctSetOnlinePara, ONLINE_PARA_TYPE* psOnlinePara, PFCT_SIMFB pFctSimFB, SIMFB_PARA_TYPE* psSimFBPara);
    };

    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class CONTINUE_PARA_TYPE, class SIMFB_PARA_TYPE>
        struct EXE_CRUSH
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' exeSync is defined as template
        //  function object EXE_SYNC.
        typedef Scout HANDLE_TYPE;
        
        
        //    Prototype of a function that specifies the parameters passed
        //    to exeSync.
        //    pFBPara
        //      Arbitrary input parameters to the function.
        //    ru32CntrFlags
        //      Reserved for future use.
        //    aushFBFlags
        //      Parameter passed to MDH of the READOUT event of the scout.
        //      Contains information for the HP/RT-feedback method of the ice program.
        //      The length ot the array is MDH_SIZE_SCOUT_PARA.
        //    lOffsetTime_us
        //      Additional time offset of the first event block of the SBB.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        //    lDelayAfterScout_us
        //      The last event block of the SBB is lengthen by this time interval.
        //      This time interval is not included into the duration returned by
        //      expSyncDurationPerRequest_us.
        typedef NLS_STATUS (*PFCT_SET_FB_PARA)(FB_PARA_TYPE* const pFBPara, uint32_t& ru32CntrFlags, unsigned short aushFBFlags[], int32_t& rlOffsetBeforeScout_us, int32_t& rlDelayAfterScout_us);
        
        //    Prototype of a function that specifies the parameters passed to an
        //    extra ADC object. 
        //    pOnlinePara
        //      Arbitrary input parameters to the function.
        //    ru32CntrFlags
        //        You can specify a combination of the following bit flags.
        //        EXESCOUTSYNC_INVOKE_ONLINE
        //         Execute extra ADC to invoke online method of the
        //         ice program. If the flag is set the offset time must be
        //         at least IScout::getMinOffsetForOnline_us
        //    aushOnlineFlags
        //      Flags passed to MDH of the 'extra' ADC, if control-flag
        //      EXESCOUTSYNC_INVOKE_ONLINE is set.
        //      Contains information for the Online method of the ice program.
        //      The length ot the array is MDH_SIZE_ONLINE_PARA.
        typedef NLS_STATUS (*PFCT_SET_ONLINE_PARA)(ONLINE_PARA_TYPE* const pOnlinePara, uint32_t& ru32CntrFlags, unsigned short aushOnlineFlags[]);
        
        //    Prototype of a function that is called between the particular
        //    event blocks. Decides whether the next eventblock should be feed
        //    (return value rbContinue == true) or wether execution stops.
        //    Input parameters to the function:
        //    lFeed_us
        //        Time interval that has been feed when the function is invoked.
        //        If lFeed == 0 means that the function is invoked prior to
        //        first event block.
        //    lRemain_us
        //        Total time left that has not yet been feed to the DSP's.
        typedef NLS_STATUS (*PFCT_CONTINUE)(CONTINUE_PARA_TYPE* const pContinuePara, bool& rbContinue, int32_t lFeed_us, int32_t lRemain_us);
        
        //   Prototype of a function that is used to simulate feedback when the
        //   sequence runs on the host.
        typedef NLS_STATUS (*PFCT_SIMFB)(SIMFB_PARA_TYPE* const pFBPara, const unsigned short aushFBFlags[]);
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        EXE_CRUSH(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  Executes scout and waits on feedback.
        //  Parameters:
        //  u32Scout
        //    Index of the scout to use.
        //    Must be within [0, rasNavSpec.size()], where rasNavSpecis the last
        //    argument passed to prep.
        //  pFctSetFBPara
        //    See comment before definition of PFCT_SET_FB_PARA. 
        //  psFBPara
        //    Passed as first parameter to the function pFctSetFBPara points to.
        //  pFctSetOnlinePara
        //    See comment before definition of PFCT_SET_ONLINE_PARA.
        //  psOnlinePara
        //    Passed as first parameter to the function pFctSetOnlinePara points to.
        //    Both parameters may be NULL, if the parameters are not needed.
        //  pFctContinue
        //    See comment before definition of PFCT_CONTINUE. 
        //  psContinuePara
        //    Passed as first parameter to the function pFctContinue points to.
        //  pFctSimFB
        //    See comment before definition of PFCT_SIMFB.
        //  psSimFBPara
        //    Passed as first parameter to the function pFctSimFB points to.
        NLS_STATUS operator()(uint32_t u32Scout, PFCT_SET_FB_PARA pFctSetFBPara, FB_PARA_TYPE* psFBPara, PFCT_SET_ONLINE_PARA pFctSetOnlinePara, ONLINE_PARA_TYPE* psOnlinePara, PFCT_CONTINUE pFctContinue, CONTINUE_PARA_TYPE* psContinuePara, PFCT_SIMFB pFctSimFB, SIMFB_PARA_TYPE* psSimFBPara);
    };

    /*
    template<class FB_PARA_TYPE>
        struct CHECK
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' check is implemented as template
        //  function object CHECK.
        typedef Scout HANDLE_TYPE;
        
        //  For a description see EXE_SYNC::PFCT_SET_FB_PARA.
        typedef bool (*PFCT_SET_FB_PARA)(FB_PARA_TYPE* const pFBPara, uint32_t& ru32CntrFlags, unsigned short aushFBFlags[], int32_t& rlOffsetBeforeScout_us, int32_t& rlDelayAfterScout_us);
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        CHECK(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  Invokes check function of Scout at position u32Scout.
        //  Uses  pFctSetFBPara/psFBPara to retrieve offset and delay time.
        bool operator()(MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, SEQCheckMode* psMode, uint32_t u32Scout, PFCT_SET_FB_PARA pFctSetFBPara, FB_PARA_TYPE* psFBPara);
    };
    */
    
    template<class ONLINE_PARA_TYPE>
        struct INVOKE_ONLINE
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' invokeOnline is implemented as template
        //  function object INVOKE_ONLINE.
        typedef Scout HANDLE_TYPE;
        
        //  For a description see EXE_SYNC::PFCT_SET_ONLINE_PARA.
        typedef NLS_STATUS (*PFCT_SET_ONLINE_PARA)(ONLINE_PARA_TYPE* const pOnlinePara, uint32_t& ru32CntrFlags, unsigned short aushOnlineFlags[]);
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        INVOKE_ONLINE(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  Creates a new event-block. If the function pFctSetOnlinePara points to
        //  sets the INVOKE_ONLINE flag the event block executes one ADC event to
        //  invoke the online method of the ice program. Otherwise an empty event
        //  block is executed.
        //  The duration of the event block is the minimum of
        //  m_pHandle->m_sOnlineADC.getStartTime() + m_pHandle->m_sOnlineADC.getRoundedDuration()
        //  and lDuration_us.
        //  If the method is used the Scout member function prepOnline must have
        //  been invoked.
        NLS_STATUS operator()(PFCT_SET_ONLINE_PARA pFctSetOnlinePara, ONLINE_PARA_TYPE* psOnlinePara, int32_t lDuration_us);
    };

    /*
    template<class ONLINE_PARA_TYPE>
        struct USE_ONLINE
    {
        //  Since the ccppc does not support template member functions
        //  the Scout 'member function' useOnline is implemented as template
        //  function object USE_ONLINE.
        typedef Scout HANDLE_TYPE;
        
        //  For a description see EXE_SYNC::PFCT_SET_ONLINE_PARA.
        typedef NLS_STATUS(*PFCT_SET_ONLINE_PARA)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]);
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        USE_ONLINE(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  If the function pFctSetOnlinePara writes the INVOKE_ONLINE flag
        //  to the ru32CntrFlags argument the static member function passes
        //  the Online parameters to the MDH of the sREADOUT-object psADC
        //  points to. Otherwise the Online parameter of psADC->Mdh are cleared. 
        NLS_STATUS operator()(uint32_t& rulCntrl, sREADOUT* psADC, PFCT_SET_ONLINE_PARA pFctSetOnlinePara, ONLINE_PARA_TYPE* psOnlinePara);
    };
    */
    
    template<class EXTRACT_PARA_TYPE>
        struct EXTRACT
    {
        //  The function object must be used to extract data received from
        //  the ice program, if the ice program was triggered via EXE_SYNC.
        typedef Scout HANDLE_TYPE;
        
        
        //  Prototype of a function that is invoked when FB data from the
        //  ice program are received.
        typedef NLS_STATUS (*PFCT_EXTRACT)(EXTRACT_PARA_TYPE* const psExtractData,SEQData& rSeqData);
        
        //  Class attributes
        HANDLE_TYPE* m_pHandle;
        
        //  Constructor
        EXTRACT(Scout* pHandle)
            : m_pHandle(pHandle)
        {}
        
        //  pFctExtract
        //    See comment before definition of PFCT_EXTRACT.
        //  psExtractPara
        //    Passed as first argument to the function pFctExtract points to.
        NLS_STATUS operator()(SEQData& rSEQData, PFCT_EXTRACT pFctExtract, EXTRACT_PARA_TYPE* psExtractPara);
        //bool operator()(SEQData& rSeqData, PFCT_EXTRACT pFctExtract, EXTRACT_PARA_TYPE* psExtractPara);
    };
    
    /*
    template<class ONLINE_PARA_TYPE>
        NLS_STATUS USE_ONLINE<ONLINE_PARA_TYPE>::operator()(uint32_t& ru32CntrFlags, sREADOUT* psADC,NLS_STATUS(*pFctSetOnlinePara)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]), ONLINE_PARA_TYPE* psOnlinePara)
    {
        try
        {
            if( m_pHandle == 0 || pFctSetOnlinePara == 0 || psADC == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            ru32CntrFlags = 0;  
            unsigned short aushOnlinePara[MDH_SIZE_ONLINE_PARA];
            memset(aushOnlinePara,0,sizeof(aushOnlinePara));

            NLS_STATUS u64Res = (*pFctSetOnlinePara)(psOnlinePara,ru32CntrFlags,aushOnlinePara);
            
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            if( ru32CntrFlags & Scout::INVOKE_ONLINE )
            {
                //  Add mandatory flags
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] |= MDH_MASK_ONLINE;
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] &= ~MDH_MASK_ONLINE_BREAK;
            }
            
            //  Pass Online parameter to ADC
            if( !IScout::setOnlinePara(psADC->getMDH(), aushOnlinePara, sizeof(aushOnlinePara)/sizeof(unsigned short)) )
            {
                m_pHandle->setNLSStatus(SBB_ERROR);
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return false;
            }
            return true;
        }
        catch(...)
        {
            if(m_pHandle != NULL) m_pHandle->setNLSStatus( SBB_ERROR );
            CAUGHT_EXCEPTION(__FILE__,__LINE__);
            return false;
        }
    }

    template<class FB_PARA_TYPE>
        bool CHECK<FB_PARA_TYPE>::operator()(MrProt* pProt, SeqLim* pSeqLim, SeqExpo* pSeqExpo, SEQCheckMode* psMode, uint32_t u32Scout,bool(*pFctSetFBPara)(FB_PARA_TYPE*const,uint32_t&,unsigned short[],int32_t&,int32_t&), FB_PARA_TYPE* psFBPara)
    {
        try
        {
            if( m_pHandle == NULL || u32Scout < 0 || m_pHandle->m_asScout.size() <= u32Scout || !m_pHandle->m_asScout[u32Scout].m_pIScout )
            {
                if(m_pHandle != NULL) m_pHandle->setNLSStatus(SBB_ERROR);
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return false;
            }
            IScout* pScout = m_pHandle->m_asScout[u32Scout].m_pIScout;
            
            //  FB para
            uint32_t     u32CntrFlags = 0;
            int32_t lOffsetTime_us      = 0L;
            int32_t lDelayAfterScout_us = 0L;
            unsigned short    aushFBPara[MDH_SIZE_SCOUT_PARA];
            memset(aushFBPara,0,sizeof(aushFBPara));
            
            
            //  Set FB para
            if( pFctSetFBPara == NULL || !(*pFctSetFBPara)(psFBPara,u32CntrFlags,aushFBPara,lOffsetTime_us,lDelayAfterScout_us) )
            {
                m_pHandle->setNLSStatus(SBB_ERROR);
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return false;
            }
            
            //  Add navigator and ima window
            aushFBPara[MDH_INDX_SCOUT_WND] = m_pHandle->m_asScout[u32Scout].m_ushImaWnd;
            
            
            if( !pScout->check(pProt,pSeqLim,pSeqExpo,psMode,lOffsetTime_us,m_pHandle->m_i32SleepIntervAfterScout_us+lDelayAfterScout_us,m_pHandle->m_i32PollIntervAfterScout_us) )
            {
                m_pHandle->setNLSStatus(pScout->getNLSStatus());
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return false;
            }
            return true;
        }
        catch(...)
        {
            if(m_pHandle != NULL) m_pHandle->setNLSStatus( SBB_ERROR );
            CAUGHT_EXCEPTION(__FILE__,__LINE__);
            return false;
        }
    }
        */

    
    
    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class SIMFB_PARA_TYPE>
        NLS_STATUS EXE_SYNC<FB_PARA_TYPE,ONLINE_PARA_TYPE,SIMFB_PARA_TYPE>::operator()(uint32_t u32Scout, NLS_STATUS(*pFctSetFBPara)(FB_PARA_TYPE*const,uint32_t&,unsigned short[],int32_t&,int32_t&), FB_PARA_TYPE* psFBPara, NLS_STATUS(*pFctSetOnlinePara)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]), ONLINE_PARA_TYPE* psOnlinePara, NLS_STATUS(*pFctSimFB)(SIMFB_PARA_TYPE* const, const unsigned short[]), X86_ARGUMENT(SIMFB_PARA_TYPE*,pSimFBPara) )
    {
        try
        {
            //  Check arguments
            if( m_pHandle == 0 || m_pHandle->m_asScout.size() <= u32Scout || m_pHandle->m_asScout[u32Scout].m_pIScout == NULL || pFctSetFBPara == NULL )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            IScout* pScout = m_pHandle->m_asScout[u32Scout].m_pIScout;
                        
            
            //  FB para
            uint32_t     u32OnlineCntrlFlags   = 0;
            uint32_t     u32FBCntrlFlags = 0;
            int32_t lOffsetTime_us      = 0;
            int32_t lDelayAfterScout_us = 0;
            unsigned short    aushFBPara[MDH_SIZE_SCOUT_PARA];
            memset(aushFBPara,0,sizeof(aushFBPara));
            
            //  Set FB para
            NLS_STATUS u64Res = (*pFctSetFBPara)(psFBPara,u32FBCntrlFlags,aushFBPara,lOffsetTime_us,lDelayAfterScout_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Add navigator and ima window
            aushFBPara[MDH_INDX_SCOUT_WND] = m_pHandle->m_asScout[u32Scout].m_ushImaWnd;

            if( fRTIsReadoutEnabled() && ((u32FBCntrlFlags&IScout::SCOUT_NO_FEEDBACK) == 0) )
            {
                //  Flag that we are ready to receive the feedback.
                m_pHandle->m_sSyncIce.acquire(0);
            }
            else
            {
                //  There will be no feedback!
                m_pHandle->m_sSyncIce.release();
            }
            

            bool bRefSignalValid = false;
            if( (this->m_pHandle->m_iRefSignal != SEQ::SIGNAL_NONE) && lOffsetTime_us >= 1000 )
            {
                u64Res = this->m_pHandle->m_sRefSignal.run(this->m_pHandle->m_iRefSignal,aushFBPara[MDH_INDX_SCOUT_ID],1000);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                lOffsetTime_us -= 1000;
                bRefSignalValid = true;
            }

            //  Online parameter
            unsigned short    aushOnlinePara[MDH_SIZE_ONLINE_PARA];
            memset(aushOnlinePara,0,sizeof(aushOnlinePara));
            
            if( pFctSetOnlinePara != 0 && lOffsetTime_us >= pScout->getMinOffsetForOnline_us() )
            {
                u64Res = (*pFctSetOnlinePara)(psOnlinePara,u32OnlineCntrlFlags,aushOnlinePara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
            
            if( u32OnlineCntrlFlags & Scout::INVOKE_ONLINE )
            {
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] |= (MDH_MASK_ONLINE|MDH_MASK_ONLINE_BREAK);
            }

            //  Simulated motion
            if( m_pHandle->m_ucSimMode != PACE::SIM_MODE_OFF )
            {
                double dPhase = 0;
                if( bRefSignalValid )
                {
                    //  Retrieve number of samples
                    uint32_t u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,0,0);
                    if( u32NSamples > 0 )
                    {
                        short* pshSignalB = new short[u32NSamples];
                        u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,pshSignalB,pshSignalB+u32NSamples);
                        short* pshSignalC = pshSignalB, *pshSignalE = pshSignalB+u32NSamples;
                        for(;pshSignalC != pshSignalE;++pshSignalC)
                        {
                            if( *pshSignalC < this->m_pHandle->m_shRefSignalMin ) this->m_pHandle->m_shRefSignalMin = *pshSignalC;
                            if( *pshSignalC > this->m_pHandle->m_shRefSignalMax ) this->m_pHandle->m_shRefSignalMax = *pshSignalC;
                        }
                        if( this->m_pHandle->m_shRefSignalMax > this->m_pHandle->m_shRefSignalMin )
                        {
                            //  Inverse logic
                            dPhase = -2*(*(pshSignalE-1)-this->m_pHandle->m_shRefSignalMin)/double(this->m_pHandle->m_shRefSignalMax-this->m_pHandle->m_shRefSignalMin)+1;
                        }
                        delete [] pshSignalB;
                    }
                }
                else
                {
                    //  Retrieve current time
                    const double dTime_ms = RTController::getInstance().getAbsTimeOfEventBlockMSec()+lOffsetTime_us/1000.;
                    dPhase = m_pHandle->m_dRefTime_ms == -1
                        ? 0
                        : 1e-3*(dTime_ms-m_pHandle->m_dRefTime_ms)*2*M_PI
                        ;
                    if( m_pHandle->m_dRefTime_ms == -1 ) m_pHandle->m_dRefTime_ms = dTime_ms;
                    int32_t i32Exp = static_cast<int32_t>(m_pHandle->m_ucSimMode)-2;
                    if( i32Exp < 0 )
                    {
                        dPhase *= raise2To(-i32Exp);
                    }
                    else
                    {
                        dPhase /= raise2To(i32Exp);
                    }
                    dPhase = sin(dPhase);
                }
                
               
                //  Unit vector in direction of the simulated motion
                VectorPat<double> sShiftVect = m_pHandle->m_asScout[u32Scout].m_sShiftVect;
                
                sShiftVect *= dPhase;
                                                
                //  Shift navigator position to simulate motion.
                NLS_STATUS u64Res = pScout->reprep(m_pHandle->m_asScout[u32Scout].m_sPosVect,sShiftVect);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }

            //  Execute the scout
            u64Res = pScout->run((u32OnlineCntrlFlags&0x0000FFFF)|(u32FBCntrlFlags&0xFFFF0000), aushFBPara, sizeof(aushFBPara)/sizeof(unsigned short), aushOnlinePara, sizeof(aushOnlinePara)/sizeof(unsigned short), lOffsetTime_us, m_pHandle->m_i32SleepIntervAfterScout_us+lDelayAfterScout_us,m_pHandle->m_i32PollIntervAfterScout_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Avoid division by zero
            if( m_pHandle->m_i32PollIntervAfterScout_us == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            
            //  If feedback is pending more than 1 [s] terminate the sequence.
            int32_t lMaxPollIntervals = std::max(int32_t(0),int32_t((1000000-m_pHandle->m_i32SleepIntervAfterScout_us)/m_pHandle->m_i32PollIntervAfterScout_us));
            int32_t lCntr = 0;
            
            //  Stop further processing until the "WAKEUP" code is seen by the DSPs
            u64Res = fRTWaitForWakeup();
#ifdef TRACE_SYNC
#ifdef WIN32
            TRACE_PUT1(TC_INFO, TF_SEQ,"++ EXE_SYNC: fRTWaitForWakeup returned <%I64x>",u64Res);
#else
            TRACE_PUT1(TC_INFO, TF_SEQ,"++ EXE_SYNC: fRTWaitForWakeup returned <%lx>",u64Res);
#endif
#endif
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Check to see if any feedback has been received yet
            while( !m_pHandle->m_sSyncIce.acquire(0) )
            {
#ifdef TRACE_SYNC
                TRACE_PUT3(TC_INFO, TF_SEQ,"++ EXE_SYNC <%d>/<%d>: sSYNC_WAKEUP Duration = <%d> us",lCntr,lMaxPollIntervals,m_pHandle->m_i32PollIntervAfterScout_us);
#endif
                //  To avoid dead-locks we stop waiting after a predifined time-out interval
                if( lCntr == lMaxPollIntervals )
                {
                    //  In a final product version it may be reasonable to proceed without motion
                    //  correction in this case.
                    TRACE_PUT3(TC_ALWAYS,TF_SEQ,"Error at %s(%d): Feedback pending since <%d> us - time-out reached.",__FILE__,__LINE__,lCntr*m_pHandle->m_i32PollIntervAfterScout_us);
                    return SEQU_ERROR;
                }
                u64Res = m_pHandle->runWakeUpEvent(m_pHandle->m_i32PollIntervAfterScout_us);
                if( MrFailed(u64Res) ) 
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                u64Res = fRTWaitForWakeup();
#ifdef TRACE_SYNC
#ifdef WIN32
                TRACE_PUT1(TC_INFO, TF_SEQ,"++ EXE_SYNC: fRTWaitForWakeup returned <%I64x>",(NLS_STATUS)sNLSStatus);
#else
                TRACE_PUT1(TC_INFO, TF_SEQ,"++ EXE_SYNC: fRTWaitForWakeup returned <%lx>",(NLS_STATUS)sNLSStatus);
#endif
#endif
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                ++lCntr;
            }
            
            //  Determine feedback time
            const int32_t i32Delay = m_pHandle->m_i32SleepIntervAfterScout_us + (lCntr+1)*m_pHandle->m_i32PollIntervAfterScout_us;
            
            //  Update statistic data
            if( i32Delay > m_pHandle->m_i32DelayMax_us ) m_pHandle->m_i32DelayMax_us = i32Delay;
            if( i32Delay < m_pHandle->m_i32DelayMin_us ) m_pHandle->m_i32DelayMin_us = i32Delay;
            m_pHandle->m_i32DelayTotal_us += i32Delay;
            
#ifdef WIN32
            //  Simulate Feedback
            if( pFctSimFB != 0 )
            {
                u64Res = (*pFctSimFB)(pSimFBPara,aushFBPara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
#else
            //  To avoid warning ...
            pFctSimFB = 0;
#endif
            
            return true;
        }
        catch(...)
        {
            TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Caught unknown exception at %s(%d).",__FILE__,__LINE__);
            return SEQU_ERROR;
        }
    }


    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class SIMFB_PARA_TYPE>
        NLS_STATUS EXE_ASYNC<FB_PARA_TYPE,ONLINE_PARA_TYPE,SIMFB_PARA_TYPE>::operator()(uint32_t u32Scout, NLS_STATUS(*pFctSetFBPara)(FB_PARA_TYPE*const,uint32_t&,unsigned short[],int32_t&,int32_t&), FB_PARA_TYPE* psFBPara, NLS_STATUS(*pFctSetOnlinePara)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]), ONLINE_PARA_TYPE* psOnlinePara, NLS_STATUS(*pFctSimFB)(SIMFB_PARA_TYPE* const, const unsigned short[]), X86_ARGUMENT(SIMFB_PARA_TYPE*,pSimFBPara) )
    {
        try
        {
            //  Check arguments
            if( m_pHandle == 0 || m_pHandle->m_asScout.size() <= u32Scout || m_pHandle->m_asScout[u32Scout].m_pIScout == 0 || pFctSetFBPara == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            IScout* pScout = m_pHandle->m_asScout[u32Scout].m_pIScout;
                        
            
            //  FB para
            uint32_t     u32OnlineCntrlFlags   = 0;
            uint32_t     u32FBCntrlFlags = 0;
            int32_t lOffsetTime_us      = 0;
            int32_t lDelayAfterScout_us = 0;
            unsigned short    aushFBPara[MDH_SIZE_SCOUT_PARA];
            memset(aushFBPara,0,sizeof(aushFBPara));
            
            //  Set FB para
            NLS_STATUS u64Res = (*pFctSetFBPara)(psFBPara,u32FBCntrlFlags,aushFBPara,lOffsetTime_us,lDelayAfterScout_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Add navigator and ima window
            aushFBPara[MDH_INDX_SCOUT_WND] = m_pHandle->m_asScout[u32Scout].m_ushImaWnd;

            if( fRTIsReadoutEnabled() && ((u32FBCntrlFlags&IScout::SCOUT_NO_FEEDBACK) == 0) )
            {
                //  Flag that we are ready to receive the feedback.
                m_pHandle->m_sSyncIce.acquire(0);
            }
            else
            {
                //  There will be no feedback!
                m_pHandle->m_sSyncIce.release();
            }
            

            bool bRefSignalValid = false;
            if( (this->m_pHandle->m_iRefSignal != SEQ::SIGNAL_NONE) && lOffsetTime_us >= 1000 )
            {
                u64Res = this->m_pHandle->m_sRefSignal.run(this->m_pHandle->m_iRefSignal,aushFBPara[MDH_INDX_SCOUT_ID],1000);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                lOffsetTime_us -= 1000;
                bRefSignalValid = true;
            }

            //  Online parameter
            unsigned short    aushOnlinePara[MDH_SIZE_ONLINE_PARA];
            memset(aushOnlinePara,0,sizeof(aushOnlinePara));
            
            if( pFctSetOnlinePara != 0 && lOffsetTime_us >= pScout->getMinOffsetForOnline_us() )
            {
                u64Res = (*pFctSetOnlinePara)(psOnlinePara,u32OnlineCntrlFlags,aushOnlinePara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
            
            if( u32OnlineCntrlFlags & Scout::INVOKE_ONLINE )
            {
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] |= (MDH_MASK_ONLINE|MDH_MASK_ONLINE_BREAK);
            }

            //  Simulated motion
            if( m_pHandle->m_ucSimMode != PACE::SIM_MODE_OFF )
            {
                double dPhase = 0;
                if( bRefSignalValid )
                {
                    //  Retrieve number of samples
                    uint32_t u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,0,0);
                    if( u32NSamples > 0 )
                    {
                        short* pshSignalB = new short[u32NSamples];
                        u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,pshSignalB,pshSignalB+u32NSamples);
                        short* pshSignalC = pshSignalB, *pshSignalE = pshSignalB+u32NSamples;
                        for(;pshSignalC != pshSignalE;++pshSignalC)
                        {
                            if( *pshSignalC < this->m_pHandle->m_shRefSignalMin ) this->m_pHandle->m_shRefSignalMin = *pshSignalC;
                            if( *pshSignalC > this->m_pHandle->m_shRefSignalMax ) this->m_pHandle->m_shRefSignalMax = *pshSignalC;
                        }
                        if( this->m_pHandle->m_shRefSignalMax > this->m_pHandle->m_shRefSignalMin )
                        {
                            //  Inverse logic
                            dPhase = -2*(*(pshSignalE-1)-this->m_pHandle->m_shRefSignalMin)/double(this->m_pHandle->m_shRefSignalMax-this->m_pHandle->m_shRefSignalMin)+1;
                        }
                        delete [] pshSignalB;
                    }
                }
                else
                {
                    //  Retrieve current time
                    const double dTime_ms = RTController::getInstance().getAbsTimeOfEventBlockMSec()+lOffsetTime_us/1000.;
                    dPhase = m_pHandle->m_dRefTime_ms == -1
                        ? 0
                        : 1e-3*(dTime_ms-m_pHandle->m_dRefTime_ms)*2*M_PI
                        ;
                    if( m_pHandle->m_dRefTime_ms == -1 ) m_pHandle->m_dRefTime_ms = dTime_ms;
                    int32_t i32Exp = static_cast<int32_t>(m_pHandle->m_ucSimMode)-2;
                    if( i32Exp < 0 )
                    {
                        dPhase *= raise2To(-i32Exp);
                    }
                    else
                    {
                        dPhase /= raise2To(i32Exp);
                    }
                    dPhase = sin(dPhase);
                }
                
               
                //  Unit vector in direction of the simulated motion
                VectorPat<double> sShiftVect = m_pHandle->m_asScout[u32Scout].m_sShiftVect;
                
                sShiftVect *= dPhase;
                                                
                //  Shift navigator position to simulate motion.
                NLS_STATUS u64Res = pScout->reprep(m_pHandle->m_asScout[u32Scout].m_sPosVect,sShiftVect);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }

            //  Execute the scout
            u64Res = pScout->run((u32OnlineCntrlFlags&0x0000FFFF)|(u32FBCntrlFlags&0xFFFF0000), aushFBPara, sizeof(aushFBPara)/sizeof(unsigned short), aushOnlinePara, sizeof(aushOnlinePara)/sizeof(unsigned short),lOffsetTime_us,lDelayAfterScout_us,0);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
                        
#ifdef WIN32
            //  Simulate Feedback
            if( pFctSimFB != 0 )
            {
                u64Res = (*pFctSimFB)(pSimFBPara,aushFBPara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
#else
            //  To avoid warning ...
            pFctSimFB = 0;
#endif
            
            return true;
        }
        catch(...)
        {
            TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Caught unknown exception at %s(%d).",__FILE__,__LINE__);
            return SEQU_ERROR;
        }
    }

    template<class ONLINE_PARA_TYPE>
        NLS_STATUS INVOKE_ONLINE<ONLINE_PARA_TYPE>::operator()(NLS_STATUS(*pFctSetOnlinePara)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]), ONLINE_PARA_TYPE* psOnlinePara, int32_t lDuration_us)
    {
        try
        {
            if( m_pHandle == NULL || pFctSetOnlinePara == NULL )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            uint32_t u32OnlineCntrlFlags = 0;
            unsigned short    aushOnlinePara[MDH_SIZE_ONLINE_PARA];
            memset(aushOnlinePara,0,sizeof(aushOnlinePara));
            
            NLS_STATUS ui64Res = (*pFctSetOnlinePara)(psOnlinePara,u32OnlineCntrlFlags,aushOnlinePara);
            if( MrFailed(ui64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return ui64Res;
            }
            
            long lEBEnd_us = IMULT_CEIL(std::max(m_pHandle->m_sOnlineADC.getStartTime()+m_pHandle->m_sOnlineADC.getRoundedDuration(),long(lDuration_us)),GRAD_RASTER_TIME);
            
            //  Create new event block
#ifdef _VB_LINE
            fRTEBInit(&fRTGetUnityMatrix());
#else
            fRTEBInit(sROT_MATRIXUnity);
#endif
            
            if( u32OnlineCntrlFlags & Scout::INVOKE_ONLINE )
            {
                //  Add mandatory flags
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] |= MDH_MASK_ONLINE|MDH_MASK_ONLINE_BREAK;
                
                //  Pass flags to MDH of READOUT object.
                if( !PACE::setOnlinePara(MDH(m_pHandle->m_sOnlineADC), aushOnlinePara, sizeof(aushOnlinePara)/sizeof(unsigned short)) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return SEQU_ERROR;
                }
                fRTEI(m_pHandle->m_sOnlineADC.getStartTime(), &m_pHandle->m_sOnlineADC);
            }
            fRTEI(lEBEnd_us);

            //  Just to satisfy unit test self-check
            mSEQTest(0,0,0,RTEB_ORIGIN_fSBBOscBit,0,0,0,0,0);	    
            
            //  The status is checked only at the end of the event block
            ui64Res = fRTEBFinish();
            if( MrFailed(ui64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return ui64Res;
            }
            return SEQU_NORMAL;
        }
        catch(...)
        {
            TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Caught unknown exception at %s(%d).",__FILE__,__LINE__);
            return SEQU_ERROR;
        }
    }

    template<class EXTRACT_PARA_TYPE>
        NLS_STATUS EXTRACT<EXTRACT_PARA_TYPE>::operator()(SEQData& rSEQData, NLS_STATUS(*pFctExtract)(EXTRACT_PARA_TYPE*const,SEQData&), EXTRACT_PARA_TYPE* psExtractPara)
    {
        try
        {
            if( m_pHandle == 0 || pFctExtract == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            
            //  ----------------------------------------------------------
            //  Data from the image reconstruction
            //  ----------------------------------------------------------
            if( strncmp(rSEQData.getID(),"PACE",4) == 0 )
            {
                NLS_STATUS ui64Res = (*pFctExtract)(psExtractPara,rSEQData);
                if( MrFailed(ui64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return ui64Res;
                }
                //  Unreserve semaphore
                m_pHandle->m_sSyncIce.release();
                return SEQU_NORMAL;
            }
            //  We are not the receiver
            return SEQU_NORMAL;
        }
        catch(...)
        {
            TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Caught unknown exception at %s(%d).",__FILE__,__LINE__);
            return SEQU_ERROR;
        }
        
    };

    template<class FB_PARA_TYPE, class ONLINE_PARA_TYPE, class CONTINUE_PARA_TYPE,class SIMFB_PARA_TYPE>
        NLS_STATUS EXE_CRUSH<FB_PARA_TYPE,ONLINE_PARA_TYPE,CONTINUE_PARA_TYPE,SIMFB_PARA_TYPE>::operator()(uint32_t u32Scout, NLS_STATUS(*pFctSetFBPara)(FB_PARA_TYPE*const,uint32_t&,unsigned short[],int32_t&,int32_t&), FB_PARA_TYPE* psFBPara, NLS_STATUS(*pFctSetOnlinePara)(ONLINE_PARA_TYPE*const,uint32_t&,unsigned short[]), ONLINE_PARA_TYPE* psOnlinePara, NLS_STATUS(*pFctContinue)(CONTINUE_PARA_TYPE* const,bool&,int32_t,int32_t), CONTINUE_PARA_TYPE* psContinuePara, NLS_STATUS(*pFctSimFB)(SIMFB_PARA_TYPE* const, const unsigned short[]),X86_ARGUMENT(SIMFB_PARA_TYPE*,pSimFBPara) )
    {
        try
        {
            //  Check arguments
            if( m_pHandle == 0 || m_pHandle->m_asScout.size() <= u32Scout || m_pHandle->m_asScout[u32Scout].m_pIScout == 0 || pFctSetFBPara == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            IScout* pScout = m_pHandle->m_asScout[u32Scout].m_pIScout;
                        
            if( fRTIsReadoutEnabled()  )
            {
                //  Flag that we are ready to receive the feedback.
                m_pHandle->m_sSyncIce.acquire(0);
            }
            else
            {
                //  There will be no feedback!
                m_pHandle->m_sSyncIce.release();
            }
            
            //  FB para
            uint32_t     u32OnlineCntrlFlags   = 0;
            uint32_t     u32FBCntrlFlags = 0;
            int32_t lOffsetTime_us      = 0;
            int32_t lDelayAfterScout_us = 0;
            unsigned short    aushFBPara[MDH_SIZE_SCOUT_PARA];
            memset(aushFBPara,0,sizeof(aushFBPara));
            
            //  Set FB para
            NLS_STATUS u64Res = (*pFctSetFBPara)(psFBPara,u32FBCntrlFlags,aushFBPara,lOffsetTime_us,lDelayAfterScout_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Add navigator and ima window
            aushFBPara[MDH_INDX_SCOUT_WND] = m_pHandle->m_asScout[u32Scout].m_ushImaWnd;
            

            bool bRefSignalValid = false;
            if( (this->m_pHandle->m_iRefSignal != SEQ::SIGNAL_NONE) && lOffsetTime_us >= 1000 )
            {
                u64Res = this->m_pHandle->m_sRefSignal.run(this->m_pHandle->m_iRefSignal,aushFBPara[MDH_INDX_SCOUT_ID],1000);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                lOffsetTime_us -= 1000;
                bRefSignalValid = true;
            }

            //  Online parameter
            unsigned short    aushOnlinePara[MDH_SIZE_ONLINE_PARA];
            memset(aushOnlinePara,0,sizeof(aushOnlinePara));
            
            if( pFctSetOnlinePara != 0 && lOffsetTime_us >= pScout->getMinOffsetForOnline_us() )
            {
                u64Res = (*pFctSetOnlinePara)(psOnlinePara,u32OnlineCntrlFlags,aushOnlinePara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
            if( u32OnlineCntrlFlags & Scout::INVOKE_ONLINE )
            {
                aushOnlinePara[MDH_INDX_ONLINE_FLAGS] |= (MDH_MASK_ONLINE|MDH_MASK_ONLINE_BREAK);
            }

            //  Simulated motion
            if( m_pHandle->m_ucSimMode != PACE::SIM_MODE_OFF )
            {
                double dPhase = 0;
                if( bRefSignalValid )
                {
                    //  Retrieve number of samples
                    uint32_t u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,0,0);
                    if( u32NSamples > 0 )
                    {
                        short* pshSignalB = new short[u32NSamples];
                        u32NSamples = this->m_pHandle->m_sRefSignal.getLatestSignal(this->m_pHandle->m_iRefSignal,pshSignalB,pshSignalB+u32NSamples);
                        short* pshSignalC = pshSignalB, *pshSignalE = pshSignalB+u32NSamples;
                        for(;pshSignalC != pshSignalE;++pshSignalC)
                        {
                            if( *pshSignalC < this->m_pHandle->m_shRefSignalMin ) this->m_pHandle->m_shRefSignalMin = *pshSignalC;
                            if( *pshSignalC > this->m_pHandle->m_shRefSignalMax ) this->m_pHandle->m_shRefSignalMax = *pshSignalC;
                        }
                        if( this->m_pHandle->m_shRefSignalMax > this->m_pHandle->m_shRefSignalMin )
                        {
                            //  Inverse logic
                            dPhase = -2*(*(pshSignalE-1)-this->m_pHandle->m_shRefSignalMin)/double(this->m_pHandle->m_shRefSignalMax-this->m_pHandle->m_shRefSignalMin)+1;
                        }
                        delete [] pshSignalB;
                    }
                }
                else
                {
                    //  Retrieve current time
                    const double dTime_ms = RTController::getInstance().getAbsTimeOfEventBlockMSec()+lOffsetTime_us/1000.;
                    dPhase = m_pHandle->m_dRefTime_ms == -1
                        ? 0
                        : 1e-3*(dTime_ms-m_pHandle->m_dRefTime_ms)*2*M_PI
                        ;
                    if( m_pHandle->m_dRefTime_ms == -1 ) m_pHandle->m_dRefTime_ms = dTime_ms;
                    int32_t i32Exp = static_cast<int32_t>(m_pHandle->m_ucSimMode)-2;
                    if( i32Exp < 0 )
                    {
                        dPhase *= raise2To(-i32Exp);
                    }
                    else
                    {
                        dPhase /= raise2To(i32Exp);
                    }
                    dPhase = sin(dPhase);
                }               
                //  Unit vector in direction of the simulated motion
                VectorPat<double> sShiftVect = m_pHandle->m_asScout[u32Scout].m_sShiftVect;
                
                sShiftVect *= dPhase;
                                                
                //  Shift navigator position to simulate motion.
                NLS_STATUS u64Res = pScout->reprep(this->m_pHandle->m_asScout[u32Scout].m_sPosVect,sShiftVect);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
                                 
            //  Execute the scout
            int32_t lFeed_us   = 0L;
            int32_t lRemain_us = pScout->getDurationPerRequest_us(lOffsetTime_us, m_pHandle->m_i32SleepIntervAfterScout_us+lDelayAfterScout_us,m_pHandle->m_i32PollIntervAfterScout_us);
            bool bContinue  = true;
            
            u64Res = (pFctContinue)(psContinuePara,bContinue,lFeed_us,lRemain_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            if( !bContinue ) return SEQU_NORMAL;
            
            u64Res = pScout->first(lFeed_us,lRemain_us,(u32OnlineCntrlFlags&0x0000FFFF)|(u32FBCntrlFlags&0xFFFF0000),aushFBPara,sizeof(aushFBPara)/sizeof(unsigned short),aushOnlinePara,sizeof(aushOnlinePara)/sizeof(unsigned short),lOffsetTime_us,m_pHandle->m_i32SleepIntervAfterScout_us+lDelayAfterScout_us,m_pHandle->m_i32PollIntervAfterScout_us);
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Stop further processing until the "WAKEUP" code is seen by the DSPs
            u64Res = fRTWaitForWakeup();
            if( MrFailed(u64Res) )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return u64Res;
            }
            
            //  Avoid division by zero
            if( m_pHandle->m_i32PollIntervAfterScout_us == 0 )
            {
                TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                return SEQU_ERROR;
            }
            
            //  If feedback is pending more than 1 [s] terminate the sequence.
            int32_t lMaxPollIntervals = std::max(int32_t(0),int32_t((1000000-m_pHandle->m_i32SleepIntervAfterScout_us)/m_pHandle->m_i32PollIntervAfterScout_us));
            int32_t lCntr = 0;
            
            while( lRemain_us > 0 || !m_pHandle->m_sSyncIce.acquire(0) )
            {
                if(lRemain_us == 0)
                {
                    //  To avoid dead-locks we stop waiting after a predifined time-out interval
                    if( lCntr == lMaxPollIntervals )
                    {
                        //  In a final product version it may be reasonable to proceed without motion
                        //  correction in this case.
                        TRACE_PUT3(TC_ALWAYS,TF_SEQ,"Error at %s(%d): Feedback pending since <%d>  [us] - time-out reached.",__FILE__,__LINE__,lCntr*m_pHandle->m_i32PollIntervAfterScout_us);
                        return SEQU_ERROR;
                    }
                    lRemain_us = m_pHandle->m_i32PollIntervAfterScout_us;
                    ++lCntr;
                    
                }
                u64Res = (pFctContinue)(psContinuePara,bContinue,lFeed_us,lRemain_us);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                if( !bContinue ) return true;
                
                u64Res = pScout->next(lFeed_us,lRemain_us);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
                
                //  Stop further processing until the "WAKEUP" code is seen by the DSPs
                u64Res = fRTWaitForWakeup();
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
            
            //  Determine feedback time
            int32_t i32Delay = m_pHandle->m_i32SleepIntervAfterScout_us + (lCntr+1)*m_pHandle->m_i32PollIntervAfterScout_us;
            
            //  Update statistic data
            if( i32Delay > m_pHandle->m_i32DelayMax_us ) m_pHandle->m_i32DelayMax_us = i32Delay;
            if( i32Delay < m_pHandle->m_i32DelayMin_us ) m_pHandle->m_i32DelayMin_us = i32Delay;
            m_pHandle->m_i32DelayTotal_us += i32Delay;
            
#ifdef WIN32
            //  Simulate Feedback
            if( pFctSimFB != 0  )
            {
                u64Res = (*pFctSimFB)(pSimFBPara,aushFBPara);
                if( MrFailed(u64Res) )
                {
                    TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Error at %s(%d).",__FILE__,__LINE__);
                    return u64Res;
                }
            }
#else
            //  To avoid warning: unused parameter ...
            pFctSimFB = 0;
#endif
            return SEQU_NORMAL;
        }
        catch(...)
        {
            TRACE_PUT2(TC_ALWAYS,TF_SEQ,"Caught unknown exception at %s(%d).",__FILE__,__LINE__);
            return SEQU_ERROR;
        }
    }
}  //  namespace PACE

#endif
