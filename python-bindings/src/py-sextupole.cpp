/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-sextupole.cpp
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
#include <sextupole.h>

void wrap_sextupole () {
  

using namespace boost::python;


class_<sextupole, bases<bmlnElmnt> >("sextupole", init<double,double>() )
  .def(init<char*, double, double>() )
  .def("setCurrent",         &sextupole::setCurrent)
  .def("Type",               &sextupole::Type);

class_<thinSextupole, bases<bmlnElmnt> >("thinSextupole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",  &thinSextupole::Type);
 
}

#if 0

class sextupole : public bmlnElmnt
{
public:
  sextupole( double, /* length   */
             double  /* strength */ );
  sextupole( char*,  /* name     */
             double, /* length   */
             double  /* strength */ );
  sextupole( bmlnElmntData& );
  sextupole( const sextupole& );
  ~sextupole();

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSextupole( this ); }

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "sextupole") != 0 ) return bmlnElmnt::isType(c); else return true; }

  bmlnElmnt* Clone() const { return new sextupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );
} ;



class thinSextupole : public bmlnElmnt
{
public:
  thinSextupole( double /* strength */ );
  thinSextupole( char*  /* name */,
                 double /* strength */ );
  thinSextupole( bmlnElmntData& );
  thinSextupole( const thinSextupole& );
  ~thinSextupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinSextupole( this ); }

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "thinSextupole") != 0 ) return bmlnElmnt::isType(c); else return true; }

  bmlnElmnt* Clone() const { return new thinSextupole( *this ); }
} ;

#endif
