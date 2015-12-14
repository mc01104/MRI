// --------------------------------------------------------------------------
// General Includes
// --------------------------------------------------------------------------
#include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#ifdef WIN32
    #include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"
	#include <sstream>
#endif

//  --------------------------------------------------------------------------
//  Application includes
//  --------------------------------------------------------------------------
#include "MrServers/MrImaging/seq/TwoCapsulesOpenLoopModifiedG/a_MiniFLASH.h"
#include "MrServers/MrImaging/seq/TwoCapsulesOpenLoopModifiedG/a_MiniFLASH_UI.h"

#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif

using namespace SEQ_NAMESPACE;
using namespace WPT_NAMESPACE;



void MiniFlashUI::createWIPParameters (SeqLim &rSeqLim, WIPParameterTool& rTool)
{
	// --------------------------------------------------------------------------------------------
	// Create the following WIP parameters:
	//
	// - 3 WIPDouble for the gradient amplitude
	// - 3 WIPDouble for the gradient duration
	// --------------------------------------------------------------------------------------------

	rTool.createDoubleParameter(WIPDblePosAmplitude, rSeqLim, "Pos. amp.", "(mT/m)", 2, -42., 42.99, 0.1, 10);

	rTool.createDoubleParameter(WIPDblePulseWidth1, rSeqLim, "Delta T1", "(ms)", 1, 0, 10000, .1, 10);
	rTool.createDoubleParameter(WIPDblePulseWidth2, rSeqLim, "Delta T2", "(ms)", 1, 0, 10000, .1, 10);
	rTool.createDoubleParameter(WIPDblePulseWidth3, rSeqLim, "Delta T3", "(ms)", 1, 0, 10000, .1, 10);

	rTool.createDoubleParameter(WIPDbleTOFF, rSeqLim, "Toff", "(ms)", 1, 0, 10000, .1, 10);

	rTool.createLongParameter  (WIPLongN1, rSeqLim, "Repetition", NULL, -5000, 5000, 1, 10);
	rTool.createLongParameter  (WIPLongN2, rSeqLim, "Repetition", NULL, -5000, 5000, 1, 10);
	rTool.createLongParameter  (WIPLongN3, rSeqLim, "Repetition", NULL, -5000, 5000, 1, 10);

	rTool.createSelectionParameter (WIPSelectionPlane, rSeqLim, "motion direction");
	rTool.addOption                (WIPSelectionPlane, sOptionDirection1, X_X);
	rTool.addDefaultOption         (WIPSelectionPlane, sOptionDirection2, Y_Y);
	rTool.addOption                (WIPSelectionPlane, sOptionDirection3, Z_Z);
}

#ifdef WIN32


    //  ----------------------------------------------------------------------
    //
    //  Name        :  getSeq
    //
    //  Description :
    /// \brief         Returns the pointer to the sequence MiniFlash
    //
    //  Return      :  MiniFlash*
    //
    //  ----------------------------------------------------------------------
    MiniFlash* getSeq (MrUILinkBase* const pThis)
    {
        return ( static_cast<MiniFlash*>(pThis->sequence().getSeq()) );
    }




    //  ----------------------------------------------------------------------
    //
    //  Name        :  getUI
    //
    //  Description :
    /// \brief         Returns the pointer to the UI class of the MiniFlash
    ///                 sequence
    //
    //  Return      :  MiniFlashUI*
    //
    //  ----------------------------------------------------------------------
    const MiniFlashUI* getUI (MrUILinkBase* const pThis)
    {
        return ( static_cast<MiniFlash*>(pThis->sequence().getSeq())->getUI() );
    }










    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    //
    //                    Definition of set value handler
    //
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------


    //  --------------------------------------------------------------------------
    //
    //  Name        : fLSetContrast
    //
    //  Description :
    /// \brief        Example of a set-value handler calling the original set-value
    ///                handler first
    ///
    ///               This handler is called, when the contrast UI element is modified.
    ///               In that case, it checks and modifies the following parameters:
    ///               - Bandwidth
    ///               - Whatever
    ///
    ///               The original handler is called, if rContrast.getOrigSetValueHandler() is set.
    ///
    ///               It returns the desired value.
    ///
    //  Return      : long
    //
    //  --------------------------------------------------------------------------





    //  --------------------------------------------------------------------------
    //  --------------------------------------------------------------------------
    //
    //                         Definition of solve handler
    //
    //  --------------------------------------------------------------------------
    //  --------------------------------------------------------------------------


    //  --------------------------------------------------------------------------
    //
    //  Name        : fSSolveGradMode
    //
    //  Description :
    /// \brief        Example of a solve handler calling the standard fUICSolveSelectionConflict
    ///
    ///               This handler is called, when the gradient mode UI element is modified, but
    ///               leads to an inconsistent protocol.
    ///               In that case, modifies the following parameters:
    ///               - TE
    ///               - TR
    ///
    ///               This is done by calling fUICSolveSelectionConflict
    ///
    ///               It returns the confirmation message.
    ///
    //  Return      : long
    //
    //  --------------------------------------------------------------------------

#endif
    ///  \brief Constructor
    ///
    MiniFlashUI::MiniFlashUI(){}


    ///  \brief Destructor
    ///
	MiniFlashUI::~MiniFlashUI(){}


    //  --------------------------------------------------------------------------
    //
    //  Name        : registerUI
    //
    //  Description :
    /// \brief        This method registers all given set / get / Solve - handlers
    ///
    ///               It can be executed on the measuement system, too, but is empty there.
    ///
    ///
    ///               It returns an NLS status
    ///
    //  Return      : long
    //
    //  --------------------------------------------------------------------------

    NLS_STATUS MiniFlashUI::registerUI (SeqLim &rSeqLim, WPT_NAMESPACE::WIPParameterTool& rTool)
	//NLS_STATUS MiniFlashUI::registerUI (SeqLim &rSeqLim)
    {

        static const char * const ptModule = {"MiniFlashUI::registerUI"};

		createWIPParameters (rSeqLim, rTool);

        #ifdef WIN32

            // ----------------------------------------------------------------------------------
            // Registration of set handler methods
            //
            //                                           parameter tag     new handler function
            // ----------------------------------------------------------------------------------



            // ----------------------------------------------------------------------------------
            // Registration of solve handler methods
            //
            //                                        parameter tag         new handler function
            // ----------------------------------------------------------------------------------



        #endif

		

        return ( SEQU__NORMAL );

    };










