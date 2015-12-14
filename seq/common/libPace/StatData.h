//  -----------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//  -----------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\StatData.h
//     Version:
//      Author: stemal8q
//        Date: n.a.
//
//        Lang: C++
//
//     Descrip: Delayed export of statistic data to file.
//
//     Classes:
//
//  -----------------------------------------------------------------

#ifndef _StatData_h
#define _StatData_h

//  Definition of NLS_STATUS
#ifdef _VB_LINE
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"
#else
#include "MrCommon/GlobalDefinitions/MrResult.h"
#endif

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

namespace PACE
{
    class StatDataImpl;
    class __IMP_EXP StatData
    {
        //  Similar 
    protected:
        StatDataImpl* m_pImpl;

    public:
        //  Writes arguments to temporary buffer
        void set(int32_t i32NRespIntrv,double  dRespCycle_us);

        //  Reads data stored in temporary buffer.
        //  Returns zero to ri32NRespIntrv, if nothing stored.
        //  Does not read the content of the associated file.
        void get(int32_t& ri32NRespIntrv,double&  rdRespCycle_us) const;

        //  Invalidates buffered data, passed during foregoing call to set.
        void clear();

        //  First stops the measurement and then writes temporary stored data to file.
        //  After this starts the measurement again.
        //  Should be called at the end of fSEQRun
        NLS_STATUS flush(int32_t i32FillBeforeHalt_us, int32_t i32FillAfterHalt_us);

        //  Destructor. 
        ~StatData();

        //  Default constructor
        StatData();

    };
} //  namespace PACE

#ifdef __IMP_EXP_DATA
#undef __IMP_EXP_DATA
#endif

#ifdef __IMP_EXP
#undef __IMP_EXP
#endif

#endif //  _StatData_h
