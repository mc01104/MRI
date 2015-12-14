//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FISPKernel.cpp
//	 Version: \main\21
//	  Author: Clinical
//	    Date: 2011-07-25 15:03:39 +02:00
//
//	    Lang: C++
//
//	 Descrip: Deklarations for FISPKernel.cpp
//
//
//  EGA Requirement Key: As shown on the following lines:
//
//   Abbrev.   Translation                                         Relevant for
//   -------   -----------                                         ------------
//   EGA-All   All of the following keys:                          All EGA requirements
//   EGA-Any   All or any of the following keys:                   All EGA requirements
//   EGA-01    {:IMPLEMENT:000_EGA_BildOri_SW_SequenzROVz::}       GR/GP   polarity
//   EGA-02    {:IMPLEMENT:000_EGA_BildPos_SW_SequenzSSelVz::}     GS      polarity
//   EGA-03    {:IMPLEMENT:000_EGA_BildMass_SW_SequenzROPC::}      GR/GP   amplitude
//   EGA-04    {:IMPLEMENT:000_EGA_BildPos_SW_SequenzSSel::}       GS      amplitude
//   EGA-05    {:IMPLEMENT:000_EGA_BildPos_SW_NCOFrequenzSSel::}   SRF     frequency
//   EGA-06    {:IMPLEMENT:000_EGA_BildPos_SW_NCOFrequenzRO::}     Readout frequency
//   EGA-07    {:IMPLEMENT:000_EGA_BildOri_SW_OrientierungTest::}  Image orientation
//
//	-----------------------------------------------------------------------------



// * -------------------------------------------------------------------------- *
// * Includes                                                                   *
// * -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/seq/FLASH/FISPKernel.h"
#include "MrServers/MrImaging/libSL/StdLoopScopes.h"
#include "MrServers/MrImaging/libSL/StdSL_ID.h"

#include "MrServers/MrImaging/libCSL/Mediator.h"

#include "MrServers/MrImaging/libSeqUtil/SeqTrace.h"
#include "ProtBasic/Interfaces/SeqLim.h"
#include "MrServers/MrProtSrv/MrProt/MrProt.h"
#include "MrServers/MrProtSrv/MrProt/MrProtSliceSeries.h"
#include "MrServers/MrProtSrv/MrProt/KSpace/MrKSpace.h"
#include "MrServers/MrProtSrv/MrProt/MeasParameter/MrRXSpec.h"
#include "MrServers/MrProtSrv/MrProt/SeqIF/SeqExpo.h"
#include "MrServers/MrMeasSrv/SeqIF/MDH/MdhProxy.h"
#include "MrServers/MrMeasSrv/SeqFW/libGSL/libGSL.h"
#include "MrServers/MrImaging/libSeqSysProp/SysProperties.h"
#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/libRT.h"

#include "MrServers/MrMeasSrv/SeqIF/libRT/include/ISLICE_POS.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSLICE_POS.h"


#include "MrCommon/UTrace/Macros.h"



// ----------------------------------------------------------------------------
// Namespace
// ----------------------------------------------------------------------------
using namespace SL;
using namespace KSpace;


// ----------------------------------------------------------------------------
// Definition of IDs
// ----------------------------------------------------------------------------
const Key<double> FISPKernel::d_MIN_RISE_TIME       ("d_MinRiseTime"     );
const Key<double> FISPKernel::d_MAX_GRAD_AMPLITUDE  ("d_MaxGradAmplitude");




FISPKernel::FISPKernel (const std::string& sIdent, Mediator* pMediator)
    : CompositeKernel  (SLS::STD, sIdent, pMediator)
    , m_OscBit              ("Osc0")
    , m_SRF                 ("SRFExcit")
    , m_SRFzSet             ("SRFzSet")
    , m_SRFzNeg             ("SRFzNeg")
    , m_ADC                 ("ADC")
    , m_ADCzSet             ("ADCzSet")
    , m_ADCzNeg             ("ADCzNeg")
    , m_RODeph              ("RODeph")
    , m_RO                  ("RO")
    , m_ROSpoil             ("ROSpoiler")
    , m_PE                  ("PE")
    , m_PERew               ("PERew")
    , m_3D                  ("3D")
    , m_3DRew               ("3DRew")
    , m_GSlcSel             ("SlcSel")
    , m_GSlcSelReph         ("SlcSelReph")
    , m_lExcitationTime_us  (0)
    , m_lPrephaseTime_us    (0)
    , m_lReadOutTime_us     (0)
    , m_lRephaseTime_us     (0)
    , m_lTEFill_us          (0)
    , m_pSampling           (NULL)
{}


FISPKernel::FISPKernel (const std::string& sScope, const std::string& sIdent, Mediator* pMediator)
    : CompositeKernel  (sScope, sIdent, pMediator)
    , m_OscBit              ("Osc0")
    , m_SRF                 ("SRFExcit")
    , m_SRFzSet             ("SRFzSet")
    , m_SRFzNeg             ("SRFzNeg")
    , m_ADC                 ("ADC")
    , m_ADCzSet             ("ADCzSet")
    , m_ADCzNeg             ("ADCzNeg")
    , m_RODeph              ("RODeph")
    , m_RO                  ("RO")
    , m_ROSpoil             ("ROSpoiler")
    , m_PE                  ("PE")
    , m_PERew               ("PERew")
    , m_3D                  ("3D")
    , m_3DRew               ("3DRew")
    , m_GSlcSel             ("SlcSel")
    , m_GSlcSelReph         ("SlcSelReph")
    , m_lExcitationTime_us  (0)
    , m_lPrephaseTime_us    (0)
    , m_lReadOutTime_us     (0)
    , m_lRephaseTime_us     (0)
    , m_lTEFill_us          (0)
    , m_pSampling           (NULL)
{}


FISPKernel::~FISPKernel ()
{}




bool FISPKernel::prep (MrProt& rMrProt, SeqLim& rSeqLim, MrProtocolData::SeqExpo& rSeqExpo)
{

    static const char* ptModule = "FISPKernel::prep";

    // ------------------------------------------------------------------------
    // Prepare leafs
    // ------------------------------------------------------------------------
    if ( getPrepMode() == PREP_LEAFS)  {

        NLS_STATUS Status = SBB_NORMAL;


        // --------------------------------------------------------------------
        // Get center line and partition index
        // --------------------------------------------------------------------

        long lCenterLine      = -1;
        long lCenterPartition = -1;

        if (m_pSampling != NULL) {
            SamplingInfo Info = m_pSampling->getSamplingInfo();

            lCenterLine      = Info.m_iCenterLine;
            lCenterPartition = Info.m_iCenterPartition;
        } else {
            lCenterLine      = rMrProt.kSpace().echoLine();
            lCenterPartition = rMrProt.kSpace().echoPartition();
        }


        // --------------------------------------------------------------------
        // Retrieve parameter from the mediator
        // --------------------------------------------------------------------
        double  dMinRiseTime      = m_pMediator->getVal (FISPKernel::d_MIN_RISE_TIME     );
        double  dMaxGradAmplitude = m_pMediator->getVal (FISPKernel::d_MAX_GRAD_AMPLITUDE);


        // --------------------------------------------------------------------
        // Register parameter at the mediator
        // --------------------------------------------------------------------
        m_pMediator->regFct (SLK::l_ADC_COUNTER, &FISPKernel::getADCCounter);



        // --------------------------------------------------------------------
        // Preparation of OscBit
        // --------------------------------------------------------------------
        m_OscBit.setCode      (SYNCCODE_OSC0);
        m_OscBit.setDuration  (10);
        m_OscBit.setStartTime (0);
        m_OscBit.prep         (0, 10);   // Channel: 0, duration: 10us



        // --------------------------------------------------------------------
        // Prepare the RF pulse object
        // --------------------------------------------------------------------
        m_SRF.setTypeExcitation       ();
        m_SRF.setFlipAngle            (rMrProt.flipAngle());
        m_SRF.setAsymmetry            (0.5);
        m_SRF.setInitialPhase         (0);
        m_SRF.setThickness            (rMrProt.sliceSeries().aFront().thickness());
        m_SRF.setDuration             (2560);
        m_SRF.setSamples              (128);
        m_SRF.setBandwidthTimeProduct (2.70);

        if (! m_SRF.prepSinc (rMrProt, rSeqExpo))   {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of rf pulse %s failed, %s %d", ptModule, m_SRF.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_SRF.getNLSStatus());
            return (false);
        }



        // --------------------------------------------------------------------
        // Prepare slice selection gradient
        // --------------------------------------------------------------------
        int32_t lSliSelRampTime = static_cast<int32_t>(fSDSRoundUpGRT( std::max<double>(SysProperties::getCoilCtrlLead(), dMinRiseTime * m_SRF.getGSAmplitude())));

        if (! m_GSlcSel.prepAmplitude (lSliSelRampTime, fSDSRoundUpGRT (m_SRF.getDuration() + lSliSelRampTime), lSliSelRampTime, m_SRF.getGSAmplitude() ) )   {  /*! EGA-04; EGA-02 !*/
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation the slice selection gradient %s failed, %s %d", ptModule, m_GSlcSel.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_GSlcSel.getNLSStatus());
            return (false);
        }


        if (! m_GSlcSel.check() )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Checking of the phase encoding gradient %s failed, %s %d", ptModule, m_GSlcSel.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_GSlcSel.getNLSStatus());
            return (false);
        }



        // --------------------------------------------------------------------
        // Calculate the excitation time
        //
        // Note: Ramp down of the slice selection gradient is not included in 
        //       the excitation time but in the prephasing time
        // --------------------------------------------------------------------
        m_lExcitationTime_us = m_GSlcSel.getDuration();



        // --------------------------------------------------------------------
        // Prepare the readout (ADC) object
        // --------------------------------------------------------------------
        double dEffectiveDwellTime_ns = rMrProt.rxSpec().effDwellTime( rSeqLim.getReadoutOSFactor() )[0];
        m_ADC.prep (rMrProt.kSpace().getlBaseResolution(), static_cast<int32_t>(dEffectiveDwellTime_ns + 0.5));



        // --------------------------------------------------------------------
        // Prepare the readout gradient
        // The ramp down time is calculated considering a readout spoiler that 
        // is directly attached to the readout gradient using maximum allowed
        // gradient amplitude
        // --------------------------------------------------------------------
        m_RO.setMaxMagnitude (dMaxGradAmplitude);
        m_RO.setMinRiseTime  (dMinRiseTime);
        m_RO.setAmplitude    (fGSLGetROMoment (rMrProt) / m_ADC.getDuration());
        m_RO.setRampUpTime   (RoundUpGRT (m_RO.getAmplitude() * dMinRiseTime));
        m_RO.setRampDownTime (RoundUpGRT (fabs((dMaxGradAmplitude - fabs(m_RO.getAmplitude())) * dMinRiseTime )));
        m_RO.setDuration     (RoundUpGRT (ceil (m_ADC.getDuration()) + static_cast<double>(m_RO.getRampUpTime())));             /*! EGA-03; EGA-01 !*/

        if( !m_RO.prep() )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of the readout gradient %s failed, %s %d", ptModule, m_RO.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_RO.getNLSStatus());
            return (false);
        }



        // --------------------------------------------------------------------
        // Check the readout gradient, amplitude must not exceed the maximum
        // allowed value
        // --------------------------------------------------------------------
        if (! m_RO.check (0.0, dMaxGradAmplitude) )   {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Checking the readout gradient %s failed, %s %d", ptModule, m_RO.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_RO.getNLSStatus());
            return (false);
        }


        // --------------------------------------------------------------------
        // Calculate the readout time
        //
        // Note: Ramp up of the readout gradient is not included in the readout
        //       time but in the prephasing time. The ramp down time of the 
        //       readout gradient is included in the dephasing time.
        // --------------------------------------------------------------------
        m_lReadOutTime_us = m_RO.getFlatTopTime();



        // --------------------------------------------------------------------
        // Prepare phase encoding gradient
        // --------------------------------------------------------------------
        if ( ! m_PE.prepPEShortestTime (rMrProt, SEQ::DIR_ASCENDING, 0.0, - lCenterLine) )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of the phase encoding table failed, %s %s %d", ptModule, m_PE.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (Status);
            return (false);
        }



        // --------------------------------------------------------------------
        // Calculate the moment required to rephase the magnetization after 
        // slice selection
        // --------------------------------------------------------------------
        const double dSliceRephasingMoment = - m_SRF.getGSAmplitude() * ( static_cast<double>(m_SRF.getDuration()) * m_SRF.getAsymmetry() + lSliSelRampTime / 2.0 );


        // --------------------------------------------------------------------
        // Prepare slice selection rewinder / 3D phase encoding table
        // --------------------------------------------------------------------
        switch (rMrProt.kSpace().getucDimension())  {

            case SEQ::DIM_2:

                // ------------------------------------------------------------
                // Prepare slice selection gradient rewinder using the shortest
                // possible timing for the given minimum rise time and maximum
                // amplitude
                // ------------------------------------------------------------
                m_GSlcSelReph.setMaxMagnitude (dMaxGradAmplitude);
                m_GSlcSelReph.setMinRiseTime  (dMinRiseTime);

                if (! m_GSlcSelReph.prepSymmetricTOTShortestTime (dSliceRephasingMoment))   {  /*! EGA-04; EGA-02 !*/
                    if ( rSeqLim.isContextNormal() )  {
                        SEQ_TRACE_ERR.print ("%s Preparation the slice rephasing gradient %s failed, %s %d", ptModule, m_GSlcSelReph.getIdent(), __FILE__, __LINE__);
                    }
                    setNLSStatus (m_GSlcSelReph.getNLSStatus());
                    return (false);
                }


                // ------------------------------------------------------------
                // Partition encoding table is disable for 2D imaging
                // ------------------------------------------------------------
                m_3D.prepAmplitude (0, 0, 0, 0.0);

            break;

            case SEQ::DIM_3:

                    // --------------------------------------------------------
                    // m_GSlcSelReph is disabled since slice rewinding can be
                    // achieved by adding a gradient offset to the partition 
                    // encoding table
                    // --------------------------------------------------------
                    m_GSlcSelReph.prepAmplitude (0, 0, 0, 0.0);


                    // --------------------------------------------------------
                    // Prepare partition encoding gradient
                    // --------------------------------------------------------
                    if ( ! m_3D.prep3DShortestTime (rMrProt, SEQ::DIR_ASCENDING, dSliceRephasingMoment, - lCenterPartition) )  {
                        if ( rSeqLim.isContextNormal() )  {
                            SEQ_TRACE_ERR.print ("%s Preparation of the partiton encoding table failed, %s %s %d", ptModule, m_3D.getIdent(), __FILE__, __LINE__);
                        }
                        setNLSStatus (Status);
                        return (false);
                    }

            break;

            default:
                SEQ_TRACE_ERR.print ("%s Invalid selection, %s %d", ptModule, __FILE__, __LINE__);
                setNLSStatus (SEQU_ERROR);
                return (false);
        }



        // --------------------------------------------------------------------
        // Prepare read dephase gradient
        // --------------------------------------------------------------------
        const double dReadOutDephasingMoment = - m_RO.getAmplitude() * (m_ADC.getDuration() + static_cast<double>(m_RO.getRampUpTime())) / 2.0;

        m_RODeph.setMaxMagnitude (dMaxGradAmplitude);
        m_RODeph.setMinRiseTime  (dMinRiseTime);
        
        if (! m_RODeph.prepSymmetricTOTShortestTime (dReadOutDephasingMoment))  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of the readout dephasing gradient %s failed, %s %d", ptModule, m_RODeph.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_RODeph.getNLSStatus());
            return (false);
        }

        if (! m_RODeph.check() )     {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Checking of the readout dephasing gradient %s failed, %s %d", ptModule, m_RODeph.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_RODeph.getNLSStatus());
            return (false);
        }



        // --------------------------------------------------------------------
        // Calculate the prephasing time
        //
        // Note: Ramp down time of the slice selection gradient and the ramp up
        //       time of the readout gradient are included in the prephasing
        //       time
        // --------------------------------------------------------------------
        switch (rMrProt.kSpace().getucDimension())  {
            case SEQ::DIM_2:
                m_lPrephaseTime_us = std::max (m_GSlcSel.getRampDownTime() + m_GSlcSelReph.getTotalTime(), m_PE.getTotalTime() );
            break;

            case SEQ::DIM_3:
                m_lPrephaseTime_us = std::max (m_GSlcSel.getRampDownTime() + m_3D.getTotalTime(), m_PE.getTotalTime() );
            break;

            default:
                SEQ_TRACE_ERR.print ("%s Invalid selection, %s %d", ptModule, __FILE__, __LINE__);
                setNLSStatus (SEQU_ERROR);
                return (false);
        }

        m_lPrephaseTime_us = std::max (m_lPrephaseTime_us, m_RODeph.getTotalTime() + m_RO.getRampUpTime() );



        // --------------------------------------------------------------------
        // Prepare phase encoding rewinder gradient using the same timing as
        // the phase encoding gradient
        // --------------------------------------------------------------------
        if ( ! m_PERew.prepPEShortestTime (rMrProt, SEQ::DIR_DESCENDING, 0.0, - lCenterLine) )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of the phase encoding table rewinder failed, %s %s %d", ptModule, m_PERew.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (Status);
            return (false);
        }


        // --------------------------------------------------------------------
        // Prepare partition encoding rewinder using the same timing as the
        // partition encoding gradient
        // --------------------------------------------------------------------
        if ( rMrProt.kSpace().getucDimension() == SEQ::DIM_3 )  {

            if ( ! m_3DRew.prep3DShortestTime (rMrProt, SEQ::DIR_DESCENDING, 0.0, - lCenterPartition) )  {
                if ( rSeqLim.isContextNormal() )  {
                    SEQ_TRACE_ERR.print ("%s Preparation of the partiton encoding table rewinder failed, %s %s %d", ptModule, m_3DRew.getIdent(), __FILE__, __LINE__);
                }
                setNLSStatus (Status);
                return (false);
            }

        } else {

            // ------------------------------------------------------------
            // Partition encoding rewinder is disable for 2D imaging
            // ------------------------------------------------------------
            m_3DRew.prepAmplitude (0, 0, 0, 0.0);

        }


        // --------------------------------------------------------------------
        // Prepare the readout spoiler
        // Note: The readout gradient and the readout spoiler merge seamlessly.
        //       I.e. the ramp times of bith gradients have to fit to each 
        //       other.
        // --------------------------------------------------------------------
        m_ROSpoil.setMaxMagnitude (dMaxGradAmplitude);
        m_ROSpoil.setMinRiseTime  (dMinRiseTime);

        if (! m_ROSpoil.prepSymmetricFLTShortestTime (0.5 * m_RO.getMomentumTOT()))  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation of the readout spoiler gradient failed: %s, %s %d", ptModule, m_ROSpoil.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (SBB_NEGATIV_TEFILL);
            return (false);
        }


        // --------------------------------------------------------------------
        // Adjust readout spoiler ramp up time in order to match the readout
        // gradient ramp down time
        // --------------------------------------------------------------------
        m_ROSpoil.setRampUpTime   (m_RO.getRampDownTime());



        // --------------------------------------------------------------------
        // Calculate the rephasing time
        //
        // Note: Ramp down time of the readout gradient are included in the 
        //       prephasing time
        // --------------------------------------------------------------------
        m_lRephaseTime_us = std::max (m_ROSpoil.getTotalTime(), m_PERew.getTotalTime() );

        if (rMrProt.kSpace().getucDimension() == SEQ::DIM_3)  {
            m_lRephaseTime_us = std::max (m_lRephaseTime_us, m_3DRew.getTotalTime() );
        }



        // --------------------------------------------------------------------
        // Calculate TEFill-times and check, whether timing can be realized
        // --------------------------------------------------------------------
        m_lTEFill_us = RoundUpGRT (rMrProt.te()[0] - static_cast<long>(m_SRF.getDuration() * m_SRF.getAsymmetry()) - m_lPrephaseTime_us - static_cast<long>(m_ADC.getDuration() / 2.0));


        if ( m_lTEFill_us < 0 )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Negative TE fill time occurred: %ld, %s %d", ptModule, m_lTEFill_us, __FILE__, __LINE__);
            }
            setNLSStatus (SBB_NEGATIV_TEFILL);
            return (false);
        }



        // --------------------------------------------------------------------
        // Calculate energy and duration of the kernel element
        // --------------------------------------------------------------------
        m_lDuration_us = m_lExcitationTime_us + m_lPrephaseTime_us + m_lTEFill_us + m_lReadOutTime_us + m_lRephaseTime_us;
        m_dEnergy_Ws   = m_SRF.getPulseEnergyWs();



        // --------------------------------------------------------------------
        // Mark kernel as successfully prepared
        // --------------------------------------------------------------------
        setPrepared();

    }


    return (true);

}




bool FISPKernel::run (MrProt& rMrProt, SeqLim& rSeqLim, MrProtocolData::SeqExpo& rSeqExpo, sSLICE_POS* pSLC)
{
    static const char* ptModule = "FISPKernel::run";

    // ------------------------------------------------------------------------
    // Check whether the kernel has successfully been prepared
    // ------------------------------------------------------------------------
    if ( isPrepared (SL::ELEMENT) )  {

        NLS_STATUS Status = SBB_NORMAL;
        

        // --------------------------------------------------------------------
        // Reset ADC counter for each new measurement
        // --------------------------------------------------------------------
        m_ADCCounter.resetIfChanged (m_pMediator->getVal (SLK::l_MEASUREMENT_LOOP_COUNTER));


        // --------------------------------------------------------------------
        // Get k-space sampling information
        // --------------------------------------------------------------------

        // The ScanInfo object holds standard information provided by the StdMediator
        SL::ScanInfo& rSI = m_pMediator->getScanInfo (rMrProt);

        // The measurement data header will be read by ICE
        MdhProxy& rMDH = m_ADC.getMDH();

        long lLineId      = 0;        // Line index
        long lPartitionId = 0;        // Partition index

        bool bPhaseFT       = false;  // Flag that tells that all points in this partition have been visited
        bool bPartitionFT   = false;  // Flag that tells that all points in this line have been visited

        bool bRefScan       = false;  // PAT reference scan
        bool bRefAndImaScan = false;  // PAT reference scan and imaging scan


        long lCenterLine      = -1;
        long lCenterPartition = -1;


        // --------------------------------------------------------------------
        // Check if KSpaceSampling instance is used
        // --------------------------------------------------------------------
        if (m_pSampling != NULL) {

            SamplingInfo Info = m_pSampling->getSamplingInfo();

            lCenterLine      = Info.m_iCenterLine;
            lCenterPartition = Info.m_iCenterPartition;


            // --------------------------------------------------------------------
            // Get k-space information from KSpaceSampling instance
            // --------------------------------------------------------------------

            if (!m_pSampling->isCalculated()) {
                UTRACE (Error, iKernelUTraceMarker, "KSpaceSampling has not been calculated before run/check of the sequence!");
                throw MiscException (SEQU_ERROR, ptModule, __FILE__, __LINE__);
            }
            
            // retrieve the scan loop counter (i.e. the measurement position)
            long lScanLoopCounter = m_pMediator->getVal (SLK::l_SCAN_LOOP_COUNTER);

            // retrieve current element
            KSpacePoint Point;
            if (m_pSampling->getKSpacePointForLoopIndex (lScanLoopCounter, Point)) {
                // get correct line and partition index
                lLineId      = static_cast <long> (Point.getLineId());
                lPartitionId = static_cast <long> (Point.getPartitionId());

                bPhaseFT       = Point.getLineFTFlag();
                bPartitionFT   = Point.getPartitionFTFlag();
                bRefScan       = Point.getPATRefScanFlag();
                bRefAndImaScan = Point.getImageScanFlag();
            }

        } else {

            // --------------------------------------------------------------------
            // Case of no KSpaceSampling-class being used
            // --------------------------------------------------------------------
            //
            // NOTE: The following code assumes that a separate lines and
            //       partitions loop is inserted in the loop structure. Please
            //       make the required changes in the method. Otherwise, the lines
            //       and partitions index will not be correct.
            //
            //       FLASH::prepareCompositeSeqLoop (...)


            // --------------------------------------------------------------------
            // Get k-space information from ScanInfo
            // --------------------------------------------------------------------

            lLineId      = rSI.getusClin();
            lPartitionId = rSI.getusCpar();

            lCenterLine      = rMrProt.kSpace().echoLine();
            lCenterPartition = rMrProt.kSpace().echoPartition();


            // --------------------------------------------------------------------
            // Calculate line and partition index in k-space for check mode
            // --------------------------------------------------------------------
            //
            // The following code shows how the positions to be checked could be defined
            // at this position.
            //
            // NOTE: This code will not consider PAT, PF, elliptical scanning, etc.!!!
            
            if (getScanContext() == SL::CHECK) {
                switch (lLineId % 2) {
                    case 0: lLineId = 0; break;
                    case 1: lLineId = lCenterLine * 2 - 1; break;
                }
            
                switch (lPartitionId % 2) {
                    case 0: lPartitionId = 0; break;
                    case 1: lPartitionId = lCenterPartition * 2 - 1; break;
                }
            }
        }


        // --------------------------------------------------------------------
        // Set the MDH parameters
        // --------------------------------------------------------------------

        rMDH.setClin (static_cast<unsigned short>(lLineId));
        rMDH.setCpar (static_cast<unsigned short>(lPartitionId));
        rMDH.setCrep (rSI.getusCrep());
        rMDH.setCacq (rSI.getusCacq());
        rMDH.setCslc (rSI.getusCslc());
        rMDH.setCphs (rSI.getusCphs());

        rMDH.setPhaseFT          (bPhaseFT);
        rMDH.setPartitionFT      (bPartitionFT);
        rMDH.setPATRefScan       (bRefScan);
        rMDH.setPATRefAndImaScan (bRefAndImaScan);

        rMDH.setLastScanInMeas   (rSI.getbLastScanInMeas()  );
        rMDH.setLastScanInConcat (rSI.getbLastScanInConcat());
        rMDH.setFirstScanInSlice (rSI.getbFirstScanInSlice());
        rMDH.setLastScanInSlice  (rSI.getbLastScanInSlice() );
        rMDH.setTREffectiveBegin (rSI.getbEffectiveTRBegin());
        rMDH.setTREffectiveEnd   (rSI.getbEffectiveTREnd()  );
        m_ADC.setRelevantForMeasTime (rSI.getbRelevantForMeasTime());

        rMDH.setKSpaceCentreColumn (static_cast<unsigned short>(0.5 * rMrProt.kSpace().getlBaseResolution()));
        rMDH.setKSpaceCentreLineNo (static_cast<unsigned short>(lCenterLine));
        if (rMrProt.kSpace().getucDimension() == SEQ::DIM_3)  {
            rMDH.setKSpaceCentrePartitionNo (static_cast<unsigned short>(lCenterPartition));
        } else {
            rMDH.setKSpaceCentrePartitionNo (static_cast<unsigned short>(0));
        }

        rMDH.addToEvalInfoMask (MDH_ONLINE);


        long lKSpaceLineId      = lLineId      - lCenterLine;
        long lKSpacePartitionId = lPartitionId - lCenterPartition;


        // --------------------------------------------------------------------
        // Update the phase-encoding tables
        // --------------------------------------------------------------------

        if (! m_PE.prepPE (rMrProt, lKSpaceLineId) )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation the partition encoding gradient %s failed, %s %d", ptModule, m_PE.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_PE.getNLSStatus());
            return (false);
        }

        if (! m_PERew.prepPE (rMrProt, lKSpaceLineId) )  {
            if ( rSeqLim.isContextNormal() )  {
                SEQ_TRACE_ERR.print ("%s Preparation the partition encoding gradient %s failed, %s %d", ptModule, m_PERew.getIdent(), __FILE__, __LINE__);
            }
            setNLSStatus (m_PERew.getNLSStatus());
            return (false);
        }

        if (rMrProt.kSpace().getucDimension() == SEQ::DIM_3)  {

            if (! m_3D.prep3D (rMrProt, lKSpacePartitionId) )  {
                if ( rSeqLim.isContextNormal() )  {
                    SEQ_TRACE_ERR.print ("%s Preparation the partition encoding gradient %s failed, %s %d", ptModule, m_3D.getIdent(), __FILE__, __LINE__);
                }
                setNLSStatus (m_3D.getNLSStatus());
                return (false);
            }

            if (! m_3DRew.prep3D (rMrProt, lKSpacePartitionId) )  {
                if ( rSeqLim.isContextNormal() )  {
                    SEQ_TRACE_ERR.print ("%s Preparation the partition encoding gradient %s failed, %s %d", ptModule, m_3DRew.getIdent(), __FILE__, __LINE__);
                }
                setNLSStatus (m_3DRew.getNLSStatus());
                return (false);
            }
        }



        // --------------------------------------------------------------------
        // Set the frequency/phase properties of the RF pulses
        // --------------------------------------------------------------------
        m_SRFzSet.prepSet (*pSLC, m_SRF) ;    /*! EGA-05 !*/
        m_SRFzNeg.prepNeg (*pSLC, m_SRF) ;    /*! EGA-05 !*/



        // --------------------------------------------------------------------
        // Set the frequency/phase properties of the readout events
        // Line and partition counter are required in order to realize 
        // offcenter shifts of the FoV
        // --------------------------------------------------------------------
        m_ADCzSet.prepSet (*pSLC, m_ADC, m_RO, lKSpaceLineId, lKSpacePartitionId) ;   /*! EGA-06 !*/
        m_ADCzNeg.prepNeg (*pSLC, m_ADC, m_RO, lKSpaceLineId, lKSpacePartitionId) ;   /*! EGA-06 !*/



        // --------------------------------------------------------------------
        // Handle RF spoiling
        // --------------------------------------------------------------------
        m_SRFzSet.increasePhase (m_dPhaseIncr);
        m_SRFzNeg.decreasePhase (m_dPhaseIncr);

        m_ADCzSet.increasePhase (m_dPhaseIncr);
        m_ADCzNeg.decreasePhase (m_dPhaseIncr);



        // ------------------------------------------------------------------------
        // Begin of event block
        // ------------------------------------------------------------------------
        fRTEBInit (pSLC->getROT_MATRIX());    /* EGA-07 */

        long lT = 0;
        long lStartTimeSRF = m_GSlcSel.getDuration() - m_SRF.getDuration();

        // ***************************************************** S E Q U E N C E   T I M I N G *************************************
        // *           Start Time    |    NCO    |  SRF  |  ADC  |             Gradient Events             | Sync
        // *             (usec)      |   Event   | Event | Event |     phase   |    read     |    slice    | Event
        // *************************************************************************************************************************
        fRTEI (lT                    ,          0,      0,      0,            0,            0,   &m_GSlcSel,&m_OscBit);
        fRTEI (lStartTimeSRF         , &m_SRFzSet, &m_SRF,      0,            0,            0,            0,        0);
        fRTEI (lT + m_GSlcSel.getDuration(),&m_SRFzNeg, 0,      0,            0,            0,            0,        0);

        if ( rMrProt.kSpace().getucDimension() == SEQ::DIM_2 )  {
            fRTEI (lT + m_GSlcSel.getTotalTime(),0,     0,      0,            0,            0,&m_GSlcSelReph,       0);
        } else {
            fRTEI (lT + m_GSlcSel.getTotalTime(),0,     0,      0,            0,            0,        &m_3D,        0);
        }

        fRTEI (m_lExcitationTime_us + m_lTEFill_us
                                     ,          0,      0,      0,        &m_PE,    &m_RODeph,            0,        0);

        // Calculate the readout gradient start time
        lT = m_lExcitationTime_us + m_lPrephaseTime_us + m_lTEFill_us - m_RO.getRampUpTime();
        fRTEI (lT                    ,          0,      0,      0,            0,        &m_RO,            0,        0);
        fRTEI (lT += m_RO.getRampUpTime(),
                                       &m_ADCzSet,      0, &m_ADC,            0,            0,            0,        0);
        fRTEI (lT + m_ADC.getRoundedDuration(),
                                         &m_ADCzNeg,    0,      0,            0,            0,            0,        0);
        fRTEI (m_lExcitationTime_us + m_lTEFill_us + m_lPrephaseTime_us + m_lReadOutTime_us
                                     ,          0,      0,      0,     &m_PERew,   &m_ROSpoil,     &m_3DRew,        0);
        fRTEI (m_lDuration_us        ,          0,      0,      0,            0,            0,            0,        0);



        // --------------------------------------------------------------------
        // Increase ADC counter
        // --------------------------------------------------------------------
        if ( getScanContext() == SL::IMAGING )  {
            m_ADCCounter.increment();
        }



        // --------------------------------------------------------------------
        // Handle Unit test
        // --------------------------------------------------------------------
        unsigned long ulTestIdent = RTEB_ORIGIN_NoName;
        if (getScanContext() == SL::CHECK)  {
            ulTestIdent = RTEB_ORIGIN_fSEQCheck;
        } else {
            ulTestIdent = RTEB_ORIGIN_fSEQRunKernel;
        }

        mSEQTest (rMrProt, rSeqLim, rSeqExpo, ulTestIdent    , 10, lLineId, pSLC->getSliceIndex(), 0, 0) ;
        mSEQTest (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockCheck, 10, lLineId, pSLC->getSliceIndex(), 0, 0) ;



        // --------------------------------------------------------------------
        // Close the event block
        // --------------------------------------------------------------------
        setNLSStatus (Status = fRTEBFinish());

        return ((Status & NLS_SEV) == NLS_SUCCESS);

    } else {
        SEQ_TRACE_ERR.print ("%s FISPKernel has not been prepared, %s %d", ptModule, __FILE__, __LINE__);
        setNLSStatus (SBB_ERROR);
        return (false);
    }

    return (true);

}


int32_t FISPKernel::getADCCounter (void) const
{
    return (m_ADCCounter.getCounter());
}


void FISPKernel::setSamplingInstance (KSpaceSampling* pSampling)
{
    m_pSampling = pSampling;
}


long FISPKernel::RoundUpGRT (long lDuration)
{
    return ( (long)((lDuration + eGradRasterTime - 1 ) / eGradRasterTime) * eGradRasterTime );
}


long FISPKernel::RoundUpGRT (double dDuration)
{
    return ( (long)(ceil(dDuration / eGradRasterTime)) * eGradRasterTime );
}
