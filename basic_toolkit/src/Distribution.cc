/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Distribution.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


/*
 * Distribtion class written by Jim Holt.
 * August 11, 1994
 *
 */

#include <math.h>
#include <stdlib.h>
#include "Distribution.h"

extern "C" double drand48( void );

Gaussian::Gaussian(long seed = 0) : Distribution(seed) {
}
Gaussian::~Gaussian() {
}
double Gaussian::getValue() {
  double r;
  double random1,random2;
  do {
    random1 = 2.0*drand48() - 1.0;
    random2 = 2.0*drand48() - 1.0;
    r = random1*random1 + random2*random2;
  } while (r >= 1.0);
  double factor = sqrt(-2.0*log(r)/r);
  return random2 * factor;
}
