//	---------------------------------------------------------
//	  Copyright (C) Siemens AG 1998  All Rights Reserved.
//	---------------------------------------------------------
//
//	 Project: NUMARIS/4
//	    File: \n4_servers1\pkg\MrServers\MrImaging\seq\common\libPace\VectPat.h
//	 Version:
//	  Author: stemal8q
//	    Date: n.a.
//
//	    Lang: C++
//
//	 Descrip: Definition of struct VectPat.
//
//	---------------------------------------------------------

#ifndef __VECTPAT_H
#define __VECTPAT_H 1


//	---------------------------------------------------------
//  Used interfaces
//	---------------------------------------------------------

//  Definition of struct SEQ
#ifdef _VB_LINE
#include "MrServers/MrProtSrv/MrProt/SeqDefines.h"
#else
#include "ProtBasic/Interfaces/VectorDefines.h"
#endif

//  Definition of class ostream
#include <iostream>

//  Definition of sqrt
#include <math.h>

//  Definition of DBL_MIN
#include <float.h>

namespace PACE
{
struct VectPat
{
    //	The type describes a three dimensional vector into coordinates of a patient
    //	based system.
	//	          ^ tra
	//	          |
	//	        +-+-+     cor
	//	        |o|o|     /
	//	        +-+-+   /
	//	      >---|---<
	//	          | /
	//	   -------+-------> sag
	//	        / |
	//	      /  /|\
	//	    /   / | \
	//	      _/  |  \_

    typedef VectPat MYTYPE;

    enum
    {
        UNDEFINED  = -1
    ,   SAGITTAL   = SEQ::SAGITTAL
    ,   CORONAL    = SEQ::CORONAL
    ,   TRANSVERSE = SEQ::TRANSVERSE
    ,   R_TO_L     = SEQ::R_TO_L      // (R)ight to (L)eft
    ,   L_TO_R     = SEQ::L_TO_R      // (L)eft to (R)ight
    ,   A_TO_P     = SEQ::A_TO_P      // (A)nterior to (P)osterior
    ,   P_TO_A     = SEQ::P_TO_A      // (P)osterior to (A)nterior
    ,   F_TO_H     = SEQ::F_TO_H      // (F)eet to (H)ead
    ,   H_TO_F     = SEQ::H_TO_F      // (H)ead to (F)eet
    };
    
    double m_dSag;
    double m_dCor;
    double m_dTra;
    
    //  Default constructor
    VectPat(double dSag = 0, double dCor = 0, double dTra = 0)
        : m_dSag(dSag)
        , m_dCor(dCor)
        , m_dTra(dTra)
    {}
    
    static void e_sag(MYTYPE& _e)
    {
        _e.m_dSag = 1.;
        _e.m_dCor = 0.;
        _e.m_dTra = 0.;
    }
    
    static void e_cor(MYTYPE& _e)
    {
        _e.m_dSag = 0.;
        _e.m_dCor = 1.;
        _e.m_dTra = 0.;
    }
    
    static void e_tra(MYTYPE& _e)
    {
        _e.m_dSag = 0.;
        _e.m_dCor = 0.;
        _e.m_dTra = 1.;
    }
    
    static void null(MYTYPE& _e)
    {
        _e.m_dSag = 0.;
        _e.m_dCor = 0.;
        _e.m_dTra = 0.;
    } 
    
    //	Constructor.
    VectPat (void (*_f)(MYTYPE&))
    {
        (*_f)(*this);
    }
    //	Returns sagittal component.
    const double& sag (void ) const
    {
        return m_dSag;
    }
    
    //	Returns coronal component.
    const double& cor (void ) const
    {
        return m_dCor;
    }
    
    //	Return transverse component.
    const double& tra (void ) const
    {
        return m_dTra;
    }
    
    //	Sets sagittal component.
    double& sag (double value)
    {
        return (m_dSag = value);
    }
    
    //	Sets coronal component
    double& cor (double value)
    {
        return (m_dCor = value);
    }
    
    //	Sets transverse component.
    double& tra (double value)
    {
        return (m_dTra = value);
    }
    
    
    //	Returns false, if applied to the null vector, true else.
    inline bool normalize();
        
    //  Assignment operator
    MYTYPE& operator = (const MYTYPE& rRHS)
    {
        if(this != &rRHS)
        {
            m_dSag = rRHS.m_dSag;
            m_dCor = rRHS.m_dCor;
            m_dTra = rRHS.m_dTra;
        }
        return *this;	
    }
    
    //  Assignment operator
    MYTYPE& operator = (void (*_f)(MYTYPE&))
    {
        (*_f)(*this);
        return *this;
    }
};

// operator /=
inline VectPat& operator /= (VectPat& rLHS, double rhs)
{
    rLHS.m_dSag/=rhs;
    rLHS.m_dCor/=rhs;
    rLHS.m_dTra/=rhs;
    return rLHS;
}

//  operator*=
inline VectPat& operator *= (VectPat& rLHS, double rhs)
{
    rLHS.m_dSag*=rhs;
    rLHS.m_dCor*=rhs;
    rLHS.m_dTra*=rhs;
    return rLHS;
}

inline VectPat & operator += (VectPat & rLHS, const VectPat& rRHS)
{
    rLHS.m_dSag += rRHS.m_dSag;
    rLHS.m_dCor += rRHS.m_dCor;
    rLHS.m_dTra += rRHS.m_dTra;
    return (rLHS);
}

inline VectPat & operator -= (VectPat & rLHS, const VectPat & rRHS)
{
    rLHS.m_dSag -= rRHS.m_dSag;
    rLHS.m_dCor -= rRHS.m_dCor;
    rLHS.m_dTra -= rRHS.m_dTra;
    return (rLHS);
}

//  Returns absolute value of x.
inline double abs (const VectPat& x)
{
    return sqrt(x.m_dSag*x.m_dSag+x.m_dCor*x.m_dCor+x.m_dTra*x.m_dTra);
}

inline bool VectPat::normalize()
{
    const double _len = abs(*this);
    if(_len < DBL_MIN)
    {
        return false;
    }
    *this/=_len;
    return true;
}

//  operator<<
inline std::ostream& operator << (std::ostream& rLHS, const VectPat& rRHS)
{
    rLHS << '(' << rRHS.m_dSag << ", " << rRHS.m_dCor << ", " << rRHS.m_dTra << ')';
    return rLHS;
}

//  The function returns scalar product of rLHS and rRHS
inline double scalar_product (const VectPat& rLHS, const VectPat& rRHS)
{
    return (rLHS.m_dSag*rRHS.m_dSag + rLHS.m_dCor*rRHS.m_dCor + rLHS.m_dTra*rRHS.m_dTra);
}

//  The function returns vector product of rLHS and rRHS
inline VectPat vector_product (const VectPat& rLHS, const VectPat& rRHS)
{
    return VectPat(
        rLHS.m_dCor*rRHS.m_dTra-rLHS.m_dTra*rRHS.m_dCor,
        rLHS.m_dTra*rRHS.m_dSag-rLHS.m_dSag*rRHS.m_dTra,
        rLHS.m_dSag*rRHS.m_dCor-rLHS.m_dCor*rRHS.m_dSag
        );
}

//  The member function returns the distance between rLHS and rRHS.
inline double dist (const VectPat& rLHS, const VectPat& rRHS)
{
    double _tmp = (rLHS.m_dSag-rRHS.m_dSag);
    double _ret = _tmp * _tmp;
    _tmp = (rLHS.m_dCor-rRHS.m_dCor);
    _ret += _tmp * _tmp;
    _tmp = (rLHS.m_dTra- rRHS.m_dTra);
    _ret += _tmp * _tmp;
    return sqrt(_ret);
}

inline int mainOrientation(const VectPat& rVect)
{
    //  See MrServers\MrMeasSrv\SeqFW\libGSL\fGSLClassOri.cpp
    //  fGSLClassOri
    //---------------------------------------------------------------------------
    // For performance reasons calculate some tmp values
    //---------------------------------------------------------------------------
    const double dAbsSagComp     = fabs(rVect.sag());
    const double dAbsCorComp     = fabs(rVect.cor());
    const double dAbsTraComp     = fabs(rVect.tra());
    bool   bAlmEqualSagCor = fabs(dAbsSagComp-dAbsCorComp) <= 1.e-6;
    bool   bAlmEqualSagTra = fabs(dAbsSagComp-dAbsTraComp) <= 1.e-6;
    bool   bAlmEqualCorTra = fabs(dAbsCorComp-dAbsTraComp) <= 1.e-6;
    
    //---------------------------------------------------------------------------
    // Check all values to determine the slice orientation (sag, cor, tra)
    //---------------------------------------------------------------------------
    if ((bAlmEqualSagCor              &&  bAlmEqualSagTra)             ||
        (bAlmEqualSagCor              &&  (dAbsSagComp < dAbsTraComp)) ||
        (bAlmEqualSagTra              &&  (dAbsSagComp > dAbsCorComp)) ||
        (bAlmEqualCorTra              &&  (dAbsCorComp > dAbsSagComp)) ||
        ((dAbsSagComp > dAbsCorComp)  &&  (dAbsSagComp < dAbsTraComp)) ||
        ((dAbsSagComp < dAbsCorComp)  &&  (dAbsCorComp < dAbsTraComp)) ||
        ((dAbsSagComp < dAbsTraComp)  &&  (dAbsTraComp > dAbsCorComp)) ||
        ((dAbsCorComp < dAbsTraComp)  &&  (dAbsTraComp > dAbsSagComp)))
    {
        //-------------------------------------------------------------------------
        // Mainly transverse...
        //-------------------------------------------------------------------------
        return VectPat::TRANSVERSE;
    }
    else if ((bAlmEqualSagCor              &&  (dAbsSagComp > dAbsTraComp)) ||
        (bAlmEqualSagTra              &&  (dAbsSagComp < dAbsCorComp)) ||
        ((dAbsSagComp < dAbsCorComp)  &&  (dAbsCorComp > dAbsTraComp)) ||
        ((dAbsSagComp > dAbsTraComp)  &&  (dAbsSagComp < dAbsCorComp)) ||
        ((dAbsSagComp < dAbsTraComp)  &&  (dAbsTraComp < dAbsCorComp)))
    {
        //-------------------------------------------------------------------------
        // Mainly coronal...
        //-------------------------------------------------------------------------
        return VectPat::CORONAL;
    }
    else if ((bAlmEqualCorTra              &&  (dAbsCorComp < dAbsSagComp)) ||
        ((dAbsSagComp > dAbsCorComp)  &&  (dAbsSagComp > dAbsTraComp)) ||
        ((dAbsCorComp > dAbsTraComp)  &&  (dAbsCorComp < dAbsSagComp)) ||
        ((dAbsCorComp < dAbsTraComp)  &&  (dAbsTraComp < dAbsSagComp)))
    {
        //-------------------------------------------------------------------------
        // Mainly sagittal...
        //-------------------------------------------------------------------------
        return VectPat::SAGITTAL;
    }
    else
    {
        //-------------------------------------------------------------------------
        // Invalid slice orientation...
        //-------------------------------------------------------------------------
        return VectPat::UNDEFINED;
    }
}

inline int points(const VectPat& rVect)
{
    switch(mainOrientation(rVect))
    {
    case VectPat::SAGITTAL:
        return ((rVect.sag() < 0.0) ? VectPat::L_TO_R : VectPat::R_TO_L);
    case VectPat::CORONAL:
        return ((rVect.cor() < 0.0) ? VectPat::P_TO_A : VectPat::A_TO_P);
    case VectPat::TRANSVERSE:
        return ((rVect.tra() < 0.0) ? VectPat::H_TO_F : VectPat::F_TO_H);
    default:
        return VectPat::UNDEFINED;
    }
}

}  //  namespace PACE
#endif //  __VECTPAT_H

