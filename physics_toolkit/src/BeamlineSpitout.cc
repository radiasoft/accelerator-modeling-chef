#include "bmlnElmnt.h"
#include "BeamlineIterator.h"

void BeamlineSpitout( int numspaces, BeamlineIterator& bi )
{
  static bmlnElmnt* q;
  static int i;

  while( true ) {
    q = bi++;
    if( 0 == q ) {return;}

    for( i = 0; i < numspaces; i++ ) {
      cout << " ";
    }
    cout << q->Type() << "  " << q->Name() << endl;

    if( 0 == strcmp("beamline",q->Type()) ) {
      BeamlineIterator newbi( (beamline*) q );
      BeamlineSpitout(numspaces+3,newbi);
    }
  }
}


void BeamlineSpitout( const beamline& bml )
{
  BeamlineIterator bi( bml );
  BeamlineSpitout( 0, bi );
}
