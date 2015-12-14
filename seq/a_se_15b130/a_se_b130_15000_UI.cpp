// --------------------------------------------------------------------------
// General Includes
// --------------------------------------------------------------------------
#include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#ifdef WIN32
  #include  "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"       // name tags of UI-resources
//  #include  <vector>
#endif


//  -------------------------------------------------------------------------- 
//  Application includes                                                       
//  -------------------------------------------------------------------------- 
#include "MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000.h"
#include "MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000_UI.h"

#ifndef SEQ_NAMESPACE
    #error SEQ_NAMESPACE not defined
#endif

using namespace SEQ_NAMESPACE;

#ifdef WIN32
namespace SEQ_NAMESPACE
{


namespace Se15b130UINS {
    
    
    //  ---------------------------------------------------------------------- 
    //                                                                         
    //  Name        :  getSeq                                                  
    //                                                                         
    //  Description :  
    /// \brief         Returns the pointer to the sequence Se15b130               
    //                                                                         
    //  Return      :  Se15b130*                                                  
    //                                                                         
    //  ---------------------------------------------------------------------- 
    Se15b130* getSeq (MrUILinkBase* const pThis)
    {
        return ( static_cast<Se15b130*>(pThis->sequence().getSeq()) );
    }
    
    
    
    
    //  ---------------------------------------------------------------------- 
    //                                                                         
    //  Name        :  getUI                                                   
    //                                                                         
    //  Description :  
    /// \brief         Returns the pointer to the UI class of the Se15b130        
    ///                 sequence                                                
    //                                                                         
    //  Return      :  Se15b130UI*                                                
    //                                                                         
    //  ---------------------------------------------------------------------- 
    const Se15b130UI* getUI (MrUILinkBase* const pThis)
    {
        return ( static_cast<Se15b130*>(pThis->sequence().getSeq())->getUI() );
    }



    //  -------------------------------------------------------------------------- 
    //  -------------------------------------------------------------------------- 
    //                                                                             
    //                         Definition of other functions                         
    //                                                                             
    //  -------------------------------------------------------------------------- 
    //  -------------------------------------------------------------------------- 
    
    /*[ Function ****************************************************************\
    *
    * Name        : pfGetTRTIFillTimes
    *               
    * Description : Serve as a delegation of the function calculateTRTIFillTimes
    *               
    \****************************************************************************/   
    bool pfGetTRTIFillTimes
    (
     MrProtocolData::MrProtData*  pMrProt,
     SeqLim*  pSeqLim,
     MrProtocolData::SeqExpo* pSeqExpo,
     Sequence*pSeq,
     long*    plNeededTI,
     long*    plNeededTR
    )
    { 
        MrProt sMrProt( pMrProt );
        Se15b130 *pSe15b130 = static_cast<Se15b130*>(pSeq->getSeq());
        return pSe15b130->calculateTRTIFillTimes (sMrProt, *pSeqLim, *pSeqExpo, plNeededTI, plNeededTR);
    }




    
    
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    //                                                                           
    //                    Definition of set value handler                        
    //                                                                           
    // --------------------------------------------------------------------------
    // --------------------------------------------------------------------------
    
    
    
    //  -------------------------------------------------------------------------- 
    //  -------------------------------------------------------------------------- 
    //                                                                             
    //                         Definition of solve handler                         
    //                                                                             
    //  -------------------------------------------------------------------------- 
    //  -------------------------------------------------------------------------- 
    
    /*[ Function ****************************************************************\
    *
    * Name        : fSSolveInversionConflict
    *               
    * Description : Solve handler for inversion pulse
    *               
    \****************************************************************************/
    unsigned fSSolveInversion
        (
        LINK_SELECTION_TYPE* const pThis,           // ...
        char**                      arg_list,       // receives confirmation message
        const void*,                                // not needed
        const MrProtocolData::MrProtData*               pOrigProt,      // Original protocol with old mode
        long                                        // Array index reserved
        )
    {
        return ( fUICSolveSelectionConflict ( pThis, arg_list, NULL, pOrigProt, 0, NULL, NULL, NULL ) );  
    }
    
    
    /*[ Function ****************************************************************\
    *
    * Name        : fSSolveFatSuppresConflict
    *               
    * Description : Solve handler for fat suppression pulse
    *               
    \****************************************************************************/
    unsigned fSSolveFatSuppress
        (
        LINK_SELECTION_TYPE* const pThis,           // ...
        char**                      arg_list,       // receives confirmation message
        const void*,                                // not needed
        const MrProtocolData::MrProtData*               pOrigProt,      // Original protocol with old mode
        long                                        // Array index reserved
        )
    {
        return ( fUICSolveSelectionConflict ( pThis, arg_list, NULL, pOrigProt, 0, NULL, NULL, NULL ) );  
    }
    
    
    /*[ Function ****************************************************************\
    *
    * Name        : fSSolveWaterSuppressionConflict
    *               
    * Description : Solve handler for fat suppression pulse
    *               
    \****************************************************************************/
    unsigned fSSolveWaterSuppression
        (
        LINK_SELECTION_TYPE* const pThis,           // ...
        char**                      arg_list,       // receives confirmation message
        const void*,                                // not needed
        const MrProtocolData::MrProtData*               pOrigProt,      // Original protocol with old mode
        long                                        // Array index reserved
        )
    {
        return ( fUICSolveSelectionConflict ( pThis, arg_list, NULL, pOrigProt, 0, NULL, NULL, NULL ) );  
    }
    
    
    
    /*[ Function ****************************************************************\
    *
    * Name        : fTIGetLimits
    *               
    * Description : calculates TI limits
    *               
    * Return      : true , if limits have been calculated successfully
    *               false, if parameter TI is read-only
    *
    \****************************************************************************/
    
    
    bool fTIGetLimits (LINK_DOUBLE_TYPE * const pThis, std::vector< MrLimitDouble >& rLimitVector, unsigned long& rulVerify, long lIndex)
    {
        const UI_ELEMENT_DOUBLE&  rTI = getUI(pThis)->m_TI;
        
        return fUICGetTILimits(pThis,rLimitVector,rulVerify,lIndex,rTI.getOrigGetLimitsHandler(),&pfGetTRTIFillTimes);
    }
    
    /*[ Function ****************************************************************\
    *
    * Name        : fSolveTITRConflict
    *               
    * Description : Solves conflict TI <-> TR
    *
    *               Note:
    *               TI limits should have been calculated using fUICGetTILimits! 
    *               
    * Return      : 0, if conflict can't be solved, else MRI_STD_CONFIRMATION_MSG 
    *
    \****************************************************************************/
    unsigned int fSolveTITR(MrUILinkLimited<double>* const pThis, char *arg_list[], const void *pVoid, const MrProtocolData::MrProtData *pOrigProt, long lIndex)
    {
        return fUICSolveTITRConflict(pThis, arg_list, pVoid, pOrigProt, lIndex, &pfGetTRTIFillTimes);
    }
    

    
} // end of namespace Se15b130UINS
} // end of namespace SEQ_NAMESPACE
#endif

///  \brief Constructor
///
Se15b130UI::Se15b130UI(){}


///  \brief Destructor
///
Se15b130UI::~Se15b130UI(){}




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

NLS_STATUS Se15b130UI::registerUI (SeqLim &rSeqLim)
{
    
    static const char * const ptModule = {"Se15b130UI::registerUI"};
    
    // Register some solve handlers
    fStdImagingInitPost (rSeqLim) ;
    
#ifdef WIN32  
    // ----------------------------------------------------------------------------------
    // Registration of solve handler methods                                             
    //                                                                                   
    //                                        parameter tag         new handler function 
    // ----------------------------------------------------------------------------------
    m_FatSuppress.registerSolveHandler (rSeqLim, MR_TAG_FAT_SUPPRESSION, Se15b130UINS::fSSolveFatSuppress);
    
    // ----------------------------------------------------------------------------------
    // Registration of solve handler methods                                             
    //                                                                                   
    //                                        parameter tag         new handler function 
    // ----------------------------------------------------------------------------------
    m_WaterSuppress.registerSolveHandler (rSeqLim, MR_TAG_WATER_SUPPRESSION, Se15b130UINS::fSSolveWaterSuppression);
    
    // ----------------------------------------------------------------------------------
    // Registration of solve handler methods                                             
    //                                                                                   
    //                                        parameter tag         new handler function 
    // ----------------------------------------------------------------------------------
    m_Inversion.registerSolveHandler (rSeqLim, MR_TAG_INVERSION, Se15b130UINS::fSSolveInversion);
    
    // * ---------------------------------------------------------------------- *
    // * Registration of TI GetLimits and Solve handler methods                 *
    // * ---------------------------------------------------------------------- *
    char tMrTagTI[64];
    sprintf(tMrTagTI,"%s.0\0",MR_TAG_TI);
    m_TI.registerGetLimitsHandler (rSeqLim, tMrTagTI, Se15b130UINS::fTIGetLimits);
    m_TI.registerSolveHandler (rSeqLim, tMrTagTI, Se15b130UINS::fSolveTITR);
#endif
    
    return ( SEQU__NORMAL );
    
}




