/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      combinedFunction.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
****** REVISION HISTORY
******
****** Mar 2007:          ostiguy@fnal.gov
****** - use covariant return types
****** - added support for reference counted elements
******
****** Dec 2007:          ostiguy@fnal.gov
****** - new typesafe propagators
******
****** Apr 2008           michelotti@fnal.gov
****** - modified signature of setLength method to conform
******   to the virtual method in base class bmlnElmnt.
****** - added placeholder setStrength method.
****** 
**************************************************************************
*************************************************************************/

#ifndef COMBINED_FUNCTION_H
#define COMBINED_FUNCTION_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/Alignment.h>



enum WHICH_MULTIPOLE { DIPOLE_FIELD, QUADRUPOLE_FIELD, SEXTUPOLE_FIELD, 
                       OCTUPOLE_FIELD, DECAPOLE_FIELD, TWELVEPOLE_FIELD, 
                       FOURTEENPOLE_FIELD, SIXTEENPOLE_FIELD, EIGHTEENPOLE_FIELD };

class Particle;
class JetParticle;
class BmlVisitor;
class ConstBmlVisitor;


class DLLEXPORT combinedFunction : public bmlnElmnt {
   
  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<combinedFunction> > PropagatorPtr;   

  combinedFunction();
  combinedFunction( char const* name);
  combinedFunction( char const* name, beamline const& );
  combinedFunction( beamline const& );

  combinedFunction( combinedFunction const& );
  
 ~combinedFunction();

  combinedFunction* Clone() const { return new combinedFunction( *this ); }

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

  void append(bmlnElmnt&);

  double AdjustPosition( Particle    const& );
  double AdjustPosition( JetParticle const& );

  void enterLocalFrame( Particle&    p ) const   { bmlnElmnt::enterLocalFrame( p ); }
  void enterLocalFrame( JetParticle& p ) const   { bmlnElmnt::enterLocalFrame( p ); }
  void leaveLocalFrame( Particle&    p ) const   { bmlnElmnt::leaveLocalFrame( p ); }
  void leaveLocalFrame( JetParticle& p ) const   { bmlnElmnt::leaveLocalFrame( p ); }

  void localPropagate(         Particle& p );
  void localPropagate(      JetParticle& p );
  void localPropagate(    ParticleBunch& b );
  void localPropagate( JetParticleBunch& b );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;
  
  void setField( boost::function<bool(bmlnElmnt const&)>, double );

  void setField( WHICH_MULTIPOLE, double field );

  double Field( WHICH_MULTIPOLE );
  double getField( WHICH_MULTIPOLE x ) 
    { return this->Field(x); }

  void setStrength( double const& );
  void setLength( double const& );

  void setSkew( WHICH_MULTIPOLE, alignmentData& );

  alignmentData Skew( WHICH_MULTIPOLE );
  alignmentData getAlignment( WHICH_MULTIPOLE x )    { return Skew(x); }

  const char* Type() const;
  bool isMagnet() const;

private:

  bool hasMultipole( ElmPtr elm, WHICH_MULTIPOLE mult );  
  std::istream& readFrom(std::istream&);
  std::ostream& writeTo(std::ostream&);

  PropagatorPtr propagator_;

};


#endif // COMBINED_FUNCTION_H
