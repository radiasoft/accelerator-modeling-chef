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

