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
};


#include "MrServers/MrVista/include/unpack.h"

#endif // __FBData_H
