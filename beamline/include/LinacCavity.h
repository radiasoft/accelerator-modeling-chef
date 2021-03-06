/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavity.h
******
******  Copyright (c) Fermi Alliance / Fermilab    
******                All Rights Reserved                             
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
****** REVISION HISTORY 
******
****** Dec 2007            ostiguy@fnal.gov
****** - new typesafe propagator scheme
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder LinacCavity::setLength method
****** - changed default wake_on attribute in constructor to conform
******   with change made in branch StRel20080125-patches.
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
*************************************************************************
*************************************************************************/

#ifndef LINACCAVITY_H
#define LINACCAVITY_H

#include <beamline/BmlnElmnt.h>
#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/ParticleFwd.h>

#include <beamline/BmlnElmnt.h>
#include <basic_toolkit/MathConstants.h>

class BmlVisitor;
class ConstBmlVisitor;

class LinacCavity;
class thinLinacCavity;

typedef boost::shared_ptr<LinacCavity>            LinacCavityPtr;
typedef boost::shared_ptr<LinacCavity const> ConstLinacCavityPtr;

//-------------------------------------------------------------------------------

class LinacCavity: public BmlnElmnt {

  class Propagator;

public:

  LinacCavity( std::string const& name,
	       double const& length_m,
               double const& rfreq_hz,           // RF frequency [Hz]
               double const& voltage_volts,      // max energy gain per turn [eV] (strength*10**9)
               double const& syncphase_rad,      // synchronous phase [radians]
               bool   wake_on=true  );                   

  LinacCavity( LinacCavity const& );

  LinacCavity* clone() const { return new LinacCavity( *this ); }
 ~LinacCavity();

  LinacCavity& operator=(LinacCavity const& o);
  
  void   setWakeOn( bool );
  bool   wakeOn() const;

  double const&          phi()                const;
  double                 designEnergyGain()   const;

  void                   setFrequency( double const& );
  double                 frequency() const;
  double const&    radialFrequency() const;
  void                 setPhi( double const& radians);  

  void accept( BmlVisitor&      v ); 
  void accept( ConstBmlVisitor& v ) const; 

  char const* Type()     const;
  bool    isMagnet()     const;
  bool    isThin()       const;
  bool    isPassive()    const;
  bool    isDriftSpace() const;

 
  std::pair<ElmPtr,ElmPtr> split( double const& pc) const;

 protected:

  void propagateReference( Particle& particle, double initialBRho, bool scaling );

 private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  // The energy gain per pass [MV] is represented by bmlnELmnt::strength_

  double                w_rf_;          // RF frequency [Hz]
  double                phi_s_;         // synchronous phase
  bool                  wakeon_;        // wake is on/off

};

#endif // LINACCAVITY_H
