//-----------------------------------------------------------------------------
//  Copyright (C) Siemens AG 1998-2006  All Rights Reserved.  Confidential
//-----------------------------------------------------------------------------
//
// Project: NUMARIS/4
//
//    File: \n4\pkg\MrServers\MrImaging\seq\actuationOpenLoop\a_MiniFLASH.cpp
//

#include "MrServers/MrImaging/seq/actuationOpenLoop/a_MiniFLASH.h"
#include "MrServers/MrImaging/seq/common/feedbackData.h"
#include "MrServers/MrImaging/seq/actuationOpenLoop/a_MiniFLASH_UI.h"
#include "MrServers/MrMeasSrv/SeqIF/csequence.h"
#include "MrServers/MrImaging/libSeqSysProp/SysProperties.h"
#include "MrServers/MrImaging/seq/SystemProperties.h"        // Siemens system properties
#include "MrServers/MrImaging/ut/libsequt.h"                 // for mSEQTest

#ifdef WIN32
    #include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
    #include <iostream>
    #include <fstream>
	using namespace std;
#endif

//#include <iostream>
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
using namespace WPT_NAMESPACE;


#ifdef WIN32
	const uint16_t CAPACITY = 10000;
	double g_gradValuesHistory[CAPACITY] = {0};
	uint16_t g_gradIndex = 0;
	fstream g_fileID;
#endif


MiniFlash::MiniFlash()
		: m_nbGrad				(4)
		, m_sGPropulsion1       ("sGPropulsion1")
		, m_sGPropulsion2       ("sGPropulsion2")
		, m_sGPropulsion3       ("sGPropulsion3")
		, m_sGPropulsion4       ("sGPropulsion4")
        , m_pUI                 (new MiniFlashUI()) 
		, m_WIPParamTool        (*this)
{
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

	rSeqLim.setMyOrigFilename ( __FILE__ );
	rSeqLim.setSequenceOwner ("USER");
    // -----------------------
    // Setting the hard limits
    // -----------------------

	rSeqLim.disableSAFEConsistencyCheck();
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
    rSeqLim.setFlipAngle            (       10.0,       90.0,          1.0,   90.000);
    //rSeqLim.setExtSrfFilename                                     ("%MEASDAT%/extrf.dat")  ;


    // The base resolution tells how many samples in readout direction are acquired in k-space. The increment-value
    // is not set directly. Instead, a key is used which is connected to values of 16, 32, 64, etc. The allowed
    // values can be looked up in "kSpaceDefines.h".
    //                              (        min,        max,    increment,  default)
    rSeqLim.setBaseResolution       (         64,        512,  SEQ::INC_64,      448); //INC_BASE2 also available
 
	

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
   //if((MRRESULT_SEV & (lStatus = createUI(rSeqLim))) == MRRESULT_SEV) {

        // The MRTRACE macro is used to log information for debugging purposes
       // MRTRACE("Instantiation of MiniFlashUI class failed!");
       // return lStatus;
    //}


    // This compiler directive restricts this code to the HOST version dll.
    // In this way the same source code can be compiled for both the Host and the MARS.
/*
    if(NULL == m_pUI) {
        MRTRACE("MiniFlashUI object pointer is null (creation failed probably)!");
        return ( SEQU_ERROR );
    }
*/

    // ----------------------------------------------
    // Declaration of pointer to UI parameter classes
    // ----------------------------------------------
    lStatus = m_pUI->registerUI(rSeqLim, m_WIPParamTool);
	//lStatus = m_pUI->registerUI(rSeqLim);
    if(MrSeverity(lStatus) != MRRESULT_SUCCESS) {
        MRTRACE("Registering MiniFlashUI object failed! Errorcode: %i", lStatus);
        return lStatus;
    }

    // -----------------------------------------------------------------
    // file containing the default postprocessing protocol (EVAProtocol)
    // -----------------------------------------------------------------
 #ifdef WIN32   
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
    

    m_dMinRiseTime = SysProperties::getGradMinRiseTime(rMrProt.gradSpec().mode());
    m_dGradMaxAmpl = SysProperties::getGradMaxAmpl(rMrProt.gradSpec().mode());
	m_dGradMaxAmpl = 21.0;


	//. Prepare Gradient 1
	m_sGPropulsion1.setMaxMagnitude(42.9);
	if((m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradDur1)<0.000001)) //if 0
		m_sGPropulsion1.setAmplitude(0.0);
	else
		m_sGPropulsion1.setAmplitude(m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradAmpl1));
	m_sGPropulsion1.setDuration((long)((m_WIPParamTool.getDoubleValue(rMrProt, WIPDoubleGradDur1))*1000/GRAD_RASTER_TIME) * GRAD_RASTER_TIME);
	m_sGPropulsion1.setRampUpTime(300);
	m_sGPropulsion1.setRampDownTime(300);
	if( !( m_sGPropulsion1.prep() ) ||
        !( m_sGPropulsion1.check() ) )
      return m_sGPropulsion1.getNLSStatus();

	//. Prepare Gradient 2
	m_sGPropulsion2.setMaxMagnitude(42.9);
	if((m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradDur2)<0.000001)) //if 0
		m_sGPropulsion2.setAmplitude(0.0);
	else
		m_sGPropulsion2.setAmplitude(m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradAmpl2));
	m_sGPropulsion2.setDuration((long)((m_WIPParamTool.getDoubleValue(rMrProt, WIPDoubleGradDur2))*1000/GRAD_RASTER_TIME) * GRAD_RASTER_TIME);
	m_sGPropulsion2.setRampUpTime(300);
	m_sGPropulsion2.setRampDownTime(300);
	if( !( m_sGPropulsion2.prep() ) ||
        !( m_sGPropulsion2.check() ) )
      return m_sGPropulsion2.getNLSStatus();

	//. Prepare Gradient 3
	m_sGPropulsion3.setMaxMagnitude(42.9);
	if((m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradDur3)<0.000001)) //if 0
		m_sGPropulsion3.setAmplitude(0.0);
	else
		m_sGPropulsion3.setAmplitude(m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradAmpl3));
	m_sGPropulsion3.setDuration((long)((m_WIPParamTool.getDoubleValue(rMrProt, WIPDoubleGradDur3))*1000/GRAD_RASTER_TIME) * GRAD_RASTER_TIME);
	m_sGPropulsion3.setRampUpTime(300);
	m_sGPropulsion3.setRampDownTime(300);
	if( !( m_sGPropulsion3.prep() ) ||
        !( m_sGPropulsion3.check() ) )
      return m_sGPropulsion3.getNLSStatus();

	//. Prepare Gradient 4
	m_sGPropulsion4.setMaxMagnitude(42.9);
	if((m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradDur4)<0.000001)) //if 0
		m_sGPropulsion4.setAmplitude(0.0);
	else
		m_sGPropulsion4.setAmplitude(m_WIPParamTool.getDoubleValue (rMrProt, WIPDoubleGradAmpl4));
	m_sGPropulsion4.setDuration((long)((m_WIPParamTool.getDoubleValue(rMrProt, WIPDoubleGradDur4))*1000/GRAD_RASTER_TIME) * GRAD_RASTER_TIME);
	m_sGPropulsion4.setRampUpTime(300);
	m_sGPropulsion4.setRampDownTime(300);
	if( !( m_sGPropulsion4.prep() ) ||
        !( m_sGPropulsion4.check() ) )
      return m_sGPropulsion4.getNLSStatus();
	
	// Gradient angles
	m_gradAngleTable[0] = (m_WIPParamTool.getSelectionValue(rMrProt, WIPSelectionGradDir1) == Z_DIR)? M_PI/2.0: 0.0;
	m_gradAngleTable[1] = (m_WIPParamTool.getSelectionValue(rMrProt, WIPSelectionGradDir2) == Z_DIR)? M_PI/2.0: 0.0;
	m_gradAngleTable[2] = (m_WIPParamTool.getSelectionValue(rMrProt, WIPSelectionGradDir3) == Z_DIR)? M_PI/2.0: 0.0;
	m_gradAngleTable[3] = (m_WIPParamTool.getSelectionValue(rMrProt, WIPSelectionGradDir4) == Z_DIR)? M_PI/2.0: 0.0;


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
    
    //. Send the delay between measurements
    //OnErrorReturn(fSBBMeasRepetDelaysRun ( rMrProt, rSeqLim, rSeqExpo, 0 ));

    if(IS_UNIT_TEST_ACTIVE(rSeqLim))
    {
        mSEQTest (rMrProt, rSeqLim, rSeqExpo, RTEB_ClockInitTR, 0, 0, m_asSLC[0].getSliceIndex(), 0, 0);
    }

    //. Loop over lines
    for (int i = 0; i < m_nbGrad; i++)
    {
        // Call Kernel function
        OnErrorReturn(runKernel(rMrProt,rSeqLim,rSeqExpo,KERNEL_IMAGE,0, 0, i));
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
	static const char *ptModule = {"MiniFlash::runKernel"};
    NLS_STATUS         lStatus  = SEQU__NORMAL;

	long lT = 0;


	applyRotMatrix(m_gradAngleTable[lLine]);

	fRTEBInit( m_asSLC[0].getROT_MATRIX());

	sGRAD_PULSE *gPtr = NULL;
	if (lLine == 0)
		gPtr = &m_sGPropulsion1;
	else if (lLine == 1)
		gPtr = &m_sGPropulsion2;
	else if (lLine == 2)
		gPtr = &m_sGPropulsion3;
	else
		gPtr = &m_sGPropulsion4;


	

	//- ***************************************************** S E Q U E N C E   T I M I N G **********************************************************************
	//- *          Start Time                   |      NCO     |    SRF   |    ADC   |                Gradient Events                 | Sync
	//- *            (usec)                     |     Event    |   Event  |   Event  |      phase     |     read     |      slice     | Event
	//- *fRTEI(                                 ,              ,          ,          ,                ,              ,                ,      );
	//- **********************************************************************************************************************************************************
	fRTEI ( lT                                  ,       0,           0,         0,            0,           *gPtr,        0,          0 );
	fRTEI ( lT += (*gPtr).getDuration()         ,       0,           0,         0,            0,             0,          0,          0 );


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


NLSStatus MiniFlash::receive(SeqLim &rSeqLim, MrProtocolData::SeqExpo &rSeqExpo, SEQData& rSEQData)
{
	static const char *ptModule = {"MiniFlash::receive"};
	NLSStatus         lStatus  = SEQU__NORMAL;

	return lStatus;
}



NLS_STATUS MiniFlash::createUI (SeqLim&)
{
	static const char *ptModule = {"MiniFlash::createUI"};
    #ifdef WIN32

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


// --------------------------------------------------------------------------------------------
///  \brief    This method apply a rotation matrix 
///
///
///  \param    axis : A value set by the user interface card      
///
///  \return   NONE
// --------------------------------------------------------------------------------------------
void MiniFlash::applyRotMatrix(double theta)
{
	sROT_MATRIX mat;

	mat.dMat[0][0] = 0; 
	mat.dMat[0][1] = cos(theta);
	mat.dMat[0][2] = 0;

	mat.dMat[1][0] = 1;
	mat.dMat[1][1] = 0;
	mat.dMat[1][2] = 0;

	mat.dMat[2][0] = 0;
	mat.dMat[2][1] = sin(theta);
	mat.dMat[2][2] = 0;

	m_asSLC[0].setROT_MATRIX(mat);
}

void MiniFlash::printRotationMatrix(const sROT_MATRIX mat)
{
	#ifdef WIN32
	for(int i =0; i<3; i++)
	{
		for(int j = 0; j<3; j++)
			cout << mat.dMat[i][j];
		cout << endl;
	}
	#endif

}

