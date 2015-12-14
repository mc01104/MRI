/*! 
***************************************************************************
\file   GPABalance.h

\brief  Class for consideration the effect GPA limitations on gradient pulse trains

This file provides the class GPABalance.
For further comments, please refer to the class description.

\author Thorsten Feiweier (Thorsten.Feiweier@siemens.com)

<b>Archive Information:</b>
\verbatim
- File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libBalance\GPABalance.h
- Last Author:  feiwthu3
- Archive Date: 2011-09-21 15:43:28 +02:00
\endverbatim

\b Language: C++

\b Copyright: &copy; Siemens AG (http://www.siemensmedical.com).
All rights reserved.   

***************************************************************************

\changed     20-Jun-2007; T.Feiweier; VB15A; CHARM: n.a.
\requirement N4_elh_DIFF_DiffusionTensorImaging
\description
	- first version

***************************************************************************

\changed     01-Jan-2009; T.Feiweier; VB15A; CHARM: n.a.
\requirement N4_elh_DIFF_DiffusionTensorImaging
\description
	- include supervision of nominal gradient amplitude

*/

/// double include protection:
#ifndef GPABalance_h
#define GPABalance_h 1

// ---------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------
// Assignment logical axes => phyiscal axes
#define GPABALANCE_X_AXIS SEQ::AXIS_PHASE
#define GPABALANCE_Y_AXIS SEQ::AXIS_READOUT
#define GPABALANCE_Z_AXIS SEQ::AXIS_SLICE
#define GPABALANCE_UNDEF_AXIS SEQ::AXIS_UNDEFINED

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include <vector>                                               /**< STL vector                     */
#include "MrServers/MrMeasSrv/SeqIF/libRT/sRF_PULSE.h"          /**< RF_PULSE and TXTYPE definition */
#include "MrServers/MrMeasSrv/SeqIF/libRT/sGRAD_PULSE.h"        /**< GRAD_PULSE definition          */
#include "MrServers/MrImaging/libINIAccess/INIAccess.h"         /**< INIAccess definition           */

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
const long   lGPABalanceStatusLength = 255;                     /**< Maximum length of status messages                  */
const long   lInfRepeatability       = 0x100000;                /**< Placeholder for infinite repeatability             */

// ---------------------------------------------------------------------------
// Type definitions
// ---------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Import/Export control
// -----------------------------------------------------------------------------
#ifdef BUILD_libBalance
  #define __OWNER
#endif
#include "MrCommon/MrGlobalDefinitions/ImpExpCtrl.h"


// ===========================================================================
/*!
\class GPABalance

\brief This class implements a generic GPA limitations calculation based on
       a series of gradient events (trapezoidal gradients).

  The class GPABalance models the internal supervision (dutycycle and ohmloss)
  of the GPA. Given a series of gradient pulses, it checks individually
  for each axis whether the GPA will allow to play out the series and if not, 
  at which gradient pulse it may fail. With respect to tolerances, conservative 
  assumptions apply - this ensures that the results of the calculations hold
  for any scanner. A method for checking the repeatability of a given series
  of gradient events and for calculating the duration of pauses required
  for an infinite repeatability are also available.

  In general, the calculation is based on a balance model: applying gradient
  pulses adds something to the account, while during pauses the account
  relaxes with a certain time constant. If a certain balance is exceeded, the 
  system will fail. The implementation allows to consider the residual balance 
  of a preceeding series of pulses - this allows for concatenating several 
  instances of the GPABalance class. Note that the order of events is quite
  important: if a series A - B works, this does not necessarily mean that 
  B - A will also run.

  Calculation takes place based on a list of trapezoidal gradient pulses
  which might even overlap. Events are collected in an ordered list (vector), 
  which is the basis of the balance calculation. Each gradient axis is
  treated separately, there are no interdependencies between different
  axes. Thus, it is usually sufficient to consider a 'worst case' axis.
  Sinusoidal gradient pulses are converted to a conservative equivalent.


  In addition to the GPA supervision, additional methods are provided that
  consider thermal longterm effects of the whole gradient system. No time
  constants are associated with these effects, and 'simple' averages are
  calculated in order to check against the corresponding limits. While in
  principle it would be possible to include these calculations in the 
  balance evaluations (see above), it turns out to be more effective ot
  handle them separately. Not only individual limitations of each axis
  but also sum effects of all axes are considered. The methods allow to
  check for infinite repeatability and to calculate pause durations that
  allow for infinite repeatability. All methods related to these long
  term effects include to tag 'RMS' in their name.


  Please note that only a limited number of systems / GPAs are supported
  by this implementation (use the GetStatus method to check).


\author Thorsten.Feiweier@siemens.com

*/
// ===========================================================================
class __IMP_EXP GPABalance
{
public:
    // ------------------------------------
    // Public typedefs and definitions
    // ------------------------------------

    /// Structure holds values required for nominal gradient amplitude supervision (root mean square)
    typedef struct
    {
        double dGrmsXP;                                       /**< x-axis: rms amplitude, positive offset [mT/m] */
        double dGrmsYP;                                       /**< y-axis: rms amplitude, positive offset [mT/m] */
        double dGrmsZP;                                       /**< z-axis: rms amplitude, positive offset [mT/m] */
        double dGrmsXN;                                       /**< x-axis: rms amplitude, negative offset [mT/m] */
        double dGrmsYN;                                       /**< y-axis: rms amplitude, negative offset [mT/m] */
        double dGrmsZN;                                       /**< z-axis: rms amplitude, negative offset [mT/m] */
        double dTime;                                         /**< corresponding averaging time  [us]   */
    } GrmsValue;

    /// Structure holds balance value for each account
    typedef struct
    {
        double dSP;                                         /**< Balance of account SP */
        double dSN;                                         /**< Balance of account SN */
        double      dSOP;                                        /**< Balance of account SOP */
        double      dSON;                                        /**< Balance of account SON */
    } GPABalanceValue;
    
    // ------------------------------------
    // Public methods
    // ------------------------------------

    // ------------------------------------
    // Constructors and deconstructors

    ///	The constructor sets up an empty event list.
    GPABalance (void);

    ///	The constructor sets up an empty event list and connects an INIAcess database.
    GPABalance (
        INIAccess* pConfig_in       /**< Imp: pointer to an INIAccess instance */
        );

	/// Copy constructor: used e.g. within function calls passing a GPABalance instance
	GPABalance (
        const GPABalance &OrigGPABalance    /**< Imp: instance to copy */
        );

	// Assignment operator: copy content of original instance
	const GPABalance& operator =(
        const GPABalance &OrigGPABalance    /**< Imp: instance to assign */
        );

    /// This destructor frees allocated memory.
    virtual ~GPABalance (void);

    /// Initialize member variables and clear event list.
    /** \b Input: 
        \n n.a.

        \b Output:
        \n all GPA model related system specific parameters, m_lStatus

        If invalid GPA model parameters are detected, m_lStatus and the
        corresponding error message are set (see below).

        Note: in order to guarantee the same behaviour of this module
        on all scanners of the same type, it is neccessary to use a
        conservative parameterization.
    */
    void Reset (void);

    /// Reset a balance value
    /** \b Input:
        \n sBalance

        \b Output:
        \n zero balance
    */
    static void ResetBalanceValue (GPABalanceValue &sBalance);

    /// Reset a rms gradient value
    /** \b Input:
        \n sGrms

        \b Output:
        \n zero Grms
    */
    static void ResetRMSValue (GrmsValue &sGrms);

    /// Check whether GPA balance models are support by current scanner
    /** \b Input: 
        \n n.a.

        \b Output:
        \n true if scanner supports balance models
    */
    static bool bIsBalanceModelSupported ( void );

    /// Return the error status of this module.
    /**       
      	\b Input:
      	\n bDumpMessage
      
      	\b Output:
        \n Trace of status message

        \b Return value:
        \n GPABalance error status (0 = no error)
    */
    long lGetStatus (
        bool bDumpMessage = false       /**< Imp: Switch enables trace dump of status message */
        );

    // ------------------------------------
    // GPA balance related methods

    /// Check whether the series of gradient events will exceed GPA limitations
    /** \b Input:
      	\n eAxis, sInitialBalance, m_sEventMap, GPA model parameters

        \b Output:
        \n sFinalBalance (balances <0 indicate that the corresponding check did not
        take place, see below), lDuration

      	\b Return value:
      	\n  0:   limits not exceeded
        \n >0:  ID of first event that overrides the GPA

        The calculation starts with the provided initial balance and returns the final
        balance of all acounts. This can be used e.g. for a concatenation of several
        gradient event series.

        The accounts are checked successively (order: positive - negative - ohmloss).
        If one check fails, the succeeding ones are not run.

        Calculation start at start time 0 and ends with the end of the last gradient pulse.
        If a pause after the last gradient pulse is required, an empty event has to be inserted.

        If an error status is present, no calculation is performed and -2 is returned.
    */
    long lCalc (
        const SEQ::GradientAxis  eAxis,                  /**< Imp: Gradient axis to evaluate       */
        const GPABalanceValue    sInitialBalance,        /**< Imp: Balance at start of first event */
        GPABalanceValue         &sFinalBalance           /**< Exp: Balance at end of last event    */
        );


    /// Calculate balance change due to enclosed gradient events
    /**
        \b Input:
        \n eAxis, sInitialBalance, m_sBalanceChange (will be calculated if no valid results is available)

        \b Output:
        \n sFinalBalance

        \b Return value:
        \n true:  success
        \n false: calculation was neccessary and did not succeed (limits exceeded) 

        Given an initial balance and the series of GPA events, the final balance of all accounts is
        calculated for the given gradient axis. This method is useful for the concatenation of
        several GPABalance instances.

        If no valid result is available yet, a calculation (lCalc) with zero 
        initial balance will be performed. If the calculation fails (GPA limits
        exceeded), the final balance will be set to zero and false is returned. 
    */
    bool bCalcBalanceChange (
        const SEQ::GradientAxis       eAxis,                 /**< Imp: Gradient axis to evaluate             */
        const GPABalanceValue  sInitialBalance,              /**< Imp: Initial balance                       */
        GPABalanceValue       &sFinalBalance                 /**< Exp: Final balance (includes evolution of initial balance) */
        );

    // Calculate balance change (generic method)
    /**
        \b Input:
        \n sInitialBalance, sBalanceChange, lDuration, lRepetitions

        \b Output:
        \n sFinalBalance

        \b Return value:
        \n true:  success
        \n false: limits exceeded

        Same functionality as above - but variables of type GPABalanceValue are used as
        input. This allows e.g. to calculate the repeatability of the concatenation of several
        GPABalance instances. The currently stored gradient events are NOT used here - everything
        is calculated based on the provided sDeltaBalance and lDuration.

        Please keep in mind that the detailed timinig of each repetition is NOT investigated!
        This means that only a rough estimation of the possible number of repetitions is
        possible. 
    */
    bool bCalcBalanceChange (
        const GPABalanceValue  sInitialBalance,              /**< Imp: Initial balance                       */
        GPABalanceValue       &sFinalBalance,                /**< Exp: Final balance (includes evolution of initial balance) */
        const GPABalanceValue &sBalanceChange,               /**< Imp: Balance change within given duration  */
        const long             lDuration,                    /**< Imp: Total duration to consider [us]       */
        const long             lRepetitions = 1              /**< Imp: Number of repetitions to consider     */
        );

    /// Check for repeatability
    /**
        \b Input:
        \n eAxis, m_sBalanceChange (calculations take place if neccessary)

        \b Return value:
        \n Number of possible repetitions (if < lInfRepeatability) or lInfRepeatability (otherwise).


        A rough (!) check is performed how often an event block of a certain duration with given 
        change of balances can be repeated. 

        If an error status is present, no calculation is performed and -2 is returned.
    */
    long lRepeatabilityCheck (
        const SEQ::GradientAxis eAxis                /**< Imp: Gradient axis to evaluate             */
        );

    /// Check repeatability (generic method)
    /**
        \b Input:
        \n sBalanceChange, lDuration

        \b Return value:
        \n Number of possible repetitions (if < lInfRepeatability) or lInfRepeatability (otherwise)

        Same functionality as above - but variables of type GPABalanceValue are used as the
        input. This allows e.g. to calculate the repeatability of the concatenation of several
        GPABalance instances. The currently stored gradient events are NOT used here - everything
        is calculated based on the provided sBalanceChange and lDuration.

        Please keep in mind that the detailed timinig of each repetition is NOT investigated!
        This means that only a rough estimation of the possible number of repetitions is
        possible. 
    */
    long lRepeatabilityCheck (
        const GPABalanceValue &sBalanceChange,          /**< Imp: Balance change within given duration */
        const long             lDuration                /**< Imp: Total duration to consider [us]      */
        );

    /// Calculate required pause for infinite repeatability
    /** \b Input:
        \n eAxis, m_sBalanceChange (calculations take place if neccessary)    

        \b Return value:
        \n Duration of required pause [us]

        Given the enclosed gradient event series, the duration of an additional pause is calculated that
        would allow for the desired repeatability. During this pause, no gradient activity (including
        shim offset currents) should be present.

        If the enclosed gradient event series cannot be applied at all, -1 is returned. If an error 
        status is present, no calculation is performed and -2 is returned.


        Please keep in mind that the detailed timinig of each repetition is NOT investigated!
        This means that only a rough estimation of the required pause duration is possible. 
    */
    long lCalcPause (
        const SEQ::GradientAxis eAxis                   /**< Imp: Gradient Axis to evaluate      */
        );


    /// Calculate required pause for infinite repeatability (generic method)
    /**
        \b Input:
        \n sBalanceChange, lDuration

        \b Return value:
        \n Duration of required pause [us]

        Same functionality as above - but variables of type GPABalanceValue are used as the
        input. This allows e.g. to calculate the required pause for the concatenation of several
        GPABalance instances. The currently stored gradient events are NOT used here - everything
        is calculated based on the provided sBalanceChange and lDuration.

        Please keep in mind that the detailed timinig of each repetition is NOT investigated!
        This means that only a rough estimation of the required pause is possible. 
    */
    long lCalcPause (
        const GPABalanceValue &sBalanceChange,          /**< Imp: Balance change within given duration */
        const long             lDuration                /**< Imp: Total duration to consider [us]      */
        );

    // ------------------------------------
    // Grms balance related methods

    /// Calculate rms gradient change due to enclosed gradient events
    /**
        \b Input:
        \n sInitialGrms, m_lFrequency, m_sGrms (will be calculated if no valid results is available)

        \b Output:
        \n sFinalGrms

        \b Return value:
        \n true:  success
        \n false: error 

        Given the rms gradient value of the preceding series of GPA events, the rms value after
        concatenating the included series of GPA events is calculated. All axes are considered.
        No evaluations take place - the final rms gradient value can e.g. be passed to a
        repeatability check or to a pause calculation (see below).
        The dominating frequency component of the gradient event series should be provided in
        advance using ::bSetFrequency().
        
        If the calculation fails the final balance will be set to the initial value and false is 
        returned.
    */
    bool bCalcRMS( 
        const GrmsValue  sInitialGrms,                  /**< Imp: Initial Grms               */
        GrmsValue       &sFinalGrms                     /**< Exp: Final Grms                 */
        );

    /// Check for infinite repeatability - single axis
    /**
        \b Input:
        \n eAxis, m_sGrms (calculations take place if neccessary)

        \b Return value:
        \n true:  infinite repeatability possible
        \n false: infinite repeatability not possible


        A check is performed if the given series of gradient events can be repeated forever.
        If called with default parameters, each individual axis as well as sum effects are
        evaluated.

        If an error status is present, no calculation is performed and -2 is returned.
    */
    bool bRepeatabilityCheckRMS(
        const SEQ::GradientAxis eAxis = GPABALANCE_UNDEF_AXIS  /**< Imp: Gradient Axis to evaluate      */
        );


    /// Check for infinite repeatability (generic method)
    /**
        \b Input:
        \n eAxis, sGrms

        \b Return value:
        \n true:  infinite repeatability possible
        \n false: infinite repeatability not possible

        Same functionality as above - but variables of type GrmsValue are used as the
        input. This allows e.g. to calculate the repeatability of the concatenation of several
        GPABalance instances. The currently stored gradient events are NOT used here - everything
        is calculated based on the provided sGrmsChange.
    */
    bool bRepeatabilityCheckRMS(
        const GrmsValue        &sGrms,                          /**< Imp: rms gradient to evaluate   */
        const SEQ::GradientAxis eAxis = GPABALANCE_UNDEF_AXIS   /**< Imp: Gradient Axis to evaluate  */
        );

    /// Calculate required pause for infinite repeatability
    /** \b Input:
        \n eAxis, m_sGrms (calculations take place if neccessary)    

        \b Return value:
        \n Duration of required pause [us]

        Given the enclosed gradient event series, the duration of an additional pause is calculated that
        would allow for infinite repeatability. During this pause, no gradient activity (including
        shim offset currents) should be present.
        If called with default parameters, each individual axis as well as sum effects are
        evaluated.

        If the enclosed gradient event series cannot be applied at all, -1 is returned. If an error 
        status is present, no calculation is performed and -2 is returned.
    */
    long lCalcPauseRMS (
        const SEQ::GradientAxis eAxis = GPABALANCE_UNDEF_AXIS  /**< Imp: Gradient Axis to evaluate      */
        );

    /// Calculate required pause for infinite repeatability (generic method)
    /**
        \b Input:
        \n sGrms, lDuration

        \b Return value:
        \n Duration of required pause [us]

        Same functionality as above - but variables of type GrmsValue are used as the
        input. This allows e.g. to calculate the required pause for the concatenation of several
        GPABalance instances. The currently stored gradient events are NOT used here - everything
        is calculated based on the provided sGrms.
    */
    long lCalcPauseRMS (
        const GrmsValue &sGrms,                                /**< Imp: rms gradient to evaluate   */
        const SEQ::GradientAxis eAxis = GPABALANCE_UNDEF_AXIS  /**< Imp: Gradient Axis to evaluate  */
        );

    // ------------------------------------
    // Common administration methods

    /// Set dominating frequency component of gradient events.
    /**       
      	\b Input:
      	\n lFrequency

        \b Output:
        \n m_lFrequency
      
      	\b Return value:
        \n  true:  success
        \n  false: error

        The provided frequency should represent the dominating frequency component of
        the gradient event series. This is required to take into account frequency
        dependent losses (higher frequency = higher loss). If a negative value is
        provided, worst case assumptions apply.
    */
    bool bSetFrequency ( 
        const long lFrequency                     /**< Imp: Frequency [Hz] */
        );

    /// Add gradient to event list.
    /**       
      	\b Input:
      	\n sGradient, lStartTime, eAxis

        \b Output:
        \n new entries in m_sEventMap, m_lStatus
      
      	\b Return value:
        \n  0:  event not added   (e.g. SEQ::AXIS_UNDEFINED, ramp time zero)
        \n >0: event ID
    */
    long int lAddEvent (
        const long lStartTime,                                /**< Imp: Event start time >= 0 [us] */
        const sGRAD_PULSE_TRAP &sGradient,                    /**< Imp: Gradient pulse with valid amplitude and timing */
        const SEQ::GradientAxis eAxis = SEQ::AXIS_UNDEFINED   /**< Imp: Gradient axis           
                                                                        (optional - default: get info from gradient pulse) */
        );

    /// Add gradient to event list.
    /**       
      	\b Input:
      	\n sGradient, lStartTime, eAxis

        \b Output:
        \n new entries in m_sEventMap, m_lStatus
      
      	\b Return value:
        \n  0:  event not added   (e.g. SEQ::AXIS_UNDEFINED, ramp time zero)
        \n >0: event ID
    */
    long int lAddEvent (
        const long lStartTime,                                /**< Imp: Event start time >= 0 [us] */
        const sGRAD_PULSE_SIN &sGradient,                     /**< Imp: Gradient pulse with valid amplitude and timing */
        const SEQ::GradientAxis eAxis = SEQ::AXIS_UNDEFINED   /**< Imp: Gradient axis           
                                                                        (optional - default: get info from gradient pulse) */
        );

    /// Add gradient to event list.
    /**       
      	\b Input:
        \n lStartTime, dAmplitude, lRampUp, lRampDown, lFlatTop, eAxis
      
        \b Output:
        \n four new entry in m_sEventMap (start ramp-up, end ramp-up,
        start ramp-down, end ramp-down), m_lStatus

      	\b Return value:
        \n  0:  event not added   (e.g. SEQ::AXIS_UNDEFINED, ramp time zero)
        \n >0: event ID
    */
    long int lAddGradient (
        const long lStartTime,            /**< Imp: Event start time [us] >= 0 */
        const double dAmplitude,          /**< Imp: Gradient amplitude [mT/m] */
        const long lRampUp,               /**< Imp: Ramp up time [us] */
        const long lRampDown,             /**< Imp: Ramp down time [us] */
        const long lFlatTop,              /**< Imp: Flat top time [us] */
        const SEQ::GradientAxis eAxis     /**< Imp: Gradient axis - one of SEQ::AXIS_PHASE, SEQ::AXIS_READ, SEQ::AXIS_SLICE
                                                    (convention: use PHASE for X, READ for Y, SLICE for Z) */
        );

    /// Add gradient to event list.
    /**       
      	\b Input:
        \n lStartTime, dAmplitude, lRampUp, lRampDown, lFlatTop, eAxis
      
        \b Output:
        \n four new entry in m_sEventMap (start ramp-up, end ramp-up,
        start ramp-down, end ramp-down), m_lStatus

      	\b Return value:
        \n  0:  event not added   (e.g. SEQ::AXIS_UNDEFINED, ramp time zero)
        \n >0: event ID
    */
    long int lAddGradientSin (
        const long lStartTime,            /**< Imp: Event start time [us] >= 0 */
        const double dAmplitude,          /**< Imp: Gradient amplitude [mT/m] */
        const long lRampUp,               /**< Imp: Ramp up time [us] */
        const long lRampDown,             /**< Imp: Ramp down time [us] */
        const long lFlatTop,              /**< Imp: Flat top time [us] */
        const SEQ::GradientAxis eAxis     /**< Imp: Gradient axis - one of SEQ::AXIS_PHASE, SEQ::AXIS_READ, SEQ::AXIS_SLICE
                                                    (convention: use PHASE for X, READ for Y, SLICE for Z) */
        );

    /// Add empty event to list.
    /**       
      	\b Input:
        \n lStartTime
      
        \b Output:
        \n 'do nothing' - entries in m_sEventMap, m_lStatus

      	\b Return value:
        \n  0:  event not added
        \n >0: event ID
    */
    long int lAddEmptyEvent (
        const long lStartTime             /**< Imp: Event start time [us] >= 0 */
        );

    /// Remove gradient from event list
    /**
        \b Input:
        \n lID

        \b Output:
        \n modified m_sEventMap

        \b Return value:
        \n true:  success
        \n false: no success (e.g. event ID not valid)
    */
    bool bRemoveEvent(
        const long lID                    /**< Imp: Event ID */
        );

    /// Scale amplitude of single gradient event with given factor
    /**
        \b Input:
        \n lID, dScale

        \b Output:
        \n modified event within m_sEventMap

        \b Return value:
        \n true:  success
        \n false: no success (e.g. event ID not valid)
    */
    bool bScaleEvent(
        const long   lID,                 /**< Imp: Event ID */
        const double dScale               /**< Imp: Scale factor */
        );

    /// Scale amplitude of multiple gradient events with given factors
    /**
        \b Input:
        \n lNumber, alID, adScale

        \b Output:
        \n modified event within m_sEventMap

        \b Return value:
        \n true:  success
        \n false: no success (e.g. event ID not valid)
    */
    bool bScaleEvent(
        const long   lNumber,             /**< Imp: Number of events   */
        const long   alID[],              /**< Imp: Event ID array     */
        const double adScale[]            /**< Imp: Scale factor array */
        );

    /// Get maximum usable gradient amplitude (valid for each axis and considering gradient offsets)
    /**
        \b Input:
        \n m_dGradSensitivity, m_dGPAMaxCurrent, m_dGradOffset

        \b Return value:
        \n maximum usable gradient amplitude [mT/m]
    */
    double dGetMaximumAmplitude (void);

    /// Get total duration of added events
    /**       
      	\b Input:
      	\n m_sEventMap
      
        \b Return value:
        \n duration of events [us]

        Calculation always start with time = 0. Thus, the total duration is identical
        to the end time of the last added gradient pulse.
    */
    long lGetDuration (void);

    /// Assign INIAccess database 
    /**
        \b Input:
        \n pConfig_in

        \b Output:
        \n m_pConfig

        \b Return value:
        \n success

        The class GPABalance makes use of Michael Zwangers tracing concept:
        based on the TRACE-settings within an ini-file, output can be
        enabled. For performance reasons, it is always required that
        the database is setup by someone else - if no database is provided,
        no output will be generated.
    */
    bool setIniAccess(
        INIAccess* pConfig_in       /**< Imp: pointer to an INIAccess instance */
        );


protected:

    // ------------------------------------
    // Protected typedefs and definitions
    // ------------------------------------

    /// This structure holds the information of each diffusion encoding event.
    typedef struct
    {
        long int lTime;                                     /**< Event time                                            */
        SEQ::GradientAxis eAxis;                            /**< Phase, read, slice or undefined (=> RF)               */
        double   dSlope;                                    /**< Gradient slope until next event [mT/m/us]             */
        long int lID;                                       /**< Event ID                                              */
        GPABalanceValue sBalanceChange;                     /**< Balance change due to current event - set within calculation */
    } GPABalanceEvent;  
    
    /// Comparator: needed for sorting the diffusion encoding events
    struct GPAEventMapCompClass
    {
        bool operator() (const GPABalanceEvent &sEvent1, const GPABalanceEvent &sEvent2)
        {
            return ( sEvent1.lTime < sEvent2.lTime);
        }
    } GPAEventMapComp;

    typedef std::vector< GPABalanceEvent >              GPAEventMap;
    typedef std::vector< GPABalanceEvent >::value_type  GPAEvent;
    typedef std::vector< GPABalanceEvent >::iterator    GPAEventMapIt;
    
    enum eAccountID {AccountID_P = 1, AccountID_N, AccountID_OP, AccountID_ON, AccountID_RMSP, AccountID_RMSN};
    
    // ------------------------------------
    // Protected methods
    // ------------------------------------

    /// Update the positive balance for the current time step
    /**
        \b Input:
        \n dBalance, dCurrentAmp, dNextAmp, dTimeStep, m_dTimeConstantP,
        m_dChargeConstantP1, m_dChargeConstantP2, m_dChargeOffsetP,
        m_dEddyReserve

        \b Output:
        \n dBalance

        For performance reasons, we use const references for some of the arguments.
    */
    void CalcBalanceP (double &dBalance, const double &dCurrentAmp, const double &dNextAmp, const double &dTimeStep);
    /// Update the negative balance for the current time step
    void CalcBalanceN (double &dBalance, const double &dCurrentAmp, const double &dNextAmp, const double &dTimeStep);
    /// Update the ohmloss balance for the current time step
    void CalcBalanceO (double &dBalance, const double &dCurrentAmp, const double &dNextAmp, const double &dTimeStep);


    /// Perform balance calculation
    /**
        \b Input:
        \n eAxis, sInitialBalance, iAccount, m_sEventMap

        \b Output:
        \n dFinalBalance (value <0 indicates that the corresponding check did not
        take place, see below), dDeltaBalance (balance change without taking into
        account the initial balance - only enclosed gradient events are considered).

        \b Return value:
      	\n  0:   limits not exceeded
        \n >0:  ID of first event that overrides the GPA

        This method is called by lCalc once for each of the three accounts (see definition
        of eAccoutID for the assignment). Since the calculation routines for each account
        are quite similar, this allows for better code maintenance. The performance
        drawback is tolerable.
    */
    long lCalcBalanceEvolution (
        const SEQ::GradientAxis eAxis,                        /**< Imp: Gradient axis to evaluate       */
        double                  dInitialBalance,              /**< Imp: Balance at start of first event */
        const eAccountID        eAccount,                     /**< Imp: Account to evaluate             */                 
        double                 &dFinalBalance,                /**< Exp: Balance at end of last event    */
        double                 &dDeltaBalance                 /**< Exp: Balance change due to enclosed gradient events */
        );

    /// Perform ms amplitude calculation
    /**
        \b Input:
        \n sInitialRMS, eAccount, m_sEventMap

        \b Output:
        \n sFinalRMS

        \b Return value:
      	\n    0:   ok
        \n  !=0:   error

        This method is called by lCalcRMS. All axes are considered.
    */
    long lCalcRMSEvolution (
        GrmsValue              sInitialRMS,              /**< Imp: rms gradient of preceding gradient events  */
        const eAccountID       eAccount,                 /**< Imp: Account to evaluate                        */                 
        GrmsValue             &sFinalRMS                 /**< Exp: rms gradient including stored events       */
        );

    /// Set up a sorted list of three values
    /**
        \b Input:
        \ndSmallValue < dLargeValue, dUserValue, bInc

        \b Output:
        \n m_dBorderLimits[0 ... 2]

        \b Return value:
        \n index where dUserValue has been sorted in

        For two of the three values, the smaller and larger one are already known (pre-sorted list).
        The task is to sort in the third value at the appropriate position. Sorting can be
        in ascending or in descending order (determined by bInc). 

        For performance reasons, we use const references for some of the arguments.
    */
    int iSetBorders   (const double &dSmallValue, const double &dLargeValue, const double &dUserValue, const bool &bInc);

    /// Set up a sorted list of four values
    /**
        \b Input:
        \n dSmallValue < dMidValue < dLargeValue, dUserValue, bInc

        \b Output:
        \n m_dBorderLimits[0 ... 3]

        \b Return value:
        \n index where dUserValue has been sorted in

        For three of the four values, the sort order is already known (pre-sorted list).
        The task is to sort in the fourth value at the appropriate position. Sorting can be
        in ascending or in descending order (determined by bInc). 

        For performance reasons, we use const references for some of the arguments.
    */
    int iSetBorders   (const double &dSmallValue, const double &dMidValue, const double &dLargeValue, const double &dUserValue, const bool &bInc);


    /// Check repeatability
    /**
        Helper method used by the two public methods defined above. For details, refer to them.
    */
    long lRepeatabilityCheck (const GPABalanceValue &sBalanceChange, const double dDuration);

    /// Calculate pause
    /**
        Helper method used by the two public methods defined above. For details, refer to them.
    */
    long lCalcPause (const GPABalanceValue &sBalanceChange, const double dDuration);

    // ------------------------------------
    // Member variables
    // ------------------------------------
    // If there are any changes to the member variables,
    // remember to update the assignment operator,
    // the copy constructor and the Reset method!

    /// Error status of this module
    long   m_lStatus;
    /// Error status text of this module
    char   m_pszStatus[lGPABalanceStatusLength + 1];
    /// Counter for event IDs
    long   m_lEventCounter;

    /// Sorted list of diffusion encoding events. See definition of GPAEventMap.
    GPAEventMap m_sEventMap;

    /// Dominating frequency content of the gradient event series [Hz]
    long   m_lFrequency;

    /// Gradient sensitivity, worst case axis (i.e. smallest value) [mT/m/A]
    double m_dGradSensitivity;

    /// Maximum gradient amplitude [mT/m]
    double m_dGradMaxAmplitude;

    /// GPA model time constant: positive current [us]
    double m_dTimeConstantP;
    /// GPA model time constant: negative current [us]
    double m_dTimeConstantN;
    /// GPA model time constant: ohmloss          [us]
    double m_dTimeConstantO;
    /// GPA model charge constant 1: positive current [1 / normalized current units]
    double m_dChargeConstantP1;
    /// GPA model charge constant 2: positive current [1 / normalized current units]
    double m_dChargeConstantP2;
    /// GPA model charge constant 1: negative current [1 / normalized current units]
    double m_dChargeConstantN1;
    /// GPA model charge constant 2: negative current [1 / normalized current units]
    double m_dChargeConstantN2;
    /// GPA model charge constant 1: ohmloss          [1 / normalized current units]
    double m_dChargeConstantO1;
    /// GPA model charge constant 2: ohmloss          [1 / normalized current units]
    double m_dChargeConstantO2;
    /// GPA model charge offset: positive current [normalized current units, > 0]
    double m_dChargeOffsetP;
    /// GPA model charge offset: negative current [normalized current units, > 0]
    double m_dChargeOffsetN;
    /// GPA model charge offset: ohmloss          [normalized current units, > 0]
    double m_dChargeOffsetO;

    /// Current normalization constant [1/A]
    double m_dCurrentNorm;

    /// Gradient offset, worst case (i.e. largest value) [mT/m, > 0]
    double m_dGradOffset;

    /// Eddy current reserve, worst case (i.e. largest value) [relative value]
    double m_dEddyReserve;

    /// Nominal gradient amplitude, used for ms calculations [mT/m]
    double m_dGradAmplNominal;

    /// Indicator for valid calculation results - speeds up succeeding calls of lCalc
    bool   m_bValidResult;

    /// Indicator for valid calculation results - speeds up succeeding calls of bCalcRMS
    bool   m_bValidResultRMS;

    /// Indicator for sorted event list (by time)
    bool   m_bSortedEvents;

    /// Axis for which the calculation is valid
    SEQ::GradientAxis m_eValidAxis;

    /// Helper array for CalcBalance methods
    double m_dBorderLimits[5];

    /// Debug level for trace output
    long   m_lDebugLevel;

    /// Pointer to IniAccess interface
    INIAccess* m_pConfig;

    /// Total balance change due to enclosed gradient events (valid only after successul calculation)
    GPABalanceValue m_sBalanceChange;

    /// rms gradient values due to enclosed gradient events (valid only after successul calculation)
    GrmsValue m_sGrms;
};

#endif

