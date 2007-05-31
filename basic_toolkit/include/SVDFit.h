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

    Matrix getStateCovariance()  const;
    bool   getWeighted()         const;
    Vector getSingularValue()    const;
    Matrix getVMatrix()          const;
    Matrix getUMatrix()          const;

    inline double getLimitNullSpace()        const  { return limW_; }
    inline void   setLimitNullSpace(double l)       { limW_   = l;  }

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

    bool   applyWeights_;
    bool   ready_;
    
    double limW_;

    void  finishConstruction();
    void  reconstruct( int, int );
    void  buildSolver();

    Matrix sqrt( Matrix& x );
};

#endif // SVDFIT_H
