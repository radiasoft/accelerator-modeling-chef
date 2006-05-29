/*
**
** Demo program:
** Calculates the Jet expansion of 
** arctan( sqrt( y^2 + z^2 )) at
** a point of evaluation specified 
** command line.  Ten iterations of
** a Newton's method algorithm are
** used.
**
** --- Leo Michelotti
** --- February 12, 1993
** 
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mxyzptlk.h"

using namespace std;

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "Usage: Limit_C_Test <y> <z> <degree>\n" );
  exit(0);
 }

 double ay        = atof( argv[1] );
 double az        = atof( argv[2] );
 int    degree    = atoi( argv[3] );

 Jet__environment::BeginEnvironment( degree );
 coord y( ay );
 coord z( az );
 Jet__environment::EndEnvironment();

 Jet m, u, s, c;
 m = sqrt( y*y + z*z );
 u = m;

 for( int i = 0; i < 10; i++ ){
  s = sin( u );
  c = cos( u );
  u = u - c*( s - m*c );
  printf( "%le\n", u.standardPart() );
  u.peekAt();
  printf( "\n" );
 }

}

/*
The output was tested against Mathematica. 
Here is the result:

churchy 2: math

Mathematica 2.0 for SPARC
Copyright 1988-91 Wolfram Research, Inc.

In[1]:= Series[ArcTan[Sqrt[(1+y)*(1+y) + z*z]], {y,0,5}, {z,0,5}]

              2    4                 2      4
        Pi   z    z        6    1   z    7 z        6
Out[1]= -- + -- - -- + O[z]  + (- - -- + ---- + O[z] ) y + 
        4    4    8             2   2     16
 
                2       4                        2       4
        1    5 z    29 z        6   2    1    5 z    47 z        6   3
>    (-(-) + ---- - ----- + O[z] ) y  + (-- - ---- + ----- + O[z] ) y  + 
        4     8      32                  12    8      32
 
         2       4                         2        4
      9 z    67 z        6   4      1     z    179 z        6   5       6
>    (---- - ----- + O[z] ) y  + (-(--) - -- + ------ + O[z] ) y  + O[y]
       16     32                    40    2      64

In[2]:= ExpandAll[%]

              2    4                 2      4
        Pi   z    z        6    1   z    7 z        6
Out[2]= -- + -- - -- + O[z]  + (- - -- + ---- + O[z] ) y + 
        4    4    8             2   2     16
 
                2       4                        2       4
        1    5 z    29 z        6   2    1    5 z    47 z        6   3
>    (-(-) + ---- - ----- + O[z] ) y  + (-- - ---- + ----- + O[z] ) y  + 
        4     8      32                  12    8      32
 
         2       4                         2        4
      9 z    67 z        6   4      1     z    179 z        6   5       6
>    (---- - ----- + O[z] ) y  + (-(--) - -- + ------ + O[z] ) y  + O[y]
       16     32                    40    2      64

In[3]:= Quit[];

*/
