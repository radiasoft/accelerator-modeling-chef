/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      Jetfwd.h
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
******  Author: ostiguy@fnal.gov
******                                               
**************************************************************************
*************************************************************************/

#ifndef JETFWD_H
#define JETFWD_H

#include <complex>

template <typename T>
class TJet;

template <typename T>
class Tcoord;

template <typename T>
class Tparam;

typedef TJet<double>                   Jet;
typedef TJet<std::complex<double> >    JetC;
typedef Tcoord<double>                 coord;
typedef Tcoord<std::complex<double> >  coordC;
typedef Tparam<double>                 param;
typedef Tparam<std::complex<double> >  paramC;

#endif // JETFWD_H
