//  ---------------------------------------------------------
//    Copyright (C) Siemens AG 1998  All Rights Reserved.
//  ---------------------------------------------------------
//
//   Project: NUMARIS/4
//      File: 
//   Version:
//    Author: SeSo_General 
//      Date: n.a. 
//
//      Lang: C++
//
//   Descrip:   
//
//   Classes: ActuationCloseLoop
//
//  ---------------------------------------------------------

#ifndef __ICEPROGACTUATIONCLOSELOOP_H__
#define __ICEPROGACTUATIONCLOSELOOP_H__

// IceScanFunctors
#include "MrServers/MrVista/include/Ice/IceUtils/IceScanFunctors.h"
// IFeedback
#include "MrServers/MrVista/include/Iris/Services/IFeedback.h"

#include "CommonDefs.h"
#include "Kalman.h"
#include "rotor.h"
#include "CircleFit.h"
#include "MRICapsule.h"
#include "MRI2Capsules.h"

#include <iostream>


// fwd decl
class SeqData;



//*****************************************************************************
/*! 
 *  The ActuationCloseLoop functor sends data to feedback service
 *  the parc framework
 */
//*****************************************************************************
class ActuationCloseLoop : public IceScanFunctors
{
public:
    ICE_SCAN_FUNCTOR(ActuationCloseLoop);

    ActuationCloseLoop();
    virtual ~ActuationCloseLoop();

    // ---------------
    // - preparation -
    // ---------------
    virtual IResult BeginInit( IParcEnvironment* env );
    virtual IResult EndInit  ( IParcEnvironment* env );

    // --------------
    // - event sink -
    // --------------
    virtual IResult ComputeScan(IceAs& srcAs, MdhProxy& aMdh, ScanControl& ctrl);
	
	void saveDataToFile(IceAs& tempAs, long iceObjColLen, long iceObjChaLen, char* filename);

	bool isInitialisationDone(); // return the status
    
private:
	IResult sendFeedBackData(double CNR, uint16_t rotationPlane);

	FBData m_fbData;

    IFeedback::Pointer m_pFeedbackSvc;    ///< Ref to feedback service object
    
    SEQData            * m_pSeqData;      ///< container for feedback data

	uint16_t		    m_callLoop; // counter for call to the ice prog
	uint16_t		    m_loopCircleFound; // counter from the moment the circle is found
	// If initilisation not done, call the circle fit function
	bool                m_isCircleFound;
	bool				m_isCircleFoundAllRotors[MAX_NB_ROTOR];
	// dynamic array, size and capacity
	double**            m_Coordinates; // Buffer for the circular fitting
	int					m_nbElem;      // number of points used for circular fit.
	int					m_capacity;    // max capacity for the dynamically allocated buffer
	int					m_nbRotors;    // number of controlled rotors
	uint8_t				m_nbCapsules;  // number of controlled capsules
//	void circleFitByTaubin (double& centerX, double& centerY, double& radius);
	int                 m_targetAngleIndex;
	double				m_targetArray[100];
//	bool circleFitCheckConvergence(double centerX, double centerY, double radius);
	// Rotor position (center and radius)
	double              m_center[MAX_NB_ROTOR][2];
	double				m_radius[MAX_NB_ROTOR];
	// Actual position of the rotor
	double				m_position[MAX_NB_ROTOR][2];// m_position[0][0] -> position x rotor 1, m_position[2][1] -> position y, rotor 3...
	double				m_positionKminus1[MAX_NB_ROTOR][2];// save the previous 2 positions for a second order finit difference velocity estimation
	double				m_positionKminus2[MAX_NB_ROTOR][2];// 

	long				m_boundariesMinus[MAX_NB_ROTOR][2];
	long				m_boundariesPlus[MAX_NB_ROTOR][2];

	MRICapsule          m_capsule;
	MRI2Capsules        m_MPC_Capsules;

	rotor				m_rotors[3]; //X, Y and Z rotors
	Kalman				m_kalmanFilter;
	Kalman				m_kalmanFilters[3];
	CircleFit			m_circleFit[MAX_NB_ROTOR];
  
	CONTROL_TYPE		m_controlType; // Control type experiment, for example, Open-loop, closed-loop max torque...
	uint16_t			m_experimentType;
	bool				m_isRotorCTRL;
	bool				m_isCapsuleCTRL;
	bool                m_isSimultaneousTrackActuation;
	bool                m_isMultiAxis;
	int 				m_spinningDirection; // 1:CW, -1: CCW

	void processingConfigurationData(MdhProxy& aMdh, long iceObjColLen, uint16_t prjAxis);
	bool checkingConsistency();
	void findingPeakPosition   (IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl);
	void findingCapsulePosition(IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl);
	void constructCorrelationMask(IceAs& srcCombineAs, IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, long lPeakPos[MAX_NB_ROTOR], uint16_t switchControl);

	//float  chooseAntenna(IceAs& srcAs, long iceObjColLen, long iceObjChaLen, IceAs& CombineAs, uint16_t& antennaIndex);
	//uint16_t chooseAntenna(IceAs& magDataFTAs, long iceObjColLen, long iceObjChaLen, float& CNR);
	float calcCNR(IceAs& magdataAs, IceAs& CombineAs, long iceObjColLen, long iceObjChaLen);

	double applyRotationMatrix(double theta, double xAxis, double zAxis);


};

#endif //__FBPERF_H__
