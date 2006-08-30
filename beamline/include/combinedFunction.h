/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      combinedFunction.h
******  Version:   2.1
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
**************************************************************************
*************************************************************************/


#ifndef COMBINED_FUNCTION_H
#define COMBINED_FUNCTION_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


enum WHICH_MULTIPOLE { DIPOLE_FIELD, QUADRUPOLE_FIELD, SEXTUPOLE_FIELD, OCTUPOLE_FIELD, DECAPOLE_FIELD, TWELVEPOLE_FIELD, FOURTEENPOLE_FIELD, SIXTEENPOLE_FIELD, EIGHTEENPOLE_FIELD };

class Particle;
class JetParticle;

class DLLEXPORT combinedFunction : public bmlnElmnt 
{
private:
  std::istream& readFrom(std::istream&);
  std::ostream& writeTo(std::ostream&);

public:
  combinedFunction();
  combinedFunction( const char* /* name */ );
  combinedFunction( const char*, const beamline& );
  combinedFunction( const beamline& );
  combinedFunction( const combinedFunction& );
  virtual ~combinedFunction();

  void append(bmlnElmnt&);
  void eliminate() {delete this;}

  virtual void enterLocalFrame( Particle&    p ) const
    { bmlnElmnt::enterLocalFrame( p ); }
  virtual void enterLocalFrame( JetParticle& p ) const
    { bmlnElmnt::enterLocalFrame( p ); }
  virtual void leaveLocalFrame( Particle&    p ) const
    { bmlnElmnt::leaveLocalFrame( p ); }
  virtual void leaveLocalFrame( JetParticle& p ) const
    { bmlnElmnt::leaveLocalFrame( p ); }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitCombinedFunction( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCombinedFunction( this ); }
  
  void setField( bmlnElmnt::CRITFUNC, double );
  void setField( WHICH_MULTIPOLE, double field );

  double Field( WHICH_MULTIPOLE );
  double getField( WHICH_MULTIPOLE x ) 
    { return this->Field(x); }

  void setSkew( WHICH_MULTIPOLE, alignmentData& );
  void setLength( double x )   // ??? Should this be here???
    { length = x; }
  alignmentData Skew( WHICH_MULTIPOLE );
  alignmentData getAlignment( WHICH_MULTIPOLE x )
    { return this->Skew(x); }

  const char* Type() const;
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new combinedFunction( *this ); }

  double AdjustPosition( const Particle& );
  double AdjustPosition( const JetParticle& );
};

#endif // COMBINED_FUNCTION_H
