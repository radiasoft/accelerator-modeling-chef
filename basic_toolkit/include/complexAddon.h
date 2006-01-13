/**************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      complexAddon.h
******  Version:   4.1
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*******
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
******             Batavia, IL   60510                                
******             michelotti@fnal.gov
******
******                                                                
****** Revision History:
******
******  Dec 2005  
******
******  Jean-Francois Ostiguy, ostiguy@fnal.gov
****** 
******  - eliminated support for obsolete compilers. 
******    Now assuming ANSI std C++.  
******                                                                
**************************************************************************
*************************************************************************/


#ifndef COMPLEXADDON_H
#define COMPLEXADDON_H

#include <complex>


const std::complex<double> complex_1( 1.0, 0.0 );
const std::complex<double> complex_0( 0.0, 0.0 );
const std::complex<double> complex_i( 0.0, 1.0 );


// ----------------------------------------------------------------------
// The following operators are used by the mxyzptlk library
// They define a partial ordering that is used to sort 
// DA variables. DO NOT modify this unless you understand
// what you are doing !
//-----------------------------------------------------------------------

inline bool operator>= ( const std::complex<double>& x, const std::complex<double>& y )
{
 return std::real(x) >= std::real(y)  &&  std::imag(x) >= std::imag(y);
}

inline bool operator< ( const std::complex<double>& x, const std::complex<double>& y )
{
 return std::real(x) < std::real(y)  &&  std::imag(x) < std::imag(y);
}

inline bool operator> ( const std::complex<double>& x, const std::complex<double>& y )
{
 return std::real(x) > std::real(y)  &&  std::imag(x) > std::imag(y);
}


#endif // COMPLEXADDON_H
