#include "CircleFit.h"

CircleFit::CircleFit(void)
{
	m_centerCX		=	0.0;
	m_centerCY		=	0.0;
	m_radius		=	0.0;
	m_centroid[0]	=	0.0;
	m_centroid[1]	=	0.0;
	m_moments[0]	=	0.0;
	m_moments[1]	=	0.0;
	m_moments[2]	=	0.0;
	m_moments[3]	=	0.0;
	m_moments[4]	=	0.0;
	m_moments[5]	=	0.0;
	m_nbElem		=	0;
	m_isCircleFound	=	false;
	m_posXSomme     = 0;
	m_posYSomme     = 0;
	m_dataInterval = 0;
}
CircleFit::CircleFit(long dataInterval)
{
	m_centerCX		=	0.0;
	m_centerCY		=	0.0;
	m_radius		=	0.0;
	m_centroid[0]	=	0.0;
	m_centroid[1]	=	0.0;
	m_moments[0]	=	0.0;
	m_moments[1]	=	0.0;
	m_moments[2]	=	0.0;
	m_moments[3]	=	0.0;
	m_moments[4]	=	0.0;
	m_moments[5]	=	0.0;
	m_nbElem		=	0;
	m_isCircleFound	=	false;
	m_dataInterval = dataInterval;
	m_posXSomme     = 0;
	m_posYSomme     = 0;

	

}

CircleFit::~CircleFit(void)
{
}

void CircleFit::getCenterCoordinates(double& x, double& y)
{
	x = m_centerCX;
	y = m_centerCY;
}


void CircleFit::incrementNbElem()
{
	m_nbElem++;
	return;
}
double CircleFit::getRadius(void)
{
	return m_radius;
}

void CircleFit::addNewPoint(double posX, double posY)
{
	m_positions[m_nbElem%MAX_NB_POS][0] = posX;
	m_positions[m_nbElem%MAX_NB_POS][1] = posY;
	m_posXSomme += posX;
	m_posYSomme += posY;

	if (m_dataInterval == 0)
	{
		incrementNbElem();
		updateCentroid(posX, posY);
		updateMoments(posX, posY);
	}
	else
	{ // Check if the data are not outlier
		calcMean();
		if(!(posX > (m_meanX + m_dataInterval) || posX < (m_meanX - m_dataInterval) ||
			 posY > (m_meanY + m_dataInterval) || posY < (m_meanY - m_dataInterval)))
		{
			incrementNbElem();
			updateCentroid(posX, posY);
			updateMoments(posX, posY);

		}
	}
	return;
}
void CircleFit:: calcMean()
{
	m_meanX = m_posXSomme / (m_nbElem+1);
	m_meanY = m_posYSomme / (m_nbElem+1);
}
void CircleFit::updateInterval(long dataInterval)
{
	m_dataInterval = dataInterval;
}

void CircleFit::updateCentroid(double posX, double posY)
{
	m_centroid[0] = m_centroid[0] + (posX - m_centroid[0])/m_nbElem;
	m_centroid[1] = m_centroid[1] + (posY - m_centroid[1])/m_nbElem;
	return;
}

void CircleFit::updateMoments (double posX, double posY)
{
	if(m_nbElem < MIN_NB_PTS)
		return;

	double Xi; //Centering data
	double Yi; 
	double Zi; 

	for(int i = 0; i<m_nbElem; i++)
	{
		Xi = m_positions[i][0]-m_centroid[0];
		Yi = m_positions[i][1]-m_centroid[1];
		Zi = Xi*Xi+Yi*Yi;
		m_moments[Mxx] += Xi*Xi ; // Mxx
		m_moments[Mxy] += Xi*Yi ; // Mxy
		m_moments[Mxz] += Xi*Zi ; // Mxz
		m_moments[Myy] += Yi*Yi ; // Myy
		m_moments[Myz] += Yi*Zi ; // Myz
		m_moments[Mzz] += Zi*Zi ; // Mzz
	}

	m_moments[Mxx] /= m_nbElem;
	m_moments[Mxy] /= m_nbElem;
	m_moments[Mxz] /= m_nbElem;
	m_moments[Myy] /= m_nbElem;
	m_moments[Myz] /= m_nbElem;
	m_moments[Mzz] /= m_nbElem;
/*
	if(m_nbElem== 6)
	{
		ICE_OUT("m_moments[Mxx] "   << m_moments[Mxx] );
		ICE_OUT("m_moments[Mxy] "   << m_moments[Mxy] );
		ICE_OUT("m_moments[Mxz] "   << m_moments[Mxz] );
		ICE_OUT("m_moments[Myy] "   << m_moments[Myy] );
		ICE_OUT("m_moments[Myz] "   << m_moments[Myz] );
		ICE_OUT("m_moments[Mzz] "   << m_moments[Mzz] );
		ICE_OUT("m_centroid[0] "   << m_centroid[0] );
		ICE_OUT("m_centroid[1] "   <<m_centroid[1] );
		ICE_OUT("m_nbElem "   << m_nbElem );
		ICE_OUT("PosX "   << posX );
		ICE_OUT("PosY "   << posY );
	}
*/
	return;
}


bool CircleFit::circleFitByTaubin()
{

	if(m_nbElem < MIN_NB_PTS)
		return false;

	//Remove outlier...The algorithm works very bad with oulier 
	// Computing the coefficients of the characteristic polynomial
	double Mz = m_moments[Mxx] + m_moments[Myy];// Mxx + Myy;
	double covXY = m_moments[Mxx]*m_moments[Myy] - m_moments[Mxy]*m_moments[Mxy]; //Mxx*Myy - Mxy*Mxy;
	double A3 = 4*Mz;
	double A2 = -3*Mz*Mz - m_moments[Mzz]; // -3*Mz*Mz - Mzz;
	double A1 = m_moments[Mzz]*Mz + 4*covXY*Mz - m_moments[Mxz]*m_moments[Mxz] - m_moments[Myz]*m_moments[Myz] - Mz*Mz*Mz;
		//A1 = Mzz*Mz + 4*covXY*Mz - Mxz*Mxz - Myz*Myz - Mz*Mz*Mz;
	double A0 = m_moments[Mxz]*m_moments[Mxz]*m_moments[Myy] + m_moments[Myz]*m_moments[Myz]*m_moments[Mxx] - 
		        m_moments[Mzz]*covXY - 2*m_moments[Mxz]*m_moments[Myz]*m_moments[Mxy] + Mz*Mz*covXY;
	double A22 = A2 + A2;
	double A33 = A3 + A3 + A3;
	double Dy;

	double xNew = 0, xOld;
	double yNew = 1e+20, yOld;

	for (int iter=1; iter < ITER_MAX; iter++)
	{
		yOld = yNew;
		yNew = A0 + xNew*(A1 + xNew*(A2 + xNew*A3));
		if (fabs(yNew) > fabs(yOld))
		{
			ICE_OUT("Newton-Taubin goes wrong direction: |ynew| > |yold|");
			xNew = 0;
			break;
		}
		Dy = A1 + xNew*(A22 + xNew*A33);
		xOld = xNew;
		xNew = xOld - yNew/Dy;
		if (fabs((xNew-xOld)/xNew) < EPSILON) 
		{
			break;
		}
		if (iter >= ITER_MAX)
		{
			ICE_OUT("Newton-Taubin will not converge");
			xNew = 0;
		}

		if (xNew<0.)
		{
			ICE_OUT("Newton-Taubin negative root:  x= \n" <<xNew);
			xNew = 0.0;
		}
	}

	//  computing the circle parameters

	double DET = xNew*xNew - xNew*Mz + covXY;
	double centerCX = (m_moments[Mxz]*(m_moments[Myy]-xNew)-m_moments[Myz]*m_moments[Mxy])/DET/2;
	double centerCY = (m_moments[Myz]*(m_moments[Mxx]-xNew)-m_moments[Mxz]*m_moments[Mxy])/DET/2;
    double radius   = sqrt(centerCX*centerCX +centerCY*centerCY+Mz );
	centerCX += m_centroid[0];
	centerCY += m_centroid[1];

	bool isConverge = circleFitCheckConvergence(centerCX, centerCY, radius);
	m_centerCX = centerCX;
	m_centerCY = centerCY;
	m_radius   = radius;

	return isConverge;
	/*
	if(m_nbElem== 6)
	{
	ICE_OUT("***************** OUFEL Inside circle fit ***********************");
	ICE_OUT("m_centerCX "   << m_centerCX <<  " centerY " <<  m_centerCY          );
	ICE_OUT(" radius "    << m_radius                                      );
	ICE_OUT("m_centroid[0] "   << m_centroid[0] <<  " m_centroid[1] " <<  m_centroid[1]          );
	ICE_OUT("m_moments[Mxz] "   << m_moments[Mxz] <<  " m_moments[Myz] " <<  m_moments[Myz]          );
	ICE_OUT("m_moments[Mxx]  "   << m_moments[Mxx]  <<  " m_moments[Mxy]  " <<  m_moments[Mxy]           );
	ICE_OUT("m_nbElem "   << m_nbElem <<  " m_moments[Mzz] " <<  m_moments[Mzz]          );
	//ICE_OUT("m_Coordinates[4][0] "   << m_Coordinates[4][0] <<  " m_Coordinates[4][1] " <<  m_Coordinates[4][1]          );
	//ICE_OUT("m_Coordinates[5][0] "   << m_Coordinates[5][0] <<  " m_Coordinates[5][1] " <<  m_Coordinates[5][1]          );
	ICE_OUT("***************** *********** ****************************");

	}
	*/


}

bool CircleFit::circleFitCheckConvergence(double x, double y, double r)
{
	if(m_nbElem < MIN_NB_PTS)
		return false;
	
	double error = (fabs(x-m_centerCX) + fabs(y-m_centerCY) + fabs(r-m_radius))/3;
	
	if(error < ERROR_MIN)
		return true;
	else
		return false;

}


bool CircleFit::isCircleFound()
{
	return m_isCircleFound;
}
