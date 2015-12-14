//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\IExcit.h
//          Version: \main\branch\5
//
//         Language: C++
//
//      Description: Definition of IExcit interface class and
//                   parameter/expo structure for binominal excitation.
//          Structs: 
//
//  -----------------------------------------------------------------

//  Definition of int32_t, ...
#include "MrCommon/MrGlobalDefinitions/MrBasicTypes.h"

//  NLS_STATUS
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"


#ifndef __IExcit_H
#define __IExcit_H

#ifdef _VB_LINE
class sRF_PULSE;
class sSLICE_POS;
#else
class ISLICE_POS;
class IRF_PULSE;
#endif



namespace PACE
{
    enum Traverse
    {
        TRAVERSE_CONST_ANGULAR_RATE   = 0x0
    ,   TRAVERSE_CONST_SLEW_RATE      = 0x1   
    };
    

    struct BINOMINAL_EXCIT_EXPO
    {
        //  Energy exposure ooer request
        double                 m_dEnergy_Ws;
        
        //  Rephase moment in slice select direction which is NOT compensated by the
        //  current event block.
        double                 m_dSlRephaseMom_usmT_m;
        
        
        //  Ramp time outside the current event block and peak amplitude of this ramp.
        double                 m_dSlPeakRampAmpl_mT_m;
        int32_t                m_i32SlRampTimeOutsideEB_us;

        //  Time from the center of the last RF-pulse to the end of the event block (excluding last ramp)
        int32_t                m_i32LastRFCenterToEnd_us;
        
        
        //  Point in time assigned as RF center (to calculate TE for example)
        //  relative to the beginning of the event block
        int32_t                m_i32RFCenter_us;
        
        //  Duration of the RF-pulse
        int32_t                m_i32Duration_us;
        
        BINOMINAL_EXCIT_EXPO()
            : m_dEnergy_Ws(0)
            , m_dSlRephaseMom_usmT_m(0)
            , m_dSlPeakRampAmpl_mT_m(0)
            , m_i32SlRampTimeOutsideEB_us(0)
            , m_i32LastRFCenterToEnd_us(0)
            , m_i32RFCenter_us(0)
            , m_i32Duration_us(0)
        {}
    };
    
    struct BINOMINAL_EXCIT_PARA
    {
        //  Pointer of prepared object of type IRF_PULSE.
        //  The s
#ifdef _VB_LINE
        sRF_PULSE* m_pRF;
#else
        IRF_PULSE* m_pRF;
#endif
        //  Number of RF-pulses of the composite pulse. Must be equal to the number of flip angles
        //  passed to the rf-pulse error prior to preparation of the rf-pulse object
        uint32_t             m_u32NRF;

        //  Larmor constant in MHz/T.
        double               m_dLarmorConst_MHz_T;

        //  To calculate the frequency difference between the off- and the on-resonance component:
        //  Nominal B0 in tesla
        double               m_dB0_T;
        //  Fat-Water shift in parts per million
        double               m_dFatWaterShift_ppm;
        
        //  Maximum gradient amplitude of the reamining gradients [mT/m]
        double               m_dGradMaxAmpl_mT_m;
        
        //  Minimum gradient rise time [us/(mT/m)]
        double               m_dGradMinRiseTime_us_mT_m;

        //  If different from 0, desired ramp time for the first/last ramp of the first/last event block.
        int32_t              m_i32FirstRampTime_us;
        int32_t              m_i32LastRampTime_us;

        //  Usually true, if prepared in context NORMAL
        bool       m_bContextNormal;
                               
        
        BINOMINAL_EXCIT_PARA()
            : m_pRF(0)
            , m_u32NRF(0)
            , m_dLarmorConst_MHz_T(42.5756)
            , m_dGradMaxAmpl_mT_m(0)
            , m_dGradMinRiseTime_us_mT_m(0)
            , m_i32FirstRampTime_us(0)
            , m_i32LastRampTime_us(0)
            , m_bContextNormal(false)
        {}
        
    };

    class IExcit
    {
    public:
        //  Abstract interface class for complex excitation pulses
#ifdef _VB_LINE
        virtual NLS_STATUS run(sSLICE_POS* pSlc, double dRFPhase_deg, double dRFDeltaFrequency_Hz, int32_t i32Offset_us) = 0;
#else
        virtual NLS_STATUS run(ISLICE_POS* pSlc, double dRFPhase_deg, double dRFDeltaFrequency_Hz, int32_t i32Offset_us) = 0;
#endif

        //  Duration of one call of the run function (i32Offset_us = 0)
        virtual int32_t getDuration_us() const = 0;

        //  Energy exposure per run-call.
        virtual double getEnergy_Ws() const    = 0;

        //  Isodelay point relative to the beginning of the event block.
        virtual int32_t getRFCenter_us() const = 0;

        //  Restores state before preparation
        virtual void   clear() = 0;
    };


}  //  END: namespace PACE


#endif // __IExcit_PROT_H
