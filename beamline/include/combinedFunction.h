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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef COMBINED_FUNCTION_H
#define COMBINED_FUNCTION_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

enum WHICH_MULTIPOLE { DIPOLE_FIELD, QUADRUPOLE_FIELD, SEXTUPOLE_FIELD, OCTUPOLE_FIELD, DECAPOLE_FIELD, TWELVEPOLE_FIELD, FOURTEENPOLE_FIELD, SIXTEENPOLE_FIELD, EIGHTEENPOLE_FIELD };

class Particle;
class JetParticle;

class combinedFunction : public bmlnElmnt 
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
