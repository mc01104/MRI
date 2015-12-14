//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\a_se_15b130\a_se_b130_15000_UI.h
//	 Version: \main\4
//	  Author: Michael Zwanger, SIEMENS AG, Medical Engineering, MRIA, +9131/84-2672
//	    Date: 2011-09-07 11:37:36 +02:00
//
//	    Lang: C++
//            
//
//
///  \file   a_se_b130_15000_UI.h
///  \brief  File containing declaraion of the UI class 
///          - Se15b130UI
///          
///  This file contains the implementation of the class Se15b130UI.
///  The sequence Se15b130UI use it to generate very nice images.
///  
//	-----------------------------------------------------------------------------



#ifndef a_se_b130_15000_UI_h
#define a_se_b130_15000_UI_h 1



//  -------------------------------------------------------------------------- *
//  Application includes                                                       *
//  -------------------------------------------------------------------------- *
#include "MrServers/MrImaging/seq/a_se_15b130/a_se_b130_15000.h"
#include "MrServers/MrProtSrv/MrProtocol/libUICtrl/UICtrl.h"


#ifdef WIN32
#include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkLimited.h"
#include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkSelection.h"
#endif


//  -------------------------------------------------------------------------- *
//  Forward declarations                                                       *
//  -------------------------------------------------------------------------- *
namespace MrProtocolData
{
    class SeqExpo;
}
class SeqLim;
class Sequence;



namespace SEQ_NAMESPACE
{


//  -------------------------------------------------------------------------- 
//                                                                             
//  Name        : Se15b130UI                                                
//                                                                             
//  Description : 
/// \brief        This class basically is a storage for the pointers to the 
///                original setValue / getValue / solve - handlers.
///                                                                             
///               The sequence registers new UI handlers, which usually do
///                something, then call the original UI handler, and then
///                do something else. To keep the information of the original
///                UI handlers, the Se15b130UI class stores the pointers
///               
///               It also provides the method registerUI to execute the
///                registration of all new handlers (and the storage of
///                 the original pointers)
///               
//  -------------------------------------------------------------------------- 

class Se15b130UI {
    
public:
    
    //  -------------------------------------------------------------- 
    //                                                                 
    //  Name        :  Se15b130UI::Se15b130UI                                     
    //                                                                 
    //  Description :  
    /// \brief         Initialization of class members                 
    //                                                                 
    //  Return      :                                                  
    //                                                                 
    //  -------------------------------------------------------------- 
    Se15b130UI();
    
    
    //  -------------------------------------------------------------- 
    //                                                                 
    //  Name        :  Se15b130UI::~Se15b130UI                                    
    //                                                                 
    //  Description :  
    /// \brief         Destructor                                      
    //                                                                 
    //  Return      :                                                  
    //                                                                 
    //  -------------------------------------------------------------- 
    virtual ~Se15b130UI();
    
    
    
    //  -------------------------------------------------------------------------- 
    //                                                                             
    //  Name        : Se15b130UI::registerUI                                                
    //                                                                             
    //  Description : 
    /// \brief        This function initializes the UI functions and
    ///                registers all given set / get / Solve - handlers                                                
    ///
    ///               It can be executed on the measuement system, too, but is empty there.
    ///               
    ///               On the host, it executes these steps                                                              
    ///               - Declaration of pointers to UI classes          
    ///               - Registration of overloaded set value handlers  
    ///
    ///               It returns an NLS status
    ///
    NLS_STATUS registerUI ( SeqLim &rSeqLim );
    
#ifdef WIN32
    
    //  -------------------------------------------------------------- 
    ///  \brief Helper class instances for UI handlers                         
    ///         - register new handler functions                            
    ///         - save pointer to original handler function  
    ///         These classes exit only on the host.               
    ///                          
    //  -------------------------------------------------------------- 
    UI_ELEMENT_SELECTION  m_FatSuppress;
    UI_ELEMENT_SELECTION m_WaterSuppress;
    UI_ELEMENT_SELECTION m_Inversion;
    UI_ELEMENT_DOUBLE m_TI;
    
#endif
    
    
    
};

}

#endif

