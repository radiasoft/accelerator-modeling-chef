/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      JetQuadrupole.h
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


#ifndef JETQUADRUPOLE_H
#define JETQUADRUPOLE_H

#include "bmlnElmnt.h"
#include "Jet"

class JetQuadrupole : public bmlnElmnt
{
 private:
  Jet JetStrength;
  int strengthIndex;
  std::istream& readFrom(std::istream&);
  std::ostream& writeTo(std::ostream&);

public:

  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "JetQuadrupole::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class TPOT_Prop : public bmlnElmnt::PropFunc
  {
  public:
  private:
    int _n;  // number of thinquad kicks
  public:
    TPOT_Prop( int /* _n */ = 4 );
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "JetQuadrupole::TPOT_Prop"; }
    int  get_n() const;
    void set_n( int );
    void setup( JetQuadrupole* ) const;
  };
  friend class TPOT_Prop;
  static TPOT_Prop LikeTPOT;

  JetQuadrupole();
  JetQuadrupole( char*  n,         // name
              double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PropFunc* = 0 );
  JetQuadrupole( double l,         // length,        in meters
              double s,         // strength = B', in Tesla-meters^-1
              int index,        // What jet index > 6
              int    m = 4,     // number of thinquad kicks
              PropFunc* = 0 );
  JetQuadrupole( bmlnElmntData& );
  JetQuadrupole( const JetQuadrupole& );
  ~JetQuadrupole();

  void setStrength( double );
  void setStrength( double, int );
  Jet getJetStrength() { return JetStrength; }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitJetQuadrupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitJetQuadrupole( this ); }

  void eliminate();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetQuadrupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new JetQuadrupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );

} ;


class JetthinQuad : public bmlnElmnt
 {
 private:
   Jet KL;
   int strengthIndex;
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

 public:
   JetthinQuad( double, int );
   JetthinQuad( char*,      // name
                double,     // B-prime L in Tesla; + = horizontally focussing
                int );      // index of focal length parameter (> 6)
   JetthinQuad( bmlnElmntData& );
   JetthinQuad( const JetthinQuad& );
   ~JetthinQuad();
 
   void setStrength( double );
   void setStrength( double, int );
 
   void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
   void localPropagate( Particle& p );
   void localPropagate( JetParticle& );

   void accept( BmlVisitor& v ) { v.visitJetthinQuad( this ); }
   void accept( ConstBmlVisitor& v ) const { v.visitJetthinQuad( this ); }

   void eliminate();
 
   const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetthinQuad") != 0 ) return bmlnElmnt::isType(c); else return 1; }

   bmlnElmnt* Clone() const { return new JetthinQuad( *this ); }

} ;
 

#endif // JETQUADRUPOLE_H
