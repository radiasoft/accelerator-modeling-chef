#include <beamline.h>
#include <iostream>

#if !defined(bmlfactory_h)
#include "bmlfactory.h"
#endif /* bmlfactory_h */

// External variable that has to be defined for the MAD parser
madparser* mp = NULL;

int
main() {
  
  // Initialize a beamline factory, using the MAD file "lattice.mad" and BRHO=123
  bmlfactory* bf = new bmlfactory( "lattice.mad", 123 );
  
  // The factory can create any beamline defined in the file. If the requested
  // beamline is not found, then NULL is returned.
  beamline* bml = bf->create_beamline( "TEVB0R2" );
  
  cout << "Energy is " << bml->Energy() << endl;

  bmlnElmnt* element;
  DeepBeamlineIterator dbi( bml );
  
  while( element = dbi++ ) {
    cout << element->Type() << "; name: " << element->Name() << endl;
  }
  
  delete bml;
  delete bf;
  return 0;
}

