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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <bmlnElmnt.h>
#include <BeamlineIterator.h>

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
