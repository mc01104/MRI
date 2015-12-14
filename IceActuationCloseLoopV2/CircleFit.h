//
//	 Project: MRI Biopsy needle Driver
//	    File: Kalman.h
//	 Version:
//	  Author: Ouajdi Felfoul, Children's Hospital Boston
//	    Date: 2014-01-14 
//
//	    Lang: C++
//
//	 Descrip: Circle fit by Taubin c++ code: takes an array of [x,y] coordinates.
//            Finds the center and radius that best fits this data.
//     
//      G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
//                  Space Curves Defined By Implicit Equations, With 
//                  Applications To Edge And Range Image Segmentation",
//      IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)
//
//            based on Matlab code by 
//            available at www.
//
/// \brief  File containing declarations for the Kalman filter
//         
//  ***************************************************************************/
//

#ifndef __CIRCLEFIT__
#define __CIRCLEFIT__

#include <cmath>
#include "MrServers/MrVista/include/Parc/Trace/IceTrace.h"

#define ITER_MAX	20
#define EPSILON     1e-12
#define ERROR_MIN   0.1

#define Mxx			0
#define Mxy			1
#define Mxz			2
#define Myy			3
#define Myz			4
#define Mzz			5

#define MAX_NB_POS	100

#define MIN_NB_PTS  60

class CircleFit
{
public:
	CircleFit(void);
	CircleFit(long dataInterval);
	~CircleFit(void);
	void getCenterCoordinates(double& x, double& y);
	double getRadius(void);
	void updateCentroid(double posX, double posY);
	void updateMoments (double posX, double posY);
	bool circleFitByTaubin();
	void addNewPoint(double PosX, double posY);
	bool isCircleFound();
	void calcMean();
	void updateInterval(long dataInterval);
private:
	void incrementNbElem();
	bool circleFitCheckConvergence(double x, double y, double r);
	// Rotor position (center and radius)
	double              m_centerCX;
	double				m_centerCY;
	double				m_radius;
	double				m_centroid[2];
	double				m_moments[6];
	int					m_nbElem;
	double				m_positions[MAX_NB_POS][2];	
	bool				m_isCircleFound;
	long				m_dataInterval;
	double              m_posXSomme;
	double              m_posYSomme;
	double              m_meanX;
	double              m_meanY;
};
#endif