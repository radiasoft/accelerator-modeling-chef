/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-circuit.cpp
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
#include <iostream>

#include <beamline/kick.h>


void wrap_kick () {
  

using namespace boost::python;


class_<hkick>("hkick")
  .def(init<double>())
  .def(init<const char*>())
  .def(init<const char*,double>())
  .def(init<const char*,double, double>())
  .def( "Type", &vkick::Type);


class_<vkick>("vkick")
  .def(init<double>())
  .def(init<const char*>())
  .def(init<const char*,double>())
  .def(init<const char*,double, double>())
  .def( "Type", &vkick::Type);


class_<kick>("kick")
  .def(init<double,double>())
  .def(init<const char*>())
  .def(init<const char*,double, double, double>())
  .def(init<const char*,double, double, double>())
  .def( "Type", &vkick::Type);
}


#if 0

public:
  vkick();            // Assumes zero kick
  vkick( double );    // kick size in radians
  vkick( char* );     // name; assumes zero kick
  vkick( char*,       // name
         double  );   // kick size in radians
  vkick( char*,       // name
         double,      // length
         double  );   // kick size in radians
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVkick( this ); }

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new vkick( *this ); }
};

#endif
