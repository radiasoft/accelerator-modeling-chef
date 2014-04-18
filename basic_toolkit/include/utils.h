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

class IntArray;                                  // From files basic_toolkit/include/IntArray.h
                                                 // and basic_toolkit/src/IntArray.cc

                                                 // Error functions
                                                 // Written in basic_toolkit/src/erf.cc
extern std::complex<double>  w(std::complex<double> const& z); 
extern std::complex<double>  erf(  std::complex<double> const& z );
extern std::complex<double>  erfc( std::complex<double> const& z ); 
extern std::complex<double>  erfSeries( std::complex<double> const& z ); 
 
extern bool nexcom( int, int, int* );            // Computes the next composition
extern bool nexcom( int, int, IntArray& );       //  of an integer into a number of parts.
                                                 //  Algorithm devised by Herbert Wilf.
                                                 // Written in basic_toolkit/src/nexcom.cc

extern "C" {int bcfRec( int, int ); }            // Recursive evaluation of binomial
                                                 //  coefficient.  (Worst way to do it!)
                                                 // Written in basic_toolkit/src/binomcoeff_recursive.c

extern "C" {int nearestInteger( double ); }      // Returns the integer nearest to its
                                                 //  double argument.
                                                 // Written in basic_toolkit/src/nearint.c


/************************************************************************** 

rg_ and cg_ are respectively std EISPACK eigensolvers for arbitrary real 
and complex matrices from, translated from fortran using f2c. 

*****************************************************************************/   

extern "C" { void rg_(int*, int*, double*, double*, double*, int*, double*,
                     int*, double*, int*); }
                                                 // Written in basic_toolkit/src/rg.c

extern "C" { void cg_(int*, int*, double*, double*, double*, double*, int*,
                     double*, double*, double*, double*, double*, int*); }
                                                 // Written in basic_toolkit/src/cg.c



#endif // UTILS_H 
