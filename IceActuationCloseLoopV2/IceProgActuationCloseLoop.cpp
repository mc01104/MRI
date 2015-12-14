//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrApplications\MrService\Seq\Ice\IceProgFbPerformance\IceProgActuationCloseLoop.cpp
//	 Version: \main\11
//	  Author: Ouajdi Felfoul
//	    Date: 2013-12-01
//
//	    Lang: C++
//
//	 Descrip: ActuationCloseLoop process rotors and capsules projections data
//	
//
//	 Classes: ActuationCloseLoop
//
//	---------------------------------------------------------

//#define DEBUG_MODE
// Actuation Close loop
#include "IceProgActuationCloseLoop.h"
// ICE_ERR etc.
#include "MrServers/MrVista/include/Parc/Trace/IceTrace.h"
// SEQData
#include "MrServers/MrMeasSrv/SeqIF/libMES/SEQData.h"
// FBData struct
#include "feedbackData.h"
// define MByte
#include "MrServers/MrVista/include/Common/MrCommonDefs.h"
#include <fstream>
#include <string>

/*
*  ctor sets default values on members
*/

ActuationCloseLoop::ActuationCloseLoop()
{
	m_pFeedbackSvc	  = 0;
	m_pSeqData		  = 0;
	m_callLoop		  = 0;
	m_loopCircleFound = 0;
	m_nbRotors        = 1;
	m_nbCapsules      = 1;
	m_isCircleFound   = false;
	m_Coordinates     = 0;
	m_nbElem		  = 0;   // number of points used for circular fit.
	m_targetAngleIndex= 0; // counter used to update the target angle in position control exp.
	m_experimentType  = 0;
	for(int i = 0; i<MAX_NB_ROTOR; i++)
	{
		m_position[i][0]		= 0.0;
		m_position[i][1]		= 0.0;
		m_positionKminus1[i][0]	= 0.0;
		m_positionKminus1[i][1]	= 0.0;
		m_positionKminus2[i][0]	= 0.0;
		m_positionKminus2[i][1]	= 0.0;
		m_center  [i][0]        = 0.0; // X-coordinate for the center of the rotor
		m_center  [i][1]		= 0.0; // Y-coordinate for the center of the rotor
		m_radius	[i]			= 0.0; // radius of the i rotor 
		for (int j =0; j<2; j++)
		{
			m_boundariesMinus[i][j] = 0;
			m_boundariesPlus [i][j] = 0;
		}
		m_isCircleFoundAllRotors[i]	= false;
	}
	m_controlType   = OPEN_LOOP;
	m_isRotorCTRL   = false;
	m_isCapsuleCTRL = false;
	m_isSimultaneousTrackActuation = false;
	m_spinningDirection = 1;
	m_isMultiAxis = false;

	m_rotors[0].setBearingRadius(0.005);
	m_rotors[0].setSaturationMag(1360000);
	m_rotors[0].setBearingDensity(8530);
	m_rotors[0].setRadiusRotorArm(0.032);//0.018
	m_rotors[0].setLoadTorque(0);
	m_rotors[0].setStaticFriction(0.00008);
	m_rotors[0].setViscousFriction(0.00001);

	m_rotors[1].setBearingRadius(0.005);
	m_rotors[1].setSaturationMag(1360000);
	m_rotors[1].setBearingDensity(8530);
	m_rotors[1].setRadiusRotorArm(0.032);
	m_rotors[1].setLoadTorque(0);
	m_rotors[1].setStaticFriction(0.00008);
	m_rotors[1].setViscousFriction(0.00001);

	m_rotors[2].setBearingRadius(0.005);
	m_rotors[2].setSaturationMag(1360000);
	m_rotors[2].setBearingDensity(8530);
	m_rotors[2].setRadiusRotorArm(0.032);
	m_rotors[2].setLoadTorque(0);
	m_rotors[2].setStaticFriction(0.00008);
	m_rotors[2].setViscousFriction(0.00001);
}

/*
*  dtor releases ressources
*/
ActuationCloseLoop::~ActuationCloseLoop()
{
	if(m_pSeqData != 0)
	{
		delete m_pSeqData;

		m_pSeqData = 0;
	}
	//free allocated data for circle fitting
	if(m_Coordinates != 0)
	{
		for(int i = 0; i<MAX_PTS_CIRCLE; i++)
			delete [] m_Coordinates[i];
		delete [] m_Coordinates;

	}


}

/*
*  not used yet 
*
*  Result: I_OK
*/
IResult ActuationCloseLoop::BeginInit(IParcEnvironment*)
{

	m_pSeqData = new SEQData();
	if (m_pSeqData == 0) 
	{
		ICE_ERR("Cannot allocate SeqData structure.");

		return I_FAIL;
	}
	// Allocate data for circle fitting
	m_Coordinates = new double*[MAX_PTS_CIRCLE]; // We need 
	for (int i = 0; i<MAX_PTS_CIRCLE; i++)
		m_Coordinates[i] = new double[2];

	for(int i = 0; i< MAX_PTS_CIRCLE; i++)
		for (int j = 0; j< 2; j++)
			m_Coordinates[i][j] = 0.0; 

	//Kalman::initializeFilter()
	//if(!m_kalmanFilter.initializeFilter())
	//return I_FAIL;
	// set ID required by fbperformance sequence
	m_pSeqData->setID("MRIR");

	if(m_pSeqData->allocData(500 MByte) == false)
	{
		ICE_ERR("failed to preallocate buffer.");

		return I_FAIL;
	}

	return I_OK;
}

/*
*  when simu mode opens a file for writing
*
*  Result: I_OK
*/
IResult ActuationCloseLoop::EndInit(IParcEnvironment* pEnv)
{
	// retrieve FeedbackService from environment
	m_pFeedbackSvc = Parc::cast<IFeedback>(pEnv->Item("FeedbackSvc"));

	if(m_pFeedbackSvc == 0)
	{
		ICE_ERR("invalid feedback service ref");
		return I_FAIL;
	}

	return I_OK;
}

/*
*  sends data to mpcu
*
*  Result: I_OK / I_FAIL
*/

IResult ActuationCloseLoop::ComputeScan(IceAs& srcAs, MdhProxy& aMdh, ScanControl& ctrl)
{
	long lPeakPos[MAX_NB_ROTOR] = {0};
	double thetaMeas[MAX_NB_ROTOR];
	uint16_t prjAxis = 0, loopTest = 0, loopTest2 = 0;
	bool status;
	uint16_t erreurFeedBack, switchControl, rotationPlane;
	//double frequencyOffset = -4500.0, readoutGradAmp = 10.3; // Hz, mT/m
	FBData fbData;
	float CNR = 0.0;
	float cnrTab[MAX_NB_ROTOR] = {0.0};

	//uint16_t channelID = 0;
	//double const TARGET_ANGLE_TAB[20] = {0., M_PI/4.0, 0., M_PI/2.0, 0., 2*M_PI/3.0, 0., 5*M_PI/6.0, 0., M_PI, 0.0, -M_PI/4.0, 0.0, -M_PI/2.0, 0, -2*M_PI/3.0, 0. , -5*M_PI/6.0, 0., -M_PI};
    double const TARGET_ANGLE_TAB[31] = {0., -M_PI/4.0, -M_PI/2.0, -2*M_PI/3.0, -5*M_PI/6.0, -M_PI, 5*M_PI/6.0, 2*M_PI/3.0, M_PI/2.0, M_PI/4.0,
		                                 0., M_PI/4.0, 0., M_PI/2.0, 0., 2*M_PI/3.0, 0., 5*M_PI/6.0, 0., M_PI, 0.0, -M_PI/4.0, 0.0, -M_PI/2.0, 0, -2*M_PI/3.0, 0. , -5*M_PI/6.0, 0., -M_PI};


	double rotatedCenter; // Position of the rotor center in the simultaneous actuation imaging projection plane
	//Retrieve some MDH parameters about the current scan.
	aMdh.getFreeParameterByIndex(0, prjAxis);		// prjAxis refers to which axis is procecessed x, y or z
	aMdh.getFreeParameterByIndex(1, loopTest);      // Keep track of the projection number
	aMdh.getFreeParameterByIndex(2, erreurFeedBack);// set to 0 in the RunKernel method in case no feedback is received
	//aMdh.getFreeParameterByIndex(3, switchControl); // switch between simultaneous and interleaved actuation and tracking
	aMdh.getFreeParameterByIndex(3, rotationPlane); // rotation plane: 0: X_Y, 1: X_Z, 2: Y_Z
	aMdh.getFreeParameterByIndex(4, loopTest2); // rotation plane: 0: X_Y, 1: X_Z, 2: Y_Z


	switchControl = 0;

	/*
	switch (rotationPlane)
	{
	case 0:
		ICE_OUT("Plan X_Y"); 
		break;
	case 1:
		ICE_OUT("Plan X_Z"); 
		break;
	case 2:
		ICE_OUT("Plan Y_Z"); 
		break;
	default:
		ICE_OUT("Unknown Plane value"); 
	}
*/

	if(erreurFeedBack == 0)
	{
		ICE_OUT("ERREUR ERREUR ERREUR ERREUR ERREUR NO FEEDBACK RECEIVED");
	}
	//DEBUG
	ICE_OUT("loopTest " <<  loopTest);  
	//ICE_OUT("loopTest2 " <<  loopTest2);  

	//ICE_OUT("prjAxis " <<  prjAxis);

	// The first ADC should be a configuration ADC containing protocol data for the experiment
	if(aMdh.getIceProgramPara(0) == 0) // COM ADC received
	{
		ICE_OUT("COM ADC RECEIVED");
		processingConfigurationData(aMdh, srcAs.getObj()->getLen(COL), prjAxis); // Process the data contained in the ADC MDH
		return I_OK;                   // Nothing more to do...exit without requesting feedback
	} 

	// Dump some protocol data for the First signal ADC 
	// DEBUG
	if (loopTest == 0)
	{

		//Dump some experiments parameters.
		ICE_OUT("ControlType " <<  m_kalmanFilter.getControlType() << " K1, K2 " << m_kalmanFilter.getFBLinearGainK1() << ", " << m_kalmanFilter.getFBLinearGainK2() );
		ICE_OUT("nbRotors " <<  m_nbRotors << " t1, t2 " << m_kalmanFilter.getTargetTheta1() << ", " << m_kalmanFilter.getTargetTheta2());
		ICE_OUT("setTargetTheta1 " <<  m_kalmanFilter.getTargetTheta1() << " mteas " << m_kalmanFilter.getTimeSuccTracking() << " FeedBackDelay " <<  aMdh.getReadOutOffcentre());
		for(int i = 0; i<MAX_NB_ROTOR; i++)
		{
			m_circleFit[i].updateInterval(50);
		}
	}

	// Check that the Experiment protocol is valid.  
	// Example of inconsistency: m_isRotorCTRL = true and m_isCapsulerCTRL = true
	// Under construction
	if (checkingConsistency() == false)
	{
		ICE_OUT("ERREUR EXPERIMENT PROTOCOL NOT CONSISTENT");
	}

	// if control type is open Loop then do nothing
	// This mode is used to save raw data without any processing
	if (m_controlType == OPEN_LOOP)
	{
		//return I_OK;
	}
	// if experience type is for capsules control
	// This mode is not supported yet. Do nothing 
	//if (m_isCapsuleCTRL == true)
	//{
	//	return I_OK;
	//}
	// if simultaneous tracking and actuation
	// This mode is not supported yet. Do nothing 
	//if (m_isSimultaneousTrackActuation == true)
		//return I_OK;

	//If we are here, then Signal data ADC is received
	// Retrieve the column and channels number for the current scan
	long iceObjColLen = srcAs.getObj()->getLen(COL);
	long iceObjChaLen = srcAs.getObj()->getLen(CHA);
	


	/************************************************************************/
	// Raw Data shaping and processing
	/************************************************************************/

	// Find the magnitude data for the current scan
	// srcCombineObj : store the Combined signal of the srcData
	IceObj::Pointer srcCombineObj ( Parc::HeapObject<IceObj>::Create());
	status = srcCombineObj->create( ICE_FORMAT_CXFL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object srcFTObj failed!");
	}
	IceAs srcCombineAs(srcCombineObj);
	srcCombineAs = (*srcCombineObj)(); // specifies whole ima object for COL and LIN
	
	// The orientation of the new object (i.e., the one that would hold the combined information from all the channels) needs to have the
	// sam orientation as the srcData.
	Ice.resetSoda(srcCombineAs);
	if(!Ice.SDC.calcSoda(srcCombineAs, srcAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for SyntheticCorrMaskAs failed!");
	}

	Ice.Combine(srcCombineAs, srcAs,IcmCXunweighted);
	
	// Find the magnitude data for combined signal
	// srcFTCombinedObj : store the FT of the srcData
	IceObj::Pointer srcFTCombinedObj ( Parc::HeapObject<IceObj>::Create());
	status = srcFTCombinedObj->create( srcCombineAs);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object srcFTObj failed!");
	}
	IceAs srcFTCombinedAS(srcFTCombinedObj);
	srcFTCombinedAS = (*srcFTCombinedObj)(); // specifies whole ima object for COL and LIN

	// Perform FT
	Ice.FT(srcFTCombinedAS, srcCombineAs);
	
	IceObj::Pointer magDataCombinedFTObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataCombinedFTObj->create( ICE_FORMAT_FL, srcFTCombinedAS);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object magDataFTObj failed!");
	}
	IceAs magDataCombinedFTAS(magDataCombinedFTObj);
	magDataCombinedFTAS = (*magDataCombinedFTObj)(); // specifies whole ima object for COL and LIN

	//Store magnitude data
	Ice.ExtractComplex(magDataCombinedFTAS, srcFTCombinedAS, IemAmplitude, 1);


	// Find the magnitude data for the current scan
	// srcFTObj : store the FT of the srcData
	IceObj::Pointer srcFTObj ( Parc::HeapObject<IceObj>::Create());
	status = srcFTObj->create( srcAs);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object srcFTObj failed!");
	}
	IceAs srcFTAs(srcFTObj);
	srcFTAs = (*srcFTObj)(); // specifies whole ima object for COL and LIN

	// Perform FT
	Ice.FT(srcFTAs, srcAs);

	// magDataFTObj : store the magnitude of the FT of the srcData
	IceObj::Pointer magDataFTObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataFTObj->create( ICE_FORMAT_FL, srcFTAs);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object magDataFTObj failed!");
	}
	IceAs magDataFTAs(magDataFTObj);
	magDataFTAs = (*magDataFTObj)(); // specifies whole ima object for COL and LIN

	//Store magnitude data
	Ice.ExtractComplex(magDataFTAs, srcFTAs, IemAmplitude, 1);

	// magDataFTObj : store the combined magnitude of the FT for all the channels
	IceObj::Pointer magDataFTCombineObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataFTCombineObj->create( ICE_FORMAT_FL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object magDataFTCombineObj failed!");
	}
	IceAs magDataFTCombineObjAs(magDataFTCombineObj);
	magDataFTCombineObjAs = (*magDataFTCombineObj)();

	Ice.resetSoda(magDataFTCombineObjAs);
	if(!Ice.SDC.calcSoda(magDataFTCombineObjAs, magDataFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for magDataFTCombineObjAs failed!");
	}
	
	// srcDataFTCombineCXFLObj : store the combined magnitude of the FT for all the channels
	IceObj::Pointer srcDataFTCombineCXFLObj ( Parc::HeapObject<IceObj>::Create());
	status = srcDataFTCombineCXFLObj->create( ICE_FORMAT_CXFL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object srcDataFTCombineCXFLObj failed!");
	}
	IceAs srcDataFTCombineCXFLObjAs(srcDataFTCombineCXFLObj);
	srcDataFTCombineCXFLObjAs = (*srcDataFTCombineCXFLObj)();

	Ice.resetSoda(srcDataFTCombineCXFLObjAs);
	if(!Ice.SDC.calcSoda(srcDataFTCombineCXFLObjAs, srcFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for srcDataFTCombineCXFLObjAs failed!");
	}

	// srcData: Raw data, freq domain, multiple channels
	// srcCombineObj: store the Combined signal of the srcData, freq domain, 1 channel
	// srcFTCombinedObj: FT of the srcCombineObj,time domaine, complex, 1 channel
	// magDataCombinedFTObj: magnitude data of srcFTCombinedObj, time domain, reel, 1 channel
	// srcFTObj: store the FT of the srcData,time domaine, complex, multiple channels
	// magDataFTObj: store the magnitude of srcFTObj, time domaine, reel, multiple channels
	// srcDataFTCombineCXFLObjAs: store the FT for all the channels
	// magDataFTCombineObjAs: store the combined magnitude of the FT for all the channels

	//Ice.Combine(magDataFTCombineObjAs, magDataFTAs, IcmAmplSelfweighted, 1, 0);
	// magDataFTCombineObjAs refers now to the data that will be processed for peak finding.

	//CNR = chooseAntenna(magDataFTAs, iceObjColLen, iceObjChaLen, magDataFTCombineObjAs, channelID);
	CNR = calcCNR(magDataFTAs, magDataFTCombineObjAs, iceObjColLen, iceObjChaLen);
	if(prjAxis == 2)
		cnrTab[0] = CNR;
	if(prjAxis == 4)
		cnrTab[1] = CNR;
	if(prjAxis == 6)
		cnrTab[2] = CNR;

    //calcCNR_CXFL(magDataFTAs, srcFTAs, srcDataFTCombineCXFLObjAs, iceObjColLen, iceObjChaLen);

	ICE_OUT( "CNR : " << CNR);

	if(m_experimentType == TWO_CAPSULES)
	{
		m_MPC_Capsules.init(iceObjColLen, iceObjChaLen,0);
		for (int i = 0; i < 2; ++i)
			m_MPC_Capsules.findCapsulePosition(srcCombineAs, magDataFTCombineObjAs, iceObjColLen, prjAxis, i + 1);
	
		m_MPC_Capsules.applyControlLaw(prjAxis);

		m_MPC_Capsules.getControlInputs(fbData);	
		
		// fill seqData
		if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
		{
			ICE_ERR("failed set data to seqData object");
			return I_FAIL;
		}

		// send data via feedback svc and return
		return m_pFeedbackSvc->Send(m_pSeqData);
	}

	// Call the appropriate position measurement method
	if (m_isRotorCTRL == true)
	{
		m_rotors[rotationPlane].updateRotorPosition(magDataFTCombineObjAs, iceObjColLen, prjAxis, switchControl);					

		if(prjAxis == 2) // The two positions are acquired.
		{
			if(m_isCircleFoundAllRotors[rotationPlane] == false) // circle not yet found, use the coming data to call circle fit
			{
				ICE_OUT("*** loop " <<  m_callLoop << " Plane" << rotationPlane <<
					    " Px " << m_rotors[rotationPlane].getXPosition() <<" Py " << m_rotors[rotationPlane].getYPosition() << " CNR2 " << CNR);

					m_circleFit[rotationPlane].addNewPoint(m_rotors[rotationPlane].getXPosition(), m_rotors[rotationPlane].getYPosition());
					m_isCircleFoundAllRotors[rotationPlane] = m_circleFit[rotationPlane].circleFitByTaubin();
				
				if (m_isCircleFoundAllRotors[rotationPlane] == true) // if the center position and radius found
				{
					float padding = 3; // mm

					double centerX, centerY;

					m_circleFit[rotationPlane].getCenterCoordinates(centerX, centerY);
					m_rotors[rotationPlane].setCenterCoordinates(centerX, centerY);
					m_rotors[rotationPlane].setRadiusRotorArm(m_circleFit[rotationPlane].getRadius()/1000.);

					double boundariesXMinus=(long) ((((m_rotors[rotationPlane].getCenterX() - (m_rotors[rotationPlane].getRadiusRotorArm() + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
					double boundariesXPlus= (long) ((((m_rotors[rotationPlane].getCenterX() + (m_rotors[rotationPlane].getRadiusRotorArm() + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
					double boundariesYMinus=(long) ((((m_rotors[rotationPlane].getCenterY() - (m_rotors[rotationPlane].getRadiusRotorArm() + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
					double boundariesYPlus= (long) ((((m_rotors[rotationPlane].getCenterY() + (m_rotors[rotationPlane].getRadiusRotorArm() + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
					m_rotors[rotationPlane].setBoundaries(boundariesXMinus, boundariesXPlus, boundariesYMinus, boundariesYPlus);

					ICE_OUT("*******************Center found********************");
					ICE_OUT("plane " << rotationPlane << " Cx " << m_rotors[rotationPlane].getCenterX() <<
						" Cy " << m_rotors[rotationPlane].getCenterY() << 
						" r " << m_rotors[rotationPlane].getRadiusRotorArm());

					return sendFeedBackData(CNR, rotationPlane);

				}
				else // Circle not found
				{
					return I_OK;
				}
			}
			else // Circle found, second projection
			{
				ICE_OUT("*** loop " <<  m_callLoop << " plane " << rotationPlane << 
					" px " << m_rotors[rotationPlane].getXPosition() <<
					" py " << m_rotors[rotationPlane].getYPosition() << 
					" CNR2 " << CNR);
				thetaMeas[0] = atan2(m_rotors[rotationPlane].getYPosition()-m_rotors[rotationPlane].getCenterY(), 
					m_rotors[rotationPlane].getXPosition()-m_rotors[rotationPlane].getCenterX());
				m_kalmanFilters[rotationPlane].initializeTheta(thetaMeas[0], 0); //index = 0 for 1 rotor
				ICE_OUT("*** Theta " <<  thetaMeas[0] ); 

				m_kalmanFilters[rotationPlane].predictControlLaw(thetaMeas[0], m_spinningDirection, CNR, X_Y, m_rotors[rotationPlane]);

				return sendFeedBackData(CNR, rotationPlane);
			}
		}
		else
			return I_OK;
	}
	else if (m_isCapsuleCTRL == true)
	{
		//findingCapsulePosition(magDataFTCombineObjAs, iceObjColLen, prjAxis, lPeakPos, switchControl);
		//findingCapsulePosition(magDataFTAs, iceObjColLen, prjAxis, lPeakPos, switchControl);
		//constructCorrelationMask(srcCombineAs, magDataFTCombineObjAs, iceObjColLen, prjAxis, lPeakPos, switchControl);

		//Find capsule position

		m_capsule.init(iceObjColLen, iceObjChaLen, m_isSimultaneousTrackActuation, m_nbCapsules);

		if(m_isSimultaneousTrackActuation == false) //interleaved mode
		{
			m_capsule.findCapsulePosition(srcCombineAs, magDataFTCombineObjAs, iceObjColLen, prjAxis);
			if(prjAxis == 2)
			{
				m_capsule.applyControlLaw(prjAxis, loopTest);
			}
			else // one prj nothing to do
				return I_OK;
		}
		else // simultaneous mode
		{
			m_capsule.findCapsulePosition(srcCombineAs, magDataFTCombineObjAs, iceObjColLen);
			m_capsule.applyPIDControl(prjAxis);
			m_capsule.applyControlLaw(prjAxis, loopTest);
		}


		m_capsule.getControlInputs(fbData);
		//
		// fill seqData
		if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
		{
			ICE_ERR("failed set data to seqData object");
			return I_FAIL;
		}

		// send data via feedback svc and return
		return m_pFeedbackSvc->Send(m_pSeqData);

		//constructCorrelationMask(srcCombineAs, magDataFTCombineObjAs, iceObjColLen, prjAxis, lPeakPos, switchControl);

	}
	
	//Save raw data in a format readable by MATLAB
	//saveDataToFile(magDataFTAs,  iceObjColLen,  iceObjChaLen, "ClosedLoop100msRep1");
	
	// Transform the position from index to mm and keep track of the three last positions for a second order 
	// finit difference velocity measurement
	//for(int i = 0; i < m_nbRotors; i++)
	//{
	if(prjAxis <= 2)
	{
		m_positionKminus2[0][prjAxis-1] = m_positionKminus1[0][prjAxis-1];
		m_positionKminus1[0][prjAxis-1] = m_position[0][prjAxis-1];
		m_position[0][prjAxis-1] = ((lPeakPos[0]/(static_cast <double> (iceObjColLen))) - 0.5)*600;
	}
	else
	{
		if(prjAxis <= 4)
		{
			m_positionKminus2[1][prjAxis-3] = m_positionKminus1[1][prjAxis-3];
			m_positionKminus1[1][prjAxis-3] = m_position[1][prjAxis-3];
			m_position[1][prjAxis-3] = ((lPeakPos[0]/(static_cast <double> (iceObjColLen))) - 0.5)*600;
		}
		else
		{
			m_positionKminus2[2][prjAxis-5] = m_positionKminus1[2][prjAxis-5];
			m_positionKminus1[2][prjAxis-5] = m_position[2][prjAxis-5];
			m_position[2][prjAxis-5] = ((lPeakPos[0]/(static_cast <double> (iceObjColLen))) - 0.5)*600;
		}
	}

/*
	if(m_callLoop <= 150)
		m_kalmanFilter.setTargetTheta(m_targetArray[0], m_targetArray[1], m_targetArray[2]);
	else 
	{
		if (m_callLoop <= 300)
			m_kalmanFilter.setTargetTheta(m_targetArray[3], m_targetArray[4], m_targetArray[5]);
		else
			m_kalmanFilter.setTargetTheta(m_targetArray[6], m_targetArray[7], m_targetArray[8]);
	}
	*/	
		/*
		if(prjAxis == 2)
			ICE_OUT("*** loop " <<  m_callLoop << " rotor" << i+1 << " Px " << m_position[i][0]);
		else
			ICE_OUT("*** loop " <<  m_callLoop << " rotor" << i+1 <<" Py " << m_position[i][1]);
*/
	//}

	// Make the position control update the theta target every 50 projections
	/*
	if (m_loopCircleFound % 51 == 0 && m_loopCircleFound != 0)
	{
		if(prjAxis == 2)
			m_targetAngleIndex++;

		if (m_targetAngleIndex > 19)
		{
			m_targetAngleIndex = 0;
		}
		m_kalmanFilter.setTargetTheta1(TARGET_ANGLE_TAB[m_targetAngleIndex]);
	}
*/


	//if simultaneous imaging and actuation, find the center position according to the rotation matrix


	// Correct the position for the susceptibility translation
	//for(int i = 0; i < m_nbRotors; i++)
	//{
	//	m_position[i][prjAxis-1] -= (((frequencyOffset/LARMOR_FREQ)*1000)/readoutGradAmp)*1000;
	    //ICE_OUT( "translation " << (((frequencyOffset/LARMOR_FREQ)*1000)/readoutGradAmp)*1000 );
	//}

	// For rotors...
	// If the center position and radius not yet found...call circleFit
	// If circle found...call the controller depending on the experiment mode.
	if (m_isRotorCTRL == true) // For rotors, the circle has to be found first 
	{
		if(m_isCircleFound == false) // circle not yet found, use the coming data to call circle fit
		{
			if(prjAxis == 2) // do this only if the x and y positions are acquired
			{

				for(int i = 0; i< m_nbRotors; i++)
				{
					ICE_OUT("*** loop " <<  m_callLoop << " rotor" << i+1 << " Px " << m_position[i][0] <<" Py " << m_position[i][1] << " CNR2 " << CNR);
					m_circleFit[i].addNewPoint(m_position[i][0], m_position[i][1]);
					m_isCircleFoundAllRotors[i] = m_circleFit[i].circleFitByTaubin();
				}
				// Check if the circle is found for all the rotors
				m_isCircleFound = true;
				for(int i = 0; i< m_nbRotors; i++)
				{
					if(m_isCircleFoundAllRotors[i] == false)
						m_isCircleFound = false;
				}

				if (m_isCircleFound == true) // if all rotors center position and radius found
				{
					float padding = 3; // mm
					for(int i = 0; i< m_nbRotors; i++) // Update the boundaries values to look for the peak
					{
						m_circleFit[i].getCenterCoordinates(m_center[i][0], m_center[i][1]);
						m_radius[i] = m_circleFit[i].getRadius();

						m_boundariesMinus[i][0] = (long) ((((m_center[i][0] - (m_radius[i] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
						m_boundariesPlus [i][0] = (long) ((((m_center[i][0] + (m_radius[i] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
						m_boundariesMinus[i][1] = (long) ((((m_center[i][1] - (m_radius[i] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
						m_boundariesPlus [i][1] = (long) ((((m_center[i][1] + (m_radius[i] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);

						ICE_OUT("*******************Center found********************");
						ICE_OUT("rotor " << i << " Cx " << m_center[i][0] <<" Cy " << m_center[i][1] << " r " << m_radius[i]);
						//std::cout << "\n*******************Center found********************";
						//std::cout << std::endl << "rotor, Cx, Cy, r {" << i << ", " << m_center[i][0] <<", " << m_center[i][1] << ", " << m_radius[i] << "}";
					}
					m_kalmanFilter.setRotorRadiusArm(m_radius[0]/1000.);
					//Circle Found ...
					//thetaMeas[0] = atan2(m_position[0][1]-m_center[0][1], m_position[0][0] - m_center[0][0]);					
					//m_kalmanFilter.init(m_position, m_positionKminus1, m_positionKminus2, m_center);
					//m_positionKminus2[0][0] = 25.19;
					//m_positionKminus2[0][1] = -1.17;
					//m_positionKminus1[0][0] = 17.58;
					//m_positionKminus1[0][1] = -50.39;
					//m_position[0][0] = 58.6;
					//m_position[0][1] = -52.15;

					//m_kalmanFilter.initializeFilter(m_position, m_positionKminus1, m_positionKminus2, m_center);
				}
			}
		}
		else// Circle found ... use coming data to update gradients values
		{
			if(prjAxis == 2)
				m_loopCircleFound++;
									
			//ICE_OUT("m_loopCircleFound " <<  m_loopCircleFound);

			// Add here simultaneous or interleaved tracking and actuation
			if(m_isSimultaneousTrackActuation == false)
			{
				if(m_isMultiAxis == true)
				{
					if(prjAxis == 6) // The 6 projections are acquired.
					{
						for(int i = 0; i< m_nbRotors; i++)
						{
							thetaMeas[i] = atan2(m_position[i][1]-m_center[i][1], m_position[i][0] - m_center[i][0]);	
							m_kalmanFilter.initializeTheta(thetaMeas[i], i); 
							ICE_OUT("loop " <<  m_callLoop << " rotor" << i+1 << " px " << m_position[i][0] <<" py " <<     m_position[i][1]);
							ICE_OUT("thetaMeas " <<  i << " "<< thetaMeas[i] );//<< i+1 << " px " << m_position[i][0] <<" py " <<     m_position[i][1]);

						}
					    m_kalmanFilter.predictControlLaw3OrthoRotors(thetaMeas, cnrTab);
					}
				}
				else
				{
					if(prjAxis == 2) // The two positions are acquired.
					{
						//ICE_OUT(" m_callLoop "    << m_callLoop                                      );
						//Alina Controller

						if(m_nbRotors == 1)
						{
							ICE_OUT("*** loop " <<  m_callLoop << " rotor1" << " px " << m_position[0][0] <<" py " << m_position[0][1] << " CNR2 " << CNR);
							thetaMeas[0] = atan2(m_position[0][1]-m_center[0][1], m_position[0][0] - m_center[0][0]);
							m_kalmanFilter.initializeTheta(thetaMeas[0], 0); //index = 0 for 1 rotor
							m_kalmanFilter.predictControlLaw(thetaMeas[0], m_spinningDirection, CNR, X_Y);

						}
						else // 2 rotors
						{
							for(int i = 0; i< m_nbRotors; i++)
							{
								thetaMeas[i] = atan2(m_position[i][1]-m_center[i][1], m_position[i][0] - m_center[i][0]);	
								m_kalmanFilter.initializeTheta(thetaMeas[i], i); 
								ICE_OUT("loop " <<  m_callLoop << " rotor" << i+1 << " px " << m_position[i][0] <<" py " <<     m_position[i][1]);
							}
							//m_kalmanFilter.predictControlLawfor2Rotors(thetaMeas[0], thetaMeas[1]);

							m_kalmanFilter.predictControlLawfor2Rotors(thetaMeas[0], thetaMeas[1]);
							//m_kalmanFilter.getControlInputs(fbData);
						}
					}
				}
			}
			else
			{					
				if (switchControl != SIMULTANEOUS) // The two first projections are for two axis to make sure to init the theta and velocity
				{	
					if(prjAxis == 2) // The two positions are acquired.
					{
						ICE_OUT("*** loop " <<  m_callLoop << " rotor1" << " px " << m_position[0][0] <<" py " << m_position[0][1] << " CNR2 " << CNR);
						thetaMeas[0] = atan2(m_position[0][1]-m_center[0][1], m_position[0][0] - m_center[0][0]);
						m_kalmanFilter.initializeTheta(thetaMeas[0], 0); //index = 0 for 1 rotor
						m_kalmanFilter.predictControlLaw(thetaMeas[0], m_spinningDirection, CNR, X_Y);
					}
				}
				else // The received projection is the only one...use it for control
				{
					ICE_OUT("*** loop " <<  m_callLoop << " rotor1" << " Pprj " << m_position[0][0] << " CNR2 " << CNR);
					// Find the center position with the rotation matrix
					// 
					double theta = m_kalmanFilter.getThetaProjection();

					rotatedCenter = applyRotationMatrix(theta, m_center[0][0], m_center[0][1]);
					ICE_OUT("rotatedCenter " <<  rotatedCenter << " m_position" << m_position[0][0] << " m_radius" << m_radius[0]);


					// put limit to the argument of asin to avoid accidental domaine error.
					// TODO. if this did happen, action has to be taken.
					double acosArg = (m_position[0][0] - rotatedCenter )/m_radius[0];
					if (acosArg <-1)
						acosArg = -1;
					if (acosArg > 1)
						acosArg = 1;

					double angleSolution1 = m_kalmanFilter.constrainAngle(acos(acosArg) - theta);
					double angleSolution2 = m_kalmanFilter.constrainAngle(-acos(acosArg) - theta);
					thetaMeas[0] = m_kalmanFilter.chooseAngle(angleSolution1, angleSolution2);

					/*
					if (m_spinningDirection == -1)
					{
						thetaMeas[0] = m_kalmanFilter.constrainAngle(theta + M_PI/2 - acos(acosArg));
					}
					else
					{
						thetaMeas[0] = m_kalmanFilter.constrainAngle(theta - M_PI/2 + acos(acosArg));
					}

*/

					ICE_OUT("(m_position[0][0]-centerRP)/m_radius[0]" <<(m_position[0][0]-rotatedCenter)/m_radius[0]);
					ICE_OUT("thetaMeas[0]" <<  thetaMeas[0]);

					m_kalmanFilter.predictControlLawForSimulImagingAndActuation(thetaMeas[0], m_spinningDirection, CNR);
				}
				
				// Send in lSlot the angle of the next projection
				// Send 10 grad values How switch from 1 reference frame to the other???
				// Find the center position in the rotated reference frame
				// m_centerRotated = m_center[0][0] * cos(angle) + m_center[0][1] * sin(angle);
				// thetaMeas[0] = m_position[0][0]-m_center[0][1], m_position[0][0] - m_center[0][0]);
			}
		}
	}
	else if(m_isCapsuleCTRL == true)
	{
		if(prjAxis == 1) // For now we support only control along x-axis
		{
			ICE_OUT("x_Position " << m_position[0][0]);
			return I_OK; // if x-axis exit without feedback.


		}
		else
		{
			ICE_OUT("z_Position " << m_position[0][1]);
			m_kalmanFilter.PI1Capsule(m_position[0][0], m_position[0][1]);
			fbData.lShot = 1;// close loop mode. radius and center found or capsule mode
			fbData.lSlot = int(CNR); // Used for sending the CNR
			fbData.SimultORInterleaved = 0;
			m_kalmanFilter.getControlInputs(fbData);
			m_callLoop++;
			//ICE_OUT("loop " <<  m_callLoop);
			//ICE_OUT("lShot ={ " << fbData.lShot);
			ICE_OUT("double uValx[] ={ " << fbData.uValx[0] << ", " << fbData.uValx[1] << ", " << fbData.uValx[2] << ", " << fbData.uValx[3] << ", " << fbData.uValx[4] << ", " << fbData.uValx[5] << ", " << fbData.uValx[6] << ", " << fbData.uValx[7] << ", " << fbData.uValx[8] << ", " << fbData.uValx[9] << "};"            );
			ICE_OUT("double uValz[] ={ " << fbData.uValz[0] << ", " << fbData.uValz[1] << ", " << fbData.uValz[2] << ", " << fbData.uValz[3] << ", " << fbData.uValz[4] << ", " << fbData.uValz[5] << ", " << fbData.uValz[6] << ", " << fbData.uValz[7] << ", " << fbData.uValz[8] << ", " << fbData.uValz[9]  << "};"            );

			// fill seqData
			if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
			{
				ICE_ERR("failed set data to seqData object");
				return I_FAIL;
			}

			// send data via feedback svc and return
			return m_pFeedbackSvc->Send(m_pSeqData);
		}

	}
	// Prepare feed back data
	fbData.lSlot = int(CNR); // Used for sending the CNR
	fbData.lShot = 0; //0 tells the sequence to run in open loop mode because radius and center not yet found
	if(m_isCircleFound == true || m_isCapsuleCTRL==true) // Tell to the sequence that the radius and center are found, so the closed-loop will start
		fbData.lShot = 1;// close loop mode. radius and center found or capsule mode

	m_kalmanFilter.getControlInputs(fbData);

	m_callLoop++;

	if(switchControl == INTERLEAVED)
	{
		if(prjAxis == 2)
		{
			/*
			ICE_OUT("**************************** OUFEL *****************************");
			ICE_OUT("Received second ADC...FEEDBACK requested. callLoop " << m_callLoop);
			ICE_OUT("*************************** OUFEL *****************************");
			ICE_OUT("aMdh.getFreeParameterByIndex(1) " << loopTest);
			*/
			// position acquisition while Spin Down
			/*if(loopTest >= 200)
			{
			for (int i = 0; i < 10; i++)
			{
			fbData.uValx[i]= 0.0;
			fbData.uValy[i]= 0.0;
			fbData.uValz[i]= 0.0;
			}

			}
			*/
			//ICE_OUT("loop " <<  m_callLoop);
			//ICE_OUT("lShot " << fbData.lShot << " lSlot " << fbData.lSlot);
			ICE_OUT("double uValx[] ={ " << fbData.uValx[0] << ", " << fbData.uValx[1] << ", " << fbData.uValx[2] << ", " << fbData.uValx[3] << ", " << fbData.uValx[4] << ", " << fbData.uValx[5] << ", " << fbData.uValx[6] << ", " << fbData.uValx[7] << ", " << fbData.uValx[8] << ", " << fbData.uValx[9] << "};"            );
			ICE_OUT("double uValy[] ={ " << fbData.uValy[0] << ", " << fbData.uValy[1] << ", " << fbData.uValy[2] << ", " << fbData.uValy[3] << ", " << fbData.uValy[4] << ", " << fbData.uValy[5] << ", " << fbData.uValy[6] << ", " << fbData.uValy[7] << ", " << fbData.uValy[8] << ", " << fbData.uValy[9] << "};"            );
			ICE_OUT("double uValz[] ={ " << fbData.uValz[0] << ", " << fbData.uValz[1] << ", " << fbData.uValz[2] << ", " << fbData.uValz[3] << ", " << fbData.uValz[4] << ", " << fbData.uValz[5] << ", " << fbData.uValz[6] << ", " << fbData.uValz[7] << ", " << fbData.uValz[8] << ", " << fbData.uValz[9]  << "};"            );
			
			fbData.SimultORInterleaved = 0;

			// fill seqData
			if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
			{
				ICE_ERR("failed set data to seqData object");
				return I_FAIL;
			}

			// send data via feedback svc and return
			return m_pFeedbackSvc->Send(m_pSeqData);
		}
		else
		{		
			// Do nothing because we send feedback only on the second ADC, when both x and y positions are acquired
			/*
			ICE_OUT("************************ OUFEL *****************************");
			ICE_OUT("Received First ADC for callLoop " << m_callLoop);
			ICE_OUT("**************************** OUFEL *****************************");
			ICE_OUT("aMdh.getFreeParameterByIndex(1) " << loopTest);
			*/
		}
	}
	else
	{
		//fbData.SimultORInterleaved = m_kalmanFilter.isGoodEstimate();

		if (m_loopCircleFound > 30)
			fbData.SimultORInterleaved = 1;
		else
			fbData.SimultORInterleaved = 0;
		// Transform the gradient into the new rotation plane...
		double gradXRotationPlane[10], gradYRotationPlane[10], gradZRotationPlane[10];
		double theta = m_kalmanFilter.getThetaProjection();
		for(int i = 0; i< 10; i++)
		{
			gradXRotationPlane[i] = fbData.uValx[i]*cos(theta) + fbData.uValz[i] * sin(theta);
			//gradYRotationPlane[i] = fbData.uValy[i];
			gradZRotationPlane[i] = -fbData.uValx[i]*sin(theta) + fbData.uValz[i] * cos(theta);
			
			// Make sure the gradient is not greater than the max value after the rotation matrix transformation
			if (gradXRotationPlane[i] > 23)
				gradXRotationPlane[i] = 23;
			if (gradXRotationPlane[i] < -23)
				gradXRotationPlane[i] = -23;

			if (gradZRotationPlane[i] > 23)
				gradZRotationPlane[i] = 23;
			if (gradZRotationPlane[i] < -23)
				gradZRotationPlane[i] = -23;

		}
		for(int i = 0; i< 10; i++)
		{
			fbData.uValx[i] = gradXRotationPlane[i];
			fbData.uValz[i] = gradZRotationPlane[i];
		}
		//...Then send the data back to the sequence
		if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
		{
			ICE_ERR("failed set data to seqData object");
			return I_FAIL;
		}

		// send data via feedback svc and return
		return m_pFeedbackSvc->Send(m_pSeqData);
	}
	return I_OK;
}
// Test : write a single float data  to a file 
void ActuationCloseLoop::saveDataToFile(IceAs& tempAs, long iceObjColLen, long iceObjChaLen, char* filename)
{
	for (int i = 0; i < iceObjChaLen; i++)
	{
		for (int j = 0; j < iceObjColLen; j++)
		{
			tempAs.modify(COL, j, 1, 1,
				CHA, i, 1, 1);
			Ice.WriteToFile(tempAs, filename, 0, "ab");
		}
	}
}


// DATA or COM scan?
/* if rotors:
setIceProgramPara(0,...) --> 0: Com data, 1: Signal Data
setIceProgramPara(1,...) --> 1: Rotor, 2: Capsule
setIceProgramPara(2,...) --> Control type (0: Open Loop, 1:Commutation control , 2:FBLinearization, 3:PI)
setIceProgramPara(3,...) --> Nb rotors
setIceProgramPara(4,...) --> Channel to be used
setIceProgramPara(5,...) --> 0: Interleaved tracking and actuation, 1: Simulatneous track/actuation
setIceProgramPara(6,...) --> Kalman gain 
setIceProgramPara(7,...) --> Velocity Kalman gain
setIceProgramPara(8,...) --> P Gain
setIceProgramPara(9,...) --> I Gain
setIceProgramPara(10,...) --> D Gain
setIceProgramPara(11,...) --> FB linearization Gain 1
setIceProgramPara(12,...) --> FB linearization Gain 2
setIceProgramPara(13,...) --> gtMeas
setIceProgramPara(14,...) --> gtRTFeedBack
setIceProgramPara(15,...) --> Theta Final 1
setIceProgramPara(16,...) --> Theta Final 2
setIceProgramPara(17,...) --> Theta Final 3
setIceProgramPara(18,...) --> Rotor arm radius
setIceProgramPara(19,...) --> Duration tracking sequence
setIceProgramPara(20,...) --> Max grad mT/m
setIceProgramPara(21,...) --> Rotor Static Friction
setIceProgramPara(22,...) --> Rotor Dynamic Friction
setIceProgramPara(23,...) --> Rotor Load torque

if capsules
setIceProgramPara(10,...) --> nbWayPoints (up to 5)

setIceProgramPara(11,...) --> prjAngle1 (degrees)
setIceProgramPara(12,...) --> prjAngle2 (degrees)
setIceProgramPara(13,...) --> prjAngle3 (degrees)
setIceProgramPara(14,...) --> prjAngle4 (degrees)
setIceProgramPara(15,...) --> prjAngle5 (degrees)

setIceProgramPara(16,...) --> Waypoint1 (index)
setIceProgramPara(17,...) --> Waypoint2 (index)
setIceProgramPara(21,...) --> Waypoint3 (index)
setIceProgramPara(22,...) --> Waypoint4 (index)
setIceProgramPara(23,...) --> Waypoint5 (index)
*/
void ActuationCloseLoop::processingConfigurationData(MdhProxy& aMdh, long iceObjColLen, uint16_t prjAxis)
{

	uint16_t temp = 0;

	double angle;

	m_MPC_Capsules.R = ::Eigen::Matrix< double, 3, 3>::Identity();
		
	ICE_OUT("RTest " <<  m_MPC_Capsules.R(0,0));  

	if(prjAxis == 1)
	{

		//m_experimentType = aMdh.getIceProgramPara(1)
		m_experimentType = TWO_CAPSULES;
		if(m_experimentType == ROTOR) // Rotor control
			m_isRotorCTRL = true;
		else if(m_experimentType == CAPSULE) // Capsule control
			m_isCapsuleCTRL = true;
		else if(m_experimentType == TWO_CAPSULES) // 2 Capsules control
			//m_is2CapsulesCTRL = true;
		
		//if(m_isRotorCTRL == true)
		//{
			ICE_OUT("m_isRotorCTRL " <<  m_isRotorCTRL << " m_isCapsuleCTRL " <<  m_isCapsuleCTRL);
		
			m_controlType = (CONTROL_TYPE) aMdh.getIceProgramPara(2);
			m_kalmanFilter.setControlType(m_controlType); 
			m_kalmanFilters[0].setControlType(m_controlType);
			m_kalmanFilters[1].setControlType(m_controlType);
			m_kalmanFilters[2].setControlType(m_controlType);
			//m_kalmanFilter.setControlType(PI);// DEBUG ONLY. DO NOT HARDCODE.
			ICE_OUT("m_controlType " <<  m_controlType );
			
			m_capsule.setPGain((double)(m_controlType)/1000.0);
			ICE_OUT("PGain " <<  (double)(m_controlType)/1000.0 );

			m_nbRotors = aMdh.getIceProgramPara(3);
			m_kalmanFilter.setNbRotors(m_nbRotors);
			m_kalmanFilters[0].setNbRotors(m_nbRotors);
			m_kalmanFilters[1].setNbRotors(m_nbRotors);
			m_kalmanFilters[2].setNbRotors(m_nbRotors);
			if(m_nbRotors>1)
				m_isMultiAxis = true;
			ICE_OUT("m_nbRotors " <<  m_nbRotors );
			
			m_capsule.setDGain(m_nbRotors);
			ICE_OUT("DGain " <<  m_nbRotors );

			//channelID = aMdh.getIceProgramPara(4);
			//channelID = 9;
			//ICE_OUT("channelID " <<  channelID );

			temp = aMdh.getIceProgramPara(5);
			m_nbCapsules = 1; // = 2; //Hardcode for 2 capsules

			if(temp == 0) // Interleaved or simultaneous
			{
				m_isSimultaneousTrackActuation = false;
			}
			else 
			{
				m_isSimultaneousTrackActuation = true;
			}
			ICE_OUT("m_isSimultaneousTrackActuation " <<  m_isSimultaneousTrackActuation );
			ICE_OUT("m_nbCapsules " <<  m_nbCapsules );

			m_isSimultaneousTrackActuation = true;

			//m_isInterleavedTrackActuation = false;// DEBUG ONLY. DO NOT HARDCODE.
			//m_isSimultaneousTrackActuation = true;// DEBUG ONLY. DO NOT HARDCODE.

			temp = aMdh.getIceProgramPara(6);
			m_kalmanFilter.setKalmanGain(temp);
			m_kalmanFilters[0].setKalmanGain(temp);
			m_kalmanFilters[1].setKalmanGain(temp);
			m_kalmanFilters[2].setKalmanGain(temp);

			ICE_OUT("KalmanGain " <<  temp );
			m_capsule.setSpoilerDuration((double)temp);
			ICE_OUT("SpoilerDuration " <<  temp);

		
			temp= aMdh.getIceProgramPara(7);
			m_kalmanFilters[0].setVelocityKalmanGain(temp);
			m_kalmanFilters[1].setVelocityKalmanGain(temp);
			m_kalmanFilters[2].setVelocityKalmanGain(temp);
			m_kalmanFilters[3].setVelocityKalmanGain(temp);
		
			ICE_OUT("VelocityKalmanGain " <<  temp );
			m_capsule.setImgMomentum((double) temp);
			ICE_OUT("ImgMomentum " <<  temp);

			temp = aMdh.getIceProgramPara(8);
			//temp = 3; // DUBUG ONLY. COMMENT THIS LINE
			m_kalmanFilter.setPGain(temp/1000.0);
			m_kalmanFilters[0].setPGain(temp/1000.0);
			m_kalmanFilters[1].setPGain(temp/1000.0);
			m_kalmanFilters[2].setPGain(temp/1000.0);
			m_capsule.setMaxGradAmplitude(temp);
			//m_capsule.setMaxGradAmplitude(21);
			ICE_OUT("MaxGradAmplitude " <<  temp);

			//temp = 1000;
			//m_capsule.setPGain(temp/1000.0);
			//ICE_OUT("PGain " <<  temp/1000.0 );

			temp = aMdh.getIceProgramPara(9);
			//temp = 1; // DUBUG ONLY. COMMENT THIS LINE
			m_kalmanFilter.setIGain(temp/1000.0);
			m_kalmanFilters[0].setIGain(temp/1000.0);
			m_kalmanFilters[1].setIGain(temp/1000.0);
			m_kalmanFilters[2].setIGain(temp/1000.0);
			
			m_capsule.setNbWayPoints((uint8_t) temp);
			m_capsule.setNbWayPoints(16); //debug
			ICE_OUT("NbWayPoints " <<  temp);

			temp = aMdh.getIceProgramPara(10);
			//temp = 10; // DUBUG ONLY. COMMENT THIS LINE
			m_kalmanFilter.setDGain(temp/20);
			m_kalmanFilters[0].setDGain(temp/20);
			m_kalmanFilters[1].setDGain(temp/20);
			m_kalmanFilters[2].setDGain(temp/20);
		
			ICE_OUT("DGain " <<  temp/1000.0 );
			//if capsule control
			//m_capsule.setNbWayPoints((uint8_t) temp);

			temp = aMdh.getIceProgramPara(11);
			m_kalmanFilter.setFBLinearGainK1(temp);
			m_kalmanFilters[0].setFBLinearGainK1(temp);
			m_kalmanFilters[1].setFBLinearGainK1(temp);
			m_kalmanFilters[2].setFBLinearGainK1(temp);
			m_kalmanFilters[3].setFBLinearGainK1(temp);
		
			ICE_OUT("FBLinearGainK1 " <<  temp );
			//if capsule control
			//m_capsule.setPrjAngles(temp, 0);



			if(temp == 1) // Used to shortcut the Calibration step
			{
				float padding = 3;

				m_isCircleFound = true;
				m_radius[0] = aMdh.getIceProgramPara(12)/1000.;
				m_center[0][0] = aMdh.getIceProgramPara(16)/1000.;
				m_center[0][0] = aMdh.getIceProgramPara(17)/1000.;

				m_boundariesMinus[0][0] = (long) ((((m_center[0][0] - (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
				m_boundariesPlus [0][0] = (long) ((((m_center[0][0] + (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
				m_boundariesMinus[0][1] = (long) ((((m_center[0][1] - (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
				m_boundariesPlus [0][1] = (long) ((((m_center[0][1] + (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);
			}


			temp = aMdh.getIceProgramPara(12);
			m_kalmanFilter.setFBLinearGainK2((double)temp);
			m_kalmanFilters[0].setFBLinearGainK2((double)temp);
			m_kalmanFilters[1].setFBLinearGainK2((double)temp);
			m_kalmanFilters[2].setFBLinearGainK2((double)temp);

			ICE_OUT("FBLinearGainK2 " <<  temp );
			//if capsule control
			//m_capsule.setPrjAngles(temp, 1);

			temp = aMdh.getIceProgramPara(13);
			m_kalmanFilter.setTimeSuccTracking((double)temp/1000.);//Convert from ms to seconds
			m_kalmanFilters[0].setTimeSuccTracking((double)temp/1000.);//Convert from ms to seconds
			m_kalmanFilters[1].setTimeSuccTracking((double)temp/1000.);//Convert from ms to seconds
			m_kalmanFilters[2].setTimeSuccTracking((double)temp/1000.);//Convert from ms to seconds

			ICE_OUT("TimeSuccTracking " <<  m_kalmanFilter.getTimeSuccTracking() );
			//if capsule control
			//m_capsule.setPrjAngles(temp, 2);

			temp = aMdh.getIceProgramPara(14);
			m_kalmanFilter.setFeedBackDelay((double)temp/1000000.);
			m_kalmanFilters[0].setFeedBackDelay((double)temp/1000000.);
			m_kalmanFilters[1].setFeedBackDelay((double)temp/1000000.);
			m_kalmanFilters[2].setFeedBackDelay((double)temp/1000000.);

			ICE_OUT("FeedBackDelay " <<  m_kalmanFilter.getFeedBackDelay());
			//if capsule control
			//m_capsule.setPrjAngles(temp, 3);

			temp = aMdh.getIceProgramPara(15);
			angle = (double)temp/1000.0f;
			//if capsule control
			//m_capsule.setPrjAngles(temp, 4);

			if(m_controlType == PI || m_controlType == LYAPUNOV_POS)
			{
				if(angle < M_PI)
				{
					m_kalmanFilter.setTargetTheta1(angle);
					m_kalmanFilters[0].setTargetTheta1(angle);
					m_kalmanFilters[1].setTargetTheta1(angle);
					m_kalmanFilters[2].setTargetTheta1(angle);

				}
				else
				{
					m_kalmanFilter.setTargetTheta1(-2*M_PI + angle);
					m_kalmanFilters[0].setTargetTheta1(-2*M_PI + angle);
					m_kalmanFilters[1].setTargetTheta1(-2*M_PI + angle);
					m_kalmanFilters[2].setTargetTheta1(-2*M_PI + angle);

				}
			}
			else
			{
				m_kalmanFilter.setTargetTheta1(angle);
				m_kalmanFilters[0].setTargetTheta1(angle);
				m_kalmanFilters[1].setTargetTheta1(angle);
				m_kalmanFilters[2].setTargetTheta1(angle);
			}
			ICE_OUT("TargetTheta " <<  m_kalmanFilter.getTargetTheta1());

			temp = aMdh.getIceProgramPara(16);
			if (temp == 1)
				m_spinningDirection = 1;
			else
				m_spinningDirection = -1;

			ICE_OUT("m_spinningDirection " <<  m_spinningDirection);
			//if capsule control
			//m_capsule.setWayPoints(temp, 0);
			//m_capsule.setPrjAngles(temp, 5);


			temp = aMdh.getIceProgramPara(17);
			m_kalmanFilter.setTargetPositionZ((double)temp-150.0);

			m_kalmanFilters[0].setTargetPositionZ((double)temp-150.0);
			m_kalmanFilters[1].setTargetPositionZ((double)temp-150.0);
			m_kalmanFilters[2].setTargetPositionZ((double)temp-150.0);

			ICE_OUT("setTargetPositionZ " <<  temp);
			//if capsule control
			//m_capsule.setWayPoints(temp, 1);

			temp = aMdh.getIceProgramPara(18);
			m_kalmanFilter.setRotorRadiusArm((double)temp/1000.);
			m_kalmanFilters[0].setRotorRadiusArm((double)temp/1000.);
			m_kalmanFilters[1].setRotorRadiusArm((double)temp/1000.);
			m_kalmanFilters[2].setRotorRadiusArm((double)temp/1000.);

			ICE_OUT("RotorRadiusArm " <<  (double)temp/1000.);

			temp = aMdh.getIceProgramPara(19);
			m_kalmanFilter.setTrackingDuration((double)temp/1000000.);
			m_kalmanFilters[0].setTrackingDuration((double)temp/1000000.);
			m_kalmanFilters[1].setTrackingDuration((double)temp/1000000.);
			m_kalmanFilters[2].setTrackingDuration((double)temp/1000000.);
			ICE_OUT("TrackingDuration " <<  (double)temp/1000000.);

			temp = aMdh.getIceProgramPara(20);
			m_kalmanFilter.setMaxGradAmplitude(temp);
			m_kalmanFilters[0].setMaxGradAmplitude(temp);
			m_kalmanFilters[1].setMaxGradAmplitude(temp);
			m_kalmanFilters[2].setMaxGradAmplitude(temp);
			m_capsule.setMaxGradAmplitude(temp);
			m_MPC_Capsules.setMaxGradAmplitude(temp);
			ICE_OUT("MaxGradAmplitude " <<  temp);

			temp = aMdh.getIceProgramPara(21);
			//temp = 1;
			m_kalmanFilter.setRotorStaticFriction((double)temp/100000.);
			m_kalmanFilters[0].setRotorStaticFriction((double)temp/100000.);
			m_kalmanFilters[1].setRotorStaticFriction((double)temp/100000.);
			m_kalmanFilters[2].setRotorStaticFriction((double)temp/100000.);

			ICE_OUT("RotorStaticFriction " <<  (double)temp/100000.);
			//if capsule control
			//m_capsule.setWayPoints(temp, 2);

			temp = aMdh.getIceProgramPara(22);
			//temp = 8;
			m_kalmanFilter.setRotorViscousFriction((double)temp/10000000.);
			m_kalmanFilters[0].setRotorViscousFriction((double)temp/10000000.);
			m_kalmanFilters[1].setRotorViscousFriction((double)temp/10000000.);
			m_kalmanFilters[2].setRotorViscousFriction((double)temp/10000000.);
			ICE_OUT("RotorViscousFriction " << (double)temp/10000000.);
			//if capsule control
			//m_capsule.setWayPoints(temp, 3);

			//double temp2 = 35;
			//m_kalmanFilter.setRotorLoad(-(double)temp2/100000.);
			//ICE_OUT("RotorLoad " << (double)temp2/100000.);
			temp = aMdh.getIceProgramPara(23);
			//if capsule control
			//m_capsule.setWayPoints(temp, 4);

			//temp = 0;
			m_kalmanFilter.setRotorLoad(-(double)temp/100000.);
			ICE_OUT("RotorLoad " << (double)temp/100000.);

			temp = aMdh.getIceProgramPara(23);
			if (temp == 0)
			{
				m_kalmanFilter.setAngleMode(0);
				m_kalmanFilters[0].setAngleMode(0);
				m_kalmanFilters[0].setAngleMode(0);
				m_kalmanFilters[0].setAngleMode(0);
			}
			else
			{
				m_kalmanFilter.setAngleMode(1);
				m_kalmanFilters[0].setAngleMode(1);
				m_kalmanFilters[1].setAngleMode(1);
				m_kalmanFilters[2].setAngleMode(1);

			}
			//if(temp > 0)
			//channelID = temp;
			m_kalmanFilter.setTargetPositionX((double)temp-150.0);
			m_kalmanFilters[0].setTargetPositionX((double)temp-150.0);
			m_kalmanFilters[1].setTargetPositionX((double)temp-150.0);
			m_kalmanFilters[2].setTargetPositionX((double)temp-150.0);

			ICE_OUT("setTargetPositionX " <<  temp);


			ICE_OUT("TimeBetweenCtrlUpdate " << m_kalmanFilter.getTimeBetweenCtrlUpdate());

	}
}


bool ActuationCloseLoop::checkingConsistency()
{
	bool isProtocolOK = true;
	if(~(m_isRotorCTRL ^ m_isCapsuleCTRL))
		isProtocolOK = false;
	return true;
}


void ActuationCloseLoop::findingPeakPosition(IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl)
{
	long lMaxLigne = 0, lMaxCol = 0;
	double theta, rotatedCenter;
	//for(int i = 0; i<iceObjChaLen; i++)

	//for(int i = 0; i<4; i++)
	//{
	if(m_isCircleFound == false) // Rotor circle not yet found. Look at the peak in all the extent of the received data
	{
		if(m_nbRotors > 1) // multiple rotors
		{
			for(int j = 0; j< m_nbRotors; j++)
			{

				if(j == 0)
					magDataFTAs.modify(COL, (long)j*(iceObjColLen/m_nbRotors), (long)(iceObjColLen/m_nbRotors), 1);//,
					//CHA, 10, 1, 1); // CHA, i, 1, 1); channel 21
				else
					magDataFTAs.modify(COL, (long)j*(iceObjColLen/m_nbRotors), (long)(iceObjColLen/m_nbRotors)-1, 1);//,
					//CHA, 5, 1, 1); //  CHA, i, 1, 1);; channel 25

				Ice.CalcStatistics(magDataFTAs, IsmMax);
				Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
				lPeakPos[j] =  lMaxLigne + j*(iceObjColLen/m_nbRotors);
			}
		}
		else
		{
			//ICE_OUT("channelID " <<  channelID );

			magDataFTAs.modify(COL, 0, iceObjColLen, 1);//,
				               //CHA, channelID, 1, 1);
			//magDataFTAs.modify(COL, 0, iceObjColLen, 1);
			Ice.CalcStatistics(magDataFTAs, IsmMax);
			Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
			lPeakPos[0] =  lMaxLigne;
		}
	}

	else //Look at the peak inside the boundaries of the circle
	{
		/*
		if(m_nbRotors > 1) // multiple rotors
		{
			for(int j = 0; j< m_nbRotors; j++)
			{
				if(j == 0)
					magDataFTAs.modify(COL, m_boundariesMinus[j][prjAxis-1], m_boundariesPlus[j][prjAxis-1] - m_boundariesMinus[j][prjAxis-1] , 1);//,
					//CHA, 10, 1, 1); // CHA, i, 1, 1); channel 21
				else
					//magDataFTAs.modify(COL, m_boundariesMinus[j][prjAxis-1], m_boundariesPlus[j][prjAxis-1] - m_boundariesMinus[j][prjAxis-1] , 1,
					//CHA, 5, 1, 1); // CHA, i, 1, 1); channel 25

				Ice.CalcStatistics(magDataFTAs, IsmMax);
				Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
				lPeakPos[j] =  lMaxLigne + m_boundariesMinus[j][prjAxis-1];

			}

		}
		else
		{
		*/
			//If simultaneous actuation and imaging, transform the boundaries values in the rotation frame
			if(switchControl == SIMULTANEOUS) 
			{
				theta = m_kalmanFilter.getThetaProjection();
				rotatedCenter = applyRotationMatrix(theta, m_center[0][0], m_center[0][1]);

				float padding = 3.0; // 3mm

			    long boundariesMinus =  static_cast <long> ((((rotatedCenter - (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);// RP for Rotation Plane.
				long boundariesPlus  =  static_cast <long> ((((rotatedCenter + (m_radius[0] + padding)) / 600.0) + 0.5)*(double)iceObjColLen);// RP for Rotation Plane.
				
				ICE_OUT("theta " << theta << " m_boundariesMinus[0][0] " <<m_boundariesMinus[0][0] << m_boundariesMinus[0][1]); 
				ICE_OUT("theta " << theta << " m_boundariesPlus [0][0] " << m_boundariesPlus[0][0] <<  m_boundariesPlus[0][1]); 
				ICE_OUT("boundariesMinus " << boundariesMinus << " boundariesPlus " <<boundariesPlus);
				ICE_OUT("lPeakPos " << lPeakPos[0]);
				magDataFTAs.modify(COL, boundariesMinus, boundariesPlus - boundariesMinus , 1);
				
				//magDataFTAs.modify(COL, m_boundariesMinus[0][prjAxis-1], m_boundariesPlus[0][prjAxis-1] - m_boundariesMinus[0][prjAxis-1] , 1,
				//CHA, channelID, 1, 1);
				//magDataFTAs.modify(COL, 0, iceObjColLen, 1); // DEBUG. Look for the peak everywhere
				Ice.CalcStatistics(magDataFTAs, IsmMax);
				Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
				lPeakPos[0] =  lMaxLigne + boundariesMinus;
				//lPeakPos[0] =  lMaxLigne; // DEBUG. Look for the peak everywhere
				//magDataFTAs.modify(COL, 0, iceObjColLen, 1);
				//Ice.WriteToFile(magDataFTAs, "ProjectionData", 0, "ab");
			}
			else
			{
				magDataFTAs.modify(COL, m_boundariesMinus[0][prjAxis-1], m_boundariesPlus[0][prjAxis-1] - m_boundariesMinus[0][prjAxis-1] , 1);
				//magDataFTAs.modify(COL, m_boundariesMinus[0][prjAxis-1], m_boundariesPlus[0][prjAxis-1] - m_boundariesMinus[0][prjAxis-1] , 1,
				//CHA, channelID, 1, 1);

				Ice.CalcStatistics(magDataFTAs, IsmMax);
				Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
				lPeakPos[0] =  lMaxLigne + m_boundariesMinus[0][prjAxis-1];
			}
		//}
	}
	//}
}

// construct correlation mask function and find the peak position. This function needs to be transformed into a C++ object.

void ActuationCloseLoop::constructCorrelationMask(IceAs& srcCombineAs, IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl)
{
	long lMaxLigne = 0, lMaxCol = 0, lMinLigne = 0, lMinCol = 0;

	double tabX[] = {0.00282007,0.00337736,0.00365536,0.0028097,0.00251336,0.00267304,0.00243657,0.0036839,0.00250654,0.00249134,0.00260886,0.00247185,0.00397869,0.00256181,0.00291847,0.0019268,0.00263587,0.00309597,0.00202794,0.00275272,0.00210529,0.00256076,0.00212158,0.002359,0.00313773,0.00293057,0.00289057,0.00222355,0.00241493,0.00243947,0.00289444,0.00289306,0.0023052,0.00295574,0.00301326,0.00252462,0.00297739,0.00229342,0.00209139,0.00257842,0.00224532,0.00325928,0.00202122,0.00232827,0.00247049,0.00277974,0.00216461,0.00251196,0.00160429,0.00216942,0.00178957,0.00264825,0.00240725,0.00190131,0.00267186,0.00213431,0.00208814,0.0025191,0.00277611,0.00244354,0.00248195,0.00191741,0.0023325,0.00287179,0.00196624,0.00342021,0.00431577,0.00242011,0.00227584,0.00224472,0.00308333,0.00243138,0.0026772,0.00307375,0.00289776,0.0026489,0.00264241,0.00230021,0.00218564,0.0025328,0.00318288,0.00383054,0.0022455,0.0021756,0.00290671,0.0031192,0.00264532,0.00252744,0.0027538,0.00385525,0.00234396,0.00245343,0.00296432,0.0038196,0.00387631,0.00309813,0.00370744,0.002892,0.00332369,0.00326018,0.00322326,0.00311245,0.00442851,0.0036549,0.00350149,0.00328389,0.00332859,0.00305476,0.00289997,0.00316785,0.0040426,0.00412715,0.00328746,0.004483,0.00409872,0.00458281,0.00399625,0.00329244,0.00421416,0.00429435,0.0036707,0.00437908,0.00363855,0.00278398,0.00553627,0.00366869,0.00338955,0.00425848,0.00331707,0.00289791,0.00268375,0.00474112,0.00337879,0.00338109,0.00310374,0.00342919,0.00312188,0.00531753,0.00549914,0.00508174,0.00372305,0.00403553,0.00472879,0.00435582,0.00397803,0.00414364,0.00514468,0.00552246,0.00481082,0.00656431,0.00460549,0.00322969,0.00481892,0.00511309,0.0043095,0.00433578,0.005097,0.00371598,0.00580981,0.00424443,0.00515539,0.00462094,0.00546854,0.00368913,0.00589005,0.00468257,0.00644639,0.00513355,0.00448833,0.00628348,0.0039921,0.0046803,0.00504252,0.00498842,0.00515877,0.00563792,0.00413224,0.0045102,0.00453315,0.00504318,0.00391264,0.00538239,0.00617502,0.00518774,0.00625962,0.00545732,0.00543054,0.00629208,0.00467689,0.00591351,0.00650533,0.00577454,0.00515844,0.00517445,0.00662308,0.00649753,0.00799641,0.00589092,0.00688788,0.00653192,0.00563818,0.00402455,0.00512587,0.0058279,0.00588628,0.00497707,0.00470383,0.00498285,0.00582612,0.00652278,0.00627934,0.00451742,0.00585215,0.00635478,0.00576551,0.00528514,0.00655497,0.00509781,0.0065588,0.00484842,0.00626682,0.00643586,0.00588301,0.00589075,0.00444861,0.00564553,0.00507378,0.00683045,0.00698284,0.00445482,0.0063355,0.00714275,0.00529559,0.00613102,0.00562531,0.00600233,0.00870774,0.0063135,0.00542617,0.00901319,0.00668152,0.00551382,0.00558694,0.00596003,0.00489976,0.00540796,0.00751005,0.00611864,0.00799947,0.00613357,0.00713526,0.00485304,0.00788694,0.0075351,0.00577199,0.00746887,0.00505828,0.00927303,0.00547827,0.006416,0.00566404,0.00599788,0.00717699,0.00771642,0.00572882,0.00659691,0.00692147,0.0114289,0.00853063,0.0081227,0.0102626,0.0103916,0.00530305,0.00629481,0.00642895,0.00581663,0.00692474,0.00490553,0.00695133,0.00785755,0.00540681,0.00519287,0.00689513,0.00558053,0.0053565,0.00538177,0.00558093,0.00599967,0.00634582,0.00693022,0.00546743,0.00726695,0.00698495,0.00586041,0.00649301,0.00710116,0.00706976,0.0052649,0.00648592,0.00526355,0.00676089,0.00619643,0.00669598,0.00653297,0.00642481,0.00565817,0.00599548,0.00802537,0.0058912,0.00398216,0.00518447,0.00820272,0.00867474,0.00499278,0.00709947,0.007228,0.00557257,0.00595033,0.00704541,0.00498182,0.00480537,0.00656143,0.00805101,0.0077512,0.00734373,0.00803879,0.00826182,0.00679412,0.00713833,0.00852112,0.00836984,0.00723322,0.007609,0.00666252,0.00670119,0.00571711,0.00639,0.00545735,0.0068206,0.00809965,0.00655152,0.00539666,0.0061777,0.00736942,0.00798403,0.00742323,0.00622258,0.00523297,0.00603254,0.00707453,0.00685456,0.00751908,0.0070418,0.00720096,0.00636229,0.00917012,0.00606187,0.00479437,0.00734157,0.00709134,0.00860009,0.00633227,0.00673786,0.00769177,0.00620617,0.0065308,0.00763861,0.00658866,0.00528098,0.00796037,0.00846544,0.00510616,0.00566534,0.00636296,0.00664239,0.00591959,0.00659012,0.00677228,0.00679802,0.00549762,0.00710633,0.00578933,0.00684115,0.00801567,0.00628701,0.00491681,0.00656792,0.00656928,0.00647453,0.00617918,0.00684681,0.00820493,0.00646606,0.00672551,0.00909988,0.00630945,0.00574396,0.00576353,0.00499515,0.00710009,0.00647514,0.00731886,0.00722941,0.0071479,0.00581585,0.0057391,0.00638725,0.0054038,0.00577336,0.0069015,0.00769327,0.00696971,0.0064643,0.00748153,0.00534648,0.00669107,0.00712927,0.00766991,0.0057057,0.00680696,0.00834324,0.00583805,0.00557754,0.00745497,0.00706592,0.00855307,0.00734185,0.00522898,0.00776827,0.00854136,0.00637432,0.00653086,0.00505372,0.00610146,0.0064368,0.00767035,0.00539163,0.00768674,0.00668587,0.00929382,0.00766743,0.00776596,0.00676489,0.00626063,0.00787024,0.00574088,0.0067677,0.00711625,0.00733049,0.00765854,0.00878773,0.00651672,0.00732812,0.0067636,0.00623193,0.00785795,0.00646507,0.00561542,0.00842823,0.00605616,0.00647026,0.00629701,0.00680891,0.00690833,0.006457,0.00640554,0.00929514,0.00603367,0.00831324,0.00710621,0.00628477,0.00619107,0.00797394,0.00532877,0.0103612,0.0187577,0.029045,0.0464244,0.0668038,0.0875485,0.110902,0.133603,0.154227,0.17042,0.184868,0.193928,0.204681,0.211647,0.214687,0.218201,0.220687,0.224,0.22664,0.229813,0.233171,0.231407,0.233069,0.235492,0.237561,0.241067,0.241982,0.242463,0.243091,0.244668,0.243042,0.242871,0.246023,0.245783,0.24544,0.244369,0.245039,0.249308,0.246838,0.245071,0.239592,0.247473,0.246198,0.241647,0.240363,0.241479,0.239547,0.236905,0.236954,0.232873,0.229802,0.22685,0.220611,0.225412,0.218494,0.219775,0.212419,0.214148,0.208693,0.206222,0.199726,0.19847,0.196669,0.191002,0.190735,0.185481,0.180222,0.171733,0.161436,0.142505,0.120271,0.0896872,0.0602376,0.0351664,0.0193907,0.0102635,0.0087309,0.0062316,0.00805667,0.00769241,0.00593255,0.00595683,0.00618646,0.00795872,0.00629828,0.00637445,0.0064978,0.00637378,0.00561244,0.00775352,0.00707128,0.00663835,0.00636387,0.00668008,0.00708433,0.00696467,0.00555678,0.00680536,0.00612869,0.00824898,0.00775823,0.00945949,0.00782319,0.00591066,0.00729479,0.00736407,0.00754969,0.00641987,0.00542978,0.00525205,0.00532521,0.00668453,0.00667982,0.00536112,0.0064198,0.00700072,0.00670339,0.00533124,0.0061443,0.00575225,0.00680415,0.00611331,0.00642401,0.00885473,0.00665114,0.00811218,0.00649539,0.00584737,0.00512743,0.00708665,0.00676977,0.0059952,0.00767442,0.00670972,0.00541918,0.00719433,0.00777843,0.00500979,0.00701251,0.00473505,0.00783215,0.00748164,0.00620304,0.00760339,0.00774556,0.00623415,0.00546343,0.00596452,0.00561161,0.00745216,0.00612954,0.00616798,0.00521812,0.00640779,0.00777492,0.00518796,0.0066067,0.00859896,0.00641258,0.00661757,0.00559772,0.00575152,0.0100208,0.00652081,0.00637323,0.0065508,0.00880872,0.00821802,0.00591887,0.00794503,0.00534644,0.00638284,0.00653294,0.00731209,0.00791603,0.00600687,0.0063887,0.00723653,0.00754875,0.00660428,0.00699844,0.00624812,0.00525,0.00704191,0.00741738,0.00615739,0.00500153,0.00642116,0.00797426,0.00737648,0.00658311,0.00570861,0.00605968,0.00493688,0.00686695,0.00729591,0.00612586,0.00697474,0.00573347,0.00625937,0.00611006,0.00623918,0.00705231,0.0076247,0.00548224,0.00839337,0.00571215,0.0070041,0.00554826,0.00526928,0.0055621,0.00524639,0.00560943,0.00570658,0.00518431,0.00563021,0.00653884,0.0073621,0.00721497,0.00517588,0.00646337,0.00669154,0.00608792,0.00678582,0.00689076,0.0054934,0.00651127,0.00647042,0.00537226,0.00774662,0.0075749,0.0064545,0.00729716,0.00670482,0.00794913,0.00599493,0.00826226,0.00696558,0.00593299,0.00698241,0.00676617,0.00731132,0.00716817,0.00722395,0.00683444,0.00598008,0.00698945,0.00454241,0.0060656,0.00825467,0.00916247,0.00674782,0.0053974,0.00772478,0.00560154,0.00565288,0.00661752,0.00822941,0.00933218,0.00704379,0.00641213,0.0076549,0.00476597,0.0061927,0.00724572,0.00602173,0.00670838,0.00715615,0.00759506,0.00701779,0.00593501,0.00804964,0.0072632,0.00534343,0.0060178,0.00585384,0.00977923,0.00715344,0.00734937,0.0098588,0.00651929,0.00700509,0.00529548,0.00590264,0.00679822,0.00430605,0.00653991,0.00930462,0.00685457,0.00577028,0.00627099,0.00911277,0.00662755,0.00690216,0.0079271,0.00610695,0.00635479,0.00852256,0.00733881,0.00887358,0.00770309,0.00701555,0.00807385,0.00513143,0.00697566,0.0067748,0.0089018,0.00638858,0.00757622,0.0069111,0.00797353,0.00718483,0.00677703,0.00633054,0.00653617,0.00656249,0.00771785,0.00818169,0.00579751,0.00965947,0.00570701,0.00555219,0.00597772,0.00598467,0.00697832,0.0090009,0.00670079,0.00477011,0.00653386,0.00777314,0.00740185,0.00562343,0.008839,0.00852255,0.00515548,0.00609074,0.00712647,0.0082595,0.00543913,0.00766759,0.00653129,0.00620543,0.00628652,0.00923369,0.00617759,0.00738634,0.00681058,0.00873836,0.00800006,0.00646961,0.00823105,0.00749053,0.00792835,0.0072184,0.00536687,0.00879713,0.00725233,0.00732949,0.00716725,0.00825439,0.00597727,0.00690202,0.00669218,0.00731167,0.00703307,0.00720027,0.00788068,0.00659948,0.00611643,0.00776561,0.00767781,0.00483885,0.00718279,0.00805342,0.00682276,0.00557457,0.00796245,0.0052894,0.00974528,0.00682674,0.0060135,0.00818772,0.0057004,0.00561653,0.00587863,0.00617598,0.00426107,0.00631795,0.00731809,0.00527488,0.00717617,0.00703733,0.00754811,0.00675854,0.00452165,0.00819115,0.00479122,0.00567645,0.00846431,0.00892247,0.00634563,0.00556684,0.00637996,0.00782614,0.00791232,0.0050724,0.00806725,0.00600381,0.00857456,0.00649243,0.0073715,0.0056472,0.0069654,0.00661479,0.0069076,0.00754886,0.00682334,0.00638522,0.00901293,0.00651755,0.00613359,0.00467821,0.00661146,0.00755444,0.00656785,0.00571606,0.0057573,0.00530419,0.00479221,0.00588922,0.0060143,0.00747686,0.00619273,0.00565157,0.00474784,0.00558199,0.00512704,0.00663282,0.00657449,0.00667983,0.00662789,0.00780254,0.00673927,0.00483268,0.00541669,0.00626936,0.00781157,0.0052093,0.00643759,0.00597999,0.00631569,0.00610736,0.00397131,0.00510585,0.00546685,0.00506632,0.00540155,0.00535437,0.00519775,0.00644518,0.0049542,0.00502034,0.00564442,0.00474195,0.0053266,0.00491264,0.00481203,0.00534637,0.00795282,0.00454286,0.00535006,0.00540726,0.00467836,0.00420402,0.00414742,0.00558968,0.00396264,0.00378519,0.00483136,0.00503071,0.00516888,0.00702291,0.00408338,0.00422629,0.00416828,0.0036598,0.0052453,0.00513152,0.00358823,0.0046908,0.00573555,0.0033221,0.00417309,0.00378836,0.00429232,0.00393817,0.00628762,0.00564056,0.00545968,0.00504121,0.00445309,0.00472382,0.00438727,0.00376657,0.00467943,0.00417624,0.00476268,0.00443945,0.00456926,0.00386669,0.00413698,0.0031859,0.00449778,0.00448654,0.00430665,0.00419531,0.00388035,0.00455843,0.00452189,0.00474853,0.0040115,0.00426812,0.00350466,0.00369914,0.00340836,0.00436439,0.00427269,0.00378824,0.0035802,0.00326259,0.00323816,0.0030345,0.00326404,0.00359181,0.00398476,0.00372545,0.00300985,0.00323398,0.00369669,0.00331912,0.00340958,0.00386845,0.00418103,0.00364105,0.00313053,0.00293305,0.00319704,0.00277005,0.00429416,0.00510055};

	double tabZ[] = {0.0122743,0.0142549,0.0112546,0.0133403,0.0168409,0.0150156,0.0141795,0.0181176,0.015049,0.0110044,0.013906,0.0185391,0.0128289,0.0208443,0.0146079,0.0166665,0.0181489,0.0179056,0.0111051,0.0154016,0.016985,0.0155723,0.0142854,0.0189962,0.0139417,0.0145511,0.0120169,0.0158249,0.0114291,0.0168087,0.0131697,0.0170029,0.0146883,0.013218,0.0201814,0.0137407,0.0112789,0.0121283,0.0142806,0.0135461,0.0167973,0.0137166,0.0139094,0.0138748,0.01215,0.0139686,0.0155636,0.0153737,0.0139755,0.0113277,0.012714,0.0142187,0.0105622,0.0162011,0.0163426,0.0155434,0.0171304,0.0118709,0.0113969,0.0142322,0.013659,0.0151927,0.0112987,0.0151945,0.0149712,0.0148102,0.0146716,0.0183956,0.017958,0.0124077,0.0160147,0.0118028,0.014966,0.0142435,0.0130633,0.0129611,0.0119375,0.0180579,0.0143177,0.0149166,0.0107544,0.019935,0.0131568,0.0141361,0.0125359,0.0175082,0.0141527,0.017387,0.0135375,0.012684,0.0194164,0.0144365,0.014593,0.0132237,0.0119813,0.0109546,0.0184286,0.0167419,0.0163146,0.0169445,0.0133487,0.0145256,0.0156505,0.0182364,0.013464,0.00876335,0.0143071,0.0139537,0.0146371,0.0143252,0.0157969,0.0168592,0.0142877,0.0174783,0.0227604,0.0125351,0.0160755,0.014294,0.0132237,0.0130043,0.0133971,0.0135512,0.0115231,0.0117869,0.0136671,0.0130172,0.0137581,0.0190944,0.0140711,0.0140216,0.0122527,0.0158126,0.0118078,0.0117006,0.0134485,0.0130164,0.0207002,0.0152433,0.0148722,0.016624,0.012757,0.0143474,0.0120761,0.0109341,0.0143768,0.0120474,0.0164478,0.0153186,0.0123714,0.017437,0.0146428,0.0116327,0.0136592,0.0148776,0.0158151,0.0107909,0.0174145,0.0148663,0.0114325,0.0156517,0.0179826,0.0147933,0.0137476,0.0119532,0.0163856,0.0141999,0.0129434,0.0157931,0.014113,0.0130627,0.0155032,0.0128831,0.0142239,0.0149769,0.0188752,0.0131045,0.0174861,0.0151108,0.0153183,0.0132756,0.0191782,0.0131265,0.0136923,0.0150986,0.0179663,0.0169124,0.0118947,0.0149915,0.0126214,0.0123701,0.0184337,0.0144322,0.0151599,0.0181106,0.013431,0.0114882,0.0148323,0.010909,0.0134532,0.0177819,0.0163833,0.0153423,0.0135958,0.0112103,0.0128435,0.0156278,0.0154903,0.012529,0.0121333,0.0151046,0.0125927,0.0122914,0.0161371,0.0103641,0.0150784,0.0165263,0.0158673,0.011691,0.016735,0.0171746,0.0155589,0.0191539,0.014941,0.0134565,0.0171987,0.0157851,0.0133655,0.0202794,0.0114855,0.0171044,0.0187221,0.0128451,0.0177417,0.015524,0.0192231,0.018493,0.014841,0.0141885,0.0132118,0.010587,0.0141349,0.0185912,0.0188232,0.0191988,0.0143841,0.016221,0.012433,0.0198391,0.0152063,0.0146579,0.0141676,0.0136422,0.0113023,0.0179742,0.016383,0.0145568,0.0150844,0.0173791,0.0167669,0.0248514,0.0132804,0.0129491,0.0128407,0.0167418,0.0260945,0.0160445,0.0147104,0.0176275,0.0163665,0.00993061,0.0127715,0.0138196,0.0118146,0.0138627,0.0133433,0.0126663,0.0151796,0.0220034,0.0164313,0.0132089,0.0181599,0.0159294,0.0160885,0.0198055,0.0156824,0.0125541,0.0139926,0.0118255,0.0152128,0.016351,0.0110806,0.0142317,0.0127328,0.0117082,0.0184077,0.0119198,0.0123529,0.0193417,0.0132313,0.0138136,0.0149604,0.00993912,0.0159792,0.013451,0.0152998,0.0133343,0.0126184,0.013051,0.0138346,0.0161497,0.018945,0.0138377,0.0140044,0.018816,0.0136405,0.0142637,0.011569,0.0192614,0.0174856,0.0170218,0.014123,0.012249,0.0144907,0.0139427,0.0103686,0.0132861,0.0132629,0.012575,0.0112505,0.0146676,0.0155406,0.0144449,0.0103756,0.0138292,0.0145474,0.0125151,0.012121,0.013902,0.0175582,0.0160307,0.0150586,0.0148797,0.0130893,0.0156429,0.0127892,0.0131711,0.0165198,0.0137803,0.0103987,0.013533,0.0163929,0.0142157,0.0145645,0.0123248,0.0126051,0.011528,0.0157734,0.0161813,0.0124081,0.0142749,0.0156119,0.0166805,0.0128063,0.0166535,0.0143757,0.0134227,0.0146824,0.0162662,0.0110997,0.0182942,0.0097867,0.0127432,0.0158042,0.0130111,0.0137946,0.0135856,0.0129014,0.0129124,0.0123459,0.0160499,0.0168829,0.014646,0.0107896,0.0130008,0.0161325,0.0130031,0.0135075,0.0153024,0.0142824,0.0194838,0.0150352,0.013907,0.011532,0.0169133,0.0171187,0.0140826,0.0186016,0.014458,0.0121287,0.0123179,0.0171849,0.0119674,0.0146864,0.0166863,0.0163069,0.0156454,0.0158438,0.0119867,0.0142423,0.0135355,0.0161761,0.015881,0.0172061,0.0147312,0.0145294,0.016805,0.0157813,0.0185042,0.0118566,0.0137295,0.0129854,0.0152936,0.0151352,0.0101553,0.0176162,0.0132034,0.0165599,0.0151992,0.0148714,0.0143882,0.0116039,0.0131671,0.0164243,0.0170115,0.0188986,0.0124713,0.0146418,0.0174322,0.012382,0.0179178,0.0137403,0.0127727,0.0141148,0.0156143,0.0151876,0.0159521,0.0102989,0.0131865,0.0173192,0.0178711,0.0145814,0.0151599,0.0143108,0.0165597,0.0190025,0.0194928,0.0159054,0.0139405,0.0181048,0.0159788,0.0117518,0.0130904,0.0140301,0.0115117,0.0137411,0.0127498,0.0177678,0.0174003,0.0217609,0.030401,0.0702949,0.210401,0.472994,0.916892,1.43647,1.92855,2.23206,2.2997,2.23244,2.12566,2.07705,2.04933,2.06976,2.05339,2.05336,2.05584,2.05976,2.04369,2.04866,2.0558,2.05697,2.02389,2.02685,1.99821,2.03061,1.96016,1.88991,1.7983,1.76301,1.63743,1.57071,1.4818,1.42753,1.38517,1.29407,1.27058,1.22192,1.19063,1.15722,1.10874,1.12025,1.08581,1.06977,1.06574,1.05959,1.03719,1.04299,1.02626,1.04115,1.02824,1.029,1.03397,1.02639,1.02851,1.05047,1.05446,1.0762,1.08669,1.09316,1.10485,1.10691,1.10488,1.07683,1.03892,0.98276,0.890005,0.783547,0.655681,0.510663,0.374514,0.267771,0.150663,0.089953,0.0427166,0.026104,0.0173691,0.0173818,0.0147602,0.0187892,0.011843,0.0157278,0.0137068,0.0154587,0.0198998,0.0125602,0.0114702,0.0185796,0.0122815,0.0116508,0.0121636,0.0172144,0.0137459,0.0167122,0.0149391,0.0136386,0.0145812,0.0140509,0.0125244,0.0104737,0.013352,0.0105886,0.0134686,0.0133382,0.012562,0.0115011,0.0146686,0.0127579,0.0155192,0.0100261,0.013843,0.0145013,0.0131534,0.0111407,0.0132067,0.0163021,0.0139336,0.0128381,0.0140764,0.0126695,0.0145408,0.0148235,0.00910284,0.0110312,0.0103875,0.0121287,0.0135706,0.00979531,0.0122463,0.00965429,0.0131535,0.0115222,0.00899767,0.0113144,0.0114915,0.0122502,0.011589,0.00960845,0.0121203,0.00884409,0.0112326,0.00995832,0.00942075,0.0077586,0.010843,0.014687,0.00848851,0.00757008,0.0093519,0.00877513,0.00927329,0.00944222,0.0129439,0.00881199,0.0104306,0.0098068,0.00988024,0.0075047,0.0116953,0.00743144,0.015294,0.010068,0.00870907,0.00717332,0.0122427,0.0102555,0.00811405,0.00877179,0.00918868,0.0112974,0.00645322,0.0100951,0.00712839,0.00849417,0.0089005,0.00862304,0.0074514,0.00854338,0.0083182,0.00944983,0.00839379,0.00844821,0.00768313,0.00729585,0.00778774,0.00682498,0.00877864,0.00719181,0.00469431,0.00966175,0.00994466,0.0098734,0.0101062,0.00617236,0.0096203,0.00961331,0.00736557,0.00777142,0.00877559,0.00961303,0.00843719,0.00746802,0.00649351,0.00717364,0.00776525,0.00625048,0.00745476,0.00630075,0.00525334,0.00801318,0.0076812,0.00585561,0.0053096,0.00436269,0.00590411,0.00684688,0.00527676,0.00493866,0.00616801,0.00535862,0.00514559,0.00547138,0.00540078,0.00744696,0.00624969,0.00583037,0.00618976,0.00650619,0.00668236,0.00604742,0.00628057,0.00652525,0.00651522,0.00734404,0.0041935,0.00617072,0.00542354,0.00406965,0.00703172,0.00585479,0.00558901,0.00474048,0.00485832,0.00502301,0.0038725,0.00370327,0.00538896,0.00552979,0.00555899,0.00507305,0.00470684,0.00562088,0.00536781,0.00677559,0.00561338,0.00462964,0.00526611,0.00326334,0.00420835,0.00476917,0.00397741,0.00457477,0.00527392,0.00580598,0.00512289,0.00531264,0.003295,0.0056995,0.00413862,0.00652106,0.00485664,0.00448244,0.00532141,0.00435371,0.00496625,0.00476247,0.00412151,0.00472749,0.0045914,0.00597702,0.00589261,0.00368654,0.00546862,0.00565547,0.00581862,0.00387107,0.00520203,0.00546459,0.00586658,0.00449364,0.00624799,0.0060529,0.00824801,0.00502966,0.00659448,0.00503585,0.00534424,0.00667992,0.00612585,0.00809034,0.00723832,0.00611263,0.00493973,0.0063638,0.0078536,0.00539898,0.00587218,0.00700695,0.00636056,0.00562088,0.0063586,0.00717701,0.00827525,0.00683458,0.00617908,0.00768587,0.00737481,0.00836364,0.00777358,0.00774312,0.00962832,0.00884494,0.00787938,0.00833649,0.00589751,0.00877099,0.00834321,0.0101783,0.0088724,0.0052206,0.00908346,0.00774711,0.00708837,0.00675778,0.00590596,0.00909657,0.00738485,0.00765758,0.0077462,0.00783694,0.00775516,0.009504,0.0106346,0.00938692,0.00790641,0.00789085,0.0102397,0.00959468,0.00677049,0.010837,0.00794969,0.00945288,0.00855309,0.00912697,0.0115699,0.00867046,0.0100317,0.00946667,0.0095172,0.00954297,0.00821746,0.0106112,0.00808773,0.00970167,0.00654617,0.00830747,0.0103224,0.00804762,0.00807718,0.00875729,0.00947792,0.00965833,0.0092805,0.00755775,0.0109619,0.00999485,0.0112339,0.00986259,0.0132297,0.0102876,0.0108117,0.00747528,0.0105946,0.0107747,0.0105952,0.010082,0.0137231,0.0101505,0.0118378,0.0115726,0.0103788,0.0122078,0.00871822,0.0104704,0.0103178,0.0134979,0.00991032,0.0131465,0.0113913,0.0130713,0.00808447,0.0121498,0.0130049,0.0131852,0.011833,0.0117161,0.0123704,0.0122534,0.0118441,0.0121862,0.0125239,0.0134113,0.015778,0.0112556,0.0135966,0.0119003,0.0131612,0.0189545,0.0106302,0.0122324,0.0100209,0.0115988,0.0124505,0.0130511,0.0107288,0.0146353,0.0121086,0.0111947,0.0121732,0.0114231,0.00964978,0.0108783,0.0118028,0.0100519,0.0138225,0.0126835,0.0176505,0.0172347,0.0146349,0.0135781,0.0131796,0.0119881,0.0103506,0.0155585,0.013748,0.0187309,0.0135986,0.0123575,0.00997691,0.0141607,0.0112945,0.0119143,0.0119165,0.011207,0.0161427,0.0117418,0.0136157,0.011916,0.0136457,0.0171481,0.011285,0.0110836,0.0202767,0.0110969,0.0143501,0.014286,0.0114804,0.0146862,0.0150888,0.0151882,0.0143146,0.0143956,0.0146829,0.0167835,0.0150034,0.0149036,0.0150027,0.0112464,0.0197311,0.0172194,0.0132266,0.0125499,0.0114103,0.0112969,0.0141546,0.0106525,0.0135082,0.0146001,0.0150919,0.0169444,0.0140221,0.0119605,0.0130254,0.0124515,0.0147459,0.0139281,0.00973829,0.0149462,0.015379,0.0169267,0.0195633,0.0096477,0.0209731,0.0130334,0.0131528,0.0143222,0.0139126,0.0118152,0.0160025,0.0168554,0.0171654,0.0148936,0.0133003,0.0128038,0.0171457,0.012676,0.0140166,0.017918,0.0146783,0.018552,0.0154192,0.0157805,0.0179003,0.0160947,0.0131209,0.0152952,0.0143198,0.0154766,0.0160216,0.0145723,0.0143827,0.0161758,0.0192676,0.0120836,0.0119117,0.0146242,0.0171784,0.0115797,0.0180688,0.017225,0.0132306,0.0143603,0.0131835,0.0121512,0.011589,0.012548,0.0149046,0.0157248,0.0187702,0.0167206,0.0139801,0.0158939,0.00993415,0.0135307,0.0141788};


	//double tabPrj1[] = {0.00606835,0.00473401,0.00463284,0.00361343,0.0046144,0.00523022,0.00443026,0.00466712,0.0036957,0.00441471,0.00407636,0.00352467,0.00420053,0.00554965,0.00463314,0.00274962,0.003774,0.00449496,0.00514698,0.00389203,0.00501435,0.00597383,0.00687835,0.00576727,0.00512573,0.00443537,0.00641505,0.00471783,0.0048733,0.00580523,0.00550183,0.00625446,0.00490195,0.00598856,0.0039186,0.00782334,0.00333564,0.00618333,0.00589051,0.00507567,0.00906068,0.00671991,0.00590287,0.003829,0.00518899,0.00499849,0.0044284,0.00515166,0.00545402,0.00752557,0.00600893,0.00530796,0.00550837,0.00528903,0.00477531,0.0065157,0.00483296,0.00631731,0.0071139,0.00674917,0.00688206,0.0055388,0.0081134,0.00798074,0.00754497,0.00785901,0.00542635,0.00644389,0.00466708,0.00736871,0.00988315,0.00670234,0.00959598,0.00949069,0.00517504,0.00496849,0.00764711,0.0117488,0.00535023,0.00917525,0.00660926,0.0118435,0.00699512,0.00695977,0.00667086,0.00846605,0.00531039,0.0111069,0.00653456,0.00470437,0.00809138,0.0052421,0.00762618,0.00775372,0.00650989,0.00817837,0.00839497,0.00899141,0.00827016,0.00863626,0.0104171,0.00531797,0.00859032,0.00849558,0.00751746,0.0090432,0.00679135,0.00697917,0.0093479,0.00912699,0.0108849,0.00887439,0.00755216,0.011712,0.00844594,0.00721662,0.00750353,0.00774065,0.0107156,0.011708,0.0125015,0.0102112,0.0101075,0.00910314,0.00696061,0.0102258,0.00923497,0.00897737,0.0134538,0.00846908,0.0108386,0.0122578,0.0110022,0.00648193,0.00905223,0.0118864,0.0133851,0.00953049,0.00774088,0.0123571,0.0126782,0.0140908,0.00834523,0.0115839,0.00890676,0.00918507,0.0100187,0.00879547,0.012561,0.00885137,0.0113474,0.0152772,0.0119531,0.00763915,0.0127125,0.0103254,0.0179108,0.011135,0.00800021,0.0132678,0.0154345,0.0120261,0.0094612,0.0123902,0.0109394,0.0107727,0.00962981,0.0156403,0.01049,0.00949677,0.0119507,0.0125407,0.00776077,0.0135759,0.0144233,0.01281,0.0148388,0.0128694,0.0129777,0.011379,0.0154269,0.00686031,0.0124175,0.0163553,0.0119235,0.0170152,0.0146771,0.0108778,0.0103869,0.0140489,0.0116525,0.0129963,0.0185217,0.0114082,0.0130715,0.0179698,0.015507,0.0137891,0.0130928,0.0106297,0.00975083,0.0159918,0.0114469,0.0152364,0.0135701,0.0139432,0.0117511,0.0109194,0.00909868,0.0128322,0.0094329,0.0117394,0.0136877,0.0140536,0.00933297,0.0114515,0.0159019,0.0189826,0.0163536,0.0156692,0.0104506,0.0119596,0.00720269,0.0108765,0.0168239,0.0104161,0.0110337,0.00947601,0.0138729,0.0148075,0.00866653,0.015838,0.0143952,0.0131542,0.0120374,0.0159875,0.0112861,0.0139639,0.0165255,0.014276,0.0131707,0.0180352,0.0153626,0.0139374,0.0197275,0.0222178,0.0105752,0.0140968,0.0123618,0.0144937,0.00977819,0.0118993,0.0166136,0.0143683,0.0150383,0.0135873,0.0136971,0.0110316,0.0156599,0.0146734,0.0162244,0.0151271,0.0158785,0.0142242,0.0103744,0.0133046,0.0139751,0.0140296,0.0141855,0.0107656,0.0107814,0.011895,0.0137528,0.0126142,0.0128015,0.00831648,0.0133251,0.0131186,0.0190047,0.0126824,0.016099,0.0131306,0.0138351,0.0175608,0.00902191,0.0143334,0.0116899,0.00993348,0.0150417,0.0117825,0.0106877,0.00954776,0.0122994,0.0121093,0.011703,0.0132537,0.0109378,0.0117138,0.0132096,0.0140163,0.011578,0.0165854,0.0115854,0.0130739,0.0125884,0.0134462,0.0168539,0.0145875,0.011678,0.0128729,0.0107712,0.0182623,0.0176638,0.0157682,0.00917573,0.0131677,0.0122408,0.0195706,0.0125868,0.0129624,0.0154459,0.0194663,0.0103715,0.00876983,0.0206988,0.0184495,0.0132571,0.0246132,0.0401819,0.0700078,0.100576,0.132428,0.204774,0.256155,0.358664,0.429064,0.522907,0.610818,0.708599,0.771787,0.833486,0.854012,0.873829,0.868335,0.852903,0.832236,0.792963,0.760786,0.71291,0.680811,0.627772,0.583642,0.544548,0.516428,0.479748,0.450256,0.427728,0.394042,0.368946,0.331566,0.298945,0.264012,0.236699,0.20112,0.160799,0.127037,0.0955054,0.06613,0.040971,0.024104,0.0182129,0.0101799,0.0120164,0.0148584,0.0122446,0.0129433,0.0156016,0.0144269,0.0110814,0.0108478,0.0178117,0.011324,0.0106541,0.0161675,0.01156,0.0306231,0.0507112,0.0803899,0.10218,0.149177,0.209204,0.255933,0.324744,0.38537,0.45162,0.516683,0.581326,0.657409,0.728677,0.795455,0.878259,0.954655,1.03506,1.10549,1.2095,1.30439,1.40842,1.4871,1.58907,1.73176,1.86361,1.94924,2.05544,2.15809,2.27837,2.31624,2.49608,2.70707,2.97909,2.70024,0.772875,0.120174,0.0203755,0.0313193,0.0102985,0.029806,0.0107212,0.0210964,0.0191232,0.0192566,0.0189992,0.013169,0.0148948,0.0136479,0.0108091,0.0190478,0.0118339,0.0157393,0.0141901,0.0138111,0.0136582,0.0147959,0.0162882,0.0149063,0.0119181,0.00697032,0.0145056,0.0173707,0.0148388,0.0119455,0.0105165,0.0130203,0.0105173,0.012907,0.00881043,0.0139581,0.0128065,0.0146382,0.0156252,0.0131204,0.0148923,0.0185253,0.00851092,0.00816647,0.0138447,0.0169228,0.0108169,0.0116651,0.0130992,0.0119906,0.0104648,0.0113195,0.00983571,0.0154636,0.0185735,0.0137824,0.0110861,0.0160211,0.0117541,0.0118792,0.0128403,0.0140897,0.0176898,0.0138341,0.0125519,0.0164314,0.0129309,0.00902763,0.011426,0.00844845,0.0117402,0.0190604,0.0142283,0.0154814,0.0135041,0.0113018,0.00849024,0.0117659,0.0118452,0.0161379,0.0118232,0.0129628,0.012038,0.0118862,0.0184835,0.0106302,0.013278,0.0133303,0.0122885,0.0158555,0.0150539,0.0138064,0.0134596,0.0117193,0.00995284,0.0108294,0.0148444,0.0121621,0.0126851,0.0116231,0.0167433,0.0109827,0.0149609,0.0120159,0.0135463,0.0130773,0.0077621,0.0138498,0.0151475,0.0127313,0.0137761,0.0124245,0.00977297,0.0107867,0.013379,0.0100133,0.0164539,0.0186411,0.012853,0.0179313,0.0150939,0.0107584,0.01402,0.00976772,0.0121097,0.0157052,0.00930337,0.0115837,0.0143576,0.0100623,0.0129372,0.0194175,0.0119808,0.0149293,0.0148015,0.0125292,0.0111694,0.00928205,0.0119919,0.0139745,0.01192,0.0151467,0.0134733,0.0146714,0.0154427,0.0156523,0.0114213,0.0144365,0.00880582,0.00916148,0.0125436,0.0154822,0.0164574,0.0123327,0.0141069,0.00945664,0.0171462,0.0178175,0.0141191,0.0115663,0.0143746,0.0104096,0.00913988,0.00932105,0.0164477,0.0106505,0.019221,0.0143611,0.0183234,0.015033,0.0094807,0.0169134,0.0110564,0.0165076,0.0158519,0.0128193,0.0113942,0.0115132,0.0137485,0.0149948,0.0109193,0.0165238,0.020343,0.015343,0.0106122,0.0153905,0.010033,0.0149175,0.0110345,0.0174671,0.0226554,0.0137217,0.0114699,0.0124507,0.0195415,0.0164105,0.0195081,0.0109024,0.0132186,0.0131487,0.0127153,0.0112392,0.0131756,0.0160893,0.0113882,0.0114418,0.0110298,0.0165236,0.016095,0.0159754,0.0145591,0.0110213,0.014955,0.00895036,0.014381,0.0137741,0.0128696,0.011015,0.0146446,0.0145986,0.0128938,0.0131753,0.0151956,0.012805,0.0170101,0.0113794,0.0101298,0.0126381,0.0124868,0.00931785,0.0144445,0.010319,0.0111292,0.0109239,0.0110456,0.0117098,0.0206171,0.0131977,0.0143356,0.0155419,0.0140059,0.011421,0.00921159,0.0128427,0.0106389,0.00801254,0.0125897,0.015986,0.012467,0.0141492,0.014356,0.0122217,0.0169196,0.0173333,0.014527,0.0116362,0.0138931,0.0111199,0.0156563,0.0101513,0.0153632,0.0097599,0.0152291,0.0129026,0.0123421,0.0127683,0.0109335,0.0153733,0.0114279,0.014663,0.0101704,0.0156351,0.0150269,0.0169898,0.0151476,0.0121304,0.00793438,0.0133919,0.0207256,0.013724,0.0156266,0.0101682,0.0107866,0.0129028,0.0119897,0.0120733,0.0124937,0.0109163,0.0157448,0.0169022,0.0106582,0.00929948,0.0141242,0.0205257,0.0135861,0.0110886,0.015342,0.0112098,0.0131467,0.0141206,0.0138707,0.0113135,0.0121867,0.0156461,0.0187054,0.0116138,0.0170222,0.011054,0.013387,0.0126758,0.0106131,0.0162227,0.0121801,0.0136281,0.0138935,0.015162,0.00894566,0.0139457,0.0104564,0.0117233,0.014039,0.0161565,0.0146218,0.0111423,0.0175981,0.011576,0.0120524,0.0167498,0.0134704,0.00883293,0.0132459,0.010369,0.0142655,0.0145852,0.0107724,0.0104669,0.00907395,0.0156721,0.0133592,0.0154806,0.0149773,0.0113941,0.016068,0.0185963,0.0116551,0.0105411,0.0136764,0.0105088,0.0147618,0.0149364,0.0123654,0.0129309,0.0103664,0.0121574,0.0171804,0.0159053,0.0115766,0.0189952,0.0115559,0.0146015,0.0182274,0.0161811,0.0173206,0.0144024,0.010734,0.014105,0.00999655,0.0109137,0.0177105,0.0123648,0.00870514,0.0129285,0.0116979,0.0123894,0.0126482,0.010418,0.0154196,0.0123708,0.0121342,0.0118176,0.0187391,0.0154145,0.00943037,0.0097891,0.0117784,0.0123049,0.00798714,0.0105411,0.0135313,0.0173082,0.0151264,0.0121534,0.01435,0.0109057,0.0116511,0.0145218,0.0130577,0.015166,0.0178678,0.0148712,0.0155506,0.0140701,0.0155712,0.00910135,0.0114955,0.011082,0.0119422,0.0128284,0.0143268,0.00948727,0.0166419,0.0117629,0.01374,0.0152297,0.0134417,0.0165751,0.0116018,0.0141004,0.0135838,0.0161944,0.00941112,0.0125835,0.0131915,0.00834983,0.0133396,0.01193,0.0135426,0.0151577,0.0107598,0.013707,0.0131808,0.0113569,0.00962849,0.0192325,0.0109672,0.0124202,0.0141706,0.0102946,0.0154167,0.0152748,0.013672,0.00840293,0.00956739,0.0170844,0.00930506,0.0112772,0.0109931,0.0121133,0.0097278,0.0132897,0.012494,0.0146118,0.00836897,0.00744474,0.00871775,0.0107125,0.0104866,0.0140835,0.00946836,0.0164173,0.0149962,0.00902454,0.0101469,0.0110261,0.0113094,0.00946943,0.00707645,0.0159641,0.0122578,0.0119054,0.00804481,0.00799675,0.0112832,0.0097687,0.00994825,0.0114076,0.0108785,0.0125528,0.0143222,0.0128533,0.0102408,0.0124692,0.00898386,0.0114043,0.0119342,0.0123637,0.0116954,0.0126929,0.00976358,0.0099056,0.00861075,0.00757645,0.0103939,0.00948797,0.00641744,0.0119768,0.00981575,0.00890497,0.00992871,0.00777804,0.00939018,0.00634227,0.00860582,0.00862778,0.00749801,0.00729581,0.0101328,0.0100331,0.00748378,0.0071282,0.00875067,0.00597831,0.00657599,0.00806896,0.00548669,0.00837092,0.00693438,0.00834368,0.00716591,0.00891,0.010452,0.0101018,0.00896883,0.00798149,0.00875564,0.00763831,0.00868312,0.00870126,0.00607887,0.00587287,0.006794,0.0070021,0.00898005,0.00870162,0.0116796,0.00551074,0.00671701,0.00710552,0.00539964,0.0056716,0.00868893,0.00716049,0.00851912,0.0066705,0.0104119,0.00727785,0.0065929,0.00601625,0.00558624,0.00549752,0.00442946,0.00703802,0.00663885,0.00576001,0.00475687,0.00646469,0.00501617,0.00543651,0.00549916,0.00733235,0.00459273,0.00531676,0.00343416,0.00472653,0.00476074,0.00453116,0.00607299,0.00435684,0.00383722,0.00440299,0.00598549,0.00589809,0.00454588,0.00328303,0.00436294,0.0059814,0.00535849,0.00355639,0.00448186,0.00604269,0.00351423,0.0056849,0.00427018,0.00563681,0.00510267,0.00399585,0.00667424,0.00505355,0.00474563,0.00423719,0.00491457,0.00668052,0.00318195,0.00596746,0.005693,0.00649266,0.00606138,0.00416462,0.0049504,0.00563267,0.00525946,0.00386423,0.00498264,0.00411388};
    //double tabPrj2[] = {0.0017113,0.00231318,0.00277717,0.00141474,0.00263247,0.00286838,0.00290052,0.00159042,0.00275439,0.00262002,0.00259851,0.0022913,0.00260991,0.00297966,0.00286164,0.00344689,0.00162757,0.00250573,0.00219358,0.00256676,0.00271361,0.00219069,0.00212191,0.00260021,0.00192401,0.0031763,0.00232166,0.00308566,0.00198758,0.00226414,0.00265725,0.00303674,0.00169713,0.00341793,0.00185171,0.0032059,0.0021914,0.00226671,0.00297261,0.00257639,0.00242065,0.00292925,0.00208169,0.00314487,0.00301428,0.00223799,0.00231428,0.00330425,0.00334162,0.0039602,0.0036417,0.00273186,0.00377791,0.00305043,0.00262599,0.00402293,0.00226487,0.00362478,0.00274932,0.00364994,0.00363792,0.00300973,0.00252951,0.00240149,0.00347925,0.00202248,0.0040441,0.00344659,0.00359889,0.00226324,0.00281796,0.00347218,0.00407608,0.00330929,0.00355928,0.00388736,0.00371627,0.00267724,0.00345483,0.00430084,0.00273347,0.00376813,0.00551711,0.00452365,0.00478712,0.00417557,0.00610989,0.00390545,0.00523691,0.00271465,0.0040273,0.00454382,0.00390133,0.00561782,0.00434382,0.00372175,0.00374899,0.00359017,0.00358468,0.00407509,0.00401192,0.0039415,0.00363846,0.00554287,0.00413294,0.00359854,0.00338927,0.00433705,0.00408903,0.00432374,0.00525705,0.0044205,0.00395542,0.00406399,0.00486826,0.00468515,0.00341299,0.00479409,0.00472882,0.00424849,0.00545019,0.0054366,0.0066845,0.0046278,0.0032291,0.00406909,0.00792501,0.00296239,0.00259778,0.00648042,0.00552435,0.00489335,0.0062675,0.00645919,0.00372396,0.00549586,0.00597815,0.00316895,0.00475416,0.00548591,0.00605094,0.00588183,0.00592478,0.00508458,0.00644517,0.00565208,0.00533421,0.00412309,0.00601814,0.00455854,0.00707612,0.004559,0.00776309,0.00467453,0.00587719,0.00710181,0.00577825,0.00724003,0.00537059,0.0051621,0.00765076,0.00678767,0.00438988,0.00824174,0.00578874,0.00979987,0.00665956,0.00773315,0.00557518,0.00739708,0.0076415,0.00418957,0.00618855,0.00503407,0.00478715,0.00511739,0.00555027,0.00617095,0.00605738,0.00673338,0.00481155,0.00617815,0.00561192,0.00783689,0.00634014,0.00713348,0.0113236,0.00701083,0.00699168,0.00609658,0.00565417,0.00796864,0.00700661,0.00513926,0.00779019,0.00650872,0.00881278,0.00632191,0.00596635,0.00815041,0.00720601,0.00684435,0.00863387,0.00630233,0.00484537,0.00640133,0.00818325,0.0045587,0.00452743,0.00654918,0.00540053,0.00630518,0.00522595,0.00739352,0.00639709,0.00479796,0.0068407,0.00626578,0.00694108,0.0065762,0.0061925,0.00624253,0.00775933,0.00784223,0.00384677,0.0055988,0.00651175,0.00834452,0.00978448,0.00588959,0.0073092,0.00599657,0.00555204,0.0102659,0.00799191,0.00763535,0.00531394,0.0092651,0.00496276,0.010605,0.00504598,0.00796951,0.00606047,0.00814541,0.00678385,0.00681589,0.00601297,0.00612552,0.00845446,0.00481155,0.00696251,0.00690534,0.00780924,0.00521578,0.00619826,0.0049261,0.00477977,0.0097609,0.00704654,0.00862793,0.00578072,0.00580339,0.00768671,0.00480558,0.00540022,0.00697799,0.00513197,0.00545106,0.00606955,0.00624588,0.00712462,0.005853,0.00488809,0.00223691,0.00827056,0.00618474,0.00823719,0.00747739,0.00587648,0.00648063,0.00859967,0.00782762,0.0115164,0.00721676,0.00584066,0.00514031,0.00770681,0.00757639,0.006808,0.0082011,0.00747301,0.00690545,0.00608143,0.00938123,0.00568253,0.00558205,0.00568622,0.00724734,0.00683438,0.00632695,0.0088235,0.00644229,0.00704713,0.00673073,0.00820722,0.00842759,0.00686594,0.00662271,0.00697457,0.00664949,0.00649675,0.00667976,0.00836017,0.00773472,0.00804051,0.00588785,0.00422604,0.00609525,0.00539887,0.00696142,0.00789068,0.00732093,0.00703814,0.00719751,0.00663264,0.00715024,0.00436898,0.00632868,0.00828378,0.00618305,0.00800763,0.00569976,0.0115491,0.00685761,0.00455036,0.00893362,0.00516649,0.00658447,0.00635766,0.00898426,0.00736585,0.00678083,0.00878783,0.00746881,0.00635938,0.00562116,0.00686034,0.00610682,0.007436,0.00472749,0.0069311,0.00618252,0.0047987,0.0064467,0.00743567,0.00731378,0.00647117,0.00591371,0.00763822,0.00640855,0.00516036,0.00845471,0.00510667,0.00596199,0.00717952,0.00606069,0.00544801,0.00734688,0.00658127,0.00846144,0.00661746,0.00681341,0.00882707,0.00778374,0.00542532,0.00393949,0.00472074,0.00801695,0.00880827,0.00569106,0.00666035,0.00730321,0.00749816,0.0112102,0.00724691,0.00711966,0.0069551,0.00714259,0.00548014,0.00534924,0.00684261,0.00759642,0.00733551,0.00829182,0.00698945,0.0115002,0.00720819,0.00659471,0.00778967,0.00558425,0.00848796,0.00659304,0.00807813,0.00712361,0.00624124,0.00680739,0.00688485,0.00735373,0.00781929,0.00427408,0.00559757,0.00718414,0.00663843,0.00840593,0.00608078,0.00836387,0.00606093,0.00878051,0.00731108,0.00814616,0.00671066,0.00645127,0.00741661,0.00733154,0.00747786,0.00648743,0.00707554,0.00705342,0.00593515,0.0053285,0.00777649,0.00934158,0.00511142,0.00814913,0.00787691,0.00667466,0.00710063,0.00455077,0.00575079,0.00547763,0.00889483,0.00918532,0.0062536,0.00505517,0.00567288,0.00759696,0.00785324,0.00948085,0.00417078,0.00639634,0.00581455,0.00714872,0.00676204,0.00729302,0.00791432,0.00763278,0.0078333,0.0080584,0.0063136,0.00664996,0.00623346,0.00679124,0.00868027,0.00686641,0.00774679,0.00832495,0.00644552,0.00748777,0.00859344,0.00745707,0.00609449,0.0047066,0.00716888,0.00907791,0.0115468,0.0170807,0.0296197,0.0477735,0.0784477,0.108328,0.138382,0.175267,0.206704,0.248077,0.28468,0.318719,0.341318,0.355769,0.366226,0.371998,0.369114,0.364258,0.352436,0.338806,0.328545,0.313767,0.303312,0.292128,0.277702,0.265605,0.252047,0.245648,0.232564,0.22344,0.217296,0.204731,0.185687,0.175914,0.15825,0.145814,0.135844,0.127449,0.123037,0.122242,0.123368,0.126274,0.139557,0.151953,0.161397,0.167708,0.166658,0.155968,0.151624,0.143972,0.14188,0.143245,0.144079,0.151953,0.162655,0.172308,0.178051,0.188006,0.198914,0.204569,0.210279,0.204739,0.204185,0.211139,0.209086,0.195071,0.221144,0.25624,0.257386,0.297298,0.293283,0.313802,0.330316,0.332489,0.350397,0.355976,0.364738,0.366997,0.372631,0.354282,0.364693,0.40653,0.369454,0.188036,0.0555799,0.013881,0.00639609,0.00615073,0.0122408,0.00770548,0.00634187,0.00585885,0.00812188,0.00570508,0.0119418,0.00646743,0.00592049,0.00807005,0.0058374,0.00980392,0.00632234,0.00816011,0.00814873,0.0081425,0.00648876,0.00513286,0.00587301,0.00627916,0.00647777,0.00629376,0.00657697,0.00485898,0.00835132,0.00554212,0.00566659,0.00748673,0.00509084,0.00682584,0.00676679,0.00665094,0.0104199,0.00781511,0.00598604,0.00869493,0.00554159,0.00706622,0.00674209,0.00926443,0.00774928,0.00582586,0.00663022,0.00741308,0.00746893,0.00754774,0.00657834,0.00586871,0.00380731,0.00619598,0.00480781,0.00768861,0.00716407,0.00648283,0.00462326,0.00703333,0.00644785,0.00791738,0.00769077,0.00633368,0.0049785,0.00737424,0.00881528,0.00614474,0.00756489,0.00667734,0.00364278,0.0064846,0.0049907,0.00609771,0.00717908,0.00583699,0.00521663,0.00568788,0.00634574,0.00773,0.00771294,0.0088869,0.00650773,0.00646466,0.00739608,0.00485313,0.00835685,0.00839434,0.00683535,0.00536429,0.00589549,0.00661688,0.00540094,0.00628074,0.00747145,0.00672993,0.00610434,0.00684089,0.00791383,0.00900676,0.00443442,0.00612895,0.00599022,0.00627645,0.00649592,0.00473397,0.00770245,0.00676859,0.0059912,0.00838245,0.00408704,0.00791584,0.00513546,0.00472319,0.00496182,0.0086761,0.0100657,0.00543281,0.00775983,0.00769654,0.00968079,0.0104637,0.00670585,0.00794782,0.00699021,0.00782537,0.00524264,0.00459969,0.00490201,0.00654752,0.00627195,0.00921263,0.00464304,0.00751281,0.00680564,0.00740501,0.00707293,0.00767008,0.00749807,0.00972603,0.00661044,0.00595891,0.00627057,0.0061878,0.00661085,0.00399152,0.00566746,0.0052411,0.00554887,0.00722874,0.00678143,0.00791673,0.00666725,0.00613423,0.00638467,0.00896755,0.00646355,0.00420621,0.00451432,0.00584433,0.00557924,0.00646412,0.00748801,0.00924125,0.0059292,0.00613477,0.00611965,0.00543463,0.00660454,0.0081042,0.00497479,0.00847801,0.00580437,0.00592877,0.00706338,0.00567675,0.00772056,0.00531698,0.00644531,0.00563224,0.00647637,0.00665635,0.00674889,0.00511773,0.00528324,0.010173,0.00558796,0.00678014,0.00859941,0.00746739,0.00576977,0.00583518,0.00785674,0.00699816,0.00551326,0.00729052,0.00617062,0.00965279,0.00765571,0.00535528,0.00574832,0.00587647,0.00832574,0.00614311,0.00954644,0.0065395,0.00778792,0.00545359,0.00847299,0.0106075,0.00796299,0.00636094,0.00509814,0.00755094,0.00653275,0.00655399,0.00504551,0.00603646,0.00508932,0.0119276,0.00783994,0.0082579,0.00884699,0.00435327,0.00780551,0.00666618,0.00615809,0.00700429,0.00692765,0.00600391,0.00730189,0.00568159,0.00679909,0.00795024,0.00893353,0.00744034,0.00761402,0.00911953,0.00834976,0.00654059,0.00714536,0.00811685,0.00666508,0.00603956,0.00874498,0.00640773,0.00611765,0.00549024,0.00594109,0.00564713,0.00770153,0.00632474,0.00619655,0.00630836,0.00448706,0.00754018,0.00650219,0.00661749,0.00661937,0.00771899,0.00525823,0.0045148,0.00584457,0.00639682,0.00771099,0.00482759,0.00914897,0.00543055,0.00688445,0.00761875,0.00803344,0.00540149,0.00610596,0.00534784,0.00782414,0.00469304,0.00609491,0.00828061,0.00679027,0.00489255,0.00793178,0.00473585,0.00540247,0.00665807,0.00530702,0.00631736,0.00661301,0.00495291,0.00586453,0.00860711,0.00634536,0.00471984,0.00587403,0.00733021,0.00564728,0.0058814,0.00511123,0.0056006,0.00860314,0.00604051,0.0056425,0.00594877,0.007745,0.00733718,0.00891626,0.00672669,0.00489165,0.00572123,0.00649089,0.00504138,0.00823473,0.00493559,0.00481484,0.00611887,0.00614955,0.00638103,0.00676448,0.00410083,0.00411229,0.00583516,0.0045069,0.00615251,0.00543621,0.0068075,0.00413815,0.00481436,0.00418368,0.00606966,0.00552142,0.00697889,0.00721046,0.00535802,0.00663923,0.00512956,0.00553972,0.00477591,0.00418854,0.0052337,0.00401176,0.00452626,0.00578924,0.00472092,0.00541866,0.00528008,0.00554373,0.00560482,0.00394046,0.00380841,0.00536663,0.0031437,0.00599635,0.00416648,0.00337174,0.00563846,0.00394655,0.00613005,0.00471528,0.00550664,0.00342393,0.00456376,0.00386107,0.00703033,0.00573674,0.0041498,0.00462775,0.00425535,0.00580057,0.00367958,0.00456419,0.00405739,0.00505866,0.00572511,0.00317353,0.00426233,0.00314853,0.0044708,0.00377423,0.00302933,0.00356011,0.00277401,0.00355192,0.00440523,0.00357015,0.00422681,0.00353734,0.00447387,0.00471096,0.00286032,0.00448661,0.00416602,0.00341083,0.00303857,0.00231019,0.00413558,0.00254736,0.00366167,0.00336429,0.00310673,0.00437774,0.00412134,0.00395392,0.00441176,0.00410134,0.00348218,0.00392443,0.00314999,0.00376233,0.0029853,0.00268852,0.00451501,0.00381023,0.00342955,0.00189858,0.00269421,0.00311317,0.00307307,0.0019118,0.00308743,0.00273347,0.00293124,0.00279002,0.00209238,0.00165891,0.00440599,0.00279034,0.00304513,0.00498943,0.00226056,0.00215612,0.00317975,0.0038621,0.00226142,0.00278576,0.00332583,0.00224002,0.00235536,0.00282762,0.0035452,0.00286939,0.00350995,0.00318573,0.00167219,0.00273066,0.00301332,0.0016529,0.00228153,0.001891,0.00279092,0.00282611,0.00230506,0.00323205,0.00215438,0.00244924,0.00292976,0.00238283,0.0023761,0.00312555,0.0031838,0.00298759,0.00223433,0.00229306,0.00200148,0.00279553,0.00258859,0.00224936,0.00263268};
	double tabPrj1[] = {0.0126087,0.0120099,0.014707,0.0140508,0.0101379,0.0110721,0.0115908,0.0107677,0.00833755,0.0113595,0.0102241,0.0125457,0.011696,0.0105483,0.011267,0.0115235,0.0103763,0.0119919,0.00979402,0.00872056,0.0122449,0.0180856,0.011209,0.0130251,0.0137351,0.0141406,0.0092905,0.0101311,0.0113759,0.0135087,0.0119321,0.0134189,0.0108093,0.0103688,0.0141127,0.0123054,0.0119609,0.0178276,0.0159943,0.0127318,0.0141022,0.0138785,0.0123405,0.0135228,0.0165716,0.0140284,0.0128852,0.0177582,0.0100774,0.01114,0.016466,0.0144516,0.0208473,0.0123461,0.0143508,0.0145103,0.00900898,0.0166384,0.0151443,0.0164452,0.0209838,0.0203839,0.0180538,0.0190021,0.0149725,0.0170196,0.0159339,0.018696,0.0168596,0.0160519,0.0188663,0.0185394,0.0192948,0.0169231,0.0170701,0.0191675,0.0225924,0.0258839,0.0183559,0.0155035,0.0182469,0.0148727,0.0159806,0.0174579,0.0213001,0.0248795,0.0191129,0.0247183,0.0201708,0.019359,0.0245987,0.0195632,0.0241769,0.0205376,0.0232223,0.030539,0.0266775,0.031893,0.0191391,0.0172226,0.0174564,0.0244358,0.0268028,0.0236711,0.0203138,0.0277878,0.022445,0.0227168,0.0223689,0.0233352,0.0183045,0.0241953,0.0220892,0.0258469,0.0215407,0.0220715,0.0237324,0.0280273,0.0200359,0.0240917,0.0209963,0.0285734,0.0362077,0.0304483,0.0248417,0.0298599,0.0295473,0.0220875,0.0235596,0.0332086,0.0236026,0.0505945,0.0335592,0.0178466,0.0302509,0.0287899,0.0253833,0.0257765,0.0323758,0.0236051,0.0278724,0.0330666,0.0324989,0.0298507,0.0356244,0.0294729,0.025914,0.0291611,0.0336081,0.0258811,0.0298425,0.0316784,0.0308173,0.0382108,0.0320868,0.0256197,0.0340204,0.0199456,0.023354,0.031209,0.0287982,0.026449,0.0276967,0.0349125,0.0295103,0.0333773,0.02219,0.0336406,0.0271081,0.0214463,0.0314467,0.0334619,0.0417179,0.027882,0.040968,0.0361025,0.0321878,0.0311357,0.0323708,0.0290818,0.0325483,0.0367004,0.0310602,0.0287278,0.0282726,0.0326183,0.0380901,0.035347,0.026802,0.0405699,0.0403415,0.0422341,0.0286569,0.0354539,0.0285321,0.0380665,0.031551,0.0355916,0.0246189,0.0349371,0.0258504,0.031649,0.0304078,0.0261769,0.031187,0.0312517,0.0323141,0.0351582,0.026446,0.0317515,0.0244904,0.029698,0.0313406,0.0359236,0.0296231,0.031291,0.0367838,0.031488,0.0321416,0.0367857,0.0352029,0.0387988,0.0251407,0.0313637,0.0377946,0.0255446,0.0332911,0.0436358,0.031556,0.0352006,0.0337082,0.0296669,0.0336012,0.0265589,0.0398497,0.0343526,0.0341085,0.0464356,0.0275952,0.0336646,0.0340378,0.0241375,0.0369155,0.0251892,0.033197,0.0481343,0.0443311,0.0477253,0.0411529,0.0317023,0.0282784,0.0414446,0.033467,0.0320818,0.0363456,0.0322888,0.0331805,0.0300225,0.0329814,0.0346886,0.0321683,0.0343264,0.0251308,0.0262358,0.0308888,0.0328226,0.0412332,0.0454725,0.0408774,0.034159,0.0355473,0.0266166,0.0346163,0.0403961,0.0376364,0.0327088,0.0299187,0.0297188,0.0415293,0.0350412,0.0293148,0.02755,0.0357932,0.0263338,0.0310371,0.0257567,0.0344872,0.035991,0.0424882,0.0417674,0.0397278,0.0392667,0.0368065,0.0417153,0.0439141,0.0312458,0.0464714,0.0478125,0.0374359,0.034971,0.0413526,0.0417787,0.0460253,0.0305203,0.0483776,0.0516816,0.0407885,0.0434807,0.0511706,0.0481307,0.047081,0.0444675,0.0537951,0.0464142,0.0439043,0.054921,0.0591141,0.0567649,0.0567671,0.0507129,0.0529135,0.061333,0.0653139,0.0667054,0.0709294,0.0733073,0.0727875,0.0827762,0.0812243,0.0944279,0.0754498,0.0913771,0.0949736,0.112039,0.0889299,0.137133,0.102763,0.152433,0.127493,0.172032,0.156301,0.209808,0.214317,0.224444,0.260074,0.371893,0.207668,0.47903,1.97712,7.23201,21.5763,25.6344,24.578,20.5961,18.014,17.1303,16.4199,16.2743,16.464,16.4479,16.4363,16.4319,16.4505,16.5654,16.565,16.5629,16.6572,16.7106,16.7998,16.8945,17.0159,17.1141,17.2545,17.3874,17.5854,17.8454,18.2058,18.7,19.2783,19.8528,20.2569,20.3026,19.774,18.6437,16.9334,14.7229,12.2152,9.62621,7.20453,5.07965,3.41387,2.14702,1.31362,0.75298,0.406904,0.240333,0.128537,0.074149,0.0572539,0.054445,0.0509284,0.0473466,0.0500987,0.0432932,0.0489344,0.0402754,0.0468858,0.0428895,0.0687986,0.0900553,0.116785,0.137786,0.190339,0.219812,0.263188,0.313661,0.342897,0.383547,0.410897,0.447497,0.455879,0.472809,0.488914,0.480919,0.465574,0.475982,0.470765,0.448528,0.420612,0.445843,0.466202,0.440678,0.369071,0.387539,0.439685,0.477187,0.455703,0.412867,0.341131,0.32576,0.347991,0.414066,0.476165,0.498691,0.520411,0.524486,0.492264,0.458453,0.419879,0.390055,0.344527,0.296582,0.278167,0.246116,0.208871,0.180079,0.175752,0.161445,0.144815,0.129225,0.11335,0.104103,0.0955952,0.0900174,0.0700137,0.0838419,0.066862,0.0618848,0.0510407,0.0567873,0.0485455,0.03831,0.0611888,0.029119,0.034434,0.0356911,0.046258,0.0234033,0.0348343,0.0312694,0.0321694,0.0411247,0.0337514,0.0344725,0.0384334,0.0404402,0.041663,0.0281002,0.0361943,0.0391852,0.0386838,0.0263706,0.0382068,0.0281604,0.0345098,0.0240039,0.0241902,0.0325252,0.0430751,0.0358209,0.0410164,0.0369443,0.0264411,0.0402032,0.0329871,0.0314803,0.0327297,0.0355019,0.0353797,0.0341612,0.0354499,0.0311603,0.0317547,0.0350347,0.0249295,0.0392593,0.0319633,0.0348846,0.0291903,0.0351229,0.0401932,0.0383788,0.0255144,0.0233771,0.0332436,0.033262,0.029743,0.0327843,0.0401063,0.0407202,0.0320517,0.0346798,0.033641,0.027048,0.0342088,0.0328026,0.0314285,0.0249712,0.0369949,0.0321671,0.0267818,0.0399331,0.0314207,0.0362296,0.0303976,0.0370325,0.0336737,0.0395529,0.0419166,0.0327772,0.029093,0.024964,0.0275303,0.0345363,0.0199663,0.0360799,0.0315835,0.0300816,0.0326763,0.0254516,0.0294488,0.0276337,0.0282114,0.0364676,0.0338441,0.0290557,0.0495734,0.026855,0.0311756,0.0218771,0.0361149,0.0276207,0.0324837,0.0342571,0.0413299,0.0347292,0.0443842,0.0345517,0.0311111,0.0290478,0.049007,0.0295839,0.042649,0.0399403,0.03822,0.0299699,0.0352659,0.0298164,0.0318308,0.0413247,0.0367134,0.0397879,0.0341816,0.0390295,0.0385215,0.0287879,0.0376334,0.0255757,0.0355598,0.0330191,0.0262374,0.0416087,0.032237,0.0295902,0.038032,0.0354285,0.0332911,0.0326652,0.0266188,0.0458,0.0236869,0.0243376,0.0318176,0.0338222,0.0277651,0.0282783,0.0366504,0.028369,0.0364881,0.043507,0.0317313,0.0276801,0.0335359,0.030623,0.0301455,0.033725,0.0320666,0.0373419,0.0270398,0.0319806,0.0367949,0.0258124,0.0308806,0.0274859,0.0266264,0.0379627,0.0325326,0.0371998,0.0259148,0.0257075,0.0330524,0.0350193,0.0244616,0.0302679,0.0342136,0.0295358,0.0377225,0.0364806,0.030948,0.0280294,0.0343228,0.0246614,0.0365191,0.0326328,0.0277973,0.0299714,0.0318368,0.0254731,0.0328161,0.0340904,0.0322489,0.0330383,0.0329966,0.0358306,0.0303691,0.0318755,0.0237271,0.0283112,0.0362025,0.0216582,0.0324809,0.0350571,0.0366746,0.0237653,0.0287841,0.0323711,0.027525,0.0318256,0.0384275,0.0344294,0.0279428,0.0313855,0.0291733,0.028627,0.0208674,0.028725,0.0321035,0.0286798,0.0336625,0.0214786,0.0280712,0.0286053,0.0309905,0.0264463,0.0287212,0.0314748,0.0321778,0.0334135,0.0266937,0.0421448,0.0269774,0.0329144,0.0278942,0.0292764,0.0233564,0.0302142,0.0313452,0.0332896,0.0432444,0.0325437,0.0285317,0.0248114,0.0292688,0.0289909,0.0375508,0.0314976,0.0353924,0.0264213,0.031871,0.0321699,0.0565483,0.0291553,0.0316267,0.0259275,0.0285037,0.0292409,0.0234537,0.0309131,0.0385135,0.0427921,0.0299871,0.0312335,0.0395569,0.0368129,0.0247411,0.0387663,0.0357143,0.0356682,0.031973,0.0344637,0.032803,0.0356718,0.0254139,0.0307486,0.0289448,0.0346217,0.0350259,0.0396012,0.0315657,0.0314726,0.027968,0.03223,0.0387076,0.0359737,0.0387122,0.0433537,0.0379132,0.0283281,0.0375067,0.0276873,0.0355672,0.0309608,0.0267404,0.0300566,0.0266267,0.0304389,0.0245551,0.0234113,0.0288795,0.0365633,0.0256308,0.0232409,0.0330325,0.025305,0.032286,0.0261008,0.0351016,0.0266734,0.0281602,0.0276527,0.0407048,0.0268426,0.02964,0.0334027,0.0376578,0.0310415,0.0280119,0.0242172,0.0286118,0.0378506,0.0327297,0.0276014,0.0300012,0.0351562,0.034443,0.0401812,0.0322834,0.0297725,0.035028,0.0364431,0.0283709,0.0214712,0.0313565,0.0281983,0.0284689,0.0253267,0.0297347,0.0346906,0.0253523,0.0310567,0.031395,0.0236498,0.0352334,0.0309575,0.0224517,0.0303648,0.0342828,0.0314442,0.0257328,0.0301793,0.0282585,0.0230843,0.0256652,0.0304405,0.0256263,0.0279764,0.028503,0.0251925,0.0283116,0.0262085,0.0353083,0.0288361,0.0389723,0.0280898,0.0234927,0.0321175,0.0286629,0.0326944,0.0283085,0.0271914,0.0217031,0.0266932,0.0377316,0.0284957,0.0206057,0.0303309,0.0308285,0.0417394,0.0380351,0.0256539,0.0281424,0.0213237,0.0327946,0.0221099,0.0247671,0.0318667,0.0266347,0.0265156,0.0324893,0.0429803,0.0265949,0.0265594,0.0239951,0.0226435,0.0285221,0.0204399,0.0210143,0.0262775,0.0240264,0.0171644,0.0273326,0.0261222,0.0246034,0.0276014,0.0279008,0.0289106,0.0276023,0.0248403,0.0295543,0.0264034,0.0237998,0.0243899,0.0163009,0.0284325,0.029434,0.0282975,0.0294146,0.0249698,0.0258787,0.028988,0.0250418,0.0149434,0.0300581,0.0246767,0.0237028,0.0233724,0.0314106,0.0240555,0.0244607,0.0236498,0.0284097,0.0320964,0.0262931,0.0256963,0.0274901,0.0222832,0.0194745,0.0195399,0.0174097,0.0222775,0.0271034,0.0228842,0.0295773,0.0240384,0.0250817,0.0237925,0.0274132,0.0190536,0.0264991,0.0186128,0.0197196,0.0271225,0.0227853,0.0226044,0.0213846,0.0156099,0.0200905,0.0186177,0.0155736,0.0272781,0.0193601,0.0304658,0.0218116,0.0176361,0.0221188,0.0183652,0.0165062,0.0227168,0.017033,0.0154139,0.0179508,0.0231059,0.0212189,0.0178558,0.0240123,0.0164343,0.0178723,0.0183739,0.0151916,0.0173009,0.0163704,0.0172383,0.0143943,0.016291,0.0184182,0.0139936,0.0208311,0.0153109,0.0169268,0.0175701,0.0214156,0.013577,0.0149528,0.0173439,0.0124194,0.0150134,0.0170088,0.0125369,0.0145057,0.0128058,0.0157863,0.0115416,0.0155416,0.0160725,0.0121809,0.0176476,0.0165014,0.0152487,0.0145864,0.0163796,0.0144106,0.0132921,0.0118924,0.0111876,0.0122003,0.01201,0.0168946,0.0161679,0.00961843,0.0118371,0.0172944,0.0133439,0.01376,0.0142642,0.009701,0.0110542,0.0112276,0.010925,0.0102008,0.0124346,0.0119041,0.0134418,0.011179,0.0106771,0.0101813,0.0151098,0.0132278,0.0169022,0.0112632,0.0149601,0.0159573,0.0114225,0.0111995,0.0116272,0.0107372,0.0086501,0.0103469,0.00990576,0.00997169,0.0094257,0.00817617,0.00903036,0.00714878,0.0102267};
	double tabPrj2[] = {0.00727639,0.00559798,0.00647788,0.00864324,0.0074448,0.00674207,0.00523699,0.00612645,0.00697496,0.00742466,0.00704684,0.00764554,0.00632668,0.00885311,0.00644274,0.00797729,0.00600735,0.00597747,0.00780213,0.0107082,0.00749548,0.00676847,0.00672937,0.00626242,0.00702591,0.00929856,0.00746932,0.00818615,0.00557957,0.00669319,0.00721941,0.00764432,0.00520584,0.00715424,0.00546797,0.00673806,0.00807389,0.00871661,0.00636903,0.00843246,0.00804041,0.00677365,0.0101749,0.00818054,0.00943311,0.00938321,0.00938295,0.00794265,0.00792327,0.0128829,0.00859231,0.00686988,0.00851349,0.00872761,0.0112335,0.00860846,0.0075435,0.00905292,0.00815169,0.00948938,0.00806093,0.0103991,0.00838917,0.00914102,0.00785347,0.0077768,0.0104331,0.00782548,0.0123283,0.0115666,0.0123688,0.0114329,0.00994411,0.00962143,0.0083411,0.00933528,0.0096832,0.00727048,0.0117159,0.00996785,0.00940826,0.00923678,0.0134667,0.0118147,0.0128786,0.00757264,0.0115701,0.0134588,0.0123671,0.0127265,0.014202,0.0137872,0.0118006,0.0135365,0.00939214,0.00972367,0.0117319,0.0152577,0.0119975,0.0127826,0.00847479,0.0166733,0.0127892,0.0129451,0.0147996,0.00967775,0.0105583,0.0121787,0.0119357,0.0180857,0.0113836,0.0127291,0.0157413,0.0113414,0.0180563,0.0160011,0.0136521,0.0142883,0.0118639,0.0121693,0.0196979,0.0178832,0.015947,0.0172281,0.0150178,0.0171041,0.0180009,0.0155166,0.0176989,0.0149027,0.0112439,0.0174232,0.0131808,0.0174481,0.0125228,0.0159085,0.0149563,0.0185611,0.0179944,0.0159489,0.0169199,0.0119987,0.0211106,0.0153598,0.0181302,0.0160586,0.016546,0.0212963,0.0179737,0.0179046,0.0155421,0.0194685,0.012025,0.0243716,0.0171677,0.0184493,0.0148338,0.0142887,0.0214365,0.0171028,0.011189,0.0153891,0.0171839,0.0210033,0.0149416,0.0170846,0.0168422,0.0206373,0.0142461,0.0161709,0.0164956,0.016351,0.0203755,0.0291715,0.0159605,0.0241677,0.0227577,0.0142035,0.0159671,0.0201978,0.0181471,0.0172434,0.0187836,0.0198104,0.0187071,0.0201643,0.0225064,0.0178295,0.0257292,0.0210714,0.0159192,0.0171146,0.0170215,0.0183224,0.022932,0.0204882,0.0160865,0.0175793,0.0188726,0.0176717,0.0178391,0.0227284,0.0216932,0.0182422,0.0200333,0.020049,0.0214346,0.0259312,0.0155662,0.0201758,0.0131597,0.0187595,0.0193113,0.0170437,0.0245672,0.0201555,0.0253957,0.0165349,0.0246874,0.0188713,0.0272832,0.0162348,0.0149983,0.0236312,0.0194774,0.0225302,0.0159874,0.0228097,0.0154688,0.0223529,0.0192731,0.0246406,0.0210163,0.018378,0.01702,0.016528,0.0169878,0.0187179,0.0267235,0.0147756,0.0197237,0.0161385,0.0197954,0.0141925,0.0167328,0.023573,0.0243636,0.0177939,0.0161417,0.0213314,0.0173007,0.022723,0.0143558,0.0206355,0.0217001,0.0217846,0.0236281,0.0198909,0.0220661,0.0226736,0.0186606,0.0252856,0.0231851,0.0251364,0.0337882,0.0263115,0.0325968,0.0275572,0.0325651,0.038516,0.0446515,0.0341739,0.0395307,0.0367678,0.0363617,0.0396546,0.042617,0.0367436,0.0406859,0.0406043,0.0440466,0.0426623,0.0477911,0.0531844,0.048445,0.039434,0.0520296,0.0424167,0.0478502,0.0429836,0.0484511,0.044536,0.0492426,0.0499619,0.0554284,0.0448859,0.0515255,0.0562519,0.0481371,0.0490551,0.0520481,0.0489078,0.0552165,0.0533999,0.0544592,0.0545298,0.0560923,0.0571635,0.0534478,0.0540705,0.061955,0.063517,0.0618305,0.0576711,0.0707946,0.0703758,0.0732569,0.0724321,0.0699918,0.064144,0.0817221,0.07704,0.0700041,0.0747425,0.0761618,0.0742052,0.0804523,0.0809846,0.0853138,0.07403,0.0742079,0.0785071,0.0870392,0.0873912,0.092322,0.0744144,0.0890461,0.0756211,0.0827696,0.0919824,0.099632,0.102225,0.108813,0.0942684,0.116338,0.10042,0.103521,0.090554,0.084777,0.0763449,0.0645195,0.062764,0.054133,0.0427631,0.0433555,0.0402921,0.0317935,0.0313673,0.0238395,0.0358538,0.017375,0.0282208,0.0229986,0.0264049,0.0305155,0.022296,0.0198817,0.0241733,0.0204698,0.022176,0.0224629,0.0197169,0.0169328,0.0317671,0.0261314,0.0245285,0.0191554,0.016425,0.0252343,0.0238407,0.0242859,0.0202251,0.026551,0.0176385,0.0181255,0.0205364,0.0303877,0.0268961,0.020775,0.0182466,0.0173096,0.0206076,0.0321686,0.0237323,0.0238842,0.0303913,0.0232885,0.0252463,0.0227239,0.0250993,0.0298326,0.0272064,0.0263502,0.023167,0.0270153,0.0239588,0.022145,0.0201622,0.0260911,0.0248424,0.0256459,0.0322147,0.0338069,0.0312574,0.0371471,0.0351062,0.0287002,0.0297123,0.0292056,0.0254302,0.0293269,0.0277114,0.0260244,0.0221229,0.0283971,0.0212041,0.0244983,0.0313874,0.0362314,0.0320096,0.0240718,0.0298221,0.0337419,0.0241517,0.0371195,0.0298002,0.0312406,0.0328964,0.0436093,0.0370765,0.0388499,0.0406129,0.0417419,0.046857,0.0549686,0.0431473,0.0573925,0.0551891,0.0659563,0.0668685,0.0754859,0.0658382,0.0888809,0.0889224,0.111304,0.113734,0.138183,0.153004,0.184349,0.201001,0.264161,0.30401,0.370794,0.44149,0.541917,0.645005,0.688389,0.927852,0.496739,1.13301,0.857203,1.55353,3.67522,4.73089,4.42206,4.53158,4.16797,4.11731,3.83037,3.94267,3.90414,3.95648,3.74956,3.75335,3.6024,3.63005,3.53964,3.57137,3.48611,3.50999,3.44983,3.47285,3.39499,3.43996,3.35858,3.38444,3.32684,3.34104,3.29561,3.30224,3.24941,3.24745,3.19739,3.20647,3.1558,3.15442,3.10324,3.09095,3.05751,3.04765,2.97818,2.9268,2.85084,2.81895,2.7647,2.75843,2.73854,2.72983,2.72474,2.73767,2.76997,2.78044,2.79924,2.82384,2.85461,2.82319,2.87918,2.8752,2.83177,2.88261,2.96201,2.88477,2.86679,2.8309,2.94381,2.85471,2.9373,3.11518,2.89708,2.63459,3.27084,2.78638,2.95815,2.99312,2.94768,2.97955,2.99929,2.94356,2.97867,2.90758,2.8263,2.81107,2.62805,2.63588,2.40319,2.35482,2.10626,2.04557,1.79187,1.68825,1.47027,1.37263,1.16283,1.04589,0.848991,0.742287,0.545708,0.460952,0.334796,0.282625,0.171468,0.145884,0.0796754,0.079053,0.0415709,0.0475058,0.0285955,0.0384483,0.0241227,0.0298212,0.0228696,0.0283115,0.0333606,0.0213136,0.0210666,0.0226358,0.0203791,0.0266306,0.0261164,0.0274184,0.0247141,0.0218929,0.0233074,0.0264945,0.0233615,0.0226405,0.0164091,0.0262756,0.0195245,0.027239,0.0239765,0.0224048,0.0194469,0.0219316,0.0163659,0.0221463,0.016975,0.0169466,0.020309,0.0247023,0.0167793,0.0202217,0.0201845,0.0202167,0.0268417,0.020322,0.0162698,0.019787,0.0263029,0.0206757,0.0206753,0.0189969,0.0231496,0.0185297,0.0240832,0.0208393,0.0275479,0.0309333,0.0181348,0.0221402,0.0213902,0.020511,0.0255572,0.019459,0.0175675,0.0188574,0.0194011,0.0246946,0.0200653,0.014721,0.0214757,0.0164884,0.0204572,0.0204641,0.018303,0.0228146,0.0202719,0.0172933,0.0210504,0.0187366,0.0135695,0.0217048,0.0211011,0.0201253,0.0193056,0.0228609,0.0179829,0.0224932,0.0198358,0.0227802,0.0243513,0.0222897,0.0196253,0.0187841,0.0175868,0.0211239,0.0188306,0.021286,0.0209638,0.0160277,0.0226994,0.0180478,0.0143749,0.0207442,0.0165575,0.0229608,0.0134566,0.0178556,0.019957,0.025255,0.0199858,0.0193336,0.019162,0.0197196,0.021274,0.0233116,0.0160787,0.0212068,0.0174107,0.021031,0.0228216,0.0168689,0.0211617,0.025779,0.0170945,0.0133109,0.0267499,0.0161424,0.0209181,0.0239105,0.0245278,0.0131675,0.0179619,0.0168091,0.0207719,0.0149325,0.0176644,0.0243055,0.0183863,0.0211985,0.0181182,0.0208311,0.0191245,0.0190601,0.0203261,0.0296224,0.0198074,0.0184174,0.0145271,0.0182425,0.023313,0.0150161,0.0189216,0.0213039,0.0289694,0.0170427,0.0207386,0.0182456,0.0233034,0.0193795,0.0129399,0.0183927,0.0187672,0.0169426,0.0178428,0.017875,0.0212571,0.0162834,0.0274941,0.0230806,0.0167037,0.0241528,0.0176354,0.0190793,0.0210589,0.0244673,0.0184635,0.0249622,0.0167642,0.0205637,0.0211956,0.0240924,0.0185783,0.0208114,0.0218807,0.0183749,0.0145855,0.0220248,0.021891,0.0153216,0.0264572,0.0212213,0.0179813,0.00799407,0.0256613,0.015399,0.0125596,0.0200791,0.0157743,0.0167145,0.017071,0.0205253,0.0229973,0.0208204,0.0210465,0.0142803,0.0220766,0.0232517,0.0199201,0.0178332,0.0247086,0.0165067,0.0164461,0.0194229,0.0203475,0.0175184,0.0262115,0.0165188,0.018779,0.0234746,0.018966,0.0156773,0.0202915,0.0172191,0.0212625,0.0184997,0.0138722,0.0209535,0.0171796,0.0162786,0.0178687,0.0188665,0.0186533,0.0184308,0.0222072,0.0148069,0.0129337,0.0122459,0.0213936,0.0175015,0.0180426,0.0255155,0.0174613,0.0181104,0.015949,0.0212799,0.0211453,0.0201394,0.0166195,0.0208178,0.0212913,0.0229329,0.0175162,0.0191579,0.0238822,0.0192714,0.01774,0.0166234,0.0183898,0.0200373,0.0111653,0.0161052,0.0166847,0.0153126,0.0147898,0.0147868,0.0209378,0.0147285,0.02642,0.0163516,0.0160765,0.0228504,0.0130161,0.0200815,0.0180272,0.0146237,0.0131872,0.0234126,0.0149134,0.0182183,0.0180235,0.0168815,0.0177982,0.0185725,0.0203628,0.0149598,0.0244074,0.0140938,0.0214784,0.0169391,0.0179669,0.0131115,0.0167748,0.0215061,0.0233745,0.020255,0.0180522,0.0164529,0.0183039,0.0203811,0.012182,0.0194285,0.0126596,0.0170681,0.0105907,0.0178451,0.0243238,0.0175426,0.0148092,0.0181425,0.0129192,0.0157145,0.0161999,0.0164487,0.01491,0.017404,0.0211997,0.014752,0.020534,0.0146299,0.0176126,0.0124723,0.0146959,0.0166721,0.0162578,0.0125495,0.0146785,0.0193055,0.0128848,0.0136318,0.0153526,0.0216564,0.0165873,0.0144524,0.0139854,0.0160625,0.016279,0.0119768,0.0148114,0.0141353,0.0130216,0.0149101,0.0131823,0.0139943,0.0153656,0.018857,0.0127698,0.0128125,0.0189265,0.0133817,0.0139091,0.0106123,0.0158698,0.0108936,0.013143,0.00983909,0.0115902,0.0140504,0.0171366,0.0128889,0.0116127,0.0124961,0.01253,0.0105337,0.0137814,0.00996275,0.0133298,0.0124853,0.0108473,0.013649,0.0124955,0.0134678,0.00824888,0.0077645,0.00918298,0.00963293,0.00843388,0.00959391,0.00948933,0.00988728,0.0101172,0.00907253,0.013946,0.00886676,0.0110614,0.00781223,0.0131206,0.015422,0.0124743,0.00834273,0.00902656,0.0129251,0.00956308,0.00935385,0.0074352,0.00862016,0.00876025,0.00835049,0.00941863,0.00891161,0.00898969,0.0105628,0.00702843,0.00903874,0.00871786,0.00866492,0.01328,0.0083914,0.0109371,0.00779622,0.0111106,0.00536558,0.00903934,0.00746824,0.00713485,0.00702754,0.00684363,0.00701948,0.00835977,0.00725414,0.00888644,0.0095847,0.00802107,0.00965408,0.00590953,0.00644022,0.00761196,0.0067303,0.00618673,0.00633582,0.00711953,0.00655924,0.00680705,0.00543914,0.00769489,0.0108524,0.00444473,0.0103886,0.00839198,0.00909031,0.00901947,0.00666165,0.00711782,0.00656482,0.00689957,0.00630466,0.00674515,0.00661042,0.00714152,0.00652369,0.00933843,0.00529772,0.00595744,0.00841638};

			// Create a new ICE object to hold the correlation mask
	IceObj::Pointer SyntheticCorrMaskObj ( Parc::HeapObject<IceObj>::Create());
	bool status = SyntheticCorrMaskObj->create( ICE_FORMAT_CXFL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object SyntheticCorrMaskAs in ActuationCloseLoop::chooseAntenna failed!");
	}
	IceAs SyntheticCorrMaskAs(SyntheticCorrMaskObj);
	SyntheticCorrMaskAs = (*SyntheticCorrMaskObj)();

			
	Ice.resetSoda(SyntheticCorrMaskAs);
	if(!Ice.SDC.calcSoda(SyntheticCorrMaskAs, magDataFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for SyntheticCorrMaskAs failed!");
	}

	// Find the peak width
	// magDataFTDerivativeObj : store the derivative of the magnitude of the FT of the srcData
	IceObj::Pointer magDataFTDerivativeObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataFTDerivativeObj->create( ICE_FORMAT_FL, COL, iceObjColLen-1);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object realDataFTObj failed!");
	}
	IceAs magDataFTDerivativeAs(magDataFTDerivativeObj);
	magDataFTDerivativeAs = (*magDataFTDerivativeObj)(); // specifies whole ima object for COL and LIN

	IceAs temp1As(magDataFTAs);
	IceAs temp2As(magDataFTAs); 

	temp1As.modify(COL, 0, iceObjColLen-1, 1);//,
	//CHA, channelID, 1, 1);
	temp2As.modify(COL, 1, iceObjColLen-1, 1);//,
	//CHA, channelID, 1, 1);

	magDataFTDerivativeAs.modify(COL, 0, iceObjColLen-1, 1);
	//CHA, channelID, 1, 1);
	Ice.Sub(magDataFTDerivativeAs, temp1As, temp2As);

	//Find the min and max on the derivative
	Ice.CalcStatistics(magDataFTDerivativeAs, IsmMinMax);
	Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
	Ice.GetStatisticsMinPos(lMinLigne, lMinCol);
	
	int peakWidth = (lMaxLigne - lMinLigne)*3;
	// Developing
	// Fill the new Ice object
	_complex fillValue, fillVoid;
	//fillValue.x = 1.0; fillValue.y = 0.0;
	//fillVoid.x = 0.0; fillVoid.y = 0.0;

	fillValue.y = 0.0;
	for(int i = 0; i < iceObjColLen; i++)
	{
		if(switchControl == 1) // mode simultaneous->Utilise le mask selon le segment
		{
			fillValue.x = tabPrj1[i];
		}
		else
		{
			if (prjAxis == 1)
			{
				fillValue.x = tabX[i];
			}
			else
			{
				fillValue.x = tabZ[i];
			}
		}
		SyntheticCorrMaskAs.modify(COL, i, 1, 1);
			//if (i < (iceObjColLen/2)+peakWidth && i > (iceObjColLen/2)-peakWidth)
			//{
		SyntheticCorrMaskAs.setValue(fillValue);
			//}
			//else
			//{
			//	SyntheticCorrMaskAs.setValue(fillVoid);
			//}
		
	}
	SyntheticCorrMaskAs.modify(COL, 0, iceObjColLen, 1);
	Ice.WriteToFile(SyntheticCorrMaskAs, "SyntheticCorrMask", 0, "ab");
	SyntheticCorrMaskAs.modify(COL, 0, iceObjColLen, 1);

	//Compute the inverse fourrier transform of the correlation mask
	IceObj::Pointer CorrMaskInverseFTObj ( Parc::HeapObject<IceObj>::Create());
	status = CorrMaskInverseFTObj->create( ICE_FORMAT_CXFL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object SyntheticCorrMaskAs in ActuationCloseLoop::chooseAntenna failed!");
	}
	IceAs CorrMaskInverseFTAs(CorrMaskInverseFTObj);
	CorrMaskInverseFTAs = (*CorrMaskInverseFTObj)();

			
	Ice.resetSoda(CorrMaskInverseFTAs);
	if(!Ice.SDC.calcSoda(CorrMaskInverseFTAs, magDataFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for SyntheticCorrMaskAs failed!");
	}
	CorrMaskInverseFTAs.modify(COL, 0, iceObjColLen, 1);
	Ice.FT(CorrMaskInverseFTAs, SyntheticCorrMaskAs,FT_Inverse);
    

	//Add imaginary part to 
	IceObj::Pointer magDataFTCXFLOBJ ( Parc::HeapObject<IceObj>::Create());
	status = magDataFTCXFLOBJ->create( ICE_FORMAT_CXFL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object magDataFTCXFLOBJ in ActuationCloseLoop::chooseAntenna failed!");
	}
	IceAs magDataFTCXFLOBJAs(magDataFTCXFLOBJ);
	magDataFTCXFLOBJAs = (*magDataFTCXFLOBJ)();

			
	Ice.resetSoda(magDataFTCXFLOBJAs);
	if(!Ice.SDC.calcSoda(magDataFTCXFLOBJAs, magDataFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for magDataFTCXFLOBJAs failed!");
	}

	float temp;
	_complex srcValue;
	for(int i = 0; i < iceObjColLen; i++)
	{
		magDataFTAs.modify(COL, i, 1, 1);
		srcCombineAs.modify(COL, i, 1, 1);
		magDataFTCXFLOBJAs.modify(COL, i, 1, 1);

		magDataFTAs.getValue(temp);
		srcCombineAs.getValue(srcValue);
		if(i == 0 || i == 100)
			ICE_OUT( "temp: " << temp);
		fillValue.x = temp;
		fillValue.y = srcValue.y;
		
		//if (i < (iceObjColLen/2)+peakWidth && i > (iceObjColLen/2)-peakWidth)
		//{
		magDataFTCXFLOBJAs.setValue(fillValue);
		//}
		//else
		//{
		//	SyntheticCorrMaskAs.setValue(fillVoid);
		//}
	}
	
	magDataFTCXFLOBJAs.modify(COL, 0, iceObjColLen, 1);
	Ice.FT(magDataFTCXFLOBJAs, magDataFTCXFLOBJAs, FT_Inverse);

	// Performe the correlation function in the frequency domain
	//Ice.ConjComplex(CorrMaskInverseFTAs, srcCombineAs);
	CorrMaskInverseFTAs.modify(COL, 0, iceObjColLen, 1);
	srcCombineAs.modify(COL, 0, iceObjColLen, 1);

	magDataFTCXFLOBJAs.modify(COL, 0, iceObjColLen, 1);

	//Ice.ConjComplMul(CorrMaskInverseFTAs, srcCombineAs, IPCMnormalized);
	Ice.ConjComplMul(CorrMaskInverseFTAs, magDataFTCXFLOBJAs, IPCMnormalized);
	
	Ice.FT(CorrMaskInverseFTAs, CorrMaskInverseFTAs);

	// magDataFTObj : store the magnitude of the Correlation function
	IceObj::Pointer magDataCorrObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataCorrObj->create( ICE_FORMAT_FL, CorrMaskInverseFTAs);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object magDataFTObj failed!");
	}
	IceAs magDataCorrAs(magDataCorrObj);
	magDataCorrAs = (*magDataCorrObj)(); // specifies whole ima object for COL and LIN

	//Store magnitude data
	Ice.ExtractComplex(magDataCorrAs, CorrMaskInverseFTAs, IemAmplitude, 1.0, 0);

	// Peak Detection to find the position
	Ice.CalcStatistics(magDataCorrAs, IsmMax);
	Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
	lPeakPos[0] = iceObjColLen - lMaxLigne;

	Ice.WriteToFile(magDataCorrAs, "Correlation", 0, "ab");
	Ice.WriteToFile(magDataFTAs, "RawData", 0, "ab");
	
	//Ice.CalcStatistics(magDataFTAs, IsmMax);
	//Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
	//lPeakPos[0] = lMaxLigne;
	
	
	//saveDataToFile(CorrMaskInverseFTAs,  iceObjColLen,  1, "CorrelationMaskAfterCorrelation");
	//saveDataToFile(magDataFTAs,  iceObjColLen,  1, "RawData");

}


void ActuationCloseLoop::findingCapsulePosition(IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl)
{
	long lMaxLigne = 0, lMaxCol = 0, lMinLigne = 0, lMinCol = 0;
	bool status;

/*
	//for(int i = 0; i<iceObjChaLen; i++)                     
	magDataFTAs.modify(COL, 0, iceObjColLen, 1);//,
		//CHA, channelID, 1, 1);
	// magDataFTDerivativeObj : store the derivative of the magnitude of the FT of the srcData
	IceObj::Pointer magDataFTDerivativeObj ( Parc::HeapObject<IceObj>::Create());
	status = magDataFTDerivativeObj->create( ICE_FORMAT_FL, magDataFTAs);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object realDataFTObj failed!");
	}
	IceAs magDataFTDerivativeAs(magDataFTDerivativeObj);
	magDataFTDerivativeAs = (*magDataFTDerivativeObj)(); // specifies whole ima object for COL and LIN

	IceAs temp1As(magDataFTAs);
	IceAs temp2As(magDataFTAs); 

	temp1As.modify(COL, 0, iceObjColLen-1, 1);//,
		//CHA, channelID, 1, 1);
	temp2As.modify(COL, 1, iceObjColLen-1, 1);//,
		//CHA, channelID, 1, 1);

	magDataFTDerivativeAs.modify(COL, 0, iceObjColLen-1, 1);
	//CHA, channelID, 1, 1);
	Ice.Sub(magDataFTDerivativeAs, temp1As, temp2As);

	//Find the min and max on the derivative
	Ice.CalcStatistics(magDataFTDerivativeAs, IsmMinMax);
	Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
	Ice.GetStatisticsMinPos(lMinLigne, lMinCol);

	ICE_OUT("Max position is " << lMaxLigne << "Min position is " << lMinLigne);
	lPeakPos[0] = (lMaxLigne + lMinLigne)/2;
	ICE_OUT("lPeakPos " << lPeakPos[0]);
*/
	magDataFTAs.modify(COL, 0, iceObjColLen, 1);

	Ice.CalcStatistics(magDataFTAs, IsmMax);
	Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
	lPeakPos[0] = lMaxLigne;

	//saveDataToFile(magDataFTDerivativeAs,  iceObjColLen,  1, "derivative");
}

/*
float ActuationCloseLoop::calcCNR(IceAs& CombineAs, long iceObjColLen)
{
	//IceSoda* sodaPtr = const_cast<IceSoda*>(magDataFTAs.calcCurrentSodaPtr());
	float maxVal, maxNoise, meanVal, stdDev, ratio = 0.0, temp = 0.0, CNR = 0.0, TheCNR = 0.0;
	bool isEmpty = true;

	CombineAs.modify(COL, 150, 200, 1);
	Ice.CalcStatistics(CombineAs, IsmMaxMeanStdDev);
	Ice.GetStatisticsMaxVal   (maxNoise);
	Ice.GetStatisticsMeanVal  (meanVal);
	Ice.GetStatisticsStdDevVal(stdDev);
	CombineAs.modify(COL, 0, iceObjColLen, 1);

	Ice.CalcStatistics(CombineAs, IsmMax);
	Ice.GetStatisticsMaxVal   (maxVal);

	CNR = fabs(maxVal - maxNoise)/stdDev;
	//saveDataToFile(CombineAs,  iceObjColLen,  1, "Combine");
	return CNR;
}
*/



float ActuationCloseLoop::calcCNR(IceAs& magDataFTAs, IceAs& CombineAs, long iceObjColLen, long iceObjChaLen)
{
	float maxVal, maxNoise, meanVal, stdDev, ratio = 0.0, CNR = 0.0, TheCNR = 0.0;
	bool isEmpty = true;
	IceObj::Pointer tempObj ( Parc::HeapObject<IceObj>::Create());
	bool status = tempObj->create( ICE_FORMAT_FL, COL, iceObjColLen);
	if(!status)
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Creation of ICE Object tempObj in ActuationCloseLoop::chooseAntenna failed!");
	}
	IceAs tempObjAs(tempObj);
	tempObjAs = (*tempObj)();

			
	Ice.resetSoda(tempObjAs);
	if(!Ice.SDC.calcSoda(tempObjAs, magDataFTAs))
	{
		ICE_OUT( "@@@@@@@@@@@@@@@@@@@@@Reset Soda for magDataFTCombineObjAs failed!");
	}


	//uint16_t antennaIndex = 0;
	//Calculate the mean and max of each antenna and return the index of the antenna with the largest ratio
	for(int i = 0; i<iceObjChaLen;i++)
	{
		//magDataFTAs.modify(COL, 0, iceObjColLen, 1,
			               //CHA, i, 1, 1);
		magDataFTAs.modify(COL, 1, 50, 1,
			               CHA, i, 1, 1);
		Ice.CalcStatistics(magDataFTAs, IsmMaxMeanStdDev);
		Ice.GetStatisticsMaxVal   (maxNoise);
	    Ice.GetStatisticsMeanVal  (meanVal);
	    Ice.GetStatisticsStdDevVal(stdDev);
		magDataFTAs.modify(COL, 0, iceObjColLen, 1,
			               CHA, i, 1, 1);
	    Ice.CalcStatistics(magDataFTAs, IsmMaxMeanStdDev);
		Ice.GetStatisticsMaxVal   (maxVal);

		ratio = maxVal/meanVal;
		CNR = fabs(maxVal - maxNoise)/stdDev;
		//ICE_OUT("CNR " << CNR );
		//if(CNR > 1)
		//{
			
		magDataFTAs.modify(COL, 0, iceObjColLen, 1,
							   CHA, i, 1, 1);

			Ice.PowerOf(tempObjAs,magDataFTAs,2.0);
			Ice.Mul(tempObjAs, tempObjAs, CNR);
			if (isEmpty == true)
			{
				Ice.Copy(CombineAs,tempObjAs);
				isEmpty = false;
			}
			else
			{
				Ice.Add(CombineAs, CombineAs, tempObjAs);
			}
		//}
			
		//if(ratio > temp)

		//lPeakPos[0] = lPeakPos[0] + lMaxLigne;
		//ICE_OUT("Antenna" << i << " maxVal: " << maxVal << " meanVal: "<< meanVal << " ratio: " << ratio << " stdDev: " << stdDev);

	}

CombineAs.modify(COL, 0, iceObjColLen, 1);
	Ice.PowerOf(CombineAs,CombineAs,0.5);
		
	CombineAs.modify(COL, 1, 50, 1);
		Ice.CalcStatistics(CombineAs, IsmMaxMeanStdDev);
		Ice.GetStatisticsMaxVal   (maxNoise);
	    Ice.GetStatisticsMeanVal  (meanVal);
	    Ice.GetStatisticsStdDevVal(stdDev);
		CombineAs.modify(COL, 0, iceObjColLen, 1);
	    Ice.CalcStatistics(CombineAs, IsmMaxMeanStdDev);
		Ice.GetStatisticsMaxVal   (maxVal);
		//ICE_OUT("Combined CNR " << CNR <<" TheCNR " << TheCNR);

		  //for (int i = 0; i < iceObjColLen; i++)
	//{
		CombineAs.modify(COL, 0, iceObjColLen, 1);
		Ice.WriteToFile(CombineAs, "CombinedChannels", 0, "ab");
		
	//}
		return fabs(maxVal - maxNoise)/stdDev;

	//ICE_OUT("antennaIndex " << antennaIndex);
	//ICE_OUT("TheCNR " << TheCNR);
	

}


double ActuationCloseLoop::applyRotationMatrix(double theta, double xAxis, double zAxis)
{
	return  xAxis*cos(theta) + zAxis*sin(theta);// RP for Rotation Plane.
}


IResult ActuationCloseLoop::sendFeedBackData(double CNR, uint16_t rotationPlane)
{
	FBData fbData;
	// Prepare feed back data
	fbData.lSlot = 0; //tells the sequence about the specific rotor center and radius found (0: not found, 1: found)
	fbData.lShot = 0; //tells the sequence about all rotors center and radius found (0: not found, 1: found)
	fbData.SimultORInterleaved = 0;//0: interleaved

	if (m_isRotorCTRL == true)
	{
		if(m_isCircleFound == true)
			fbData.lShot = 1;// radius and center of all rotors found
		if(m_isCircleFoundAllRotors[rotationPlane] == true)
		{
			fbData.lSlot = 1;	
			fbData.lShot = 1;// radius and center of all rotors found // DEBUG
		}
		m_kalmanFilters[rotationPlane].getControlInputs(fbData);
	}
	else // capsule control
	{
		fbData.lSlot = 1;//only closed loop mode for capsules
		fbData.lShot = 1;//only closed loop mode for capsules
		m_capsule.getControlInputs(fbData);
	}

	// fill seqData
	if(!m_pSeqData->setData(&fbData, sizeof(fbData)))
	{
		ICE_ERR("failed set data to seqData object");
		return I_FAIL;
	}

	ICE_OUT("double uValx[] ={ " << fbData.uValx[0] << ", " << fbData.uValx[1] << ", " << fbData.uValx[2] << ", " << fbData.uValx[3] << ", " << fbData.uValx[4] << ", " << fbData.uValx[5] << ", " << fbData.uValx[6] << ", " << fbData.uValx[7] << ", " << fbData.uValx[8] << ", " << fbData.uValx[9] << "};"            );
	ICE_OUT("double uValy[] ={ " << fbData.uValy[0] << ", " << fbData.uValy[1] << ", " << fbData.uValy[2] << ", " << fbData.uValy[3] << ", " << fbData.uValy[4] << ", " << fbData.uValy[5] << ", " << fbData.uValy[6] << ", " << fbData.uValy[7] << ", " << fbData.uValy[8] << ", " << fbData.uValy[9]  << "};"            );
	ICE_OUT("double uValz[] ={ " << fbData.uValz[0] << ", " << fbData.uValz[1] << ", " << fbData.uValz[2] << ", " << fbData.uValz[3] << ", " << fbData.uValz[4] << ", " << fbData.uValz[5] << ", " << fbData.uValz[6] << ", " << fbData.uValz[7] << ", " << fbData.uValz[8] << ", " << fbData.uValz[9]  << "};"            );
	// send data via feedback svc and return
	return m_pFeedbackSvc->Send(m_pSeqData);
}