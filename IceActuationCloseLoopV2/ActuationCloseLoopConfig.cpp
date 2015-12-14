//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_appl4\pkg\MrApplications\MrService\Seq\Ice\IceProgFbPerformance\FBComposer\FbPerformanceComposer.cpp
//	 Version:
//	  Author: SeSo_General 
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: 
//	
//
//	 Classes: FbPerformanceComposer
//
//	---------------------------------------------------------

#include "ActuationCloseLoopConfig.h" 

IResult ActuationCloseLoopConfigurator::Compose( ProtocolComposer::Toolbox& toolbox )
{
  using namespace ProtocolComposer;

  // compose the IRIS node
  toolbox.Init( "IRIS" );

  // get aquisition pipe service
  MrPipeService pipeService = toolbox.FindPipeService("acquisition");

  // create fbperformance functor
  MrFunctor functor = pipeService.CreateFunctor( "RTFeedback",  
                                                "ActuationCloseLoop@IceProgActuationCloseLoopV2" );

  // connect to root functor
  functor.ConnectInput( "ComputeScan" , "FeedbackRoot", "ScanReady" );

  return I_OK;
}

//#include "MrServers/MrVista/include/Parc/Reflection/ObjectMap.h"

//BEGIN_OBJECT_MAP()
 // OBJECT_ENTRY(ActuationCloseLoopConfigurator)
//END_OBJECT_MAP()
