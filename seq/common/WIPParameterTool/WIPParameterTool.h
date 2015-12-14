//    -----------------------------------------------------------------------------
//  Copyright (C) Siemens AG 2001  All Rights Reserved.  Confidential
//    -----------------------------------------------------------------------------
//
// Project: NUMARIS/4
//    File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\WIPParameterTool\WIPParameterTool.h
//
//    Lang: C++
//
// Descrip: WIPParameterTool class header
//
//    -----------------------------------------------------------------------------
#pragma once

#ifdef WIN32
#include "MrServers/MrProtSrv/MrProtocol/libUICtrl/UICtrl.h"
#endif

#include "MrServers/MrMeasSrv/SeqIF/Sequence/Sequence.h"
#include "ProtBasic/Interfaces/SeqLim.h"
#include "MrServers/MrProtSrv/MrProt/MrProt.h"

#include <map>
#include <vector>


// DLL exports/imports
#ifdef BUILD_WIPParameterTool
#define __OWNER
#endif
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"


namespace WPT_NAMESPACE
{

// --------------------------------------------------------------------------------------------
///  \brief    This class represents one item of a selection box.
///
///            Each item has the following attributes:
///            - The index (i.e. the position in the drop-down list if all options are
///              displayed)
///            - The option text to be displayed in the drop-down list
///            - The stored value (i.e. the representation of this value in the protocol-
///              WipMemBlock)
// --------------------------------------------------------------------------------------------
class __IMP_EXP WIPSelectionItem
{
public:

    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a selection item instance.
    ///
    ///  \param    iIndex        Index of this item (i.e. the position in the drop-down list)
    ///  \param    sOptionText   The option text to displayed in the drop-down list
    ///  \param    lStoredValue  The value to be stored in the WipMemBlock
    ///                          (important to allow interpreting the protocol value in
    ///                          ICE-programs)
    // --------------------------------------------------------------------------------------------
    WIPSelectionItem (int iIndex, const char* sOptionText, int32_t lStoredValue);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a selection item instance.
    ///
    ///  \param    iIndex        Index of this item (i.e. the position in the drop-down list)
    ///  \param    sOptionText   The option text to displayed in the drop-down list
    ///  \param    lStoredValue  The value to be stored in the WipMemBlock
    ///                          (important to allow interpreting the protocol value in
    ///                          ICE-programs)
    // --------------------------------------------------------------------------------------------
    WIPSelectionItem (int iIndex, std::string& sOptionText, int32_t lStoredValue);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Compares the option text of this item with the given text.
    ///
    ///  \param    rsText  Text to be compared with
    ///
    ///  \return   \arg \c true  : If the text matches
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    bool isText (const char* rsText) const;


    // --------------------------------------------------------------------------------------------
    ///  \return   The index of this item in the drop-down list
    // --------------------------------------------------------------------------------------------
    int getIndex (void) const;


    // --------------------------------------------------------------------------------------------
    ///  \return   The option text of this item
    // --------------------------------------------------------------------------------------------
    const std::string& getText (void) const;


    // --------------------------------------------------------------------------------------------
    ///  \return   The stored value for this item
    // --------------------------------------------------------------------------------------------
    int32_t getStoredValue (void) const;


private:

    int          m_iIndex;
    std::string  m_sOptionText;
    int32_t      m_lStoredValue;
};





// --------------------------------------------------------------------------------------------
///  \brief    This toolkit provides simple methods to add WIP parameters.
///
///            The tool will automatically register standard implementations of the following
///            UI handlers:
///
///            - GetValue
///            - SetValue
///            - GetLimits
///            - GetLabelID
///            - GetUnitID
///            - GetOptions
///            - Format
///
///            It is recommended to define your WIP parameter positions with an enum
///            (e.g. in the header file of your sequence):
///            <pre>
///              enum WIPParamPositions {
///                  WIPParamLong      = 0,
///                  WIPParamDouble    = 1,
///                  WIPParamSelection = 7,
///                  WIPParamBool      = 8
///              };
///            </pre>
///
///            The WIP parameters must be created in the sequence init method (preferably by the
///            associated sequence UI link object). For the host side, it is possible to register
///            customized handler methods.
///
///            <pre>
///              rTool.createLongParameter (WIPParamLong, rSeqLim, "WIP long", "unit", lMin, lMax, lInc, lDef);
///              
///              rTool.createDoubleParameter (WIPParamDouble, rSeqLim, "WIP double", "unit 2", 2, dMin, dMax, dInc, dDef);
///
///              rTool.createSelectionParameter (WIPParamSelection, rSeqLim, "WIP selection");
///              rTool.addOption (WIPParamSelection, "first option");
///              rTool.addOption (WIPParamSelection, "second option");
///              rTool.addOption (WIPParamSelection, "third option");
///
///              #ifdef WIN32
///                  rTool.registerSolveHandler (WIPParamLong, solveHandlerLong);
///                  rTool.createBoolParameter (WIPParamBool, rSeqLim, "WIP bool", false);
///              #endif
///            </pre>
///
///            Some important notices:
///
///            - The create-methods must be called under Windows AND Linux.
///            - The prepare-method of the WIPParameterTool must be called in each prepare and
///              before WIP-parameters are read.
///            - In sequence-prepare, the values must be read from the protocol
///              (e.g. rTool.getLongValue (rMrProt, WIPParamLong);)
///            - In UI-handlers, the values should be read from the UI
///              (e.g. rTool.getLongValue (WIPParamLong);)
// --------------------------------------------------------------------------------------------
class WIPParameterTool
{
public:

    // forward declarations
    // (these classes are defined in the CPP file and are not important for the toolkit interface)
    class WIPParam;
    class LongParam;
    class LongArrayParam;
    class DoubleParam;
    class DoubleArrayParam;
    class BoolParam;
    class SelectionParam;

    typedef std::map <long, WIPParam*> ParamMap;
    typedef ParamMap::const_iterator  ParamMapIter;

    // UTRACE marker used by WIPParameterTool
    __IMP_EXP static const int UTRACE_MARKER;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Constructor for the WIPParameterTool.
    ///
    ///  \remarks  Each tool instance must be associated with one sequence instance. Do not create
    ///            multiple tools for one sequence!
    // --------------------------------------------------------------------------------------------
    __IMP_EXP WIPParameterTool (const SeqIF& rSequence);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Default destructor.
    // --------------------------------------------------------------------------------------------
    __IMP_EXP virtual ~WIPParameterTool (void);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for long values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at the position
    ///            lWIPMemBlockPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    lMin             Minimum value
    ///  \param    lMax             Maximum value
    ///  \param    lInc             Increment
    ///  \param    lDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createLongParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        int32_t     lMin,
        int32_t     lMax,
        int32_t     lInc,
        int32_t     lDef
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for long values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at position lPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    lMin             Minimum value
    ///  \param    lMax             Maximum value
    ///  \param    lInc             Increment
    ///  \param    lDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createLongParameter (
        long        lPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        int32_t     lMin,
        int32_t     lMax,
        int32_t     lInc,
        int32_t     lDef
    )
    {
        return createLongParameter (lPos, lPos, rSeqLim, sLabel, sUnit, lMin, lMax, lInc, lDef);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI array parameter for long values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter values will be stored in WIPMemBlock.alFree[] at the positions
    ///            [lWIPMemBlockPos, lWIPMemBlockPos + 1, ..., lWIPMemBlockPos + lSize - 1].
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    lSize            Size of the array
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    lMin             Minimum value
    ///  \param    lMax             Maximum value
    ///  \param    lInc             Increment
    ///  \param    lDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createLongArrayParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        long        lSize,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        int32_t     lMin,
        int32_t     lMax,
        int32_t     lInc,
        int32_t     lDef
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the limits of one element in a WIP UI array parameter.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Index of the element
    ///  \param    lMin         Minimum value
    ///  \param    lMax         Maximum value
    ///  \param    lInc         Increment
    ///  \param    lDef         Default value
    ///
    ///  \throw    exception  If no LongArrayParam has been registered for this position
    ///  \throw    exception  If the array index is out of bounds
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void setLimits (
        long    lPos,
        long    lArrayIndex,
        int32_t lMin,
        int32_t lMax,
        int32_t lInc,
        int32_t lDef
    );
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for double values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.adFree[] at the position
    ///            lWIPMemBlockPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.adFree[])
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    uPrecision       Number of digits to be displayed
    ///  \param    dMin             Minimum value
    ///  \param    dMax             Maximum value
    ///  \param    dInc             Increment
    ///  \param    dDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createDoubleParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        unsigned    uPrecision,
        double      dMin,
        double      dMax,
        double      dInc,
        double      dDef
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for double values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.adFree[] at position lPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    uPrecision       Number of digits to be displayed
    ///  \param    dMin             Minimum value
    ///  \param    dMax             Maximum value
    ///  \param    dInc             Increment
    ///  \param    dDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createDoubleParameter (
        long        lPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        unsigned    uPrecision,
        double      dMin,
        double      dMax,
        double      dInc,
        double      dDef
    )
    {
        return createDoubleParameter (lPos, lPos, rSeqLim, sLabel, sUnit, uPrecision, dMin, dMax, dInc, dDef);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI array parameter for double values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter values will be stored in WIPMemBlock.adFree[] at the positions
    ///            [lWIPMemBlockPos, lWIPMemBlockPos + 1, ..., lWIPMemBlockPos + lSize - 1].
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.adFree[])
    ///  \param    lSize            Size of the array
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///  \param    uPrecision       Number of digits to be displayed
    ///  \param    dMin             Minimum value
    ///  \param    dMax             Maximum value
    ///  \param    dInc             Increment
    ///  \param    dDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createDoubleArrayParameter (
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
        double      dDef
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the limits of one element in a WIP UI array parameter.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Index of the element
    ///  \param    dMin         Minimum value
    ///  \param    dMax         Maximum value
    ///  \param    dInc         Increment
    ///  \param    dDef         Default value
    ///
    ///  \throw    exception  If no DoubleArrayParam has been registered for this position
    ///  \throw    exception  If the array index is out of bounds
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void setLimits (
        long   lPos,
        long   lArrayIndex,
        double dMin,
        double dMax,
        double dInc,
        double dDef
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for bool values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at the position
    ///            lWIPMemBlockPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    bDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createBoolParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        bool        bDef = false
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for bool values.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at position lPos.
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    bDef             Default value
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createBoolParameter (
        long        lPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        bool        bDef = false
    )
    {
        return createBoolParameter (lPos, lPos, rSeqLim, sLabel, bDef);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for selections.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at the position
    ///            lWIPMemBlockPos.
    ///
    ///  \param    lPos              Position on the parameter card
    ///  \param    lWIPMemBlockPos   Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    rSeqLim           Reference to the sequence limits object
    ///  \param    sLabel            Label ID string
    ///  \param    sUnit             Unit ID string
    ///  \param    lNumberOfOptions  Number of options
    ///  \param    asOptions         Array containing the option texts
    ///  \param    iDefaultOptionId  Default option index
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createSelectionParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        long        lNumberOfOptions,
        const char* asOptions[],
        int         iDefaultOptionId = 0
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for selections.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at position lPos.
    ///
    ///  \param    lPos              Position on the parameter card
    ///  \param    rSeqLim           Reference to the sequence limits object
    ///  \param    sLabel            Label ID string
    ///  \param    sUnit             Unit ID string
    ///  \param    lNumberOfOptions  Number of options
    ///  \param    asOptions         Array containing the option texts
    ///  \param    iDefaultOptionId  Default option index
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createSelectionParameter (
        long        lPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit,
        long        lNumberOfOptions,
        const char* asOptions[],
        int         iDefaultOptionId = 0
    )
    {
        return createSelectionParameter (lPos, lPos, rSeqLim, sLabel, sUnit, lNumberOfOptions, asOptions, iDefaultOptionId);
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for selections.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///            You can add options later by using the addOption method.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at the position
    ///            lWIPMemBlockPos.
    ///
    ///  \see      WIPParameterTool::addOption
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createSelectionParameter (
        long        lPos,
        long        lWIPMemBlockPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit = NULL
    );


    // --------------------------------------------------------------------------------------------
    ///  \brief    Creates a WIP UI parameter for selections.
    ///
    ///            The information passed in this method will be used by the standard handler
    ///            methods provided by the WIPParameterTool.
    ///
    ///            You can add options later by using the addOption method.
    ///
    ///  \remarks  The WIP parameter value will be stored in WIPMemBlock.alFree[] at position lPos.
    ///
    ///  \see      WIPParameterTool::addOption
    ///
    ///  \param    lPos             Position on the parameter card
    ///  \param    lWIPMemBlockPos  Position in the protocol (i.e. in WipMemBlock.alFree[])
    ///  \param    rSeqLim          Reference to the sequence limits object
    ///  \param    sLabel           Label ID string
    ///  \param    sUnit            Unit ID string
    ///
    ///  \return   \arg \c true  : If the WIP parameter could be registered
    ///            \arg \c false : If an error occurred
    ///
    ///  \throw    exception  If the UI link object could not be created
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool createSelectionParameter (
        long        lPos,
        SeqLim&     rSeqLim,
        const char* sLabel,
        const char* sUnit = NULL
    )
    {
        return createSelectionParameter (lPos, lPos, rSeqLim, sLabel, sUnit);
    }

    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to the WIP parameter at the given position.
    ///
    ///            In the WIPMemBlock this option will be represented by the option index.
    ///
    ///  \see      WIPParameterTool::addDefaultOption
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    sOption  Option text
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void addOption (long lPos, const char* sOptionText);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to the WIP parameter at the given position.
    ///
    ///  \see      WIPParameterTool::addDefaultOption
    ///
    ///  \param    lPos          Position on the parameter card
    ///  \param    sOption       Option text
    ///  \param    lStoredValue  Value to be stored in the WIPMemBlock (e.g. an enumeration value)
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void addOption (long lPos, const char* sOptionText, int32_t lStoredValue);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to the WIP parameter at the given position.
    ///
    ///            This option is set to be the default option for this WIP parameter. In the
    ///            WIPMemBlock this option will be represented by the option index.
    ///
    ///  \see      WIPParameterTool::addOption
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    sOption  Option text
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void addDefaultOption (long lPos, const char* sOptionText);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Adds an option to the WIP parameter at the given position.
    ///
    ///  \see      WIPParameterTool::addDefaultOption
    ///
    ///  \param    lPos          Position on the parameter card
    ///  \param    sOption       Option text
    ///  \param    lStoredValue  Value to be stored in the WIPMemBlock (e.g. an enumeration value)
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void addDefaultOption (long lPos, const char* sOptionText, int32_t lStoredValue);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Reserves memory within the array WIPMemBlock.alFree[] to avoid interference of
    ///            parameters.
    ///
    ///            The same memory cannot be reserved twice.
    ///
    ///  \param    lStartPos          Position on the parameter card
    ///  \param    lNumberOfElements  Option text
    ///
    ///  \return   \arg \c true  : If the memory could be reserved
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If the start position or the number of elements is negative
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool reserveLongsInWIPMemBlock (long lStartPos, long lNumberOfElements = 1);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Reserves memory within the array WIPMemBlock.adFree[] to avoid interference of
    ///            parameters.
    ///
    ///            The same memory can not be reserved twice.
    ///
    ///  \param    lStartPos          Position on the parameter card
    ///  \param    lNumberOfElements  Option text
    ///
    ///  \return   \arg \c true  : If the memory could be reserved
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If the start position or the number of elements is negative
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool reserveDoublesInWIPMemBlock (long lStartPos, long lNumberOfElements = 1);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Unreserves memory within the array WIPMemBlock.alFree[].
    ///
    ///  \param    lStartPos          Position on the parameter card
    ///  \param    lNumberOfElements  Option text
    ///
    ///  \throw    exception  If the start position or the number of elements is negative
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void unreserveLongsInWIPMemBlock (long lStartPos, long lNumberOfElements = 1);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Unreserves memory within the array WIPMemBlock.adFree[].
    ///
    ///  \param    lStartPos          Position on the parameter card
    ///  \param    lNumberOfElements  Option text
    ///
    ///  \throw    exception  If the start position or the number of elements is negative
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void unreserveDoublesInWIPMemBlock (long lStartPos, long lNumberOfElements = 1);


    // --------------------------------------------------------------------------------------------
    ///  \brief    This method must be called in the prepare method of the sequence.
    ///
    ///            If this method returns false, you must return SEQU_ERROR!!! Otherwise, setting
    ///            the default value will not work!
    ///
    ///            This method sets all UI elements to the stored default values. Continuous
    ///            resetting of the values is prevented by storing the initialization status in the
    ///            protocol and by checking the prepare context. Only if the context is
    ///            "PrepForMrProtUpdate", the default values will be set.
    ///
    ///            Furthermore, this method checks whether the given parameters are all within the
    ///            specified limits.
    ///
    ///            Example usage for sequence prepare method:
    ///            <pre>
    ///              if (!m_WIPParameterTool.prepare (rMrProt, rSeqLim)) return SEQU_ERROR;
    ///            </pre>
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    ///  \param    rSeqLim  Reference to the sequence limits object
    ///
    ///  \return   \arg \c true  : If the default values were set
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool prepare (MrProt& rMrProt, SeqLim& rSeqLim);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the maximum number of WIP parameters that can be displayed on the UI
    ///            "special card".
    ///
    ///  \return   The maximum number of WIP parameters
    // --------------------------------------------------------------------------------------------
    __IMP_EXP static long getMaxNumberOfWIPParameters (void);
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \param    rMrProt  Reference to the protocol
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getLongValue (const MrProt& rMrProt, long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of a WIP array parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \param    rMrProt      Reference to the protocol
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getLongValue (const MrProt& rMrProt, long lPos, long lArrayIndex) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \param    rMrProt  Reference to the protocol
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP double getDoubleValue (const MrProt& rMrProt, long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of a WIP array parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \param    rMrProt      Reference to the protocol
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP double getDoubleValue (const MrProt& rMrProt, long lPos, long lArrayIndex) const;
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \param    rMrProt  Reference to the protocol
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool getBoolValue (const MrProt& rMrProt, long lPos) const;
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \see      WIPParameterTool::isSelectionValue
    ///
    ///  \param    rMrProt  Reference to the protocol
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getSelectionValue (const MrProt& rMrProt, long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            The value will be read from the given protocol.
    ///
    ///  \see      WIPParameterTool::isSelectionValue
    ///
    ///  \param    rMrProt  Reference to the protocol
    ///  \param    lPos     Position on the parameter card
    ///
    ///  \return   The selected item
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP const WIPSelectionItem& getSelectedItem (const MrProt& rMrProt, long lPos) const;
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Tells whether a WIP parameter has been registered for the given position.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   \arg \c true  : If a WIP parameter has been registered
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool isParamRegistered (long lPos) const;

    
    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Returns the WIP param object for the parameter at the given position.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   A pointer to the given UI link object (or NULL if it does not exist)
    // --------------------------------------------------------------------------------------------
    WIPParam* getParam (long lPos) const;


#ifdef WIN32

    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Returns the associated WIPParam object for the given UI link object.
    ///
    ///  \param    rUILink  Reference to the UI link object
    ///
    ///  \return   A pointer to the requested WIPParam
    // --------------------------------------------------------------------------------------------
    WIPParam* getParam (const MrUILinkBase& rUILink) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getLongValue (long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP array parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getLongValue (long lPos, long lArrayIndex) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP double getDoubleValue (long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP array parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP double getDoubleValue (long lPos, long lArrayIndex) const;
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool getBoolValue (long lPos) const;
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \see      WIPParameterTool::isSelectionValue
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The parameter value
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP int32_t getSelectionValue (long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the value of the WIP parameter at the given position.
    ///
    ///            This method will call the getValue handler of the corresponding UILink element.
    ///
    ///  \see      WIPParameterTool::isSelectionValue
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The option text
    ///
    ///  \throw    exception  If no parameter is registered at the given position
    ///  \throw    exception  If the registered parameter does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP const WIPSelectionItem& getSelectedItem (long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP parameter at the given position.
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    lNewVal  New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setLongValue (long lPos, int32_t lNewVal) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP array parameter at the given position.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///  \param    lNewVal      New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setLongValue (long lPos, long lArrayIndex, int32_t lNewVal) const;

    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP parameter at the given position.
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    dNewVal  New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setDoubleValue (long lPos, double dNewVal) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP array parameter at the given position.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    lArrayIndex  Position within the array
    ///  \param    dNewVal      New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setDoubleValue (long lPos, long lArrayIndex, double dNewVal) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP parameter at the given position.
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    bNewVal  New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setBoolValue (long lPos, bool bNewVal) const;

    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP parameter at the given position.
    ///
    ///  \param    lPos     Position on the parameter card
    ///  \param    lNewVal  New parameter value
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setSelectionValue (long lPos, int32_t lNewVal) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the value of the WIP parameter at the given position.
    ///
    ///  \param    lPos         Position on the parameter card
    ///  \param    sOptionText  New option text
    ///
    ///  \return   \arg \c true  : If the update was successful
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the registered WIP parameter does not have the correct type
    ///  \throw    exception  If the UI link object does not yet exist
    // --------------------------------------------------------------------------------------------
    __IMP_EXP bool setSelectionText (long lPos, const char* sOptionText) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the UI tag string for the given position on the parameter card.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   The UI tag for the given position
    ///
    ///  \throw    exception  If the position index is out of bounds
    // --------------------------------------------------------------------------------------------
    __IMP_EXP static inline const char* getUITag (long lPos);
    
    
    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Returns the UI link object for the parameter at the given position.
    ///
    ///  \param    lPos  Position on the parameter card
    ///
    ///  \return   A pointer to the requested UI link object (or NULL if it does not yet exist)
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    // --------------------------------------------------------------------------------------------
    __IMP_EXP MrUILinkBase* getUILink (long lPos) const;


    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Converts and returns the UI link object for the parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    rpUILink  Export reference for the pointer to the UI link object
    ///
    ///  \return   \arg \c true  : If the UI link object existed and could be converted
    ///            \arg \c false : Otherwise
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    template <typename UI_LINK_TYPE>
    inline bool getUILinkAs (long lPos, UI_LINK_TYPE*& rpUILink) const
    {
        MrUILinkBase* pTmpUILink = getUILink (lPos);

        if (pTmpUILink != NULL) {
            rpUILink = dynamic_cast <UI_LINK_TYPE*> (pTmpUILink);
            
            if (rpUILink != NULL) {
                return true;
            } else {
                UTRACE (Error, UTRACE_MARKER, "WIPParameterTool: UI element has the wrong type, conversion failed (position: " << lPos << ")!");
                throw std::logic_error ("WIPParameterTool: UI element has the wrong type, conversion failed!");
            }
        }

        return false;
    }

    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Returns the associated WIPParameterTool instance for the given UI link object.
    ///
    ///            This method can be used in handler methods.
    ///
    ///  \param    rUILink                  Reference to the UI link object
    ///  \param    bExceptionIfNotExisting  Tells this method if an exception should be thrown
    ///                                     if no tool instance exists
    ///
    ///  \return   A pointer to the associated WIPParameterTool instance.
    ///
    ///  \throw    exception  If no WIPParameterTool instance exists for this UI element
    // --------------------------------------------------------------------------------------------
    __IMP_EXP static WIPParameterTool* getInstanceFor (const MrUILinkBase& rUILink, bool bExceptionIfNotExisting = true);


    // --------------------------------------------------------------------------------------------
    // Standard handlers (will be automatically registered)
    // --------------------------------------------------------------------------------------------
    __IMP_EXP static long     stdGetArraySize        (MrUILinkArray* const, long);
    __IMP_EXP static long     stdGetValue            (LINK_LONG_TYPE* const pUILink, long);
    __IMP_EXP static long     stdGetValueInArray     (LINK_LONG_TYPE* const pUILink, long lArrayIndex);
    __IMP_EXP static long     stdSetValue            (LINK_LONG_TYPE* const pUILink, long lNewVal, long);
    __IMP_EXP static long     stdSetValueInArray     (LINK_LONG_TYPE* const pUILink, long lNewVal, long lArrayIndex);
    __IMP_EXP static bool     stdGetLimits           (LINK_LONG_TYPE* const pUILink, std::vector <MrLimitLong>& rLimitVector, unsigned long& rulVerify, long);
    __IMP_EXP static bool     stdGetLimitsInArray    (LINK_LONG_TYPE* const pUILink, std::vector <MrLimitLong>& rLimitVector, unsigned long& rulVerify, long lArrayIndex);
    __IMP_EXP static double   stdGetValue            (LINK_DOUBLE_TYPE* const pUILink, long);
    __IMP_EXP static double   stdGetValueInArray     (LINK_DOUBLE_TYPE* const pUILink, long);
    __IMP_EXP static double   stdSetValue            (LINK_DOUBLE_TYPE* const pUILink, double dNewVal, long);
    __IMP_EXP static double   stdSetValueInArray     (LINK_DOUBLE_TYPE* const pUILink, double dNewVal, long);
    __IMP_EXP static bool     stdGetLimits           (LINK_DOUBLE_TYPE* const pUILink, std::vector <MrLimitDouble>& rLimitVector, unsigned long& rulVerify, long);
    __IMP_EXP static bool     stdGetLimitsInArray    (LINK_DOUBLE_TYPE* const pUILink, std::vector <MrLimitDouble>& rLimitVector, unsigned long& rulVerify, long);
    __IMP_EXP static unsigned stdGetPrecision        (LINK_DOUBLE_TYPE* const pUILink, long);
    __IMP_EXP static unsigned stdGetPrecisionInArray (LINK_DOUBLE_TYPE* const pUILink, long);
    __IMP_EXP static bool     stdGetValue            (LINK_BOOL_TYPE* const pUILink, long);
    __IMP_EXP static bool     stdSetValue            (LINK_BOOL_TYPE* const pUILink, bool bNewVal, long);
    __IMP_EXP static bool     stdGetOptions          (LINK_BOOL_TYPE* const pUILink, std::vector <unsigned>& rOptionVector, unsigned long& rulVerify, long);
    __IMP_EXP static unsigned stdGetValue            (LINK_SELECTION_TYPE* const pUILink, long);
    __IMP_EXP static unsigned stdSetValue            (LINK_SELECTION_TYPE* const pUILink, unsigned NewVal, long);
    __IMP_EXP static int      stdFormat              (LINK_SELECTION_TYPE* const pUILink, unsigned nID, char* arg_list[], long);
    __IMP_EXP static bool     stdGetOptions          (LINK_SELECTION_TYPE* const pUILink, std::vector <unsigned>& rOptionVector, unsigned long& rulVerify, long);
    __IMP_EXP static unsigned stdGetUnitId           (MrUILinkBase* const pUILink, char* arg_list[], long);
    __IMP_EXP static unsigned stdGetLabelId          (MrUILinkBase* const pUILink, char* arg_list[], long);
    __IMP_EXP static unsigned stdGetLabelIdInArray   (MrUILinkBase* const pUILink, char* arg_list[], long);
    __IMP_EXP static unsigned stdGetToolTip          (MrUILinkBase* const pUILink, char* arg_list[], long);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Sets the tool-tip for the given parameter.
    ///
    ///            Internally, this method will register a standard tool-tip-handler that returns
    ///            the text that has been provided in this method.
    ///
    ///  \param    lPos   Position on the parameter card
    ///  \param    sText  Text to be displayed as tool-tip
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void setToolTip (long lPos, const char* sText);


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerFormatHandler (long lPos, LINK_SELECTION_TYPE::PFctFormat pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerFormatHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLabelIdHandler (long lPos, LINK_LONG_TYPE::PFctGetLabelId pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLabelIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLabelIdHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetLabelId pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLabelIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLabelIdHandler (long lPos, LINK_BOOL_TYPE::PFctGetLabelId pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLabelIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLabelIdHandler (long lPos, LINK_SELECTION_TYPE::PFctGetLabelId pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLabelIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLimitsHandler (long lPos, LINK_LONG_TYPE::PFctGetLimits pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLimitsHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetLimitsHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetLimits pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetLimitsHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetOptionsHandler (long lPos, LINK_BOOL_TYPE::PFctGetOptions pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetOptionsHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetOptionsHandler (long lPos, LINK_SELECTION_TYPE::PFctGetOptions pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetOptionsHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetPrecisionHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetPrecision pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetPrecisionHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the customized tool tip will only appear while
    ///            hovering the cursor over the value field on the right side. The tool tip for
    ///            the label will still display all element values of the array.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerToolTipHandler (long lPos, LINK_LONG_TYPE::PFctGetToolTipId pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetToolTipIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the customized tool tip will only appear while
    ///            hovering the cursor over the value field on the right side. The tool tip for
    ///            the label will still display all element values of the array.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerToolTipHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetToolTipId pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetToolTipIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerToolTipHandler (long lPos, LINK_BOOL_TYPE::PFctGetToolTipId pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetToolTipIdHandler (pHandler);
        }
    }
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerToolTipHandler (long lPos, LINK_SELECTION_TYPE::PFctGetToolTipId pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetToolTipIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetUnitIdHandler (long lPos, LINK_LONG_TYPE::PFctGetUnitId pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetUnitIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetUnitIdHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetUnitId pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetUnitIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetUnitIdHandler (long lPos, LINK_BOOL_TYPE::PFctGetUnitId pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetUnitIdHandler (pHandler);
        }
    }
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetUnitIdHandler (long lPos, LINK_SELECTION_TYPE::PFctGetUnitId pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetUnitIdHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \remarks  Please be aware that the getLongValue method will call the registered UI handler
    ///            on the host side. However, on the MCIR side, the value will be taken directly
    ///            from the protocol.
    ///
    ///  \see      getLongValue
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetValueHandler (long lPos, LINK_LONG_TYPE::PFctGetValue pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \remarks  Please be aware that the getDoubleValue method will call the registered UI handler
    ///            on the host side. However, on the MCIR side, the value will be taken directly
    ///            from the protocol.
    ///
    ///  \see      getDoubleValue
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetValueHandler (long lPos, LINK_DOUBLE_TYPE::PFctGetValue pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \remarks  Please be aware that the getBoolValue method will call the registered UI handler
    ///            on the host side. However, on the MCIR side, the value will be taken directly
    ///            from the protocol.
    ///
    ///  \see      getBoolValue
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetValueHandler (long lPos, LINK_BOOL_TYPE::PFctGetValue pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \remarks  Please be aware that the getSelectionValue method will call the registered UI
    ///            handler on the host side. However, on the MCIR side, the value will be taken
    ///            directly from the protocol.
    ///
    ///  \see      getSelectionValue
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerGetValueHandler (long lPos, LINK_SELECTION_TYPE::PFctGetValue pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerGetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the given handler will be registered to both
    ///            UILink instances (array and element instance).
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerIsAvailableHandler (long lPos, LINK_LONG_TYPE::PFctIsAvailable pHandler) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the given handler will be registered to both
    ///            UILink instances (array and element instance).
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerIsAvailableHandler (long lPos, LINK_DOUBLE_TYPE::PFctIsAvailable pHandler) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the given handler will be registered to both
    ///            UILink instances (array and element instance).
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerIsAvailableHandler (long lPos, LINK_BOOL_TYPE::PFctIsAvailable pHandler) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the given handler will be registered to both
    ///            UILink instances (array and element instance).
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerIsAvailableHandler (long lPos, LINK_SELECTION_TYPE::PFctIsAvailable pHandler) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///            In case of array parameters, the given handler will be registered to both
    ///            UILink instances (array and element instance).
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerIsVisibleHandler (long lPos, MrUILinkBase::PFctIsVisible pHandler) const;


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSetValueHandler (long lPos, LINK_LONG_TYPE::PFctSetValue pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSetValueHandler (long lPos, LINK_DOUBLE_TYPE::PFctSetValue pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSetValueHandler (long lPos, LINK_BOOL_TYPE::PFctSetValue pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSetValueHandler (pHandler);
        }
    }
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSetValueHandler (long lPos, LINK_SELECTION_TYPE::PFctSetValue pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSetValueHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSolveHandler (long lPos, LINK_LONG_TYPE::PFctSolve pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSolveHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSolveHandler (long lPos, LINK_DOUBLE_TYPE::PFctSolve pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSolveHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSolveHandler (long lPos, LINK_BOOL_TYPE::PFctSolve pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSolveHandler (pHandler);
        }
    }
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerSolveHandler (long lPos, LINK_SELECTION_TYPE::PFctSolve pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerSolveHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerTryHandler (long lPos, LINK_LONG_TYPE::PFctTry pHandler) const
    {
        LINK_LONG_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerTryHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerTryHandler (long lPos, LINK_DOUBLE_TYPE::PFctTry pHandler) const
    {
        LINK_DOUBLE_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerTryHandler (pHandler);
        }
    }


    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerTryHandler (long lPos, LINK_BOOL_TYPE::PFctTry pHandler) const
    {
        LINK_BOOL_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerTryHandler (pHandler);
        }
    }
    
    
    // --------------------------------------------------------------------------------------------
    ///  \brief    Registers the given handler for the WIP parameter at the given position.
    ///
    ///  \param    lPos      Position on the parameter card
    ///  \param    pHandler  Pointer to the handler method
    ///
    ///  \throw    exception  If no WIP parameter is registered at the given position
    ///  \throw    exception  If the UI link object does not have the correct type
    // --------------------------------------------------------------------------------------------
    __IMP_EXP void registerTryHandler (long lPos, LINK_SELECTION_TYPE::PFctTry pHandler) const
    {
        LINK_SELECTION_TYPE* pUILink = NULL;

        if (getUILinkAs (lPos, pUILink)) {
            pUILink->registerTryHandler (pHandler);
        }
    }

#endif


private:

    // This map stores all parameters registered to this WIPParameterTool instance
    ParamMap m_ParamMap;

    const SeqIF* m_pSequence;

    // These vectors store which elements within the WIPMemBlocks are already reserved
    std::vector <bool> m_ReservedInLongArray;
    std::vector <bool> m_ReservedInDoubleArray;

    static long lMaxNumberOfWIPParameters;

    static std::vector <WIPParameterTool*> m_Instances;


    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Returns the associated WIPParameterTool instance for the given sequence.
    ///
    ///  \param    pSequence  Pointer to the sequence
    ///
    ///  \return   A pointer to the associated WIPParameterTool instance or NULL.
    // --------------------------------------------------------------------------------------------
    static WIPParameterTool* getInstanceFor (const SeqIF& rSequence);


    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Adds the given WIPParam object to the internally stored list.
    ///
    ///  \param    rParam  Reference to the WIPParam object to be added
    // --------------------------------------------------------------------------------------------
    void addParam (WIPParam& rParam);


#ifdef WIN32

    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Tells whether the default values were already set in the UI.
    ///
    ///  \see      WIPParameterTool::setDefaultValues
    ///  \see      WIPParameterTool::setDefaultValuesInitialized
    ///
    ///  \param    rMrProt  Reference to the measurement protocol
    ///
    ///  \return   \arg \c true  : If the default values were already set
    ///            \arg \c false : Otherwise
    // --------------------------------------------------------------------------------------------
    bool getDefaultValuesInitialized (MrProt& rMrProt) const;


    // --------------------------------------------------------------------------------------------
    ///  \internal
    ///  \brief    Stores the information about the initialization status in the provided
    ///            measurement protocol.
    ///
    ///  \see      WIPParameterTool::setDefaultValues
    ///  \see      WIPParameterTool::getDefaultValuesInitialized
    ///
    ///  \param    rMrProt       Reference to the measurement protocol
    ///  \param    bInitialized  Initialization status
    // --------------------------------------------------------------------------------------------
    void setDefaultValuesInitialized (MrProt& rMrProt, bool bInitialized) const;

#endif
};

} // end namespace WPT_NAMESPACE
