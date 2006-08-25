/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      MLPtr.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Author:    Jean-Francois Ostiguy
******             Email: ostiguy@fnal.gov                         
******
******                                                                
**************************************************************************
*************************************************************************/
#include <MLPtr.h>
#include <TML.h>

template<>
template<>
MLPtr<double>::operator MLPtr<std::complex<double> >() const
{
  return  MLPtr<std::complex<double> >( new TML<std::complex<double> >(**this) );
}



