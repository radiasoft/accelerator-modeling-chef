/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-rfcavity.cpp
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

#include <rfcavity.h>

void wrap_rfcavity () {
  

using namespace boost::python;


class_<rfcavity, bases<bmlnElmnt> >("rfcavity")
  .def( init<const char*>() )
  .def( init<double,double,double,double,double,double>() )    
  .def( init<char*, double,double,double,double,double,double>() )    
  .def("getPhi",       &rfcavity::getPhi)
  .def("getFrequency", &rfcavity::getFrequency )
  .def("getQ",         &rfcavity::getQ)
  .def("getR",         &rfcavity::getR)
  .def("Type",         &rfcavity::Type);


class_<thinrfcavity, bases<bmlnElmnt> >("thinrfcavity", init<char *>() )
  .def(init< double,   double, double, double, double> () )    
  .def(init< char*,  double,   double, double, double, double> () )    
  .def("getPhi",       &thinrfcavity::getPhi)
  .def("getFrequency", &thinrfcavity::getFrequency )
  .def("getQ",         &thinrfcavity::getQ)
  .def("getR",         &thinrfcavity::getR)
  .def("Type",         &thinrfcavity::Type);

}


#if 0

class rfcavity : public bmlnElmnt
{
private:
  double w_rf;                  // RF frequency [MHz]  // ??? What??
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  rfcavity( const char* = "NONAME" ); // Name
  rfcavity( double,   // length [m]
            double,   // RF frequency [MHz]
            double,   // max energy gain per turn [eV] (strength)
            double,   // synchronous phase [radians]
            double,   // Q
            double    // R shunt impedance
          );
  rfcavity( char*,    // Name
            double,   // length [m]
            double,   // RF frequency [MHz]
            double,   // max energy gain per turn [eV] (strength)
            double,   // synchronous phase [radians]
            double,   // Q
            double    // R shunt impedance
          );
  rfcavity( const rfcavity& );
  ~rfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitRfcavity( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitRfcavity( this ); }

  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rfcavity") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rfcavity( *this ); }
  inline double getPhi() const { return phi_s; }
  inline double getFrequency() const { return w_rf; }
  inline double getQ() const { return Q; }
  inline double getR() const { return R; }
};


//
// *** class thinRFCavity written by Jian Ping Shan
// *** April 4, 1994
//

class thinrfcavity : public bmlnElmnt
{
private:
  double w_rf;                  // RF frequency [MHz]  // ??? What??
  double phi_s;                 // synchronous phase
  double sin_phi_s;             // sine of synchronous phase
  // The max energy gain per turn [GeV] is represented by bmlnELmnt::strength
  double Q;                     // quality factor
  double R;                     // shunt impedance
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  thinrfcavity( char * ); // Name
  thinrfcavity( double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV] (strength)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( char *,   // Name
                double,   // RF frequency [MHz]
                double,   // max energy gain per turn [eV] (strength)
                double,   // synchronous phase [radians]
                double,   // Q
                double    // R shunt impedance
                );
  thinrfcavity( const thinrfcavity& );
  ~thinrfcavity();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinrfcavity( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinrfcavity( this ); }

  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinrfcavity") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new thinrfcavity( *this ); }
  inline double getPhi() const { return phi_s; }
  inline double getFrequency() const { return w_rf; }
  inline double getQ() const { return Q; }
  inline double getR() const { return R; }
};


#endif
