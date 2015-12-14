// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FLASH_UI.cpp
//   Author: H IM MR PLM AW T-AFW
//
//     Lang: C++
//
//  Descrip: Source file for FLASH Sequence UI
//
// --------------------------------------------------------------------------------------------

#include "MrServers/MrImaging/seq/FLASH/FLASH_UI.h"


#ifdef WIN32
#include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"
#include <sstream>
#endif


#ifndef SEQ_NAMESPACE
#error SEQ_NAMESPACE not defined
#endif

using namespace SEQ_NAMESPACE;
using namespace WPT_NAMESPACE;


void FlashUI::createWIPParameters (SeqLim &rSeqLim, WIPParameterTool& rTool)
{
    // --------------------------------------------------------------------------------------------
    // Create the following WIP parameters:
    //
    // - WIPLong
    // - WIPDouble
    // - WIPBool        (checkbox)
    // - WIPSelection   (drop down list)
    // - WIPMaxTE       (show how to solve conflicts with standard sequence parameters)
    // - WIPOnlineRecon (checkbox to activate online reconstruction)
    // --------------------------------------------------------------------------------------------

    rTool.createLongParameter      (WIPLong,      rSeqLim, "WIP long", NULL, 0, 100, 2, 12);

    rTool.createDoubleParameter    (WIPDouble,    rSeqLim, "WIP double", "mm", 2, 0.15, 2.8, 0.05, 2.1);

    rTool.createBoolParameter      (WIPBool,      rSeqLim, "WIP bool", true);


    // --------------------------------------------------------------------------------------------
    // Create a selection parameter
    //
    // Each option for this selection parameter is characterized by the following information:
    // - an option text
    // - an option value (which will be stored in the protocol)
    // - an option index (assigned automatically - starting with 0)
    //
    // If no option value is specified, the option index will be used as option value
    //
    // Methods to add an option to the selection parameter at position Pos:
    //   addOption        (Pos, Text, Value)
    //   addDefaultOption (Pos, Text, Value)
    // --------------------------------------------------------------------------------------------

    rTool.createSelectionParameter (WIPSelection, rSeqLim, "WIP selection");
    rTool.addOption                (WIPSelection, sOption1, 10);
    rTool.addDefaultOption         (WIPSelection, sOption2, 20);
    rTool.addOption                (WIPSelection, sOption3, 30);
    
    // example parameter to show how to treat a conflict between
    // a WIP parameter and a standard sequence parameter (TE)
    rTool.createLongParameter (WIPMaxTE, rSeqLim, "WIP max TE", "ms", 0, 100, 1, 100);

    // parameter to activate or activate online reconstruction
    rTool.createBoolParameter (WIPOnlineRecon, rSeqLim, "Online reconstruction", true);

    // create a long array parameter, where the limits are different for different array elements
    rTool.createLongArrayParameter (WIPLongArray, 16, 3, rSeqLim, "WIP long array", "mm", 0, 8, 1, 4);
    rTool.setLimits (WIPLongArray, 0, (int32_t) 0, 8L, 2L, 4L);
    rTool.setLimits (WIPLongArray, 1, (int32_t) 0, 8L, 1L, 3L);

    // create a double array parameter, where the limits are different for different array elements
    rTool.createDoubleArrayParameter (WIPDoubleArray, 10, 3, rSeqLim, "WIP double array", "mm", 2, 0, 1, 0.02, 0.2);
    rTool.setLimits (WIPDoubleArray, 1, 0.0, 2.0, 0.01, 0.5);
}


#ifdef WIN32

// --------------------------------------------------------------------------------------------
///  \brief    This solve handler acts on the parameter WIPLong.
///
///            The method tries to alter WIPDouble and WIPSelection to solve example
///            conflicts (a) and (b).
// --------------------------------------------------------------------------------------------
unsigned int solveHandlerWIPLong (LINK_LONG_TYPE* const pUILink, char**, const void*, const MrProtocolData::MrProtData*, long)
{
    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);

    if (pTool->getLongValue (WIPLong) > 50) {
        // solve WIP parameter conflict (a)
        if (pTool->getDoubleValue (WIPDouble) > 1) {
            pTool->setDoubleValue (WIPDouble, 1);
        }

        // solve WIP parameter conflict (b)
        if (pTool->getSelectedItem (WIPSelection).isText (sOption1)) {
            pTool->setSelectionText (WIPSelection, sOption2);
        }

        // return if prepare call is successful
        if (pUILink->sequence().prepareForBinarySearch (&pUILink->prot())) {
            return MRI_STD_CONFIRMATION_MSG;
        }
    }
    
    return 0;
}



// --------------------------------------------------------------------------------------------
///  \brief    This solve handler acts on the parameter WIPDouble.
///
///            The method tries to alter WIPLong to solve example conflict (a).
// --------------------------------------------------------------------------------------------
unsigned int solveHandlerWIPDouble (LINK_DOUBLE_TYPE* const pUILink, char**, const void*, const MrProtocolData::MrProtData*, long)
{
    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);

    // solve WIP parameter conflict (a)
    if (pTool->getDoubleValue (WIPDouble) > 1) {
        if (pTool->getLongValue (WIPLong) > 50) {
            pTool->setLongValue (WIPLong, 50);
        }

        // return if prepare call is successful
        if (pUILink->sequence().prepareForBinarySearch (&pUILink->prot())) {
            return MRI_STD_CONFIRMATION_MSG;
        }
    }
    
    return 0;
}



// --------------------------------------------------------------------------------------------
///  \brief    This solve handler acts on the parameter WIPSelection.
///
///            The method tries to alter WIPSelection to solve example conflict (b).
// --------------------------------------------------------------------------------------------
unsigned int solveHandlerWIPSelection (LINK_SELECTION_TYPE* const pUILink, char**, const void*, const MrProtocolData::MrProtData*, long)
{
    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);

    // solve WIP parameter conflict (b)
    if (pTool->getSelectedItem (WIPSelection).isText (sOption1)) {
        if (pTool->getLongValue (WIPLong) > 50) {
            pTool->setLongValue (WIPLong, 50);
        }

        // return if prepare call is successful
        if (pUILink->sequence().prepareForBinarySearch (&pUILink->prot())) {
            return MRI_STD_CONFIRMATION_MSG;
        }
    }
    
    return 0;
}



// --------------------------------------------------------------------------------------------
///  \brief    This solve handler acts on the parameter WIPMaxTE.
///
///            The method tries to alter the TE value to solve example conflict (c).
// --------------------------------------------------------------------------------------------
unsigned int solveHandlerWIPMaxTE (LINK_LONG_TYPE* const pUILink, char**, const void*, const MrProtocolData::MrProtData*, long)
{
    // get the UI element for TE
    LINK_DOUBLE_TYPE* pTE = _searchElm <LINK_DOUBLE_TYPE> (pUILink, MR_TAG_TE);

    if (pTE != NULL) {
        WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);

        // get TE and WIPMaxTE value
        double dTE = pTE->value (0);
        long lWIPMaxTEValue = pTool->getLongValue (WIPMaxTE);

        // solve WIP parameter conflict (c)
        if (dTE >= lWIPMaxTEValue) {

            // set the TE-value to the next possible lower value
            // (Up to 10 ms, the increment is 0.1 ms. Above that the increment is 1 ms.)
            if (lWIPMaxTEValue <= 10) {
                pTE->value (lWIPMaxTEValue - 0.1, 0);
            } else {
                pTE->value (lWIPMaxTEValue - 1, 0);
            }

            // return if prepare call is successful
            if (pUILink->sequence().prepareForBinarySearch (&pUILink->prot())) {
                return MRI_STD_CONFIRMATION_MSG;
            }
        }
    }
    
    return 0;
}



// --------------------------------------------------------------------------------------------
///  \brief    This pointer stores the original solve handler for TE. We need this solve
///            handler to deal with conflicts which were not introduced by our new WIP
///            parameters.
// --------------------------------------------------------------------------------------------
LINK_DOUBLE_TYPE::PFctSolve pOrigSolveHandlerTE = NULL;



// --------------------------------------------------------------------------------------------
///  \brief    This solve handler acts on the sequence parameter TE.
///
///            At first, the original solve handler is called. Subsequently, the sequence is
///            prepared with the modified protocol. In case the prepare is successful, the
///            method returns.
///
///            If the prepare still failed, the parameter WIPMaxtTE is altered to solve
///            conflict (c).
// --------------------------------------------------------------------------------------------
unsigned int solveHandlerTE (LINK_DOUBLE_TYPE* const pTE, char** arg_list, const void* pMem, const MrProtocolData::MrProtData* pOrigProt, long lPos)
{
    // call original solve handler
    if (pOrigSolveHandlerTE != NULL) {
        unsigned int iResult = pOrigSolveHandlerTE (pTE, arg_list, pMem, pOrigProt, lPos);
        
        if (iResult != 0) {
            // return if prepare call is successful
            if (pTE->sequence().prepareForBinarySearch (&pTE->prot())) {
                return iResult;
            }
        }
    }
    
    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pTE);

    // get TE and WIPMaxTE value
    double dTE = pTE->value (0);
    long lWIPMaxTEValue = pTool->getLongValue (WIPMaxTE);

    // solve conflict (c)
    if (dTE >= lWIPMaxTEValue) {
        pTool->setLongValue (WIPMaxTE, long (dTE) + 1);

        // return if prepare call is successful
        if (pTE->sequence().prepareForBinarySearch (&pTE->prot())) {
            return MRI_STD_CONFIRMATION_MSG;
        }
    }

    // conflict could not be solved
    return 0;
}



// --------------------------------------------------------------------------------------------
///  \brief    This tool tip handler creates a tool tip text for the parameter WIPBool.
///
///            The text contains some information about the current measurement.
// --------------------------------------------------------------------------------------------
unsigned int toolTipHandlerBool (LINK_BOOL_TYPE* const pUILink, char* arg_list[], long)
{
    MrProt rMrProt (pUILink->prot());

    // prepare sequence (required to get sequence exports)
    if (!pUILink->sequence().prepareForBinarySearch (rMrProt)) {
        THROW_MR_MSG (KSpace::KSpaceException, "Could not prepare sequence in customized ToolTipHandler!");
    }

    // get sequence exports
    const SeqExpo &rSeqExpo = pUILink->sequence().getSEQ_BUFF()->getSeqExpo();
    
    // build tool tip text
    std::ostringstream TmpStream;
    TmpStream << "Customized tool tip"                                               << std::endl;
    TmpStream                                                                        << std::endl;
    TmpStream << "Lines to be measured: "      << rSeqExpo.getMeasuredPELines()      << std::endl;
    TmpStream << "Echo line: "                 << rMrProt.kSpace().echoLine()        << std::endl;
    TmpStream                                                                        << std::endl;
    TmpStream << "Partitions to be measured: " << rSeqExpo.getMeasured3dPartitions() << std::endl;
    TmpStream << "Echo partition: "            << rMrProt.kSpace().echoPartition()   << std::endl;

    // copy text into static variable sToolTip -> will still exist after leaving this method
    static char sToolTip [2048];
    strcpy (sToolTip, TmpStream.str().c_str());
    
    // fill arg_list
    arg_list [0] = sToolTip;

    // tell UI to display the text in arg_list [0];
    return MRI_STD_STRING;
}



bool isAvailableHandlerLongArray (LINK_LONG_TYPE* const pUILink, long)
{
    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);

    return pTool->getBoolValue (WIPBool);
}



void FlashUI::registerUIHandlers (SeqLim &rSeqLim, WIPParameterTool& rTool)
{
    // Register solve handlers for WIP parameter conflicts (a) and (b)
    rTool.registerSolveHandler (WIPLong,      solveHandlerWIPLong);
    rTool.registerSolveHandler (WIPDouble,    solveHandlerWIPDouble);
    rTool.registerSolveHandler (WIPSelection, solveHandlerWIPSelection);

    // Register solve handlers for WIP parameter conflict (c)
    rTool.registerSolveHandler (WIPMaxTE, solveHandlerWIPMaxTE);
    LINK_DOUBLE_TYPE* pTE = _searchElm <LINK_DOUBLE_TYPE> (&rSeqLim, MR_TAG_TE);
    if (pTE != NULL) {
        pOrigSolveHandlerTE = pTE->registerSolveHandler (solveHandlerTE);
    }

    // Register customized tool tip handler
    rTool.registerToolTipHandler (WIPBool, toolTipHandlerBool);

    // Register isAvailable handler
    rTool.registerIsAvailableHandler (WIPLongArray, isAvailableHandlerLongArray);
}

#endif


FlashUI::FlashUI (void)
{
}



FlashUI::~FlashUI (void)
{
}



NLS_STATUS FlashUI::registerUI (SeqLim &rSeqLim, WIPParameterTool& rTool)
{
    createWIPParameters (rSeqLim, rTool);
    
#ifdef WIN32
    registerUIHandlers (rSeqLim, rTool);
#endif

    return SEQU__NORMAL;
}


// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
