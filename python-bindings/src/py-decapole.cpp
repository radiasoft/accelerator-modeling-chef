/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-decapole.cpp
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

#include <decapole.h>


void wrap_decapole () {
  

using namespace boost::python;


class_<thinDecapole, bases<bmlnElmnt> >("thinDecapole", init<double>())
  .def( init<char*, double>() )
  .def( "Type",    &thinDecapole::Type);


}
 
#if 0

class thinDecapole : public bmlnElmnt
{
public:
  thinDecapole( double /* strength */ );
  thinDecapole( char*  /* name */,
                double /* strength */ );
  thinDecapole( bmlnElmntData& );
  thinDecapole( const thinDecapole& );
  ~thinDecapole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinDecapole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinDecapole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinDecapole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinDecapole( *this ); }
} ;

#endif
