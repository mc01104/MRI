//  -----------------------------------------------------------------
//  Copyright (C) Siemens AG 1998  All Rights Reserved.  Confidential
//  -----------------------------------------------------------------
//
//          Project: NUMARIS/4
//             File: \n4\pkg\MrServers\MrImaging\seq\a_DW_BLADE\THIN.h
//          Version: \main\branch\5
//
//         Language: C++
//
//      Description: Binominal excitation pulse for on-resonance (water) excitation.
//          Structs: 
//
//  -----------------------------------------------------------------


//  Definition of struct PENCIL_BEAM_EXPO
#include "MrServers/MrImaging/seq/common/libPace/IExcit.h"

//  Definition of sRF_PULSE_RECT
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"

//  Definition of sGRAD_PULSE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"

//  Definition of sFREQ_PHASE
#include "MrServers/MrMeasSrv/SeqIF/libRT/sFREQ_PHASE.h"

//  Definition of NLS_STATUS
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"


#ifndef __RBinominalExcit_sel_H
#define __RBinominalExcit_sel_H

namespace PACE
{
    class RBinominalExcit_sel : public IExcit
    {
    public:

        typedef struct BINOMINAL_EXCIT_PARA PARA;

        typedef struct BINOMINAL_EXCIT_EXPO EXPO;

        RBinominalExcit_sel();

        virtual ~RBinominalExcit_sel();

        NLS_STATUS prep(EXPO* pExpo, PARA* pPara);
        
        //  Return value is undefined unless class has been prepared.
        virtual int32_t getDuration_us() const;
        virtual int32_t getRFCenter_us() const;
        virtual double getEnergy_Ws() const;

        //  Restores state before preparation
        virtual void   clear();


        virtual NLS_STATUS run(ISLICE_POS* pSlc, double dRFPhase_deg, double dRFDeltaFrequency_Hz, int32_t i32Offset_us);
    protected:
#ifdef _VB_LINE
        sRF_PULSE* m_pRF;
#else
        IRF_PULSE* m_pRF;
#endif
        //  Number of RF pulses 
        uint32_t                      m_u32NRF;
        //  Object used to set frequency and phase for RF pulse
        sFREQ_PHASE                   m_sRFSet;   
        //  Object used to reset synthesizer
        sFREQ_PHASE                   m_sRFNeg;

        //  Slice selection gradient
        sGRAD_PULSE_TRAP              m_sGSS_f; //  (f)ront 
        sGRAD_PULSE_TRAP              m_sGSS_i; //  (i)ntermediate  NRF > 2
        sGRAD_PULSE_TRAP              m_sGSS_b; //  (b)ack NRF > 1

        //  Slice refocusing gradient
        sGRAD_PULSE_TRAP              m_sGSR;
    };
};

#endif
