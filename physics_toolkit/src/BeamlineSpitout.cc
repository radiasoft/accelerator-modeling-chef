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
****** REVISION HISTORY
****** 
****** Mar 2007        ostiguy@fnal.gov
****** -use new style beamline iterators
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>

using namespace std;

void BeamlineSpitout( int numspaces, beamline::const_iterator& it )
{

    for ( ; it != it.end(); ++it ) 
    {

      for( int i = 0; i < numspaces; ++i ) {
        cout << " ";
      }
      cout << (*it)->Type() << "  " << (*it)->Name() << endl;

      if( typeid(**it) == typeid(beamline) ) 
      {
        beamline::const_iterator newit = it;
        BeamlineSpitout(numspaces+3, newit);
      }
    }
}


