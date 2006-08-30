/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-sector.cpp
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

#include <beamline/sector.h>

void wrap_sector () {
  

using namespace boost::python;


 class_<sector, bases<bmlnElmnt> >("sector") 
   .def( init<char*>() )
   .def( init<char*, double>() )
   .def( init<Jet*,  double>() )
   .def( init<char*, Jet*, double>() ) 
   .def("Type",         &sector::Type);
 
      //.def("getMap",    sector::getMap)
 
}

#if 0

class sector : public bmlnElmnt
{
private:
  char mapType;        // ??? Unnecessary. Get rid of this!
  Mapping myMap;
  double betaH     [2];  // 0 = entry;  1 = exit
  double alphaH    [2];
  double deltaPsiH;
  double betaV     [2];
  double alphaV    [2];
  double deltaPsiV;
  double mapMatrix[BMLN_dynDim][BMLN_dynDim];
  double (*DeltaT) ( double );
  Jet    (*JetDeltaT) ( const Jet& );

  std::ostream& writeTo ( std::ostream& );
  std::istream& readFrom( std::istream& );

public:

  class JET_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sector::JET_Prop"; }
  };
 
  friend class JET_Prop;
  static JET_Prop defaultPropagate; 

  sector( const char* = 0, double = 0.0 );
  sector( double* betaH,  double* alphaH,  double* psiH,
          double* betaV,  double* alphaV,  double* psiV, double length );
  sector( char*, double* betaH,  double* alphaH,  double* psiH,
                 double* betaV,  double* alphaV,  double* psiV, double length );

  sector( Jet*, 
          double /* length */, 
          char = 1, /* mapType */
          PropFunc*    = &sector::defaultPropagate );

  sector( char*, 
          Jet*, 
          double, 
          char = 1,    /* mapType */ 
          PropFunc*    = &sector::defaultPropagate );

  sector( const Mapping&,  
          double /* length */, 
          char = 1, /* mapType */ 
          PropFunc*    = &sector::defaultPropagate );

  sector( char*, 
          const Mapping&,  
          double, 
          char = 1, /* mapType */ 
          PropFunc*    = &sector::defaultPropagate );

  sector( const sector& );
  ~sector();

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  Mapping getMap() const;

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitSector( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSector( this ); }

  void setFrequency( double (*)( double ) );
  void setFrequency( Jet (*)( const Jet& ) );
  void setLength( double );

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "sector") != 0 ) return bmlnElmnt::isType(c); else return true; }

  bmlnElmnt* Clone() const { return new sector( *this ); }
} ;


#endif // SECTOR_H
