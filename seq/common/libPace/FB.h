//  ---------------------------------------------------------
//   Copyright (C) Siemens AG 1999  All Rights Reserved.
//  ---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\FB.h
//	 Version: \main\5
//	  Author: CC_SEQUENCES STEMAL8Q
//	    Date: 2011-09-14 16:29:09 +02:00
//
//	    Lang: C++
//
//	 Descrip: (F)eed(B)ack:
//            Definitions used by image reconstruction and sequence.
//	 Classes:
//
//  ---------------------------------------------------------

#ifndef __FB_H
#define __FB_H
class MdhProxy;
class SEQData;

//  Definition of struct VectPat
#include "MrServers/MrImaging/seq/common/libPace/VectPat.h"

//  Definition of uint32_t, ...
#ifdef _VB_LINE
#include "MrCommon/MrGlobalDefinitions/MrBasicTypes.h"
#include "MrServers/MrMeasSrv/SeqIF/MDH/MdhProxy.h"
#else
#include "MrCommon/GlobalDefinitions/MrBasicTypes.h"
#endif

#ifdef max 
#undef max
#endif
#ifdef VXWORKS
#include "MrServers/MrImaging/seq/common/libPace/emulate_limits.h"
#else
#include <limits>
#endif

namespace PACE
{
#ifdef _VB_LINE
    //  Distinguishes PACE scans from non PACE scans.
    //  VD11A: Defined in MrServers/MrMeasSrv/SeqIF/MDH/MdhProxy.h
    const MdhBitField MDH_PACE(44UL);  

    //  Uses same bit flag as MDH_SLC_AVERAGED, but has different interpretation
    //  Marks those scans with number of averages (acquisitions) == ceil(dAverage)
    //  Scans with number of averages (acquisitions) == floor(dAverage) are not marked.
    const MdhBitField MDH_ACQ_CEIL(33);
#endif
    enum
    {
        //  Number of feedback para used. Must be increased
        //  if list of MDH_INDX_SCOUT_... defines is augmented.
        MDH_SIZE_SCOUT_PARA                    = 6
            
        //  Common
    ,   MDH_INDX_SCOUT_ID                      = 0
    ,   MDH_INDX_SCOUT_FLAGS                   = 1
    ,   MDH_INDX_SCOUT_FLAGS_L                 = MDH_INDX_SCOUT_FLAGS
    ,   MDH_INDX_SCOUT_FLAGS_H                 = 2
    ,   MDH_INDX_SCOUT_WND                     = 3
        
        //  Triggering
    ,   MDH_INDX_SCOUT_TRIGGER_PULSE           = 4
        
        //  Retrospective DVA algorithm
    ,   MDH_INDX_SCOUT_GATING_CHRON_ID         = 4

    //  Just for backward compatibility. Must be zero
    ,   MDH_INDX_SCOUT_CHA                     = 5,
        
        //  --------------------------------------------------
        //  BEGIN:  Common control flags (independent of selected
        //          motion compensation method)
        
        //  Phase correction scan.
        //  Instructs the IceProgram to determine the relative Phase between
        //  the different channels (needed for complex combine with phase corr.).
        MDH_MASK_SCOUT_PHASE_CORR              = 0x0001,
        
        //  Reference scan. 
        MDH_MASK_SCOUT_REF_SCAN                = 0x0002,
        
        //  If set the displacement (detected displacement + Offset value),
        //  is used as seed.
        MDH_MASK_SCOUT_USE_AS_SEED             = 0x0004,
        
        //  Scan executed to instruct the gating algorithm 
        MDH_MASK_SCOUT_STATISTIC_SCAN          = 0x0008,
        
        //  No feedback expected
        MDH_MASK_SCOUT_NO_FEEDBACK             = 0x0010,

        //  WARNING: MDH_MASK_SCOUT_CALC_MEDIAN_RESP_INTERV uses 0x0020
        
        //  Indicates that the flagged scan is the first
        //  one of a somehow regular sampling pattern.
        //  I.e. that the regular sampling pattern was
        //  interrupted between this and the previous
        //  scan.
        //MDH_MASK_SCOUT_SAMPLING_PATTERN_START  = 0x0020,

        
        //  No displacement information can be retrieven from
        //  the scout
        //MDH_MASK_SCOUT_NO_DISPL                = 0x0020,
        
        //  Return shift vector relative to seed.
        MDH_MASK_SCOUT_CALC_SHIFT              = 0x0040,
        
        //  Clears any previous stored reference image
        MDH_MASK_SCOUT_CLEAR_REF_IMA           = 0x0080,
        
        //  END:    Common control flags (independent of selected
        //          motion compensation method)
        //  --------------------------------------------------
        
        
        
        //  --------------------------------------------------
        //  BEGIN:  Flags needed to control profile image
        
        //  Mask of flags that control the profile image
        MDH_MASK_SCOUT_IMA_FLAGS               = 0x0f00,
        
        //   Start bit of image flags
        MDH_SBIT_SCOUT_IMA_FLAGS               = 8,
        
        //  Scan that is marked with a special colour
        //  (e.g. scan during imaging scan or immediately before or after the imaging scan)
        //  If double triggering is used the flag is used to mark
        //  the trigger points in the online display. 
        MDH_MASK_SCOUT_STRESS                  = 0x0100,
        
        //  Draw reference line
        MDH_MASK_SCOUT_REF_LINE                = 0x0200,
        
        //  Draw sensitivity window
        MDH_MASK_SCOUT_SENS_WINDOW             = 0x0400,
        
        //  Draw aceept window and seed position
        MDH_MASK_SCOUT_ACCEPT_WINDOW           = 0x0800,
        
        //  END:  Flags needed to control profile image
        //  --------------------------------------------------
        
        //  If a non-trivial series of motion vaues corresponds
        //  to one feedback-call (extrenal motion sensor):
        //  Masks those bit-flags of the scout call that correspond
        //  to earliest/latest/intermediate motion states.
        MDH_MASK_SCOUT_ALL       = MDH_MASK_SCOUT_STATISTIC_SCAN|MDH_MASK_SCOUT_NO_FEEDBACK|(MDH_MASK_SCOUT_IMA_FLAGS&(~MDH_MASK_SCOUT_STRESS)),
        MDH_MASK_SCOUT_EARLIEST  = MDH_MASK_SCOUT_ALL,
        MDH_MASK_SCOUT_LATEST    = MDH_MASK_SCOUT_ALL|MDH_MASK_SCOUT_IMA_FLAGS,
        
        //  --------------------------------------------------
        //  BEGIN:  Flags needed for BH&Follow algorithm
        
        //  Instruct ice program to check whether the absolute value of the 
        //  difference between the displacement (detected displacement + Offset value)
        //  and the seed, is less than the acceptance window. The result is
        //  send with the feedback.
        MDH_MASK_SCOUT_FOLLOW_GATE             = 0x2000,
        
        MDH_MASK_SCOUT_FOLLOW_ALL      = MDH_MASK_SCOUT_ALL,
        MDH_MASK_SCOUT_FOLLOW_EARLIEST = MDH_MASK_SCOUT_EARLIEST,
        MDH_MASK_SCOUT_FOLLOW_LATEST   = MDH_MASK_SCOUT_LATEST|MDH_MASK_SCOUT_FOLLOW_GATE,
        
        
        
        //  END:  Flags needed for the BH&Follow algorithm
        //  --------------------------------------------------
        
        //  --------------------------------------------------
        //  BEGIN:  Definitions used for gating
        //
        
        //  Clear previously stored data
        MDH_MASK_H_SCOUT_GATE_RESET                      = 0x0001,
        
        //  Return priority of profile (ROPE) to be measured next.
        MDH_MASK_H_SCOUT_GATE_DECIDE                     = 0x0002,

        //  Request to assign the latest measured physiological
        //  signal point to a specified chronological ID.
        MDH_MASK_H_SCOUT_GATE_ASSIGN_PSIGNAL_TO_CHRON_ID = 0x0004,
        
        //  Gating after Trigger:
        //  Return priority of profile (ROPE) if and only if a trigger
        //  was generated.
        MDH_MASK_H_SCOUT_GATE_DECIDE_AFTER_TRIGGER       = 0x0008,

        //  Return number of detected respiratory intervals.
        //MDH_MASK_SCOUT_GATE_CALC_RESP_INTERV        = 0x8000,
        
        
        
        //  See MDH_MASK_SCOUT_EARLIEST, ...
        //MDH_MASK_SCOUT_GATE_ALL      = MDH_MASK_SCOUT_ALL,
        //MDH_MASK_SCOUT_GATE_EARLIEST = MDH_MASK_SCOUT_EARLIEST|MDH_MASK_SCOUT_GATE_RESET,
        //MDH_MASK_SCOUT_GATE_LATEST   = MDH_MASK_SCOUT_LATEST  |MDH_MASK_SCOUT_GATE_DECIDE|MDH_MASK_SCOUT_GATE_CALC_RESP_INTERV,
        
        
        //  END:  Definitions used for gating
        //  --------------------------------------------------
        
        //  --------------------------------------------------
        //  BEGIN:  Definitions used for triggering
        //
        
        //  Diregard all physilogical data stored previously.
        MDH_MASK_SCOUT_TRIGGER_RESET            = 0x1000,
        
        //  Test whether selected trigger conditions are fulfilled.
        MDH_MASK_SCOUT_TRIGGER                  = 0x2000,
        
        //  First scout past a previous trigger event
        MDH_MASK_SCOUT_FIRST_PAST_TRIGGER_EVENT = 0x4000,
        
        //  Return number of detected respiratory intervals.
        MDH_MASK_SCOUT_CALC_RESP_INTERV         = 0x8000,

        //  Return median respiratory interval (breathing cycle).
        MDH_MASK_SCOUT_CALC_MEDIAN_RESP_INTERV  = 0x0020,

        
        //  See MDH_MASK_SCOUT_EARLIEST, ...
        MDH_MASK_SCOUT_TRIGGER_EARLIEST = MDH_MASK_SCOUT_EARLIEST|MDH_MASK_SCOUT_TRIGGER_RESET| MDH_MASK_SCOUT_FIRST_PAST_TRIGGER_EVENT,
        MDH_MASK_SCOUT_TRIGGER_LATEST   = MDH_MASK_SCOUT_LATEST  |MDH_MASK_SCOUT_TRIGGER|MDH_MASK_SCOUT_CALC_RESP_INTERV,
        MDH_MASK_SCOUT_TRIGGER_ALL      = MDH_MASK_SCOUT_ALL,
        
        
        
        //  END:  Definitions used for triggering
        //  --------------------------------------------------
        
        
        //  --------------------------------------------------
        //  BEGIN:  Definitions used for retrospective DVA algorithm
        //
        
        //  Diregard all data stored previously.
        //MDH_MASK_SCOUT_DVA_RESET               = 0x1000,
        
        //  Decide with profile is acquired next
        //MDH_MASK_SCOUT_DVA_DECIDE              = 0x2000,
        
        //  Store measured displacement for current profile.
        //MDH_MASK_SCOUT_DVA_STORE_DISPL         = 0x4000,
        
        //MDH_MASK_SCOUT_DVA_EARLIEST = MDH_MASK_SCOUT_EARLIEST|MDH_MASK_SCOUT_DVA_RESET,
        //MDH_MASK_SCOUT_DVA_LATEST   = MDH_MASK_SCOUT_LATEST  |MDH_MASK_SCOUT_DVA_STORE_DISPL|MDH_MASK_SCOUT_DVA_DECIDE,
        //MDH_MASK_SCOUT_DVA_ALL      = MDH_MASK_SCOUT_ALL,
        
        
        //  END:  Definitions used for retrospective DVA algorithm
        //  --------------------------------------------------
        
        
        //  Number of Online para used. Must be increased
        //  if list of MDH_INDX_ONLINE_... defines is augmented.
        MDH_SIZE_ONLINE_PARA                   = 4,
        
        //  ID
        MDH_INDX_ONLINE_ID                     = 0,
        
        //  Bit flags
        //  Note: The Diffusion SBB uses IceProgramPara 0, 1, 2
        MDH_INDX_ONLINE_FLAGS                  = 3,
        
        //  Insert specifiers
        MDH_INDX_ONLINE_INSERT                 = 2,
        
        //  Designated channel
        MDH_INDX_ONLINE_CHA                    = 1,
        
        //  If the flag is set the online method of the ice
        //  program must send the profile imgage to the host.
        MDH_MASK_ONLINE                          = 0x0001,
        
        //  If the flags is set the online method returns after
        //  the profile image has been send. If the flag is not
        //  set the ice program must also invoke the online method
        //  of it's base class.
        MDH_MASK_ONLINE_BREAK                    = 0x0002,
        
        //  If the flag is set and 'Store profile images' is selected
        //  the profile images are stored in the data base.
        MDH_MASK_ONLINE_LAST_ADC                 = 0x0004,
        
        //  Resets the clock that measures the pause between
        //  the breath-holds
        MDH_MASK_ONLINE_CLK_RESET                = 0x0008,
        
        //  Zoom profile image in
        MDH_MASK_ONLINE_ZOOM_IN                  = 0x0010,
        
        //  The Scout with ScoutID == OnlineID generated a respiratory
        //  trigger event.
        MDH_MASK_ONLINE_TRIGGER_ECG              = 0x0020,
        
        //  Display Actual detected shift
        MDH_MASK_ONLINE_HEADER_L2                = 0x0800,
        MDH_MASK_ONLINE_HEADER_L2_SHIFT          = 0x0800,
        
        MDH_MASK_ONLINE_HEADER_L3                = 0x00c0,
        
        //  Display respiratory interval
        MDH_MASK_ONLINE_HEADER_L3_RESP_INTRV       = 0x0040,
        
        //  Median temporal displacement between two adjacent
        //  respiratory trigger events.
        MDH_MASK_ONLINE_HEADER_L3_TRIGGER_RESP     = 0x0080,
        
        //  Median temporal displacement between two adjacent
        //  respiratory+ECG trigger events.
        MDH_MASK_ONLINE_HEADER_L3_TRIGGER_RESP_ECG = 0x00c0,
        
        MDH_MASK_ONLINE_HEADER_L1                 = 0x0700,
        MDH_MASK_ONLINE_HEADER_L1_SCOUT           = 0x0100,
        MDH_MASK_ONLINE_HEADER_L1_MONITOR         = 0x0200,
        MDH_MASK_ONLINE_HEADER_L1_SCAN            = 0x0300,
        MDH_MASK_ONLINE_HEADER_L1_BH              = 0x0400,
        MDH_MASK_ONLINE_HEADER_L1_STATISTIC       = 0x0500,
        MDH_MASK_ONLINE_HEADER_L1_PAUSE           = 0x0600,
        MDH_MASK_ONLINE_HEADER_L1_RESP_INTRV      = 0x0700,
        
        
        MDH_MASK_ONLINE_FOOTER                   = 0x7000,
        MDH_MASK_ONLINE_FOOTER_CLICK             = 0x1000,
        MDH_MASK_ONLINE_FOOTER_HOLD_BREATH       = 0x2000,
        MDH_MASK_ONLINE_FOOTER_CONT_BREATH       = 0x3000,
        MDH_MASK_ONLINE_FOOTER_FREE_BREATH       = 0x4000,
        MDH_MASK_ONLINE_FOOTER_STOP              = 0x5000,
        
        MDH_MASK_ONLINE_HEADER_L4                = 0x8000,
        MDH_MASK_ONLINE_HEADER_L4_THRESHOLD      = 0x8000,
        
        
        
        //  Definitions for the free header parameters of
        //  the anatomical imaging ADC
        
        //  Cronological ID, delivered with the feedback data
        //  of the forgoing scout
        MDH_ANATOM_INDX_FREE_CHRONID             = 0,
        
        //  The concatenation counter
        //MDH_ANATOM_INDX_FREE_CONC                = 1,
        
        //  Cluster ID, delivered with the feedback data
        //  of the forgoing scout
        //MDH_ANATOM_INDX_FREE_CLUSTERID           = 2
    };

    //  The element MDH_INDX_SCOUT_WND contains the indeces of the scout lNav
    //  and the image windows [lImaWndBegin, lImaWndEnd[ and the search window
    //  [lSearchWndBegin,lSearchWndBegin+lImaWndEnd-lImaWndBegin[ in compressed form.
    //  The static member function can be used to pack and retrieve the indeces.
    bool packWnd(unsigned short& rushDst, uint32_t u32Nav, uint32_t u32ImaWndBegin, uint32_t u32ImaWndEnd, uint32_t u32SearchWndBegin);
    void unpackWnd(uint32_t& ru32Nav, uint32_t& u32ImaWndBegin, uint32_t& u32ImaWndEnd, uint32_t& ru32SearchWndBegin, unsigned short ushSrc);

    //  The first static member function passes the free header parameter to the
    //  MDH object referenced by rMDH.
    //  The second static member function retrieves the free header parameter
    //  from the MDH object referenced by rMDH.
    //  Note: If the static member functions is needed,
    //        the file "Measurement\MdhProxy.h" must precede
    //        IScout.h in the clients include hierachy.
    bool setOnlinePara(MdhProxy& rMDH, const unsigned short aushPara[], size_t uSize);
    bool getOnlinePara(unsigned short aushPara[], const MdhProxy& rMDH, size_t uSize);
    
    bool setFBPara(MdhProxy& rMDH, const unsigned short aushPara[], size_t uSize);
    bool getFBPara(unsigned short aushPara[], const MdhProxy& rMDH, size_t uSize);


    //  -----------------------------------------------------------------
    //  Definition of the objects that are send from the ICE program to
    //  the sequence
    //  -----------------------------------------------------------------
    
    struct FBSObjBase
    {
        //  Common base class of all (F)eed(B)ack (S)end objects.
        
        //  -------------------------------------------------------------
        //  Some local definitions.
        //  -------------------------------------------------------------
        
        typedef FBSObjBase   MYTYPE;
        
        enum
        {
            TYPE_ID             = 0x1,
                MASK_SCOUT_FLAGS    = 0xffff0000,
                SBIT_SCOUT_FLAGS    = 16,
                MASK_FEEDBACK_FLAGS = 0x0000ffff,
                SBIT_FEEDBACK_FLAGS = 0,
                
                //  # elements
                ELEMENT_COUNT       = 4
        };
        
        //  -------------------------------------------------------------
        //  Attributes
        //  -------------------------------------------------------------
        
        //  The Feedback-channel.
        int32_t m_i32FBCha;
        
        //  Send counter. To each feedback request a unique number is assigned
        //  that is passed via the ADC to the ICE world and which is send back
        //  with the feedback. Links the received data to a particular request.
        int32_t m_i32FBID;
        
        //  Type-ID to identify the derived type (must be unique).
        int32_t m_i32TypeID;
        
        //  Begin of actual FB data.
        //  See FEEDBACK_... of the derived types.
        int32_t  m_i32Flags;
        
        //  -------------------------------------------------------------
        //  Static class members
        //  -------------------------------------------------------------
        
        //  Extracts data from an object of type SEQData.
        static bool extract(MYTYPE& rDst, SEQData& rSeqData);
        
        //  Sets Type-ID of object rDst points to.
        static inline int32_t setTypeID(MYTYPE& rDst)
        {
            return rDst.m_i32TypeID = TYPE_ID;
        }
        
        //  This method is used by the ice program. It passes the original
        //  scout flags send with the MDH to the high order flags of the
        //  m_i32Flags variable.
        inline void packScoutFlags(unsigned short ushFlags)
        {
            m_i32Flags &= ~MASK_SCOUT_FLAGS;
            m_i32Flags |= ushFlags<<SBIT_SCOUT_FLAGS;
        }
        //  This method can be used by the sequence to retrieve the original
        //  scout flags when feedback has been received.
        inline unsigned short unpackScoutFlags() const
        {
            return static_cast<unsigned short>((m_i32Flags&MASK_SCOUT_FLAGS)>>SBIT_SCOUT_FLAGS);
        }
        
        inline void packFeedbackFlags(unsigned short ushFlags)
        {
            m_i32Flags &= ~MASK_FEEDBACK_FLAGS;
            m_i32Flags |= ushFlags<<SBIT_FEEDBACK_FLAGS;
        }
        inline unsigned short unpackFeedbackFlags() const
        {
            return static_cast<unsigned short>((m_i32Flags&MASK_FEEDBACK_FLAGS)>>SBIT_FEEDBACK_FLAGS);
        }
        
        //  Restores state after construction
        static void clear(MYTYPE& rDst)
        {
            rDst.m_i32FBCha  = 0;
            rDst.m_i32FBID   = 0;
            rDst.m_i32TypeID = TYPE_ID;
            rDst.m_i32Flags  = 0;
        }
        
        //  -------------------------------------------------------------
        //  Construction, assignment
        //  -------------------------------------------------------------
        
        //  Inline default constructor.
        FBSObjBase()
            : m_i32FBCha(0)
            , m_i32FBID(0)
            , m_i32TypeID(TYPE_ID)
            , m_i32Flags(0)
        {}
        
        //  Copy constructor
        FBSObjBase(const MYTYPE& rRHS)
            : m_i32FBCha (rRHS.m_i32FBCha)
            , m_i32FBID  (TYPE_ID)
            , m_i32TypeID(rRHS.m_i32TypeID)
            , m_i32Flags (rRHS.m_i32Flags)
        {}
        
        //  Assignment operator.
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(this != &rRHS)
            {
                m_i32FBCha  = rRHS.m_i32FBCha;
                m_i32FBID   = rRHS.m_i32FBID;
                m_i32TypeID = rRHS.m_i32TypeID;
                m_i32Flags  = rRHS.m_i32Flags;
            }
            return *this;
        }
    };


    struct FBSFollow : public FBSObjBase
    {
        //  Feedback object that can be used for slice following
        //  and accept/reject gating algorithm.
        
        //  -------------------------------------------------------------
        //  Some local definitions.
        //  -------------------------------------------------------------
        
        typedef FBSFollow   MYTYPE;
        typedef FBSObjBase  BASE_TYPE;
        
        enum
        {
            //  Unique type identification
            //  (Must contain all bit flags of base types)
            TYPE_ID                               = 0x2,
                
            //  The following two flags are mutually exclusive
            FEEDBACK_OUT_OF_GATE                  = 0x01,
            FEEDBACK_WITHIN_GATE                  = 0x02,
            
            //  If the flag is not set the returned shit vector
            //  is undefined and must not be used for slice
            //  following.
            FEEDBACK_SHIFT_VALID                  = 0x04,
            
            //  # elements
            ELEMENT_COUNT                         = FBSObjBase::ELEMENT_COUNT+sizeof(VectPat)/sizeof(double)
                
        };
        
        
        //  -------------------------------------------------------------
        //  Attributes
        //  -------------------------------------------------------------
        
        //  Shift vector defined in a patient orientated coordinate frame.
        VectPat m_sShiftVect;
        
        //  -------------------------------------------------------------
        //  Static class members
        //  -------------------------------------------------------------
        
        //  Extracts data from an object of type SEQData.
        static bool extract(MYTYPE& rDst, SEQData& rSeqData);
        
        //  Sets Type-ID of object rDst points to.
        static inline int32_t setTypeID(MYTYPE& rDst)
        {
            FBSObjBase::setTypeID(rDst);
            return rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  Restores state after construction
        static void clear(MYTYPE& rDst)
        {
            FBSObjBase::clear(rDst);
            rDst.m_sShiftVect = VectPat::null;
            rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  -------------------------------------------------------------
        //  Construction, assignment
        //  -------------------------------------------------------------
        
        //  Default constructor.
        FBSFollow()
            : FBSObjBase()
            , m_sShiftVect(VectPat::null)
        {
            //  Overwrite type ID
            m_i32TypeID |= TYPE_ID;
        }
        
        //  Copy constructor.
        FBSFollow(const MYTYPE& rRHS)
            : FBSObjBase(rRHS)
            , m_sShiftVect(rRHS.m_sShiftVect)
        {
            m_i32TypeID |= (rRHS.m_i32TypeID&TYPE_ID);
        }
        
        //  Assignment operator
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(this != &rRHS)
            {
                FBSObjBase::operator=(rRHS);
                m_sShiftVect  = rRHS.m_sShiftVect;
                m_i32TypeID  |= (rRHS.m_i32TypeID&TYPE_ID);
            }
            return *this;
        }
        
    };

    struct FBSTrigger : public FBSFollow
    {
        //  Feedback send object used for the 'Trigger' channel.
        
        //  -------------------------------------------------------------
        //  Some local definitions.
        //  -------------------------------------------------------------
        
        typedef FBSTrigger  MYTYPE;
        typedef FBSFollow   BASE_TYPE;
        
        enum
        {
            //  Unique type identification
            TYPE_ID                          = 0x20
                
                //  Trigger event
        ,   FEEDBACK_TRIGGER                 = 0x10
                
        ,   FEEDBACK_RESP_INTRV_VALID        = 0x20

        ,   FEEDBACK_MEDIAN_RESP_INTRV_VALID = 0x40
                
            //  # elements
        ,   ELEMENT_COUNT                    = FBSFollow::ELEMENT_COUNT+2
        };
        
        //  -------------------------------------------------------------
        //  Attributes
        //  -------------------------------------------------------------
        
        //  # detected respiratory intervals.
        int32_t m_i32RespIntrv;

        double  m_dMedianRespIntrv_us;
        
        //  -------------------------------------------------------------
        //  Static class members
        //  -------------------------------------------------------------
        
        //  Extracts data from an object of type SEQData.
        static bool extract(MYTYPE& rDst, SEQData& rSeqData);
        
        //  Sets Type-ID of object rDst points to.
        static inline int32_t setTypeID(MYTYPE& rDst)
        {
            FBSFollow::setTypeID(rDst);
            return rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  Restores state after construction
        static inline void clear(MYTYPE& rDst)
        {
            FBSFollow::clear(rDst);
            rDst.m_i32RespIntrv        = 0;
            rDst.m_dMedianRespIntrv_us = 0;
            rDst.m_i32TypeID    |= TYPE_ID;
        }
        
        //  -------------------------------------------------------------
        //  Members
        //  -------------------------------------------------------------
        
        //  Inline default constructor.
        FBSTrigger()
            : FBSFollow()
            , m_i32RespIntrv(0)
            , m_dMedianRespIntrv_us(0)
        {
            //  Complete type ID
            m_i32TypeID |= TYPE_ID;
        }
        
        //  Copy constructor.
        FBSTrigger(const MYTYPE& rRHS)
            : FBSFollow(rRHS)
            , m_i32RespIntrv(rRHS.m_i32RespIntrv)
            , m_dMedianRespIntrv_us(rRHS.m_dMedianRespIntrv_us)
        {
            m_i32TypeID |= (rRHS.m_i32TypeID&TYPE_ID);
        }
        
        //  Assignment operator
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(this != &rRHS)
            {
                FBSFollow::operator=(rRHS);
                m_i32RespIntrv         = rRHS.m_i32RespIntrv;
                m_dMedianRespIntrv_us  = rRHS.m_dMedianRespIntrv_us;
                m_i32TypeID           |= (rRHS.m_i32TypeID&TYPE_ID);
            }
            return *this;
        }
        
        friend std::ostream& operator << (std::ostream& rLHS, const MYTYPE& rRHS);
        
    };

    //  The other direction: Data object send from running sequence to ICE
    struct SYNC_TRIGGER
    {
        //  See SyncRC::TIMING_PARA for explanation of the members
        int32_t  m_i32AcqPeriod_us;      
        int32_t  m_i32AcqPeriodRR;
        int32_t  m_i32RespTriggerDelay_us;
        int32_t  m_i32CardTriggerDelay_us;
        int32_t  m_i32NHalts;
        int32_t  m_i32NConc;
        
        //  Default constructor.
        SYNC_TRIGGER()
            : m_i32AcqPeriod_us(0)
            , m_i32AcqPeriodRR(0)
            , m_i32RespTriggerDelay_us(0)
            , m_i32CardTriggerDelay_us(0)
            , m_i32NHalts(1)
            , m_i32NConc(1)
        {}
        
    };

    struct FBSROPE : public FBSFollow
    {
        //  Feedback send object used for the 'ROPE' channel.
        
        //  -------------------------------------------------------------
        //  Some local definitions.
        //  -------------------------------------------------------------
        
        typedef FBSROPE     MYTYPE;
        typedef FBSFollow   BASE_TYPE;
        
        enum
        {
            //  Unique type identification
            TYPE_ID                    = 0x40
                
            //  Disable ADC
        ,   FEEDBACK_OUT_OF_GATE       = 0x0200
        ,   FEEDBACK_GAT_TRIGGER       = 0x0400
        ,   FEEDBACK_PHS_CORR          = 0x0800
                
            //  Valid flags
        ,   FEEDBACK_CHRON_ID_VALID    = 0x0010
        ,   FEEDBACK_PROFILE_ID_VALID  = 0x0020
        ,   FEEDBACK_RESP_INTRV_VALID  = 0x0040
        ,   FEEDBACK_SCAN_CNTR_VALID   = 0x0080
        ,   FEEDBACK_CLUSTER_ID_VALID  = 0x0100
                
                //  Phase correction scan desired
                //FEEDBACK_PHS_CORR         = 0x0800,
                
            //  End of concatenation.
            //  All profiles acquired at least once and all profiles
            //  within gate.
        ,   FEEDBACK_PSET_WITHIN_GATE = 0x1000
            
            //  Each profile acquired at least once but not necessary
            //  all profiles within gate.
        ,   FEEDBACK_PSET_COMPLETE    = 0x2000
            
            //  Last scout since timeout reached
            //  Not all profiles necessary within gateor acquired
        ,   FEEDBACK_TIMEOUT          = 0x4000
            
            //  First feedback that doesn't carry the OUT_OF_GATE
            //  flag since previous call to reset
        ,   FEEDBACK_FIRST_IN_GATE    = 0x8000           
            
            //  # elements
        ,   ELEMENT_COUNT             = FBSFollow::ELEMENT_COUNT+5
        };
        
        //  -------------------------------------------------------------
        //  Attributes
        //  -------------------------------------------------------------
        
        //  Chronological ID of the scan to be acquired next.
        //  Must be passed to the MDH of all profiles acquired
        //  as reaction on the feedback result
        uint32_t  m_u32ChronID;
        
        //  ProfileID of the profiles to be acquired next.
        uint32_t  m_u32ProfileID;
        
        //  Number of scans already within the (current) gate.
        uint32_t m_u32ScanCntr;
        
        //  # detected respiratory intervals.
        uint32_t m_u32RespIntrv;
        
        //  Cluster ID of the scan to be acquired next.
        //  Must be passed to the MDH of all profiles acquired
        //  as reaction on the feedback result.
        //  A particular cluster ID serves as a common attribute of
        //  profiles that belong to the same motion state.
        //  Thereby 'same' is in general coarser than the
        //  resolution of the motion detection algorithm.
        uint32_t m_u32ClusterID;
        
        //  -------------------------------------------------------------
        //  Static class members
        //  -------------------------------------------------------------
        
        //  Extracts data from an object of type SEQData.
        static bool extract(MYTYPE& rDst, SEQData& rSeqData);
        
        //  Sets Type-ID of object rDst points to.
        static inline int32_t setTypeID(MYTYPE& rDst)
        {
            FBSFollow::setTypeID(rDst);
            return rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  Restores state after construction
        static inline void clear(MYTYPE& rDst)
        {
            FBSFollow::clear(rDst);
            rDst.m_u32ChronID    = std::numeric_limits<uint32_t>::max();
            rDst.m_u32ProfileID  = 0;
            rDst.m_u32ScanCntr   = 0;
            rDst.m_u32RespIntrv  = 0;
            rDst.m_u32ClusterID  = std::numeric_limits<uint32_t>::max();
            rDst.m_i32TypeID    |= TYPE_ID;
        }
        
        //  -------------------------------------------------------------
        //  Members
        //  -------------------------------------------------------------
        
        //  Inline default constructor.
        FBSROPE()
            : FBSFollow()
            , m_u32ChronID(std::numeric_limits<uint32_t>::max())
            , m_u32ProfileID(0)
            , m_u32ScanCntr(0)
            , m_u32RespIntrv(0)
            , m_u32ClusterID(std::numeric_limits<uint32_t>::max())
        {
            //  Complete type ID
            m_i32TypeID |= TYPE_ID;
        }
        
        //  Copy constructor.
        FBSROPE(const MYTYPE& rRHS)
            : FBSFollow(rRHS)
            , m_u32ChronID(rRHS.m_u32ChronID)
            , m_u32ProfileID(rRHS.m_u32ProfileID)
            , m_u32ScanCntr(rRHS.m_u32ScanCntr)
            , m_u32RespIntrv(rRHS.m_u32RespIntrv)
            , m_u32ClusterID(rRHS.m_u32ClusterID)
        {
            m_i32TypeID |= (rRHS.m_i32TypeID&TYPE_ID);
        }
        
        //  Assignment operator
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(this != &rRHS)
            {
                FBSFollow::operator=(rRHS);
                m_u32ChronID   = rRHS.m_u32ChronID;
                m_u32ProfileID = rRHS.m_u32ProfileID;
                m_u32ScanCntr  = rRHS.m_u32ScanCntr;
                m_u32RespIntrv = rRHS.m_u32RespIntrv;
                m_u32ClusterID = rRHS.m_u32ClusterID;
                m_i32TypeID   |= (rRHS.m_i32TypeID&TYPE_ID);
            }
            return *this;
        }        
    };

    struct FBSGate : public FBSObjBase
    {
        //  Feedback object that can be used for prospective gating algorithm
        
        //  -------------------------------------------------------------
        //  Some local definitions.
        //  -------------------------------------------------------------
        
        typedef FBSGate     MYTYPE;
        typedef FBSObjBase  BASE_TYPE;
        
        enum
        {
            //  Unique type identification
            TYPE_ID                               = 0x80
                
            //  If the flag is not set the returned 
            //  profile id is undefined.
        ,   FEEDBACK_PROFILE_ID_VALID             = 0x10
                
            //  All profiles acquired at least once and all profiles
            //  within gate.
        ,   FEEDBACK_PSET_WITHIN_GATE             = 0x100

        //  Indicates that member m_u32ScanCntr is set 
        ,   FEEDBACK_SCAN_CNTR_VALID               = 0x0080
                
                
            //  # elements
        ,   ELEMENT_COUNT                         = FBSObjBase::ELEMENT_COUNT+2
        };
        
        
        //  -------------------------------------------------------------
        //  Attributes
        //  -------------------------------------------------------------
        
        //  ID of the profile to be measured next, if FEEDBACK_PROFILE_ID_VALID
        uint32_t  m_u32ProfileID;

        //  Number of scans already within the (current) gate.
        uint32_t  m_u32ScanCntr;
        
        //  -------------------------------------------------------------
        //  Static class members
        //  -------------------------------------------------------------
        
        //  Extracts data from an object of type SEQData.
        static bool extract(MYTYPE& rDst, SEQData& rSeqData);
        
        //  Sets Type-ID of object rDst points to.
        static inline int32_t setTypeID(MYTYPE& rDst)
        {
            FBSObjBase::setTypeID(rDst);
            return rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  Restores state after construction
        static void clear(MYTYPE& rDst)
        {
            FBSObjBase::clear(rDst);
            rDst.m_u32ProfileID = std::numeric_limits<uint32_t>::max();
            rDst.m_u32ScanCntr  = 0;
            rDst.m_i32TypeID |= TYPE_ID;
        }
        
        //  -------------------------------------------------------------
        //  Construction, assignment
        //  -------------------------------------------------------------
        
        //  Default constructor.
        FBSGate()
            : FBSObjBase()
            , m_u32ProfileID(std::numeric_limits<uint32_t>::max())
            , m_u32ScanCntr(0)
        {
            //  Overwrite type ID
            m_i32TypeID |= TYPE_ID;
        }
        
        //  Copy constructor.
        FBSGate(const MYTYPE& rRHS)
            : FBSObjBase(rRHS)
            , m_u32ProfileID(rRHS.m_u32ProfileID)
            , m_u32ScanCntr(rRHS.m_u32ScanCntr)

        {
            m_i32TypeID |= (rRHS.m_i32TypeID&TYPE_ID);
        }
        
        //  Assignment operator
        MYTYPE& operator=(const MYTYPE& rRHS)
        {
            if(this != &rRHS)
            {
                FBSObjBase::operator=(rRHS);
                m_u32ProfileID  = rRHS.m_u32ProfileID;
                m_u32ScanCntr   = rRHS.m_u32ScanCntr;
                m_i32TypeID    |= (rRHS.m_i32TypeID&TYPE_ID);
            }
            return *this;
        }
        
    };
    
    inline std::ostream& operator << (std::ostream& rLHS, const FBSObjBase& rRHS)
    {
        rLHS << "CHA: "  << rRHS.m_i32FBCha;
        rLHS << "; ID: " << rRHS.m_i32FBID;
        return rLHS;
    }

    inline std::ostream& operator << (std::ostream& rLHS, const FBSFollow& rRHS)
    {
        rLHS << static_cast<const FBSObjBase&>(rRHS);
        bool bOr = false;
        rLHS << "; FLAGS: ";
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_OUT_OF_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "OGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_WITHIN_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "IGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_SHIFT_VALID)
        {
            if(bOr) rLHS << '|';
            rLHS << "SHIFT_VAL";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_SHIFT_VALID)
        {
            rLHS << "; SHIFT: " << rRHS.m_sShiftVect;
        }
        return rLHS;
    }
    
    inline std::ostream& operator << (std::ostream& rLHS, const FBSTrigger& rRHS)
    {
        rLHS << static_cast<const FBSObjBase&>(rRHS);
        bool bOr = false;
        rLHS << "; FLAGS: ";
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_OUT_OF_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "OGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_WITHIN_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "IGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_SHIFT_VALID)
        {
            if(bOr) rLHS << '|';
            rLHS << "SHIFT_VAL";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSTrigger::FEEDBACK_TRIGGER )
        {
            if(bOr) rLHS << '|';
            rLHS << "TRIGGER";
            bOr = true;
        }
        if( bOr == false )
        {
            rLHS << '-';
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_SHIFT_VALID)
        {
            rLHS << "; SHIFT: " << rRHS.m_sShiftVect;
        }
        if(rRHS.m_i32Flags & FBSTrigger::FEEDBACK_RESP_INTRV_VALID)
        {
            rLHS << "; RSP-INTRV: " << rRHS.m_i32RespIntrv;
        }
        return rLHS;
    }
    inline std::ostream& operator << (std::ostream& rLHS, const FBSROPE& rRHS)
    {
        rLHS << static_cast<const FBSObjBase&>(rRHS);
        bool bOr = false;
        rLHS << "; FLAGS: ";
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_OUT_OF_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "OGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_WITHIN_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "IGATE";
            bOr = true;
        }
        if(rRHS.m_i32Flags & FBSROPE::FEEDBACK_PSET_WITHIN_GATE)
        {
            if(bOr) rLHS << '|';
            rLHS << "PSET_C";
            bOr = true;
        }
        if( bOr == false )
        {
            rLHS << '-';
        }
        if(rRHS.m_i32Flags & FBSROPE::FEEDBACK_PROFILE_ID_VALID )
        {
            if(bOr) rLHS << '|';
            rLHS << "; PID: " << rRHS.m_u32ProfileID;
        }
        if(rRHS.m_i32Flags & FBSROPE::FEEDBACK_CHRON_ID_VALID )
        {
            if(bOr) rLHS << '|';
            rLHS << "; CID: " << rRHS.m_u32ChronID;
        }
        if(rRHS.m_i32Flags & FBSFollow::FEEDBACK_SHIFT_VALID)
        {
            rLHS << "; SHIFT: " << rRHS.m_sShiftVect;
        }
        if(rRHS.m_i32Flags & FBSROPE::FEEDBACK_RESP_INTRV_VALID)
        {
            rLHS << "; RINTRV: " << rRHS.m_u32RespIntrv;
        }
        if(rRHS.m_i32Flags & FBSROPE::FEEDBACK_SCAN_CNTR_VALID)
        {
            rLHS << "; SCNTR: " << rRHS.m_u32ScanCntr;
        }
        return rLHS;
    }
    
    inline std::ostream& TRACE_SCOUT_FLAGS(std::ostream& rOut, unsigned short ushFlags)
    {
        bool bOr = false;
        if(ushFlags & MDH_MASK_SCOUT_PHASE_CORR)
        {
            if(bOr) rOut << '|';
            rOut << "PHCORR";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_REF_SCAN)
        {
            if(bOr) rOut << '|';
            rOut << "REF";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_USE_AS_SEED)
        {
            if(bOr) rOut << '|';
            rOut << "SEED";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_STATISTIC_SCAN)
        {
            if(bOr) rOut << '|';
            rOut << "STAT";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_STRESS)
        {
            if(bOr) rOut << '|';
            rOut << "STRESS";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_REF_LINE)
        {
            if(bOr) rOut << '|';
            rOut << "RLIN";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_SENS_WINDOW)
        {
            if(bOr) rOut << '|';
            rOut << "SWND";
            bOr = true;
        }
#if 0
        if(ushFlags & MDH_MASK_SCOUT_ACCEPT_WINDOW)
        {
        if(bOr) rOut << '|';
        rOut << "AWND";
        bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_GATE_DECIDE)
        {
        if(bOr) rOut << '|';
        rOut << "DEC";
        bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_GATE_STORE_DISPL)
        {
        if(bOr) rOut << '|';
        rOut << "DISPL";
        bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_CALC_SHIFT)
        {
        if(bOr) rOut << '|';
        rOut << "CSHIFT";
        bOr = true;
        }
        if(ushFlags & MDH_MASK_SCOUT_CALC_NORM)
        {
        if(bOr) rOut << '|';
        rOut << "CNORM";
        bOr = true;
        }
#endif
        return rOut;
    }
    
    inline std::ostream& TRACE_ONLINE_FLAGS(std::ostream& rOut, unsigned short ushFlags)
    {
        bool bOr = false;
        
        if(ushFlags & MDH_MASK_ONLINE)
        {
            if(bOr) rOut << '|';
            rOut << "ONL";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_ONLINE_BREAK)
        {
            if(bOr) rOut << '|';
            rOut << "BREAK";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_ONLINE_LAST_ADC)
        {
            if(bOr) rOut << '|';
            rOut << "LAST";
            bOr = true;
        }
        if(ushFlags & MDH_MASK_ONLINE_CLK_RESET)
        {
            if(bOr) rOut << '|';
            rOut << "CLKRESET";
            bOr = true;
        }
        return rOut;
    }

} // namespace PACE

#endif // __FB_H
