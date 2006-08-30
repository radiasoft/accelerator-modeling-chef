/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      gcd.c
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


/*
    Function returns the greatest common divisor of x and y.
*/
int gcd( x, y )
int x;
int y;
{
int larger, smaller;
int remainder;

if( x < 0 ) x = -x;
if( y < 0 ) y = -y;

if( x == 0 ) {
  if( y == 0 ) return 1;
  else return y;
  }

if( y == 0 ) return x;

if ( x < y ) {
  larger = y;
  smaller = x;
  }
else {
  larger = x;
  smaller = y;
  }

remainder = larger%smaller;

while( remainder != 0 ) {
  larger = smaller;
  smaller = remainder;
  remainder = larger%smaller;
  }

return smaller;

}

