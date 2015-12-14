//	-----------------------------------------------------------------------------
//	  
//	-----------------------------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\actuationCloseLoopV3\SE_Track_Kernel.h
//	 Version:
//	  Author: 
//	    Date: 12-20-2013
//
//	    Lang: C++
//
//	 Descrip: 
//
//
//  ***************************************************************************/


#ifndef _SETRACKKERNELL_H_
#define _SETRACKKERNELL_H_


//  -------------------------------------------------------------------------- 
//  General Includes                                                           
//  -------------------------------------------------------------------------- 
#include "MrServers/MrImaging/libSBB/StdSeqIF.h"
#include "MrServers/MrMeasSrv/MeasUtils/nlsmac.h"       // * definition of type NLS_STATUS  *


class SE_Track_Kernel
{
public:
	SE_Track_Kernel(void);
	~SE_Track_Kernel(void);
	//  -------------------------------------------------------------------------- 
        //                                                                             
        //  Name        :  SE_Track_Kernel::prepare                                              
        //                                                                             
        //  Description :  
        /// \brief <b>     Preparation of the sequence during binary search and prior  
        ///                 to sequence execution  </b>                                    
        //                                                                             
        //  Return      :  NLS status                                                  
        //                                                                             
        //  -------------------------------------------------------------------------- 
        virtual NLSStatus prepare (MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo);

        //   -------------------------------------------------------------------------- 
        //                                                                              
        //   Name        :  SE_Track_Kernel::runKernel                                            
        //                                                                              
        //   Description :  
        ///  \brief <b>     Executes the basic timing of the real-time sequence.   </b>     
        ///                                                                              
        ///                 The method runKernel plays out a sequence "Kernel",
        ///                  consisting of one or more lines in k-Space.
        ///                 It is called by SeqLoop.
        ///                                                                              
        //   Return      :  NLS status                                                  
        //                                                                              
        //   -------------------------------------------------------------------------- 
        virtual NLS_STATUS runKernel(MrProt &rMrProt, SeqLim &rSeqLim, SeqExpo &rSeqExpo, long lKernelMode, long lSlice, long lPartition, long lLine);


protected:

};
