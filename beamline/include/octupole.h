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

#include <bmlnElmnt.h>

class octupole : public bmlnElmnt
{
public:
  octupole( double,       /* length   */
            double        /* strength */ );
  octupole( const char*,  /* name     */
            double,       /* length   */
            double        /* strength */ );
  octupole( const octupole& );
  octupole( bmlnElmntData& );
  ~octupole();

  void accept( BmlVisitor& v ) { v.visitOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitOctupole( this ); }

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "octupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bool isMagnet() const;

  bmlnElmnt* Clone() const { return new octupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
};



class thinOctupole : public bmlnElmnt
{
public:
  thinOctupole( double       /* strength */ );
  thinOctupole( const char*  /* name */,
                double       /* strength */ );
  thinOctupole( bmlnElmntData& );
  thinOctupole( const thinOctupole& );
  ~thinOctupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinOctupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinOctupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bool isMagnet() const;
  
  bmlnElmnt* Clone() const { return new thinOctupole( *this ); }
} ;

#endif // OCTUPOLE_H
