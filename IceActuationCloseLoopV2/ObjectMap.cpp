//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 2005  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: 
//	 Version:
//	  Author: BV_ICE
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: 
//	 Classes:
//
//	-----------------------------------------------------------------------------

// IceMemoryDemoConfigurator:
#include "MrServers/MrVista/Ice/IceIdeaFunctors/IceActuationCloseLoopV2/IceProgActuationCloseLoop.h"
// IceMemoryDemoFunctor:
#include "MrServers/MrVista/Ice/IceIdeaFunctors/IceActuationCloseLoopV2/ActuationCloseLoopConfig.h"

// OBJECT_MAP macro definitions
#include "MrServers/MrVista/include/Parc/Reflection/ObjectMap.h"

// Export of classes outside of this Dll (needed once per Dll):
// make objects loadable for MrParc

BEGIN_OBJECT_MAP()
  OBJECT_ENTRY(ActuationCloseLoop)
  OBJECT_ENTRY(ActuationCloseLoopConfigurator)
END_OBJECT_MAP()