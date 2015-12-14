//
//	 Project: MRI Biopsy needle Driver
//	    File: Kalman.h
//	 Version:
//	  Author: Ouajdi Felfoul, Children's Hospital Boston
//	    Date: 2014-01-14 
//
//	    Lang: C++
//
//	 Descrip: Kalman filter declaration based on MATLAB code by Aaron Becker
//
//
/// \brief  File containing declarations for the Kalman filter
//         
//  ***************************************************************************/
//

#ifndef __KALMAN_h
#define __KALMAN_h

#define MAX_KALMAN_EST	100
#define GRAD_AXIS		3 
#define MAX_NB_ROTOR    3
#define _USE_MATH_DEFINES

#include"CommonDefs.h"
#include"rotor.h"
#include "MrServers/MrVista/include/Parc/Trace/IceTrace.h"
#include "feedbackData.h"
#include <fstream>
#include <stdlib.h>
#include <iostream>


class Kalman
{
public:
	Kalman(void);
	~Kalman(void);
	//Setters
	void setKalmanGain(double K);
	void setVelocityKalmanGain(double K);
	void setTimeSuccTracking(double tMeas);
	void setTimeBetweenCtrlUpdate(double tControl);
	void setTrackingDuration(double TrackDuration);
	void setFeedBackDelay(double delay);
	void setThetaEstimate(double thetaEstimate);
	void setVelocityEstimate(double velocityEstimate);
	void setFBLinearGainK1(double K1);
	void setFBLinearGainK2(double K2);
	void setPGain(double P);
	void setIGain(double I);
	void setDGain(double D);
	void setTargetTheta1(double theta1);
	void setTargetTheta2(double theta2);
	void setTargetTheta(double theta1, double theta2, double theta3);
	void setNbRotors(int nb);
	void setControlType(CONTROL_TYPE type);
	void setMaxGradAmplitude(double amp);
	void setRotorStaticFriction(double statFric);
	void setRotorViscousFriction(double visFric);
	void setRotorLoad(double load);
	void setRotorRadiusArm(double rra);
	void setAngleMode(int mode);
	void setThetaProjection(double theta); // set angle used for the simultaneous actuation and tracking
	void setTargetPositionZ(double posZ);
	void setTargetPositionX(double posX);

	//Getters 
	double getKalmanGain(void);
	double getVelocityKalmanGain(void);
	double getTimeSuccTracking(void);
	double getTimeBetweenCtrlUpdate(void);
	double getTrackingDuration(void);
	double getFeedBackDelay(void);
	double getThetaEstimate(void);
	double getVelocityEstimate(void);
	double getFBLinearGainK1(void);
	double getFBLinearGainK2(void);
	double getPGain();
	double getIGain();
	double getDGain();
	double getTargetTheta1(void);
	double getTargetTheta2(void);
	double getThetaProjection(void); //Get angle used for the simultaneous actuation and tracking

	CONTROL_TYPE getControlType();

	bool initializeFilter(); // Read from a Config file the rotors and Kalman parameters
	void initializeFilter(double m_position[3][2], double m_positionKminus1[3][2], double m_positionKminus2[3][2], double dm_center[3][2]);// Initialise the theta and theta velocity

	void initializeTheta(double theta, unsigned int index);
	void predictControlLaw(double thetaMeas, int direction, float CNR, rtPlane plane, rotor& rotor1);
	void predictControlLaw(double thetaMeas, int direction, float CNR, rtPlane plane);
	void predictControlLaw(double thetaMeas, int direction, float CNR);
	void predictControlLawfor2Rotors(double thetaMeas1, double thetaMeas2);
	void predictControlLaw3OrthoRotors(double thetaMeas[MAX_NB_ROTOR], float CNR[MAX_NB_ROTOR]);

	void predictControlLawForSimulImagingAndActuation(double thetaMeas, int direction, float CNR);

	//void setNbRotors(int nb);
	int  getNbRotors();

	void getControlInputs(FBData& controlInputs);
	void PI1Capsule (double positionX, double positionZ);

	double constrainAngle(double x);
	bool isGoodEstimate();

	double chooseAngle(double angleSolution1, double angleSolution2);
private:
		
	void maxTorqueControl (double theta, double& GradX, double& GradY, double& GradZ, int direction);
	void maxTorqueControl (double theta, double& gradX, double& gradY, double& gradZ, int direction, rtPlane plane);
	void fbLinearization2Rotors  (double theta1, double theta2, double dtheta1, double dtheta2, double& GradX, double& GradY, double& GradZ);
	void fbLinearization1Rotor   (double theta1, double dTheta1, double& GradX, double& GradY, double& GradZ);
	void PI1Rotor   (double theta1, double velocity, double& GradX, double& GradY, double& GradZ);
	double secondOrderFinitDiffVelocity(double thetaKminus2, double thetaKminus1, double thetaK);
    void LyapunovPositionControl(double theta, double velocity, double& gradX, double& gradY, double& gradZ);
    void LyapunovVelocityControl(double theta, double velocity, double& gradX, double& gradY, double& gradZ);
	void LyapunovPositionControlMultiRotors(double theta[], double velocity[], double& gradX, double& gradY, double& gradZ);
    void LyapunovVelocityControlMultiRotors(double theta[], double velocity[], double& gradX, double& gradY, double& gradZ);

	int  signOf(double x);


	double angleConv(double angle);
	double angleDiff(double a, double b);
	double unwrap(double previousAngle, double newAngle);

	double angleCorrectionForXandZDelay(double x, double z, double velocity, double timeDelay, double& delayEstimatedVelocity);
	double angleCorrectionForProjectionsDelay(double thetaMeas, double thetaEstimate, double velocityEstimate, double timeDelay);
	
	bool   m_isInitialized[MAX_NB_ROTOR];
	double m_gain;			// K = How much I trust the measurement/ How much I trust the model
	double m_Kvelocity;     // Kalman Gain,intuitively, how much more reliable is the measurement than the estimate
	double m_tMeas;			// time between successive tracking
	double m_tControl;		// Time between CTRL update
	double m_noControlCycles; // numControlCycles = m_tMeas/m_tControl;
	double m_tTracking; // Duration of the tracking sequence.

	// PI controller gains
	double m_PGain;
	double m_IGain;
	double m_DGain;
	double m_intPI;
	double m_intPIX;
	double m_intPIZ;
	double m_maxGradAmplitude;
	// Kalman state estimation
	double m_thetaEstimate[MAX_NB_ROTOR];	//
	double m_thetaKminus1[MAX_NB_ROTOR];
	double m_thetaKminus2[MAX_NB_ROTOR];
	double m_velocityEstimate[MAX_NB_ROTOR];//
	double m_covariancePosition[MAX_NB_ROTOR];
	double m_covarianceVelocity[MAX_NB_ROTOR];
	double m_feedBackDelay; // delay between the ADC and the next actuation bloc
	double m_feedBackDelayMultiRotors[MAX_NB_ROTOR];
	double m_thetaEstimateAtTrackingTime;
	bool   m_isGoodEstimate;
    // Simultaneous tracking and actuation
	double m_thetaProjection;
	rotor  m_rotor[MAX_NB_ROTOR];
	int	   m_nbRotors;	
	double m_uVals[MAX_KALMAN_EST][GRAD_AXIS]; // Array of normalized gradient intensities; input for the sequence.
	double m_targetTheta [MAX_NB_ROTOR]; // desired angle for each rotor
	double m_targetXPosition;
	double m_targetYPosition;
	double m_targetZPosition;

	//
	double m_gainFBLinear[2];			// K = How much I trust the measurement/ How much I trust the model

	int m_angleMode; // 1 : -pi to pi, 0: 0 to 2pi
    CONTROL_TYPE m_controlType;


};

#endif