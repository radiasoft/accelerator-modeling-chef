/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      octupole.h
******  Version:   2.1
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
#ifndef OCTUPOLE_H
#define OCTUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>

#include <beamline/Particle.h>

template<typename T> 
class TVector;

class DLLEXPORT octupole : public bmlnElmnt
{
public:
  octupole();
  octupole( double const& length,    double const& strength);
  octupole( char const* name, double const& length,    double const& strength);
  octupole( octupole const& );
  octupole* Clone() const { return new octupole( *this ); }

  virtual ~octupole();

  void accept( BmlVisitor& v ) { v.visitOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitOctupole( this ); }

  void setStrength( double const& );
  void setStrength( double const&, int );

  void setCurrent( double const& );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  bool isMagnet() const;

  void Split( double const&, bmlnElmnt**, bmlnElmnt** ) const;

};



class DLLEXPORT thinOctupole : public bmlnElmnt 
{
public:
  thinOctupole();
  thinOctupole( double const&  strength);
  thinOctupole( char const* name,  double const& strength );
  thinOctupole( thinOctupole const& );

  thinOctupole* Clone() const { return new thinOctupole( *this ); }

  virtual ~thinOctupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v )            { v.visitThinOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinOctupole( this ); }

  const char* Type() const;
  bool isMagnet() const;
  
} ;

#endif // OCTUPOLE_H
