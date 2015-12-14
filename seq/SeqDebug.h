//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\SeqDebug.h
//       Authors: Michael Zwanger, SIEMENS AG, UB Med, MRIA, +9131/84-2672
//	    Date: n.a. 
//
//	    Lang: C++
//
//	 Descrip: Definitions and macros for Sequence tracing
//
//	---------------------------------------------------------

#ifndef SeqDebug_h
#define SeqDebug_h 1

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include "MrCommon/MrNFramework/MrTrace/MPCUTrace/MPCUTrace.h"           // for TRACE_PUT macros
#include <stdlib.h>


//-----------------------------------------------------------------------------
//  Set the name of the SeqLim pointer to default value if not set by sequence
//-----------------------------------------------------------------------------

#ifndef SEQDEBUG_SEQLIM
#define SEQDEBUG_SEQLIM pSeqLim
#endif

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

// Severity codes
#define DEBUG_CALL          0x00000001
#define DEBUG_RETURN        0x00000002
#define DEBUG_INPUT         0x00000004
#define DEBUG_RESULT        0x00000008
#define DEBUG_LANDMARK      0x00000010
#define DEBUG_INTERNAL      0x00000020
#define DEBUG_6             0x00000040
#define DEBUG_7             0x00000080

// Origin codes (for use inside the sequence)
#define DEBUG_INIT          0x00000100
#define DEBUG_PREP          0x00000200
#define DEBUG_CHECK         0x00000400
#define DEBUG_RECEIVE       0x00000800
#define DEBUG_DELIVER       0x00001000
#define DEBUG_RUN           0x00002000
#define DEBUG_KERNEL        0x00004000
#define DEBUG_LIBSEQUT      0x00008000

// Origin codes (for libSBB and libSeqUtil)
#define DEBUG_SBBKernel     0x00010000	// Kernel SBBs
#define DEBUG_SBBKernelRT   0x00020000  // Real Time Part of the Kernel SBBs
#define DEBUG_SBBRF         0x00040000	// SBBs using RF: Sats, IR, ...
#define DEBUG_SBB           0x00080000		
#define DEBUG_SEQLOOP       0x00100000	// SeqLoop class methods
#define DEBUG_21            0x00200000	
#define DEBUG_22            0x00400000
#define DEBUG_SEQUTIL       0x00800000

// Execution control
#define UNITTEST_ON         0x01000000
#define DEBUG_25            0x02000000
#define DEBUG_26            0x04000000
#define DEBUG_27            0x08000000
#define DEBUG_28            0x10000000
#define DEBUG_29            0x20000000
#define DEBUG_30            0x40000000
#define DEBUG_WIP           0x80000000	// Work in Progress



//-----------------------------------------------------------------------------
// fTREI macro - for debugging
//-----------------------------------------------------------------------------

#if 0
/* NOTE: Enable the following macro for debugging measurement aborts of libRT
         if building up an  event table fails due to wrong raster times.
WARNING: Never enable this macro in a sequence used for scanning - the 
         diagnostic output exceeds MCPU performance.
*/
#pragma message ("******************************************************")
#pragma message ("*** WARNING: fRTEI macros for screen dump enabled  ***")
#pragma message ("******************************************************")
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sREADOUT.h"
#include "MrServers/MrMeasSrv/SeqIF/libRT/sSYNC.h"

#define fRTEI(time,nco,rf,adc,phase,read,slice,sync) {\
  printf ("%8d ", time) ;\
  if (rf)    printf ("%5.1f/%4d  ",\
                     ((sRF_PULSE*)rf)->getFlipAngle(), ((sRF_PULSE*)rf)->getDuration() ); \
  else printf (". . . . .   ") ; \
  if (nco)   printf ("%6d/%8.2f ",\
                     ((sFREQ_PHASE*)nco)->getFrequency(), ((sFREQ_PHASE*)nco)->getPhase() );\
  else printf (". . . . . . . . ") ;\
  if (adc)   printf ("%3d/%6.1f  ",\
                     ((sREADOUT*)adc)->getColumns(), ((sREADOUT*)adc)->getDuration() );\
  else printf (" . . . . .  ") ;\
  if (phase) printf ("%5.2f/%4d/%5d/%4d  ", \
                     ((sGRAD_PULSE*)phase)->getAmplitude(), \
                     ((sGRAD_PULSE*)phase)->getRampUpTime(), \
                     ((sGRAD_PULSE*)phase)->getDuration(), \
                     ((sGRAD_PULSE*)phase)->getRampDownTime() ); \
  else printf (". . . . . . . . . . .  ") ; \
  if (read) printf ("%5.2f/%4d/%5d/%4d  ", \
                     ((sGRAD_PULSE*)read)->getAmplitude(), \
                     ((sGRAD_PULSE*)read)->getRampUpTime(), \
                     ((sGRAD_PULSE*)read)->getDuration(), \
                     ((sGRAD_PULSE*)read)->getRampDownTime() ); \
  else printf (". . . . . . . . . . .  ") ; \
  if (slice) printf ("%5.2f/%4d/%5d/%4d  ", \
                     ((sGRAD_PULSE*)slice)->getAmplitude(), \
                     ((sGRAD_PULSE*)slice)->getRampUpTime(), \
                     ((sGRAD_PULSE*)slice)->getDuration(), \
                     ((sGRAD_PULSE*)slice)->getRampDownTime() ); \
  else printf (". . . . . . . . . . .  ") ; \
  if (sync)  printf ("SYNC %2d/%4d\n",\
                     ((sSYNC*)sync)->getCode(), ((sSYNC*)sync)->getDuration()) ;\
  else printf (". . . . . .\n") ; \
  fRTEI(time,nco,rf,adc,phase,read,slice,sync) ;\
}
#endif


//-----------------------------------------------------------------------------
//  Inline functions
//-----------------------------------------------------------------------------
#ifdef BUILD_libSeqSysProp
  #define __OWNER
#endif
 
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"
long __IMP_EXP getMaskFromRegistry (const char* origin, bool bTraceErrors=true);


//////////////////////////////////////////////////////////////////////////////


inline long getMaskFromRegistry (const char* origin, long DebugBit, long mask)
{
  static long lMyMask ;
  lMyMask = getMaskFromRegistry (origin) ;
  if (mask & DebugBit)
      lMyMask = lMyMask | mask ;
  return lMyMask ;
}

//////////////////////////////////////////////////////////////////////////////
#if 0 // #### needed???

#if (defined VXWORKS) || (defined BUILD_PLATFORM_LINUX)
  inline long getiValueFromRegistry (const char*)
#else
  inline long getiValueFromRegistry (const char* parameter)
#endif
{  
#if (defined VXWORKS) || (defined BUILD_PLATFORM_LINUX)
  static long iMyMask ;
  iMyMask = 0 ;
#else
  // get debug mode from registry:
  static DWORD iMyMask = static_cast<DWORD>(0);
  static HKEY  _keyHandle;
  static TCHAR tstringbuffer[160];
  static TCHAR tstringbuffer2[80];
  
  if (strlen(parameter)>78) 
  {
    TRACE_PUT1 (TC_INFO, TF_SEQ, "getMaskFromRegistry ERROR: strlen(\"%s\")>=78", parameter) ;
    return 0 ;
  }
  
  mbstowcs (tstringbuffer2, parameter, 79) ;  // convert to wide characters
  
  _stprintf (tstringbuffer, _T("SOFTWARE\\Siemens\\Numaris4\\Config\\Modality\\ImaCalLimits\\%s"), tstringbuffer2);
  
  if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,	// handle of open key 
		                tstringbuffer,	    // address of name of subkey to open 
		                0,			            // reserved 
		                KEY_ALL_ACCESS,	    // security access mask 
		                &_keyHandle		      // address of handle of open key 
		) == ERROR_SUCCESS) 
	{
    static DWORD _type = 0;
    static DWORD iMyMaskSize = sizeof(DWORD);
    
    RegQueryValueEx( _keyHandle,		    // handle of key to query 
		                 _T("V1"),		      // address of name of value to query 
		                 NULL,		          // reserved 
		                 &_type,		        // address of buffer for value type 
		                 (LPBYTE)&iMyMask,	// address of data buffer 
		                 &iMyMaskSize	      // address of data buffer size
		               );
		               
    RegCloseKey(_keyHandle);
  }
  else
    TRACE_PUT1 (TC_INFO, TF_SEQ, "getMaskFromRegistry ERROR: Could not open reg key \"%s\"", parameter) ;
#endif
  return long(iMyMask) ;
}

//////////////////////////////////////////////////////////////////////////////

#if (defined VXWORKS) || (defined BUILD_PLATFORM_LINUX)
  inline double getdValueFromRegistry (const char*)
#else
  inline double getdValueFromRegistry (const char* parameter)
#endif
{  
  double dValue = 0;
  
#ifdef _WINDOWS
  // get debug mode from registry:

  try {
    CsaConfig csaConfig (_T("ImaCalLimits"), _T("Numaris4"), CsaConfig::Site);
    if ( !csaConfig.isValid() )
    {
      TRACE_PUT1 (TC_INFO, TF_SEQ, "getValueFromRegistry: valid failed for parameter %s", parameter);
      dValue = 0.0 ;
    }
    if (!csaConfig[parameter].get(dValue) )
    {
      TRACE_PUT1 (TC_INFO, TF_SEQ, "getValueFromRegistry: get value failed for parameter %s", parameter);
      dValue = 0 ;
    }
  }
  catch (...) {
    // MedCom hat's mal wieder gefetzt...
    TRACE_PUT0 (TC_INFO, TF_SEQ, "getValueFromRegistry: failed (catch)");
    dValue = 0.0 ;
  }
#endif
  return double(dValue) ;
}
#endif



//-----------------------------------------------------------------------------
//  Macros
//-----------------------------------------------------------------------------


#define IS_DEBUG_BIT(seqlimref, bitmask) \
	( (seqlimref).getSeqDebugMode().getDebugMask() & (bitmask) )

#define IS_DEBUG_BITMASK(seqlimref, bitmask) \
	( ((seqlimref).getSeqDebugMode().getDebugMask() & (bitmask)) == (bitmask))


#define PRINT_TRACE0(seqlimref, severity, origin, format) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT1 (TC_INFO, TF_SEQ, "%s: "format, ptModule) ; \
	}

#define PRINT_TRACE1(seqlimref, severity, origin, format, arg1) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT2 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1) ; \
	}

#define PRINT_TRACE2(seqlimref, severity, origin, format, arg1, arg2) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT3 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2) ; \
	}

#define PRINT_TRACE3(seqlimref, severity, origin, format, arg1, arg2, arg3) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT4 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3) ; \
	}

#define PRINT_TRACE4(seqlimref, severity, origin, format, arg1, arg2, arg3, arg4) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT5 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4) ; \
	}

#define PRINT_TRACE5(seqlimref, severity, origin, format, arg1, arg2, arg3, arg4, arg5) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT6 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4, arg5) ; \
	}

#define PRINT_TRACE6(seqlimref, severity, origin, format, arg1, arg2, arg3, arg4, arg5, arg6) \
	{\
	if ( IS_DEBUG_BIT (seqlimref, severity)  &&  IS_DEBUG_BIT (seqlimref, origin) ) \
		TRACE_PUT7 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4, arg5, arg6) ; \
	}


// macros for tracing in classes

#define PRINT0(mask, format) \
	{\
	if ( m_lDebugLevel & mask ) \
		TRACE_PUT1 (TC_INFO, TF_SEQ, "%s: "format, ptModule) ; \
	}

#define PRINT1(mask, format, arg1) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT2 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1) ; \
	}

#define PRINT2(mask, format, arg1, arg2) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT3 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2) ; \
	}

#define PRINT3(mask, format, arg1, arg2, arg3) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT4 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3) ; \
	}

#define PRINT4(mask, format, arg1, arg2, arg3, arg4) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT5 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4) ; \
	}

#define PRINT5(mask, format, arg1, arg2, arg3, arg4, arg5) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT6 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4, arg5) ; \
	}

#define PRINT6(mask, format, arg1, arg2, arg3, arg4, arg5, arg6) \
	{\
	if (  m_lDebugLevel & mask ) \
		TRACE_PUT7 (TC_INFO, TF_SEQ, "%s: "format, ptModule, arg1, arg2, arg3, arg4, arg5, arg6) ; \
	}



#define IS_UNIT_TEST_ACTIVE(seqlimref) \
	( (seqlimref).getSeqDebugMode().getDebugMask() & UNITTEST_ON )




#define PRINT_DEBUGMASK(seqLimRef) \
  {\
    unsigned long i, status ;\
    char bitarray[40] ;\
    char outline[200] ;\
    status = (seqLimRef).getSeqDebugMode().getDebugMask() ;\
    sprintf (outline, "%s: DebugMask = 0x%08lx = ", ptModule, status) ;\
    unsigned long mask = 1 ;\
    for (i=0; i<32; i++) {\
	  bitarray[31-i] = char( ( (mask & status) != 0)  + '0') ;\
	  mask <<= 1 ;\
	}\
    strcpy (bitarray+32, "\n\0") ;\
    strcat (outline, bitarray) ;\
    TRACE_PUT0 (TC_INFO, TF_SEQ, outline) ; \
  }


// -----------------------------------------------------------------------
// defines for comfortable debug-Output
//
// note: if you want to use these macros, you've gotta define DEBUG_ORIGIN
// -----------------------------------------------------------------------

#define TR_RUN(seqlimref)       {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_CALL    ) && !(seqlimref).isContextPrepForBinarySearch()) entryTR("running" )}
#define TR_END(seqlimref)       {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RETURN  ) && !(seqlimref).isContextPrepForBinarySearch()) entryTR("finished")}

#define IS_TR_RUN(seqlimref)    (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_CALL    ) && !(seqlimref).isContextPrepForBinarySearch())
#define IS_TR_END(seqlimref)    (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RETURN  ) && !(seqlimref).isContextPrepForBinarySearch())
#define IS_TR_INP(seqlimref)    (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INPUT   ) && !(seqlimref).isContextPrepForBinarySearch())
#define IS_TR_RES(seqlimref)    (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RESULT  ) && !(seqlimref).isContextPrepForBinarySearch())
#define IS_TR_INT(seqlimref)    (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INTERNAL) && !(seqlimref).isContextPrepForBinarySearch())
#define IS_TR_LAND(seqlimref)   (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_LANDMARK) && !(seqlimref).isContextPrepForBinarySearch())

#define D_TR_INP(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INPUT   ) && !(seqlimref).isContextPrepForBinarySearch()) doubleTR(A,B)}
#define L_TR_INP(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INPUT   ) && !(seqlimref).isContextPrepForBinarySearch()) longTR(A,B)  }
#define S_TR_INP(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INPUT   ) && !(seqlimref).isContextPrepForBinarySearch()) stringTR(A,B)}
#define T_TR_INP(seqlimref,A)   {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INPUT   ) && !(seqlimref).isContextPrepForBinarySearch()) textTR(A)    }

#define D_TR_RES(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RESULT  ) && !(seqlimref).isContextPrepForBinarySearch()) doubleTR(A,B)}
#define L_TR_RES(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RESULT  ) && !(seqlimref).isContextPrepForBinarySearch()) longTR(A,B)  }
#define S_TR_RES(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RESULT  ) && !(seqlimref).isContextPrepForBinarySearch()) stringTR(A,B)}
#define T_TR_RES(seqlimref,A)   {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_RESULT  ) && !(seqlimref).isContextPrepForBinarySearch()) textTR(A)    }

#define D_TR_INT(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INTERNAL) && !(seqlimref).isContextPrepForBinarySearch()) doubleTR(A,B)}
#define L_TR_INT(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INTERNAL) && !(seqlimref).isContextPrepForBinarySearch()) longTR(A,B)  }
#define S_TR_INT(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INTERNAL) && !(seqlimref).isContextPrepForBinarySearch()) stringTR(A,B)}
#define T_TR_INT(seqlimref,A)   {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_INTERNAL) && !(seqlimref).isContextPrepForBinarySearch()) textTR(A)    }

#define D_TR_LAND(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_LANDMARK) && !(seqlimref).isContextPrepForBinarySearch()) doubleTR(A,B)}
#define L_TR_LAND(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_LANDMARK) && !(seqlimref).isContextPrepForBinarySearch()) longTR(A,B)  }
#define S_TR_LAND(seqlimref,A,B) {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_LANDMARK) && !(seqlimref).isContextPrepForBinarySearch()) stringTR(A,B)}
#define T_TR_LAND(seqlimref,A)   {if (IS_DEBUG_BITMASK(seqlimref,DEBUG_ORIGIN|DEBUG_LANDMARK) && !(seqlimref).isContextPrepForBinarySearch()) textTR(A)    }

#define DOUBLE_TR(seqlimref,A,B) {TRACE_PUT3(TC_INFO, TF_SEQ, "%s: %-40s = %15.6f"  ,ptModule, (char*) A, (double)(B));}
#define LONG_TR(seqlimref,A,B)   {TRACE_PUT3(TC_INFO, TF_SEQ, "%s: %-40s = %8ld"    ,ptModule, (char*) A, (long)  (B));}
#define STRING_TR(seqlimref,A,B) {TRACE_PUT3(TC_INFO, TF_SEQ, "%s: %-40s = %s"      ,ptModule, (char*) A, (char*) (B));}
#define TEXT_TR(seqlimref,A)     {TRACE_PUT2(TC_INFO, TF_SEQ, "%s: %s"              ,ptModule, (char*) A             );}

#define ENTRY_TR(seqlimref,A)    {TRACE_PUT2(TC_INFO, TF_SEQ, "%s: %-8s -----------------------------------------", ptModule,A);}


// -----------------------------------------------------------------------
// Old style macros
//
// note: these require the SeqLim pointer *pSeqLim
// -----------------------------------------------------------------------
#define mIsDebugBit(bitmask) IS_DEBUG_BIT(*pSeqLim, bitmask)
#define mIsDebugBitMask(bitmask) IS_DEBUG_BITMASK(*pSeqLim, bitmask)

#define mPrintTrace0(severity, origin, format) PRINT_TRACE0(*pSeqLim, severity, origin, format)
#define mPrintTrace1(severity, origin, format, arg1) PRINT_TRACE1(*pSeqLim, severity, origin, format, arg1)
#define mPrintTrace2(severity, origin, format, arg1, arg2) PRINT_TRACE2(*pSeqLim, severity, origin, format, arg1, arg2)
#define mPrintTrace3(severity, origin, format, arg1, arg2, arg3) PRINT_TRACE3(*pSeqLim, severity, origin, format, arg1, arg2, arg3)
#define mPrintTrace4(severity, origin, format, arg1, arg2, arg3, arg4) PRINT_TRACE4(*pSeqLim, severity, origin, format, arg1, arg2, arg3, arg4)
#define mPrintTrace5(severity, origin, format, arg1, arg2, arg3, arg4, arg5) PRINT_TRACE5(*pSeqLim, severity, origin, format, arg1, arg2, arg3, arg4, arg5)
#define mPrintTrace6(severity, origin, format, arg1, arg2, arg3, arg4, arg5, arg6) PRINT_TRACE6(*pSeqLim, severity, origin, format, arg1, arg2, arg3, arg4, arg5, arg6)


#define mIsUnittestActive() IS_UNIT_TEST_ACTIVE(*pSeqLim)

#define mPrintDebugMask() PRINT_DEBUGMASK(*pSeqLim)


#define mTRrun         TR_RUN(*pSeqLim)                              
#define mTRend         TR_END(*pSeqLim)                              
                                                                     
#define mIsTRrun       IS_TR_RUN(*pSeqLim)                           
#define mIsTRend       IS_TR_END(*pSeqLim)                           
#define mIsTRinp       IS_TR_INP(*pSeqLim)                           
#define mIsTRres       IS_TR_RES(*pSeqLim)                           
#define mIsTRint       IS_TR_INT(*pSeqLim)                           
#define mIsTRland      IS_TR_LAND(*pSeqLim)                          
                                                                     
#define mDTRinp(A,B)   D_TR_INP(*pSeqLim,A,B)                        
#define mLTRinp(A,B)   L_TR_INP(*pSeqLim,A,B)                        
#define mSTRinp(A,B)   S_TR_INP(*pSeqLim,A,B)                        
#define mTTRinp(A)     T_TR_INP(*pSeqLim,A)                          
                                                                     
#define mDTRres(A,B)   D_TR_RES(*pSeqLim,A,B)                        
#define mLTRres(A,B)   L_TR_RES(*pSeqLim,A,B)                        
#define mSTRres(A,B)   S_TR_RES(*pSeqLim,A,B)                        
#define mTTRres(A)     T_TR_RES(*pSeqLim,A)                          
                                                                     
#define mDTRint(A,B)   D_TR_INT(*pSeqLim,A,B)                        
#define mLTRint(A,B)   L_TR_INT(*pSeqLim,A,B)                        
#define mSTRint(A,B)   S_TR-INT(*pSeqLim,A,B)                        
#define mTTRint(A)     T_TR_INT(*pSeqLim,A)                          
                                                                     
#define mDTRland(A,B)  D_TR_LAND(*pSeqLim,A,B)                       
#define mLTRland(A,B)  L_TR_LAND(*pSeqLim,A,B)                       
#define mSTRland(A,B)  S_TR_LAND(*pSeqLim,A,B)                       
#define mTTRland(A)    T_TR_LAND(*pSeqLim,A)                         
                                                                     
#define doubleTR(A,B)  DOUBLE_TR(*pSeqLim,A,B)                       
#define longTR(A,B)    LONG_TR(*pSeqLim,A,B)                         
#define stringTR(A,B)  STRING_TR(*pSeqLim,A,B)                       
#define textTR(A)      TEXT_TR(*pSeqLim,A)                           
                                                                     
#define entryTR(A)     ENTRY_TR(*pSeqLim,A)                          


#endif
//-----------------------------------------------------------------------------
//  End of file
//-----------------------------------------------------------------------------

