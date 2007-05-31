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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - covariant return types
****** - support for reference counted elements
**************************************************************************
*************************************************************************/

#ifndef QUADRUPOLE_H
#define QUADRUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;
class quadrupole;
class thinQuad;

typedef boost::shared_ptr<quadrupole> QuadrupolePtr; 
typedef boost::shared_ptr<thinQuad>   ThinQuadPtr; 

typedef boost::shared_ptr<quadrupole const> ConstQuadrupolePtr; 
typedef boost::shared_ptr<thinQuad const>   ConstThinQuadPtr; 


class DLLEXPORT quadrupole : public bmlnElmnt
{


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



  // length    in meters^-1
  // strength (B') in Tesla-meters^-1


  quadrupole();
  quadrupole( const char* name, double const& length, double const& strength, PropFunc* = &quadrupole::LikeTPOT );
  quadrupole(                   double const& length, double const& strength, PropFunc* = &quadrupole::LikeTPOT );
  quadrupole( quadrupole const& );

  quadrupole* Clone() const { return new quadrupole( *this ); }

  virtual ~quadrupole();

  void setStrength( double const& );
  void setStrength( double const&, int );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p )   { (*propfunc_)( this, p );        }
  void localPropagate( JetParticle& p )   { (*propfunc_)( this, p );        }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void releasePropFunc();
  void setupPropFunc();

  const char* Type() const;
  bool isMagnet() const;

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

private:
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
} ;



class DLLEXPORT thinQuad : public bmlnElmnt
{


public:

  // integrated_strength (B'L) in Tesla; + = horizontally focussing

  thinQuad();
  thinQuad(                   double const& integrated_strength );    
  thinQuad( char const* name, double const& integrated_strength );    // B'L in Tesla; + = horizontally focussing
  thinQuad( thinQuad const& );

  thinQuad* Clone() const { return new thinQuad( *this ); }

  virtual ~thinQuad();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;

  bool        isMagnet()   const;
  char const* Type()       const;
  
} ;



#endif // QUADRUPOLE_H
