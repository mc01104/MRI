//  -----------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//  -----------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\PACE_UI.h
//     Version: \main\3
//      Author: stemal8q
//        Date: 2011-03-31 11:47:59 +02:00
//
//        Lang: C++
//
//     Descrip: Special parameters for development purposes.
//
//     Classes:
//
//  -----------------------------------------------------------------

#ifndef _PACE_UI_h
#define _PACE_UI_h

//  Definition of NLS_STATUS
#ifdef _VB_LINE
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"
#else
#include "MrCommon/GlobalDefinitions/MrResult.h"
#endif

//  Definition of 
//#include "ProtBasic/Interfaces/NavigatorDefines.h"


//  -----------------------------------------------------------------
//  Import - Export - Control
//  -----------------------------------------------------------------
#ifdef __IMP_EXP
#undef __IMP_EXP
#endif 

#ifdef LOCAL_PACE_BUILD
#define __IMP_EXP
#pragma message( "Local PACE build" )
#else  //   LOCAL_PACE_BUILD not defined
#ifdef BUILD_libPACE
#define __OWNER
#endif
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h" // import/export control
#endif  //  LOCAL_PACE_BUILD

class SeqLim;
#ifdef _VB_LINE
#ifdef WIN32
struct HKEY__;
#endif
#endif

namespace PACE
{
    //  Standard initialization of the PACE related SeqLim parameters.
    //  The parameters iDef, iMode1, ..., iMode13 receive the modi
    //  supported by the sequence.
    //  The mode passed to the iDef argument defines the deafult.
    //  e.g. fInit(&rSeqLim,SEQ::RESP_COMP_OFF,SEQ::RESP_COMP_BREATH_HOLD,SEQ::RESP_COMP_TRIGGER);
    //  It is strongly recommended that the function is invoked after the other SeqLim parameters have been initialized.
    __IMP_EXP NLS_STATUS fInitUI(SeqLim* pSeqLim);

#ifdef _VB_LINE
#ifdef WIN32
    
    unsigned long _readDwordFromRegistry(struct HKEY__ * hKeyRoot, const unsigned short* const lpszPath, const unsigned short* const lpszEntry, unsigned long nDefault);
#endif // WIN32
#endif // _VB_LINE
} //  namespace PACE

#ifdef __IMP_EXP_DATA
#undef __IMP_EXP_DATA
#endif

#ifdef __IMP_EXP
#undef __IMP_EXP
#endif

#endif // _PACE_UI_h
