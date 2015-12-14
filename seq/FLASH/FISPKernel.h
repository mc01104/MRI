//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FISPKernel.h
//	 Version: \main\9
//	  Author: KUEHBESZ
//	    Date: 2011-07-25 15:03:38 +02:00
//	    Date: 2011-07-25 15:03:38 +02:00
//	    Date: 2011-07-25 15:03:38 +02:00
//
//	    Lang: C++
//
//	 Descrip: MR::Measurement::Sequence::SeqLoop
//
//	 Classes:
//
//	-----------------------------------------------------------------------------
#ifndef __FISPKernel_h
#define __FISPKernel_h 1



// * -------------------------------------------------------------------------- *
// * Includes                                                                   *
// * -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/libCSL/CompositeKernel.h"
#include "MrServers/MrImaging/libCSL/Key.h"

#include "MrServers/MrImaging/ut/libsequt.h"

#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"

#include "MrServers/MrImaging/libCSL/Counter.h"

#include "MrServers/MrImaging/seq/FLASH/FLASHSampling.h"




// * -------------------------------------------------------------------------- *
// * Import/export control                                                      *
// * -------------------------------------------------------------------------- *
#ifdef BUILD_SEQU
  #define __OWNER
#endif

#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h" // import/export control


namespace SL  {

// This is the UTrace marker used for traces from the FISPKernel
static int iKernelUTraceMarker = 1;


// ----------------------------------------------------------------------------
/// \brief Simple FISP kernel
///
/// This class defines a simple FISP kernel. The kernel supports
/// - 2D and 3D imaging
/// - rewinder gradients
/// - readout spoiler gradients
/// - rf spoiling can be added using the \c RFSpoiledLeaf decorator
///
/// \par Default Values
/// - none
///
/// \par Exported Values
/// - \c SLK::l_ADC_COUNTER                 Counts the ADC; starts from zero
///                                         for each measurement
///
/// \par Imported Values
/// - \c SLK::l_LINE_LOOP_LENGTH            Number of lines to measure 
/// - \c SLK::l_PARTITION_LOOP_LENGTH       Number of partitions to measure
/// - \c SLK::l_LINES_TO_CHECK
/// - \c SLK::l_PARTITIONS_TO_CHECK
/// - \c FISPKernel::d_MIN_RISE_TIME        Minimum rise time in us/mT/m that 
///                                         will be used by the gradient pulses
/// - \c FISPKernel::d_MAX_GRAD_AMPLITUDE   Maximum gradient amplitude in mT/m
///                                         that will be used by the gradient 
///                                         pulses
// ----------------------------------------------------------------------------
class __IMP_EXP FISPKernel : public CompositeKernel  {

    public:

        // ------------------------------------------------------------------------------
        // Keys used in the FISP kernel
        // ------------------------------------------------------------------------------
        static const Key<double> d_MIN_RISE_TIME;
        static const Key<double> d_MAX_GRAD_AMPLITUDE;


        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::FISPKernel
        //
        // Description :
        ///              \brief     Constructor
        //
        // Parameters  :
        ///              \param     sIdent      String identifier of the leaf
        ///              \param     pMediator   Pointer to Mediator class 
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        FISPKernel (const std::string& sIdent, Mediator* pMediator);



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::FISPKernel
        //
        // Description :
        ///              \brief     Constructor
        //
        // Parameters  :
        ///              \param     sScope      String identifier of the scope of this element
        ///              \param     sIdent      String identifier of the leaf
        ///              \param     pMediator   Pointer to Mediator class 
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        FISPKernel (const std::string& sScope, const std::string& sIdent, Mediator* pMediator);



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::~FISPKernel
        //
        // Description :
        ///              \brief     Destructor
        //
        // Parameters  :
        ///              \param     -
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        virtual ~FISPKernel ();



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::prep
        //
        // Description :
        ///              \brief     Prepares the FISP kernel
        ///
        ///                         Preparation of the FISP kernel consists of the following
        ///                         steps:
        ///                         - preparation all real time event used to build the 
        ///                           kernel such as rf pulse, ADC and gradient pulses
        ///                         - calculation of the start times of the real time events
        //
        // Parameters  :
        ///              \param     rMrProt     Reference to the protocol class
        ///              \param     rSeqLim     Reference to the sequnce limits class
        ///              \param     rSeqExpo    Reference to the sequence exports class
        //
        // Return      :
        ///              \return    \arg \c     true  : successful execution \n
        ///                         \arg \c     false : error occured
        // 
        // ------------------------------------------------------------------------------
        virtual bool prep (MrProt& rMrProt, SeqLim& rSeqLim, MrProtocolData::SeqExpo& rSeqExpo);



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::run
        //
        // Description :
        ///              \brief     Executes the FISP kernel
        //
        // Parameters  :
        ///              \param     rMrProt     Reference to the protocol class
        ///              \param     rSeqLim     Reference to the sequnce limits class
        ///              \param     rSeqExpo    Reference to the sequence exports class
        ///              \param     pSLC        Pointer to the slice position object
        //
        // Return      :
        ///              \return    \arg \c     true  : successful execution \n
        ///                         \arg \c     false : error occured
        // 
        // ------------------------------------------------------------------------------
        virtual bool run (MrProt& rMrProt, SeqLim& rSeqLim, MrProtocolData::SeqExpo& rSeqExpo, sSLICE_POS* pSLC);



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::getADCCounter
        //
        // Description :
        ///              \brief     Returns the ADC counter.
        ///
        ///                         Counts each kernel call within the current measurement, 
        ///                         will be set to zero at the beginning of each 
        ///                         measurement. This counter is used for physiologically 
        ///                         triggered measurements to mark ADCs as relevant for 
        ///                         scan time calculation.
        //
        // Parameters  :
        ///              \param     -
        //
        // Return      :
        ///              \return    Current ADC counter
        // 
        // ------------------------------------------------------------------------------
        int32_t getADCCounter (void) const;



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::setSamplingInstance
        //
        // Description :
        ///              \brief     Sets the used KSpaceSampling instance to access the
        ///                         calculated k-space trajectory
        //
        // Parameters  :
        ///              \param     pSampling  New instance
        // 
        // ------------------------------------------------------------------------------
        void setSamplingInstance (KSpace::KSpaceSampling* pSampling);


    protected:


        // ------------------------------------------------------------------------------
        // Gradient raster time: 10us
        // ------------------------------------------------------------------------------
        enum { eGradRasterTime = 10 };


        // ------------------------------------------------------------------------------
        /// Osc bit
        // ------------------------------------------------------------------------------
        sSYNC_OSC m_OscBit;


        // ------------------------------------------------------------------------------
        /// Rf pulse object used for excitation, sinc type
        // ------------------------------------------------------------------------------
        sRF_PULSE_SINC m_SRF;


        // ------------------------------------------------------------------------------
        /// Frequency phase object used to apply frequency and phase offsets of the 
        /// rf pulse
        // ------------------------------------------------------------------------------
        sFREQ_PHASE m_SRFzSet;


        // ------------------------------------------------------------------------------
        /// Frequency phase event used to undo frequency and phase offsets that have 
        /// accumulated during excitation
        // ------------------------------------------------------------------------------
        sFREQ_PHASE m_SRFzNeg;


        // ------------------------------------------------------------------------------
        /// ADC real time event
        // ------------------------------------------------------------------------------
        sREADOUT m_ADC;


        // ------------------------------------------------------------------------------
        /// Frequency phase event used to apply frquency and phase offsets at the 
        /// beginning of the ADC
        // ------------------------------------------------------------------------------
        sFREQ_PHASE m_ADCzSet;


        // ------------------------------------------------------------------------------
        /// Frequency phase event used to undo frequency and phase offsets that have 
        /// accumulated during ADC duration
        // ------------------------------------------------------------------------------
        sFREQ_PHASE m_ADCzNeg;


        // ------------------------------------------------------------------------------
        /// Readout dephasing gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE m_RODeph;


        // ------------------------------------------------------------------------------
        /// Readout gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE_RO m_RO;


        // ------------------------------------------------------------------------------
        /// Readout spoiler gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE m_ROSpoil;


        // ------------------------------------------------------------------------------
        /// Phase encoding gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE_PE m_PE;


        // ------------------------------------------------------------------------------
        /// Phase encoding rewinder gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE_PE m_PERew;


        // ------------------------------------------------------------------------------
        /// Partition encoding gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE_3D m_3D;


        // ------------------------------------------------------------------------------
        /// Partition encoding gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE_3D m_3DRew;


        // ------------------------------------------------------------------------------
        /// Slice selection
        // ------------------------------------------------------------------------------
        sGRAD_PULSE m_GSlcSel;


        // ------------------------------------------------------------------------------
        /// Slice rephasing gradient
        // ------------------------------------------------------------------------------
        sGRAD_PULSE m_GSlcSelReph;


        // ------------------------------------------------------------------------------
        /// Duration of the excitation block in us. This block includes:
        /// - ramp up and flat top of the slice selection gradient
        /// - rf pulse
        // ------------------------------------------------------------------------------
        long m_lExcitationTime_us;


        // ------------------------------------------------------------------------------
        /// Duration of the prephasing block in us. This block includes:
        /// - ramp down of the slice selection gradient
        /// - phase encoding gradient
        /// - partition encoding gradient
        /// - readout dephasing gradient
        /// - ramp up of the readout gradient
        // ------------------------------------------------------------------------------
        long m_lPrephaseTime_us;

        
        // ------------------------------------------------------------------------------
        /// Duration of the readout block in us. This block includes:
        /// - flat top of the readout gradient
        /// - ADC
        // ------------------------------------------------------------------------------
        long m_lReadOutTime_us;


        // ------------------------------------------------------------------------------
        /// Duration of the rephasing block in us. This block includes:
        /// - ramp down of the readout gradient
        /// - readout spoiler
        /// - phase encoding rewinder
        /// - partition encoding rewinder
        // ------------------------------------------------------------------------------
        long m_lRephaseTime_us;


        // ------------------------------------------------------------------------------
        /// TE fill time in us
        // ------------------------------------------------------------------------------
        long m_lTEFill_us;


        // ------------------------------------------------------------------------------
        /// ADC counter
        /// Counts each kernel call within the current measurement, will be set to zero 
        /// at the beginning of each measurement. This counter is used for physiologically 
        /// triggered measurements to mark ADCs as relevant for scan time calculation.
        // ------------------------------------------------------------------------------
        Counter m_ADCCounter;


        // ------------------------------------------------------------------------------
        /// The Sampling instance is required to measure the correct k-space
        /// trajectory set some of the MDH flags.
        // ------------------------------------------------------------------------------
        KSpace::KSpaceSampling* m_pSampling;


        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::RoundUpGRT
        //
        // Description :
        ///              \brief     The argument is rounded up to the gradient raster time
        ///                         (10us)
        //
        // Parameters  :
        ///              \param     lDuration_us    Duration to be rounded
        //
        // Return      :
        ///              \return    Duration in us rounded to the gradient raster time
        // 
        // ------------------------------------------------------------------------------
        static long RoundUpGRT (long lDuration);



        // ------------------------------------------------------------------------------
        //
        // Name        :            FISPKernel::RoundUpGRT
        //
        // Description :
        ///              \brief     The argument is rounded up to the gradient raster time
        ///                         (10us)
        //
        // Parameters  :
        ///              \param     dDuration_us    Duration to be rounded
        //
        // Return      :
        ///              \return    Duration in us rounded to the gradient raster time
        // 
        // ------------------------------------------------------------------------------
        static long RoundUpGRT (double dDuration);

};



} // End of namespace SL *


#endif

