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
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef LINACCAVITY_H
#define LINACCAVITY_H

#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/MathConstants.h>

class BmlVisitor;
class ConstBmlVisitor;
class RefRegVisitor;

class LinacCavity;
class thinLinacCavity;

typedef boost::shared_ptr<LinacCavity>            LinacCavityPtr;
typedef boost::shared_ptr<LinacCavity const> ConstLinacCavityPtr;


//-------------------------------------------------------------------------------

class LinacCavity: public bmlnElmnt {

  friend class elm_core_access;

public:

  LinacCavity( const  char*  name,
	       double const& length_m,
               double const& rfreq_hz,         // RF frequency [Hz]
               double const& voltage_volts,    // max energy gain per turn [eV] (strength*10**9)
               double const& syncphase_rad);   // synchronous phase [radians]

  LinacCavity( LinacCavity const& );

  LinacCavity* Clone() const { return new LinacCavity( *this ); }
 ~LinacCavity();

  double const&  getPhi()                const;
  double         getDesignEnergyGain()   const;

  void                   setFrequency( double const& );
  double                 getFrequency() const;
  double const&    getRadialFrequency() const;
  void                 setPhi( double const& radians);  
  void            setStrength( double const& eV);  


  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  
  void accept( BmlVisitor&      v ); 
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type() const;
  bool    isMagnet() const;

private:

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

  // The energy gain per pass [MV] is represented by bmlnELmnt::strength_

  double                w_rf_;          // RF frequency [Hz]
  double                phi_s_;         // synchronous phase


};


#endif // LINACCAVITY_H
