/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      MappPropFunc.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/MappPropFunc.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using namespace std;

int MappPropFunc::operator() ( bmlnElmnt* b, Particle& p ) 
{
  // cout << "DGN> Entering MappPropFunc::operator()" << endl;
  // cout << "DGN> " << p.State() << endl;
  // cout << "DGN> " << _myMap( p.State() ) << endl;
  p.State() = _myMap( p.State() );
  // cout << "DGN> " << p.State() << endl;
  // cout << "DGN> Leaving MappPropFunc::operator()" << endl;
  return 0;
}


int MappPropFunc::operator() ( bmlnElmnt* b, JetParticle& p ) 
{
  p.State() = _myMap( p.State() );
  return 0;
}


MappPropFunc::MappPropFunc( JetParticle& p, bmlnElmnt& b )
{
  // This assumes the p has already been set 
  // up correctly for the in-state.
  b.localPropagate( p );
  _myMap = p.State();


}


MappPropFunc::MappPropFunc( const Mapping& map )
: _myMap( map )
{}


MappPropFunc::MappPropFunc( const MappPropFunc& x )
: _myMap( x._myMap )
{}

void MappPropFunc::setMapping( const Mapping& x )
{
   _myMap = x;
}


Mapping MappPropFunc::getMapping()
{
  return _myMap;
}

