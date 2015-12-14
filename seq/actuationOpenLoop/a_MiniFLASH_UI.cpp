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
#include "MrServers/MrImaging/seq/actuationOpenLoop/a_MiniFLASH.h"
#include "MrServers/MrImaging/seq/actuationOpenLoop/a_MiniFLASH_UI.h"

#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif

using namespace SEQ_NAMESPACE;
using namespace WPT_NAMESPACE;

	void MiniFlashUI::createWIPParameters (SeqLim &rSeqLim, WIPParameterTool& rTool)
	{
		rTool.createSelectionParameter (WIPSelectionGradDir1, rSeqLim, "Grad1 direction");
		rTool.addDefaultOption         (WIPSelectionGradDir1, "x",  X_DIR);
		rTool.addOption                (WIPSelectionGradDir1, "z",  Z_DIR);
		rTool.createDoubleParameter(WIPDoubleGradAmpl1, rSeqLim, "grad1 amplitude", "(mT/m)", 2, -42, 42.99, 0.01, 1);
		rTool.createDoubleParameter(WIPDoubleGradDur1, rSeqLim, "grad1 duration", "(ms)", 2, 0, 60000, 0.01, 1);

		rTool.createSelectionParameter (WIPSelectionGradDir2, rSeqLim, "Grad2 direction");
		rTool.addDefaultOption         (WIPSelectionGradDir2, "x",  X_DIR);
		rTool.addOption                (WIPSelectionGradDir2, "z",  Z_DIR);
		rTool.createDoubleParameter(WIPDoubleGradAmpl2, rSeqLim, "grad2 amplitude", "(mT/m)", 2, -42, 42.99, 0.01, 1);
		rTool.createDoubleParameter(WIPDoubleGradDur2, rSeqLim, "grad2 duration", "(ms)", 2, 0, 60000, 0.01, 1);
		
		rTool.createSelectionParameter (WIPSelectionGradDir3, rSeqLim, "Grad3 direction");
		rTool.addDefaultOption         (WIPSelectionGradDir3, "x",  X_DIR);
		rTool.addOption                (WIPSelectionGradDir3, "z",  Z_DIR);
		rTool.createDoubleParameter(WIPDoubleGradAmpl3, rSeqLim, "grad3 amplitude", "(mT/m)", 2, -42, 42.99, 0.01, 1);
		rTool.createDoubleParameter(WIPDoubleGradDur3, rSeqLim, "grad3 duration", "(ms)", 2, 0, 60000, 0.01, 1);

		rTool.createSelectionParameter (WIPSelectionGradDir4, rSeqLim, "Grad4 direction");
		rTool.addDefaultOption         (WIPSelectionGradDir4, "x",  X_DIR);
		rTool.addOption                (WIPSelectionGradDir4, "z",  Z_DIR);
		rTool.createDoubleParameter(WIPDoubleGradAmpl4, rSeqLim, "grad4 amplitude", "(mT/m)", 2, -42, 42.99, 0.01, 1);
		rTool.createDoubleParameter(WIPDoubleGradDur4, rSeqLim, "grad4 duration", "(ms)", 2, 0, 60000, 0.01, 1);
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










