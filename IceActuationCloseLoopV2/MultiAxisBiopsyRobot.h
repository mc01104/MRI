//
//	 Project: Multi Axis MRI Biopsy needle Driver
//	    File: Kalman.h
//	 Version:
//	  Author: Ouajdi Felfoul, Children's Hospital Boston
//	    Date: 2014-01-14 
//
//	    Lang: C++
//
//	 Descrip: Controller for the multiaxis needle robot
//
//
/// \brief  File containing declarations for the multiaxis rotors controller
//         
//  ***************************************************************************/
//

#ifndef __MULTI_AXIS_BIOPSY_h
#define __MULTI_AXIS_BIOPSY_h

#include"CommonDefs.h"
#include "Kalman.h"
#include "rotor.h"

class MultiAxisBiopsyRobot
{
public:
	MultiAxisBiopsyRobot(void);
	~MultiAxisBiopsyRobot(void);
private:

	Kalman* estimatorRotorX;
	Kalman* estimatorRotorY;
	Kalman* estimatorRotorZ;

	rotor* rotorX;
	rotor* rotorY;
	rotor* rotorZ;
};


#endif //__MULTI_AXIS_BIOPSY_h