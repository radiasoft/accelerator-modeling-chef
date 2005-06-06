/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-lambertson.cpp
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
#include <lambertson.h>


void wrap_lambertson () {
  

using namespace boost::python;


class_<thinLamb,bases<bmlnElmnt> >("thinLamb", init<>() )
  .def(init<char*>() )
  .def(init<char*, double, beamline*, double*>() )
  .def(init<double, beamline*, double*>() )
  .def("setSeptum",   &thinLamb::setSeptum ) 
  .def("setBeamline", &thinLamb::setBeamline ) 
  .def("setRefState", &thinLamb::setRefState )
  .def("getRefState", &thinLamb::getRefState )
  .def("eliminate",   &thinLamb::eliminate );
}


#if 0
class thinLamb : public bmlnElmnt
{
private:
  double    xSeptum;       // position of Lambertson septum in meters.
  beamline* ExtBeamline;   // pointer to the beamline of extracted particles.
  double    RefState[6];   // A particle in the "field" region
                           // with phase space state of RefState()
                           // will be on the reference orbit of the
                           // extraction line.  The specified coordinates are 
                           // SUBTRACTED from the particle coordinates.
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  
  thinLamb();
  thinLamb( char* n);      // name
  
  thinLamb( char*  n,      // name
            double x,      // x position of the septum.
            beamline* b,   // extracted beamline pointer.
            double* s );   // reference state for extraction beamline.
  
  thinLamb( double x,      // x position of the septum.
            beamline* b,   // extracted beamline pointer.
            double* s );   // reference state for extraction beamline.
  
  thinLamb( const thinLamb& );
  ~thinLamb();
  
  void setSeptum( double x); 
  void setBeamline( beamline* b); 
  void setRefState( const double* s);

  void getRefState( double* );
  
  void localPropagate( ParticleBunch& x );
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );
  
  void accept( BmlVisitor& v ) { v.visitThinLamb( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinLamb( this ); }

  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinLamb") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinLamb( *this ); }
};
 

#endif
