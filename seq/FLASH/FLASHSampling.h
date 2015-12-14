// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FLASHSampling.h
//   Author: H IM MR PLM AW T-AFW
//
//     Lang: C++
//
//  Descrip: Header file for FLASHSampling
//
// --------------------------------------------------------------------------------------------

#ifndef __FLASHSampling_h
#define __FLASHSampling_h 1


#include "MrServers/MrImaging/libKSpace/SamplingInfo.h"
#include "MrServers/MrImaging/libKSpace/SamplingParams.h"
#include "MrServers/MrImaging/libKSpace/SamplingTable.h"
#include "MrServers/MrImaging/libKSpace/OrderStrategy.h"


// DLL exports/imports
#ifdef BUILD_SEQU
#define __OWNER
#endif
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"


namespace KSpace {

    // forward declarations
    class EllipticalMask;
    class Mask2D;
    class PATMask;
    class PFMask;
    class CombinedMask2D;
    class Loop2D;
    class ComposedLoop3D;
}



namespace SEQ_NAMESPACE {

    // --------------------------------------------------------------------------------------------
    ///  \brief    This class is an extension to the class SamplingParams. The additional
    ///            parameters are required for realizing PAT and PF in PE direction.
    // --------------------------------------------------------------------------------------------
    class __IMP_EXP FlashSamplingParams : public KSpace::SamplingParams
    {
    public:

        KSpace::OrderMode  m_eLinesOrderMode;

        FlashSamplingParams (void);
        virtual ~FlashSamplingParams (void);

        virtual std::string toString (const std::string& rDelimiter = std::string ("; ")) const;
        virtual bool compare (const KSpace::SamplingParams& rOther) const;
        virtual void copyFrom (const SamplingParams& rOther);

    private:
        FlashSamplingParams (const FlashSamplingParams& rRight);
        FlashSamplingParams& operator= (const FlashSamplingParams& rRight);
    };







    // --------------------------------------------------------------------------------------------
    ///  \brief    This class is the sampling class for the FLASH sequence.
    ///
    ///            The following features are realized:
    ///            - 3D-measurements
    ///            - Parallel acquisition technique (PAT)
    ///            - Partial fourier (PF)
    ///            - Elliptical scanning
    // --------------------------------------------------------------------------------------------
    class __IMP_EXP FlashSampling : public KSpace::SamplingTable <KSpace::KSpacePoint>
    {
    public:

        // --------------------------------------------------------------------------------------------
        ///  \brief    Constructs a main sampling instance and creates and connects all required
        ///            modules.
        // --------------------------------------------------------------------------------------------
        FlashSampling (void);


        // --------------------------------------------------------------------------------------------
        ///  \brief    Implicitly deletes all internal modules.
        // --------------------------------------------------------------------------------------------
        virtual ~FlashSampling (void);


        // --------------------------------------------------------------------------------------------
        ///  \brief    Returns information about the currently prepared sampling table.
        ///
        ///  \see      calculateSamplingInfo()
        ///
        ///  \return   SamplingInfo about the currently prepared sampling table
        // --------------------------------------------------------------------------------------------
        virtual KSpace::SamplingInfo getSamplingInfo (void);


    protected:

        KSpace::SamplingInfo m_SamplingInfo;
        bool                 m_bSamplingInfoOutdated;

        // Modules for the lines direction
        KSpace::Loop2D*         m_pLinesLoop;
        KSpace::OrderStrategy*  m_pLinesOrder;
        KSpace::PATMask*        m_pLinesPATMask;
        KSpace::PFMask*         m_pLinesPFMask;
        KSpace::CombinedMask2D* m_pLinesMask;

        // Modules for the partitions direction
        KSpace::Loop2D*        m_pPartitionsLoop;
        KSpace::OrderStrategy* m_pPartitionsOrder;

        // Module that composes both loops to a 3D loop
        KSpace::ComposedLoop3D* m_pLoop3D;

        // Module to realize an elliptical mask
        KSpace::EllipticalMask* m_pEllipticalMask;


        // --------------------------------------------------------------------------------------------
        ///  \brief    Prepares all internal modules.
        ///
        ///  \return   \arg \c true  : If preparation was successful
        ///            \arg \c false : Otherwise
        // --------------------------------------------------------------------------------------------
        virtual bool prepareInternal (void);


        // --------------------------------------------------------------------------------------------
        ///  \brief    Calculates all internal modules and builds the k-space sampling table.
        // --------------------------------------------------------------------------------------------
        virtual void calculateInternal (void);

        
        // --------------------------------------------------------------------------------------------
        ///  \brief    Reads the given parameters and adjusts the internal modules accordingly.
        ///
        ///  \param    rParams  Sampling parameters
        // --------------------------------------------------------------------------------------------
        virtual void configure (const KSpace::SamplingParams& rParams);


        // --------------------------------------------------------------------------------------------
        ///  \brief    Calculates information about the currently prepared sampling table.
        // --------------------------------------------------------------------------------------------
        virtual void calculateSamplingInfo (void);


    private:

        FlashSampling (const FlashSampling& rRight);
        FlashSampling& operator= (const FlashSampling& rRight);
    };
}


#endif

// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
