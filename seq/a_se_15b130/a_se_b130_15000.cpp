//  [ Compilation unit *******************************************************
//
//  Project: NUMARIS/4
//
//  File        : \n4\pkg\MrServers\MrImaging\seq\a_se_15b130\a_se_b130_15000.cpp
//
//  Author      : Michael Zwanger, SIEMENS AG, Medical Engineering, MRIA, +9131/84-2672
//
//  Date        : 2011-09-27 15:54:27 +02:00
//
//  Lang        : cpp
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
//
//
//
/// \file   a_se_b130_15000.cpp
/// \brief  The spin echo sequence se_b130_15000
///
/// This file contains the implementation of the class Se15b130.
///
//  **************************************************************************



// ------------------------------------------------------------------------------
// Application includes
// ------------------------------------------------------------------------------
#include "MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000.h"
#include "MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000_UI.h"

// -------------
// Include files
// -------------
//#include "MrServers/MrMeasSrv/SeqIF/sde_allincludes.h"
#include "MrServers/MrImaging/seq/SystemProperties.h"                 // * Siemens system properties *
#include "MrServers/MrProtSrv/MrProtocol/libUICtrl/UICtrl.h"                      // support for UILink functionality
#include "MrServers/MrMeasSrv/MeasNuclei/Base/MeasNucleus.h" // for MeasNucleus

#include  "MrServers/MrMeasSrv/MeasUtils/MeasMath.h"                       // defines, macros, inline functions
#include  "MrServers/MrImaging/libSBB/libSBB.h"                   // fSBB... prototypes
#include  "MrServers/MrMeasSrv/SeqFW/libSSL/libSSL.h"                   // fSSL... prototypes
#include  "MrServers/MrImaging/libSeqUtil/libSeqUtil.h"           // fSU... prototypes

//Currently we do UT only on Host, but maybe in the future we'll do UT on Mars, then the code must be changed.
#ifdef WIN32
#include  "MrServers/MrImaging/ut/libsequt.h"                              // fSEQTest prototypes, symbols
#endif

#include "MrServers/MrProtSrv/MrProt/MrProt.h"
#include "ProtBasic/Interfaces/SeqLim.h"
#include "ProtBasic/Interfaces/MrPreparationPulses.h"

#include "MrServers/MrProtSrv/MrProt/KSpace/MrKSpace.h"
#include "MrServers/MrProtSrv/MrProt/MeasParameter/MrRXSpec.h"
#include "ProtBasic/Interfaces/MeasAqcDefs.h" //MeasAqcDefs



#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif

using namespace std;


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
#ifdef SEQUENCE_CLASS_SE15B130
SEQIF_DEFINE (SEQ_NAMESPACE::Se15b130)
#endif

using namespace SEQ_NAMESPACE;

Se15b130::Se15b130()
: m_lLinesToMeasure (0)
, m_pUI             (NULL)
, m_sSRF01     ("se15b130ex")
, m_sSRF01zSet ("sSRF01zSet")
, m_sSRF01zNeg ("sSRF01zNeg")
, m_sSRF02     ("se15b130ref")
, m_sSRF02zSet ("sSRF02zSet")
, m_sSRF02zNeg ("sSRF02zNeg")
, m_sGP_sp("sGP_sp")
, m_sGR_sp("sGR_sp")
, m_sGS_EX(RTEIDENT_PulseSS)
, m_sGR_de("sGR_de")
, m_sGS_re("sGS_re")
, m_sGS_RE(RTEIDENT_PulseSS)
, m_sGR_RO("sGS_RO")
, m_sTAB01(RTEIDENT_TabKy)
, m_sADC01     ("sADC01")
, m_sADC01zSet ("sADC01zSet")
, m_sADC01zNeg ("sADC01zNeg")
, m_mySBBList()
, m_CSatFat  (&m_mySBBList)
, m_CSatWat  (&m_mySBBList)
, m_MSat     (&m_mySBBList)
, m_lTRMin   (0)
, m_lTIMinAdd1(0)
, m_lTIMinAdd2(0)
, m_lScanTimeBasic(BASIC_SCANTIMEus)
, m_lScanTimeAllSats(0)
, m_lNegativeFillTime(0)
, m_lEchoLine(0)
{}






Se15b130::~Se15b130()
{

    //  ----------------------------------------------------------------------
    ///  Delete existing UI instance
    //  ----------------------------------------------------------------------
    if ( m_pUI )  {
        delete m_pUI;
        m_pUI = NULL;
    }

}


/*[ Function ****************************************************************\
*
* Name        : calculateTRTIFillTimes
*
* Description : Calculates TR and TI fill times, evaluates the result of
*               mySeqLoop.TrTiFillTimes in order to support fUICGetTILimits
*               and fUICSolveTITRConflict.
*
* Return      : bool
*
\****************************************************************************/
bool Se15b130::calculateTRTIFillTimes
(
 MrProt &rMrProt,
 SeqLim &rSeqLim,
 SeqExpo &rSeqExpo,
 long*    plNeededTI,
 long*    plNeededTR
 )
{
    bool bSuccess          = false;

    static const char *ptModule = {"calculateTRTIFillTimes"};


    bSuccess = m_mySeqLoop.TrTiFillTimes(rMrProt, rSeqLim, rSeqExpo,
        m_lTRMin,
        1,                // Number of TR fills per TR for one slice
        m_lTIMinAdd1,
        m_lTIMinAdd2,
        m_lScanTimeBasic,
        m_lScanTimeAllSats,
        &m_lNegativeFillTime);

#ifdef WIN32
    return fUICEvaluateSeqLoopTrTiFillTimes(rMrProt, rSeqLim, bSuccess, m_mySeqLoop.getlTIneeded(), m_mySeqLoop.getlTRneeded(), plNeededTI, plNeededTR);
#else
    if (bSuccess)  // if successful, plNeeded.. must be set even if fUICEvaluateSeqLoopTrTiFillTimes is not called
    {
        *plNeededTI=rMrProt.getalTI()[0];
        *plNeededTR=rMrProt.getalTR()[0];
    }
    return bSuccess;

#endif
}







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
NLSStatus Se15b130::initialize (SeqLim &rSeqLim)
{

    static const char *ptModule = {"Se15b130::initialize"};

    NLS_STATUS  lStatus = SEQU__NORMAL;


    //   ----------------------------------------------------------------------
    //
    //   Definition of sequence hard limits
    //
    //   ----------------------------------------------------------------------
    double      dMin, dMax, dInc, dDef;
    long        lMin, lMax, lInc, lDef;

    mPrintTrace0 (DEBUG_INIT, DEBUG_CALL,
        "() >>>> Welcome to USS Enterprise  (Build: "__DATE__"   "__TIME__")") ;
    if ( rSeqLim.getSeqDebugMode().getDebugMask() & DEBUG_INIT ) mPrintDebugMask ();

    /* -------------- */
    /* Initialization */
    /* -------------- */
    dMin = dMax = dInc = dDef = 0.;
    lMin = lMax = lInc = lDef = 0;


    //-----------------------------------------------------------------------------
    // Add RSats to the SBBList
    //-----------------------------------------------------------------------------
    for ( long lI=0; lI<MAXRSATS; lI++ )  {
        m_RSat[lI].addToSBBList (&m_mySBBList);
    }

    //-----------------------------------------------------------------------------
    // let me introduce myself...
    //-----------------------------------------------------------------------------
    rSeqLim.setMyOrigFilename ( __FILE__ );
    rSeqLim.setSequenceOwner  ( SEQ_OWNER_SIEMENS );
    rSeqLim.setSequenceHintText( (char *) "\n\
                                                Application: All-purpose sequence for T1-weighted imaging (TR < T1).          \n\
                                                Basics: 2D spin echo; single echo; multi-slice; symmetric echo sampling; \n\
                                                no gradient moment nulling.                                      \n\
Build: "__DATE__"   "__TIME__"\n")  ;
    // set DICOM sequence type:
    rSeqLim.setSequenceType                       (               SEQ::SEQUENCE_TYPE_TSE);
    rSeqLim.getSequenceType().setDisplayMode      (                          SEQ::DM_OFF);


    //-----------------------------------------------------------------------------
    // the system requirements: frequency, and gradient power
    //-----------------------------------------------------------------------------
    rSeqLim.setAllowedFrequency     (8000000, 500000000); // Hz
    rSeqLim.setRequiredGradAmpl     (10.0); //  mT/m
    rSeqLim.setRequiredGradSlewRate (10.0); // (mT/m)/ms
    rSeqLim.setGradients            (SEQ::GRAD_FAST) ;
    rSeqLim.getGradients().setDisplayMode (SEQ::DM_OFF);



    //-----------------------------------------------------------------------------
    // This sequence may be used with any nucleus, if the line below is used.
    // Note that for nuclei with a low Gamma (e.g. Na, C) this sequence will not
    //  work (i.e., even "insert sequence" fails) on systems with
    //  e.g. Ultra gradients, because the timing is hard-coded.
    //-----------------------------------------------------------------------------
    // rSeqLim.setSupportedNuclei(SEQ::NUCLEI_ALL);



    // -----------------------------
    // Base matrix size of the image  (min, max, mult, def)
    // -----------------------------
    rSeqLim.setBaseResolution (64, 512, SEQ::INC_64, 256);


    //-----------------------------------------------------------------------------
    // Phase encoding parameters
    //-----------------------------------------------------------------------------
    rSeqLim.setPhasePartialFourierFactor (SEQ::PF_OFF, SEQ::PF_7_8, SEQ::PF_6_8,
        SEQ::PF_5_8, SEQ::PF_HALF);
    rSeqLim.setPhaseOversampling         (0.0, 1.0, 0.01, 0.0);
    rSeqLim.setPELines                   (32, 1024, 1,
        rSeqLim.getBaseResolution().getDef());     // is the default

    /* ------------ */
    /* In-plane FOV */
    /* ------------ */
    dMax = SysProperties::getFoVMax();
    dInc =   1.0;
    dDef = 300.0;

    // The calculation of the min FOV is no longer necessary as it will be found
    // automatically by the binary search. Just write "dMin = 25".
    // Nevertheless it is done here just for demonstration.
    dMin =  65.000 * rSeqLim.getBaseResolution().getMin() / 256 ;
    dMin = fSDSdRoundUpMinimum(dMin, dMax, dInc);
    if (dDef < dMin) dDef = dMin;
    if (dDef > dMax) dDef = dMax;
    rSeqLim.setReadoutFOV ( dMin, dMax, dInc, dDef );

    dMin =  50.000 * (double) rSeqLim.getBaseResolution().getMin() / 256.;
    dMin = fSDSdRoundUpMinimum(dMin, dMax, dInc);
    if (dDef < dMin) dDef = dMin;
    if (dDef > dMax) dDef = dMax;
    rSeqLim.setPhaseFOV   ( dMin, dMax, dInc, dDef );


    /* ------------- */
    /* Set TR limits */
    /* ------------- */
    /* TR has to be a multiple of the min gradient switch time */
    lMin      = BASIC_SCANTIMEus ;
    lInc      = (lMin <= 10000) ? 10 : 100;
    lMin      = fSDSRoundToInc(lMin,lInc);
    lMax      = 5000000 ;
    lDef      = 300000 ;
    if (lDef < lMin) lDef = lMin;
    rSeqLim.setTR (0, lMin, lMax, lInc, lDef)  ;


    // --------------------------------------------------------------------------
    // Echo Time TE             (  No.,      min,      max,      inc,      def) ;
    // --------------------------------------------------------------------------
    rSeqLim.setTE			  ( 0  ,    15000,   100000,      100,     15000) ;
    rSeqLim.setContrasts            (        1,        1,        0,         1) ;


    //-------------------------------
    //  Set TI limits
    //-------------------------------
    rSeqLim.setInversion (SEQ::INVERSION_OFF, SEQ::SLICE_SELECTIVE);
    //rSeqLim.setSTIRMode  (SEQ::OFF, SEQ::ON);
    lMin = 8000 ; //SeqBuildBlockIRsel::SBBIRselGetDuration();
    lMin    = fSDSRoundToInc (lMin,1000);
    lDef    = (lMin <= 150000) ? 150000 : lMin;
    rSeqLim.setTI                          (    0,     lMin,  2000000,     1000,   150000 );

    // --------------------------------------------------------------------------------------
    // Slices                        (        min,          max,         inc,         def)  ;
    // --------------------------------------------------------------------------------------
    rSeqLim.setConcatenations       (           1,K_NO_SLI_MAX,           1,           1)  ;
    rSeqLim.setSlices               (           1,K_NO_SLI_MAX,           1,           1)  ;
    rSeqLim.setSliceThickness       (       2.000,      10.000,       0.500,       5.000)  ;
    rSeqLim.setSliceDistanceFactor  (       0.000,       8.000,       0.010,       0.200)  ;
    rSeqLim.setMultiSliceMode (SEQ::MSM_INTERLEAVED, SEQ::MSM_SEQUENTIAL);
    rSeqLim.setSliceSeriesMode (SEQ::INTERLEAVED, SEQ::ASCENDING, SEQ::DESCENDING );
    rSeqLim.enableSliceShift ();
    rSeqLim.enableMSMA ();
    rSeqLim.enableOffcenter ();
    rSeqLim.setAllowedSliceOrientation (SEQ::DOUBLE_OBLIQUE);

    /* ------------- */
    /* 3D-Partitions */
    /* ------------- */
    rSeqLim.setDimension (SEQ::DIM_2) ;
    // rSeqLim.setPartition (1,1,1,1) ;
    // rSeqLim.setPartitions   ( 8,  256,     1,    64 );    // this is the default
    // rSeqLim.setMinSliceResolution (0.5);  // this is the default
    // rSeqLim.setEllipticalScanning (ES_OFF) ;

    // --------------------------------------------------------------------------------------
    // Loop control
    // --------------------------------------------------------------------------------------
    rSeqLim.setTD               (        0,        0, 30000000,     1000,       0 )  ;
    rSeqLim.setAverages                   (        1,       32,        1,        1)  ;
    rSeqLim.setRepetitions                (        0,      512,        1,        0)  ;

    lInc = 100000;
    lMax = fSDSRoundToInc(2100000000-lInc+1,lInc);
    rSeqLim.setRepetitionsDelayTime        (        0,     lMax,     lInc,        0)  ;

    rSeqLim.setIntro                       (SEQ::ON, SEQ::OFF);
    rSeqLim.setAveragingMode               (SEQ::INNER_LOOP, SEQ::OUTER_LOOP);


    // --------------------------------------------------------------------------------------
    // Preparation Pulses                    (       min,       max,       inc,       def)  ;
    // --------------------------------------------------------------------------------------
    rSeqLim.setRSats                        (         0,         6,         1,         0)  ;
    rSeqLim.setRSatThickness                (     3.000,   150.000,     1.000,    50.000)  ;

    rSeqLim.setPSatMode	   ( SEQ::PSAT_NONE, SEQ::PSAT_SINGLE_REG, SEQ::PSAT_DOUBLE_REG)  ;
    rSeqLim.setPSatThickness                (     3.000,   150.000,     1.000,    50.000)  ;
    rSeqLim.setPSatGapToSlice               (     5.000,    50.000,     1.000,    10.000)  ;

    rSeqLim.setMTC                          (SEQ::OFF, SEQ::ON);
    //rSeqLim.setDarkBlood                    (SEQ::DB_OFF) ;

    if (SysProperties::getNominalB0() < LOW_FIELD_B0) {
        rSeqLim.setFatSuppression             (SEQ::FAT_SUPPRESSION_OFF);
        rSeqLim.getFatSuppression().setDisplayMode (SEQ::DM_OFF);
        rSeqLim.setWaterSuppression           (SEQ::WATER_SUPPRESSION_OFF);
        rSeqLim.getWaterSuppression().setDisplayMode (SEQ::DM_OFF);
    }
    else {
        rSeqLim.setFatSuppression             (SEQ::FAT_SUPPRESSION_OFF, SEQ::FAT_SATURATION);
        rSeqLim.setWaterSuppression           (SEQ::WATER_SUPPRESSION_OFF, SEQ::WATER_SATURATION);
        rSeqLim.setFatSatMode                 (SEQ::FAT_SAT_STRONG, SEQ::FAT_SAT_WEAK           );
    }


    // --------------------------------------------------------------------------------------
    // Physiologic measurements
    // --------------------------------------------------------------------------------------
    // rSeqLim.setPhases      ( 1, K_NO_SLI_MAX, 1, 1 );    is the default

    rSeqLim.addPhysioMode (SEQ::SIGNAL_CARDIAC,     SEQ::METHOD_TRIGGERING);
    rSeqLim.addPhysioMode (SEQ::SIGNAL_RESPIRATION, SEQ::METHOD_TRIGGERING);


    // --------------------------------------------------------------------------------------
    // RF                                        (     min,       max,      inc,      def)  ;
    // --------------------------------------------------------------------------------------
    rSeqLim.setFlipAngle                        (   50.000,  130.000,    1.000,   90.000)  ;
    rSeqLim.setExtSrfFilename                                     ("%MEASDAT%/extrf.dat")  ;
    //  rSeqLim.setRFSpoiling (RFS_OFF) ;


    //-----------------------------------------------------------------------------
    // ADC bandwidth (Hz/pixel)
    //-----------------------------------------------------------------------------
    rSeqLim.setBandWidthPerPixel      (0, 130, 130, 1, 130) ;


    /* -------------------------------- */
    /* Data Receive & Image calculation */
    /* -------------------------------- */
    rSeqLim.set2DInterpolation (SEQ::NO, SEQ::YES);
    rSeqLim.setReconstructionMode (SEQ::RECONMODE_MAGNITUDE, SEQ::RECONMODE_PHASE,
        SEQ::RECONMODE_REAL_PART, SEQ::RECONMODE_MAGN_PHASE,
        SEQ::RECONMODE_REAL_PHASE);
    m_mySeqLoop.initRegistryEntries() ;


    // * ------------------------------------------------------------------------------------ *
    // * Database control                                                                     *
    // * ------------------------------------------------------------------------------------ *
    rSeqLim.setMultipleSeriesMode (SEQ::MULTIPLE_SERIES_EACH_MEASUREMENT, SEQ::MULTIPLE_SERIES_OFF, SEQ::MULTIPLE_SERIES_EACH_SLICE, SEQ::MULTIPLE_SERIES_EACH_SLICE_AND_MEASUREMENT);



    //-----------------------------------------------------------------------------
    // Adjustment parameter
    //-----------------------------------------------------------------------------
    rSeqLim.setAdjShim (SEQ::ADJSHIM_TUNEUP, SEQ::ADJSHIM_STANDARD);



    // -------------------------
    // Set SAT pulse identifiers
    // -------------------------
    {
        char    ptIdentdummy[7] ;
        int lI ;
        for (lI = 0; lI < MAXRSATS; lI++) {
            sprintf(ptIdentdummy, "SE%1d", lI+1);
            m_RSat[lI].setIdent(ptIdentdummy);
        }
        m_CSatFat.setIdent("SEFS");
        m_CSatWat.setIdent("SEWS");
        m_MSat.setIdent("SEMS");
    }


#if OSCBIT_IN_KERNEL
    // Prepare the osc bit
    m_OscBit.setCode( SYNCCODE_OSC0 );
    m_OscBit.setIdent (RTEIDENT_Osc0) ;
    m_OscBit.setDuration (10);

    // The Osc Bits is not performed by SeqLoop, but within the kernel itself
    m_mySeqLoop.setPerformOscBit ( false );
#endif

    // ----------------------
    // Configure UI behaviour
    // ----------------------
    // Do not display the following UI parameters:
    rSeqLim.getEPIFactor().setDisplayMode  (SEQ::DM_OFF);

#ifdef WIN32
    /* ------------ */
    /* Sequence UT  */
    /* ------------ */
    if ( mIsUnittestActive() ) {
    /*    SeqUT.DisableTestCase (lGsFirstMomentNotRephasedErr, RTEB_ORIGIN_fSEQRunKernel,
    "No intention to rephase momentum 1st order ") ;
    SeqUT.DisableTestCase (lGrFirstMomentNotRephasedErr, RTEB_ORIGIN_fSEQRunKernel,
    "No intention to rephase momentum 1st order ") ;
        */
        //    SeqUT.PrintExceptionList (stdout);
    }
#endif

    //file containing the default postprocessing protocol (EVAProtocol)
#ifdef WIN32
    rSeqLim.setDefaultEVAProt (_T("%SiemensEvaDefProt%\\Inline\\Inline.evp"));
#endif

    // ----------------------------------------------------------------------------------
    // advise SeqLoop to calculate required TR TI fill times, if actual values are not OK
    //-----------------------------------------------------------------------------------
    m_mySeqLoop.setbHandleTRTIConflict(true);

    //  ----------------------------------------------------------------------
    //  Instantiate of UI class
    //  ----------------------------------------------------------------------
    if ( (NLS_SEV & (lStatus = createUI (rSeqLim))) == NLS_SEV )  {
        TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: Instantiation of UI class failed: Gre::createUI(SeqLim*)", ptModule);
        return ( lStatus );
    }

    //  ----------------------------------------------------------------------
    //  Declaration of pointer to UI parameter classes
    //  ----------------------------------------------------------------------
    lStatus = m_pUI->registerUI (rSeqLim);

    if ( NLS_SEVERITY(lStatus) != NLS_SUCCESS )  {
        TRACE_PUT2(TC_INFO, TF_SEQ, "%s : Initialization of UI failed : 0x%lx"  , ptModule, lStatus);
        return ( lStatus );
    }


    return (lStatus);

}




//  --------------------------------------------------------------------------
//
//  Name        :  Se15b130::prepare
//
//  Description :
/// \brief         Preparation of the sequence during binary search and prior
///                 to sequence execution
//
//  Return      :  NLS status
//
//  --------------------------------------------------------------------------
NLSStatus Se15b130::prepare (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo)
{

    static const char *ptModule = {"Se15b130::prepare"};

    NLS_STATUS   lStatus = SEQU__NORMAL;                     // * Return status *

    MrProt mrProt(rMrProt);
    MrKSpace& mrKSpace = mrProt.kSpace();


    //   ----------------------------------------------------------------------
    //
    //   Preparation of sequence
    //
    //   ----------------------------------------------------------------------
    long         lEchoLine             = mrKSpace.echoLine();   // Number of line in K-Space center
    long         lPreparingScans       = 0;                      // Number of preparing scans
    double       dRfEnergyInSBBs       = 0.;                     /* RF energy in SBB calls    */
    double       dRfEnergyInSRFs       = 0.;                     /* RF energy in SRF          */
    long	       lKernelRequestsPerMeasurement = 0;
    double       lEffSatRepTime        = 0;
    long         lNeededTI             = 0;                        //TI needed to execute measurement
    long         lNeededTR             = 0;                        //TR needed to execute measurement



    long         lI                    = 0;                      /* Helper variables          */

    /// These Vars are now global
    //  long         m_lTRMin                = 0;                      /* Minimum TI                */
    //  long         m_lTIMinAdd1            = 0;                      /* Minimum TI additional     */
    //  long         m_lTIMinAdd2            = 0;                      /* Minimum TI additional     */
    //  const long   m_lScanTimeBasic        = BASIC_SCANTIMEus;
    //  long         m_lScanTimeAllSats      = 0;
    //  long         m_lNegativeFillTime     = 0;                      /* Binary search flag        */

    // ---------------------------------------------------------------
    // get the first nucleus from the protocol
    // ---------------------------------------------------------------
    MeasNucleus myMeasNucleus(rMrProt.getsTXSPEC().getasNucleusInfo()[0].gettNucleus().c_str());

    int32_t lLinesToMeasure = 0 ;

    lStatus = mrKSpace.linesToMeasure(lLinesToMeasure);
    CheckStatusPB(lStatus,"linesToMeasure");

    mPrintTrace0 (DEBUG_PREP, DEBUG_CALL, "() >>>>") ;
    mPrintTrace2 (DEBUG_PREP, DEBUG_LANDMARK, "LinesToMeasure=%ld, EchoLine=%ld",
        lLinesToMeasure, lEchoLine) ;


    // ---------------------------------------------------------------
    // Combination of partial Fourier < 6/8 and RealPartImages
    // are not allowed (charm 274463)
    // ---------------------------------------------------------------
    if( ((rMrProt.getsKSpace().getucPhasePartialFourier() == SEQ::PF_HALF) ||
        (rMrProt.getsKSpace().getucPhasePartialFourier() == SEQ::PF_5_8 )) &&
        (mrProt.calcRealPartImages() )
        )
        return SEQU_ERROR ;
    // Real part images are futhermore only allowed if IR is selected.
    // But this is already handled by the UILink layer.



    // ---------------------------------------------------------------
    // Prepare SeqLoop
    // ---------------------------------------------------------------
    m_mySeqLoop.setbSpoilGradAfterKernel (true);
    m_mySeqLoop.setlConcMode (Concatenations_NORMAL) ;	// default
    m_mySeqLoop.setdDistFacMinIfConcYes (0.0) ;         // default
    //  m_mySeqLoop.setbPreparingScans (true) ;			// default

    if( mrProt.calcRealPartImages() ) {
        // Perform a one-line phase correction scan (without IR):
        m_mySeqLoop.setPhaseCorScans  (true) ;
        m_mySeqLoop.setlPreparingTime (0);
    }
    else {
        // No phase correction for realpart images:
        m_mySeqLoop.setPhaseCorScans  (false) ;
        m_mySeqLoop.setlPreparingTime (2000000 /* µs */);
    }

    m_mySeqLoop.setpCppSequence(this);

    if (!m_mySeqLoop.prep (rMrProt, rSeqLim, rSeqExpo))
        return(m_mySeqLoop.getNLSStatus());

    /* ---------------------------------------------- */
    /* Get the number of requests of each RF pulse */
    /* ---------------------------------------------- */
    lKernelRequestsPerMeasurement = m_mySeqLoop.getKernelRequestsPerMeasurement (rSeqLim);



    // ----------------------
    // configure the Sat SBBs
    // ----------------------
    lEffSatRepTime = double(rMrProt.getalTR()[0] / rMrProt.getsSliceArray().getlSize())
        * rMrProt.getsSliceArray().getlConc();

    for (lI=0; lI<MAXRSATS; lI++) {
        m_RSat[lI].setRequestsPerMeasurement (lKernelRequestsPerMeasurement);
        m_RSat[lI].adaptFlipAngle (lEffSatRepTime, rSeqLim);
    }

    m_CSatFat.setRequestsPerMeasurement (lKernelRequestsPerMeasurement);
    m_CSatFat.adaptFlipAngle (lEffSatRepTime, rSeqLim, rMrProt);
    m_CSatFat.setCSatMode (SBBCSatCode_Fat);

    m_CSatWat.setRequestsPerMeasurement (lKernelRequestsPerMeasurement);
    m_CSatWat.adaptFlipAngle (lEffSatRepTime, rSeqLim, rMrProt);
    m_CSatWat.setCSatMode (SBBCSatCode_Water);

    m_MSat.setRequestsPerMeasurement (lKernelRequestsPerMeasurement);
    m_MSat.setFrequencyOffset (MSATOFFSETFREQHz);
    m_MSat.setEffTrTime(((double) rMrProt.getalTR()[0] / rMrProt.getsSliceArray().getlSize()) * rMrProt.getsSliceArray().getlConc() / 1000.0 );
    // MSat expects Milliseconds ;-)


    // -------------------------------------------
    // Prepare all SBBs derived from SeqBuildBlock
    // -------------------------------------------

    if (!m_mySBBList.prepSBBAll (rMrProt, rSeqLim, rSeqExpo, &dRfEnergyInSBBs))
        return(m_mySBBList.getpSBBLastPrep()->getNLSStatus()) ;

    /* Add energy from SBBIR function to dRfEnergyInSBBs if selected */
    /* For each SRF01 pulse one IR pulse is sent. Therefore number   */
    /* of requests are the same.                                     */
    dRfEnergyInSBBs  += m_mySeqLoop.getRfEnergyRecovery() * lKernelRequestsPerMeasurement
        * (rMrProt.getlRepetitions()+1) ;

    //-----------------------------------------------------------
    // Calculate Sat time
    //-----------------------------------------------------------
    m_lScanTimeAllSats = 0;

    for (lI=0;lI<MAXRSATS;lI++)
        m_lScanTimeAllSats  +=  m_RSat[lI].getDurationPerRequest();
    m_lScanTimeAllSats  +=  m_CSatFat.getDurationPerRequest();
    m_lScanTimeAllSats  +=  m_CSatWat.getDurationPerRequest();
    m_lScanTimeAllSats  +=  m_MSat.getDurationPerRequest();


    /* --------------------------- */
    /* Calculate the TE fill times */
    /* --------------------------- */
    lStatus = fSBBTeFillTimesStd(rMrProt,rSeqLim,rSeqExpo,m_alTEFil,&m_lNegativeFillTime);
    CheckStatusB(lStatus);


    /* --------------------------------------------------- */
    /* Calculate the minimum TR value and the TR fill time */
    /* --------------------------------------------------- */
    // m_lTIMinAdd1 -> additional scan time for SBBs inside TI, when TI is not interleaved
    // m_lTIMinAdd2 -> additional scan time for SBBs inside TI, when TI is interleaved

    m_lTRMin     = (m_lScanTimeBasic + 2*m_alTEFil[0] + m_lScanTimeAllSats);

    if (rMrProt.preparationPulses().getucInversion() != SEQ::INVERSION_OFF) {
        m_lTIMinAdd1 = m_lScanTimeAllSats;
        m_lTIMinAdd2 = m_lScanTimeAllSats;
    }
    else
        m_lTIMinAdd1 = m_lTIMinAdd2 = 0;


    if (! calculateTRTIFillTimes(rMrProt,rSeqLim,rSeqExpo,&lNeededTI,&lNeededTR)) return m_mySeqLoop.getNLSStatus();


    /* ----------------------------------- */
    /* Calculate the minimum permitted FOV */
    /* ----------------------------------- */
    //this is no longer needed, because we check
    //the RO and PE gradient pulses for GPA-specs

    /* ------------------------------- */
    /* Prepare the RF pulse structures */
    /* ------------------------------- */
    m_sSRF01.setTypeExcitation    ();
    m_sSRF01.setDuration          (2048);                       // specified min. ratio of slice select gradients (refoc/excit.) is 1.2
    m_sSRF01.setFlipAngle         (rMrProt.getadFlipAngleDegree()[0]);
    m_sSRF01.setInitialPhase      (90);
    m_sSRF01.setFamilyName        ("SE2560A90.SE90_12A2_2") ;
    m_sSRF01.setThickness         (rMrProt.getsSliceArray().getasSlice()[0].getdThickness() ) ;

    m_sSRF02.setTypeRefocussing   ();
    m_sSRF02.setDuration          (2560);                       // specified min. ratio of slice select gradients (refoc/excit.) is 1.2
    m_sSRF02.setFlipAngle         (180);
    m_sSRF02.setInitialPhase      (0.0);
    m_sSRF02.setFamilyName        ("SE2560A180.SE180_12A2_2") ;
    m_sSRF02.setThickness         (rMrProt.getsSliceArray().getasSlice()[0].getdThickness());

    if(! m_sSRF01.prepExternal(rMrProt,rSeqExpo) )   return (m_sSRF01.getNLSStatus());
    if(! m_sSRF02.prepExternal(rMrProt,rSeqExpo) )   return (m_sSRF02.getNLSStatus());

    dRfEnergyInSRFs  = lKernelRequestsPerMeasurement * m_sSRF01.getPulseEnergyWs() * (rMrProt.getlRepetitions()+1) ;
    dRfEnergyInSRFs += lKernelRequestsPerMeasurement * m_sSRF02.getPulseEnergyWs() * (rMrProt.getlRepetitions()+1) ;

    /* ------------------------------ */
    /* Prepare the readout structures */
    /* ------------------------------ */
    m_sADC01.setColumns (rMrProt.getsKSpace().getlBaseResolution());
    // m_sADC01.setDwellTime (READOUTTIMEus * 1000 / rMrProt.getsKSpace().getlBaseResolution());
    m_sADC01.setDwellTime (static_cast<long>(mrProt.rxSpec().effDwellTime( rSeqLim.getReadoutOSFactor() )[0]));

    m_sADC01.getMDH().setKSpaceCentreColumn ( int(0.5 * rMrProt.getsKSpace().getlBaseResolution()) );

    /* ---------------------------------- */
    /* Prepare the readout gradient pulse */
    /* ---------------------------------- */
    m_sGR_RO.setDuration  (9180) ;                                             /*! EGA-03; EGA-01 !*/
    m_sGR_RO.setRampTimes (1000) ;
    m_sGR_RO.setAmplitude (1.0E9 / (myMeasNucleus.getLarmorConst()             /*! EGA-03; EGA-01 !*/
        * rMrProt.getsSliceArray().getasSlice()[0].getdReadoutFOV()
        * 2 * rMrProt.getsRXSPEC().getalDwellTime()[0]) ) ;     /* Factor 2 due to oversampling */
    if (mIsDebugBitMask (DEBUG_PREP|DEBUG_LANDMARK))
        cout << "GR_RO:   Ampl=" << m_sGR_RO.getAmplitude()
        << "    Dur=" <<  m_sGR_RO.getDuration()
        << "     Ramp=" << m_sGR_RO.getRampUpTime() << endl ;
    if (! m_sGR_RO.prep() ) return (m_sGR_RO.getNLSStatus());
    if (! m_sGR_RO.check() ) return (m_sGR_RO.getNLSStatus());


    /* --------------------------------------------------------------------- */
    /* Prepare the "table" gradient and check the maximum gradient strengths */
    /* --------------------------------------------------------------------- */
    if(! m_sTAB01.prepPE(1000, 3940, 1000, rMrProt, SEQ::DIR_DESCENDING, 0.0,
        lLinesToMeasure - lEchoLine -1)  )  /*! EGA-03; EGA-01 !*/
        return(m_sTAB01.getNLSStatus());
    if(! m_sTAB01.check()                                                        )
        return(m_sTAB01.getNLSStatus());

    if(! m_sTAB01.prepPE(1000, 3940, 1000, rMrProt, SEQ::DIR_DESCENDING, 0.0, - lEchoLine) )  /*! EGA-03; EGA-01 !*/
        return(m_sTAB01.getNLSStatus());
    if(! m_sTAB01.check() )
        return(m_sTAB01.getNLSStatus());


    /* ------------------------------------- */
    /* Prepare the gradient pulse structures */
    /* ------------------------------------- */
    /*--------------------------------------------------------------------------------- */
    /*                      SPOILER IN PC-DIRECTION                                     */
    /* There are two possibilities to prepare the spoiler in phase encoding direction:  */
    /* Either you use the fSUCalcPESpoilAmpl function where you pass the minimum readout*/
    /* time and the scale value from the preparation function of NUMARIS3.5             */
    /* or you divide this scale factor e.g. by the value 2.21617 for the se_90rb56_160rb56*/
    /* sequence located in the NUMARIS3.5 at dTAB01Fst =  2.21617 * dGradAmplGP         */
    /* and use the m_sTAB01.getAmplitude() function. See se_90rb60_160rb56 :              */
    /* m_sTAB01.getAmplitude()* 1.92349 <-- (1.923492 = 4.265960/2.21617).                */
    /*                                                                                  */
    /* The disadvantage of the second way is that it demands that m_sTAB01.prepPE with    */
    /* -rMrProt.getsKSpace().echoLine() is called at the end. Because only then the        */
    /* amplitude of the PC-Table is correct. Otherwise it is one line step smaler and   */
    /* then the amplitude of the spoiler is too small too.                              */
    /*--------------------------------------------------------------------------------- */

    // Spoiler gradient in phase-encode direction
    // Our value = old value / PE ref value = 1.228430 / 0.974620 = 1.26041945
    if (! m_sGP_sp.prepAmplitude(1000, 5000, 1000, m_sTAB01.getAmplitude() * 1.260419) )
        return(m_sGP_sp.getNLSStatus());
    if (! m_sGP_sp.check() )
        return (m_sGP_sp.getNLSStatus());

    // Spoiler gradient in readout direction
    if (! m_sGR_sp.prepAmplitude(1000, 5000, 1000, m_sGR_RO.getAmplitude() * 1.260419) )
        return(m_sGR_sp.getNLSStatus());
    if (! m_sGR_sp.check() )
        return (m_sGR_sp.getNLSStatus());

    //slice-selective gradient for  90 deg excitation pulse
    if (! m_sGS_EX.prepAmplitude(1000, 8560, 1000, m_sSRF01.getGSAmplitude()         ) ) /*! EGA-04; EGA-02 !*/
        return(m_sGS_EX.getNLSStatus());
    if (! m_sGS_EX.check() )
        return (m_sGS_EX.getNLSStatus());

    if (! m_sGR_de.prepAmplitude(1000, 3940, 1000, m_sGR_RO.getAmplitude() * 1.228426) )
        return(m_sGR_de.getNLSStatus());
    if (! m_sGR_de.check() )
        return (m_sGR_de.getNLSStatus());

    // ... or we can do it this way:
    m_sGS_re.setDuration (1940);
    m_sGS_re.setRampTimes (1000);
    //m_sGS_re.setAmplitude (m_sSRF01.getGSAmplitude() * -0.4639175);
    m_sGS_re.setAmplitude ( (-m_sSRF01.getGSAmplitude() * 1780  +  m_sSRF02.getGSAmplitude() * 880) / 1940 );
    if (! m_sGS_re.prep() ) return (m_sGS_re.getNLSStatus());
    if (! m_sGS_re.check() ) return (m_sGS_re.getNLSStatus());

    if (! m_sGS_RE.prepAmplitude (1000, 4440, 1000, m_sSRF02.getGSAmplitude()           ) ) /*! EGA-04; EGA-02 !*/
        return (m_sGS_RE.getNLSStatus());
    if (! m_sGS_RE.check() ) return (m_sGS_RE.getNLSStatus());


    /* --------------------- */
    /* Set the receiver gain */
    /* --------------------- */
    if (rMrProt.getsSliceArray().getasSlice()[0].getdThickness() <= 10.0 /* mm */) {
        lStatus = fSSLSetRxGain (K_RX_GAIN_CODE_HIGH, rMrProt, rSeqLim);
        CheckStatusPB (lStatus, "fSSLSetRxGain") ;
    }
    else {
        lStatus = fSSLSetRxGain (K_RX_GAIN_CODE_LOW, rMrProt, rSeqLim);
        CheckStatusPB (lStatus, "fSSLSetRxGain") ;
    }


    // ---------------------------------------------------------------------------
    // Calculate the rotation matrices and offsets for slices
    // ---------------------------------------------------------------------------
    CheckStatusPB (lStatus = fSUPrepSlicePosArray (rMrProt, rSeqLim, m_asSLC),
        "fSUPrepSlicePosArray") ;


    /* -------------------------------- */
    /* Export parameters to ICE program */
    /* -------------------------------- */

    rSeqExpo.setICEProgramFilename ( "%SiemensIceProgs%\\IceProgram2D");

    if (rMrProt.getsPrepPulses().getucInversion() == SEQ::INVERSION_OFF)
        fSUSetSequenceString ("se", rMrProt, rSeqExpo);
    else
        fSUSetSequenceString ("ir", rMrProt, rSeqExpo);


    // * ---------------------------------------------------------------------- *
    // * Set echo train length (-> used to set corresponding DICOM attributes)  *
    // * ---------------------------------------------------------------------- *
    rSeqExpo.setEchoTrainLength         (static_cast<int32_t>(1) );
    rSeqExpo.setGradientEchoTrainLength (static_cast<int16_t>(0) );
    rSeqExpo.setRFEchoTrainLength       (static_cast<int16_t>(1) );



    // reset pSeqExpo buffer (might still contain multiple energy blocks from previous call of fSeqPrep() in binary search !!!)
    rSeqExpo.resetAllRFBlockInfos();     // note: return status MUST NOT be checked in this case: will return false, if no energy blocks available (that's the general case at this point!)


    rSeqExpo.setMeasureTimeUsec             (m_mySeqLoop.getMeasurementTimeUsec (rMrProt, rSeqLim));
    rSeqExpo.setTotalMeasureTimeUsec        (m_mySeqLoop.getTotalMeasTimeUsec   (rMrProt, rSeqLim));
    rSeqExpo.setRelevantReadoutsForMeasTime (m_mySeqLoop.getNumberOfRelevantADCs());

    //  cout << "dRfEnergyInSRFs=" << dRfEnergyInSRFs << "    dRfEnergyInSBBs=" << dRfEnergyInSBBs << endl ;
    rSeqExpo.setRFEnergyInSequence_Ws  (dRfEnergyInSRFs + dRfEnergyInSBBs );


    //
    // improved support of SAR-lookahead:
    //
#ifdef WIN32
    if ( rSeqLim.isContextNormal() )
    {
        // export Intro (TokTokTok) as separate block without RF
        if( rMrProt.intro() )
        {
            double dTotalMeasTime_s = static_cast<double>(rSeqExpo.getTotalMeasureTimeUsec()) * 1e-06;      // convert from [us] to [s]
            double dIntroTime_s     = static_cast<double>(m_mySeqLoop.getTokTokTokTime())     * 1e-06;      // convert from [us] to [s]

            // first, reset all blocks
            // note: this also deletes the previous pSeqExpo->setRFEnergyInSequence_Ws()
            //       (setRFEnergyInSequence_Ws() is equivalent to specifing one single block)
            if (! rSeqExpo.resetAllRFBlockInfos() )
            {
                TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: rSeqExpo.resetAllRFBlockInfos()  failed", ptModule);
                return SEQU_ERROR;
            }

            // first block: Intro without energy
            if ( ! rSeqExpo.addRFBlockInfo (dIntroTime_s,
                                            SeqExpoRFBlockInfo::VALUETYPE_ACTUAL,
                                            rMrProt.getsTXSPEC().getasNucleusInfo()[0].gettNucleus().c_str(),
                                            0.0,                    // Energy
                                            SeqExpoRFBlockInfo::VALUETYPE_ACTUAL) )
            {
                TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: rSeqExpo.addRFBlockInfo 1 failed  ", ptModule);
                return SEQU_ERROR;
            }

            // second block: remaining time containing total energy
            if ( ! rSeqExpo.addRFBlockInfo (dTotalMeasTime_s - dIntroTime_s,
                                            SeqExpoRFBlockInfo::VALUETYPE_ACTUAL,
                                            rMrProt.getsTXSPEC().getasNucleusInfo()[0].gettNucleus().c_str(),
                                            dRfEnergyInSRFs + dRfEnergyInSBBs,
                                            SeqExpoRFBlockInfo::VALUETYPE_ACTUAL) )
            {
                TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: rSeqExpo.addRFBlockInfo 2 failed  ", ptModule);
                return SEQU_ERROR;
            }
        }
    }
#endif  //win32


    rSeqExpo.setMeasuredPELines        (lLinesToMeasure);
    // (How many columns the sequence measures. This does not include oversampling.)
    rSeqExpo.setMeasured3dPartitions   (1);

    if( mrProt.calcRealPartImages() ) {
        rSeqExpo.setPhaseCorScans             (rMrProt.getlContrasts());
        rSeqExpo.setOnlinePhaseCorrectionAlgo (ICE_ONLINEPC_AUTOCORR|ICE_ONLINEPC_CROSSCORR|ICE_ONLINEPC_FILTERED); // CHARM 280286
    }
    else {
        rSeqExpo.setOnlinePhaseCorrectionAlgo (0);
        rSeqExpo.setPhaseCorScans             (0);
    }

    rSeqExpo.setPreScans               (lPreparingScans);

    // Set phase correction algorithm:
    // Real:                    Extra phase correction scan
    // Phase:                   Submatrix
    // else if partial Fourier: Margosian

    if ( mrProt.calcRealPartImages() )  {
        // Note that we have an extra phase correction scan in this case
        // Note that RealPartImages is not allowed with pF=PF_HALF,PF_5_8
        rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);
    }
    else if ( mrProt.calcPhaseImages() )  {
        rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_SUBMATRIX);

        rSeqExpo.setNoOfPhaseCorrLines   (  16 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setLinSlopeLength       (  16 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setNoOfPhaseCorrColumns ( 128 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setColSlopeLength       ( 128 * rMrProt.getsKSpace().getlBaseResolution() / 256);
    }
    else if ( rMrProt.getsKSpace().getucPhasePartialFourier() != SEQ::PF_OFF )  {
        rSeqExpo.setPCAlgorithm         (SEQ::PC_ALGORITHM_MARGOSIAN);

        rSeqExpo.setNoOfPhaseCorrLines   (  16 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setLinSlopeLength       (  16 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setNoOfPhaseCorrColumns ( 128 * rMrProt.getsKSpace().getlBaseResolution() / 256);
        rSeqExpo.setColSlopeLength       ( 128 * rMrProt.getsKSpace().getlBaseResolution() / 256);
    }
    else  {
        rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);
    }


    /*
    if ( rMrProt.getsKSpace().getucPhasePartialFourier() == SEQ::PF_HALF ||
    rMrProt.getsKSpace().getucPhasePartialFourier() == SEQ::PF_5_8  )
    // Note that RealPart images are not allowed in this mode
    rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_MARGOSIAN);

      else if ( mrProt.calcRealPartImages() )
      // Note that we have an extra phase correction scan in this case
      rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);

        else if ( rMrProt.calcPhaseImages() )
        rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_SUBMATRIX);

          else if ( rMrProt.getsKSpace().getucPhasePartialFourier() != SEQ::PF_OFF )
          rSeqExpo.setPCAlgorithm         (SEQ::PC_ALGORITHM_SUBMATRIX);

            else
            rSeqExpo.setPCAlgorithm          (SEQ::PC_ALGORITHM_NONE);
    */


    // Set IceProgName and OnlineFFT mode at the end of SeqPrep, because it needs valid exports
    if (!m_mySeqLoop.setIceProgram(rMrProt, rSeqLim, rSeqExpo,
        m_mySeqLoop.getDataRateMBytePerSec(rMrProt, rSeqLim, rMrProt.getlContrasts())))
        return m_mySeqLoop.getNLSStatus();


    //-------------------------------------------------------------------------------------
    // now we are sure the sequence will run: do we have the correct TR and TI values ???
    //-------------------------------------------------------------------------------------
    if (lNeededTR!=rMrProt.getalTR()[0] || lNeededTI!=rMrProt.getalTI()[0])
    {
        return SEQU_ERROR;
    }


    // set application card for postprocessing
    //
#ifdef WIN32
    rSeqExpo.setApplicationCard(SEQ::APPLICATION_CARD_INLINE);
#endif


    /* --------------------------- */
    /* End of sequence preparation */
    /* --------------------------- */
    return (lStatus) ;

FINISHED:
    rSeqExpo.setNegativeFillTime (m_lNegativeFillTime) ;

    return (lStatus);

}



//  --------------------------------------------------------------------------
//
//  Name        :  Se15b130::check
//
//  Description :
/// \brief         Check of the sequence for gradient stimulation
///
///                This method is called by the framework prior to a
///                 measurement on the host to ensure, that
///                 - no gradient overflow occurs
///                 - the stimulation will not exceed the threshold
///
//  Return      :  NLS status
//
//  --------------------------------------------------------------------------
NLSStatus Se15b130::check (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, SEQCheckMode *  pSEQCheckMode)
{

    static const char *ptModule = {"Se15b130::check"};
    NLS_STATUS   lStatus = SEQU__NORMAL;


    //  ----------------------------------------------------------------------
    //
    //  Sequence check:
    //     - max. gradient amplitude
    //     - GSWD look ahead
    //
    //  ----------------------------------------------------------------------
    mPrintTrace0 (DEBUG_CHECK, DEBUG_CALL, "() >>>>") ;


    int32_t lLinesToMeasure = 0 ;

    MrKSpace mrKSpace(rMrProt);

    lStatus = mrKSpace.linesToMeasure(lLinesToMeasure);
    CheckStatusPB(lStatus,"linesToMeasure");



    /* -------------------------- */
    /* Set the looping parameters */
    /* -------------------------- */
    //psLOOP->lRFSpoiling = FALSE;

    m_mySeqLoop.setlinesToCheck        (4);
    m_mySeqLoop.setlineNoToCheck       (0, 0);
    m_mySeqLoop.setlineNoToCheck       (1, 1);
    m_mySeqLoop.setlineNoToCheck       (2, lLinesToMeasure - 2);
    m_mySeqLoop.setlineNoToCheck       (3, lLinesToMeasure - 1);

    m_mySeqLoop.setpartitionsToCheck   (1);
    m_mySeqLoop.setparNoToCheck        (0, 0);


    /* ----------------------- */
    /* Execute the check loops */
    /* ----------------------- */
    if(! m_mySeqLoop.check ( NULL,
        rMrProt,
        rSeqLim,
        rSeqExpo,
        m_asSLC,
        &m_sADC01)
        ) lStatus=m_mySeqLoop.getNLSStatus();
    CheckStatusPB(lStatus,"SeqLoop::check");

FINISHED:
    mPrintTrace0 (DEBUG_CHECK, DEBUG_RETURN, "() <<<<") ;

    return(lStatus);
}



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
NLSStatus Se15b130::run (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo)
{

    static const char *ptModule = {"Se15b130::run"};
    NLS_STATUS lStatus          = SEQU__NORMAL;

    MrKSpace mrKSpace(rMrProt);

    //  ----------------------------------------------------------------------
    //
    //  Execution of seuqence timing
    //
    //  ----------------------------------------------------------------------
    mPrintTrace1 (DEBUG_RUN, DEBUG_CALL, "() <%s> started", rSeqLim.getLinkedSeqFilename() ) ;

    /* ---------------------------------------- */
    /* Initialization of the unit test function */
    /* ---------------------------------------- */
    mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunStart,0,0,0,0,0); /*! EGA-All !*/

    /* --------------------------------------- */
    /* Set the line independent MDH parameters */
    /* --------------------------------------- */
    m_sADC01.getMDH().setKSpaceCentreLineNo    ((unsigned short)mrKSpace.echoLine());

    /* ----------------------------- */
    /* Execute the measurement loops */
    /* ----------------------------- */
    if(! m_mySeqLoop.run( NULL, //(NLS_STATUS (*)(TYPESFor_fSEQRunKernel))fSEQRunKernel,
        rMrProt,
        rSeqLim,
        rSeqExpo,
        m_asSLC,
        &m_sADC01)
        ) lStatus=m_mySeqLoop.getNLSStatus();
    CheckStatusPB(lStatus,"SeqLoop::run");

FINISHED:

    mSEQTest(rMrProt,rSeqLim,rSeqExpo,RTEB_ORIGIN_fSEQRunFinish,0,0,0,0,0); /*! EGA-All !*/
    mPrintTrace1 (DEBUG_RUN, DEBUG_CALL | DEBUG_RETURN, "() <%s> finished",
        rSeqLim.getLinkedSeqFilename() ) ;

    return(lStatus);
}




//   --------------------------------------------------------------------------
//
//   Name        :  Se15b130::runKernel
//
//   Description :
///                 Executes the basic timing of the real-time sequence.
//
//   Return      :  NLS status
//
//   --------------------------------------------------------------------------
NLS_STATUS Se15b130::runKernel(MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, long lKernelMode, long lSlice, long lPartition, long lLine)
{

    static const char *ptModule = {"Se15b130::runl"};
    NLS_STATUS         lStatus  = SEQU__NORMAL;


    //  ----------------------------------------------------------------------
    //
    //  Excecution of sequence kernel
    //
    //  ----------------------------------------------------------------------
    MrKSpace mrKSpace(rMrProt);
    unsigned long      ulTestIdent      = 0 ;
    long               lT               = 0 ;
    long               lI ;

    m_lEchoLine        = (long) mrKSpace.echoLine(); // Number of line in K-Space center

    lStatus = mrKSpace.linesToMeasure(m_lLinesToMeasure);
    CheckStatusPB(lStatus,"linesToMeasure");

    mPrintTrace2 (DEBUG_RUN, DEBUG_CALL, "(slice=%ld, line=%ld) ",
        m_asSLC[lSlice].getSliceIndex(), lLine  ) ;


    /* ------------------------------------ */
    /* Necessary for the sequence unit test */
    /* ------------------------------------ */
    if (lKernelMode == KERNEL_CHECK)
        ulTestIdent = RTEB_ORIGIN_fSEQCheck;
    else
        ulTestIdent = RTEB_ORIGIN_fSEQRunKernel;


    /* ---------------------------------------- */
    /* Calculate the ky phase-encoding value(s) */
    /* ---------------------------------------- */
    if (lKernelMode == KERNEL_PHASECOR) {
        m_sTAB01.setAmplitude (0.0);
        if (! m_sTAB01.prep())
            return m_sTAB01.getNLSStatus();
    }
    else {
        if(! m_sTAB01.prepPE(rMrProt, lLine-m_lEchoLine) )
            return m_sTAB01.getNLSStatus();
    }


    /* ------------------------------------- */
    /* Set the line dependent MDH parameters */
    /* ------------------------------------- */
    m_sADC01.getMDH().setCseg (0);   // to link phase corr scans with imaging scans
    m_sADC01.getMDH().setEvalInfoMask (m_sADC01.getMDH().getEvalInfoMask() | MDH_ONLINE) ;
    m_sADC01.getMDH().setPhaseFT (lLine+1 == m_lLinesToMeasure  &&
        m_sADC01.getMDH().getCacq()+1 == rMrProt.getlAverages() );

    /* --------------------------------------------------- */
    /* Set the frequency/phase properties of the RF pulses */
    /* --------------------------------------------------- */
    m_sSRF01zSet.prepSet (m_asSLC[lSlice], m_sSRF01,  lLine-m_lEchoLine) ;    /*! EGA-05 !*/
    m_sSRF01zNeg.prepNeg (m_asSLC[lSlice], m_sSRF01,  lLine-m_lEchoLine) ;    /*! EGA-05 !*/

    m_sSRF02zSet.prepSet (m_asSLC[lSlice], m_sSRF02) ;                   /*! EGA-05 !*/
    m_sSRF02zNeg.prepNeg (m_asSLC[lSlice], m_sSRF02) ;                   /*! EGA-05 !*/

    /* -------------------------------------------------------- */
    /* Set the frequency/phase properties of the readout events */
    /* -------------------------------------------------------- */
    m_sADC01zSet.prepSet (m_asSLC[lSlice], m_sADC01, m_sGR_RO) ;   /*! EGA-06 !*/
    m_sADC01zNeg.prepNeg (m_asSLC[lSlice], m_sADC01, m_sGR_RO) ;   /*! EGA-06 !*/


    /* -------------------------------------------------------- */
    /* RF alternation for every other line                      */
    /* -------------------------------------------------------- */
    if ((lLine + m_sADC01.getMDH().getCacq()) % 2)  {
        m_sSRF01zSet.increasePhase (180.0);
        m_sSRF01zNeg.decreasePhase (180.0);
        m_sADC01zSet.increasePhase (180.0);
        m_sADC01zNeg.decreasePhase (180.0);
    }

    /* ------------------------------------------- */
    /* Execute sequence building-block function(s) */
    /* ------------------------------------------- */

    if (! m_MSat.run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice]) )
        return m_MSat.getNLSStatus() ;
    for (lI=MAXRSATS-1; lI>=0; lI--) { //send RSats in inverse order
        if (! m_RSat[lI].run(rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice]) )
            return m_RSat[lI].getNLSStatus() ;
    }
    if(! m_CSatFat.run (rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice]) )
        return m_CSatFat.getNLSStatus() ;
    if(! m_CSatWat.run (rMrProt, rSeqLim, rSeqExpo, &m_asSLC[lSlice]) )
        return m_CSatWat.getNLSStatus() ;



    fRTEBInit( m_asSLC[lSlice].getROT_MATRIX() );    /* EGA-07 */
    lT=0;
    /************************************* S E Q U E N C E   T I M I N G *************************************/
    /*            Start Time       |    NCO    |  SRF  |  ADC  |    Gradient Events    | Sync                */
    /*              (usec)         |   Event   | Event | Event | phase | read  | slice | Event               */
    /*fRTEI(                       ,           ,       ,       ,       ,       ,       ,        );   [ Clock]*/
    /*********************************************************************************************************/
#if OSCBIT_IN_KERNEL
    fRTEI(lT+=                  0,          0,      0,      0,      0,      0,      0, &m_OscBit);
#endif
    fRTEI(lT+=                  0,          0,      0,      0,&m_sGP_sp,&m_sGR_sp,&m_sGS_EX,       0); /*[     0]*/
    //fRTEI(lT+=               6000,&m_sSRF01zSet,&m_sSRF01,      0,      0,      0,      0,       0); /*[  6000]*/
    fRTEI(lT+=               6256,&m_sSRF01zSet,&m_sSRF01,      0,      0,      0,      0,       0); /*[  6256]*/
    fRTEI(lT+m_sSRF01.getDuration(),&m_sSRF01zNeg,      0,      0,      0,      0,      0,       0); /*[  8560]*/
    //fRTEI(lT+=               2560,          0,      0,      0,&sTAB01,&sGR_de,      0,       0); /*[  8560]*/
    fRTEI(lT+=               2304,          0,      0,      0,&m_sTAB01,&m_sGR_de,      0,       0); /*[  8560]*/
    fRTEI(lT+=               1000,          0,      0,      0,      0,      0,&m_sGS_re,       0); /*[  9560]*/
    fRTEI(lT+=               2940,          0,      0,      0,      0,      0,      0,       0); /*[ 12500]*/
    fRTEI(lT+=m_alTEFil[ 0]        ,          0,      0,      0,      0,      0,      0,       0); /*[ 12500]*/
    fRTEI(lT+=                  0,          0,      0,      0,      0,      0,&m_sGS_RE,       0); /*[ 12500]*/
    fRTEI(lT+=               1000,&m_sSRF02zSet,&m_sSRF02,      0,      0,      0,      0,       0); /*[ 13500]*/
    fRTEI(lT+m_sSRF02.getDuration(),&m_sSRF02zNeg,      0,      0,      0,      0,      0,       0); /*[ 16060]*/
    fRTEI(lT+=               3440,          0,      0,      0,      0,      0,      0,       0); /*[ 16940]*/
    fRTEI(lT+=m_alTEFil[ 0]        ,          0,      0,      0,      0,      0,      0,       0); /*[ 16940]*/
    fRTEI(lT+=                  0,          0,      0,      0,      0,&m_sGR_RO,      0,       0); /*[ 16940]*/
    fRTEI(lT+=               1500,&m_sADC01zSet,      0,&m_sADC01,      0,      0,      0,       0); /*[ 18440]*/
    fRTEI(lT+m_sADC01.getRoundedDuration(),&m_sADC01zNeg,0,     0,      0,      0,      0,       0); /*[ 26120]*/
    fRTEI(lT+=               9160,          0,      0,      0,      0,      0,      0,       0); /*[ 27600]*/

    mSEQTest (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockCheck, 10, lLine,
        m_asSLC[lSlice].getSliceIndex(), 0, 0) ;
    mSEQTest (rMrProt, &rSeqLim, &rSeqExpo, ulTestIdent    , 10, lLine,
        m_asSLC[lSlice].getSliceIndex(), 0, 0) ;
    CheckStatusPB(lStatus = fRTEBFinish(),"fRTEBFinish [*0010*]");

FINISHED:


    return (lStatus);
}








NLS_STATUS Se15b130::createUI (SeqLim &)
{

    static const char *ptModule = {"Se15b130::createUI"};

#ifdef WIN32

    //  ----------------------------------------------------------------------
    //  Delete existing instance if necessary
    //  ----------------------------------------------------------------------
    if ( m_pUI )  {
        delete m_pUI;
        m_pUI = NULL;
    }



    //  ----------------------------------------------------------------------
    //  Instantiation of the UI class
    //  ----------------------------------------------------------------------
    try  {
        m_pUI = new Se15b130UI();
    }

    catch (...)  {
        delete m_pUI;
        m_pUI = NULL;

        TRACE_PUT1(TC_ALWAYS, TF_SEQ,"%s: Cannot instantiate UI class !", ptModule);
        return ( SEQU_ERROR );
    }

#endif

    return ( SEQU_NORMAL );

}   // end: Se15b130::createUI



const Se15b130UI* Se15b130::getUI (void) const
{
    return ( m_pUI );
}






/*---------------------------------------------------------------------------*/
/*  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential        */
/*---------------------------------------------------------------------------*/
