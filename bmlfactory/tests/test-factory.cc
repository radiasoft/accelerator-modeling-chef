#include <beamline.h>
#include <iostream>
#include <fstream.h>

#if !defined(bmlfactory_h)
#include "bmlfactory.h"
#endif /* bmlfactory_h */

// External variable that has to be defined for the MAD parser
madparser* mp = NULL;

int
main( int argc, char** argv ) {
  
  if( argc != 5 ) {
    cout << "Usage: "
         << argv[0]
         << " <.mad file> <.bml file> <name of beamline> <momentum [GeV/c]>"
         << endl;
    return 1;
  }

  ofstream bmlStream( argv[2] );
  double momentum = atof( argv[4] );

  double brho = momentum/PH_CNV_brho_to_p;

  bmlfactory bf( argv[1], brho );
  
  // The factory can create any beamline defined in the file. If the requested
  // beamline is not found, then NULL is returned.
  beamline* bml = bf.create_beamline( argv[3] );
  
  cout << "Energy is " << bml->Energy() << endl;

  bmlnElmnt* element;
  DeepBeamlineIterator dbi( bml );
  
  while( element = dbi++ ) {
    cout << element->Type() << "; name: " << element->Name() << endl;
  }
  
  bmlStream << (*bml);

  delete bml;

  bmlStream.close();
  return 0;
}

