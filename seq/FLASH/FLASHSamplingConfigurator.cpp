//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 2009  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrServers\MrImaging\seq\FLASH\FLASHSamplingConfigurator.cpp
//	 Version: \main\4
//	  Author: breujo00
//	    Date: 2011-06-15 16:40:29 +02:00
//
//	    Lang: C++
//
//	 Descrip: MR::Measurement::Sequence::KSpace
//
//	 Classes:
//
//	-----------------------------------------------------------------------------
#include "FLASHSamplingConfigurator.h"

#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"
#include "MrServers/MrProtSrv/MrProt/MrProt.h"


using namespace KSpace;
using namespace KSA;
using namespace SEQ_NAMESPACE;


FlashSamplingConfigurator::FlashSamplingConfigurator (void)
    : m_FlashSamplingParams ()
    , m_FlashSampling       ()
{
}



FlashSamplingConfigurator::~FlashSamplingConfigurator (void)
{
}



NLSStatus FlashSamplingConfigurator::translate (const MrProt& rMrProt, SeqLim& rSeqLim)
{
    NLSStatus Status = SEQU__NORMAL;
    bool bBinarySearch = rSeqLim.isContextPrepForBinarySearch();
    

    // --------------------------------------------------------------------------------------------
    // Select the appropriate sampling class instance for the given protocol
    // --------------------------------------------------------------------------------------------
    //
    // For the FLASH sequence, there is only one sampling class being used. Therefore, the protocol
    // has no influence on the chosen sampling class instance. However, it is important to select
    // the sampling class instance and a corresponding sampling parameters object before the
    // standard translation is called. Otherwise, an exception would occur since the standard
    // sampling configurator does not hold a default sampling parameters instance.

    selectSampling       (&m_FlashSampling);
    selectSamplingParams (&m_FlashSamplingParams);
    

    // --------------------------------------------------------------------------------------------
    // Perform the default translation
    // --------------------------------------------------------------------------------------------

    Status = SamplingConfigurator::translate (rMrProt, rSeqLim);
    if (Status.isError()) {
        return Status;
    }


    // --------------------------------------------------------------------------------------------
    // Additional adjustments of the sampling parameters (optional)
    // --------------------------------------------------------------------------------------------
    //
    // The FLASH sequence needs only the standard translation.


    return Status;
}
