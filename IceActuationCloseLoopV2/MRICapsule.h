//
//	 Project: MRI Capsule control
//	    File: MRICapsule.h
//	 Version:
//	  Author: Ouajdi Felfoul, Children's Hospital Boston
//	    Date: 2014-07-09 
//
//	    Lang: C++
//
//	 Descrip: 
//
//
/// \brief  File containing declarations for the MRI's capsule control
//         
//  ***************************************************************************/
//

#ifndef __MRI_CAPSULE_h
#define __MRI_CAPSULE_h

// IceScanFunctors
#include "MrServers/MrVista/include/Ice/IceUtils/IceScanFunctors.h"
// IFeedback
#include "MrServers/MrVista/include/Iris/Services/IFeedback.h"
// IceObj::Pointer
#include "MrServers/MrVista/include/Ice/IceBasic/IceObj.h"
// IceStorage
#include "MrServers/MrVista/include/Ice/IceBasic/IceStorage.h"
// IceAsData
#include "MrServers/MrVista/include/Ice/IceBasic/IceAsData.h"
// IceWrapper
#include "MrServers/MrVista/include/Ice/IceAlgos/IceWrapper.h"
#include "MrServers/MrVista/include/Parc/Trace/IceTrace.h"

#include "CommonDefs.h"
#include "feedbackData.h"


class MRICapsule
{
public:
	MRICapsule(void);
	~MRICapsule(void);

	void findCapsulePosition(IceAs& srcCombineAs, IceAs& magDataFTAs, long iceObjColLen);
	void findCapsulePosition(IceAs& srcCombineAs, IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis);

	void init(long iceObjColLen, long iceObjChaLen, bool alternateOrSimult, uint8_t nbCapsules);
	
	void getControlInputs(FBData& controlInputs);
	void applyControlLaw(uint16_t prjAxis, uint16_t prjNumber);
	void applyPIDControl(uint16_t prjAxis);

	//Setters
	void setNbWayPoints(uint8_t nbWayPoint);
	void setWayPoints(uint16_t wayPoint, uint8_t index);
	void setPrjAngles(uint16_t prjAngle, uint8_t index);
	void setPGain(double gain);
	void setDGain(double gain);
	void setMaxGradAmplitude(double max);
	void setSpoilerDuration(double Ts);
	void setImgMomentum(double momentum);


private:
	//Utility method
	void createCorrMask();
	void indexTo_mm(long id, long prjAxis);// 

	//General declaration
	uint8_t m_nbCapsules;
	long m_iceObjColLen;
	long m_iceObjChaLen;
	bool m_actuationMode; // 0: Alternate, 1: simultaneous
	//position
	double m_positionIndex[NB_MAX_CAPSULES][NB_AXIS]; // Support up to NB_MAX_CAPSULES(3) capsules and stores the x, y and z index positions
	double m_position[NB_MAX_CAPSULES][NB_AXIS]; // Support up to NB_MAX_CAPSULES(3) capsules and stores the x, y and z positions in mm
	double m_positionKMinus1[NB_MAX_CAPSULES][NB_AXIS]; // Support up to NB_MAX_CAPSULES(3) capsules and stores the x, y and z previous positions in mm
	double m_positionKMinus2[NB_MAX_CAPSULES][NB_AXIS]; // Support up to NB_MAX_CAPSULES(3) capsules and stores the x, y and z before previous positions in mm
	
	double m_velocity[NB_MAX_CAPSULES][NB_AXIS]; // Support up to NB_MAX_CAPSULES(3) capsules and stores the x, y and z positions in mm
	//controller
	int m_state; // Holds the waypoints state
	// PI controller gains
	double m_PGain;
	double m_IGain;
	double m_DGain;
	double m_intPI;
	double m_intPIX;
	double m_intPIZ;
	double m_maxGradAmplitude;
	double m_spoilerTs;
	double m_imagingMomentum;
	int	   m_direction;
	int    m_counter;
	int    m_prjLag;
	double m_uValx;
	double m_uValy;
	double m_uValz;

	long	m_boundariesMinus[2];
	long	m_boundariesPlus[2];

	SequenceType m_sequenceType;
	//double m_mask[MAX_NB_WAYPOINTS][PRJ_SIZE]; //Correlation mask associated with each projection
	// Provided for user convenience
    IceWrapper Ice;

	//Path info
	uint8_t  m_nbWayPoints;
	uint16_t m_prjAngles[MAX_NB_WAYPOINTS]; //angles to be used for simulatneous actuation and imaging
	uint16_t m_wayPoints[MAX_NB_WAYPOINTS]; //intermediate targets in the coordinate of the projections angle 
	//Correlation masks
	double** m_maskPrj; // For capsule 1
	double** m_maskb2Prj;// For capsule 2
	double** m_maskPrjInv;//debug only
	/*
	double** m_SEmaskX;
	double** m_SEmaskY;
	double** m_SEmaskZ;
	double** m_GREmaskX;
	double** m_GREmaskY;
	double** m_GREmaskZ;
*/



};

#endif