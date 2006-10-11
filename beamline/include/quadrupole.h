/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      quadrupole.h
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

#ifndef QUADRUPOLE_H
#define QUADRUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class DLLEXPORT quadrupole : public bmlnElmnt
{
private:
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "quadrupole::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class TPOT_Prop : public bmlnElmnt::PropFunc
  {
  private:
    int _n;  // number of thinquad kicks
  public:
    TPOT_Prop( int /* _n */ = 4 );
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "quadrupole::TPOT_Prop"; }
    int  get_n() const;
    void set_n( int );
    void setup( quadrupole* ) const;
  };
  friend class TPOT_Prop;
  static TPOT_Prop LikeTPOT;

  quadrupole();
  quadrupole( const char*  n,   // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              PropFunc*    = &quadrupole::LikeTPOT );

  quadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              PropFunc*    = &quadrupole::LikeTPOT );


  quadrupole( bmlnElmntData& );
  quadrupole( const quadrupole& );
  ~quadrupole();

  void setStrength( double );
  void setStrength( double, int );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitQuadrupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitQuadrupole( this ); }

  void releasePropFunc();
  void setupPropFunc();
  void eliminate();

  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "quadrupole") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;

  quadrupole* Clone() const { return new quadrupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
} ;

class DLLEXPORT thinQuad : public bmlnElmnt
{
public:
  thinQuad();
  thinQuad( double );    // B'L in Tesla; + = horizontally focussing
  thinQuad( const char*, // name
            double );    // B'L in Tesla; + = horizontally focussing
  thinQuad( bmlnElmntData& );
  thinQuad( const thinQuad& );
  ~thinQuad();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );
  void eliminate();

  void accept( BmlVisitor& v ) { v.visitThinQuad( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinQuad( this ); }

  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "thinQuad") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bool isMagnet() const;
  
  thinQuad* Clone() const { return new thinQuad( *this ); }
} ;


#endif // QUADRUPOLE_H
