#ifndef __ROTOR__
#define __ROTOR__

#define _USE_MATH_DEFINES

#include <cmath>
// IceWrapper
#include "MrServers/MrVista/include/Ice/IceAlgos/IceWrapper.h"
#include "MrServers/MrVista/include/Parc/Trace/IceTrace.h"


class rotor
{
public:
	rotor(void);
	~rotor(void);

	void	setBearingRadius (double br);
	void	setRadiusRotorArm(double rra);
	void	setSaturationMag (double sat);
	void	setBearingDensity(double den);
	void	setLoadTorque    (double load);
	void	setCenterCoordinates(double centerX, double centerY);
	void    setRadius(double radius);
	void	setStaticFriction(double statFric);
	void	setViscousFriction(double visFric);
	void    setBoundaries(double, double, double, double);

	double getVolMag();
    double getBearingVol();
	double gettotalInertia();
	double getMassBearing();
	double getViscousFriction();
	double getRadiusRotorArm();
	double getLoad();
	double getSaturationMag();
	double getFriction(double velocityEstimate);
	double getStaticFriction();
	double getXPosition();
	double getYPosition();
	double getRadius();
	double getCenterX();
	double getCenterY();

	void   updateRotorPosition(IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, uint16_t switchControl);

	// TODO add setters
private:

		//Utility method
	void indexTo_mm(long prjAxis, long iceObjColLen);// fills m_position, m_positionKMinus1 and m_positionKMinus2

	double m_bearingRadius;
	double m_saturationMag;
	double m_bearingDensity;
	double m_radiusRotorArm;
	double m_loadTorque;
	double m_staticFriction;
	double m_viscousFriction; 

	// Rotor position (center and radius)
	double m_center[2]; // We suppose the rotor is spinning in 1 plane
	// Actual position of the rotor
	double m_positionIndex[2];//
	double m_position[2];  //

	double				m_positionKMinus1[2];// save the previous 2 positions for a second order finit difference velocity estimation
	double				m_positionKMinus2[2];// 

	long				m_boundariesMinus[2];
	long				m_boundariesPlus[2];

	bool m_isCircleFound;

	// Provided for user convenience
    IceWrapper Ice;


};

#endif