//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\a_se_b130_15000.h
//	 Version:
//	  Author: Michael Zwanger, SIEMENS AG, Medical Engineering, MRIA, +9131/84-2672
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Deklarations for a_se_b130_15000.cpp
//
//
/// \brief  File containing declarations for the sequences 
///         - se_b130_15000
///         
/// This file contains the declaration of the class Se15b130.
/// The sequence Se15b130 use it to generate very nice images.
/// 
//  ***************************************************************************/
/// 



#ifndef a_se_b130_15000_h
#define a_se_b130_15000_h 1


//  -------------------------------------------------------------------------- 
//  General Includes                                                           
//  -------------------------------------------------------------------------- 
#include "MrServers/MrImaging/libSBB/StdSeqIF.h"
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"       // * definition of type NLS_STATUS  *

//  -------------------------------------------------------------------------- 
//  Application includes                                                       
//  -------------------------------------------------------------------------- 
#include  "MrServers/MrImaging/libSBB/SEQLoop.h"        // class SeqLoop
#include  "MrServers/MrImaging/libSBB/SBBMSat.h"        // class SeqBuildBlockMSat
#include  "MrServers/MrImaging/libSBB/SBBCSat.h"        // class SeqBuildBlockCSat
#include  "MrServers/MrImaging/libSBB/SBBRSat.h"        // class SeqBuildBlockRSat


#define OSCBIT_IN_KERNEL    (     1)  // 1 = send osc bit at the beginning of the kernel
// 0 = send osc bit from SeqLoop
#define BASIC_SCANTIMEus    ( 27600)  /* Basic scan time of kernel       */
//#define READOUTTIMEus       (  7680)  /* Readout time in this sequence   */
#define MAXRSATS 6            /* Maximum number of allowed RSats */


#ifdef BUILD_SEQU
#define __OWNER
#endif
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"


//  -------------------------------------------------------------------------- 
//  Forward declarations                                                       
//  -------------------------------------------------------------------------- 
//namespace MrProtocolData
//{
//    class SeqExpo;
//}
//class MrProt;
//class SeqLim;
//class Sequence;


namespace SEQ_NAMESPACE
{
    class Se15b130UI;
    
    //  -------------------------------------------------------------------------- 
    //
    /// \brief <b> Class definition of Se15b130. This class is used by the sequences
    ///         - Se15b130
    ///         
    /// This file contains the declaration of the class Se15b130.
    /// The sequence Se15b130 use it to generate very nice images.
    /// 
    //  -------------------------------------------------------------------------- 
    class __IMP_EXP Se15b130 : public StdSeqIF
    {
        
    public:
        
        
        //  ------------------------------------------------------------------ 
        //                                                                     
        //  Name        :  Se15b130::Se15b130                                        
        //                                                                     
        //  Description :  
        /// \brief         Initialization of class members                     
        //                                                                     
        //  Return      :  void                                                
        //                                                                     
        //  ------------------------------------------------------------------ 
        Se15b130();
        
        
        
        //  ------------------------------------------------------------------ 
        //                                                                     
        //  Name        :  Se15b130::~Se15b130                                       
        //                                                                     
        //  Description :  
        /// \brief         Destructor. Deletes existing Se15b130UI instances.                                          
        //                                                                     
        //  Return      :  void                                                
        //                                                                     
        //  ------------------------------------------------------------------ 
        virtual ~Se15b130();
        
        
        
        //   --------------------------------------------------------------------------  
        //                                                                               
        //   Name        :  Se15b130::initialize                                            
        //                                                                               
        //   Description :  
        ///  \brief        Initialization of the sequence                               
        ///                                                                               
        ///                On the host, the object m_pUI will actually contain sensible 
        ///                  data after Se15b130::initialize. On the measurement system, it
        ///                  is basically an empty object behind it. 
        ///                                                                               
        //   Return      :  NLS status                                                   
        //                                                                               
        //   --------------------------------------------------------------------------  
        virtual NLSStatus initialize (SeqLim &rSeqLim);
        
        
        
        //  -------------------------------------------------------------------------- 
        //                                                                             
        //  Name        :  Se15b130::prepare                                              
        //                                                                             
        //  Description :  
        /// \brief <b>     Preparation of the sequence during binary search and prior  
        ///                 to sequence execution  </b>                                    
        //                                                                             
        //  Return      :  NLS status                                                  
        //                                                                             
        //  -------------------------------------------------------------------------- 
        virtual NLSStatus prepare (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo);
        
        
        
        //  -------------------------------------------------------------------------- 
        //                                                                             
        //  Name        :  Se15b130::check                                                
        //                                                                             
        //  Description :  
        /// \brief  <b>    Check of the sequence for gradient stimulation </b>     
        ///                                                                             
        ///                This method is called by the framework prior to a 
        ///                 measurement on the host to ensure, that 
        ///                 - no gradient overflow occurs                                                                             
        ///                 - the stimulation will not exceed the threshold  
        ///
        //  Return      :  NLS status                                                  
        //                                                                             
        //  -------------------------------------------------------------------------- 
        virtual NLSStatus check (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, SEQCheckMode *  pSEQCheckMode);
        
        
        //  -------------------------------------------------------------------------- 
        //                                                                             
        //  Name        :  Se15b130::run                                                  
        //                                                                             
        //  Description :                                                              
        ///     \brief     Execution of the sequence                                   
        //                                                                             
        //  Return      :  NLS status                                                  
        //                                                                             
        //  -------------------------------------------------------------------------- 
        virtual NLSStatus run (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo);
        
        
        
        //   -------------------------------------------------------------------------- 
        //                                                                              
        //   Name        :  Se15b130::runKernel                                            
        //                                                                              
        //   Description :  
        ///  \brief <b>     Executes the basic timing of the real-time sequence.   </b>     
        ///                                                                              
        ///                 The method runKernel plays out a sequence "Kernel",
        ///                  consisting of one or more lines in k-Space.
        ///                 It is called by SeqLoop.
        ///                                                                              
        //   Return      :  NLS status                                                  
        //                                                                              
        //   -------------------------------------------------------------------------- 
        virtual NLS_STATUS runKernel(MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, long lKernelMode, long lSlice, long lPartition, long lLine);
        
        
        
        
        
        //  -------------------------------------------------------------- 
        //                                                                 
        //  Name        :  getUI                                           
        //                                                                 
        //  Description :  
        /// \brief <b>     Returns the pointer to the Se15b130 UI class  </b>     
        ///                                                                 
        ///                This method is only sensible on the host.
        ///                On the measurement system, it will return an nearly empty object.                                                 
        ///                                                                 
        //  Return      :  Se15b130UI*                                        
        //                                                                 
        //  -------------------------------------------------------------- 
        const Se15b130UI* getUI (void) const;
        
        
        
        
        //  ------------------------------------------------------------------ 
        //  Declare additional P U B L I C member functions, e.g. functions    
        //  that will be accessed in UI handlers (outside the class).          
        //  ------------------------------------------------------------------ 
        
        bool calculateTRTIFillTimes( MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, long* plNeededTI, long* plNeededTR);
        
        
        
        
        
        
    protected: 
        
        //  ------------------------------------------------------------------ 
        ///  No of phase encoding lines with iPAT                             
        //  ------------------------------------------------------------------ 
        int32_t m_lLinesToMeasure;
        
        /* -------------------------------------------------------- */
        /* Slice position information (rotation matrices and shifts)*/
        /* -------------------------------------------------------- */
        sSLICE_POS             m_asSLC[K_NO_SLI_MAX];
        
        /* --------- */
        /* RF Pulses */
        /* --------- */
        sRF_PULSE_EXT          m_sSRF01     ;
        sFREQ_PHASE            m_sSRF01zSet ;
        sFREQ_PHASE            m_sSRF01zNeg ;
        
        sRF_PULSE_EXT          m_sSRF02     ;
        sFREQ_PHASE            m_sSRF02zSet ;
        sFREQ_PHASE            m_sSRF02zNeg ;
        
        /* --------------- */
        /* Gradient Pulses */
        /* --------------- */
        sGRAD_PULSE            m_sGP_sp;
        sGRAD_PULSE            m_sGR_sp;
        sGRAD_PULSE            m_sGS_EX;    /* Slice-select pulse */
        sGRAD_PULSE            m_sGR_de;
        sGRAD_PULSE            m_sGS_re;
        sGRAD_PULSE            m_sGS_RE;    /* Slice-select pulse */
        sGRAD_PULSE_RO         m_sGR_RO;            /* Readout pulse      */
        
        /* --------------- */
        /* Gradient Tables */
        /* --------------- */
        sGRAD_PULSE_PE         m_sTAB01;
        
        /* -------------- */
        /* Readout Events */
        /* -------------- */
        sREADOUT               m_sADC01     ;
        sFREQ_PHASE            m_sADC01zSet ;
        sFREQ_PHASE            m_sADC01zNeg ;
        
        //-------------------------
        // Sequence Building Blocks
        //-------------------------
        SeqLoop        m_mySeqLoop;
        
        // -----------
        // Sync Events
        // -----------
        sSYNC                  m_OscBit;
        
        //-------------------------
        // Sequence Building Blocks
        //-------------------------
        SBBList                m_mySBBList;
        
        SeqBuildBlockRSat      m_RSat[MAXRSATS];
        SeqBuildBlockCSat      m_CSatFat;
        SeqBuildBlockCSat      m_CSatWat;
        SeqBuildBlockMSat      m_MSat;
        
        //  ------------------------------------------------------------------ 
        //  Declare additionl member variables of the sequence, here.          
        //                                                                     
        //  Initialization should be done in the constructor of the class.     
        //  Maybe, that these variables have to be deleted in the destructor.  
        //  ------------------------------------------------------------------ 
        
        long         m_alTEFil[K_NO_TIME_ELEMENTS]; // TE fill times
        long         m_lTRMin;                      /* Minimum TI                */
        long         m_lTIMinAdd1;                      /* Minimum TI additional     */
        long         m_lTIMinAdd2;                      /* Minimum TI additional     */
        const long   m_lScanTimeBasic;
        long         m_lScanTimeAllSats;
        long         m_lNegativeFillTime;                      /* Binary search flag        */
        long         m_lEchoLine;
        
        
        //  ------------------------------------------------------------------ 
        //  Declare additional member functions of the sequence                
        //  ------------------------------------------------------------------ 
        
        
        //  -------------------------------------------------------------- 
        /// \brief <b> UI class for Se15b130                                             
        ///
        ///         This class is basically empty on the measurement system
        //  -------------------------------------------------------------- 
        Se15b130UI* m_pUI;
        
        
        
        
        //  ------------------------------------------------------------------ 
        //                                                                     
        //  Name        :  Se15b130::createUI                                     
        //                                                                     
        //  Description :  
        /// \brief <b>     Instantiation of UI classes   </b>                      
        //                                                                     
        //  Return      :  NLS status                                          
        //                                                                     
        //  ------------------------------------------------------------------ 
        virtual NLS_STATUS createUI (SeqLim &rSeqLim);
        
        
        
        //  ------------------------------------------------------------------ 
        //                                                                     
        //  Name        :  Se15b130::UnusedArg                                    
        //                                                                     
        //  Description :  
        /// \brief         Used to avoid compiler warnings                     
        //                                                                     
        //  Return      :  void                                                
        //                                                                     
        //  ------------------------------------------------------------------ 
        template< class TYPE > void UnusedArg (TYPE Argument) const { if( false ) { TYPE Dummy; Dummy = Argument; } };
        
        
    private:
        
        //  ------------------------------------------------------------------ 
        ///  Copy constructor not implemented                                   
        //  ------------------------------------------------------------------ 
        Se15b130 (const Se15b130 &right);
        
        
        
        //  ------------------------------------------------------------------ 
        ///  Assignment operator not implemented                                
        //  ------------------------------------------------------------------ 
        Se15b130 & operator=(const Se15b130 &right);
        
        
        
        
    };
    
}

#endif
