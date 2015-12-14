#include "rotor.h"

rotor::rotor(void)
{
	m_bearingRadius  = 0.003; //m
	m_saturationMag  = 1.36*pow(10.0,6.0); 
	m_radiusRotorArm = 0.036; //m
	m_loadTorque     = 0.0;
	m_staticFriction = 8.0*pow(10.0,-5.0);//8.0*pow(10.0,-5.0)
	m_viscousFriction = 100*pow(10.0,-7.0);// 100*pow(10.0,-7.0);
	m_bearingDensity  = 8530;
	m_center[0] = -1;
	m_center[1] = -1;
	m_positionIndex[0] = -1;//
	m_positionIndex[1] = -1;
	m_position[0] = -1;//
	m_position[1] = -1;

	m_positionKMinus1[0] = -1;// save the previous 2 positions for a second order finit difference velocity estimation
	m_positionKMinus2[1] = -1;// 

	m_boundariesMinus[0] = -1;
	m_boundariesMinus[1] = -1;
	m_boundariesPlus[0] = -1;
	m_boundariesPlus[1] = -1;


	m_isCircleFound  = false;
}

rotor::~rotor(void)
{
}

double rotor::getVolMag()
{
	return  getBearingVol()* m_saturationMag; //bearing volume* magnetic susceptibility
}


double rotor::getBearingVol()
{
	return (4.0f/3.0f)*M_PI*pow(m_bearingRadius,3);

}

double rotor::getMassBearing()
{
	return getBearingVol()*m_bearingDensity; //  7850 
}

double rotor::gettotalInertia()
{
	return 2*getMassBearing()*m_radiusRotorArm*m_radiusRotorArm;
}


double rotor::getRadiusRotorArm()
{
	return m_radiusRotorArm;
}
double rotor::getViscousFriction()
{
	return m_viscousFriction;
}

double rotor::getLoad()
{
	return m_loadTorque;
}
double rotor::getSaturationMag()
{
	return m_saturationMag;
}

double rotor::getFriction(double velocityEstimate)
{

	if (velocityEstimate > 0)
		return m_staticFriction;
	else
		if(velocityEstimate < 0)
			return -1*m_staticFriction;
		else
			return 0.0;

}

double rotor::getStaticFriction()
{

	return  m_staticFriction;

}

void rotor::setBearingRadius (double br)
{
	m_bearingRadius = br;
}
void rotor::setRadiusRotorArm(double rra)
{
	m_radiusRotorArm = rra;
}
void rotor::setSaturationMag (double sat)
{
	m_saturationMag = sat;
}
void rotor::setBearingDensity(double den)
{
	m_bearingDensity = den;
}
void rotor::setLoadTorque (double load)
{
	m_loadTorque = load;
}
void rotor::setStaticFriction(double statFric)
{
	m_staticFriction = statFric;
}
void rotor::setViscousFriction(double visFric)
{
	m_viscousFriction = visFric;
}
void rotor::setCenterCoordinates(double centerX, double centerY)
{
	m_center[0] = centerX;
	m_center[1] = centerY;
}


void rotor::setBoundaries(double minusX, double plusX, double minusY, double plusY)
{
	m_boundariesMinus[0] = minusX;
	m_boundariesMinus[1] = minusY;
	m_boundariesPlus[0]  = plusX;
	m_boundariesPlus[1]  = plusY;
}

void rotor::updateRotorPosition(IceAs& magDataFTAs, long iceObjColLen, uint16_t prjAxis, uint16_t switchControl)
{
	long lMaxLigne = 0, lMaxCol = 0;
	double theta, rotatedCenter;

	if(m_isCircleFound == false) // Rotor circle not yet found. Look at the peak in all the extent of the received data
	{
		magDataFTAs.modify(COL, 0, iceObjColLen, 1);//,
		Ice.CalcStatistics(magDataFTAs, IsmMax);
		Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
		m_positionIndex[prjAxis-1] = lMaxLigne;
		//lPeakPos[0] =  lMaxLigne;
	}
	else //Look at the peak inside the boundaries of the circle
	{
		magDataFTAs.modify(COL, m_boundariesMinus[prjAxis-1], m_boundariesPlus[prjAxis-1] - m_boundariesMinus[prjAxis-1] , 1);
		Ice.CalcStatistics(magDataFTAs, IsmMax);
		Ice.GetStatisticsMaxPos(lMaxLigne, lMaxCol);
		m_positionIndex[prjAxis-1] = lMaxLigne + m_boundariesMinus[prjAxis-1];
		//lPeakPos[0] =  lMaxLigne + m_boundariesMinus[0][prjAxis-1];
	}
	indexTo_mm(prjAxis, iceObjColLen);
}


double rotor::getXPosition()
{
	return m_position[0];
}
double rotor::getYPosition()
{
	return m_position[1];
}

double rotor::getCenterX()
{
	return m_center[0];
}
double rotor::getCenterY()
{
	return m_center[1];
}

void rotor::indexTo_mm(long prjAxis, long iceObjColLen)// fills m_position, m_positionKMinus1 and m_positionKMinus2
{		
	m_positionKMinus2[prjAxis-1] = m_positionKMinus1[prjAxis-1];
	m_positionKMinus1[prjAxis-1] = m_position[prjAxis-1];
	m_position[prjAxis-1] = ((m_positionIndex[prjAxis-1]/(static_cast <double> (iceObjColLen))) - 0.5)*600;
}
