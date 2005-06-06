/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-thinpoles.cpp
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
#include <thinpoles.h>

using namespace boost::python;

void wrap_thinpoles () {


class_<thin12pole, bases<bmlnElmnt> >("thin12pole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",      &thin12pole::Type);


class_<thin14pole, bases<bmlnElmnt> >("thin14pole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",      &thin14pole::Type);


class_<thin16pole, bases<bmlnElmnt> >("thin16pole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",      &thin16pole::Type);


class_<thin18pole, bases<bmlnElmnt> >("thin18pole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",      &thin18pole::Type);


class_<thinMultipole, bases<bmlnElmnt> >("thinMultipole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",      &thinMultipole::Type);
}


#if 0


class thin12pole : public bmlnElmnt
{
public:
  thin12pole( double /* strength */ );
  thin12pole( char*  /* name */,
              double /* strength */ );
  thin12pole( bmlnElmntData& );
  thin12pole( const thin12pole& );
  ~thin12pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin12pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin12pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin12pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thin12pole( *this ); }
} ;

class thin14pole : public bmlnElmnt
{
public:
  thin14pole( double /* strength */ );
  thin14pole( char*  /* name */,
              double /* strength */ );
  thin14pole( bmlnElmntData& );
  thin14pole( const thin14pole& );
  ~thin14pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin14pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin14pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin14pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin14pole( *this ); }
} ;

class thin16pole : public bmlnElmnt
{
public:
  thin16pole( double /* strength */ );
  thin16pole( char*  /* name */,
              double /* strength */ );
  thin16pole( bmlnElmntData& );
  thin16pole( const thin16pole& );
  ~thin16pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin16pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin16pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin16pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin16pole( *this ); }
} ;

class thin18pole : public bmlnElmnt
{
public:
  thin18pole( double /* strength */ );
  thin18pole( char*  /* name */,
              double /* strength */ );
  thin18pole( bmlnElmntData& );
  thin18pole( const thin18pole& );
  ~thin18pole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThin18pole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThin18pole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thin18pole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thin18pole( *this ); }
} ;

class thinMultipole : public bmlnElmnt
{
public:
  thinMultipole( double /* strength */ );
  thinMultipole( char*  /* name */,
                double /* strength */ );
  thinMultipole( bmlnElmntData& );
  thinMultipole( const thinMultipole& );
  ~thinMultipole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinMultipole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinMultipole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinMultipole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thinMultipole( *this ); }
} ;

#endif
