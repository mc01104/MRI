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
#include "MrServers/MrImaging/seq/capsuleSinusoid/a_MiniFLASH.h"
#include "MrServers/MrImaging/seq/capsuleSinusoid/a_MiniFLASH_UI.h"

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

		rTool.createDoubleParameter(WIPDoubleAmplitude, rSeqLim, "G", "(mT/m)", 8, -42, 42.99, .1, 10);
		rTool.createDoubleParameter(WIPDoublePeriod, rSeqLim, "T(D)", "(s or (a.u.))", 5, -150, 150, .0005, .5);
		rTool.createDoubleParameter(WIPDoubleFreqOffset, rSeqLim, "Frequency", "(Hz)", 0, -30000, 30000, 100, 2000);
		
		rTool.createLongParameter  (WIPLongRepetition, rSeqLim, "Repetition", NULL, 1, 5000, 1, 10);

		rTool.createLongParameter  (WIPLongChannelID, rSeqLim, "Not used", NULL, -150, 150, 1, 10);

		rTool.createSelectionParameter (WIPSelectionType, rSeqLim, "Actuation mode", "init");
		rTool.addDefaultOption         (WIPSelectionType, sOptionType1,  SIMULTANEOUS_CL);
		rTool.addOption                (WIPSelectionType, sOptionType2,  ALTERNATE_CL);
		rTool.addOption                (WIPSelectionType, sOptionType3,  INITIALIZE);
		rTool.addOption                (WIPSelectionType, sOptionType4,  ACTIVE_MARKER);
		rTool.addOption                (WIPSelectionType, sOptionType5,  MULTI_CAPSULES);
		
		rTool.createSelectionParameter (WIPSelectionSeq, rSeqLim, "tracking sequence");
		rTool.addDefaultOption         (WIPSelectionSeq, sOptionSeq2, GRADIENT_ECHO);
		rTool.addOption                (WIPSelectionSeq, sOptionSeq1, SPIN_ECHO);
		rTool.addOption                (WIPSelectionSeq, sOptionSeq3, NO_TRACKING);

		rTool.createSelectionParameter (WIPSelectionPlane, rSeqLim, "Actuation plane");
		rTool.addDefaultOption         (WIPSelectionPlane, sOptionPlane2, X_Z);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane1, X_Y);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane3, Y_Z);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane4, ARB1);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane5, ARB2);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane6, X_Z_PRIME);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane7, X_X);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane8, Y_Y);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane9, Z_Z);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane10, X_Y_PRIME);
		rTool.addOption                (WIPSelectionPlane, sOptionPlane11, Y_Z_PRIME);

		rTool.createDoubleParameter(WIPDoubleK1, rSeqLim, "Cos Freq", "Hz", 8, -10000., 10000., .1, 2);
		rTool.createDoubleParameter(WIPDoubleK2, rSeqLim, "Not used", " ", 2, 0, 20000, 1, 2);

		rTool.createDoubleParameter(WIPDoubleThetaF1, rSeqLim, "Not used", " ", 2, -150, 150, 0.01, 1);
		rTool.createDoubleParameter(WIPDoubleRTDelay, rSeqLim, "Not used", " ", 0, 0, 65000, 1, 1);
		rTool.createDoubleParameter(WIPDoubleDurationEXT, rSeqLim, "Not used", " ", 3, 0, 2, 0.001, 0.1);
	
		rTool.createDoubleParameter(WIPDoubleWakeUpDelay, rSeqLim, "Not used", " ", 0, 0, 65000, 1, 1);

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










