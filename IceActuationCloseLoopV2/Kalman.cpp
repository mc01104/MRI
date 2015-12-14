//  Project: NUMARIS/4
//
//  File        : \n4\pkg\MrServers\MrImaging\seq\a_se_15b130\a_se_b130_15000.cpp
//
//  Author      : Ouajdi Felfoul, Children's Hospital Boston
//
//  Date        : 2014-01-14 
//
//  Lang        : cpp
//
// \file   Kalman.cpp
/// \brief  Kalman filter for 1 Rotor motion prediction based on MATLAB code made by 
///         Aaron Becker.
/// This file contains the implementation of the Kalman filter.
///
//  **************************************************************************


#include "Kalman.h"

#define MAX_TORQUE_CONTROL 1
#define FBLINEARIZATION_CONTROL 2
#define PI_CONTROL 3

Kalman::Kalman(void)
{
	m_gain		= 100;	  // K = How much I trust the measurement/ How much I trust the model
	m_Kvelocity = 10;
	m_tMeas		= 0.065;	      //seconds. TODO sequence dependent variable	
	m_tControl	= m_tMeas/10.0; //seconds. TODO sequence dependant variable
	m_noControlCycles = m_tMeas/m_tControl;
	m_tTracking = 0.02; // Default 20 ms for two axis
	m_nbRotors = 2;
	m_thetaProjection = 0.0; // Angle that defines the projection direction for simu. actuation and tracking

	for(int i = 0; i<MAX_KALMAN_EST; i++)
		for(int j = 0; j<GRAD_AXIS; j++)
			m_uVals[i][j] = 0.0;

	for(int i = 0; i<MAX_NB_ROTOR; i++)
	{
		m_thetaEstimate[i] = 0.0;
		m_velocityEstimate[i] = 0.0;
		m_covariancePosition[i] = 1.0;
		m_covarianceVelocity[i] = 1.0;
		m_targetTheta[i] = 0.0;
		m_thetaKminus2[i] = 0.0;
		m_thetaKminus1[i] = 0.0;
		m_isInitialized[i] = false;
	}
    m_rotor[0].setBearingRadius(0.005);
	m_rotor[0].setSaturationMag(1360000);
	m_rotor[0].setBearingDensity(8530);
	m_rotor[0].setRadiusRotorArm(0.032);//0.018
	m_rotor[0].setLoadTorque(0);
	m_rotor[0].setStaticFriction(0.00008);
	m_rotor[0].setViscousFriction(0.00001);

	m_rotor[1].setBearingRadius(0.005);
	m_rotor[1].setSaturationMag(1360000);
	m_rotor[1].setBearingDensity(8530);
	m_rotor[1].setRadiusRotorArm(0.032);
	m_rotor[1].setLoadTorque(0);
	m_rotor[1].setStaticFriction(0.00008);
	m_rotor[1].setViscousFriction(0.00001);

	m_rotor[2].setBearingRadius(0.005);
	m_rotor[2].setSaturationMag(1360000);
	m_rotor[2].setBearingDensity(8530);
	m_rotor[2].setRadiusRotorArm(0.032);
	m_rotor[2].setLoadTorque(0);
	m_rotor[2].setStaticFriction(0.00008);
	m_rotor[2].setViscousFriction(0.00001);


	m_maxGradAmplitude = 20.0; // mT/m

	m_gainFBLinear[0] = -0.0224;			// K = Feedback linearization gain
	m_gainFBLinear[1] = -0.1512;

	m_targetTheta[0] = 0.0;
	m_targetTheta[1] = 0.0;

	m_targetXPosition = 0.0;
	m_targetYPosition = 0.0;
	m_targetZPosition = 0.0;

	m_thetaEstimateAtTrackingTime = 100; //Large value so the comparaison between the estimate and the measurement will fail the first cycle
	m_isGoodEstimate = false;
	//m_DGain = 0;
	m_intPI = 0.0; // Set the integral error of the PI controller to 0
	m_intPIX = 0.0;
	m_intPIZ = 0.0;
	//m_controlType = OPEN_LOOP;
	m_feedBackDelay = 0.033; // 33 ms but it will be updated by the sequence
	m_feedBackDelayMultiRotors[0] = 0.05565;
	m_feedBackDelayMultiRotors[1] = 0.03778;
	m_feedBackDelayMultiRotors[2] = 0.02;

	m_angleMode = 0; // default : 0 to 2pi
}

Kalman::~Kalman(void)
{
}


bool Kalman::initializeFilter()
{
	bool returnValue = true;
	char buffer[100];
	double dValue;
	//std::ifstream configFile("C:\\MedCom\\MriCustomer\\ice\\ConfigRotor.txt");// Compilation in the MRI
	//std::ifstream configFile("Z:\\n4\\x86\\prod\\bin\\ConfigRotor.txt");
	std::ofstream CECIESTUNTEST;
	std::ifstream configFile;
	/*
	char * descr = getenv("CustomerIceProgs");
	if (descr) // Works on the scanner
	{
		ICE_OUT( "JOUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUD " << descr);
		ICE_OUT( "value of PATH is: " << descr); 
	}
	else  // Works in standalone PC
	{
		ICE_OUT( "JOUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUD " << descr);
		ICE_OUT( "variable not defined"); 
	}
	*/
	CECIESTUNTEST.open("CECIESTUNTEST.txt");
	configFile.open("ConfigRotor.txt");
	
	//configFile.open("Z:\\n4\\x86\\prod\\bin\\ConfigRotor.txt");
//Use path : //opt//medcom//temp/
	if(!configFile.fail()) //open success
	{
		configFile >> buffer >> m_nbRotors; // read the nbRotor value
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setBearingRadius(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setSaturationMag(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setBearingDensity(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setRadiusRotorArm(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setLoadTorque(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setStaticFriction(dValue);
		}
		configFile >> buffer;
		for (int i = 0; i<m_nbRotors; i++)
		{
			configFile >> dValue;
			m_rotor[i].setViscousFriction(dValue);
		}
		configFile >> buffer >> m_gain;
		configFile >> buffer >> m_gainFBLinear[0];
		configFile >> buffer >> m_gainFBLinear[1];
		configFile >> buffer >> m_targetTheta[0];
		configFile >> buffer >> m_targetTheta[1];
		m_noControlCycles = m_tMeas/m_tControl;
		configFile >> buffer >> m_maxGradAmplitude;
	}
	else
		returnValue = false;

	return returnValue;

}


void Kalman::initializeTheta(double theta, unsigned int index)
{

	if(m_isInitialized[index] != true)
	{		
		m_thetaKminus2[index]  = theta;
		m_thetaKminus1[index]  = theta;
		m_thetaEstimate[index] = theta;
		m_isInitialized[index] = true;
	}

}

void Kalman::setKalmanGain(double K)
{
	m_gain = K;
	return;
}

double Kalman::getKalmanGain()
{
	return m_gain;
}
void Kalman::setVelocityKalmanGain(double K)
{
	m_Kvelocity = K;
	return;
}

double Kalman::getVelocityKalmanGain()
{
	return m_Kvelocity;
}

void Kalman::setPGain(double P)
{
	m_PGain = P;
	return;

}

void Kalman::setDGain(double D)
{
	m_DGain = D;
	return;

}
void Kalman::setIGain(double I)
{
	m_IGain = I;
	return;
}
double Kalman::getPGain()
{
	return m_PGain;

}
double  Kalman::getIGain()
{
	return m_IGain;
}
double  Kalman::getDGain()
{
	return m_DGain;
}
void Kalman::setTimeSuccTracking(double tMeas)
{
	m_tMeas = tMeas; // in s
	m_tControl	= tMeas/10.0; //seconds
	return;
}

double Kalman::getTimeSuccTracking()
{
	return m_tMeas;
}


void Kalman::setTimeBetweenCtrlUpdate(double tControl)
{
	m_tControl = tControl;
	return;
}
double Kalman::getTimeBetweenCtrlUpdate()
{
	return m_tControl;
}

/** @brief calculates a magnetic gradient that applies the maximum torque to a rotor at angle theta
 *  @input theta, the current position measurement
 *
 * Direction 1 moves CW, -1 moves CCW, 0 makes it stop.
 */
void Kalman::maxTorqueControl(double theta, double& gradX, double& gradY, double& gradZ, int direction)
{
 /*	
	double perpDirection = theta+M_PI/2; //torque is maximized when gradient is 90 deg ahead of rotor angle
	gradX = sin(perpDirection);
	gradY = 0;
	gradZ = cos(perpDirection);
*/	
    gradX = -direction*sin(theta);
	gradY = 0;
	gradZ = direction*cos(theta);


	return;
}

void Kalman::maxTorqueControl(double theta, double& gradX, double& gradY, double& gradZ, int direction, rtPlane plane)
{
 /*	
	double perpDirection = theta+M_PI/2; //torque is maximized when gradient is 90 deg ahead of rotor angle
	gradX = sin(perpDirection);
	gradY = 0;
	gradZ = cos(perpDirection);
	*/	
	if(plane == X_Z)
	{
		//Spin around the y-axis
		gradX = -direction*sin(theta);
		gradY = 0;
		gradZ = direction*cos(theta);

	}
	if(plane == X_Y)
	{
		//Spin around the z-axis
		gradX = -direction*sin(theta);
		gradY = direction*cos(theta);
		gradZ = 0;
	}
	if(plane == Y_Z)
	{
		//Spin around the x-axis
		gradX = 0;
		gradY = direction*cos(theta);
		gradZ = -direction*sin(theta);
	}

	return;
}

void Kalman::LyapunovPositionControl(double theta, double velocity, double& gradX, double& gradY, double& gradZ)
{
	double thetaX = theta, thetaY = 0, thetaZ = 0;
	double errX = m_PGain * (m_targetTheta[0] - theta) - velocity;
	double errY = 0;
	double errZ = 0;
	gradX = -signOf(errY*sin(thetaY) - errZ*cos(thetaZ)) * m_maxGradAmplitude;
	gradY = -signOf(errZ*sin(thetaZ) - errX*cos(thetaX)) * m_maxGradAmplitude;
    gradZ = -signOf(errX*sin(thetaX) - errY*cos(thetaY)) * m_maxGradAmplitude;
    ICE_OUT("err "   << errX << " P " << m_PGain << " target " << m_targetTheta[0] << " gradX" << gradX << " gradY" << gradY << " gradZ" << gradZ);

}


void Kalman::LyapunovVelocityControl(double theta, double velocity, double& gradX, double& gradY, double& gradZ)
{
	double thetaX = theta, thetaY = 0, thetaZ = 0;
	// m_targetTheta are desired velocities
	double errX = m_targetTheta[0] - velocity;
	double errY = 0;
	double errZ = 0;
	gradX = -signOf(errY*sin(thetaY) - errZ*cos(thetaZ)) * m_maxGradAmplitude;
	gradY = -signOf(errZ*sin(thetaZ) - errX*cos(thetaX)) * m_maxGradAmplitude;
    gradZ = -signOf(errX*sin(thetaX) - errY*cos(thetaY)) * m_maxGradAmplitude;
    ICE_OUT("err "   << errX << " gradX" << gradX << " gradY" << gradY << " gradZ" << gradZ);

}

void Kalman::LyapunovPositionControlMultiRotors(double theta[MAX_NB_ROTOR], double velocity[MAX_NB_ROTOR], double& gradX, double& gradY, double& gradZ)
{
	double thetaX = theta[0], thetaY = theta[1], thetaZ = theta[2];
	double errX = m_PGain * (m_targetTheta[0] - thetaX) - velocity[0];
	double errY = m_PGain * (m_targetTheta[1] - thetaY) - velocity[1];
	double errZ = m_PGain * (m_targetTheta[2] - thetaZ) - velocity[2];

	gradX = -signOf(errY*sin(thetaY) - errZ*cos(thetaZ)) * m_maxGradAmplitude;
	gradY = -signOf(errZ*sin(thetaZ) - errX*cos(thetaX)) * m_maxGradAmplitude;
    gradZ = -signOf(errX*sin(thetaX) - errY*cos(thetaY)) * m_maxGradAmplitude;
    
	ICE_OUT("err "   << errX << " P " << m_PGain << " target " << m_targetTheta[0] << " gradX" << gradX << " gradY" << gradY << " gradZ" << gradZ);

}
void Kalman::LyapunovVelocityControlMultiRotors(double theta[MAX_NB_ROTOR], double velocity[MAX_NB_ROTOR], double& gradX, double& gradY, double& gradZ)
{
	double thetaX = theta[0], thetaY = theta[1], thetaZ = theta[2];
	// m_targetTheta are desired velocities
	    
	ICE_OUT("thetaX "   << theta[0] << " thetaY" << theta[1] << " thetaZ " << theta[2]);
	ICE_OUT("velocityX "   << velocity[0] << " velocityY" << velocity[1] << " velocityZ " << velocity[2]);

	double errX = m_targetTheta[0] - velocity[0];
	double errY = m_targetTheta[1] - velocity[1];
	double errZ = m_targetTheta[2] - velocity[2];

	gradX = -signOf(errY*sin(thetaY) - errZ*cos(thetaZ)) * m_maxGradAmplitude;
	gradY = -signOf(errZ*sin(thetaZ) - errX*cos(thetaX)) * m_maxGradAmplitude;
    gradZ = -signOf(errX*sin(thetaX) - errY*cos(thetaY)) * m_maxGradAmplitude;

    ICE_OUT("errX, Y, Z "   << errX << ", "   << errY << ", "   << errZ << " gradX, Y, Z" << gradX << ", " << gradY << ", " << gradZ);

}
int Kalman::signOf(double x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}
void Kalman::PI1Rotor(double theta, double velocity, double& gradX, double& gradY, double& gradZ)
{
	double err = m_targetTheta[0] - theta;
	const double INTEGRAL_MAX = M_PI/4.0;

	err = constrainAngle(err);	// ensure that  -pi<=err<=pi

	// ensure that the integral error has the same sign as the error.
	if (err*m_intPI < 0)
		m_intPI = 0.0;

	m_intPI += err; // integrate error

	if (m_intPI > INTEGRAL_MAX ) // Prevent integral windup
		m_intPI = INTEGRAL_MAX; 
	else if (m_intPI < -INTEGRAL_MAX )
		m_intPI = -INTEGRAL_MAX; 

	double u = (m_PGain * err)+ (m_IGain * m_intPI) - (m_DGain * velocity);

	// Saturate u here.  Abs(u)<= m_maxGradAmplitude

    //Saturate the outputs
    if(u > m_maxGradAmplitude)
        u = m_maxGradAmplitude;
    if(u < -m_maxGradAmplitude)
        u = -m_maxGradAmplitude;

    //ICE_OUT("theta " <<theta <<" err "   << err << " m_intPI " << m_intPI << " P " << m_PGain << " I " << m_IGain << " D " << m_DGain << " target " << m_targetTheta[0] );

	//if(err > 0) // If positive err turn CW
	//{
		gradZ = cos(theta)*u;
		gradY = 0;
		gradX = -sin(theta)*u;

		//ICE_OUT("gradX " <<gradX <<" gradZ "   << gradZ);

	//}
	//else // If negative err turn CCW
	//{
		//gradX = -cos(theta)*u;
		//gradY = 0;
		//gradZ = sin(theta)*u;

	//}
/*
	
	if (theta >= 0 && theta < M_PI/2)
	{
		perpDirection = theta-M_PI/2;
		gradX = -cos(perpDirection)*u;
		gradY = 0;
		gradZ = -sin(perpDirection)*u;
	}
	else if(theta >= M_PI/2)
	{
		perpDirection = theta+M_PI/2;
		gradX = sin(perpDirection)*u;
		gradY = 0;
		gradZ = cos(perpDirection)*u;

	}
	else if(theta < 0 && theta > (-1)*M_PI/2)
	{
		perpDirection = theta+M_PI/2;
		gradX = sin(perpDirection)*u;
		gradY = 0;
		gradZ = cos(perpDirection)*u;
	}
	else if(theta <= (-1)*M_PI/2)
	{
		perpDirection = theta-M_PI/2;
		gradX = -cos(perpDirection)*u;
		gradY = 0;
		gradZ = -sin(perpDirection)*u;

	}
*/
/*
	if(gradX > m_maxGradAmplitude)
		gradX = m_maxGradAmplitude;
	if(gradX < -1*m_maxGradAmplitude)
		gradX = -1*m_maxGradAmplitude;

	if(gradZ > m_maxGradAmplitude)
		gradZ = m_maxGradAmplitude;
	if(gradZ < -1*m_maxGradAmplitude)
		gradZ = -1*m_maxGradAmplitude;

	if(gradY > m_maxGradAmplitude)
		gradY = m_maxGradAmplitude;
	if(gradY < -1*m_maxGradAmplitude)
		gradY = -1*m_maxGradAmplitude;

*/
}


void Kalman::PI1Capsule(double positionX, double positionZ)
{
	double errX = m_targetXPosition - positionX;
	const double INTEGRAL_MAX = 20;

	// ensure that the integral error has the same sign as the error.
	if (errX*m_intPIX < 0)
		m_intPIX = 0.0;

	m_intPIX += errX;
	
	if (m_intPIX > INTEGRAL_MAX ) // Prevent integral windup
		m_intPIX = INTEGRAL_MAX; 
	else if (m_intPIX < -INTEGRAL_MAX )
		m_intPIX = -INTEGRAL_MAX; 

	double ux = (((m_PGain*errX)+(m_IGain * m_intPIX)));

	if(ux > m_maxGradAmplitude)
		ux = m_maxGradAmplitude;
	if(ux < -1*m_maxGradAmplitude)
		ux = -1*m_maxGradAmplitude;

	double errZ = m_targetZPosition - positionZ;
	// ensure that the integral error has the same sign as the error.
	if (errZ*m_intPIZ < 0)
		m_intPIZ = 0.0;

	m_intPIZ += errZ; 

	if (m_intPIZ > INTEGRAL_MAX ) // Prevent integral windup
		m_intPIZ = INTEGRAL_MAX; 
	else if (m_intPIZ < -INTEGRAL_MAX )
		m_intPIZ = -INTEGRAL_MAX; 

	double uz = (((m_PGain*errZ)+(m_IGain * m_intPIZ)));

	if(uz > m_maxGradAmplitude)
		uz = m_maxGradAmplitude;
	if(uz < -1*m_maxGradAmplitude)
		uz = -1*m_maxGradAmplitude;

	for(int i = 0; i<m_noControlCycles; i++)
	{
		m_uVals[i][0] = ux;
		m_uVals[i][1] = 0;
		m_uVals[i][2] = uz;
	}
	//ICE_OUT("position " <<position << " target " << m_targetTheta[0] <<" err "   << err << " m_intPI " << m_intPI << " P " << m_PGain << " I " << m_IGain  );
	//ICE_OUT("Gx " << u  );

}

void Kalman::fbLinearization1Rotor(double theta, double dtheta, double& gradX, double& gradY, double& gradZ)
{
	    
	double upsilon[2];
	double Tf1 = m_rotor[0].getStaticFriction();
	//double Tf2 = m_rotor[1].getStaticFriction();
	double J1 = m_rotor[0].gettotalInertia();
	//double J2 = m_rotor[1].gettotalInertia();
	double b1 = m_rotor[0].getViscousFriction();
	//double b2 = m_rotor[1].getViscousFriction();
	//double Fl1 = m_rotor[0].getLoad();
	//double Fl2 = m_rotor[1].getLoad();
	//double R1 = 1, R2 = 1;

	double alpha=(m_rotor[0].getRadiusRotorArm()*1*m_rotor[0].getBearingVol()*m_rotor[0].getSaturationMag())/J1;
	//double beta =(m_rotor[1].getRadiusRotorArm()*1*m_rotor[1].getBearingVol()*m_rotor[1].getSaturationMag())/J2;

	//ICE_OUT("t1 " <<theta <<" dt1 "   << dtheta << " target1 " << m_targetTheta[0]);

	//std::cout << std::endl <<"t1 " <<theta1 << " t2 " << theta2 <<" dt1 "   << dtheta1<<" dt2 "   << dtheta1;
	
	upsilon[0] = m_gainFBLinear[0]*(theta-m_targetTheta[0]) + m_gainFBLinear[1]*dtheta;
	upsilon[1] = -1*m_gainFBLinear[0]*(theta-m_targetTheta[0]) - m_gainFBLinear[1]*dtheta;    
   
	//ICE_OUT("upsilon[0] " << upsilon[0] << " upsilon[1] " << upsilon[1] << " alpha " << alpha << " beta " << beta);
	/*
	gradX=(sin(theta1)*(Tf2 + Fl2*R2))/(J2*beta*sin(theta1 - theta2)) - (sin(theta2)*(Tf1 + J1*upsilon + b1*dtheta1 + Fl1*R1))/(J1*alpha*sin(theta1 - theta2));
    gradZ=(cos(theta1)*(Tf2 + J2*upsilon[1] + b2*dtheta2 + Fl2*R2))/(J2*beta*sin(theta1 - theta2)) - (cos(theta2)*(Tf1 + J1*upsilon[0] + b1*dtheta1 + Fl1*R1))/(J1*alpha*sin(theta1 - theta2));
	gradY = 0;
	*/
	gradX= (1/(alpha*cos(theta)))*(upsilon[0] + (b1/J1*dtheta));
	gradZ= (1/(alpha*sin(theta)))*(upsilon[1] - (Tf1/J1));
	gradY = 0;

	gradX *= 1000; // mT/m
	gradZ *= 1000;
	gradY *= 1000;

	if(gradX > m_maxGradAmplitude)
		gradX = m_maxGradAmplitude;
	if(gradX < -1*m_maxGradAmplitude)
		gradX = -1*m_maxGradAmplitude;

	if(gradZ > m_maxGradAmplitude)
		gradZ = m_maxGradAmplitude;
	if(gradZ < -1*m_maxGradAmplitude)
		gradZ = -1*m_maxGradAmplitude;

	if(gradY > m_maxGradAmplitude)
		gradY = m_maxGradAmplitude;
	if(gradY < -1*m_maxGradAmplitude)
		gradY = -1*m_maxGradAmplitude;

	
	gradX /= 1000; // Back to T/m
	gradZ /= 1000;
	gradY /= 1000;


	return;
}

void Kalman::fbLinearization2Rotors(double theta1, double theta2, double dtheta1, double dtheta2, double& gradX, double& gradY, double& gradZ)
{
	    
	double upsilon[2];
	double Tf1 = m_rotor[0].getStaticFriction();
	double Tf2 = m_rotor[1].getStaticFriction();
	double J1 = m_rotor[0].gettotalInertia();
	double J2 = m_rotor[1].gettotalInertia();
	double b1 = m_rotor[0].getViscousFriction();
	double b2 = m_rotor[1].getViscousFriction();
	double Fl1 = m_rotor[0].getLoad();
	double Fl2 = m_rotor[1].getLoad();
	double R1 = 1, R2 = 1;

	double alpha=(m_rotor[0].getRadiusRotorArm()*1*m_rotor[0].getBearingVol()*m_rotor[0].getSaturationMag())/J1;
	double beta =(m_rotor[1].getRadiusRotorArm()*1*m_rotor[1].getBearingVol()*m_rotor[1].getSaturationMag())/J2;

	ICE_OUT("t1 " <<theta1 << " t2 " << theta2 <<" dt1 "   << dtheta1<<" dt2 "   << dtheta2 << " target1 " << m_targetTheta[0]<< " target2 " << m_targetTheta[1]);

	//std::cout << std::endl <<"t1 " <<theta1 << " t2 " << theta2 <<" dt1 "   << dtheta1<<" dt2 "   << dtheta1;
	
	upsilon[0] = m_gainFBLinear[0]*(theta1-m_targetTheta[0]) + m_gainFBLinear[1]*dtheta1;
	upsilon[1] = m_gainFBLinear[0]*(theta2-m_targetTheta[1]) + m_gainFBLinear[1]*dtheta2;    
   
	//ICE_OUT("upsilon[0] " << upsilon[0] << " upsilon[1] " << upsilon[1] << " alpha " << alpha << " beta " << beta);

	gradX=(sin(theta1)*(Tf2 + J2*upsilon[1] + b2*dtheta2 + Fl2*R2))/(J2*beta*sin(theta1 - theta2)) - (sin(theta2)*(Tf1 + J1*upsilon[0] + b1*dtheta1 + Fl1*R1))/(J1*alpha*sin(theta1 - theta2));
    gradZ=(cos(theta1)*(Tf2 + J2*upsilon[1] + b2*dtheta2 + Fl2*R2))/(J2*beta*sin(theta1 - theta2)) - (cos(theta2)*(Tf1 + J1*upsilon[0] + b1*dtheta1 + Fl1*R1))/(J1*alpha*sin(theta1 - theta2));
	gradY = 0;

	gradX *= 1000; // mT/m
	gradZ *= 1000;
	gradY *= 1000;

	if(gradX > m_maxGradAmplitude)
		gradX = m_maxGradAmplitude;
	if(gradX < -1*m_maxGradAmplitude)
		gradX = -1*m_maxGradAmplitude;

	if(gradZ > m_maxGradAmplitude)
		gradZ = m_maxGradAmplitude;
	if(gradZ < -1*m_maxGradAmplitude)
		gradZ = -1*m_maxGradAmplitude;

	if(gradY > m_maxGradAmplitude)
		gradY = m_maxGradAmplitude;
	if(gradY < -1*m_maxGradAmplitude)
		gradY = -1*m_maxGradAmplitude;

	gradX /= 1000; // Back to T/m
	gradZ /= 1000;
	gradY /= 1000;

	return;
}

	
void Kalman::predictControlLawfor2Rotors(double thetaMeas1, double thetaMeas2)
{
	//double dBx, dBy, dBz, forceMag;
	double theta1diff, theta2diff;
	theta1diff = (thetaMeas1 - m_thetaEstimate[0]);
	theta2diff = (thetaMeas2 - m_thetaEstimate[1]);

	if(theta1diff > M_PI)
		theta1diff = theta1diff-2*M_PI;
	else if(theta1diff < -M_PI)
		theta1diff = theta1diff+2*M_PI;

	if(theta2diff > M_PI)
		theta2diff = theta2diff-2*M_PI;
	else if(theta2diff < -M_PI)
		theta2diff = theta2diff+2*M_PI;

	m_velocityEstimate[0] = theta1diff/m_tMeas;
	m_velocityEstimate[1] = theta2diff/m_tMeas;

		
	//ICE_OUT("t1 " << thetaMeas1 << " t2 " << thetaMeas2 <<" v1 "   << m_velocityEstimate[0]<<" v2 "   << m_velocityEstimate[1] << "t1Old " << m_thetaEstimate[0] << " t2Old " << m_thetaEstimate[1]);

	fbLinearization2Rotors(thetaMeas1, thetaMeas2, m_velocityEstimate[0], m_velocityEstimate[1], m_uVals[0][0], m_uVals[0][1], m_uVals[0][2]);

	for(int i = 1; i <m_noControlCycles; i++)
	{
		m_uVals[i][0] = m_uVals[0][0];
		m_uVals[i][1] = m_uVals[0][1];
		m_uVals[i][2] = m_uVals[0][2];
	}
	

	ICE_OUT("Gx " << m_uVals[0][0] << " Gz " << m_uVals[0][2]);

	m_thetaEstimate[0] = thetaMeas1;
	m_thetaEstimate[1] = thetaMeas2;


	/*
	// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
	m_thetaEstimate[0] = atan2(sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas1), cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas1)); 
	m_thetaEstimate[1] = atan2(sin(m_thetaEstimate[1]) + m_gain*sin(thetaMeas2), cos(m_thetaEstimate[1]) + m_gain*cos(thetaMeas2)); 
		
	ICE_OUT("**************** REAL DATA ******************");
	//std::cout<<"\n**************** REAL DATA ******************";
	for(int i = 0; i<m_noControlCycles; i++)
	{
		//calculate the control law based on the current state
		fbLinearization(m_thetaEstimate[0], m_thetaEstimate[1], m_velocityEstimate[0], m_velocityEstimate[1], m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);

		//if(i == 0)
		//{
			//ICE_OUT("a1 " << m_thetaEstimate[0] << " a2 " << m_thetaEstimate[1] <<" v1 "   << m_velocityEstimate[0]<<" v2 "   << m_velocityEstimate[1]);
			//ICE_OUT("Gx " << m_uVals[i][0] << " Gy " << m_uVals[i][1] <<" Gz "   << m_uVals[i][2]);
		//}
		//apply the control to the model to PREDICT the next state
        //magnetic gradients
		//dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
        //dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
		//dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];
		dBx = m_uVals[i][0];
		dBy = m_uVals[i][1];
		dBz = m_uVals[i][2];

	
		ICE_OUT("Gx " <<dBx << " Gz " << dBz);
		//std::cout << std::endl << "Gx " <<dBx << " Gz " << dBz;

		// calculate the force on this rotor
		forceMag = m_rotor[0].getVolMag()*(dBx*sin(m_thetaEstimate[0]+M_PI/2)+dBy*0+dBz*cos(m_thetaEstimate[0]+M_PI/2));
		//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
		// update the estimated state
		m_thetaEstimate[0]    += m_tControl*m_velocityEstimate[0];
		m_thetaEstimate[1]    += m_tControl*m_velocityEstimate[1];

		m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
			(m_rotor[0].getRadiusRotorArm()*forceMag -
			m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() + m_rotor[0].getFriction(m_velocityEstimate[0]));

		m_velocityEstimate[1] += m_tControl*(1/m_rotor[1].gettotalInertia())*
			(m_rotor[1].getRadiusRotorArm()*forceMag -
			m_rotor[1].getViscousFriction()*m_velocityEstimate[1] - m_rotor[1].getLoad() + m_rotor[1].getFriction(m_velocityEstimate[1]));

			
		//std::cout<<"\n**************** Estimated DATA ******************";
		ICE_OUT("**************** Estimated DATA ******************");
		//ICE_OUT("\n "<< i << " Force " << forceMag << " theta " << m_thetaEstimate <<" velocity "   << m_velocityEstimate);

	}
	//
	*/

}

double Kalman::chooseAngle(double angleSolution1, double angleSolution2)
{
	double distance1, distance2;
	distance1 = fabs(angleSolution1 - m_thetaEstimateAtTrackingTime);
	distance2 = fabs(angleSolution2 - m_thetaEstimateAtTrackingTime);

	if (distance1 > distance2)
		return angleSolution2;
	
		
	return angleSolution1;

}

void Kalman::predictControlLawForSimulImagingAndActuation(double thetaMeas, int direction, float CNR)
{
	double dBx, dBy, dBz, forceMag, theta1diff, dThetaMeas;
	//double dThetaMeas2, theta2diff;
	long nbGradAfterTracking = 0;
	//double K_L = 0.00001;
	double newLoad;
	static int correctModelCnt = 0; // Counter set to 2 when two consecutive good positions received.
	double measurementNoise = 0.0, processNoise = 0.02;
	//double angleTest;
	const double CNR_CUTOFF = 5.0;
	const double NOISE_OFFSET = 0.0012;
	const double MAX_LOAD = 0.00055;
	const double LOAD_GAIN = 0.00001;
	const double TRACKING_DURATION = 0.01072; // seconds
	int const NB_CONTROL_CYCLE = 11;
	double thetaArray[NB_CONTROL_CYCLE], velocityArray[NB_CONTROL_CYCLE], covarianceArray[NB_CONTROL_CYCLE], covarianceVelocityArray[NB_CONTROL_CYCLE];

	ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0]);

	if(abs(thetaMeas - m_thetaEstimateAtTrackingTime) < 0.2)
		m_isGoodEstimate = true;


	ICE_OUT("Good Estimate: "<<m_isGoodEstimate);

	theta1diff = (thetaMeas - m_thetaKminus1[0]);
	if(direction != 1) // The only possible value for direction are 1 and -1
		direction = -1;

	if(m_controlType == MAX_TORQUE)
	{
		// Use in case of high rotation velocity. Constrain angle between 0 and 2*pi
		// Calculate only positive (or negative) velocity
		if(m_angleMode == 0)
		{
			if(direction == 1)
				if(theta1diff < 0)
					theta1diff = theta1diff+2*M_PI;
			if(direction == -1)
				if(theta1diff > 0)
					theta1diff = theta1diff-2*M_PI;
		}
		else
			theta1diff = constrainAngle(theta1diff);// Constrain angle between -pi and pi. Support positive and negative velocity

		dThetaMeas = theta1diff / (m_tMeas + TRACKING_DURATION);//m_tTracking); //Velocity measurement based on a first order finit difference
			
		m_thetaKminus2[0] = m_thetaKminus1[0]; // Keep track of the last three angles
		m_thetaKminus1[0] = thetaMeas; // Might be usuful for a second order finit difference velocity measurment.
		
		//Debug
		ICE_OUT("dThetaMeas " <<  dThetaMeas );

		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		// An enhancment would be to correct based on the model and not assuming a constant velocity
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi

		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;// Add more zeros at wish to reach infinity ;)

		}
		// Kalman filter equations
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Update the load based on the model if three consecutive good measurements are received
		if (CNR > CNR_CUTOFF)
		{
			correctModelCnt++;
		}
		else
		{
			correctModelCnt = 0;
		}

		if(correctModelCnt == 3)
		{
			correctModelCnt = 2;

			newLoad = m_rotor[0].getLoad() + (m_velocityEstimate[0] - dThetaMeas)*LOAD_GAIN;

			if(fabs(newLoad) > MAX_LOAD)
			{
				newLoad = m_rotor[0].getLoad();
			}
			//increase load
			m_rotor[0].setLoadTorque(newLoad);
		}


		//DEBUG
		ICE_OUT("load " << m_rotor[0].getLoad() << " m_gain " <<  m_gain << " m_Kvelocity " << m_Kvelocity << " thetaMeas_corrected " <<  thetaMeas << " dThetaMeas " <<  dThetaMeas);
		ICE_OUT("m_covariancePosition[0] " << m_covariancePosition[0] << " m_covarianceVelocity[0] " <<  m_covarianceVelocity[0]);

		// Needed to account for the tracking duration in the thetaEstimate calculation
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );
		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				// Update the projection angle for Simultaneous actuation and tracking 
				// In the case the next cycle a single projection for actuation and imaging is used
				if (direction == -1)
					setThetaProjection(constrainAngle(m_thetaEstimate[0]-M_PI/2.0));
				else
					setThetaProjection(constrainAngle(m_thetaEstimate[0]+M_PI/2.0));

				ICE_OUT("direction " << direction<< " ThetaProjection " << getThetaProjection());

				m_thetaEstimate[0] += TRACKING_DURATION * m_velocityEstimate[0];//m_tTracking * m_velocityEstimate[0];
				m_thetaEstimateAtTrackingTime = m_thetaEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;
			//calculate the control law based on the current state
			maxTorqueControl(m_thetaEstimate[0], m_uVals[i][0], m_uVals[i][1], m_uVals[i][2],direction);

			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];

			// calculate the force on this rotor
			forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
				(m_rotor[0].getRadiusRotorArm()*forceMag
				- m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() - m_rotor[0].getFriction(m_velocityEstimate[0]));

		}
		
		//calculate the control law based on the current state
		//thetaArray[(int)m_noControlCycles-1]    = m_thetaEstimate[0]; // debug
		//velocityArray[(int)m_noControlCycles-1] = m_velocityEstimate[0]; //debug

		//maxTorqueControl(m_thetaEstimate[0], m_uVals[(int)m_noControlCycles-1][0], m_uVals[(int)m_noControlCycles-1][1], m_uVals[(int)m_noControlCycles-1][2],direction);
			thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
			velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
			covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
			covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );

	}

}


void Kalman::predictControlLaw3OrthoRotors(double thetaMeas[MAX_NB_ROTOR], float CNR[MAX_NB_ROTOR])
{
	double theta1diff[MAX_NB_ROTOR];
	double dThetaMeas[MAX_NB_ROTOR];
	double measurementNoise[MAX_NB_ROTOR] = {0.0};
	double dBx, dBy, dBz;
	double forceMag[MAX_NB_ROTOR];
	double processNoise = 0.02;
	double const DELAY_XZ = 0.0075;// 7.5 ms delay between x and z
	const double CNR_CUTOFF = 5.0;
	const double NOISE_OFFSET = 0.0012;
	const double MAX_LOAD = 0.00055;
	const double LOAD_GAIN = 0.00001;
	int const NB_CONTROL_CYCLE = 11;
	long nbGradAfterTracking = 0;
	double thetaArray[NB_CONTROL_CYCLE][MAX_NB_ROTOR], velocityArray[NB_CONTROL_CYCLE][MAX_NB_ROTOR], covarianceArray[NB_CONTROL_CYCLE][MAX_NB_ROTOR], covarianceVelocityArray[NB_CONTROL_CYCLE][MAX_NB_ROTOR];
		
	ICE_OUT("thetaMeas[0] " << thetaMeas[0] << " thetaMeas[1] " << thetaMeas[1] <<" thetaMeas[2] " << thetaMeas[2]);

	ICE_OUT("target1 "<<m_targetTheta[0]<<" Target2 " <<m_targetTheta[1]<<" Target3 "<<m_targetTheta[2]);
	for(int i =0; i<MAX_NB_ROTOR;i++)
	{
		thetaMeas[i] = angleCorrectionForProjectionsDelay(thetaMeas[i], m_thetaEstimate[i], m_velocityEstimate[i], DELAY_XZ);
		thetaMeas[i] = constrainAngle(thetaMeas[i]);
		theta1diff[i] = constrainAngle((thetaMeas[i] - m_thetaKminus1[i]));
		dThetaMeas[i] = theta1diff[i] / (m_tMeas + m_tTracking); //Velocity measurement based on a first order finit difference
		ICE_OUT("thetaMeas[" << i << "] "<< thetaMeas[i] << "dThetaMeas[" << i<<"] " << dThetaMeas[i]);

		m_thetaKminus2[i] = m_thetaKminus1[i];
		m_thetaKminus1[i] = thetaMeas[i];
		thetaMeas[i] += m_feedBackDelayMultiRotors[i]*m_velocityEstimate[i];
		thetaMeas[i] = constrainAngle(thetaMeas[i]);// Constrain angle between -pi and pi
		if(CNR[i] >= CNR_CUTOFF)
			measurementNoise[i] = NOISE_OFFSET;
		else
		{
			if(CNR[i] != 0)
				measurementNoise[i] = NOISE_OFFSET + (CNR_CUTOFF/CNR[i]) -1;
			else
				measurementNoise[i] = 10000000;// Add more zeros at wish to reach infinity ;)

		}
		// Kalman filter equations
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
		m_gain = m_covariancePosition[i]/ (m_covariancePosition[i]+measurementNoise[i]);
		m_Kvelocity = m_covarianceVelocity[i]/ (m_covarianceVelocity[i]+ 15*measurementNoise[i]+0.5);
		m_covariancePosition[i] = (1-m_gain)*m_covariancePosition[i] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[i] = (1-m_Kvelocity)*m_covarianceVelocity[i]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[i] * dThetaMeas[i]) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR[i] < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[i] = atan2((1-m_gain)*sin(m_thetaEstimate[i]) + m_gain*sin(thetaMeas[i]), (1-m_gain)*cos(m_thetaEstimate[i]) + m_gain*cos(thetaMeas[i])); 
		m_velocityEstimate[i] = (1-m_Kvelocity)*m_velocityEstimate[i] + m_Kvelocity * dThetaMeas[i];
		
		m_thetaEstimate[i] = constrainAngle(m_thetaEstimate[i]);// Constrain angle between -pi and pi

		ICE_OUT("m_thetaEstimate[" << i << "] "<< m_thetaEstimate[i] << "m_velocityEstimate[" << i<<"] " << m_velocityEstimate[i] << "dThetaMeas["<<i<<"] " << dThetaMeas[i]);
	
	}

	nbGradAfterTracking = 3;
	
	//Apply the control Law and the model
	for(int i = 0; i<m_noControlCycles; i++)
	{
		if ((m_noControlCycles - i) == nbGradAfterTracking)
		{
			for(int j = 0; j<MAX_NB_ROTOR; j++)
			{
				m_thetaEstimate[j] += m_tTracking * m_velocityEstimate[j];
			}
		}
		for(int j = 0; j<MAX_NB_ROTOR; j++)
		{
			thetaArray[i][j]    = m_thetaEstimate[j]; // debug
			velocityArray[i][j] = m_velocityEstimate[j]; //debug
			covarianceArray[i][j] = m_covariancePosition[j];//debug
			covarianceVelocityArray[i][j] = m_covarianceVelocity[j];//debug
		}
		for(int j = 0; j<MAX_NB_ROTOR; j++)
		{
			m_covariancePosition[j] = m_covariancePosition[j] + processNoise;
			m_covarianceVelocity[j] = m_covarianceVelocity[j] + 10.0*processNoise;
			//calculate the control law based on the current state
		}
		
		LyapunovVelocityControlMultiRotors(m_thetaEstimate, m_velocityEstimate, m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);

		//apply the control to the model to PREDICT the next state
		//magnetic gradients
		dBx = m_uVals[i][0]/1000.0;
		dBy = m_uVals[i][1]/1000.0;
		dBz = m_uVals[i][2]/1000.0;

		// calculate the force on each rotor
		forceMag[0] = m_rotor[0].getVolMag()*(-dBz*sin(m_thetaEstimate[0])+dBx*0+dBy*cos(m_thetaEstimate[0]));
		forceMag[1] = m_rotor[1].getVolMag()*(-dBx*sin(m_thetaEstimate[1])+dBy*0+dBz*cos(m_thetaEstimate[1]));
		forceMag[2] = m_rotor[2].getVolMag()*(-dBx*sin(m_thetaEstimate[2])+dBz*0+dBy*cos(m_thetaEstimate[2]));

		//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
		// update the estimated state
		for(int j = 0; j<MAX_NB_ROTOR; j++)
		{
			m_thetaEstimate[j] += m_tControl*m_velocityEstimate[j];
			m_thetaEstimate[j] = constrainAngle(m_thetaEstimate[j]);// Constrain angle between -pi and pi
			m_velocityEstimate[j] += m_tControl*(1/m_rotor[j].gettotalInertia())*
				(m_rotor[j].getRadiusRotorArm()*forceMag[j]
				- m_rotor[j].getViscousFriction()*m_velocityEstimate[j] - m_rotor[j].getLoad() - m_rotor[j].getFriction(m_velocityEstimate[j]));
		}
	}
	for(int j = 0; j<MAX_NB_ROTOR; j++)
	{
		thetaArray[NB_CONTROL_CYCLE-1][j]              = m_thetaEstimate[j]; // debug
		velocityArray[NB_CONTROL_CYCLE-1][j]           = m_velocityEstimate[j]; //debug
		covarianceArray[NB_CONTROL_CYCLE-1][j]         = m_covariancePosition[j];//debug
		covarianceVelocityArray[NB_CONTROL_CYCLE-1][j] = m_covarianceVelocity[j];//debug
	}

}

/** @brief updates the state estimate uses the current state estimate to project the model into the future and calculate the necessary control
 *  @input thetaMeas, the current position measurement
 */
void Kalman::predictControlLaw(double thetaMeas, int direction, float CNR, rtPlane plane, rotor& rotor1)
{
	double dBx, dBy, dBz, forceMag, theta1diff, dThetaMeas, dThetaMeas2;
	//double theta2diff;
	long nbGradAfterTracking = 0;
	//double K_L = 0.00001;
	double newLoad;
	static int correctModelCnt = 0; // Counter set to 2 when two consecutive good positions received.
	double measurementNoise = 0.0, processNoise = 0.02;
	//double angleTest;
	double const DELAY_XZ = 0.0075;// 7.5 ms delay between x and z
	const double CNR_CUTOFF = 5.0;
	const double NOISE_OFFSET = 0.0012;
	const double MAX_LOAD = 0.00055;
	const double LOAD_GAIN = 0.00001;
	int const NB_CONTROL_CYCLE = 11;
	double thetaArray[NB_CONTROL_CYCLE], velocityArray[NB_CONTROL_CYCLE], covarianceArray[NB_CONTROL_CYCLE], covarianceVelocityArray[NB_CONTROL_CYCLE];

	thetaMeas =	angleCorrectionForProjectionsDelay(thetaMeas, m_thetaEstimate[0], m_velocityEstimate[0], DELAY_XZ);

	if(abs(thetaMeas - m_thetaEstimateAtTrackingTime) < 0.2)
		m_isGoodEstimate = true;

	ICE_OUT("Good Estimate: "<<m_isGoodEstimate);

	//Debug

	theta1diff = (thetaMeas - m_thetaKminus1[0]);

	if(direction != 1) // The only possible value for direction are 1 and -1
		direction = -1;

	ICE_OUT(" m_controlType " <<  m_controlType);

	if(m_controlType == MAX_TORQUE)
	{
		// Use in case of high rotation velocity. Constrain angle between 0 and 2*pi
		if(m_angleMode == 0)
		{
			if(direction == 1)
				if(theta1diff < 0)
					theta1diff = theta1diff+2*M_PI;
			if(direction == -1)
				if(theta1diff > 0)
					theta1diff = theta1diff-2*M_PI;
		}
		else
			theta1diff = constrainAngle(theta1diff);// Constrain angle between -pi and pi
		
		//dThetaMeas2 = secondOrderFinitDiffVelocity( m_thetaKminus2[0], m_thetaKminus1[0], thetaMeas);

		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); //Velocity measurement based on a first order finit difference
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;

		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);


		//ICE_OUT("dThetaMeas " <<  dThetaMeas  );
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		// to do correction based on the model
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		
		// Adaptive load 
		//m_rotor[0].setLoadTorque(m_rotor[0].getLoad() + K_L * (m_velocityEstimate[0] - dThetaMeas));
		//ICE_OUT("Adaptive Load " <<  m_rotor[0].getLoad() );

		// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;// Add more zeros at wish to reach infinity ;)

		}
		// Kalman filter equations
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Update the load based on the model if three consecutive good measurements are received
		if (CNR > CNR_CUTOFF)
		{
			correctModelCnt++;
		}
		else
		{
			correctModelCnt = 0;
		}

		if(correctModelCnt == 3)
		{
			correctModelCnt = 2;

			newLoad = rotor1.getLoad() + (m_velocityEstimate[0] - dThetaMeas)*LOAD_GAIN;

			if(fabs(newLoad) > MAX_LOAD)
			{
				newLoad = rotor1.getLoad();
			}
			//increase load
			rotor1.setLoadTorque(newLoad);
		}


		//DEBUG
		ICE_OUT("load " << rotor1.getLoad() << " m_gain " <<  m_gain << " m_Kvelocity " << m_Kvelocity << " thetaMeas_corrected " <<  thetaMeas << " dThetaMeas " <<  dThetaMeas);
		ICE_OUT("m_covariancePosition[0] " << m_covariancePosition[0] << " m_covarianceVelocity[0] " <<  m_covarianceVelocity[0]);

		// Needed to account for the tracking duration in the thetaEstimate calculation
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );


		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				// Update the projection angle for Simultaneous actuation and tracking 
				// In the case the next cycle a single projection for actuation and imaging is used
				if (direction == -1)
					setThetaProjection(constrainAngle(m_thetaEstimate[0]-M_PI/2.0));
				else
					setThetaProjection(constrainAngle(m_thetaEstimate[0]+M_PI/2.0));

				ICE_OUT("direction " << direction<< " ThetaProjection " << getThetaProjection());

				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
				m_thetaEstimateAtTrackingTime = m_thetaEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;
			//calculate the control law based on the current state
			maxTorqueControl(m_thetaEstimate[0], m_uVals[i][0], m_uVals[i][1], m_uVals[i][2],direction, plane);

			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];

			// calculate the force on each rotor
			if(plane == X_Z)
				forceMag = rotor1.getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			if(plane == X_Y)
				forceMag = rotor1.getVolMag()*(-dBx*sin(m_thetaEstimate[2])+dBz*0+dBy*cos(m_thetaEstimate[2]));
			if(plane == Y_Z)
				forceMag = rotor1.getVolMag()*(-dBz*sin(m_thetaEstimate[0])+dBx*0+dBy*cos(m_thetaEstimate[0]));

			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/rotor1.gettotalInertia())*
				(rotor1.getRadiusRotorArm()*forceMag
				- rotor1.getViscousFriction()*m_velocityEstimate[0] - rotor1.getLoad() - rotor1.getFriction(m_velocityEstimate[0]));

		}
		
		//calculate the control law based on the current state
		//thetaArray[(int)m_noControlCycles-1]    = m_thetaEstimate[0]; // debug
		//velocityArray[(int)m_noControlCycles-1] = m_velocityEstimate[0]; //debug

		//maxTorqueControl(m_thetaEstimate[0], m_uVals[(int)m_noControlCycles-1][0], m_uVals[(int)m_noControlCycles-1][1], m_uVals[(int)m_noControlCycles-1][2],direction);
			thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
			velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
			covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
			covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );

	}
	if(m_controlType == FBLINEARIZATION)
	{

		theta1diff = (thetaMeas - m_thetaEstimate[0]);

		if(theta1diff > M_PI)
			theta1diff = theta1diff-2*M_PI;
		else if(theta1diff < -M_PI)
			theta1diff = theta1diff+2*M_PI;

		m_velocityEstimate[0] = theta1diff/(m_tMeas+0.01); // 0.01 for tracking delay

		ICE_OUT("t1 " << thetaMeas << " v1 "   << m_velocityEstimate[0] << " target1 " << m_targetTheta[0] << " t1Old " << m_thetaEstimate[0] << " t2Old " << m_thetaEstimate[1]);

		fbLinearization1Rotor(thetaMeas, m_velocityEstimate[0], m_uVals[0][0], m_uVals[0][1], m_uVals[0][2]);

		for(int i = 1; i <m_noControlCycles; i++)
		{
			m_uVals[i][0] = m_uVals[0][0];
			m_uVals[i][1] = m_uVals[0][1];
			m_uVals[i][2] = m_uVals[0][2];
		}

		m_thetaEstimate[0] = thetaMeas;

		ICE_OUT("FBLINEARIZATION Gx " << m_uVals[0][0] << " Gz " << m_uVals[0][2]);

	}
	if(m_controlType == PI || m_controlType == LYAPUNOV_POS || m_controlType == LYAPUNOV_VEL)
	{

		theta1diff = constrainAngle(theta1diff); // Elapsed angle since the last imaging
		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); // Velocity measurement using finit difference
		
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;


		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		ICE_OUT("thetaMeas corrected " <<  thetaMeas << " Target " << m_targetTheta[0]);
		
				// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;

		}
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement

		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Correct the angle to account for the delay between the x and z axis
		//m_thetaEstimate[0] += (1-cos(2*m_thetaEstimate[0])) * m_velocityEstimate[0] * DELAY_XZ /2;
		//m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		//ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );
		
		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;

			//calculate the control law based on the current state
			if (m_controlType == PI)
				PI1Rotor(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else if(m_controlType == LYAPUNOV_POS)
				LyapunovPositionControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else
				LyapunovVelocityControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			//dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			//dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			//dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];
			dBx = m_uVals[i][0]/1000.0;
			dBy = m_uVals[i][1]/1000.0;
			dBz = m_uVals[i][2]/1000.0;


			// calculate the force on this rotor
			forceMag = rotor1.getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/rotor1.gettotalInertia())*
				(rotor1.getRadiusRotorArm()*forceMag
				- rotor1.getViscousFriction()*m_velocityEstimate[0] - rotor1.getLoad() - rotor1.getFriction(m_velocityEstimate[0]));
		}
		thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
		velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
		covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
		covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		//ICE_OUT("dBx "    <<  m_uVals[0][0] << " "<<  m_uVals[1][0] << " "<<  m_uVals[2][0]<< " " <<  m_uVals[3][0]<< " " <<  m_uVals[4][0]<< " " <<  m_uVals[5][0] << " "<< m_uVals[6][0] << " " << m_uVals[7][0] << " " << m_uVals[8][0] << " " << m_uVals[9][0]);
		//ICE_OUT("dBz "    <<  m_uVals[0][2] << " "<<  m_uVals[1][2] << " "<<  m_uVals[2][2]<< " " <<  m_uVals[3][2]<< " " <<  m_uVals[4][2]<< " " <<  m_uVals[5][2] << " "<< m_uVals[6][2] << " " << m_uVals[7][2] << " " << m_uVals[8][2] << " " << m_uVals[9][2]);
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		//ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		//ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );
	}
	//ICE_OUT("\nthetaMeas "   << thetaMeas );
	//for(int i = 0; i<m_noControlCycles; i++)
	//{
	//	ICE_OUT("\nm_uVals[" << i <<"]"   << m_uVals[i][0] << " " << m_uVals[i][2] );
	//}
	//ICE_OUT("\nm_thetaEstimate "   << m_thetaEstimate );
	//ICE_OUT("\nm_velocityEstimate "   << m_velocityEstimate );

	return;

}


/** @brief updates the state estimate uses the current state estimate to project the model into the future and calculate the necessary control
 *  @input thetaMeas, the current position measurement
 */
void Kalman::predictControlLaw(double thetaMeas, int direction, float CNR, rtPlane plane)
{
	double dBx, dBy, dBz, forceMag, theta1diff, dThetaMeas, dThetaMeas2;
	//double theta2diff;
	long nbGradAfterTracking = 0;
	//double K_L = 0.00001;
	double newLoad;
	static int correctModelCnt = 0; // Counter set to 2 when two consecutive good positions received.
	double measurementNoise = 0.0, processNoise = 0.02;
	//double angleTest;
	double const DELAY_XZ = 0.0075;// 7.5 ms delay between x and z
	const double CNR_CUTOFF = 5.0;
	const double NOISE_OFFSET = 0.0012;
	const double MAX_LOAD = 0.00055;
	const double LOAD_GAIN = 0.00001;
	int const NB_CONTROL_CYCLE = 11;
	double thetaArray[NB_CONTROL_CYCLE], velocityArray[NB_CONTROL_CYCLE], covarianceArray[NB_CONTROL_CYCLE], covarianceVelocityArray[NB_CONTROL_CYCLE];

	thetaMeas =	angleCorrectionForProjectionsDelay(thetaMeas, m_thetaEstimate[0], m_velocityEstimate[0], DELAY_XZ);

	if(abs(thetaMeas - m_thetaEstimateAtTrackingTime) < 0.2)
		m_isGoodEstimate = true;

	ICE_OUT("Good Estimate: "<<m_isGoodEstimate);

	//Debug

	theta1diff = (thetaMeas - m_thetaKminus1[0]);

	if(direction != 1) // The only possible value for direction are 1 and -1
		direction = -1;

	//ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " m_controlType " <<  m_controlType);

	if(m_controlType == MAX_TORQUE)
	{
		// Use in case of high rotation velocity. Constrain angle between 0 and 2*pi
		if(m_angleMode == 0)
		{
			if(direction == 1)
				if(theta1diff < 0)
					theta1diff = theta1diff+2*M_PI;
			if(direction == -1)
				if(theta1diff > 0)
					theta1diff = theta1diff-2*M_PI;
		}
		else
			theta1diff = constrainAngle(theta1diff);// Constrain angle between -pi and pi
		
		//dThetaMeas2 = secondOrderFinitDiffVelocity( m_thetaKminus2[0], m_thetaKminus1[0], thetaMeas);

		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); //Velocity measurement based on a first order finit difference
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;

		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);


		//ICE_OUT("dThetaMeas " <<  dThetaMeas  );
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		// to do correction based on the model
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		
		// Adaptive load 
		//m_rotor[0].setLoadTorque(m_rotor[0].getLoad() + K_L * (m_velocityEstimate[0] - dThetaMeas));
		//ICE_OUT("Adaptive Load " <<  m_rotor[0].getLoad() );

		// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;// Add more zeros at wish to reach infinity ;)

		}
		// Kalman filter equations
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Update the load based on the model if three consecutive good measurements are received
		if (CNR > CNR_CUTOFF)
		{
			correctModelCnt++;
		}
		else
		{
			correctModelCnt = 0;
		}

		if(correctModelCnt == 3)
		{
			correctModelCnt = 2;

			newLoad = m_rotor[0].getLoad() + (m_velocityEstimate[0] - dThetaMeas)*LOAD_GAIN;

			if(fabs(newLoad) > MAX_LOAD)
			{
				newLoad = m_rotor[0].getLoad();
			}
			//increase load
			m_rotor[0].setLoadTorque(newLoad);
		}


		//DEBUG
		ICE_OUT("load " << m_rotor[0].getLoad() << " m_gain " <<  m_gain << " m_Kvelocity " << m_Kvelocity << " thetaMeas_corrected " <<  thetaMeas << " dThetaMeas " <<  dThetaMeas);
		ICE_OUT("m_covariancePosition[0] " << m_covariancePosition[0] << " m_covarianceVelocity[0] " <<  m_covarianceVelocity[0]);

		// Needed to account for the tracking duration in the thetaEstimate calculation
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );


		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				// Update the projection angle for Simultaneous actuation and tracking 
				// In the case the next cycle a single projection for actuation and imaging is used
				if (direction == -1)
					setThetaProjection(constrainAngle(m_thetaEstimate[0]-M_PI/2.0));
				else
					setThetaProjection(constrainAngle(m_thetaEstimate[0]+M_PI/2.0));

				ICE_OUT("direction " << direction<< " ThetaProjection " << getThetaProjection());

				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
				m_thetaEstimateAtTrackingTime = m_thetaEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;
			//calculate the control law based on the current state
			maxTorqueControl(m_thetaEstimate[0], m_uVals[i][0], m_uVals[i][1], m_uVals[i][2],direction, plane);

			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];

			// calculate the force on each rotor
			if(plane == X_Z)
				forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			if(plane == X_Y)
				forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[2])+dBz*0+dBy*cos(m_thetaEstimate[2]));
			if(plane == Y_Z)
				forceMag = m_rotor[0].getVolMag()*(-dBz*sin(m_thetaEstimate[0])+dBx*0+dBy*cos(m_thetaEstimate[0]));

			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
				(m_rotor[0].getRadiusRotorArm()*forceMag
				- m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() - m_rotor[0].getFriction(m_velocityEstimate[0]));

		}
		
		//calculate the control law based on the current state
		//thetaArray[(int)m_noControlCycles-1]    = m_thetaEstimate[0]; // debug
		//velocityArray[(int)m_noControlCycles-1] = m_velocityEstimate[0]; //debug

		//maxTorqueControl(m_thetaEstimate[0], m_uVals[(int)m_noControlCycles-1][0], m_uVals[(int)m_noControlCycles-1][1], m_uVals[(int)m_noControlCycles-1][2],direction);
			thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
			velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
			covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
			covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );

	}
	if(m_controlType == FBLINEARIZATION)
	{

		theta1diff = (thetaMeas - m_thetaEstimate[0]);

		if(theta1diff > M_PI)
			theta1diff = theta1diff-2*M_PI;
		else if(theta1diff < -M_PI)
			theta1diff = theta1diff+2*M_PI;

		m_velocityEstimate[0] = theta1diff/(m_tMeas+0.01); // 0.01 for tracking delay

		ICE_OUT("t1 " << thetaMeas << " v1 "   << m_velocityEstimate[0] << " target1 " << m_targetTheta[0] << " t1Old " << m_thetaEstimate[0] << " t2Old " << m_thetaEstimate[1]);

		fbLinearization1Rotor(thetaMeas, m_velocityEstimate[0], m_uVals[0][0], m_uVals[0][1], m_uVals[0][2]);

		for(int i = 1; i <m_noControlCycles; i++)
		{
			m_uVals[i][0] = m_uVals[0][0];
			m_uVals[i][1] = m_uVals[0][1];
			m_uVals[i][2] = m_uVals[0][2];
		}

		m_thetaEstimate[0] = thetaMeas;

		ICE_OUT("FBLINEARIZATION Gx " << m_uVals[0][0] << " Gz " << m_uVals[0][2]);

	}
	if(m_controlType == PI || m_controlType == LYAPUNOV_POS || m_controlType == LYAPUNOV_VEL)
	{

		theta1diff = constrainAngle(theta1diff); // Elapsed angle since the last imaging
		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); // Velocity measurement using finit difference
		
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;


		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		ICE_OUT("thetaMeas corrected " <<  thetaMeas << " Target " << m_targetTheta[0]);
		
				// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;

		}
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement

		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Correct the angle to account for the delay between the x and z axis
		//m_thetaEstimate[0] += (1-cos(2*m_thetaEstimate[0])) * m_velocityEstimate[0] * DELAY_XZ /2;
		//m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		//ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );
		
		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;

			//calculate the control law based on the current state
			if (m_controlType == PI)
				PI1Rotor(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else if(m_controlType == LYAPUNOV_POS)
				LyapunovPositionControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else
				LyapunovVelocityControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			//dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			//dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			//dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];
			dBx = m_uVals[i][0]/1000.0;
			dBy = m_uVals[i][1]/1000.0;
			dBz = m_uVals[i][2]/1000.0;


			// calculate the force on this rotor
			forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
				(m_rotor[0].getRadiusRotorArm()*forceMag
				- m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() - m_rotor[0].getFriction(m_velocityEstimate[0]));
		}
		thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
		velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
		covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
		covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		//ICE_OUT("dBx "    <<  m_uVals[0][0] << " "<<  m_uVals[1][0] << " "<<  m_uVals[2][0]<< " " <<  m_uVals[3][0]<< " " <<  m_uVals[4][0]<< " " <<  m_uVals[5][0] << " "<< m_uVals[6][0] << " " << m_uVals[7][0] << " " << m_uVals[8][0] << " " << m_uVals[9][0]);
		//ICE_OUT("dBz "    <<  m_uVals[0][2] << " "<<  m_uVals[1][2] << " "<<  m_uVals[2][2]<< " " <<  m_uVals[3][2]<< " " <<  m_uVals[4][2]<< " " <<  m_uVals[5][2] << " "<< m_uVals[6][2] << " " << m_uVals[7][2] << " " << m_uVals[8][2] << " " << m_uVals[9][2]);
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		//ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		//ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );
	}
	//ICE_OUT("\nthetaMeas "   << thetaMeas );
	//for(int i = 0; i<m_noControlCycles; i++)
	//{
	//	ICE_OUT("\nm_uVals[" << i <<"]"   << m_uVals[i][0] << " " << m_uVals[i][2] );
	//}
	//ICE_OUT("\nm_thetaEstimate "   << m_thetaEstimate );
	//ICE_OUT("\nm_velocityEstimate "   << m_velocityEstimate );

	return;

}



/** @brief updates the state estimate uses the current state estimate to project the model into the future and calculate the necessary control
 *  @input thetaMeas, the current position measurement
 */
void Kalman::predictControlLaw(double thetaMeas, int direction, float CNR)
{
	double dBx, dBy, dBz, forceMag, theta1diff, dThetaMeas, dThetaMeas2;
	//double theta2diff;
	long nbGradAfterTracking = 0;
	//double K_L = 0.00001;
	double newLoad;
	static int correctModelCnt = 0; // Counter set to 2 when two consecutive good positions received.
	double measurementNoise = 0.0, processNoise = 0.02;
	//double angleTest;
	double const DELAY_XZ = 0.0075;// 7.5 ms delay between x and z
	const double CNR_CUTOFF = 5.0;
	const double NOISE_OFFSET = 0.0012;
	const double MAX_LOAD = 0.00055;
	const double LOAD_GAIN = 0.00001;
	int const NB_CONTROL_CYCLE = 11;
	double thetaArray[NB_CONTROL_CYCLE], velocityArray[NB_CONTROL_CYCLE], covarianceArray[NB_CONTROL_CYCLE], covarianceVelocityArray[NB_CONTROL_CYCLE];

	thetaMeas =	angleCorrectionForProjectionsDelay(thetaMeas, m_thetaEstimate[0], m_velocityEstimate[0], DELAY_XZ);

	if(abs(thetaMeas - m_thetaEstimateAtTrackingTime) < 0.2)
		m_isGoodEstimate = true;

	ICE_OUT("Good Estimate: "<<m_isGoodEstimate);

	//Debug

	theta1diff = (thetaMeas - m_thetaKminus1[0]);

	if(direction != 1) // The only possible value for direction are 1 and -1
		direction = -1;

	//ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " m_controlType " <<  m_controlType);

	if(m_controlType == MAX_TORQUE)
	{
		// Use in case of high rotation velocity. Constrain angle between 0 and 2*pi
		if(m_angleMode == 0)
		{
			if(direction == 1)
				if(theta1diff < 0)
					theta1diff = theta1diff+2*M_PI;
			if(direction == -1)
				if(theta1diff > 0)
					theta1diff = theta1diff-2*M_PI;
		}
		else
			theta1diff = constrainAngle(theta1diff);// Constrain angle between -pi and pi
		
		//dThetaMeas2 = secondOrderFinitDiffVelocity( m_thetaKminus2[0], m_thetaKminus1[0], thetaMeas);

		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); //Velocity measurement based on a first order finit difference
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;

		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);


		//ICE_OUT("dThetaMeas " <<  dThetaMeas  );
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		// to do correction based on the model
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		
		// Adaptive load 
		//m_rotor[0].setLoadTorque(m_rotor[0].getLoad() + K_L * (m_velocityEstimate[0] - dThetaMeas));
		//ICE_OUT("Adaptive Load " <<  m_rotor[0].getLoad() );

		// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;// Add more zeros at wish to reach infinity ;)

		}
		// Kalman filter equations
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement
		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Update the load based on the model if three consecutive good measurements are received
		if (CNR > CNR_CUTOFF)
		{
			correctModelCnt++;
		}
		else
		{
			correctModelCnt = 0;
		}

		if(correctModelCnt == 3)
		{
			correctModelCnt = 2;

			newLoad = m_rotor[0].getLoad() + (m_velocityEstimate[0] - dThetaMeas)*LOAD_GAIN;

			if(fabs(newLoad) > MAX_LOAD)
			{
				newLoad = m_rotor[0].getLoad();
			}
			//increase load
			m_rotor[0].setLoadTorque(newLoad);
		}


		//DEBUG
		ICE_OUT("load " << m_rotor[0].getLoad() << " m_gain " <<  m_gain << " m_Kvelocity " << m_Kvelocity << " thetaMeas_corrected " <<  thetaMeas << " dThetaMeas " <<  dThetaMeas);
		ICE_OUT("m_covariancePosition[0] " << m_covariancePosition[0] << " m_covarianceVelocity[0] " <<  m_covarianceVelocity[0]);

		// Needed to account for the tracking duration in the thetaEstimate calculation
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );


		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				// Update the projection angle for Simultaneous actuation and tracking 
				// In the case the next cycle a single projection for actuation and imaging is used
				if (direction == -1)
					setThetaProjection(constrainAngle(m_thetaEstimate[0]-M_PI/2.0));
				else
					setThetaProjection(constrainAngle(m_thetaEstimate[0]+M_PI/2.0));

				ICE_OUT("direction " << direction<< " ThetaProjection " << getThetaProjection());

				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
				m_thetaEstimateAtTrackingTime = m_thetaEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;
			//calculate the control law based on the current state
			maxTorqueControl(m_thetaEstimate[0], m_uVals[i][0], m_uVals[i][1], m_uVals[i][2],direction);

			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];

			// calculate the force on this rotor
			forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
				(m_rotor[0].getRadiusRotorArm()*forceMag
				- m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() - m_rotor[0].getFriction(m_velocityEstimate[0]));

		}
		
		//calculate the control law based on the current state
		//thetaArray[(int)m_noControlCycles-1]    = m_thetaEstimate[0]; // debug
		//velocityArray[(int)m_noControlCycles-1] = m_velocityEstimate[0]; //debug

		//maxTorqueControl(m_thetaEstimate[0], m_uVals[(int)m_noControlCycles-1][0], m_uVals[(int)m_noControlCycles-1][1], m_uVals[(int)m_noControlCycles-1][2],direction);
			thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
			velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
			covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
			covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );

	}
	if(m_controlType == FBLINEARIZATION)
	{

		theta1diff = (thetaMeas - m_thetaEstimate[0]);

		if(theta1diff > M_PI)
			theta1diff = theta1diff-2*M_PI;
		else if(theta1diff < -M_PI)
			theta1diff = theta1diff+2*M_PI;

		m_velocityEstimate[0] = theta1diff/(m_tMeas+0.01); // 0.01 for tracking delay

		ICE_OUT("t1 " << thetaMeas << " v1 "   << m_velocityEstimate[0] << " target1 " << m_targetTheta[0] << " t1Old " << m_thetaEstimate[0] << " t2Old " << m_thetaEstimate[1]);

		fbLinearization1Rotor(thetaMeas, m_velocityEstimate[0], m_uVals[0][0], m_uVals[0][1], m_uVals[0][2]);

		for(int i = 1; i <m_noControlCycles; i++)
		{
			m_uVals[i][0] = m_uVals[0][0];
			m_uVals[i][1] = m_uVals[0][1];
			m_uVals[i][2] = m_uVals[0][2];
		}

		m_thetaEstimate[0] = thetaMeas;

		ICE_OUT("FBLINEARIZATION Gx " << m_uVals[0][0] << " Gz " << m_uVals[0][2]);

	}
	if(m_controlType == PI || m_controlType == LYAPUNOV_POS || m_controlType == LYAPUNOV_VEL)
	{

		theta1diff = constrainAngle(theta1diff); // Elapsed angle since the last imaging
		dThetaMeas = theta1diff / (m_tMeas + m_tTracking); // Velocity measurement using finit difference
		
		m_thetaKminus2[0] = m_thetaKminus1[0];
		m_thetaKminus1[0] = thetaMeas;


		ICE_OUT("thetaMeas " <<  thetaMeas  << " m_thetaKminus1 " <<  m_thetaKminus1[0] << " dThetaMeas " <<  dThetaMeas);
		// Correct the angle for m_tMeas (seconds) elapsed since the position is acquired 
		thetaMeas += m_feedBackDelay*m_velocityEstimate[0];
		thetaMeas = constrainAngle(thetaMeas);// Constrain angle between -pi and pi
		ICE_OUT("thetaMeas corrected " <<  thetaMeas << " Target " << m_targetTheta[0]);
		
				// Use the CNR to estimate the measurement noise
		if(CNR >= CNR_CUTOFF)
			measurementNoise = NOISE_OFFSET;
		else
		{
			if(CNR != 0)
				measurementNoise = NOISE_OFFSET + (CNR_CUTOFF/CNR) -1;
			else
				measurementNoise = 10000000;

		}
		// CORRECT The m_thetaEstimate and m_velocityEstimate based on the measurement

		m_gain = m_covariancePosition[0]/ (m_covariancePosition[0]+measurementNoise);
		m_Kvelocity = m_covarianceVelocity[0]/ (m_covarianceVelocity[0]+ 15*measurementNoise+0.5);
		m_covariancePosition[0] = (1-m_gain)*m_covariancePosition[0] + m_feedBackDelay*processNoise/m_tControl; //Increase the convariane because of the time between the measurement and the current time
		m_covarianceVelocity[0] = (1-m_Kvelocity)*m_covarianceVelocity[0]+ 10.0*m_feedBackDelay*processNoise/m_tControl;
		
		if( (m_velocityEstimate[0] * dThetaMeas) < 0 )//if the estimated  and the measured velocities are of different sign...
		{
			if(CNR < CNR_CUTOFF) // ...and if poor signal quality, completely ignore the measurement
			{
				m_Kvelocity = 0;
				m_gain = 0;
			}
		}
		m_thetaEstimate[0] = atan2((1-m_gain)*sin(m_thetaEstimate[0]) + m_gain*sin(thetaMeas), (1-m_gain)*cos(m_thetaEstimate[0]) + m_gain*cos(thetaMeas)); 
		m_velocityEstimate[0] = (1-m_Kvelocity)*m_velocityEstimate[0] + m_Kvelocity * dThetaMeas;
		
		m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi

		// Correct the angle to account for the delay between the x and z axis
		//m_thetaEstimate[0] += (1-cos(2*m_thetaEstimate[0])) * m_velocityEstimate[0] * DELAY_XZ /2;
		//m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi
		nbGradAfterTracking = (long) (m_feedBackDelay/m_tControl);
		//ICE_OUT("nbGradAfterTracking " <<  nbGradAfterTracking );
		
		for(int i = 0; i<m_noControlCycles; i++)
		{
			if ((m_noControlCycles - i) == nbGradAfterTracking)
			{
				m_thetaEstimate[0] += m_tTracking * m_velocityEstimate[0];
			}
			thetaArray[i]    = m_thetaEstimate[0]; // debug
			velocityArray[i] = m_velocityEstimate[0]; //debug
			covarianceArray[i] = m_covariancePosition[0];//debug
			covarianceVelocityArray[i] = m_covarianceVelocity[0];//debug
		
			m_covariancePosition[0] = m_covariancePosition[0] + processNoise;
			m_covarianceVelocity[0] = m_covarianceVelocity[0] + 10.0*processNoise;

			//calculate the control law based on the current state
			if (m_controlType == PI)
				PI1Rotor(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else if(m_controlType == LYAPUNOV_POS)
				LyapunovPositionControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			else
				LyapunovVelocityControl(m_thetaEstimate[0], m_velocityEstimate[0],  m_uVals[i][0], m_uVals[i][1], m_uVals[i][2]);
			//apply the control to the model to PREDICT the next state
			//magnetic gradients
			//dBx = (m_maxGradAmplitude/1000)*m_uVals[i][0];
			//dBy = (m_maxGradAmplitude/1000)*m_uVals[i][1];
			//dBz = (m_maxGradAmplitude/1000)*m_uVals[i][2];
			dBx = m_uVals[i][0]/1000.0;
			dBy = m_uVals[i][1]/1000.0;
			dBz = m_uVals[i][2]/1000.0;


			// calculate the force on this rotor
			forceMag = m_rotor[0].getVolMag()*(-dBx*sin(m_thetaEstimate[0])+dBy*0+dBz*cos(m_thetaEstimate[0]));
			//ICE_OUT("\n "<< i << " vol " << m_rotor.getVolMag() << " dBx " << dBx <<" dBz "   << dBz);
			// update the estimated state
			
			m_thetaEstimate[0] += m_tControl*m_velocityEstimate[0];
			m_thetaEstimate[0] = constrainAngle(m_thetaEstimate[0]);// Constrain angle between -pi and pi


			m_velocityEstimate[0] += m_tControl*(1/m_rotor[0].gettotalInertia())*
				(m_rotor[0].getRadiusRotorArm()*forceMag
				- m_rotor[0].getViscousFriction()*m_velocityEstimate[0] - m_rotor[0].getLoad() - m_rotor[0].getFriction(m_velocityEstimate[0]));
		}
		thetaArray[NB_CONTROL_CYCLE-1]    = m_thetaEstimate[0]; // debug
		velocityArray[NB_CONTROL_CYCLE-1] = m_velocityEstimate[0]; //debug
		covarianceArray[NB_CONTROL_CYCLE-1] = m_covariancePosition[0];//debug
		covarianceVelocityArray[NB_CONTROL_CYCLE-1] = m_covarianceVelocity[0];//debug

		ICE_OUT("ThetaE " <<  thetaArray[0] << " "<< thetaArray[1] << " "<< thetaArray[2]<< " " << thetaArray[3]<< " " << thetaArray[4]<< " " << thetaArray[5] << " "<<thetaArray[6] << " " <<thetaArray[7] << " " <<thetaArray[8] << " " <<thetaArray[9] << " " <<thetaArray[10]   );
		//ICE_OUT("dBx "    <<  m_uVals[0][0] << " "<<  m_uVals[1][0] << " "<<  m_uVals[2][0]<< " " <<  m_uVals[3][0]<< " " <<  m_uVals[4][0]<< " " <<  m_uVals[5][0] << " "<< m_uVals[6][0] << " " << m_uVals[7][0] << " " << m_uVals[8][0] << " " << m_uVals[9][0]);
		//ICE_OUT("dBz "    <<  m_uVals[0][2] << " "<<  m_uVals[1][2] << " "<<  m_uVals[2][2]<< " " <<  m_uVals[3][2]<< " " <<  m_uVals[4][2]<< " " <<  m_uVals[5][2] << " "<< m_uVals[6][2] << " " << m_uVals[7][2] << " " << m_uVals[8][2] << " " << m_uVals[9][2]);
		ICE_OUT("VeloCE " <<  velocityArray[0] << " "<< velocityArray[1] << " "<< velocityArray[2]<< " " << velocityArray[3]<< " " << velocityArray[4]<< " " << velocityArray[5] << " "<<velocityArray[6] << " " <<velocityArray[7] << " " <<velocityArray[8] << " " <<velocityArray[9]<< " " <<velocityArray[10]   );
		//ICE_OUT("CovPos "  <<  covarianceArray[0] << " "<< covarianceArray[1] << " "<< covarianceArray[2]<< " " << covarianceArray[3]<< " " << covarianceArray[4]<< " " << covarianceArray[5] << " "<<covarianceArray[6] << " " <<covarianceArray[7] << " " <<covarianceArray[8] << " " <<covarianceArray[9]<< " " <<covarianceArray[10]   );
		//ICE_OUT("CovVel "  <<  covarianceVelocityArray[0] << " "<< covarianceVelocityArray[1] << " "<< covarianceVelocityArray[2]<< " " << covarianceVelocityArray[3]<< " " << covarianceVelocityArray[4]<< " " << covarianceVelocityArray[5] << " "<<covarianceVelocityArray[6] << " " <<covarianceVelocityArray[7] << " " <<covarianceVelocityArray[8] << " " <<covarianceVelocityArray[9] << " " <<covarianceVelocityArray[10]  );
	}
	//ICE_OUT("\nthetaMeas "   << thetaMeas );
	//for(int i = 0; i<m_noControlCycles; i++)
	//{
	//	ICE_OUT("\nm_uVals[" << i <<"]"   << m_uVals[i][0] << " " << m_uVals[i][2] );
	//}
	//ICE_OUT("\nm_thetaEstimate "   << m_thetaEstimate );
	//ICE_OUT("\nm_velocityEstimate "   << m_velocityEstimate );

	return;

}


void Kalman::getControlInputs(FBData& controlInputs)
{
// I need to apply the rotation matrix here and Correct the axis in the sequence code
	controlInputs.prjAngle = m_thetaProjection;

	ICE_OUT("m_nbRotors: " << m_nbRotors);

	if(m_nbRotors == 1)
	{
		if(m_controlType == MAX_TORQUE)
		{			
			for (int i = 0; i<m_noControlCycles; i++)
			{
				for (int j = 0; j<GRAD_AXIS;j++)
				{
					if(m_uVals[i][j] * m_maxGradAmplitude > m_maxGradAmplitude)
						m_uVals[i][j] = 1;
					if(m_uVals[i][j] * m_maxGradAmplitude < -1*m_maxGradAmplitude)
						m_uVals[i][j] = -1;
				}
			}

			for (int i = 0; i < m_noControlCycles; i++)
			{
				controlInputs.uValx[i]= m_uVals[i][0] * m_maxGradAmplitude;
				controlInputs.uValy[i]= m_uVals[i][1] * m_maxGradAmplitude;
				controlInputs.uValz[i]= m_uVals[i][2] * m_maxGradAmplitude;
			}
		}
		else if(m_controlType == FBLINEARIZATION)
		{
			for (int i = 0; i < m_noControlCycles; i++)
			{
				controlInputs.uValx[i] = m_uVals[i][0] * 1000;
				controlInputs.uValy[i] = m_uVals[i][1] * 1000;
				controlInputs.uValz[i] = m_uVals[i][2] * 1000;
			}
		}
		else if(m_controlType == PI || m_controlType == LYAPUNOV_POS || m_controlType == LYAPUNOV_VEL)
		{
			for (int i = 0; i < m_noControlCycles; i++)
			{
				controlInputs.uValx[i] = m_uVals[i][0];
				controlInputs.uValy[i] = m_uVals[i][1];
				controlInputs.uValz[i] = m_uVals[i][2];
			}
		}
	}
	else
	{
		for (int i = 0; i < m_noControlCycles; i++)
		{
			controlInputs.uValx[i]= m_uVals[i][0];
			controlInputs.uValy[i]= m_uVals[i][1];
			controlInputs.uValz[i]= m_uVals[i][2];
		}
	}
}

int  Kalman::getNbRotors()
{
	return m_nbRotors;

}
void Kalman::setNbRotors(int nb)
{
	m_nbRotors = nb;
}

void Kalman::setFBLinearGainK1(double K1)
{
	m_gainFBLinear[0] = K1;

}
void Kalman::setFBLinearGainK2(double K2)
{
	m_gainFBLinear[1] = K2;

}

void Kalman::setTargetTheta1(double theta1)
{
	m_targetTheta[0] = theta1;
}
void Kalman::setTargetTheta2(double theta2)
{
	m_targetTheta[1] = theta2;
}

void Kalman::setTargetTheta(double theta1, double theta2, double theta3)
{
	m_targetTheta[0] = theta1;
	m_targetTheta[1] = theta2;
	m_targetTheta[2] = theta3;
}

double Kalman::getFBLinearGainK1(void)
{
	return m_gainFBLinear[0];

}
double Kalman::getFBLinearGainK2(void)
{	
	return m_gainFBLinear[1];
}
double Kalman::getTargetTheta1(void)
{
	return m_targetTheta[0];

}
double Kalman::getTargetTheta2(void)
{	
	return m_targetTheta[1];
}

void Kalman::setControlType(CONTROL_TYPE type)
{
	m_controlType = type;

}
CONTROL_TYPE Kalman::getControlType()
{
	return m_controlType;
}

void Kalman::setTrackingDuration(double duration)
{
	m_tTracking = duration;
}
double Kalman::getTrackingDuration()
{
	return m_tTracking;
}

void Kalman::setFeedBackDelay(double delay)
{
	m_feedBackDelay = delay;

}
double Kalman::getFeedBackDelay()
{
	return m_feedBackDelay;
}

void Kalman::setMaxGradAmplitude(double amp)
{
	m_maxGradAmplitude = amp;
}

void Kalman::setRotorStaticFriction(double statFric)
{
	m_rotor[0].setStaticFriction(statFric);
	m_rotor[1].setStaticFriction(statFric);
	m_rotor[2].setStaticFriction(statFric);

}
void Kalman::setRotorViscousFriction(double visFric)
{
	m_rotor[0].setViscousFriction(visFric);
	m_rotor[1].setViscousFriction(visFric);
	m_rotor[2].setViscousFriction(visFric);
}
void Kalman::setRotorLoad(double load)
{
	m_rotor[0].setLoadTorque(load);
	m_rotor[1].setLoadTorque(load);
	m_rotor[2].setLoadTorque(load);
}
void Kalman::setRotorRadiusArm(double rra)
{
	m_rotor[0].setRadiusRotorArm(rra);
	m_rotor[1].setRadiusRotorArm(rra);
	m_rotor[2].setRadiusRotorArm(rra);

}

// Initialise the theta and theta velocity

double Kalman::constrainAngle(double x)
{
	x = fmod(x + M_PI, 2*M_PI);

	if(x<0)
		x += 2*M_PI;
	

	return x - M_PI;

}
double Kalman::angleConv(double angle)
{
	return fmod(constrainAngle(angle), 2*M_PI);
}

double Kalman::angleDiff(double a, double b)
{
	double dif = fmod(b - a + M_PI, 2*M_PI);
	if(dif<0)
		dif += 2*M_PI;
	return dif - M_PI;
}
double Kalman::unwrap(double previousAngle, double newAngle)
{
	return previousAngle-angleDiff(newAngle, angleConv(previousAngle));

}

void Kalman::initializeFilter(double m_position[3][2], double m_positionKminus1[3][2], double m_positionKminus2[3][2], double m_center[3][2])
{
	double theta[3];

	theta[2] = (atan2(m_position[0][1]-m_center[0][1], m_position[0][0] - m_center[0][0]));
	theta[1] = (atan2(m_positionKminus1[0][1]-m_center[0][1], m_positionKminus1[0][0] - m_center[0][0]));
	theta[0] = (atan2(m_positionKminus2[0][1]-m_center[0][1], m_positionKminus2[0][0] - m_center[0][0]));

	ICE_OUT("theta[0] " << theta[0] << " theta[1] " << theta[1] << " theta[2] " << theta[2]);

	theta[2] = constrainAngle(theta[2]);
	theta[1] = constrainAngle(theta[1]);
	theta[0] = constrainAngle(theta[0]);

	ICE_OUT("theta[0] " << theta[0] << " theta[1] " << theta[1] << " theta[2] " << theta[2]);

	double thetaUnwrap[3];
	thetaUnwrap[0] = theta[0];
	thetaUnwrap[1] = unwrap(thetaUnwrap[0], theta[1]);
	thetaUnwrap[2] = unwrap(thetaUnwrap[1], theta[2]);
		
	ICE_OUT("thetaUnwrap[0] " << thetaUnwrap[0] << " thetaUnwrap[1] " << thetaUnwrap[1] << " thetaUnwrap[2] " << thetaUnwrap[2]);

	m_thetaKminus1[0]  = theta[2];
	m_thetaEstimate[0] = theta[2];
	m_velocityEstimate[0] = 1/(2*(m_tMeas + m_tTracking+0.001)) *(3*thetaUnwrap[2] - 4*thetaUnwrap[1] + thetaUnwrap[0]);

	ICE_OUT("m_velocityEstimate[0] " << m_velocityEstimate[0]);

	m_isInitialized[0] = true;


}

double Kalman::secondOrderFinitDiffVelocity(double thetaKminus2, double thetaKminus1, double thetaK)
{
	double velocity;
	double thetaUnwrapKminus2, thetaUnwrapKminus1, thetaUnwrap;
	thetaUnwrapKminus2 = thetaKminus2;
	thetaUnwrapKminus1 = unwrap(thetaKminus2, thetaKminus1);
	thetaUnwrap = unwrap(thetaUnwrapKminus1, thetaK);
	velocity = 1/(2*(m_tMeas + m_tTracking+0.001)) *(3*thetaUnwrap - 4*thetaUnwrapKminus1 + thetaUnwrapKminus2);
	ICE_OUT("thetaKminus2 " << thetaKminus2 << " thetaKminus1 " << thetaKminus1 << " thetaK " << thetaK);
	ICE_OUT("thetaUnwrapKminus2 " << thetaUnwrapKminus2 << " thetaUnwrapKminus1 " << thetaUnwrapKminus1 << " thetaUnwrap " << thetaUnwrap);

	return velocity;

}

void Kalman::setAngleMode(int mode)
{
	m_angleMode = mode;
	if (mode != 0)
		m_angleMode = 1;
}

// Correction using Aaron Becker's approximation
double Kalman::angleCorrectionForProjectionsDelay(double thetaMeas, double thetaEstimate, double velocityEstimate, double timeDelay)
{
	return thetaMeas = thetaMeas + (1-cos(2*thetaEstimate)) * velocityEstimate * timeDelay /2;
}

//Correction using analytical formulation. Less efficient because of discritization error.
double Kalman::angleCorrectionForXandZDelay(double posX, double posZ, double velocity, double timeDelay, double& delayEstimatedVelocity)
{
	double A1, A2, A3, A4, D1, D2, D3, D4, angleM = 0.0, r = m_rotor[0].getRadiusRotorArm();

	posX = posX/1000.;
	posZ = posZ/1000.;

	double thetaZ = posZ/r;
	if(thetaZ > 1)
		thetaZ = 1;
	else if(thetaZ < -1)
		thetaZ = -1;
	
	double thetaX = posX/r;
	if(thetaX > 1)
		thetaX = 1;
	else if(thetaX < -1)
		thetaX = -1;

	A1 = constrainAngle(asin(thetaZ));
	A2 = constrainAngle(M_PI - asin(thetaZ));
	A3 = constrainAngle(acos(thetaX) + velocity*timeDelay);
	A4 = constrainAngle(-acos(thetaX) + velocity*timeDelay);

	D1 = constrainAngle(A1 - A3);
	D2 = constrainAngle(A1 - A4);
	D3 = constrainAngle(A2 - A3);
	D4 = constrainAngle(A2 - A4);

	if(fabs(D1) < fabs(D2) && fabs(D1) < fabs(D3) && fabs(D1) < fabs(D4))
	{
		angleM = A1;
		delayEstimatedVelocity = velocity + D1/timeDelay;
	}
	else if(fabs(D2) < fabs(D3) && fabs(D2) < fabs(D4))
	{
		angleM = A1;
		delayEstimatedVelocity = velocity + D2/timeDelay;
	}
	else if(fabs(D3) < fabs(D4))
	{
		angleM = A2;
		delayEstimatedVelocity = velocity + D3/timeDelay;
	}
	else
	{
		angleM = A2;
		delayEstimatedVelocity = velocity + D4/timeDelay;
	}

	return angleM;
}


double Kalman::getThetaProjection()
{
	return m_thetaProjection;
}
void Kalman::setThetaProjection(double prjAngle)
{
	m_thetaProjection = prjAngle;
}

bool Kalman::isGoodEstimate()
{
	return m_isGoodEstimate;
}

void Kalman::setTargetPositionZ(double posZ)
{
	m_targetZPosition = posZ;
}
void Kalman::setTargetPositionX(double posX)
{
	m_targetXPosition = posX;
}
