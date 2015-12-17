//
//	 Project: MRI Capsule control
//	    File: MRICapsule.h
//	 Version:
//	  Author: Ouajdi Felfoul, Children's Hospital Boston
//	    Date: 2015-04-28 
//
//	    Lang: C++
//
//	 Descrip: 
//
//
/// \brief  File containing declarations for the MRI's capsule control
//                                  (Support up to 2 capsules)
//         
//  ***************************************************************************/
//

#ifndef __MRI_2CAPSULES_h
#define __MRI_2CAPSULES_h

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

#include <Eigen/Dense>

#define NB_CAPSULES 2
#define NB_PRJ_AXIS 2 // x and z
#define OVERSAMPLED_FOV 500

class MRI2Capsules
{
public:
	MRI2Capsules(void);
	~MRI2Capsules(void);

	void findCapsulePosition(IceAs& srcCombineAs, IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, int capsuleID);

	void init(long iceObjColLen, long iceObjChaLen, bool alternateOrSimult);
	
	void getControlInputs(FBData& controlInputs);
	void applyControlLaw(uint16_t prjAxis);
	
	//george's implementation of MPC controller
	void applyMPCControl(double* error, double* U0, double gradient, double* U);

	//Setters
	void setPGain(double gain);
	void setDGain(double gain);
	void setMaxGradAmplitude(double max);
	void setSpoilerDuration(double Ts);
	void setImgMomentum(double momentum);

	void setR1(double radius);
	void setR2(double radius);
	void setMPCHorizon(int N);
	void setGainIterative(double gain);
	void setGainGlobal(double gain);
	void setGainP(double gain);
	void setGainQ(double gain);
	
// George's code to test compiling Eigen on Siemens
	::Eigen::MatrixXf R;
	double* m_U_pred; 
	double* m_U0;
private:
	//Utility method
	void createCorrMask();
	void indexTo_mm(long id, long prjAxis);// fills m_position, m_positionKMinus1 and m_positionKMinus2

	// Controller methods
	void computePositionError();
	void computeVelocity();

	//General declaration
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
 
	// PID controller variables
	double m_PGain;
	double m_IGain;
	double m_DGain;

	// General Control
	double m_positionError[NB_MAX_CAPSULES][NB_AXIS];
	double m_goal[NB_MAX_CAPSULES][NB_AXIS];
	
	double m_spoilerTs;


	double m_intPI;
	double m_intPIX;
	double m_intPIZ;
	double m_maxGradAmplitude;
	double m_periodSec;
	double m_imagingMomentum;		//this is to compute the energy sent during imaging
	int	   m_direction;
	int    m_counter;
	int    m_prjLag;
	double m_uValx;
	double m_uValy;
	double m_uValz;
	int    m_nbCapsules;
	long	m_boundariesMinus[2];
	long	m_boundariesPlus[2];

	SequenceType m_sequenceType;
	//double m_mask[MAX_NB_WAYPOINTS][PRJ_SIZE]; //Correlation mask associated with each projection
	// Provided for user convenience
    IceWrapper Ice;

	//Correlation masks
	double** m_maskPrj;

	// MPC controller parameters
	double			m_R1;
	double			m_R2;
	int				m_MPC_horizon;
	double			m_k_iter;
	double			m_global_gain;
	double			m_kq;
	double			m_kp;
	
};

#endif