#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      BeamlineSpitout.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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


#include "bmlnElmnt.h"
#include "BeamlineIterator.h"

using namespace std;

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
