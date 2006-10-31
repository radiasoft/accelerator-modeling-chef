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

    void setLinearResponse( const Matrix& );
    void setErrorCovariance( const Matrix& );

    Matrix solve( const Matrix& ) const;
    Matrix getStateCovariance()   const;

  private:
    int    _rows;
    int    _cols;
    Matrix _response;
    Matrix _solver;

    Matrix _cov;
    Matrix _covInv;
    Matrix _sig;
    Matrix _sigInv;

    Matrix _xU;
    Vector _xW;
    Matrix _xV;

    bool   _applyWeights;
    bool   _ready;

    void _finishConstruction();
    void _reconstruct( int, int );
    void _buildSolver();
};

#endif // SVDFIT_H
