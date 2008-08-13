/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPart.h
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
****** Dec 2007  ostiguy@fnal.gov
****** - new typesafe propagator scheme
****** May 2008  ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef LINACCAVITYPARTS_H
#define LINACCAVITYPARTS_H

#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/MathConstants.h>

class BmlVisitor;
class ConstBmlVisitor;
class RefRegVisitor;

class LCavityUpstream;
class LCavityDnstream;

typedef boost::shared_ptr<LCavityUpstream>                 LCavityUpstreamPtr;
typedef boost::shared_ptr<LCavityUpstream const>      ConstLCavityUpstreamPtr;

typedef boost::shared_ptr<LCavityDnstream>                LCavityDnstreamPtr;
typedef boost::shared_ptr<LCavityDnstream  const>    ConstLCavityDnstreamPtr;


//-------------------------------------------------------------------------------

class LCavityUpstream: public bmlnElmnt {

  class Propagator;

public:

 LCavityUpstream( std::string const& name,
	       double const& length_m,
               double const& rfreq_hz,         // RF frequency [Hz]
               double const& voltage_volts,    // max energy gain per turn [eV] (strength*10**9)
               double const& syncphase_rad);   // synchronous phase [radians]
                 

  LCavityUpstream( LCavityUpstream const& );

  LCavityUpstream* Clone() const;
 ~LCavityUpstream();

  LCavityUpstream& operator=(  LCavityUpstream const& rhs);

  double const&          getPhi()                const;
  double                 getDesignEnergyGain()   const;

  void                   setFrequency( double const& );
  double                 getFrequency() const;
  double const&    getRadialFrequency() const;
  void                 setPhi( double const& radians);  

  void accept( BmlVisitor&      v ); 
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type()  const;
  bool    isMagnet()  const;
  bool    isThin()    const;
  bool    isPassive() const;

 private:

  double                w_rf_;          // RF frequency [Hz]
  double                phi_s_;         // synchronous phase
};

//-------------------------------------------------------------------------------

class LCavityDnstream: public bmlnElmnt {

  class Propagator;

public:

  LCavityDnstream( std::string const& name,
	       double const& length_m,
               double const& rfreq_hz,         // RF frequency [Hz]
               double const& voltage_volts,    // max energy gain per turn [eV] (strength*10**9)
	       double const& syncphase_rad);   // synchronous phase [radians]

  LCavityDnstream( LCavityDnstream const& );

  LCavityDnstream* Clone() const;
 ~LCavityDnstream();

  LCavityDnstream& operator=(  LCavityDnstream const& rhs);

  double const&          getPhi()                const;
  double                 getDesignEnergyGain()   const;

  void                   setFrequency( double const& );
  double                 getFrequency() const;
  double const&    getRadialFrequency() const;
  void                 setPhi( double const& radians);  

  
  void accept( BmlVisitor&      v ); 
  void accept( ConstBmlVisitor& v ) const; 

  const char* Type()  const;
  bool    isMagnet()  const;
  bool    isThin()    const;
  bool    isPassive() const;

 private:

  double                w_rf_;          // RF frequency [Hz]
  double                phi_s_;         // synchronous phase

};

#endif // LINACCAVITYPARTS_H
