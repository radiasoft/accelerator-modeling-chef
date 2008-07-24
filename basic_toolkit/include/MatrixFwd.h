/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      MatrixFwd.h
******                                                                
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******  Author:    Jean-Francois Ostiguy
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#ifndef MATRIXFWD_H
#define MATRIXFWD_H

#include <complex>

template <typename T>
class TMatrix;

typedef TMatrix<double>                         Matrix;
typedef TMatrix<std::complex<double> >          MatrixC;
typedef TMatrix<int>                            MatrixI;


#endif // MATRIXFWD_H
