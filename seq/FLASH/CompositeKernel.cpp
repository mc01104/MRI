//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrServers\MrImaging\seq\FLASHBack\CompositeKernel.cpp
//	 Version:
//	  Author: Clinical
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Deklarations for CompositeKernel.cpp
//
//	 Classes:
//
//	-----------------------------------------------------------------------------



// * -------------------------------------------------------------------------- *
// * Includes                                                                   *
// * -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/seq/FLASH/CompositeKernel.h"





// * -------------------------------------------------------------------------- *
// * Namespace                                                                  *
// * -------------------------------------------------------------------------- *
using namespace SL;




CompositeKernel::CompositeKernel (const std::string& sIdent, Mediator* pMediator)
    : Leaf  ("Std", sIdent, pMediator)
{
    // * ---------------------------------------------------------------------- *
    // * Set the trait of the element                                           *
    // * ---------------------------------------------------------------------- *
    seteTrait (KERNEL);

    // * ---------------------------------------------------------------------- *
    // * Kernel is active during sequence check                                 *
    // * ---------------------------------------------------------------------- *
    setCheckMode (SL::CHECK_MODE_ACTIVE, SL::ELEMENT);
}


CompositeKernel::CompositeKernel (const std::string& sScope, const std::string& sIdent, Mediator* pMediator)
    : Leaf  (sScope, sIdent, pMediator)
{
    // * ---------------------------------------------------------------------- *
    // * Set the trait of the element                                           *
    // * ---------------------------------------------------------------------- *
    seteTrait (KERNEL);

    // * ---------------------------------------------------------------------- *
    // * Kernel is active during sequence check                                 *
    // * ---------------------------------------------------------------------- *
    setCheckMode (SL::CHECK_MODE_ACTIVE, SL::ELEMENT);
}


CompositeKernel::~CompositeKernel ()
{}