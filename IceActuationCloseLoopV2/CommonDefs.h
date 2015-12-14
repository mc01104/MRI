//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 2003  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrServers\MrVista\include\Common\MrCommonDefs.h
//	 Version:
//	  Author: CC_IMAMANAG
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Usefull macros, definitions, and typedefs.
//	
//
//	 Classes: 
//
//	---------------------------------------------------------

#ifndef __COMMON_DEFS__
#define __COMMON_DEFS__

#define NB_MAX_CAPSULES 3
#define NB_AXIS 3 // x, y and z
#define MAX_NB_WAYPOINTS 30
#define PRJ_SIZE 2048
#define INTERLEAVED 0
#define SIMULTANEOUS 1
#define MAX_PTS_CIRCLE 1000
#define LARMOR_FREQ    42580000
#define ROTOR 1
#define CAPSULE 2
#define TWO_CAPSULES 3

enum CONTROL_TYPE {
	OPEN_LOOP       =  0,
	MAX_TORQUE      =  1,
	FBLINEARIZATION =  2,
	PI              =  3,
	LYAPUNOV_POS    =  4,
	LYAPUNOV_VEL    =  5,
};


enum rtPlane{
	X_Y = 0,
	X_Z = 1,
	Y_Z = 2,
};

enum SequenceType{
	SPIN_ECHO = 0,
	GRADIENT_ECHO = 1,
};

#endif // if !defined(MrCommonDefs_H)
