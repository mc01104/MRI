//-----------------------------------------------------------------------------
//  Copyright (C) Siemens AG 1998-2006  All Rights Reserved.  Confidential
//-----------------------------------------------------------------------------
//
// Project: NUMARIS/4
//
//    File: \n4\pkg\MrServers\MrImaging\seq\a_MiniFLASH\a_MiniFLASH.cpp
//
// Authors: Mark Brown;       SMS MED US CSG TDC;
//          Brian Dale;       SMS MED US CSG MR R&D;
//          Wolfgang Rehwald; SMS MED US CSG MR R&D;
//          Keith Heberlein;  SMS MED US CSG MR R&D;
//
//          Original Author:  Anonymous
//
//    Lang: C++
//
// Descrip: This is the demo sequence MiniFLASH.
//          DO NOT base any diagnosis on images generated by this sequence.
//
//////////////////////////////////////////////////////////////////////////////////////////
//
//                                MiniFLASH Sequence Diagram
//
//                       |-----------TE-------------|
//
//              RF --|sSRF01|--------------------------------------------
//
//              Gs -|sGSliSel|-|sGSliSelReph|-------------|sGSpoil|------
//
//              Gp ----------|sGPhasEnc|------------------|sGPhasEncRew|-
//
//              Gr ----------|sGReadDeph|-------|sGradRO|----------------
//
//             ADC -----------------------------|sADC01|-----------------
//
//                |------------------------------TR----------------------|
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH.h"
#include "MrServers/MrImaging/seq/a_MiniFLASH/a_MiniFLASH_UI.h"
#include "MrServers/MrMeasSrv/SeqIF/csequence.h"
#include "MrServers/MrImaging/libSeqSysProp/SysProperties.h"
#include "MrServers/MrImaging/seq/SystemProperties.h"        // Siemens system properties
#include "MrServers/MrImaging/ut/libsequt.h"                 // for mSEQTest

#ifdef WIN32
    #include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#endif


// Macro: return S, if S is an error code
// (applies to functions returning MRRESULT-values)
#define OnErrorReturn(S) if(!MrSucceeded(S)) return(S)


#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif



//  --------------------------------------------------------------------------
//
//  Name        :  SEQIF_DEFINE
//
//  Description :
///  \brief        Create instance of the sequence
//
//  Return      :  SeqIF *
//
//  --------------------------------------------------------------------------
#ifdef SEQUENCE_CLASS_MiniFLASH
    SEQIF_DEFINE (SEQ_NAMESPACE::MiniFlash)
#endif
using namespace SEQ_NAMESPACE;

MiniFlash::MiniFlash()
        : m_sSRF01              ("fl_templ_ex")
        , m_sSRF01zSet          ("sSRF01zSet")
        , m_sSRF01zNeg          ("sSRF01zNeg")
        , m_sADC01              ("sADC01")
        , m_sADC01zSet          ("sADC01zSet")
        , m_sADC01zNeg          ("sADC01zNeg")
        , m_sGSliSel            ("sGSliSel")
        , m_sGSliSelReph        ("sGSliSelReph")
        , m_sGradRO             ("sGradRO")
        , m_sGReadDeph          ("sGReadDeph")
        , m_sGPhasTab           ("sGPhasTab")
        , m_sGPhasTabRew        ("sGPhasTabRew")
        , m_sGSpoil             ("sGSpoil")
        , m_lLinesToMeasure     (0)
        , m_lLinesPerSec        (0)
        , m_lCenterLine         (0)
        , m_dRFSpoilIncrement   (0)
        , m_dRFSpoilPhase       (0)
        , m_dMinRiseTime        (100000)
        , m_dGradMaxAmpl        (0)
        , m_pUI                 (NULL) {
    // no further instructions...
}


MiniFlash::~MiniFlash() {
    if(NULL != m_pUI) {
        // delete the user interface (UI) instance (if a UI instance has been created)
        delete m_pUI;
        m_pUI = NULL;
    }
}



NLSStatus MiniFlash::initialize(SeqLim &rSeqLim) {
    // This string is intended to store, where we are in the code (used for debugging by the MRTRACE macro).
    static const char *ptModule = {"MiniFLASH::initialize"};

    // Default return value
    NLS_STATUS lStatus = SEQU__NORMAL;


    // -----------------------
    // Setting the hard limits
    // -----------------------


    // RF frequency is given as a long integer number in Hertz
    // The maximum allowed RF frequency is sufficient for 1H (hydrogen nuclei) in a 3-Tesla-system
    //                         (    min,       max)
    rSeqLim.setAllowedFrequency(8000000, 130850000);


    // The repetition time (TR) and echo time (TE) are given as long integer numbers in micro seconds.
    //                              (index,  min,        max,    increment,  default)
    rSeqLim.setTR                   (    0,  100,    5000000,           10,    20000);
    rSeqLim.setTE                   (    0,  100,     100000,           10,    10000);
    rSeqLim.setBandWidthPerPixel    (    0,   80,        900,           10,      260);


    // The flip angle is given as a double number in degrees.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setFlipAngle            (       10.0,       90.0,          1.0,   15.000);


    // The base resolution tells how many samples in readout direction are acquired in k-space. The increment-value
    // is not set directly. Instead, a key is used which is connected to values of 16, 32, 64, etc. The allowed
    // values can be looked up in "kSpaceDefines.h".
    //                              (        min,        max,    increment,  default)
    rSeqLim.setBaseResolution       (         64,        512,  SEQ::INC_64,      128); //INC_BASE2 also available


    // The field-of-view (FOV) is given as double numbers in mm.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setReadoutFOV           (        100,        500,            1,      300);
    rSeqLim.setPhaseFOV             (        100,        500,            1,      300);


    // Here, we set the number of lines in phase encoding(PE)-direction.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setPELines              (         32,       1024,            1,      128);


    // The MiniFLASH sequence is intended for single-slice measurements only.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setSlices               (          1,          1,            1,        1);


    // The slice thickness is given as a double number in mm.
    //                              (        min,        max,    increment,  default)
    rSeqLim.setSliceThickness       (      2.000,     10.000,        0.500,    5.000);



    // --------------------------------------
    // Instantiation of the MiniFlashUI-class
    // --------------------------------------

    

    // MRRESULT_SEV is a mask for the "severity" bits. This if-statement checks whether the severity bits
    // are set (which indicates that something went wrong).
    if((MRRESULT_SEV & (lStatus = createUI(rSeqLim))) == MRRESULT_SEV) {

        // The MRTRACE macro is used to log information for debugging purposes
        MRTRACE("Instantiation of MiniFlashUI class failed!");
        return lStatus;
    }


    // This compiler directive restricts this code to the HOST version dll.
    // In this way the same source code can be compiled for both the Host and the MARS.
#ifdef WIN32
    if(NULL == m_pUI) {
        MRTRACE("MiniFlashUI object pointer is null (creation failed probably)!");
        return ( SEQU_ERROR );
    }


    // ----------------------------------------------
    // Declaration of pointer to UI parameter classes
    // ----------------------------------------------
    lStatus = m_pUI->registerUI(rSeqLim);

    if(MrSeverity(lStatus) != MRRESULT_SUCCESS) {
        MRTRACE("Registering MiniFlashUI object failed! Errorcode: %i", lStatus);
        return lStatus;
    }

    // -----------------------------------------------------------------
    // file containing the default postprocessing protocol (EVAProtocol)
    // -----------------------------------------------------------------
    
    // The _T(x) macro deals with the unicode conversion of strings.
    rSeqLim.setDefaultEVAProt (_T("%SiemensEvaDefProt%\\Inline\\Inline.evp"));
#endif
    return lStatus;
}



//. -------------------------------------------------------------------------
//.. fSEQPrep: prepare real time elements and calculate energy and timing
//. -------------------------------------------------------------------------
NLSStatus MiniFlash::prepare(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo)
{
    static const char *ptModule = {"MiniFlash::prepare"};
    NLS_STATUS   lStatus = SEQU__NORMAL;                     // * Return status *
    long         lTEFil;
    double       dMeasureTimeUsec      = 0.0;


    // --------------------
    // get some information
    // --------------------
    
    OnErrorReturn(rMrProt.kSpace().linesToMeasure(m_lLinesToMeasure));


    m_lCenterLine  = rMrProt.kSpace().echoLine();

    m_dMinRiseTime = SysProperties::getGradMinRiseTime(rMrProt.gradSpec().mode());
    m_dGradMaxAmpl = SysProperties::getGradMaxAmpl(rMrProt.gradSpec().mode());
    dMeasureTimeUsec = (double)m_lLinesToMeasure * rMrProt.tr()[0];

	// make sure that m_lLinesPerSec is at least 1.
    m_lLinesPerSec =  std::max<long>(1,long(m_lLinesToMeasure * 1000000. /dMeasureTimeUsec));





    // ------------------------------------------------
    // Preparation of all required real time components
    // ------------------------------------------------
    rSeqExpo.setRelevantReadoutsForMeasTime(m_lLinesToMeasure / m_lLinesPerSec);

    //. Prepare RF pulse
    m_sSRF01.setTypeExcitation   ();
    m_sSRF01.setDuration         (2560) ;
    m_sSRF01.setFlipAngle        (rMrProt.flipAngle());
    m_sSRF01.setInitialPhase     (0);
    m_sSRF01.setThickness        (rMrProt.sliceSeries().aFront().thickness());
    m_sSRF01.setSamples          (128);
    m_sSRF01.setBandwidthTimeProduct(2.70);
    if(! m_sSRF01.prepSinc(rMrProt,rSeqExpo)) return (m_sSRF01.getNLSStatus());

    //. Prepare ADC
    m_sADC01.setColumns (rMrProt.kSpace().getlBaseResolution());
    m_sADC01.setDwellTime ((long)((rMrProt.rxSpec().effDwellTime(rSeqLim.getReadoutOSFactor())[0]) + 0.5));
    m_sADC01.getMDH().setKSpaceCentreColumn((unsigned short)(rMrProt.kSpace().getlBaseResolution()/2));
    //. Prepare readout gradient
    if(!m_sGradRO.prepRO(600,(fSDSRoundUpGRT(m_sADC01.getDuration()+600)),
        600,rMrProt,m_sADC01.getDwellTime()))return(m_sGradRO.getNLSStatus());

    //. Prepare and check phase encoding
    if(!m_sGPhasTab.prepPE(600, 1000 , 600, rMrProt, SEQ::DIR_ASCENDING, 0.0,
        m_lLinesToMeasure - m_lCenterLine -1))return(m_sGPhasTab.getNLSStatus());
    if(!m_sGPhasTab.check()) return(m_sGPhasTab.getNLSStatus());
    if(!m_sGPhasTab.prepPE(600, 1000, 600, rMrProt, SEQ::DIR_ASCENDING, 0.0,
        - m_lCenterLine)) return(m_sGPhasTab.getNLSStatus());
    if(!m_sGPhasTab.check()) return(m_sGPhasTab.getNLSStatus());

    // Copy info to PE rewinder
    m_sGPhasTabRew=m_sGPhasTab;

    //. Prepare and check slice selection gradient
    if(!m_sGSliSel.prepAmplitude(600, fSDSRoundUpGRT(m_sSRF01.getDuration()+600),
        600, m_sSRF01.getGSAmplitude()))return(m_sGSliSel.getNLSStatus());
    if (! m_sGSliSel.check() ) return (m_sGSliSel.getNLSStatus());

    //. Check if gradients fit into TE
    long lTEfil = fSDSRoundDownGRT( rMrProt.te()[0]
        - (m_sSRF01.getDuration()/2  + m_sGSliSel.getRampDownTime()
        + m_sADC01.getDuration()/2 + m_sGradRO.getRampUpTime()));
    if ( lTEfil <= 0 ) return SBB_NEGATIV_TEFILL ;

    //. Prepare and check slice rephase gradient
    m_sGSliSelReph.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSliSelReph.setMinRiseTime(m_dMinRiseTime);
    if (! m_sGSliSelReph.prepSymmetricTOTShortestTime(-m_sGSliSel.getMomentum(
        m_sGSliSel.getDuration()-m_sSRF01.getDuration()/2,m_sGSliSel.getTotalTime())))
        return (m_sGSliSelReph.getNLSStatus());

    //. Prepare and check readout dephasing gradient
    m_sGReadDeph.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGReadDeph.setMinRiseTime(m_dMinRiseTime);
    if (! m_sGReadDeph.prepSymmetricTOTShortestTime(-m_sGradRO.getMomentum(
        0,(long)(m_sGradRO.getRampUpTime() + m_sADC01.getDuration()/2))))
        return (m_sGReadDeph.getNLSStatus());

    //. Prepare and check spoiler gradient
    m_sGSpoil.setMaxMagnitude(m_dGradMaxAmpl);
    m_sGSpoil.setMinRiseTime (m_dMinRiseTime);
    if(!m_sGSpoil.prepSymmetricTOTExactMomentum(m_sGSliSel.getMomentumTOT()/2,
        m_sGPhasTabRew.getTotalTime())) return (m_sGSpoil.getNLSStatus());

    //. Check if gradients fit into TE
    long lTENeed =   std::max(m_sGPhasTab.getTotalTime(), m_sGReadDeph.getTotalTime())
        -m_sGSliSel.getRampDownTime();
    lTENeed =   std::max( lTENeed , m_sGSliSelReph.getTotalTime() ) ;
    lTENeed += (long)(m_sSRF01.getDuration()/2 + m_sGSliSel.getRampDownTime()
        + m_sGradRO.getRampUpTime() + m_sADC01.getDuration()/2);
    lTEFil= long(rMrProt.te()[0] - lTENeed);
    if (lTEFil< 0 ) return SBB_NEGATIV_TEFILL ;

    //. Determine the start time of some RT elements and check if gradients fit into TR
    m_sGradRO.setStartTime(fSDSRoundUpGRT(m_sGSliSel.getDuration()
        - m_sSRF01.getDuration()/2 + rMrProt.te()[0]
        - m_sADC01.getRoundedDuration()/2 - m_sGradRO.getRampUpTime()));
    if(rMrProt.tr()[0]<(m_sGradRO.getStartTime()+m_sGradRO.getDuration()
        +std::max(m_sGPhasTabRew.getTotalTime(),m_sGSpoil.getTotalTime())))
        return SBB_NEGATIV_TRFILL ;
    m_sADC01.setStartTime(m_sGradRO.getStartTime() + m_sGradRO.getRampUpTime());

    // Set receiver gain
    OnErrorReturn(fSSLSetRxGain(K_RX_GAIN_CODE_HIGH,rMrProt,rSeqLim));
    // Prepare the slice position array
    OnErrorReturn(fSUPrepSlicePosArray (rMrProt, rSeqLim, m_asSLC));

    // Set sequence string for display in images
    fSUSetSequenceString              ("fl", rMrProt, rSeqExpo);

    //. Fill export section
    rSeqExpo.setRFEnergyInSequence_Ws(m_lLinesToMeasure
        * m_sSRF01.getPulseEnergyWs());
    rSeqExpo.setMeasuredPELines      (m_lLinesToMeasure);
    rSeqExpo.setMeasureTimeUsec      (dMeasureTimeUsec);
    rSeqExpo.setTotalMeasureTimeUsec (dMeasureTimeUsec);
    rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);
    rSeqExpo.setOnlineFFT            (SEQ::ONLINE_FFT_PHASE);
    rSeqExpo.setICEProgramFilename   ("%SiemensIceProgs%\\IceProgramOnline2D");    // Only 2D reconstruction allowed
    
    rSeqExpo.setICEProgramParam(ICE_PROGRAM_PARA_SHOW_OFFLINE, SEQ::SO_SHOW_NO);

    return (lStatus);
}

//. -------------------------------------------------------------------------
//.. fSEQCheck: check lines at the border of k-space (gradient overflow, GSWD)
//. -------------------------------------------------------------------------
NLSStatus MiniFlash::check(MrProt &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo, SEQCheckMode * /*pSEQCheckMode*/)
{
    
    static const char *ptModule = {"MiniFlash::check"};
    NLS_STATUS   lStatus = SEQU__NORMAL;
    long        lL                = 0;
    long        alLCk[4]          = {0, 1, rSeqExpo.getMeasuredPELines() - 2, rSeqExpo.getMeasuredPELines() - 1};
    while (lL < 4 )
    {
        OnErrorReturn(runKernel(rMrProt,rSeqLim,rSeqExpo,KERNEL_CHECK,0, 0, alLCk[lL]));
        lL++;
    }
    return(lStatus);
}

//. -------------------------------------------------------------------------
//.. fSEQRun: run the sequence
//. -------------------------------------------------------------------------
NLSStatus MiniFlash::run(MrProt  &rMrProt, SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo)
{
    static const char *ptModule = {"MiniFlash::run"};
    NLS_STATUS lStatus          = SEQU__NORMAL;
    //. Initialize sequence test
    if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    {
        mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunStart,0,0,0,0,0);
    }
    long lLine;
    long lCurrKernelCalls = 0;

    //. Send the delay between measurements
    OnErrorReturn(fSBBMeasRepetDelaysRun ( rMrProt, rSeqLim, rSeqExpo, 0 ));
    // Fill some entries of the measurement data header
    m_sADC01.getMDH().setEvalInfoMask (m_sADC01.getMDH().getEvalInfoMask() | MDH_ONLINE) ;
    m_sADC01.getMDH().setKSpaceCentreLineNo    ((unsigned short)rMrProt.kSpace().echoLine());
    m_sADC01.getMDH().setKSpaceCentrePartitionNo (0);

    if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    {
        mSEQTest (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockInitTR, 0, 0, m_asSLC[0].getSliceIndex(), 0, 0);
    }

    //. Loop over lines
    for (lLine = 0; lLine < m_lLinesToMeasure; lLine++ )
    {
        lCurrKernelCalls ++;
        // Fill some entries of the measurement data header

        if(!(lCurrKernelCalls % m_lLinesPerSec)) m_sADC01.setRelevantForMeasTime();
        m_sADC01.getMDH().setFirstScanInSlice( lLine == 0 );
        m_sADC01.getMDH().setLastScanInSlice ( lLine == m_lLinesToMeasure - 1);
        m_sADC01.getMDH().setLastScanInConcat( lLine == m_lLinesToMeasure - 1);
        m_sADC01.getMDH().setLastScanInMeas  ( lLine == m_lLinesToMeasure - 1);


        // Call Kernel function
        OnErrorReturn(runKernel(rMrProt, rSeqLim,
            rSeqExpo,KERNEL_IMAGE,0, 0, lLine));
    }
    if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    {
        mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunFinish,0,0,0,0,0);
    }
    return(SEQU__NORMAL);
}


//. -------------------------------------------------------------------------
//.. runKernel
//. -------------------------------------------------------------------------
NLS_STATUS MiniFlash::runKernel(MrProt &rMrProt, SeqLim &rSeqLim,
                             MrProtocolData::SeqExpo &rSeqExpo, long lKernelMode, long /* lSlice */, long /* lPartition */, long lLine)
{
    static const char *ptModule = {"MiniFlash::runKerenl"};
    NLS_STATUS         lStatus  = SEQU__NORMAL;
    //. Prepare Phase encoding and rewinder
    if(! m_sGPhasTab.prepPE(rMrProt,lLine-m_lCenterLine))
        return m_sGPhasTab.getNLSStatus();
    if(! m_sGPhasTabRew.prepPE(rMrProt, m_lCenterLine-lLine) )
        return m_sGPhasTabRew.getNLSStatus();

    //. Fill measurement data header
    m_sADC01.getMDH().setClin            ((unsigned short)lLine);
    m_sADC01.getMDH().setPhaseFT         (lLine == m_lLinesToMeasure - 1);

    //. Prepare FrequencyPhase objects of the RF pulse
    m_sSRF01zSet.prepSet (m_asSLC[0], m_sSRF01) ;
    m_sSRF01zNeg.prepNeg (m_asSLC[0], m_sSRF01) ;

    //. Prepare FrequencyPhase objects of the ADC
    m_sADC01zSet.prepSet (m_asSLC[0], m_sADC01, m_sGradRO,lLine-m_lCenterLine,0) ;
    m_sADC01zNeg.prepNeg (m_asSLC[0], m_sADC01, m_sGradRO,lLine-m_lCenterLine,0) ;

    // Calculate additional phase needed for RF spoiling and add to FreqPhase
    m_dRFSpoilIncrement += RFSPOIL_INCREMENTdeg ;
    m_dRFSpoilPhase += m_dRFSpoilIncrement ;
    m_dRFSpoilPhase     = fmod(m_dRFSpoilPhase,     (double) RFMAXPHASEdeg);
    m_dRFSpoilIncrement = fmod(m_dRFSpoilIncrement, (double) RFMAXPHASEdeg);

    m_sADC01zSet.increasePhase(m_dRFSpoilPhase);
    m_sADC01zNeg.decreasePhase(m_dRFSpoilPhase);

    m_sSRF01zSet.increasePhase(m_dRFSpoilPhase);
    m_sSRF01zNeg.decreasePhase(m_dRFSpoilPhase);

    //. Initialize real time event block with actual rotation matrix
    fRTEBInit( m_asSLC[0].getROT_MATRIX());

    //- ***************************************************** S E Q U E N C E   T I M I N G **********************************************************************
    //- *                             Start Time                   |      NCO     |    SRF   |    ADC   |                Gradient Events                 | Sync
    //- *                               (usec)                     |     Event    |   Event  |   Event  |      phase     |     read     |      slice     | Event
    //- *fRTEI(                                                    ,              ,          ,          ,                ,              ,                ,      );
    //- **********************************************************************************************************************************************************
    fRTEI ( 0                                                      ,             0,         0,         0,               0,             0,     &m_sGSliSel,    0 );
    fRTEI ( m_sGSliSel.getDuration() - m_sSRF01.getDuration()      , &m_sSRF01zSet, &m_sSRF01,         0,               0,             0,               0,    0 );
    fRTEI ( m_sGSliSel.getDuration()                               , &m_sSRF01zNeg,         0,         0,    &m_sGPhasTab, &m_sGReadDeph,               0,    0 );
    fRTEI ( m_sGSliSel.getTotalTime()                              ,             0,         0,         0,               0,             0, &m_sGSliSelReph,    0 );
    fRTEI ( m_sGradRO.getStartTime()                               ,             0,         0,         0,               0,    &m_sGradRO,               0,    0 );
    fRTEI ( m_sADC01.getStartTime()                                , &m_sADC01zSet,         0, &m_sADC01,               0,             0,               0,    0 );
    fRTEI ( m_sADC01.getStartTime() + m_sADC01.getRoundedDuration(), &m_sADC01zNeg,         0,         0,               0,             0,               0,    0 );
    fRTEI ( m_sGradRO.getStartTime() + m_sGradRO.getDuration()     ,             0,         0,         0, &m_sGPhasTabRew,             0,      &m_sGSpoil,    0 );
    fRTEI ( rMrProt.tr()[0]                                        ,             0,         0,         0,               0,             0,               0,    0 );

    //. Call sequence unit test
    if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    {
        if (lKernelMode == KERNEL_CHECK)
            mSEQTest(rMrProt, rSeqLim, rSeqExpo, RTEB_ORIGIN_fSEQCheck,  10, lLine, m_asSLC[0].getSliceIndex(), 0, 0);
        else
            mSEQTest(rMrProt, rSeqLim, rSeqExpo, RTEB_ORIGIN_fSEQRunKernel,10,lLine,m_asSLC[0].getSliceIndex(), 0, 0);
        mSEQTest    (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockCheck,        10, lLine, m_asSLC[0].getSliceIndex(), 0, 0);
    }
    //. End real time event block
    OnErrorReturn((NLS_STATUS)fRTEBFinish());

    return(lStatus);
}


NLS_STATUS MiniFlash::createUI (SeqLim&)
{
    #ifdef WIN32

        static const char *ptModule = {"MiniFlash::createUI"};

        //  ----------------------------------------------------------------------
        //  Delete existing instance if necessary
        //  ----------------------------------------------------------------------
        if(m_pUI)  {
            delete m_pUI;
            m_pUI = NULL;
        }



        //  ----------------------------------------------------------------------
        //  Instantiation of the UI class
        //  ----------------------------------------------------------------------
        try  {
            m_pUI = new MiniFlashUI();
        }

        catch (...)  {
            delete m_pUI;
            m_pUI = NULL;

            TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: Cannot instantiate UI class !", ptModule);
            return ( SEQU_ERROR );
        }

    #endif

    return ( SEQU_NORMAL );

}   // end: MiniFlash::createUI



const MiniFlashUI* MiniFlash::getUI (void) const
{
    return ( m_pUI );
}