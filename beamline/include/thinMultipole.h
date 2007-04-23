/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinMultipole.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******                                           
**************************************************************************
*************************************************************************/

#ifndef THINMULTIPOLE_H
#define THINMULTIPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class thinMultipole;

typedef boost::shared_ptr<thinMultipole>            ThinMultipolePtr;
typedef boost::shared_ptr<thinMultipole const> ConstThinMultipolePtr;

class DLLEXPORT thinMultipole : public bmlnElmnt {

public:

  thinMultipole(); 
  thinMultipole( double const& strength);
  thinMultipole( char const* name, double const& strength);
  thinMultipole( thinMultipole const& );

  thinMultipole* Clone() const { return new thinMultipole( *this ); }

  virtual ~thinMultipole();

  void setPole(int n, std::complex<double> const&  coeff);

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

  //private:

  //std::vector< std::pair<int, std::complex<double> > terms_;

} ;

#endif // THINMULTIPOLE_H
