//-----------------------------------------------------------------------------
//  Copyright (C) Siemens AG 1999  All Rights Reserved.  Confidential
//-----------------------------------------------------------------------------
//
// Project: NUMARIS/4
//
//    File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\iPAT\iPAT.h
//
//  Author: n.a.
//          
//    Date: n.a.
//
//    Lang: C++
//
// Descrip: Implements common functionality for iPAT sequences.
//
//-----------------------------------------------------------------------------
#ifndef ___iPAT_h
#define ___iPAT_h
#include "ProtBasic/Interfaces/SeqLim.h"
#include "MrServers/MrProtSrv/MrProt/SeqIF/SeqExpo.h"
#include "MrServers/MrImaging/libSeqUtil/ReorderInfo.h"
#include "ProtBasic/Interfaces/MrPat.h"



#ifdef WIN32
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkLimited.h"
    #include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkSelection.h"
#endif



//
//  Handling of iPAT related UI-handlers
//
//      IMPORTANT: We have to destinguish between 'conventional' sequences and 'class-type' sequences:
//
//      - for conventional sequences, a global instance ('myUIHandlersPAT') of class UIHandlersPAT is provided, 
//        that contains UI related functionality common to all sequences with iPAT support.
//
//      - for class-type sequences, it is mandatory that each sequence object brings along its own instance of UIHandlersPAT.
//        Unfortunately, it is not possible to keep UIHandlersPAT within the actual sequence as a sequence class member, 
//        since the UILink framework wouldn't have access to it (UILink may access the sequence base class, 
//        but doesn't know about sequence variants).
//        For this reason, class SeqIF_UIHandlersPAT (derived from class SeqIF) is provided, which contains UIHandlersPAT
//        as a member together with the corresponding  getUIHandlersPAT()  function. 
//        If now a class-type sequence is derived from SeqIF_UIHandlersPAT instead of SeqIF, 
//        the UI can access UIHandlersPAT as follows
//                  (static_cast<SeqIF_UIHandlersPAT*>(pThis->sequence().getSeq()))->getUIHandlersPAT()
//      
//      Both variants are controled by a compiler flag SEQUENCE_CLASS.
//      The compiler flag acts on the method  getPointerToUIHandlersPAT():
//      
//      - default (no define SEQUENCE_CLASS):
//        global instance 'myUIHandlersPAT' is used
//
//      - SEQUENCE_CLASS is defined: 
//        pointer to UIHandlersPAT is retrieved from the sequence class (see above)
//      
//
//      The method PrepareCalculation is called from within iPAT.cpp. For sequences that switch
//      between different reorder methods, e.g., reorderGRE() and reorder3dE(), it is necessary
//      to initialize the reorder object before the call to choose the correct PrepareCalculation() function.
//
//      For non class based sequences this method is registered as a function pointer in the call
//          fPATRegisterUILinkHandlersForSegmentedSequences(
//              pSeqLim, &Reorder, SEQ::INC_GRE_SEGMENTS, lPATDefOptNoRefLines, NULL, mySetReorderMethod))
//      where mySetReorderMethod(...) is of type bool fun(MrProtocolData::MrProtData* , SeqLim*)
//
//      Class based sequences do not register this function pointer, but must overload the method
//      bool SeqIF_UIHandlersPAT::PATInitializeReorderMethod(MrProtocolData::MrProtData* , SeqLim*);
// 
//      in iPAT.cpp the return values are presently ignored.


#include "MrServers/MrImaging/libSBB/StdSeqIF.h"       // for class SeqIF


namespace SEQ_NAMESPACE
{

// 
// class UIHandlersPAT
//
class UIHandlersPAT  {

    public:
        UIHandlersPAT();

        #ifdef WIN32

		    /// Functions for accessing original UILink functions, esp. setValue handlers.
		    ///   The pointers to these functions are stored as member variables
            MrUILinkLimited<long>::PFctSetValue         get_PAT_OrigTurboFactorSetFct() const           { return (m_pPAT_OrigSetValue_TurboFactor); }
            MrUILinkLimited<long>::PFctSetValue         get_PAT_OrigSegmentsSetFct() const              { return (m_pPAT_OrigSetValue_Segments); }
            MrUILinkLimited<long>::PFctSetValue         get_PAT_OrigBaseResolutionSetFct() const        { return (m_pPAT_OrigSetValue_BaseResolution); }
			// - original acceleration factor PE will also be used for PAT averaging
            MrUILinkLimited<long>::PFctSetValue         get_PAT_OrigAccelFactorPESetFct() const         { return (m_pPAT_OrigSetValue_PATAccelFActorPE); }

            MrUILinkLimited<double>::PFctSetValue       get_PAT_OrigReadFOVSetFct() const               { return (m_pPAT_OrigSetValue_ReadFOV); }
            MrUILinkLimited<double>::PFctSetValue       get_PAT_OrigPhaseFOVSetFct() const              { return (m_pPAT_OrigSetValue_PhaseFOV); }
            MrUILinkLimited<double>::PFctSetValue       get_PAT_OrigPhaseOversamplingSetFct() const     { return (m_pPAT_OrigSetValue_PhaseOversampling); }
            MrUILinkLimited<double>::PFctSetValue       get_PAT_OrigPhaseResolutionSetFct() const       { return (m_pPAT_OrigSetValue_PhaseResolution); }

            MrUILinkSelection<unsigned>::PFctSetValue   get_PAT_OrigPhasePartialFourierSetFct() const   { return (m_pPAT_OrigSetValue_PhasePartialFourier); }
            MrUILinkSelection<unsigned>::PFctSetValue   get_PAT_OrigPATModeSetFct() const               { return (m_pPAT_OrigSetValue_PATMode); }

			/// For, e.g., PATaveraging: original setValue handler for PATRefScanMode
            MrUILinkSelection<unsigned>::PFctSetValue   get_PAT_OrigRefScanModeSetFct() const           { return (m_pPAT_OrigSetValue_RefScanMode); }
			// - also: original setValue handler for Averages
            MrUILinkLimited<long>::PFctSetValue         get_OrigAveragesSetFct() const                  { return (m_pOrigSetValue_Averages); }
            // - for automatic refScanMode switching: need (extended) solveHandler
            MrUILinkSelection<unsigned>::PFctSolve      get_PAT_OrigRefScanModeSolveFct() const         { return (m_pPAT_OrigSolve_RefScanMode); }
            // - PATaveraging: restrict the display of the RefScanMode to the automatically chosen one
            MrUILinkSelection<unsigned>::PFctGetOptions get_PAT_OrigRefScanModeGetOptionsFct() const    { return (m_pPAT_OrigGetOptions_RefScanMode); }

            MrUILinkLimited<long>::PFctGetLimits        get_PAT_OrigRefLinesPEGetLimitsHandler() const  { return (m_pPAT_OrigGetLimits_PATRefLinesPE); }


			/// Functions for storing the pointers to original UILink functions as member variables
            void set_PAT_OrigTurboFactorSetFct          (MrUILinkLimited<long>::PFctSetValue pFctPtr)       { m_pPAT_OrigSetValue_TurboFactor         = pFctPtr; }
            void set_PAT_OrigSegmentsSetFct             (MrUILinkLimited<long>::PFctSetValue pFctPtr)       { m_pPAT_OrigSetValue_Segments            = pFctPtr; }
            void set_PAT_OrigBaseResolutionSetFct       (MrUILinkLimited<long>::PFctSetValue pFctPtr)       { m_pPAT_OrigSetValue_BaseResolution      = pFctPtr; }
            void set_PAT_OrigAccelFactorPESetFct        (MrUILinkLimited<long>::PFctSetValue pFctPtr)       { m_pPAT_OrigSetValue_PATAccelFActorPE    = pFctPtr; }

            void set_PAT_OrigReadFOVSetFct              (MrUILinkLimited<double>::PFctSetValue pFctPtr)     { m_pPAT_OrigSetValue_ReadFOV             = pFctPtr; }
            void set_PAT_OrigPhaseFOVSetFct             (MrUILinkLimited<double>::PFctSetValue pFctPtr)     { m_pPAT_OrigSetValue_PhaseFOV            = pFctPtr; }
            void set_PAT_OrigPhaseOversamplingSetFct    (MrUILinkLimited<double>::PFctSetValue pFctPtr)     { m_pPAT_OrigSetValue_PhaseOversampling   = pFctPtr; }
            void set_PAT_OrigPhaseResolutionSetFct      (MrUILinkLimited<double>::PFctSetValue pFctPtr)     { m_pPAT_OrigSetValue_PhaseResolution     = pFctPtr; }

            void set_PAT_OrigPhasePartialFourierSetFct  (MrUILinkSelection<unsigned>::PFctSetValue pFctPtr) { m_pPAT_OrigSetValue_PhasePartialFourier = pFctPtr; }
            void set_PAT_OrigPATModeSetFct              (MrUILinkSelection<unsigned>::PFctSetValue pFctPtr) { m_pPAT_OrigSetValue_PATMode             = pFctPtr; }

			/// For, e.g., PATaveraging setValue, see above
            void set_PAT_OrigRefScanModeSetFct          (MrUILinkSelection<unsigned>::PFctSetValue pFctPtr) { m_pPAT_OrigSetValue_RefScanMode         = pFctPtr; }
			// - also: Averages setValue
            void set_OrigAveragesSetFct                 (MrUILinkLimited<long>::PFctSetValue pFctPtr)       { m_pOrigSetValue_Averages                = pFctPtr; }
            // - for automatic refScanMode switching: need (extended) solveHandler
            void set_PAT_OrigRefScanModeSolveFct        (MrUILinkSelection<unsigned>::PFctSolve pFctPtr)    { m_pPAT_OrigSolve_RefScanMode            = pFctPtr; }
            // - restriction to automatic mode for PATaveraging
            void set_PAT_OrigRefScanModeGetOptionsFct   (MrUILinkSelection<unsigned>::PFctGetOptions pFctPtr) { m_pPAT_OrigGetOptions_RefScanMode     = pFctPtr; }

			// refLines PE getLimits handler will also be used for PATaveraging
            void set_PAT_OrigRefLinesPEGetLimitsHandler (MrUILinkLimited<long>::PFctGetLimits pFctPtr)      { m_pPAT_OrigGetLimits_PATRefLinesPE      = pFctPtr; }

            void set_lUILinkPATRefLinesOpt (long lValue) { m_lUILinkPATRefLinesOpt = lValue;  }
            long get_lUILinkPATRefLinesOpt (void) const  { return ( m_lUILinkPATRefLinesOpt ); }

            void         set_pReorder (ReorderInfo* pRI) { m_pReorder = pRI;  }
            ReorderInfo* get_pReorder (void) const       { return ( m_pReorder );  }
        #endif

    protected:
        #ifdef WIN32

		    /// Member variables for storing pointers to original UILink functions
            MrUILinkLimited  <long>    ::PFctSetValue   m_pPAT_OrigSetValue_TurboFactor;
            MrUILinkLimited  <long>    ::PFctSetValue   m_pPAT_OrigSetValue_Segments;
            MrUILinkLimited  <long>    ::PFctSetValue   m_pPAT_OrigSetValue_BaseResolution;
            MrUILinkLimited  <long>    ::PFctSetValue   m_pPAT_OrigSetValue_PATAccelFActorPE;

            MrUILinkLimited  <double>  ::PFctSetValue   m_pPAT_OrigSetValue_ReadFOV;
            MrUILinkLimited  <double>  ::PFctSetValue   m_pPAT_OrigSetValue_PhaseFOV;
            MrUILinkLimited  <double>  ::PFctSetValue   m_pPAT_OrigSetValue_PhaseOversampling;
            MrUILinkLimited  <double>  ::PFctSetValue   m_pPAT_OrigSetValue_PhaseResolution;

            MrUILinkSelection<unsigned>::PFctSetValue   m_pPAT_OrigSetValue_PhasePartialFourier;
            MrUILinkSelection<unsigned>::PFctSetValue   m_pPAT_OrigSetValue_PATMode;

			// For PATaveraging
            MrUILinkSelection<unsigned>::PFctSetValue   m_pPAT_OrigSetValue_RefScanMode;
            MrUILinkLimited  <long>    ::PFctSetValue   m_pOrigSetValue_Averages;
            // - for automatic refScanMode selection
            MrUILinkSelection<unsigned>::PFctSolve      m_pPAT_OrigSolve_RefScanMode;
            // - only display automatic refScanMode
            MrUILinkSelection<unsigned>::PFctGetOptions m_pPAT_OrigGetOptions_RefScanMode;

            MrUILinkLimited  <long>    ::PFctGetLimits  m_pPAT_OrigGetLimits_PATRefLinesPE;

            long         m_lUILinkPATRefLinesOpt;
            ReorderInfo* m_pReorder;

        #endif
};


class SeqIF_UIHandlersPAT : public StdSeqIF
{

    public:
        UIHandlersPAT * getUIHandlersPAT() { return (&m_OrigHandlersPAT); }

        virtual bool PATInitializeReorderMethod ( MrProt & , SeqLim & ) = 0;
        virtual bool PATInitializeReorderMethod(MrProtocolData::MrProtData* , SeqLim*);
        virtual bool fPATCheckAndUpdateReferenceLineNumber(MrProtocolData::MrProtData* , SeqLim*, ReorderInfo*, long );

        // facility to store and retrieve an arbitrary pointer
        // this allows to implement a WIP UI that is used with several sequences
        // and accessed via getSequence()
        void * getGenericUIPointer() { return m_pGenericUI;};
        void * setGenericUIPointer(void * pUI) { void * pOld = m_pGenericUI; m_pGenericUI = pUI; return pOld; };


    protected:
        UIHandlersPAT   m_OrigHandlersPAT;
        void * m_pGenericUI;
};


// ------------------------------------------------------------------------------
// Function    : fPATRegisterUILinkHandlersForSegmentedSequences
// ------------------------------------------------------------------------------
//               
// Description : Registers SetValue-handlers for all parameters that influence
//               the number of phase encoding lines. Those set value handlers
//               are typically needed for segmented sequences.
//
//               NOTE: should be called late in fSEQInit, i.e. after all UILink
//                     registration steps have been performed.
//
//               Pointers to orig. setValueHandlers and additional information (e.g. pointer to
//               ReorderInfo class, default number of Ref.Lines, etc) are stored in 
//               class UIHandlersPAT.
//
//               IMPORTANT:
//               'class-type' sequences may specify a pointer to an own object of UIHandlersPAT,
//               otherwise (i.e. pointer is NULL) the global instance 'myUIHandlersPAT' will be used instead
//
// Return      : true , if for success
//               false, else
//
// ------------------------------------------------------------------------------
bool fPATRegisterUILinkHandlersForSegmentedSequences(SeqLim*,ReorderInfo*,SEQ::Increment,long, 
                                                     UIHandlersPAT * pUIHandlersPAT = NULL );


NLS_STATUS fPATSetDefaultSeqLim (SeqLim*, bool bInitPAT3D=false);
NLS_STATUS fPATPrepPost         (MrProtocolData::MrProtData* , SeqLim*, SeqExpo*, ReorderInfo *pReorderInfo=NULL);



bool fPATRegisterUILinkHandlersForSegmentedSequences(SeqLim&,ReorderInfo*,SEQ::Increment,long, 
                                                     UIHandlersPAT * pUIHandlersPAT = NULL );

/*
NLS_STATUS fPATSetDefaultSeqLim (SeqLim&, bool bInitPAT3D=false);
NLS_STATUS fPATPrepPost         (MrProt& , SeqLim&, SeqExpo&, ReorderInfo *pReorderInfo=NULL);
*/

#ifdef WIN32
static const long lPATDefOptNoRefLines = 24;

static const long lPATDefOptNoRefLinesIntrinsic = 32;

template<class TYPE>
const UIHandlersPAT *getPointerToUIHandlersPAT(TYPE* pThis);


template<class TYPE_RET, class TYPE>
TYPE_RET _PATSetValueAndUpdateRefLines (TYPE* const _this, TYPE_RET _NewVal, long lIndex, typename TYPE::PFctSetValue pOrigSetValue)
{

    TYPE_RET _ret;

    const UIHandlersPAT *pUIHandlersPAT = getPointerToUIHandlersPAT(_this);

    if (pOrigSetValue)
    {   
        _ret = (*pOrigSetValue)(_this,_NewVal,lIndex);
    }
    else
    {
        // can't help => doing nothing
        return _this->value(lIndex);
    }

    if (_this->prot().getsPat().getucPATMode()!=SEQ::PAT_MODE_NONE)
    {
        if( pUIHandlersPAT == NULL )
        {
            // can't help => doing nothing
            return _this->value(lIndex);
        }


        //bool bSuccess = fPATCheckAndUpdateReferenceLineNumber(&_this->prot(), const_cast<SeqLim*>(&_this->seqLimits()), pUIHandlersPAT->get_pReorder(), pUIHandlersPAT->get_lUILinkPATRefLinesOpt() );
		// - compiler complains about unused variable bSuccess
        (void)
#ifdef SEQUENCE_CLASS
        (static_cast<SeqIF_UIHandlersPAT*>(_this->sequence().getSeq()))->
#endif
        fPATCheckAndUpdateReferenceLineNumber(&_this->prot(), const_cast<SeqLim*>(&_this->seqLimits()), pUIHandlersPAT->get_pReorder(), pUIHandlersPAT->get_lUILinkPATRefLinesOpt() );
        // NOTE:
        // We can't help, if fPATCheckAndUpdateReferenceLineNumber can't help,
        // so return status bSuccess is not checked, fSEQPrep will reject the
        // the new protocol anyway.
    }

    return _ret;
}
#endif
}

#endif // ___iPAT_h

