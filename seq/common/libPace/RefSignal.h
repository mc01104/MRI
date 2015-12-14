//  -----------------------------------------------------------------
//      Copyright (C) Siemens AG 1998  All Rights Reserved.
//  -----------------------------------------------------------------
//
//     Project: NUMARIS/4
//        File: \n4\pkg\MrServers\MrImaging\seq\common\libPace\RefSignal.h
//     Version:
//      Author: stemal8q
//        Date: n.a.
//
//        Lang: C++
//
//     Descrip: Passes latest physiological signal to ICE.
//
//     Classes:
//
//  -----------------------------------------------------------------

#ifndef _RefSignal_h
#define _RefSignal_h

#ifdef _VB_LINE
//  Definition of NLS_STATUS
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"

//  Definition of SEQ::PhysioSignal
#include "MrServers/MrProtSrv/MrProt/SeqDefines.h"
#else
//  Definition of NLS_STATUS
#include "MrCommon/GlobalDefinitions/MrResult.h"
//  Definition of SEQ::PhysioSignal
#include "ProtBasic/Interfaces/PhysioDefines.h"
#endif

namespace PACE
{
    class RefSignalImpl;

    class RefSignal
    {
    protected:
        RefSignalImpl* m_pImpl;
    public:
        //  Default constructor
        RefSignal();

        //  Destructor
        ~RefSignal();

        NLS_STATUS run(SEQ::PhysioSignal iRefSignal, unsigned short ushUID, int32_t i32Duration_us);

        //  Returns latest signal, retrieven by run method.
        //  If pshSampleB == pshSampleE, the function returns the number of stored samples.
        //  Otherwise it writes the number of requested/available samples to the array pshSampleB points to
        //  and returns the actual number samples written to pshSampleB.
        //  Thersby the returned value is within [0,pshSampleE-pshSampleB].
        uint32_t getLatestSignal(SEQ::PhysioSignal iRefSignal, short* pshSampleB, short* pshSampleE);
    };
}
#endif
