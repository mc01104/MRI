//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\INavScout.h
//          Version: \main\branch\5
//
//         Language: C++
//
//      Description: Definition of interface class INavScout
//          Structs: 
//
//  -----------------------------------------------------------------

//  -----------------------------------------------------------------
//  Base class definition.
//  -----------------------------------------------------------------

#include "MrServers/MrImaging/seq/common/libPace/IScout.h"

//  -----------------------------------------------------------------
//  Forward Declararions
//  -----------------------------------------------------------------

#ifndef __INAVSCOUT_H
#define __INAVSCOUT_H

namespace PACE
{
    //  -----------------------------------------------------------------
    //  Definition of class INavScout
    //  -----------------------------------------------------------------
    
    class INavScout : public IScout
    {
    public:
        //  Creates object of type INavScout
        static INavScout* create();
        
        //	----------------------------------------------------
        //  Static attributes
        //	----------------------------------------------------
        
        //  Hard limits for number of samples.
        static const long   s_lSAMPLES_MIN;
        static const long   s_lSAMPLES_MAX;
        static const long   s_lSAMPLES_DEF;
        
        //  Hard limits for flip angle [deg].
        static const double s_dFLIP_ANGLE_MIN_DEG;
        static const double s_dFLIP_ANGLE_MAX_DEG;
        static const double s_dFLIP_ANGLE_INC_DEG;
        static const double s_dFLIP_ANGLE_DEF_DEG;
        
        //  Hard limits for sleep (poll) interval [us]
        static const long   s_lSLEEP_INTERV_MIN_US;
        static const long   s_lSLEEP_INTERV_MAX_US;
        static const long   s_lSLEEP_INTERV_INC_US;
        static const long   s_lSLEEP_INTERV_DEF_US;
        
        //  Hard limits for 2DRF resolution [mm]
        static const double s_d2DRF_RES_MIN_MM;
        static const double s_d2DRF_RES_MAX_MM;
        static const double s_d2DRF_RES_INC_MM;
        static const double s_d2DRF_RES_DEF_MM;
        
        //  Hard limits for 2DRF sidelope distance [mm]
        static const double s_d2DRF_SIDELOPE_DIST_MIN_MM;
        static const double s_d2DRF_SIDELOPE_DIST_MAX_MM;
        static const double s_d2DRF_SIDELOPE_DIST_INC_MM;
        static const double s_d2DRF_SIDELOPE_DIST_DEF_MM;
        
        
    };
}  //  namespace PACE
#endif
