#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      MappPropFunc.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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


#include "MappPropFunc.h"

using namespace std;

int MappPropFunc::operator() ( bmlnElmnt* b, Particle& p ) 
{
  cout << "DGN> Entering MappPropFunc::operator()" << endl;
  cout << "DGN> " << p.State() << endl;
  cout << "DGN> " << _myMap( p.State() ) << endl;
  p.setState( _myMap( p.State() ) );
  cout << "DGN> " << p.State() << endl;
  cout << "DGN> Leaving MappPropFunc::operator()" << endl;
  return 0;
}


int MappPropFunc::operator() ( bmlnElmnt* b, JetParticle& p ) 
{
  p.setState( _myMap( p.State() ) );
  return 0;
}


MappPropFunc::MappPropFunc( JetParticle& p, bmlnElmnt& b )
{
  // This assumes the p has already been set 
  // up correctly for the in-state.
  b.localPropagate( p );
  _myMap.CopyFrom( p.State() );
}


MappPropFunc::MappPropFunc( const Mapping& map )
: _myMap( map )
{
}

MappPropFunc::MappPropFunc( const MappPropFunc& x )
: _myMap( x._myMap )
{
}

void MappPropFunc::setMapping( const Mapping& x )
{
  _myMap.CopyFrom( x );
}


Mapping MappPropFunc::getMapping()
{
  return _myMap;
}

