/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      utils.h
******                                                                
******  Copyright  Universities Research Association, Inc./Fermilab    
******             All Rights Reserved                             
******                                                                
******  Authors:    Leo Michelotti        michelotti@fnal.gov                                    
******              Jean-Francois Ostiguy ostiguy@fnal.gov                                                    
****** 
****** Copyright Universities Research Association, Inc./ Fermilab    
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
****** REVISION HISTORY
****** Mar 2007     ostiguy@fnal.gov
****** - added version of nexcom that takes an IntArray as an argument
******
**************************************************************************
*************************************************************************/

/* A collection of misc utility functions */ 

#ifndef UTILS_H
#define UTILS_H

#include <basic_toolkit/globaldefs.h>
#include <complex>

class IntArray;

extern std::complex<double>  w(std::complex<double> const& z); 
extern std::complex<double>  erf(  std::complex<double> const& z );
extern std::complex<double>  erfc( std::complex<double> const& z ); 
extern std::complex<double>  erfSeries( std::complex<double> const& z ); 
 
extern bool nexcom( int, int, int* );            // Computes the next composition
                                                 //  of an integer into a number of parts.
                                                 //  Algorithm devised by Herbert Wilf.

extern bool nexcom( int, int, IntArray& );       // Computes the next composition
                                                 //  of an integer into a number of parts.
                                                 //  Algorithm devised by Herbert Wilf.

extern "C" {int bcfRec( int, int ); }            // Recursive evaluation of binomial
                                                 //  coefficient.

extern "C" {int nearestInteger( double ); }      // Returns the integer nearest to its
                                                 //  double argument.

/************************************************************************** 

rg_ and cg_ are respectively std EISPACK eigensolvers for arbitrary real 
and complex matrices from, translated from fortran using f2c. 

*****************************************************************************/   

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
                 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*,
                      double*,double*,double*, double*, double*,int*); }



#endif // UTILS_H 
