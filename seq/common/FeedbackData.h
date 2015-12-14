//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_appl4\pkg\MrApplications\MrService\Seq\Ice\IceProgFbPerformance\FeedbackData.h
//	 Version:
//	  Author: CC_SESO WUEBMAEJ
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: data transportation class to fb_performance seq
//	
//
//	 Classes: FBData
//
//	---------------------------------------------------------
           
#ifndef __FBData_H        
#define __FBData_H   

#include "MrServers/MrVista/include/pack.h"

struct FBData
{
  PACKED_MEMBER(uint32_t, lSlot);
  PACKED_MEMBER(uint32_t, lShot);
  PACKED_MEMBER(uint32_t, SimultORInterleaved);
  PACKED_MEMBER(double, prjAngle);
  PACKED_MEMBER(double, uValx[10]);
  PACKED_MEMBER(double, uValy[10]);
  PACKED_MEMBER(double, uValz[10]);

/*
  PACKED_MEMBER(double, uValx1);
  PACKED_MEMBER(double, uValx2);
  PACKED_MEMBER(double, uValx3);
  PACKED_MEMBER(double, uValx4);
  PACKED_MEMBER(double, uValx5);
  PACKED_MEMBER(double, uValx6);
  PACKED_MEMBER(double, uValx7);
  PACKED_MEMBER(double, uValx8);
  PACKED_MEMBER(double, uValx9);
  PACKED_MEMBER(double, uValx10);

  PACKED_MEMBER(double, uValy1);
  PACKED_MEMBER(double, uValy2);
  PACKED_MEMBER(double, uValy3);
  PACKED_MEMBER(double, uValy4);
  PACKED_MEMBER(double, uValy5);
  PACKED_MEMBER(double, uValy6);
  PACKED_MEMBER(double, uValy7);
  PACKED_MEMBER(double, uValy8);
  PACKED_MEMBER(double, uValy9);
  PACKED_MEMBER(double, uValy10);

  PACKED_MEMBER(double, uValz1);
  PACKED_MEMBER(double, uValz2);
  PACKED_MEMBER(double, uValz3);
  PACKED_MEMBER(double, uValz4);
  PACKED_MEMBER(double, uValz5);
  PACKED_MEMBER(double, uValz6);
  PACKED_MEMBER(double, uValz7);
  PACKED_MEMBER(double, uValz8);
  PACKED_MEMBER(double, uValz9);
  PACKED_MEMBER(double, uValz10);
*/
};

#include "MrServers/MrVista/include/unpack.h"

#endif // __FBData_H
