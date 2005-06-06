/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-monitor.cpp
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
#include <monitor.h>

using namespace boost::python;

 struct monitorWrap: monitor {
  monitorWrap(PyObject* self):
       _self(self) {}

  monitorWrap(PyObject* self, const char* name):
       _self(self),monitor(name) {}

  monitorWrap(PyObject* self, const char* name, double length):
       _self(self),monitor(name,length) {}

    bool on();  
    bool off();
    const char* version();
   
    PyObject* _self;
     
 };

bool monitorWrap::on(){ 
return call_method<bool>(_self, "on"); }

bool monitorWrap::off(){ 
return call_method<bool>(_self, "off"); }

const char* monitorWrap::version(){ 
return call_method<const char*>(_self, "version"); }

void wrap_monitor () {


class_<monitor, bases<bmlnElmnt>, monitorWrap, boost::noncopyable >("monitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double>() )
  .def("on",               &monitorWrap::on)  
  .def("off",              &monitorWrap::off) 
  .def("version",          &monitorWrap::version)
  .def("setDriftFraction", &monitor::setDriftFraction);
 

          // operator[]( int );  // Readout of data

class_<hmonitor, bases<monitor> >("hmonitor", init<>() )
  .def( init<const char*>() )
  .def( init<const char*, double>() );


class_<vmonitor, bases<monitor> >("vmonitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double>() );

 
}

#if 0

class monitor : public bmlnElmnt
{
private:
  double   _driftFraction;

protected:
  std::ostream* _outputStreamPtr;  // not owned
  bool          _onOffSwitch;
  double*       _rgr;
  
public:
  // Constructors
  monitor();
  monitor( const char* /*name*/ );
  monitor( const char* /*name*/, double /*length*/ );
  monitor( const monitor& );
  virtual ~monitor();

  // ---------------------
  void setOutputStream( std::ostream& );
  void setOutputStream( std::ostream* );

  virtual bool on();  // returns previous state
  virtual bool off();

  virtual const char* version();

  double operator[]( int );  // Readout of data

  double setDriftFraction( double f ) 
  { double ret = _driftFraction; 
    if ( f <= 1 && f >= 0 ) _driftFraction = f; 
    return ret; 
  }
  double getDriftFraction() 
  { return _driftFraction; }

  // Is this "virtual" tag really necessary?  Probably!
  virtual std::ostream& writeTo(std::ostream&); 
  virtual std::istream& readFrom(std::istream&);

  void getState(double *s) 
  { for ( int i=0; i<6; i++ ) s[i] = _rgr[i]; } // DANGEROUS!!

  // ---------------------
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMonitor( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMonitor( this ); }
  
  inline bool State() const { return _onOffSwitch; }
  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "monitor") != 0 ) return bmlnElmnt::isType(c); 
    else return 1;
  }

  bmlnElmnt* Clone() const 
  { return new monitor( *this ); }
};



class hmonitor : public monitor
{
public:
  // Constructors
  hmonitor();
  hmonitor( const char* /*name*/ );
  hmonitor( const char*, double ); 
  hmonitor( const hmonitor& );
  virtual ~hmonitor();

  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "hmonitor") != 0 ) return monitor::isType(c); 
    else return 1; 
  }
  bmlnElmnt* Clone() const 
  { return new hmonitor( *this ); }
} ;


class vmonitor : public monitor
{
public:
  // Constructors
  vmonitor();  // Data to be written to standard output
  vmonitor( const char* /*name*/ );
  vmonitor( const char*, double ); 
  vmonitor( const vmonitor& );
  virtual ~vmonitor();

  double operator[]( int );    // Readout of data
  void localPropagate( ParticleBunch& x ) 
  { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "vmonitor") != 0 ) return monitor::isType(c); 
    else return 1; 
  }
  bmlnElmnt* Clone() const 
  { return new vmonitor( *this ); }
} ;


#endif // MONITOR_H
