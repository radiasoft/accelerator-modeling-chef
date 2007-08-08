/*************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      SVDFit.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 234
******             Batavia, IL   60510
******                                     
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******                                                                
****** Revision History:
******   October 31, 2006
******   - Initial version.  Still a work in progress.
******   
**************************************************************************
*************************************************************************/

#ifndef SVDFIT_H
#define SVDFIT_H

#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

class SVDFit
{
  public: 
    SVDFit();
   ~SVDFit();

    void setLinearResponse( Matrix const& );
    void setErrorCovariance( Matrix const& );
    void setWeighted( bool );

    Matrix solve( Matrix const& ) const;

    inline Matrix U() const;
    Matrix D() const;
    inline Matrix V() const;
    inline Vector singularValues() const;
    Matrix stateCovariance() const;

    void setNullSpaceThreshold(double);
    inline double nullSpaceThreshold() const;

    inline bool isWeighted() const;

    static Matrix sqrt( Matrix& );

  private:
    int    rows_;
    int    cols_;

    Matrix response_;
    Matrix solver_;

    Matrix  cov_;
    Matrix  covInv_;
    Matrix  sig_;
    Matrix  sigInv_;

    Matrix  xU_;
    Vector  xW_;
    Matrix  xV_;
    Matrix  xU0_;
    Vector  xW0_;
    Matrix  xV0_;

    bool   applyWeights_;
    bool   ready_;
    
    double limW_;
    static const double threshold_;

    void  finishConstruction_();
    void  reconstruct_( int, int );
    void  buildSolver_();
};


// Inlined functions
// -----------------
inline Matrix SVDFit::U() const
{
  return xU0_;
}


inline Matrix SVDFit::V() const
{
  return xV0_;
}


inline Vector SVDFit::singularValues() const
{
  return xW0_;
}


inline double SVDFit::nullSpaceThreshold() const
{ 
  return limW_; 
}


inline bool SVDFit::isWeighted() const
{
  return applyWeights_;
}

#endif // SVDFIT_H
