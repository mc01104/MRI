//    -----------------------------------------------------------------------------
//  Copyright (C) Siemens AG 2001  All Rights Reserved.  Confidential
//    -----------------------------------------------------------------------------
//
// Project: NUMARIS/4
//    File: \n4\pkg\MrServers\MrImaging\seq\common\WIPParameterTool\WIPParameterTool.cpp
//
//    Lang: C++
//
// Descrip: WIPParameterTool class body
//
//    -----------------------------------------------------------------------------


#ifdef WIN32
#define _AFXDLL
#include <afxwin.h>
#include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkBase.h"
#include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkBaseData.h"
#include "MrServers/MrProtSrv/MrProtocol/libUILink/UILinkArray.h"
#include "MrServers/MrProtSrv/MrProtocol/UILink/MrStdNameTags.h"

#endif

#include "WIPParameterTool.h"

#include "MrServers/MrMeasSrv/SeqIF/Sequence/sequmsg.h"     // SEQU_... error codes
#include "MrServers/MrProtSrv/MrProt/ProtDefines.h"
#include "ProtBasic/Interfaces/MrWipMemBlock.h"

#include "MrCommon/UTrace/Macros.h"

#include <map>
#include <exception>

using namespace std;
using namespace WPT_NAMESPACE;


// --------------------------------------------------------------------------------------------
// \brief    The protocol must store the information that the default values have already been
//           set or not. This information is stored in the last element of alFree[] of the
//           WipMemBlock.
//
//           Storing the information in the protocol is necessary. Otherwise, the first
//           sequence prepare after loading an old protocol would overwrite the WIP parameter
//           values.
//
//           The chosen number 12345 is arbitrary.
// --------------------------------------------------------------------------------------------
enum InitStatus {
    NOT_YET_INITIALIZED = 0,
    ALREADY_INITIALIZED = 12345
};


// --------------------------------------------------------------------------------------------
/// \brief    Only up to 30 option values can be supported.
///
///           Apparently, this limitation comes from using 32-bit numbers for storing the
///           current selection status. Every bit refers to one option. The remaining 2 bits
///           are (probably) reserved for other issues.
// --------------------------------------------------------------------------------------------
#define MAX_NUM_OF_OPTIONS 30


// --------------------------------------------------------------------------------------------
/// \brief    Standard number of digits to be displayed.
// --------------------------------------------------------------------------------------------
#define STD_PRECISION 3


#ifndef WIN32

// This code is taken from "UILinkSelection.h"
// This code is required for compilation under Linux
#define UMASK_MODIFIER   0x00ff0000
#define OFFSET_MODIFIER  16

// The UI works with resource IDs. Therefore, the option index must be translated.
// The GET- and SET_MODIFIER-methods perform the conversion
inline unsigned GET_MODIFIER (unsigned uiResourceId)
{
    return (uiResourceId&UMASK_MODIFIER)>>OFFSET_MODIFIER;
}

#undef UMASK_MODFIER
#undef OFFSET_MODIFIER

#endif


// --------------------------------------------------------------------------------------------
///  \brief    This method checks if the given value lVal satisfies the conditions specified
///            by lMin, lMax and lInc.
///
///  \param    lMinVal  Minimum value
///  \param    lMaxVal  Maximum value
///  \param    lInc     Increment value (defines raster of valid values)
///  \param    lVal     Value to be tested
///
///  \return   \arg \c true  : If the value is within the specified range and on the raster
///            \arg \c false : Otherwise
// --------------------------------------------------------------------------------------------
static bool checkLimitViolation (int32_t lMinVal, int32_t lMaxVal, int32_t lInc, int32_t lVal)
{
    if (lVal < lMinVal) return false;
    if (lVal > lMaxVal) return false;

    // check if point is on raster
    if ((lVal - lMinVal) % lInc != 0) return false;

    return true;
}


// --------------------------------------------------------------------------------------------
///  \brief    This method checks if the given value dVal satisfies the conditions specified
///            by lMin, lMax and lInc.
///
///  \param    dMinVal  Minimum value
///  \param    dMaxVal  Maximum value
///  \param    dInc     Increment value (defines raster of valid values)
///  \param    dVal     Value to be tested
///
///  \return   \arg \c true  : If the value is within the specified range and on the raster
///            \arg \c false : Otherwise
// --------------------------------------------------------------------------------------------
static bool checkLimitViolation (double dMinVal, double dMaxVal, double dInc, double dVal)
{
    double dEpsilon = 1e-6 * dInc;

    if (dVal < dMinVal - dEpsilon) return false;
    if (dVal > dMaxVal + dEpsilon) return false;

    // check if point is on raster
    double dNearestGridPoint = dMinVal + dInc * floor ((dVal - dMinVal) / dInc + 0.5);
    if (fabs (dVal - dNearestGridPoint) > dEpsilon) return false;

    return true;
}


// --------------------------------------------------------------------------------------------
///  \brief    This method checks if the given limits are reasonable.
///
///            - Min val must be smaller than max val.
///            - Inc must be positive
///            - Default val must be in range [min; max]
///            - Max val must be on raster (defined by min and inc)
///            - Default val must be on raster
///
///  \param    MinVal      Minimum value
///  \param    MaxVal      Maximum value
///  \param    Inc         Increment value (defines raster of valid values)
///  \param    DefaultVal  Value to be tested
///
///  \throw    exception  If any of the listed conditions is not fulfilled
// --------------------------------------------------------------------------------------------
template <typename DATA_TYPE>
static void checkLimits (DATA_TYPE MinVal, DATA_TYPE MaxVal, DATA_TYPE Inc, DATA_TYPE DefaultVal, std::string& rsLabel)
{
    if (MinVal > MaxVal) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Given min val is larger than max val (UI element: " << rsLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Given min val is larger than max val!");
    }

    if (Inc <= 0) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Negative increment value (UI element: " << rsLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Negative increment value!");
    }

    // check that MaxVal is on raster defined by MinVal and Inc
    if (!checkLimitViolation (MinVal, MaxVal, Inc, MaxVal)) {
        UTRACE (Warn, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Max val is not on the raster defined by min val and increment (UI element: " << rsLabel << ")!");
    }

    // check that DefaultVal is on raster
    if (!checkLimitViolation (MinVal, MaxVal, Inc, DefaultVal)) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Default val is not on the raster defined by min val and increment (UI element: " << rsLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Default val is not on the raster defined by min val and increment!");
    }
}


// --------------------------------------------------------------------------------------------
///  \brief    This method calculates the nearest long value that fulfills the given limits.
///
///  \remarks  This method assumes that the given limits are valid.
///
///  \param    lMinVal  Minimum value
///  \param    lMaxVal  Maximum value
///  \param    lInc     Increment value (defines raster of valid values)
///  \param    lVal     Value to be corrected
///
///  \return   The corrected value
// --------------------------------------------------------------------------------------------
static long correctValueForLimits (long lMinVal, long lMaxVal, long lInc, long lVal)
{
    if (lVal < lMinVal) return lMinVal;
    if (lVal > lMaxVal) return lMaxVal;

    if (((lVal - lMinVal) % lInc) != 0) {
        return lMinVal + long( floor ( double(lVal - lMinVal) / double(lInc) + 0.5) ) * lInc;
    }

    return lVal;
}


// --------------------------------------------------------------------------------------------
///  \brief    This method calculates the nearest double value that fulfills the given limits.
///
///  \remarks  This method assumes that the given limits are valid.
///
///  \param    dMinVal  Minimum value
///  \param    dMaxVal  Maximum value
///  \param    dInc     Increment value (defines raster of valid values)
///  \param    dVal     Value to be corrected
///
///  \return   The corrected value
// --------------------------------------------------------------------------------------------
static double correctValueForLimits (double dMinVal, double dMaxVal, double dInc, double dVal)
{
    if (dVal < dMinVal) return dMinVal;
    if (dVal > dMaxVal) return dMaxVal;

    return dMinVal + floor ((dVal - dMinVal) / dInc + 0.5) * dInc;
}



// --------------------------------------------------------------------------------------------
///  \brief    Sub-classes of this class store information about created WIP parameters.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::WIPParam
{
public:
    WIPParam (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : m_lPos            (lPos)
        , m_lWIPMemBlockPos (lWIPMemBlockPos)
        , m_sLabel          ()
        , m_sToolTip        ()
        , m_sUnit           ()
#ifdef WIN32
        , m_pUILink         (NULL)
#endif
    {
        if (sLabel == NULL) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: WIP parameters must be initialized with a label (UI position: " << lPos << ")!");
            throw std::logic_error ("WIPParameterTool: WIP parameters must be initialized with a label!");
        }

        if ((lPos >= getMaxNumberOfWIPParameters()) || (lPos < 0)) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: WIP parameter position out of bounds (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: WIP parameter position out of bounds!");
        }

        m_sLabel = std::string (sLabel);

        if (m_sLabel.size() <= 0) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: WIP parameter labels must have at least one character (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: WIP parameter labels must have at least one character!");
        }

        if (sUnit != NULL) {
            m_sUnit = std::string (sUnit);
        }
    }

    virtual ~WIPParam (void) {}

#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    This method sets the WIP parameter to the default value in the given protocol.
    ///
    ///            This method must be implemented by sub-classes.
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    // --------------------------------------------------------------------------------------------
    virtual void setDefaultValueInProt (MrProtocolData::MrProtData& rMrProtData) const = 0;


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method checks whether the given UILink object is related to this WIP
    ///            parameter.
    ///
    ///  \remarks  Array parameters are represented by two UILink objects.
    ///
    ///  \param    rUILink  Reference to a UILink object
    ///
    ///  \return   \arg \c true  : If the given UILink object is related to this parameter
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool isParamFor (const MrUILinkBase& rUILink) const
    {
        if (&rUILink == m_pUILink) return true;

        return false;
    }
#endif


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method tests whether the currently set value is within the originally
    ///            specified limits. The method will be called for each registered parameter
    ///            in WIPParameterTool::prepare.
    ///
    ///            This method must be implemented by sub-classes.
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    ///
    ///  \return   \arg \c true  : If the current value is within limits
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const = 0;


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method reserves elements in the WIPMemBlock of the protocol for storing
    ///            the values (e.g. in alFree[]).
    ///
    ///            This method will be called when a new parameter is added.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool
    ///
    ///  \return   \arg \c true  : If the elements could be successfully reserved
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool reserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        return rTool.reserveLongsInWIPMemBlock (m_lWIPMemBlockPos);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method unreserves elements in the WIPMemBlock of the protocol (e.g. in
    ///            alFree[]).
    ///
    ///            This method should unreserve the elements reserved by the method
    ///            reserveElementsInWIPMemBlock.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool
    // --------------------------------------------------------------------------------------------
    virtual void unreserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        rTool.unreserveLongsInWIPMemBlock (m_lWIPMemBlockPos);
    }


    long           m_lPos;
    long           m_lWIPMemBlockPos;
    std::string    m_sLabel;
    std::string    m_sToolTip;
    std::string    m_sUnit;

#ifdef WIN32
    MrUILinkBase*  m_pUILink;
#endif

};




// --------------------------------------------------------------------------------------------
/// \brief    This class is the base class of LongParam, DoubleParam, which have in common that
///           there is a valid parameter range.
// --------------------------------------------------------------------------------------------
template <typename DATA_TYPE>
class DataParam : public WIPParameterTool::WIPParam
{
public:
    DataParam <DATA_TYPE> (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : WIPParam (lPos, lWIPMemBlockPos, sLabel, sUnit)
        , m_MinVal     (0)
        , m_MaxVal     (0)
        , m_Inc        (0)
        , m_DefaultVal (0)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the limits for this parameter.
    ///
    ///  \param    MinVal      Minimum value for the parameter
    ///  \param    MaxVal      Maximum value for the parameter
    ///  \param    Inc         Increment value (defines the grid spacing for adjustable values)
    ///  \param    DefaultVal  Default value (i.e. used for the default protocol)
    // --------------------------------------------------------------------------------------------
    virtual void setLimits (DATA_TYPE MinVal, DATA_TYPE MaxVal, DATA_TYPE Inc, DATA_TYPE DefaultVal)
    {
        checkLimits (MinVal, MaxVal, Inc, DefaultVal, m_sLabel);

        m_MinVal     = MinVal;
        m_MaxVal     = MaxVal;
        m_Inc        = Inc;
        m_DefaultVal = DefaultVal;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method returns a pointer to the member within the protocol which holds
    ///            the value of this WIP parameter. The method is used by get- and set-methods
    ///            which act directly on the protocol.
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    // --------------------------------------------------------------------------------------------
    virtual DATA_TYPE* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData) const = 0;


#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in WIPParam.
    // --------------------------------------------------------------------------------------------
    virtual void setDefaultValueInProt (MrProtocolData::MrProtData& rMrProtData) const
    {
        DATA_TYPE* pProtValue = getPointerToProtValue (rMrProtData);

        if (pProtValue != NULL) {
            *pProtValue = m_DefaultVal;
        }
    }
#endif


    // --------------------------------------------------------------------------------------------
    ///  \return   The stored default value
    // --------------------------------------------------------------------------------------------
    virtual DATA_TYPE getDefaultValue (void) const
    {
        return m_DefaultVal;
    }


    DATA_TYPE  m_MinVal;
    DATA_TYPE  m_MaxVal;
    DATA_TYPE  m_Inc;
    DATA_TYPE  m_DefaultVal;
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for long values.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::LongParam : public DataParam <int32_t>
{
public:
    LongParam (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : DataParam <int32_t> (lPos, lWIPMemBlockPos, sLabel, sUnit)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in DataParam.
    ///
    ///            Long parameters are stored in alFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual int32_t* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        return &(rMrProtData.getsWipMemBlock().getalFree()[m_lWIPMemBlockPos]);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkCurrentValue.
    ///
    ///  \see      WIPParam::checkCurrentValue
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        int32_t lCurVal = *getPointerToProtValue (rMrProtData);

        return checkLimitViolation (m_MinVal, m_MaxVal, m_Inc, lCurVal);
    }
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for double values.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::DoubleParam : public DataParam <double>
{
public:
    DoubleParam (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : DataParam <double> (lPos, lWIPMemBlockPos, sLabel, sUnit)
        , m_lPrecision (STD_PRECISION)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in DataParam.
    ///
    ///            Long parameters are stored in adFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual double* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        return &(rMrProtData.getsWipMemBlock().getadFree()[m_lWIPMemBlockPos]);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkCurrentValue.
    ///
    ///  \see      WIPParam::checkCurrentValue
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        double dCurVal = *getPointerToProtValue (rMrProtData);

        return checkLimitViolation (m_MinVal, m_MaxVal, m_Inc, dCurVal);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::reserveElementsInWIPMemBlock.
    ///
    ///            Reserves memory in adFree[] instead of alFree[].
    ///
    ///  \see      WIPParam::reserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual bool reserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        return rTool.reserveDoublesInWIPMemBlock (m_lWIPMemBlockPos);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::unreserveElementsInWIPMemBlock.
    ///
    ///            Unreserves memory in adFree[] instead of alFree[].
    ///
    ///  \see      WIPParam::unreserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual void unreserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        rTool.unreserveDoublesInWIPMemBlock (m_lWIPMemBlockPos);
    }


    long  m_lPrecision;
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for array parameters.
// --------------------------------------------------------------------------------------------
class ArrayParam : public WIPParameterTool::WIPParam
{
public:
    ArrayParam (long lPos, long lWIPMemBlockPos, long lSize, const char* sLabel, const char* sUnit)
        : WIPParam        (lPos, lWIPMemBlockPos, sLabel, sUnit)
        , m_lSize         (lSize)
#ifdef WIN32
        , m_pUILinkArrray (NULL)
#endif
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method checks if the given index is within bounds.
    ///
    ///  \param    lArrayPos  Position within the array
    ///
    ///  \throw    exception  If the value is not within bounds
    // --------------------------------------------------------------------------------------------
    virtual void checkIndex (long lArrayPos) const
    {
        if ((lArrayPos < 0) ||(lArrayPos >= m_lSize)) {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Array index out of bounds (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Array index out of bounds!");
        }
    }

#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::isParamFor. This method takes the two related UILink
    ///            objects into account (i.e. also the MrUILinkBase object).
    ///
    ///  \param    rUILink  Reference to a UILink object
    ///
    ///  \return   \arg \c true  : If the given UILink object is related to this parameter
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool isParamFor (const MrUILinkBase& rUILink) const
    {
        if (WIPParam::isParamFor (rUILink) || (&rUILink == m_pUILinkArrray)) return true;

        return false;
    }
#endif

    long  m_lSize;

#ifdef WIN32
    MrUILinkArray*  m_pUILinkArrray;
#endif
};





// --------------------------------------------------------------------------------------------
/// \brief    This class is the base class of LongParam, DoubleParam, which have in common that
///           there is a valid parameter range.
// --------------------------------------------------------------------------------------------
template <typename DATA_TYPE>
class NumericArrayParam : public ArrayParam
{
public:
    NumericArrayParam <DATA_TYPE> (long lPos, long lWIPMemBlockPos, long lSize, const char* sLabel, const char* sUnit)
        : ArrayParam   (lPos, lWIPMemBlockPos, lSize, sLabel, sUnit)
        , m_MinVal     (lSize)
        , m_MaxVal     (lSize)
        , m_Inc        (lSize)
        , m_DefaultVal (lSize)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the parameter limits for the given element within the array.
    ///
    ///  \param    lArrayPos   Position within the array
    ///  \param    MinVal      Minimum value for the parameter
    ///  \param    MaxVal      Maximum value for the parameter
    ///  \param    Inc         Increment value (defines the grid spacing for adjustable values)
    ///  \param    DefaultVal  Default value (i.e. used for the default protocol)
    // --------------------------------------------------------------------------------------------
    virtual void setLimits (long lArrayPos, DATA_TYPE MinVal, DATA_TYPE MaxVal, DATA_TYPE Inc, DATA_TYPE DefaultVal)
    {
        checkLimits (MinVal, MaxVal, Inc, DefaultVal, m_sLabel);

        checkIndex (lArrayPos);

        m_MinVal [lArrayPos]     = MinVal;
        m_MaxVal [lArrayPos]     = MaxVal;
        m_Inc [lArrayPos]        = Inc;
        m_DefaultVal [lArrayPos] = DefaultVal;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method returns a pointer to the member within the protocol which holds
    ///            the value of this WIP parameter. The method is used by get- and set-methods
    ///            which act directly on the protocol.
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    // --------------------------------------------------------------------------------------------
    virtual DATA_TYPE* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData, long lArrayPos) const = 0;


#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in WIPParam.
    // --------------------------------------------------------------------------------------------
    virtual void setDefaultValueInProt (MrProtocolData::MrProtData& rMrProtData) const
    {
        for (long lArrayPos = 0; lArrayPos < m_lSize; lArrayPos++) {
            DATA_TYPE* pProtValue = getPointerToProtValue (rMrProtData, lArrayPos);

            if (pProtValue != NULL) {
                *pProtValue = m_DefaultVal [lArrayPos];
            }
        }
    }
#endif


    // --------------------------------------------------------------------------------------------
    ///  \return   The stored default value for the element at position lArrayPos.
    // --------------------------------------------------------------------------------------------
    virtual DATA_TYPE getDefaultValue (long lArrayPos) const
    {
        checkIndex (lArrayPos);

        return m_DefaultVal [lArrayPos];
    }


    std::vector <DATA_TYPE>  m_MinVal;
    std::vector <DATA_TYPE>  m_MaxVal;
    std::vector <DATA_TYPE>  m_Inc;
    std::vector <DATA_TYPE>  m_DefaultVal;
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for long array values.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::LongArrayParam : public NumericArrayParam <int32_t>
{
public:
    LongArrayParam (long lPos, long lWIPMemBlockPos, long lSize, const char* sLabel, const char* sUnit)
        : NumericArrayParam <int32_t> (lPos, lWIPMemBlockPos, lSize, sLabel, sUnit)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in DataParam.
    ///
    ///            Long parameters are stored in alFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual int32_t* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData, long lArrayPos) const
    {
        checkIndex (lArrayPos);
        return &(rMrProtData.getsWipMemBlock().getalFree()[m_lWIPMemBlockPos + lArrayPos]);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkCurrentValue. Checks the limits for all elements.
    ///
    ///  \see      WIPParam::checkCurrentValue
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        for (long lArrayPos = 0; lArrayPos < m_lSize; lArrayPos++) {
            int32_t lCurVal = *getPointerToProtValue (rMrProtData, lArrayPos);

            if (!checkLimitViolation (m_MinVal [lArrayPos], m_MaxVal [lArrayPos], m_Inc [lArrayPos], lCurVal)) {
                return false;
            }
        }

        return true;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::reserveElementsInWIPMemBlock.
    ///
    ///            Reserves memory for all elements of the parameter array.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool instance
    ///
    ///  \see      WIPParam::reserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual bool reserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        return rTool.reserveLongsInWIPMemBlock (m_lWIPMemBlockPos, m_lSize);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::unreserveElementsInWIPMemBlock.
    ///
    ///            Unreserves memory for all elements of the parameter array.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool instance
    ///
    ///  \see      WIPParam::unreserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual void unreserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        rTool.unreserveLongsInWIPMemBlock (m_lWIPMemBlockPos, m_lSize);
    }
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for double array values.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::DoubleArrayParam : public NumericArrayParam <double>
{
public:
    DoubleArrayParam (long lPos, long lWIPMemBlockPos, long lSize, const char* sLabel, const char* sUnit)
        : NumericArrayParam <double> (lPos, lWIPMemBlockPos, lSize, sLabel, sUnit)
        , m_lPrecision (STD_PRECISION)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in DataParam.
    ///
    ///            Long parameters are stored in alFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual double* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData, long lArrayPos) const
    {
        checkIndex (lArrayPos);
        return &(rMrProtData.getsWipMemBlock().getadFree()[m_lWIPMemBlockPos + lArrayPos]);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkCurrentValue. Checks the limits for all elements.
    ///
    ///  \see      WIPParam::checkCurrentValue
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        for (long lArrayPos = 0; lArrayPos < m_lSize; lArrayPos++) {
            double dCurVal = *getPointerToProtValue (rMrProtData, lArrayPos);

            if (!checkLimitViolation (m_MinVal [lArrayPos], m_MaxVal [lArrayPos], m_Inc [lArrayPos], dCurVal)) {
                return false;
            }
        }

        return true;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::reserveElementsInWIPMemBlock.
    ///
    ///            Reserves memory for all elements of the parameter array.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool instance
    ///
    ///  \see      WIPParam::reserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual bool reserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        return rTool.reserveDoublesInWIPMemBlock (m_lWIPMemBlockPos, m_lSize);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Override for WIPParam::unreserveElementsInWIPMemBlock.
    ///
    ///            Unreserves memory for all elements of the parameter array.
    ///
    ///  \param    rTool  Reference to the WIPParameterTool instance
    ///
    ///  \see      WIPParam::unreserveElementsInWIPMemBlock
    // --------------------------------------------------------------------------------------------
    virtual void unreserveElementsInWIPMemBlock (WIPParameterTool& rTool) const
    {
        rTool.unreserveDoublesInWIPMemBlock (m_lWIPMemBlockPos, m_lSize);
    }


    long m_lPrecision;
};





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for bool values.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::BoolParam : public WIPParameterTool::WIPParam
{
public:
    BoolParam (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : WIPParam      (lPos, lWIPMemBlockPos, sLabel, sUnit)
        , m_bDefaultVal (false)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Bool parameters are stored in alFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual bool* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        // The reinterpret cast is required here, because there is no array of bool values
        // in WipMemBlock. One of the long values of alFree[] is used to store a bool instead.

        return reinterpret_cast <bool*> (&(rMrProtData.getsWipMemBlock().getalFree()[m_lWIPMemBlockPos]));
    }

#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in WIPParam.
    // --------------------------------------------------------------------------------------------
    virtual void setDefaultValueInProt (MrProtocolData::MrProtData& rMrProtData) const
    {
        // clear the value initially
        // (int32_t may theoretically have more bits than a bool value -> setting the bool value does
        //  set all of the bits of the int32_t value)
        rMrProtData.getsWipMemBlock().getalFree()[m_lWIPMemBlockPos] = 0;

        bool* pProtValue = getPointerToProtValue (rMrProtData);

        if (pProtValue != NULL) {
            *pProtValue = m_bDefaultVal;
        }
    }
#endif

    // --------------------------------------------------------------------------------------------
    ///  \return   The stored default value for the element at position lArrayPos.
    // --------------------------------------------------------------------------------------------
    virtual bool getDefaultValue (void) const
    {
        return m_bDefaultVal;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkLimitViolation. Returns always true.
    ///
    ///  \see      WIPParam::checkLimitViolation
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& /*rMrProtData*/) const
    {
        return true;
    }


    bool m_bDefaultVal;
};





WIPSelectionItem::WIPSelectionItem (int iIndex, const char* sOptionText, int32_t lStoredValue)
    : m_iIndex       (iIndex)
    , m_sOptionText  (sOptionText)
    , m_lStoredValue (lStoredValue)
{
}



WIPSelectionItem::WIPSelectionItem (int iIndex, std::string& sOptionText, int32_t lStoredValue)
    : m_iIndex       (iIndex)
    , m_sOptionText  (sOptionText)
    , m_lStoredValue (lStoredValue)
{
}



bool WIPSelectionItem::isText (const char* rsText) const
{
    return (m_sOptionText.compare (rsText)) == 0 ? true : false;
}



int WIPSelectionItem::getIndex (void) const
{
    return m_iIndex;
}



const std::string& WIPSelectionItem::getText (void) const
{
    return m_sOptionText;
}



int32_t WIPSelectionItem::getStoredValue (void) const
{
    return m_lStoredValue;
}





// --------------------------------------------------------------------------------------------
///  \brief    WIPParam for selections.
// --------------------------------------------------------------------------------------------
class WIPParameterTool::SelectionParam : public WIPParameterTool::WIPParam
{
public:
    SelectionParam (long lPos, long lWIPMemBlockPos, const char* sLabel, const char* sUnit)
        : WIPParam              (lPos, lWIPMemBlockPos, sLabel, sUnit)
        , m_aSelectionItems     ()
        , m_MapValueToId        ()
        , m_MapTextToId         ()
        , m_iDefaultOptionIndex (0)
    {
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Selection parameters are stored in alFree[] of the WipMemBlock.
    // --------------------------------------------------------------------------------------------
    virtual int32_t* getPointerToProtValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        return &(rMrProtData.getsWipMemBlock().getalFree() [m_lWIPMemBlockPos]);
    }

#ifdef WIN32
    // --------------------------------------------------------------------------------------------
    ///  \brief    This method implements the pure virtual method in WIPParam.
    // --------------------------------------------------------------------------------------------
    virtual void setDefaultValueInProt (MrProtocolData::MrProtData& rMrProtData) const
    {
        int32_t* pProtValue = getPointerToProtValue (rMrProtData);

        if (pProtValue != NULL) {
            *pProtValue = m_aSelectionItems [m_iDefaultOptionIndex].getStoredValue();
        }
    }
#endif

    // --------------------------------------------------------------------------------------------
    ///  \return   The index of the item that is selected by default
    // --------------------------------------------------------------------------------------------
    virtual int getDefaultIndex (void) const
    {
        return m_iDefaultOptionIndex;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Finds the item for the given value.
    ///
    ///  \param    lValue  Value to be searched for
    ///  \param    rpItem  Export reference for the pointer to the searched item
    ///
    ///  \return   \arg \c true  : If the acccording item could be found
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool getItemForValue (const int32_t lValue, const WIPSelectionItem*& rpItem) const
    {
        std::map <int32_t, int>::const_iterator Result = m_MapValueToId.find (lValue);

        if (Result != m_MapValueToId.end()) {
            int iOptionIndex = Result->second;

            // check if option index is within bounds
            if ((iOptionIndex >= 0) && (size_t(iOptionIndex) < m_aSelectionItems.size())) {
                 rpItem = &m_aSelectionItems [iOptionIndex];

                return true;
            }
        }

        return false;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Finds the item for the given option text.
    ///
    ///  \param    sText   Text of the option to be searched for
    ///  \param    rpItem  Export reference for the pointer to the searched item
    ///
    ///  \return   \arg \c true  : If the acccording item could be found
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    virtual bool getItemForText (const char* sText, const WIPSelectionItem*& rpItem) const
    {
        std::map <const char*, int>::const_iterator Result = m_MapTextToId.find (sText);

        if (Result != m_MapTextToId.end()) {
            int iOptionIndex = Result->second;

            // check if option index is within bounds
            if ((iOptionIndex >= 0) && (size_t(iOptionIndex) < m_aSelectionItems.size())) {
                rpItem = &m_aSelectionItems [iOptionIndex];

                return true;
            }
        }

        return false;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Implementation of WIPParam::checkLimitViolation.
    ///
    ///  \see      WIPParam::checkLimitViolation
    // --------------------------------------------------------------------------------------------
    virtual bool checkCurrentValue (MrProtocolData::MrProtData& rMrProtData) const
    {
        // get current value
        int32_t lValue = *getPointerToProtValue (rMrProtData);

        const WIPSelectionItem* pTmp;

        if (getItemForValue (lValue, pTmp)) {
            return true;
        }

        return false;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to this selection parameter.
    ///
    ///  \param    sOptionText   Option text
    ///  \param    lStoredValue  Value to be stored in the WIPMemBlock
    ///
    ///  \return   A reference to the added WIPSelectionItem instance.
    // --------------------------------------------------------------------------------------------
    WIPSelectionItem& addOption (const char* sOptionText, int32_t lStoredValue)
    {
        int iOptionIndex = (int)m_aSelectionItems.size();

        if (sOptionText == NULL) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Cannot add option. Text for option " << iOptionIndex << " is a NULL pointer (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Cannot add option. Text for option is NULL pointer!");
        }

        if (m_aSelectionItems.size() >= MAX_NUM_OF_OPTIONS) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Cannot add option. Number of options is already at maximum: " << iOptionIndex << " elements (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Cannot add option. Number of options is already at maximum!");
        }

        if (lStoredValue < 0) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: The given value (" << lStoredValue << ") is negative (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: The given value is negative!");
        }

        const WIPSelectionItem* pItem;
        if (getItemForValue (lStoredValue, pItem)) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: The value (" << lStoredValue << ") for the option \"" << sOptionText << "\" was already assigned to option \"" << pItem->getText() << "\" (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Cannot add option. The value was already assigned!");
        }

        if (getItemForText (sOptionText, pItem)) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: The text \"" << sOptionText << "\" for the option with value (" << lStoredValue << ") was already assigned to option with value (" << pItem->getStoredValue() << ") (UI element: " << m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Cannot add option. The option text was already assigned!");
        }

        m_aSelectionItems.push_back (WIPSelectionItem (iOptionIndex, sOptionText, lStoredValue));

        // update maps
        m_MapValueToId [lStoredValue] = iOptionIndex;
        m_MapTextToId [sOptionText]   = iOptionIndex;

        return m_aSelectionItems [iOptionIndex];
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to this selection parameter.
    ///
    ///            The value to be stored in the WIPMemBlock will be assigned automatically.
    ///
    ///  \param    sOptionText   Option text
    ///
    ///  \return   A reference to the added WIPSelectionItem instance.
    // --------------------------------------------------------------------------------------------
    WIPSelectionItem& addOption (const char* sOptionText)
    {
        return addOption (sOptionText, m_aSelectionItems.size());
    }


    std::vector <WIPSelectionItem>  m_aSelectionItems;
    std::map <int32_t, int>         m_MapValueToId;
    std::map <const char*, int>     m_MapTextToId;
    int                             m_iDefaultOptionIndex;

    static WIPSelectionItem         m_Undefined;
};

WIPSelectionItem  WIPParameterTool::SelectionParam::m_Undefined (-1, "undefined", -1);





const int WIPParameterTool::UTRACE_MARKER = 1;

long WIPParameterTool::lMaxNumberOfWIPParameters = 14;



long WIPParameterTool::getMaxNumberOfWIPParameters (void)
{
    return lMaxNumberOfWIPParameters;
}



// --------------------------------------------------------------------------------------------
///  \brief    This method converts the given WIPParam into the desired sub-class.
///
///  \throw    std::logic_error  If the given pointer is a NULL pointer
///  \throw    std::logic_error  If the UILink object is not a registered WIP parameter
///  \throw    std::logic_error  If the parameter is of the wrong type
// --------------------------------------------------------------------------------------------
template <typename WIP_PARAM_TYPE>
WIP_PARAM_TYPE* getAs (WIPParameterTool::WIPParam* pParam)
{
    if (pParam == NULL) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Parameter conversion failed - NULL pointer!");
        throw std::logic_error ("WIPParameterTool: Parameter conversion failed - NULL pointer!");
    }

    WIP_PARAM_TYPE* pResult = dynamic_cast <WIP_PARAM_TYPE*> (pParam);

    if (pResult == NULL) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: WIP parameter has the wrong type (UI element: " << pParam->m_sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: WIP parameter has the wrong type!");
    }

    return pResult;
}


#ifdef WIN32

// --------------------------------------------------------------------------------------------
///  \brief    This method gets the WIPParam for the given UILink object and converts it to
///            the desired sub-class.
///
///  \throw    std::logic_error  If the given pointer is a NULL pointer
///  \throw    std::logic_error  If the UILink object is not a registered WIP parameter
///  \throw    std::logic_error  If the parameter is of the wrong type
// --------------------------------------------------------------------------------------------
template <typename WIP_PARAM_TYPE>
WIP_PARAM_TYPE* getAs (MrUILinkBase* pUILink)
{
    if (pUILink == NULL) {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Parameter conversion failed - pointer to UILink object is NULL!");
        throw std::logic_error ("WIPParameterTool: Parameter conversion failed - pointer to UILink object is NULL!");
    }

    WIPParameterTool* pTool = WIPParameterTool::getInstanceFor (*pUILink);
    WIPParameterTool::WIPParam* pParam = pTool->getParam (*pUILink);
    return getAs <WIP_PARAM_TYPE> (pParam);
}



const char* WIPParameterTool::getUITag (long lPos)
{
    switch (lPos) {
        case  0 : return MR_TAG_SEQ_WIP1;
        case  1 : return MR_TAG_SEQ_WIP2;
        case  2 : return MR_TAG_SEQ_WIP3;
        case  3 : return MR_TAG_SEQ_WIP4;
        case  4 : return MR_TAG_SEQ_WIP5;
        case  5 : return MR_TAG_SEQ_WIP6;
        case  6 : return MR_TAG_SEQ_WIP7;
        case  7 : return MR_TAG_SEQ_WIP8;
        case  8 : return MR_TAG_SEQ_WIP9;
        case  9 : return MR_TAG_SEQ_WIP10;
        case 10 : return MR_TAG_SEQ_WIP11;
        case 11 : return MR_TAG_SEQ_WIP12;
        case 12 : return MR_TAG_SEQ_WIP13;
        case 13 : return MR_TAG_SEQ_WIP14;
        default :
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: UI tag for position " << lPos << " does not exist!");
            throw std::logic_error ("WIPParameterTool: UI tag for the given position does not exist!");
    }

    return NULL;
}

#endif


std::vector <WIPParameterTool*> WIPParameterTool::m_Instances;



WIPParameterTool::WIPParameterTool (const SeqIF& rSequence)
    : m_ParamMap              ()
    , m_pSequence             (&rSequence)
    , m_ReservedInLongArray   (K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE)
    , m_ReservedInDoubleArray (K_WIP_MEM_BLOCK_FREE_DBL_MAX_SIZE)
{
    // make sure that no other WIPParameterTool instance has been registered for this sequence
    if (getInstanceFor (rSequence) != NULL) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: There was already a WIPParameterTool instance registered for this sequence!");
        throw std::logic_error ("WIPParameterTool: There was already a WIPParameterTool instance registered for this sequence!");
    }

    m_Instances.push_back (this);

    unsigned int iI;

    for (iI = 0; iI < m_ReservedInLongArray.size(); iI++) {
        m_ReservedInLongArray [iI] = false;
    }

    for (iI = 0; iI < m_ReservedInDoubleArray.size(); iI++) {
        m_ReservedInDoubleArray [iI] = false;
    }

    // The last element of the long array is already used for marking whether
    // the default values have allready been set.
    // (see method getDefaultValuesInitialized (...))
    m_ReservedInLongArray [K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE - 1] = true;
}



WIPParameterTool::~WIPParameterTool (void)
{
    // delete stored WIP parameter data
    ParamMapIter DataIter;
    for (DataIter = m_ParamMap.begin(); DataIter != m_ParamMap.end(); DataIter++) {
        delete (DataIter->second);
    }

    // delete this tool instance from static list of instances
    std::vector <WIPParameterTool*>::iterator InstIter;
    for (InstIter = m_Instances.begin(); InstIter != m_Instances.end(); InstIter++) {
        if (*InstIter == this) {
            m_Instances.erase (InstIter);
            break;
        }
    }
}



bool WIPParameterTool::createLongParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit,
    int32_t     lMin,
    int32_t     lMax,
    int32_t     lInc,
    int32_t     lDef)
{
    LongParam* pParam = new LongParam (lPos, lWIPMemBlockPos, sLabel, sUnit);
    pParam->setLimits (lMin, lMax, lInc, lDef);

#ifdef WIN32
    LINK_LONG_TYPE* pUILink = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        pUILink = dynamic_cast <LINK_LONG_TYPE*> (getParam (lPos)->m_pUILink);
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILink = _create <LINK_LONG_TYPE> (&rSeqLim, getUITag (lPos));
    }
    
    // register standard UI handler methods
    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler   (stdGetValue);
        pUILink->registerSetValueHandler   (stdSetValue);
        pUILink->registerGetLimitsHandler  (stdGetLimits);
        pUILink->registerGetUnitIdHandler  (stdGetUnitId);
        pUILink->registerGetLabelIdHandler (stdGetLabelId);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



bool WIPParameterTool::createLongArrayParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    long        lSize,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit,
    int32_t     lMin,
    int32_t     lMax,
    int32_t     lInc,
    int32_t     lDef)
{
    LongArrayParam* pParam = new LongArrayParam (lPos, lWIPMemBlockPos, lSize, sLabel, sUnit);

    // set specified limits for all array elements
    for (long lArrayPos = 0; lArrayPos < lSize; lArrayPos++) {
        pParam->setLimits (lArrayPos, lMin, lMax, lInc, lDef);
    }

#ifdef WIN32
    MrUILinkArray*  pUILinkArray = NULL;
    LINK_LONG_TYPE* pUILink      = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        LongArrayParam* pPrevParam = getAs <LongArrayParam> (getParam (lPos));

        pUILink      = dynamic_cast <LINK_LONG_TYPE*> (pPrevParam->m_pUILink);
        pUILinkArray = pPrevParam->m_pUILinkArrray;
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILinkArray = _createArray <LINK_LONG_TYPE> (&rSeqLim, getUITag (lPos), lSize, pUILink);
    }
    
    // register standard UI handler methods
    if (pUILinkArray != NULL) {
        pParam->m_pUILinkArrray = pUILinkArray;

        pUILinkArray->registerSizeHandler (stdGetArraySize);
    }

    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler   (stdGetValueInArray);
        pUILink->registerSetValueHandler   (stdSetValueInArray);
        pUILink->registerGetLimitsHandler  (stdGetLimitsInArray);
        pUILink->registerGetUnitIdHandler  (stdGetUnitId);
        pUILink->registerGetLabelIdHandler (stdGetLabelIdInArray);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



void WIPParameterTool::setLimits (
    long    lPos,
    long    lArrayPos,
    int32_t lMin,
    int32_t lMax,
    int32_t lInc,
    int32_t lDef)
{
    LongArrayParam* pParam = getAs <LongArrayParam> (getParam (lPos));

    pParam->setLimits (lArrayPos, lMin, lMax, lInc, lDef);
}



bool WIPParameterTool::createDoubleParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit,
    unsigned    uPrecision,
    double      dMin,
    double      dMax,
    double      dInc,
    double      dDef)
{
    DoubleParam* pParam = new DoubleParam (lPos, lWIPMemBlockPos, sLabel, sUnit);

    pParam->setLimits (dMin, dMax, dInc, dDef);
    pParam->m_lPrecision = uPrecision;

#ifdef WIN32
    LINK_DOUBLE_TYPE* pUILink = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        pUILink = dynamic_cast <LINK_DOUBLE_TYPE*> (getParam (lPos)->m_pUILink);
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILink = _create <LINK_DOUBLE_TYPE> (&rSeqLim, getUITag (lPos));
    }

    // register standard UI handler methods
    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler     (stdGetValue);
        pUILink->registerSetValueHandler     (stdSetValue);
        pUILink->registerGetLimitsHandler    (stdGetLimits);
        pUILink->registerGetPrecisionHandler (stdGetPrecision);
        pUILink->registerGetUnitIdHandler    (stdGetUnitId);
        pUILink->registerGetLabelIdHandler   (stdGetLabelId);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



bool WIPParameterTool::createDoubleArrayParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    long        lSize,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit,
    unsigned    uPrecision,
    double      dMin,
    double      dMax,
    double      dInc,
    double      dDef)
{
    DoubleArrayParam* pParam = new DoubleArrayParam (lPos, lWIPMemBlockPos, lSize, sLabel, sUnit);

    // set specified limits for all array elements
    for (long lArrayPos = 0; lArrayPos < lSize; lArrayPos++) {
        pParam->setLimits (lArrayPos, dMin, dMax, dInc, dDef);
    }

    pParam->m_lPrecision = uPrecision;

#ifdef WIN32
    MrUILinkArray*  pUILinkArray = NULL;
    LINK_DOUBLE_TYPE* pUILink    = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        DoubleArrayParam* pPrevParam = getAs <DoubleArrayParam> (getParam (lPos));

        pUILink      = dynamic_cast <LINK_DOUBLE_TYPE*> (pPrevParam->m_pUILink);
        pUILinkArray = pPrevParam->m_pUILinkArrray;
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILinkArray = _createArray <LINK_DOUBLE_TYPE> (&rSeqLim, getUITag (lPos), lSize, pUILink);
    }
    
    // register standard UI handler methods
    if (pUILinkArray != NULL) {
        pParam->m_pUILinkArrray = pUILinkArray;

        pUILinkArray->registerSizeHandler (stdGetArraySize);
    }

    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler   (stdGetValueInArray);
        pUILink->registerSetValueHandler   (stdSetValueInArray);
        pUILink->registerGetLimitsHandler  (stdGetLimitsInArray);
        pUILink->registerGetUnitIdHandler  (stdGetUnitId);
        pUILink->registerGetLabelIdHandler (stdGetLabelIdInArray);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



void WIPParameterTool::setLimits (
    long   lPos,
    long   lArrayPos,
    double dMin,
    double dMax,
    double dInc,
    double dDef)
{
    DoubleArrayParam* pParam = getAs <DoubleArrayParam> (getParam (lPos));

    pParam->setLimits (lArrayPos, dMin, dMax, dInc, dDef);
}



bool WIPParameterTool::createBoolParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    SeqLim&     rSeqLim,
    const char* sLabel,
    bool        bDef)
{
    BoolParam* pParam = new BoolParam (lPos, lWIPMemBlockPos, sLabel, NULL);
    pParam->m_bDefaultVal = bDef;

#ifdef WIN32
    LINK_BOOL_TYPE* pUILink = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        pUILink = dynamic_cast <LINK_BOOL_TYPE*> (getParam (lPos)->m_pUILink);
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILink = _create <LINK_BOOL_TYPE> (&rSeqLim, getUITag (lPos));
    }

    // register standard UI handler methods
    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler   (stdGetValue);
        pUILink->registerSetValueHandler   (stdSetValue);
        pUILink->registerGetOptionsHandler (stdGetOptions);
        pUILink->registerGetUnitIdHandler  (stdGetUnitId);
        pUILink->registerGetLabelIdHandler (stdGetLabelId);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



bool WIPParameterTool::createSelectionParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit,
    long        lNumberOfOptions,
    const char* asOptions[],
    int         iDefaultOptionId)
{
    SelectionParam* pParam = new SelectionParam (lPos, lWIPMemBlockPos, sLabel, sUnit);

    if (lNumberOfOptions < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Number of options is negative (UI element: " << sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Number of options is negative!");
    }

    if (lNumberOfOptions > MAX_NUM_OF_OPTIONS) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Number of options too large: " << lNumberOfOptions << " (UI element: " << sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Number of options is too large!");
    }

    if ((asOptions == NULL) && (lNumberOfOptions != 0)) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Array of options is NULL pointer (UI element: " << sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Array of options is NULL pointer!");
    }

    if (asOptions != NULL) {
        for (int iI = 0; iI < lNumberOfOptions; iI++) {
            pParam->addOption (asOptions [iI]);
        }
    }

    pParam->m_iDefaultOptionIndex = iDefaultOptionId;
    
#ifdef WIN32
    LINK_SELECTION_TYPE* pUILink = NULL;

    // check if a UI link object has already been registered
    if (isParamRegistered (lPos)) {
        pUILink = dynamic_cast <LINK_SELECTION_TYPE*> (getParam (lPos)->m_pUILink);
    }

    // try to create a new UI link object if it did not exist already
    if (pUILink == NULL) {
        pUILink = _create <LINK_SELECTION_TYPE> (&rSeqLim, getUITag (lPos));
    }

    // register standard UI handler methods
    if (pUILink != NULL) {
        pParam->m_pUILink = pUILink;

        pUILink->registerGetValueHandler   (stdGetValue);
        pUILink->registerSetValueHandler   (stdSetValue);
        pUILink->registerFormatHandler     (stdFormat);
        pUILink->registerGetOptionsHandler (stdGetOptions);
        pUILink->registerGetUnitIdHandler  (stdGetUnitId);
        pUILink->registerGetLabelIdHandler (stdGetLabelId);

    } else {
        if (!rSeqLim.isContextNormal()) {
            UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Creating UI element failed (UI element: " << sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Creating UI element failed!");
        }
    }
#endif

    addParam (*pParam);

    return true;
}



bool WIPParameterTool::createSelectionParameter (
    long        lPos,
    long        lWIPMemBlockPos,
    SeqLim&     rSeqLim,
    const char* sLabel,
    const char* sUnit)
{
    return createSelectionParameter (
        lPos,
        lWIPMemBlockPos,
        rSeqLim,
        sLabel,
        sUnit,
        0,
        NULL,
        0
    );
}



void WIPParameterTool::addOption (long lPos, const char* sOptionText, int32_t lStoredValue)
{
    SelectionParam* pParam = getAs <SelectionParam> (getParam (lPos));

    pParam->addOption (sOptionText, lStoredValue);
}



void WIPParameterTool::addOption (long lPos, const char* sOptionText)
{
    SelectionParam* pParam = getAs <SelectionParam> (getParam (lPos));

    pParam->addOption (sOptionText);
}



void WIPParameterTool::addDefaultOption (long lPos, const char* sOptionText)
{
    SelectionParam* pParam = getAs <SelectionParam> (getParam (lPos));

    pParam->m_iDefaultOptionIndex = pParam->addOption (sOptionText).getIndex();
}



void WIPParameterTool::addDefaultOption (long lPos, const char* sOptionText, int32_t lStoredValue)
{
    SelectionParam* pParam = getAs <SelectionParam> (getParam (lPos));

    pParam->m_iDefaultOptionIndex = pParam->addOption (sOptionText, lStoredValue).getIndex();
}



bool WIPParameterTool::reserveLongsInWIPMemBlock (long lStartPos, long lNumberOfElements)
{
    long lCurPos;

    if (lStartPos < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to reserve element with negative index in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to reserve element with negative index in WIPMemBlock!");
    }

    if (lNumberOfElements < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to reserve negative number of elements in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to reserve negative number of elements in WIPMemBlock!");
    }

    if (lStartPos + lNumberOfElements > K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE) {
        return false;
    }

    // check if any of the elements is already reserved
    for (lCurPos = lStartPos; lCurPos < lStartPos + lNumberOfElements; lCurPos++) {
        if (m_ReservedInLongArray [lCurPos]) return false;
    }

    // reserve elements
    for (lCurPos = lStartPos; lCurPos < lStartPos + lNumberOfElements; lCurPos++) {
        m_ReservedInLongArray [lCurPos] = true;
    }

    return true;
}



bool WIPParameterTool::reserveDoublesInWIPMemBlock (long lStartPos, long lNumberOfElements)
{
    long lCurPos;

    if (lStartPos < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to reserve element with negative index in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to reserve element with negative index in WIPMemBlock!");
    }

    if (lNumberOfElements < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to reserve negative number of elements in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to reserve negative number of elements in WIPMemBlock!");
    }

    if (lStartPos + lNumberOfElements > K_WIP_MEM_BLOCK_FREE_DBL_MAX_SIZE) {
        return false;
    }

    // check if any of the elements is already reserved
    for (lCurPos = lStartPos; lCurPos < lStartPos + lNumberOfElements; lCurPos++) {
        if (m_ReservedInDoubleArray [lCurPos]) return false;
    }

    // reserve elements
    for (lCurPos = lStartPos; lCurPos < lStartPos + lNumberOfElements; lCurPos++) {
        m_ReservedInDoubleArray [lCurPos] = true;
    }

    return true;
}



void WIPParameterTool::unreserveLongsInWIPMemBlock (long lStartPos, long lNumberOfElements)
{
    if (lStartPos < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to unreserve element with negative index in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to unreserve element with negative index in WIPMemBlock!");
    }

    if (lNumberOfElements < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to unreserve negative number of elements in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to unreserve negative number of elements in WIPMemBlock!");
    }

    long lTo = std::min <long> (lStartPos + lNumberOfElements, K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE);

    // unreserve elements
    for (long lCurPos = lStartPos; lCurPos < lTo; lCurPos++) {
        m_ReservedInLongArray [lCurPos] = false;
    }
}



void WIPParameterTool::unreserveDoublesInWIPMemBlock (long lStartPos, long lNumberOfElements)
{
    if (lStartPos < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to unreserve element with negative index in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to unreserve element with negative index in WIPMemBlock!");
    }

    if (lNumberOfElements < 0) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Tried to unreserve negative number of elements in WIPMemBlock!");
        throw std::logic_error ("WIPParameterTool: Tried to unreserve negative number of elements in WIPMemBlock!");
    }

    long lTo = std::min <long> (lStartPos + lNumberOfElements, K_WIP_MEM_BLOCK_FREE_DBL_MAX_SIZE);

    // unreserve elements
    for (long lCurPos = lStartPos; lCurPos < lTo; lCurPos++) {
        m_ReservedInDoubleArray [lCurPos] = false;
    }
}



// The default values for WIP parameters can only be set if the context is
// ContextPrepForMrProtUpdate. Unfortunately, there is no way to tell the
// framework directly to start a prepare with this context. Instead, the
// first sequence prepare must return SEQU_ERROR to tell the sequence that
// a second prepare call with ContextPrepForMrProtUpdate is required.
//
// Therefore, this method will return false, if:
//   - the default values have not yet been set
//   - and the context is not ContextPrepForMrProtUpdate
//
bool WIPParameterTool::prepare (MrProt& rMrProt, SeqLim& rSeqLim)
{
    bool bSuccess = true;

#ifdef WIN32
    if (!getDefaultValuesInitialized (rMrProt)) {
        if (rSeqLim.isContextPrepForMrProtUpdate()) {

            // set default values
            ParamMapIter DataIter;
            for (DataIter = m_ParamMap.begin(); DataIter != m_ParamMap.end(); DataIter++) {
                DataIter->second->setDefaultValueInProt (*(rMrProt.getProtData().get()));
            }

            setDefaultValuesInitialized (rMrProt, true);
        } else {
            bSuccess = false;
        }
    }
#endif

    // check if current values fulfill the specified limits
    ParamMapIter DataIter;
    for (DataIter = m_ParamMap.begin(); DataIter != m_ParamMap.end(); DataIter++) {
        if (!DataIter->second->checkCurrentValue (*(rMrProt.getProtData().get()))) {
            return false;
        }
    }


    return bSuccess;
}



WIPParameterTool* WIPParameterTool::getInstanceFor (const SeqIF& rSequence)
{
    // Todo: Replace this method by calling a dedicated getMethod of the sequence which should
    //       return the WIPParameterTool instance associated with this sequence
    std::vector <WIPParameterTool*>::iterator iter;
    for (iter = m_Instances.begin(); iter != m_Instances.end(); iter++) {
        if ((*iter)->m_pSequence == &rSequence) {
            return *iter;
        }
    }

    return NULL;
}


#ifdef WIN32

WIPParameterTool* WIPParameterTool::getInstanceFor (const MrUILinkBase& rUILink, bool bExceptionIfNotExisting)
{
    WIPParameterTool* pTool = getInstanceFor (*(rUILink.sequence().getSeq()));

    if ((pTool == NULL) && bExceptionIfNotExisting) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: No WIPParameterTool instance found for given UILink (UI tag: " << rUILink.GetTag() << ")!");
        throw std::logic_error ("WIPParameterTool: No WIPParameter tool instance found for given UILink!");
    }

    return pTool;
}



// --------------------------------------------------------------------------------------------
///  \brief    Standard method for reading WIP parameter values from the UI. This method
///            will call the getValue handler of the associated UI link object.
///
///  \param    rTool   WIPParameterTool instance
///  \param    lPos    Position on the parameter card
///
///  \return   The WIP parameter value
///
///  \throw    exception  If no WIP parameter is registered at the given position
///  \throw    exception  If the WIP parameter does not have the correct type
///  \throw    exception  If the UI link object does not exist or has the wrong type
// --------------------------------------------------------------------------------------------
template <typename UI_LINK_TYPE, typename WIP_PARAM_TYPE, typename DATA_TYPE>
static DATA_TYPE getValue (const WIPParameterTool& rTool, long lPos, long lArrayPos = 0)
{
    DATA_TYPE Result = 0;

    WIPParameterTool::WIPParam* pParam = rTool.getParam (lPos);

    if (pParam->m_pUILink != NULL) {
        UI_LINK_TYPE* pUILink = dynamic_cast <UI_LINK_TYPE*> (pParam->m_pUILink);

        if (pUILink != NULL) {
            Result = pUILink->value (lArrayPos);
        } else {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Getting value failed - UI Link has wrong type (UI element: " << pParam->m_sLabel << ")!");
            throw std::logic_error ("WIPParameterTool: Getting value failed - UI Link has wrong type!");
        }
    } else {
        WIP_PARAM_TYPE* pCastParam = dynamic_cast <WIP_PARAM_TYPE*> (pParam);

        if (pCastParam != NULL) {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Getting value failed - UI element does not exist (UI position: " << lPos << ")!");
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Hint: In sequence-prepare() you must call the getValue()-method with protocol!");
            throw std::logic_error ("WIPParameterTool: Getting value failed - UI element does not exist!");
        } else {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Getting value failed - WIP parameter has wrong type (UI position: " << lPos << ")!");
            throw std::logic_error ("WIPParameterTool: Getting value failed - WIP parameter has wrong type!");
        }
    }

    return Result;
}



int32_t WIPParameterTool::getLongValue (long lPos) const
{
    return static_cast <int32_t> (getValue <LINK_LONG_TYPE, LongParam, long> (*this, lPos));
}



int32_t WIPParameterTool::getLongValue (long lPos, long lArrayPos) const
{
    return static_cast <int32_t> (getValue <LINK_LONG_TYPE, LongParam, long> (*this, lPos, lArrayPos));
}



double WIPParameterTool::getDoubleValue (long lPos) const
{
    return getValue <LINK_DOUBLE_TYPE, DoubleParam, double> (*this, lPos);
}



double WIPParameterTool::getDoubleValue (long lPos, long lArrayPos) const
{
    return getValue <LINK_DOUBLE_TYPE, DoubleParam, double> (*this, lPos, lArrayPos);
}



bool WIPParameterTool::getBoolValue (long lPos) const
{
    return getValue <LINK_BOOL_TYPE, BoolParam, bool> (*this, lPos);
}



int32_t WIPParameterTool::getSelectionValue (long lPos) const
{
    return getSelectedItem (lPos).getStoredValue();
}



const WIPSelectionItem& WIPParameterTool::getSelectedItem (long lPos) const
{
    unsigned int uiResourceId = getValue <LINK_SELECTION_TYPE, SelectionParam, unsigned int> (*this, lPos);

    SelectionParam* pParam = dynamic_cast <SelectionParam*> (getParam (lPos));

    if (pParam != NULL) {
        // The UI works with resource IDs. Therefore, the option index must be translated.
        // The GET- and SET_MODIFIER-methods perform the conversion
        unsigned int uiOptionIndex = GET_MODIFIER (uiResourceId);

        if (uiOptionIndex < pParam->m_aSelectionItems.size()) {
            return pParam->m_aSelectionItems [uiOptionIndex];
        }
    }

    return SelectionParam::m_Undefined;
}

#endif





template <typename WIP_PARAM_TYPE, typename DATA_TYPE>
class GetValue {
public:
    // --------------------------------------------------------------------------------------------
    ///  \brief    Standard method for getting WIP parameter values directly from the protocol.
    ///
    ///  \param    rTool    WIPParameterTool instance
    ///  \param    rMrProt  WIPParameterTool instance
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The WIP parameter value
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the WIP parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    static DATA_TYPE get (const WIPParameterTool& rTool, const MrProt& rMrProt, long lPos)
    {
        DATA_TYPE Result = 0;

        WIP_PARAM_TYPE* pParam = dynamic_cast <WIP_PARAM_TYPE*> (rTool.getParam (lPos));

        if (pParam != NULL) {
            Result = *(pParam->getPointerToProtValue (*(rMrProt.getProtData())));
        } else {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Getting value failed - WIP parameter has wrong type (UI position: " << lPos << ")!");
            throw std::logic_error ("WIPParameterTool: Getting value failed - WIP parameter has wrong type!");
        }

        return Result;
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Standard method for getting WIP array parameter values directly from the
    ///            protocol.
    ///
    ///  \param    rTool      WIPParameterTool instance
    ///  \param    rMrProt    WIPParameterTool instance
    ///  \param    lPos       Position on the parameter card
    ///  \param    lArrayPos  Position within the array
    ///
    ///  \return   The WIP parameter value
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the WIP parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    static DATA_TYPE getInArray (const WIPParameterTool& rTool, const MrProt& rMrProt, long lPos, long lArrayPos)
    {
        DATA_TYPE Result = 0;

        WIP_PARAM_TYPE* pParam = dynamic_cast <WIP_PARAM_TYPE*> (rTool.getParam (lPos));

        if (pParam != NULL) {
            Result = *(pParam->getPointerToProtValue (*(rMrProt.getProtData()), lArrayPos));
        } else {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Getting value failed - WIP parameter has wrong type (UI position: " << lPos << ")!");
            throw std::logic_error ("WIPParameterTool: Getting value failed - WIP parameter has wrong type!");
        }

        return Result;
    }
};





int32_t WIPParameterTool::getLongValue (const MrProt& rMrProt, long lPos) const
{
    return GetValue <LongParam, int32_t>::get (*this, rMrProt, lPos);
}



int32_t WIPParameterTool::getLongValue (const MrProt& rMrProt, long lPos, long lArrayPos) const
{
    return GetValue <LongArrayParam, int32_t>::getInArray (*this, rMrProt, lPos, lArrayPos);
}



double WIPParameterTool::getDoubleValue (const MrProt& rMrProt, long lPos) const
{
    return GetValue <DoubleParam, double>::get (*this, rMrProt, lPos);
}



double WIPParameterTool::getDoubleValue (const MrProt& rMrProt, long lPos, long lArrayPos) const
{
    return GetValue <DoubleArrayParam, double>::getInArray (*this, rMrProt, lPos, lArrayPos);
}



bool WIPParameterTool::getBoolValue (const MrProt& rMrProt, long lPos) const
{
    return GetValue <BoolParam, bool>::get (*this, rMrProt, lPos);
}



int32_t WIPParameterTool::getSelectionValue (const MrProt& rMrProt, long lPos) const
{
    return static_cast <int32_t> (GetValue <SelectionParam, unsigned int>::get (*this, rMrProt, lPos));
}



const WIPSelectionItem& WIPParameterTool::getSelectedItem (const MrProt& rMrProt, long lPos) const
{
    int32_t lOptionValue = static_cast <int32_t> (GetValue <SelectionParam, unsigned int>::get (*this, rMrProt, lPos));

    SelectionParam* pParam = dynamic_cast <SelectionParam*> (getParam (lPos));

    if (pParam != NULL) {
        const WIPSelectionItem* pItem = NULL;

        if (pParam->getItemForValue (lOptionValue, pItem)) {
            return *pItem;
        }
    }

    return SelectionParam::m_Undefined;
}


#ifdef WIN32

// --------------------------------------------------------------------------------------------
///  \brief    Standard method for setting WIP parameter values (only for the host side). This
///            method will call directly the registered setValue handler.
///
///  \param    rTool   WIPParameterTool instance
///  \param    lPos    Position on the parameter card
///  \param    NewVal  New value
///
///  \throw    exception  If no WIP parameter is registered at the given position
///  \throw    exception  If the WIP parameter does not have the correct type
///  \throw    exception  If the UI link object does not exist or has the wrong type
// --------------------------------------------------------------------------------------------
template <typename UI_LINK_TYPE, typename WIP_PARAM_TYPE, typename DATA_TYPE>
static bool setValue (const WIPParameterTool& rTool, long lPos, DATA_TYPE NewVal, long lArrayIndex = 0)
{
    WIPParameterTool::WIPParam* pParam = rTool.getParam (lPos);

    if (pParam->m_pUILink != NULL) {
        UI_LINK_TYPE* pUILink = dynamic_cast <UI_LINK_TYPE*> (pParam->m_pUILink);

        if (pUILink != NULL) {
            pUILink->value (NewVal, lArrayIndex);
        } else {
            UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Setting value failed - UI Link has wrong type (UI position: " << lPos << ")!");
            throw std::logic_error ("WIPParameterTool: Setting value failed - UI Link has wrong type!");
        }
    } else {
        UTRACE (Error, WIPParameterTool::UTRACE_MARKER, "WIPParameterTool: Setting value failed - UI Link not yet initialized (UI position: " << lPos << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - UI Link not yet initialized!");
    }

    return true;
}



bool WIPParameterTool::setLongValue (long lPos, int32_t lNewVal) const
{
    return setValue <LINK_LONG_TYPE, LongParam, long> (*this, lPos, static_cast <long> (lNewVal));
}



bool WIPParameterTool::setLongValue (long lPos, long lArrayIndex, int32_t lNewVal) const
{
    return setValue <LINK_LONG_TYPE, LongArrayParam, long> (*this, lPos, static_cast <long> (lNewVal), lArrayIndex);
}



bool WIPParameterTool::setDoubleValue (long lPos, double dNewVal) const
{
    return setValue <LINK_DOUBLE_TYPE, DoubleParam, double> (*this, lPos, dNewVal);
}



bool WIPParameterTool::setDoubleValue (long lPos, long lArrayIndex, double dNewVal) const
{
    return setValue <LINK_DOUBLE_TYPE, DoubleArrayParam, double> (*this, lPos, dNewVal, lArrayIndex);
}



bool WIPParameterTool::setBoolValue (long lPos, bool bNewVal) const
{
    return setValue <LINK_BOOL_TYPE, BoolParam, bool> (*this, lPos, bNewVal);
}



bool WIPParameterTool::setSelectionValue (long lPos, int32_t lNewVal) const
{
    WIPParam* pParam = getParam (lPos);

    SelectionParam* pSelectionParam = dynamic_cast <SelectionParam*> (pParam);

    if (pSelectionParam == NULL) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - WIP parameter has wrong type (UI position: " << lPos << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - WIP parameter has wrong type!");
    }

    const WIPSelectionItem* pItem;

    if (!pSelectionParam->getItemForValue (lNewVal, pItem)) {
         UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - Option with value (" << lNewVal << ") does not exist (UI element: " << pSelectionParam->m_sLabel << ")!");
         throw std::logic_error ("WIPParameterTool: Setting value failed - Option does not exist!");
    }

    LINK_SELECTION_TYPE* pUILinkSelection = dynamic_cast <LINK_SELECTION_TYPE*> (pParam->m_pUILink);

    if (pUILinkSelection == NULL) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - UI Link has wrong type (UI element: " << pSelectionParam->m_sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - UI Link has wrong type!");
    }

    // The UI works with resource IDs. Therefore, the option index must be translated.
    // The GET- and SET_MODIFIER-methods perform the conversion
    unsigned int uiResourceId = MRI_STD_STRING;
    SET_MODIFIER (uiResourceId, static_cast <unsigned char> (pItem->getIndex()));
    pUILinkSelection->value (uiResourceId, 0);

    return true;
}



bool WIPParameterTool::setSelectionText (long lPos, const char* sOptionText) const
{
    WIPParam* pParam = getParam (lPos);

    SelectionParam* pSelectionParam = dynamic_cast <SelectionParam*> (pParam);

    if (pSelectionParam == NULL) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - WIP parameter has wrong type (UI position: " << lPos << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - WIP parameter has wrong type!");
    }

    const WIPSelectionItem* pItem;

    if (!pSelectionParam->getItemForText (sOptionText, pItem)) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - Option \"" << sOptionText << "\" does not exist (UI element: " << pSelectionParam->m_sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - Option does not exist!");
    }

    LINK_SELECTION_TYPE* pUILinkSelection = dynamic_cast <LINK_SELECTION_TYPE*> (pParam->m_pUILink);

    if (pUILinkSelection == NULL) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Setting value failed - UI Link has wrong type (UI element: " << pSelectionParam->m_sLabel << ")!");
        throw std::logic_error ("WIPParameterTool: Setting value failed - UI Link has wrong type!");
    }

    // The UI works with resource IDs. Therefore, the option index must be translated.
    // The GET- and SET_MODIFIER-methods perform the conversion
    unsigned int uiResourceId = MRI_STD_STRING;
    SET_MODIFIER (uiResourceId, static_cast <unsigned char> (pItem->getIndex()));
    pUILinkSelection->value (uiResourceId, 0);

    return true;
}



long WIPParameterTool::stdGetArraySize (MrUILinkArray* const pUILinkArray, long)
{
    ArrayParam* pParam = getAs <ArrayParam> (pUILinkArray);
    return pParam->m_lSize;
}



long WIPParameterTool::stdGetValue (LINK_LONG_TYPE* const pUILink, long) 
{
    LongParam* pParam = getAs <LongParam> (pUILink);
    return static_cast <long> (*(pParam->getPointerToProtValue (pUILink->prot())));
}



long WIPParameterTool::stdGetValueInArray (LINK_LONG_TYPE* const pUILink, long lArrayPos) 
{
    LongArrayParam* pParam = getAs <LongArrayParam> (pUILink);
    return static_cast <long> (*(pParam->getPointerToProtValue (pUILink->prot(), lArrayPos)));
}



double WIPParameterTool::stdGetValue (LINK_DOUBLE_TYPE* const pUILink, long) 
{
    DoubleParam* pParam = getAs <DoubleParam> (pUILink);
    return *(pParam->getPointerToProtValue (pUILink->prot()));
}



double WIPParameterTool::stdGetValueInArray (LINK_DOUBLE_TYPE* const pUILink, long lArrayPos) 
{
    DoubleArrayParam* pParam = getAs <DoubleArrayParam> (pUILink);
    return static_cast <double> (*(pParam->getPointerToProtValue (pUILink->prot(), lArrayPos)));
}



bool WIPParameterTool::stdGetValue (LINK_BOOL_TYPE* const pUILink, long) 
{
    BoolParam* pParam = getAs <BoolParam> (pUILink);
    return *(pParam->getPointerToProtValue (pUILink->prot()));
}



unsigned WIPParameterTool::stdGetValue (LINK_SELECTION_TYPE* const pUILink, long) 
{
    SelectionParam* pParam = getAs <SelectionParam> (pUILink);

    long lOptionValue = *(pParam->getPointerToProtValue (pUILink->prot()));

    const WIPSelectionItem* pItem;

    if (pParam->getItemForValue (lOptionValue, pItem)) {
        unsigned uiResourceId = MRI_STD_STRING;
        SET_MODIFIER (uiResourceId, static_cast <unsigned char> (pItem->getIndex()));
        return uiResourceId;
    } else {
        return 0;
    }
}



long WIPParameterTool::stdSetValue (LINK_LONG_TYPE* const pUILink, long lNewVal, long) 
{
    LongParam* pParam = getAs <LongParam> (pUILink);
    long lCorrectedVal = correctValueForLimits (pParam->m_MinVal, pParam->m_MaxVal, pParam->m_Inc, lNewVal);
    *(pParam->getPointerToProtValue (pUILink->prot())) = static_cast <int32_t> (lCorrectedVal);
    return lCorrectedVal;
}



long WIPParameterTool::stdSetValueInArray (LINK_LONG_TYPE* const pUILink, long lNewVal, long lArrayPos)
{
    LongArrayParam* pParam = getAs <LongArrayParam> (pUILink);
    long lCorrectedVal = correctValueForLimits (pParam->m_MinVal [lArrayPos], pParam->m_MaxVal [lArrayPos], pParam->m_Inc [lArrayPos], lNewVal);
    *(pParam->getPointerToProtValue (pUILink->prot(), lArrayPos)) = static_cast <int32_t> (lCorrectedVal);
    return lCorrectedVal;
}



double WIPParameterTool::stdSetValue (LINK_DOUBLE_TYPE* const pUILink, double dNewVal, long) 
{
    DoubleParam* pParam = getAs <DoubleParam> (pUILink);
    double dCorrectedVal = correctValueForLimits (pParam->m_MinVal, pParam->m_MaxVal, pParam->m_Inc, dNewVal);
    *(pParam->getPointerToProtValue (pUILink->prot())) = dCorrectedVal;
    return dCorrectedVal;
}



double WIPParameterTool::stdSetValueInArray (LINK_DOUBLE_TYPE* const pUILink, double dNewVal, long lArrayPos)
{
    DoubleArrayParam* pParam = getAs <DoubleArrayParam> (pUILink);
    double dCorrectedVal = correctValueForLimits (pParam->m_MinVal [lArrayPos], pParam->m_MaxVal [lArrayPos], pParam->m_Inc [lArrayPos], dNewVal);
    *(pParam->getPointerToProtValue (pUILink->prot(), lArrayPos)) = dCorrectedVal;
    return dCorrectedVal;
}



bool WIPParameterTool::stdSetValue (LINK_BOOL_TYPE* const pUILink, bool bNewVal, long) 
{
    BoolParam* pParam = getAs <BoolParam> (pUILink);
    *(pParam->getPointerToProtValue (pUILink->prot())) = bNewVal;
    return bNewVal;
}



unsigned WIPParameterTool::stdSetValue (LINK_SELECTION_TYPE* const pUILink, unsigned uiNewResourceId, long) 
{
    SelectionParam* pParam = getAs <SelectionParam> (pUILink);

    // The UI works with resource IDs. Therefore, the option index must be translated.
    // The GET- and SET_MODIFIER-methods perform the conversion
    unsigned uiNewOptionIndex = GET_MODIFIER (uiNewResourceId);

    *(pParam->getPointerToProtValue (pUILink->prot())) = pParam->m_aSelectionItems [uiNewOptionIndex].getStoredValue();

    unsigned uiReturnedResourceId = MRI_STD_STRING;
    SET_MODIFIER (uiReturnedResourceId, static_cast <unsigned char> (uiNewOptionIndex));

    return uiReturnedResourceId;
}



bool WIPParameterTool::stdGetLimits (
    LINK_LONG_TYPE* const pUILink,
    std::vector <MrLimitLong>& rLimitVector,
    unsigned long& rulVerify,
    long)
{
    rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
    rLimitVector.resize (1);

    LongParam* pParam = getAs <LongParam> (pUILink);

    return rLimitVector [0].setEqualSpaced (pParam->m_MinVal, pParam->m_MaxVal, pParam->m_Inc);
}



bool WIPParameterTool::stdGetLimitsInArray (
    LINK_LONG_TYPE* const pUILink,
    std::vector <MrLimitLong>& rLimitVector,
    unsigned long& rulVerify,
    long lArrayPos)
{
    rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
    rLimitVector.resize (1);

    LongArrayParam* pParam = getAs <LongArrayParam> (pUILink);

    return rLimitVector [0].setEqualSpaced (pParam->m_MinVal [lArrayPos], pParam->m_MaxVal [lArrayPos], pParam->m_Inc [lArrayPos]);
}



bool WIPParameterTool::stdGetLimits (
    LINK_DOUBLE_TYPE* const pUILink,
    std::vector <MrLimitDouble>& rLimitVector,
    unsigned long& rulVerify,
    long)
{
    rulVerify = LINK_DOUBLE_TYPE::VERIFY_BINARY_SEARCH;
    rLimitVector.resize (1);

    DoubleParam* pParam = getAs <DoubleParam> (pUILink);

    return rLimitVector [0].setEqualSpaced (pParam->m_MinVal, pParam->m_MaxVal, pParam->m_Inc);
}



bool WIPParameterTool::stdGetLimitsInArray (
    LINK_DOUBLE_TYPE* const pUILink,
    std::vector <MrLimitDouble>& rLimitVector,
    unsigned long& rulVerify,
    long lArrayPos)
{
    rulVerify = LINK_LONG_TYPE::VERIFY_BINARY_SEARCH;
    rLimitVector.resize (1);

    DoubleArrayParam* pParam = getAs <DoubleArrayParam> (pUILink);

    return rLimitVector [0].setEqualSpaced (pParam->m_MinVal [lArrayPos], pParam->m_MaxVal [lArrayPos], pParam->m_Inc [lArrayPos]);
}



unsigned WIPParameterTool::stdGetPrecision (LINK_DOUBLE_TYPE* const pUILink, long)
{
    DoubleParam* pParam = getAs <DoubleParam> (pUILink);
    return pParam->m_lPrecision;
}



unsigned WIPParameterTool::stdGetPrecisionInArray (LINK_DOUBLE_TYPE* const pUILink, long)
{
    DoubleArrayParam* pParam = getAs <DoubleArrayParam> (pUILink);
    return pParam->m_lPrecision;
}



unsigned WIPParameterTool::stdGetLabelId (MrUILinkBase* const pUILink, char* arg_list[], long)
{
    WIPParam* pParam = getInstanceFor (*pUILink)->getParam (*pUILink);

    arg_list [0] = const_cast <char*> (pParam->m_sLabel.c_str());

    return MRI_STD_STRING;
}



unsigned WIPParameterTool::stdGetLabelIdInArray (MrUILinkBase* const pUILink, char* arg_list[], long lArrayPos)
{
    WIPParam* pParam = getInstanceFor (*pUILink)->getParam (*pUILink);

    // print label with array position into new string
    static char sDynamicLabelId [256];
    sprintf (sDynamicLabelId, "%s %ld", pParam->m_sLabel.c_str(), (lArrayPos + 1));

    arg_list [0] = sDynamicLabelId;

    return MRI_STD_STRING;
}



unsigned WIPParameterTool::stdGetUnitId (MrUILinkBase* const pUILink, char* arg_list[], long)
{
    WIPParam* pParam = getInstanceFor (*pUILink)->getParam (*pUILink);

    std::string& rsLabel = pParam->m_sUnit;
    arg_list [0] = const_cast <char*> (rsLabel.c_str());

    return MRI_STD_STRING;
}



int WIPParameterTool::stdFormat (LINK_SELECTION_TYPE* const pUILink, unsigned uiResourceId, char* arg_list[], long)
{
    SelectionParam* pParam = getAs <SelectionParam> (pUILink);

    // The UI works with resource IDs. Therefore, the option index must be translated.
    // The GET- and SET_MODIFIER-methods perform the conversion
    unsigned int uiOptionIndex = GET_MODIFIER (uiResourceId);

    WIPSelectionItem* pItem = NULL;
    if (uiOptionIndex < pParam->m_aSelectionItems.size()) {
        pItem = &pParam->m_aSelectionItems [uiOptionIndex];
    } else {
        // if the option does not exist, use the text for undefined option
        pItem = &pParam->m_Undefined;
    }

     arg_list [0] = const_cast <char*> (pItem->getText().c_str());

    return true;
}



bool WIPParameterTool::stdGetOptions (LINK_BOOL_TYPE* const, std::vector <unsigned>& rOptionVector, unsigned long& rulVerify, long)
{
    rOptionVector.resize (2);

    rulVerify = LINK_BOOL_TYPE::VERIFY_ON;

    rOptionVector [0] = 0;
    rOptionVector [1] = 1;

    return true;
}



bool WIPParameterTool::stdGetOptions (LINK_SELECTION_TYPE* const pUILink, std::vector <unsigned>& rResourceIdVector, unsigned long& rulVerify, long)
{
    SelectionParam* pParam = getAs <SelectionParam> (pUILink);

    rResourceIdVector.resize (pParam->m_aSelectionItems.size());

    rulVerify = LINK_SELECTION_TYPE::VERIFY_ON;

    // The UI works with resource IDs. Therefore, the option index must be translated.
    // The GET- and SET_MODIFIER-methods perform the conversion

    for (unsigned int iI = 0; iI < pParam->m_aSelectionItems.size(); iI++) {
        rResourceIdVector [iI] = MRI_STD_STRING;
        SET_MODIFIER (rResourceIdVector [iI], static_cast <unsigned char> (iI));
    }

    return true;
}



unsigned WIPParameterTool::stdGetToolTip (MrUILinkBase* const pUILink, char* arg_list[], long)
{
    WIPParam* pParam = getInstanceFor (*pUILink)->getParam (*pUILink);

    arg_list [0] = const_cast <char*> (pParam->m_sToolTip.c_str());

    return MRI_STD_STRING;
}



/// This macro is only needed for the next method.
/// Unfortunately, registerGetToolTipIdHandler is not available for MrUILinkBase.
/// We can't change this in libUILink because we need to stay compatible with the
/// VD11 version of libUILink.
#define TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER(UI_LINK_TYPE)                           \
    {                                                                                \
        UI_LINK_TYPE* pUILink = dynamic_cast <UI_LINK_TYPE*> (pParam->m_pUILink);    \
        if (pUILink != NULL) {                                                       \
            pUILink->registerGetToolTipIdHandler (stdGetToolTip);                    \
            return;                                                                  \
        }                                                                            \
    }

void WIPParameterTool::setToolTip (long lPos, const char* sText)
{
    WIPParam* pParam = getParam (lPos);

    pParam->m_sToolTip.assign (sText);

    TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER (LINK_LONG_TYPE);
    TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER (LINK_DOUBLE_TYPE);
    TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER (LINK_BOOL_TYPE);
    TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER (LINK_SELECTION_TYPE);
}

#undef TRY_TO_REGISTER_GET_TOOL_TIP_HANDLER



void WIPParameterTool::registerIsAvailableHandler (long lPos, LINK_LONG_TYPE::PFctIsAvailable pHandler) const
{
    LINK_LONG_TYPE* pUILink = NULL;

    if (getUILinkAs (lPos, pUILink)) {
        pUILink->registerIsAvailableHandler (pHandler);
    }

    // Register the isAvailableHandler also for the UILinkArray instance
    ArrayParam* pArrayParam = dynamic_cast <ArrayParam*> (getParam (lPos));
    if ((pArrayParam != NULL) && (pArrayParam->m_pUILinkArrray != NULL)) {
        pArrayParam->m_pUILinkArrray->registerIsAvailableHandler (reinterpret_cast <MrUILinkBase::PFctIsAvailable> (pHandler));
    }
}



void WIPParameterTool::registerIsAvailableHandler (long lPos, LINK_DOUBLE_TYPE::PFctIsAvailable pHandler) const
{
    LINK_DOUBLE_TYPE* pUILink = NULL;

    if (getUILinkAs (lPos, pUILink)) {
        pUILink->registerIsAvailableHandler (pHandler);
    }

    // Register the isAvailableHandler also for the UILinkArray instance
    ArrayParam* pArrayParam = dynamic_cast <ArrayParam*> (getParam (lPos));
    if ((pArrayParam != NULL) && (pArrayParam->m_pUILinkArrray != NULL)) {
        pArrayParam->m_pUILinkArrray->registerIsAvailableHandler (reinterpret_cast <MrUILinkBase::PFctIsAvailable> (pHandler));
    }
}



void WIPParameterTool::registerIsAvailableHandler (long lPos, LINK_BOOL_TYPE::PFctIsAvailable pHandler) const
{
    LINK_BOOL_TYPE* pUILink = NULL;

    if (getUILinkAs (lPos, pUILink)) {
        pUILink->registerIsAvailableHandler (pHandler);
    }

    // Register the isAvailableHandler also for the UILinkArray instance
    ArrayParam* pArrayParam = dynamic_cast <ArrayParam*> (getParam (lPos));
    if ((pArrayParam != NULL) && (pArrayParam->m_pUILinkArrray != NULL)) {
        pArrayParam->m_pUILinkArrray->registerIsAvailableHandler (reinterpret_cast <MrUILinkBase::PFctIsAvailable> (pHandler));
    }
}



void WIPParameterTool::registerIsAvailableHandler (long lPos, LINK_SELECTION_TYPE::PFctIsAvailable pHandler) const
{
    LINK_SELECTION_TYPE* pUILink = NULL;

    if (getUILinkAs (lPos, pUILink)) {
        pUILink->registerIsAvailableHandler (pHandler);
    }

    // Register the isAvailableHandler also for the UILinkArray instance
    ArrayParam* pArrayParam = dynamic_cast <ArrayParam*> (getParam (lPos));
    if ((pArrayParam != NULL) && (pArrayParam->m_pUILinkArrray != NULL)) {
        pArrayParam->m_pUILinkArrray->registerIsAvailableHandler (reinterpret_cast <MrUILinkBase::PFctIsAvailable> (pHandler));
    }
}



void WIPParameterTool::registerIsVisibleHandler (long lPos, MrUILinkBase::PFctIsVisible pHandler) const
{
    MrUILinkBase* pUILink = NULL;

    if (getUILinkAs (lPos, pUILink)) {
        pUILink->registerIsVisibleHandler (pHandler);
    }

    // Register the isAvailableHandler also for the UILinkArray instance
    ArrayParam* pArrayParam = dynamic_cast <ArrayParam*> (getParam (lPos));
    if ((pArrayParam != NULL) && (pArrayParam->m_pUILinkArrray != NULL)) {
        pArrayParam->m_pUILinkArrray->registerIsVisibleHandler (pHandler);
    }
}



MrUILinkBase* WIPParameterTool::getUILink (long lPos) const
{
    WIPParam* pParam = getParam (lPos);

    if (pParam != NULL) {
        return pParam->m_pUILink;
    }

    return NULL;
}



bool WIPParameterTool::getDefaultValuesInitialized (MrProt& rMrProt) const
{
    if (rMrProt.getData().getsWipMemBlock().getalFree() [K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE - 1] == ALREADY_INITIALIZED) {
        return true;
    } else {
        return false;
    }
}



void WIPParameterTool::setDefaultValuesInitialized (MrProt& rMrProt, bool bInitialized) const
{
    long lValue = 0;

    if (bInitialized) {
        lValue = ALREADY_INITIALIZED;
    } else {
        lValue = NOT_YET_INITIALIZED;
    }

    rMrProt.getData().getsWipMemBlock().getalFree() [K_WIP_MEM_BLOCK_FREE_LONG_MAX_SIZE - 1] = lValue;
}



WIPParameterTool::WIPParam* WIPParameterTool::getParam (const MrUILinkBase& rUILink) const
{
    ParamMapIter DataIter;
    for (DataIter = m_ParamMap.begin(); DataIter != m_ParamMap.end(); DataIter++) {
        if (DataIter->second->isParamFor (rUILink)) {
            return DataIter->second;
        }
    }

    UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: WIP parameter not registered for the given UI element (UI tag: " << rUILink.GetCharTag() << ")!");
    throw std::logic_error ("WIPParameterTool: WIP parameter not registered for the given UI element!");
}

#endif


bool WIPParameterTool::isParamRegistered (long lPos) const
{
    ParamMap::const_iterator DataIter = m_ParamMap.find (lPos);

    if (DataIter != m_ParamMap.end()) {
        return true;
    } else {
        return false;
    }
}



WIPParameterTool::WIPParam* WIPParameterTool::getParam (long lPos) const
{
    ParamMap::const_iterator DataIter = m_ParamMap.find (lPos);

    if (DataIter != m_ParamMap.end()) {
        return DataIter->second;
    }

    UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: No WIP parameter registered at given UI position (" << lPos << ")!");
    throw std::logic_error ("WIPParameterTool: No WIP parameter registered at given UI position!");
}



void WIPParameterTool::addParam (WIPParam& rParam)
{
    long lPos = rParam.m_lPos;

    // delete possibly existing previous WIP parameter data
    if (isParamRegistered (lPos)) {
        WIPParam* pPrevParam = getParam (lPos);

        pPrevParam->unreserveElementsInWIPMemBlock (*this);

        delete pPrevParam;
    }

    if (!rParam.reserveElementsInWIPMemBlock (*this)) {
        UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: Elements in WIP memory block already reserved by another parameter (UI position: " << lPos << ")!");
        throw std::logic_error ("WIPParameterTool: Elements in WIP memory block already reserved by another parameter!");
    }

    m_ParamMap [lPos] = &rParam;
}
