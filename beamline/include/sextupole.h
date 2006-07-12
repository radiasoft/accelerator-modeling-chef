/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      sextupole.h
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

#ifndef SEXTUPOLE_H
#define SEXTUPOLE_H

#include <bmlnElmnt.h>
#include <Jet.h>

class sextupole : public bmlnElmnt
{
public:
  sextupole( double,       /* length   */
             double        /* strength */ );
  sextupole( const char*,  /* name     */
             double,       /* length   */
             double        /* strength */ );
  sextupole( bmlnElmntData& );
  sextupole( const sextupole& );
  ~sextupole();

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSextupole( this ); }

  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "sextupole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new sextupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
} ;



class thinSextupole : public bmlnElmnt
{
public:
  thinSextupole( double        /* strength */ );
  thinSextupole( const char*   /* name */,
                 double        /* strength */ );
  thinSextupole( bmlnElmntData& );
  thinSextupole( const thinSextupole& );
  ~thinSextupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinSextupole( this ); }

  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "thinSextupole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new thinSextupole( *this ); }
} ;

#endif // SEXTUPOLE_H
