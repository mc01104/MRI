// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
//
//  Project: NUMARIS/4
//     File: \n4_servers1\pkg\MrServers\MrImaging\seq\FLASH\FLASHSampling.cpp
//   Author: H IM MR PLM AW T-AFW
//
//     Lang: C++
//
//  Descrip: Source file for FLASHSampling
//
// --------------------------------------------------------------------------------------------

#include "MrServers/MrImaging/seq/FLASH/FLASHSampling.h"

#include "MrServers/MrImaging/seq/FLASH/FLASH.h"

#include "MrServers/MrImaging/libKSpace/KSpaceException.h"
#include "MrServers/MrImaging/libKSpace/Loop2D.h"
#include "MrServers/MrImaging/libKSpace/ComposedLoop3D.h"
#include "MrServers/MrImaging/libKSpace/OrderStrategy.h"
#include "MrServers/MrImaging/libKSpace/SamplingParams.h"
#include "MrServers/MrImaging/libKSpace/PATMask.h"
#include "MrServers/MrImaging/libKSpace/PFMask.h"
#include "MrServers/MrImaging/libKSpace/CombinedMask2D.h"
#include "MrServers/MrImaging/libKSpace/CombinedMask3D.h"
#include "MrServers/MrImaging/libKSpace/ComposedMask3D.h"
#include "MrServers/MrImaging/libKSpace/EllipticalMask.h"


using namespace KSpace;
using namespace SEQ_NAMESPACE;


FlashSamplingParams::FlashSamplingParams (void)
    : SamplingParams    ()
    , m_eLinesOrderMode (OrderStrategy::ORDER_MODE_ASCENDING)
{
}



FlashSamplingParams::~FlashSamplingParams (void)
{
}



std::string FlashSamplingParams::toString (const std::string& rDelimiter) const
{
    std::ostringstream Result;

    Result << SamplingParams::toString (rDelimiter);
    Result << "lines order mode: " << OrderStrategy::getNameForOrderMode (m_eLinesOrderMode) << rDelimiter;
    
    return Result.str();
}



bool FlashSamplingParams::compare (const SamplingParams& rOther) const
{
    if (!SamplingParams::compare (rOther)) return false;

    const FlashSamplingParams& rConverted = convertParameters <FlashSamplingParams> (rOther);

    // members of CaipirinhaSamplingParams
    if (m_eLinesOrderMode != rConverted.m_eLinesOrderMode) return false;
    
    return true;
}



void FlashSamplingParams::copyFrom (const SamplingParams& rOther)
{
    SamplingParams::copyFrom (rOther);

    const FlashSamplingParams& rConverted = convertParameters <FlashSamplingParams> (rOther);

    m_eLinesOrderMode = rConverted.m_eLinesOrderMode;
}







FlashSampling::FlashSampling (void)
    : SamplingTable <KSpacePoint> ()
    , m_SamplingInfo          ()
    , m_bSamplingInfoOutdated (true)
    , m_pLinesLoop            (NULL)
    , m_pLinesOrder           (NULL)
    , m_pLinesPATMask         (NULL)
    , m_pLinesPFMask          (NULL)
    , m_pLinesMask            (NULL)
    , m_pPartitionsLoop       (NULL)
    , m_pPartitionsOrder      (NULL)
    , m_pLoop3D               (NULL)
    , m_pEllipticalMask       (NULL)
{
    // --------------------------------------------------------------------------------------------
    // Initialize modules that cover the lines direction
    // --------------------------------------------------------------------------------------------

    m_pLinesLoop    = new Loop2D();
    m_pLinesOrder   = new OrderStrategy();
    m_pLinesPATMask = new PATMask();
    m_pLinesPFMask  = new PFMask();
    m_pLinesMask    = new CombinedMask2D();

    m_pLinesMask->addMask (*m_pLinesPATMask);
    m_pLinesMask->addMask (*m_pLinesPFMask);
    m_pLinesLoop->setMask (*m_pLinesMask);
    m_pLinesLoop->setOrderStrategy (*m_pLinesOrder);

    
    // --------------------------------------------------------------------------------------------
    // Initialize modules that cover the partitions direction
    // --------------------------------------------------------------------------------------------

    m_pPartitionsLoop  = new Loop2D();
    m_pPartitionsOrder = new OrderStrategy();
    m_pPartitionsLoop->setOrderStrategy (*m_pPartitionsOrder);


    // --------------------------------------------------------------------------------------------
    // Initialize module that connects both loops to a 3D-loop and the elliptical mask
    // --------------------------------------------------------------------------------------------

    m_pLoop3D         = new ComposedLoop3D (*m_pLinesLoop, *m_pPartitionsLoop);
    m_pEllipticalMask = new EllipticalMask();

    setChild ("Loop 3D", *m_pLoop3D);
    setChild ("Elliptical Mask", *m_pEllipticalMask);
}



FlashSampling::~FlashSampling (void)
{
    delete m_pLinesLoop;
    delete m_pLinesOrder;
    delete m_pLinesPATMask;
    delete m_pLinesPFMask;
    delete m_pLinesMask;

    delete m_pPartitionsLoop;
    delete m_pPartitionsOrder;

    delete m_pLoop3D;
    delete m_pEllipticalMask;
}



SamplingInfo FlashSampling::getSamplingInfo (void)
{
    if (m_bSamplingInfoOutdated) {
        calculateSamplingInfo();
    }

    return m_SamplingInfo;
}



void FlashSampling::configure (const SamplingParams& rParams)
{
    // The super method must be called at first
    SamplingTable <KSpacePoint>::configure (rParams);

    const FlashSamplingParams& rConverted = convertParameters <FlashSamplingParams> (rParams);

    // implicitly sets the size of all child modules (m_pLinesLoop, m_pPartitionsLoop, etc.)
    m_pLoop3D->getMetrics().setSize (rConverted.m_iNumberOfLines, rConverted.m_iNumberOfPartitions);

    // Adjust modules for lines loop
    m_pLinesOrder->setMode                        (rConverted.m_eLinesOrderMode);
    m_pLinesPATMask->setPATFactor                 (rConverted.m_iLinesPATFactor);
    m_pLinesPATMask->setNumberOfReferenceElements (rConverted.m_iNumberOfReferenceLines);
    m_pLinesPFMask->setPFFactor                   (rConverted.m_dLinesPFFactor);

    // Adjust modules for partitions loop
    m_pPartitionsOrder->setMode (OrderStrategy::ORDER_MODE_ASCENDING);

    if (rConverted.m_bEllipticalScanning) {
        m_pEllipticalMask->setActive (true);

        // Elliptical mask is no child module of m_pLoop3D -> set size here
        m_pEllipticalMask->getMetrics().setSize (rConverted.m_iNumberOfLines, rConverted.m_iNumberOfPartitions);
    } else {
        m_pEllipticalMask->setActive (false);
    }

    resetPreparation();
}



bool FlashSampling::prepareInternal (void)
{
    m_bSamplingInfoOutdated = true;

    // The super-method must be called at first
    if (!SamplingTable <KSpacePoint>::prepareInternal()) {
        KS_TRACE_PREP_ERROR ("Calling super-method SamplingTable <KSpacePoint>::prepareInternal() failed!");
        return false;
    }

    if (!m_pEllipticalMask->prepare()) {
        KS_TRACE_PREP_ERROR ("The elliptical mask could not be prepared!");
        return false;
    }

    if (!m_pLoop3D->prepare()) {
        KS_TRACE_PREP_ERROR ("The 3D-loop could not be prepared!");
        return false;
    }

    return true;
}



void FlashSampling::calculateInternal (void)
{
    m_bSamplingInfoOutdated = true;

    // The super method must be called at first
    SamplingTable <KSpacePoint>::calculateInternal();

    m_pLoop3D->calculate();
    m_pEllipticalMask->calculate();

    // Start with an empty table
    clearTable();
    
    // Insert elements from m_pLoop3D, but only those accepted by the elliptical mask
    append (*m_pLoop3D, m_pEllipticalMask);

    // Set FT-flags and PAT-flags
    setFTFlags (m_pLoop3D->getMetrics().getNumberOfLines(), m_pLoop3D->getMetrics().getNumberOfPartitions());
    setPATFlags (m_pLinesPATMask, NULL);
}



void FlashSampling::calculateSamplingInfo (void)
{
    // --------------------------------------------------------------------------------------------------------------
    // Create a temporary mask that combines all masks
    // --------------------------------------------------------------------------------------------------------------

    ComposedMask3D TempComposedMask3D (m_pLinesMask, NULL);

    CombinedMask3D TempCombinedMask3D;
    TempCombinedMask3D.addMask (*m_pEllipticalMask);
    TempCombinedMask3D.addMask (TempComposedMask3D);

    TempCombinedMask3D.getMetrics().copyFrom (m_pLoop3D->getMetrics());


    // --------------------------------------------------------------------------------------------------------------
    // Use the temporary mask to calculate the sampling information
    // --------------------------------------------------------------------------------------------------------------

    if (!TempCombinedMask3D.prepare()) {
        THROW_MR_MSG (KSpaceException, "TempCombinedMask3D could not be prepared!");
    }

    TempCombinedMask3D.calculate();

    m_SamplingInfo.setInfoFor (TempCombinedMask3D, m_pLinesPATMask, NULL);


    m_SamplingInfo.m_iLoopIndexAtKSpaceCenter          = -1;
    m_SamplingInfo.m_iEstimatedLoopIndexAtKSpaceCenter = -1;

    if (isCalculated()) {

        // --------------------------------------------------------------------------------------------------------------
        // Calculation of loop index at k-space center if the sampling table has been actually calculated.
        // --------------------------------------------------------------------------------------------------------------

        int iCenterLine      = m_SamplingInfo.m_iCenterLine;
        int iCenterPartition = m_SamplingInfo.m_iCenterPartition;

        int iNum = getTotalNumberOfKSpacePoints();

        for (int iI = 0; iI < iNum; iI++) {
            KSpacePoint& rPoint = elementAt (iI);
            if ((rPoint.getLineId() == iCenterLine) && (rPoint.getPartitionId() == iCenterPartition)) {
                m_SamplingInfo.m_iLoopIndexAtKSpaceCenter = iI;
                break;
            }
        }

        if (m_SamplingInfo.m_iLoopIndexAtKSpaceCenter < 0) {
            THROW_MR_MSG (KSpaceException, "Loop index of k-space center could not be calculated (is never measured)!");
        }

        m_SamplingInfo.m_iEstimatedLoopIndexAtKSpaceCenter = m_SamplingInfo.m_iLoopIndexAtKSpaceCenter;
    }
}


// --------------------------------------------------------------------------------------------
//    Copyright (C) Siemens AG 2010  All Rights Reserved. Confidential.
// --------------------------------------------------------------------------------------------
