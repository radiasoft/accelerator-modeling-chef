/*************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      SVDFitPL.h
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

#ifndef SVDFITPLPL_H
#define SVDFITPLPL_H

#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

//! \brief The Singular Value Decompostion Solver used in Steering 
/*! Cloned from CHEF... Yes, this is bad. 

   Upgrade: Setting the lower limit on the Null Space
    Note: Current CHEF version probably has this capability.
        To re-align, at some point.  
*/ 
class SVDFitPL
{
  public: 
    SVDFitPL();
    ~SVDFitPL();

    void setLinearResponse( const Matrix& );
    void setErrorCovariance( const Matrix& );
    void setWeighted( bool );

    Matrix solve( const Matrix& ) const;

    Matrix getStateCovariance()   const;
    bool   getWeighted() const;
    Vector getSingularValue() const;
    Matrix getVMatrix() const;
    Matrix getUMatrix() const;
    inline double getLimitNullSpace() const  {return _limW;}
    inline void setLimitNullSpace(double l) {_limW = l;}
    inline void setSolverMode(int n) {_solveMode = n;}

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
    Matrix _xVOrig;

    bool   _applyWeights;
    bool   _ready;
    
    double _limW;
    int _solveMode; // Picking the solution only from a specific mode 

    void _finishConstruction();
    void _reconstruct( int, int );
    void _buildSolver();
    Matrix sqrt( Matrix& x );
};

#endif // SVDFITPL_H