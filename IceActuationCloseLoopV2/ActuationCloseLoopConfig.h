//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrApplications\MrService\Seq\Ice\IceProgFbPerformance\FbPerformanceComposer.h
//	 Version:
//	  Author: SeSo_General 
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Composer for feedback performance imager test
//	
//
//	 Classes: FbPerformanceProtocolComposer
//
//	---------------------------------------------------------

#ifndef __ACTUATIONCLOSELOOPCONFIG_h
#define __ACTUATIONCLOSELOOPCONFIG_h

#include "MrServers/MrVista/include/Parc/ProtocolComposer.h"

class ActuationCloseLoopConfigurator : public Parc::Component, public ProtocolComposer::IComposer
{
  public:
    DECLARE_PARC_COMPONENT(ActuationCloseLoopConfigurator);

    virtual IResult Compose( ProtocolComposer::Toolbox& toolbox );
};
#endif // __ACTUATIONCLOSELOOPCONFIG_h
