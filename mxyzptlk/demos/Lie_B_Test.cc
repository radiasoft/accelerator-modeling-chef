/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate the harmonic oscillator
** vector field and obtain the linear
** flow map, or more properly, its
** derivatives.
**
** This particular appliation suffers
** from its being a linear vector field,
** so that the derivatives are not 
** calculated exactly.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include "mxyzptlk.h"

main() {

  Jet::BeginEnvironment( 6 );
  coord x( 0.0 );
  coord y( 0.0 );
  Jet::EndEnvironment();

  LieOperator v;
  v.SetComponent(  0,  y );
  v.SetComponent(  1, -x );
  v.expMap( 6.2831853072, x ) .printCoeffs();
  v.expMap( 6.2831853072, y ) .printCoeffs();
}
