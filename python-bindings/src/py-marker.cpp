/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-marker.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
#include <boost/python.hpp>

#include <marker.h>


void wrap_marker () {
  

using namespace boost::python;

 class_<marker, bases<bmlnElmnt> >("marker", init<>() )
   .def(init<char* >());
 
}


#if 0

class marker : public bmlnElmnt
{
public:
  marker();                   // Data to be written to standard output
  marker( char* );            // Name identifier.
  marker( const marker& );
  marker( bmlnElmntData& );
  ~marker();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMarker( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMarker( this ); }

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "marker") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new marker( *this ); }
} ;

#endif // MARKER_H
