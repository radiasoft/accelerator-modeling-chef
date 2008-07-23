/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      VectorFwd.h
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
******                                                                
**************************************************************************
*************************************************************************/

#ifndef VECTORFWD_H
#define VECTORFWD_H

template <typename T>
class TVector;

typedef class TVector<double>                  Vector;
typedef class TVector<std::complex<double> >   VectorC;
typedef class TVector<int>                     VectorI;


#endif // VECTORD_H 
