// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FLASH_UI.h
//   Author: H IM MR PLM AW T-AFW
//
//     Lang: C++
//
//  Descrip: Header file for FLASH Sequence UI
//
//  Remarks: This is the demo sequence FLASH.
//           DO NOT base any diagnosis on images generated by this sequence!
//
// --------------------------------------------------------------------------------------------

#ifndef FLASH_UI_h
#define FLASH_UI_h 1

#include "MrServers/MrImaging/seq/FLASH/FLASH.h"

#include "MrServers/MrImaging/libSBB/StdSeqIF.h"
#include "MrServers/MrProtSrv/MrProt/SeqBuff.h"
#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"

#include "MrServers/MrProtSrv/MrProt/KSpace/MrKSpace.h"

namespace SEQ_NAMESPACE {

    // --------------------------------------------------------------------------------------------
    ///  \brief    This class contains methods and objects dedicated to the UI handling for the
    ///            sequence FLASH.
    ///
    ///            The sequence uses the WIPParameterTool to create 6 WIP parameters. A few example
    ///            handlers show how the UI behavior can be altered.
    ///
    ///            The use of solve handlers is demonstrated on the basis of a few simple example
    ///            conflicts. The following conditions are forbidden:
    ///
    ///            - (a) : (WIPLong > 50) AND (WIPDouble > 1.0)
    ///            - (b) : (WIPSelection == sOption1) AND (WIPLong > 50)
    ///            - (c) : (WIPMaxTE < TE)
    // --------------------------------------------------------------------------------------------
    class FlashUI
    {
    public:

        FlashUI (void);


        virtual ~FlashUI (void);


        // --------------------------------------------------------------------------------------------
        ///  \brief    This method creates WIP parameters and registers customized handler methods.
        ///
        ///  \see      FlashUI::createWIPParameters
        ///  \see      FlashUI::registerUIHandlers
        ///
        ///  \param    rSeqLim  Reference to the sequence limits object
        ///  \param    rTool    Reference to the WIPParameterTool associated to the sequence
        ///
        ///  \return   SEQU_NORMAL
        // --------------------------------------------------------------------------------------------
        virtual NLS_STATUS registerUI (SeqLim &rSeqLim, WPT_NAMESPACE::WIPParameterTool& rTool);
        
        
    protected:

        // --------------------------------------------------------------------------------------------
        ///  \brief    This method creates 6 WIP parameters.
        ///
        ///            - WIPLong
        ///            - WIPDouble
        ///            - WIPBool        (checkbox)
        ///            - WIPSelection   (drop down list)
        ///            - WIPMaxTE       (show how to solve conflicts with standard sequence parameters)
        ///            - WIPOnlineRecon (checkbox to activate online reconstruction)
        ///
        ///  \param    rSeqLim  Reference to the sequence limits object
        ///  \param    rTool    Reference to the WIPParameterTool associated to the sequence
        // --------------------------------------------------------------------------------------------
        virtual void createWIPParameters (SeqLim &rSeqLim, WPT_NAMESPACE::WIPParameterTool& rTool);
        
        
#ifdef WIN32
        // --------------------------------------------------------------------------------------------
        ///  \brief    This method registers customized handler methods that take care of the
        ///            example conflicts. Furthermore, a tool tip handler is registered to WIPBool.
        ///
        ///  \param    rTool  Reference to the WIPParameterTool associated to the sequence
        // --------------------------------------------------------------------------------------------
        virtual void registerUIHandlers (SeqLim &rSeqLim, WPT_NAMESPACE::WIPParameterTool& rTool);
#endif


    private:
        // copy constructor and assignment operator should not be accessible
        FlashUI (const FlashUI &rRight);
        FlashUI& operator= (const FlashUI &rRight);
    };
}


#endif

// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
