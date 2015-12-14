//	-----------------------------------------------------------------------------
//	  Copyright (C) Siemens AG 2010  All Rights Reserved.
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4\pkg\MrServers\MrImaging\seq\FLASH\FLASHSamplingConfigurator.h
//	 Version: \main\3
//	  Author: breujo00
//	    Date: 2011-06-07 18:17:53 +02:00
//
//	    Lang: C++
//
//	 Descrip: MR::Measurement::Sequence::KSpace
//
//	 Classes:
//
//	-----------------------------------------------------------------------------
#ifndef FLASH_FLASHSamplingConfigurator_h
#define FLASH_FLASHSamplingConfigurator_h 1


#include "MrServers/MrImaging/seq/FLASH/FLASHSampling.h"
#include "MrServers/MrImaging/libKSA/SamplingConfigurator.h"


namespace SEQ_NAMESPACE {

    // --------------------------------------------------------------------------------------------
    ///  \ingroup  grp_libKSpace_Classes
    ///
    ///  \brief    This sampling configurator is dedicated to the FLASH sequence.
    ///
    ///            The adaptions required for this sequence are as follows:
    ///            - The method setLimits sets the desired UI parameter limits
    ///            - The method translate selects m_FlashSampling as sampling class instance and
    ///              m_FlashSamplingParams as parameters instance.
    // --------------------------------------------------------------------------------------------
    class FlashSamplingConfigurator : public KSA::SamplingConfigurator
    {
    public:

        // --------------------------------------------------------------------------------------------
        ///  \brief    Standard constructor.
        // --------------------------------------------------------------------------------------------
        FlashSamplingConfigurator (void);
        
        
        // --------------------------------------------------------------------------------------------
        ///  \brief    Standard destructor.
        // --------------------------------------------------------------------------------------------
        virtual ~FlashSamplingConfigurator (void);
        
        
        // --------------------------------------------------------------------------------------------
        ///  \brief    This method is typically intended for choosing between different sampling class
        ///            instances and for translating the given protocol into a sampling parameters
        ///            instance.
        ///
        ///            However, the FLASH demo sequence uses only one sampling class. Therefore, this
        ///            method will always select m_FlashSampling to be used as sampling class
        ///            instance.
        ///
        ///  \param    rMrProt  Reference to the protocol
        ///  \param    rSeqLim  Reference to the sequence limits
        ///
        ///  \return   The NLS Status
        // --------------------------------------------------------------------------------------------
        virtual NLSStatus translate (const MrProt& rMrProt, SeqLim& rSeqLim);


    protected:

        FlashSamplingParams  m_FlashSamplingParams;
        FlashSampling        m_FlashSampling;


    private:

        FlashSamplingConfigurator (const FlashSamplingConfigurator& rRight);
        FlashSamplingConfigurator& operator= (const FlashSamplingConfigurator& rRight);
    };
}

#endif
