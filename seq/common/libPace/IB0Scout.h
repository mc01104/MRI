//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\IB0Scout.h
//          Version: \main\9
//
//         Language: C++
//
//      Description: Navigator which is used to detect physiologically
//                   induced B0-fluctuations.
//          Structs: 
//
//  -----------------------------------------------------------------

//  -----------------------------------------------------------------
//  Base class definition.
//  -----------------------------------------------------------------

#include "MrServers/MrImaging/seq/common/libPace/IScout.h"

//  -----------------------------------------------------------------
//  Used interfaces
#ifdef _VB_LINE
#include "MrServers/MrProtSrv/MrProt/MrSlice.h"
#else
#include "ProtBasic/Interfaces/Slice.h"
#endif

//  -----------------------------------------------------------------
//  Forward Declararions
//  -----------------------------------------------------------------

#ifndef __IB0SCOUT_H
#define __IB0SCOUT_H

class MrProt;
class SeqLim;

#ifdef _VB_LINE
class SeqExpo;
namespace MrProtocolData
{
    typedef ::SeqExpo SeqExpo;
}
#else
namespace MrProtocolData
{
  class SeqExpo;
}
#endif
#ifdef _VB_LINE
class sRF_PULSE;
#else
class IRF_PULSE;
class IRF_PULSE_SINC;
#endif
class sRF_PULSE_SINC;
namespace PACE
{
    template <class TYPE> class TArray;
    //  -----------------------------------------------------------------
    //  Definition of class IB0Scout
    //  -----------------------------------------------------------------

    struct B0SCOUT_PARA
    {
        //  Prepared RF-pulse.
        //  Ownership of the object IRF_PULSE points to is not inherited
#ifdef _VB_LINE
        sRF_PULSE* m_pRF_alpha;
        sRF_PULSE* m_pRF_beta;
#else
        IRF_PULSE* m_pRF_alpha;
        IRF_PULSE* m_pRF_beta;
#endif
        
        //  Number of samples of readout event.
        int32_t    m_i32Samples;
        
        //  Real dwell time [ns]
        //  Null indicates to use shortest dwell time possible.
        int32_t    m_i32RealDwellTime_ns;

        //  The parameter is only used if m_i32RealDwellTime_ns == 0
        //  If != 0, the actual dwell time will be an integer multiple of the parametr.
        //  Note that the specified value must be an integer multiple of the system dwell 
        //  time raster (RX_READOUT_RASTER_NS).
        //  If equal to null the system dwell raster is used
        int32_t    m_i32RealDwellTimeRaster_ns;

        //  Index of Coil select to be used
        uint32_t  m_u32CoilSelectIndex;

        //  Number of phase encoding lines. Default is one.
        int32_t    m_i32NPELin;
        
        //  Number of echoes
        uint32_t    m_u32NEco;

        //  If different from zero, determines the TE difference
        //  between subsequent echoes.
        int32_t     m_i32DeltaTE_us;

        //  Echo time of the first Echo
        int32_t     m_i32TE1_us;

        //  Number of excitation pulses
        uint32_t    m_u32NRF;

        //  If different from 0, prefered ramp time for the last ramp of the last event block.
        int32_t     m_i32LastRampTime_us;

        //  Maximum gradient amplitude [mT/m].
        double m_dGradMaxAmpl_mT_m;
        
        //  Minimum gradient rise time [us/(mT/m)].
        double m_dGradMinRiseTime_usm_mT;
        
        //  The navigator itself.
        Slice       m_sNav;
        //  MrProtocolData::MrNavigatorData  m_sNav;
        
        //  Phase oversampling factor
        double     m_dPhOS;
        
        //  phase dispersion of the spoiler gradient at the end
        //  of the scout sequence along the slice selection direction 
        //  in units of PI
        double     m_dSpoilerPhaseDispersion_pi;

        //  To calculate the frequency difference between the off- and the on-resonance component:
        //  Nominal B0 in tesla
        double               m_dB0_T;
        //  Fat-Water shift in parts per million
        double               m_dFatWaterShift_ppm;

        //  Starting phase used for RF Spoiling
        //  Phase evolution Phi_0 = ePhi0, Phi_j = Phi_j-1 + j*Phi0, j = 1, 2, 3 . . . .
        double               m_dRFSpoilPhasePhi0_rad;
        //  Second argment for Phi_j = fmodf( Phi_j-1 + j*Phi0,dRFSpoilPhaseMax_rad)
        double               m_dRFSpoilPhaseMax_rad;

        //  Indicates whether RF-Spoiling should be used
        bool                 m_bRFSpoiling;
        
        //  Velocity compensation in read and slice direction
        bool                 m_bVelComp;

        //  In the case of multiple echoes.
        //  EPI-like readout gradient waveform.
        bool                 m_bBipolar;
        
        //  Usually true, if prepared in context NORMAL
        bool                 m_bContextNormal;
        
        //  Default constructor
        B0SCOUT_PARA();
        
    };
    
    //  -----------------------------------------------------------------
    //  Definition of class IB0Scout
    //  -----------------------------------------------------------------    
    class IB0Scout : public IScout
    {
    public:
        //  Creates object of type SBBScoutScan
        static IB0Scout* create();

        typedef B0SCOUT_PARA PARA;

        //  Mandatory preparation of the class.
        virtual NLS_STATUS prep(const PARA* pPara) = 0;

        //  Optional initialisation of parameter structure from measurement protocol
        //  If used rpRfPulse receives pointer to sinc RF-Pulse object.
        //  The caller of the function gets the owner of the exported RF-pulse object.
        //  The last parameter is index of the navigator object to be used.
#ifdef _VB_LINE
        static NLS_STATUS init(sRF_PULSE_SINC*&       rpRfPulse_alpha, TArray<double>& radFlipAngle_alpha_deg, sRF_PULSE_SINC*&       rpRfPulse_beta, TArray<double>& radFlipAngle_beta_deg, PARA* pPara, MrProt*       pProt, SeqLim* pSeqLim, SeqExpo*                 pSeqExpo, uint32_t u32NavPos, const char* pszIdent_alpha, const char* pszIdent_beta);
#else
        static NLS_STATUS init(MrPtr<IRF_PULSE_SINC>& rpRfPulse_alpha, TArray<double>& radFlipAngle_alpha_deg, MrPtr<IRF_PULSE_SINC>& rpRfPulse_beta, TArray<double>& radFlipAngle_beta_deg, PARA* pPara, const MrProt* pProt, SeqLim* pSeqLim, MrProtocolData::SeqExpo* pSeqExpo, uint32_t u32NavPos, const char* pszIdent_alpha, const char* pszIdent_beta);
#endif
        
    };
}  //  namespace PACE

#endif
