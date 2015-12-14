//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrServers\MrImaging\seq\FLASHBack\CompositeKernel.h
//	 Version:
//	  Author: KUEHBESZ
//	    Date: n.a.
//	    Date: n.a.
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: MR::Measurement::Sequence::SeqLoop
//
//	 Classes:
//
//	-----------------------------------------------------------------------------
#ifndef __CompositeKernel_h
#define __CompositeKernel_h 1


// * -------------------------------------------------------------------------- *
// * Includes                                                                   *
// * -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/libCSL/Leaf.h"




// * -------------------------------------------------------------------------- *
// * Import/export control                                                      *
// * -------------------------------------------------------------------------- *
//#ifdef BUILD_libSL
#ifdef BUILD_SEQU
  #define __OWNER
#endif

#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h" // import/export control





namespace SL  {



// ----------------------------------------------------------------------------
/// \brief Base class for kernel elements
///
// ----------------------------------------------------------------------------
class __IMP_EXP CompositeKernel : public Leaf  {

    public:


        // ------------------------------------------------------------------------------
        //
        // Name        :            CompositeKernel::CompositeKernel
        //
        // Description :
        ///              \brief     Constructor
        //
        // Parameters  :
        ///              \param     sIdent      String identifier of the leaf
        ///              \param     pMediator   Pointer to Mediator class 
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        CompositeKernel (const std::string& sIdent, Mediator* pMediator);



        // ------------------------------------------------------------------------------
        //
        // Name        :            CompositeKernel::CompositeKernel
        //
        // Description :
        ///              \brief     Constructor
        //
        // Parameters  :
        ///              \param     sScope      String identifier of the scope of this element
        ///              \param     sIdent      String identifier of the leaf
        ///              \param     pMediator   Pointer to Mediator class 
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        CompositeKernel (const std::string& sScope, const std::string& sIdent, Mediator* pMediator);



        // ------------------------------------------------------------------------------
        //
        // Name        :            CompositeKernel::~CompositeKernel
        //
        // Description :
        ///              \brief     Destructor
        //
        // Parameters  :
        ///              \param     -
        //
        // Return      :
        ///              \return    void
        // 
        // ------------------------------------------------------------------------------
        virtual ~CompositeKernel ();



};

} // End of namespace SL *
#endif