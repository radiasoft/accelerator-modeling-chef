/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      RandomOrthogonal.h
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
******                                                                
******  Author:    Leo Michelotti                                     
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
****** 
******  Revision History 
******
******  Aug 2006   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov
******
******             RandomOrthogonal class header and implementation now in
******             files distinct from TMatrix header and implementation.
******                                                                
**************************************************************************
*************************************************************************/

#ifndef RANDOMORTHOGONAL_H
#define RANDOMORTHOGONAL_H


// --------------------------------------------------------------------------------------------
// Special class RandomOrthogonal
// --------------------------------------------------------------------------------------------

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Matrix.h>

class DLLEXPORT RandomOrthogonal
{
  public: 
    RandomOrthogonal( int /* dimension */ );
    ~RandomOrthogonal();

    void omitIndex( int );  
    void omitIndex( int, int );  
    void includeIndex( int );
    void includeIndex( int, int );
    // By default, all are included
    void setNumberOfPasses( int );
    // By default, = 1.      

    void setRange( int, double /* radians */, double );
    void setRange( int, int, double /* radians */, double );
    // Default range = [ 0, 2 pi ) for all indices

    MatrixD build();
    
  private:
    int      _dim;
    int      _passes;
    bool**   _omitted;        // owned
    double** _lowerTheta;     // owned
    double** _upperTheta;     // owned
    double** _rangeTheta;     // owned
};


#endif // RANDOMORTHOGONAL_H
