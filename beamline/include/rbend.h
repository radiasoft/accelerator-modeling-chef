/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      rbend.h
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


#ifndef RBEND_H
#define RBEND_H

#include "bmlnElmnt.h"

class rbend : public bmlnElmnt
{
private:
  // ??? REMOVE: static void P_Face    ( const bmlnElmnt*, Particle& );
  // ??? REMOVE: static void J_Face    ( const bmlnElmnt*, JetParticle& );
  double poleFaceAngle;
  double tanPFAngle;

  // bmlnElmnt::strength -> magnetic field [T]
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);

public:
  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class NoEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::NoEdge_Prop"; }

    NoEdge_Prop();
    ~NoEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
  private:
    double _fastArcsin( double x     ) const;
    Jet    _fastArcsin( const Jet& x ) const;
    char   _approx;
  };
  static NoEdge_Prop NoEdge;

  class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Exact_Prop"; }

    Exact_Prop();
    ~Exact_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Exact_Prop Exact;

  class InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::InEdge_Prop"; }

    InEdge_Prop();
    ~InEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static InEdge_Prop InEdge;

  class OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::OutEdge_Prop"; }

    OutEdge_Prop();
    ~OutEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static OutEdge_Prop OutEdge;

  // ------------------------------------------------------

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::Exact );

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::Exact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( const rbend& );
  ~rbend();

  void eliminate();

  void accept( BmlVisitor& v ) { v.visitRbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitRbend( this ); }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );


  double PoleFaceAngle()       const {return poleFaceAngle;} 
  double getPoleFaceAngle()    const {return poleFaceAngle;} 
  double getTanPoleFaceAngle() const {return tanPFAngle;}    
  double getEntryAngle()       const {return poleFaceAngle;} 

  double setPoleFaceAngle( const Particle& );
  double setPoleFaceAngle( const JetParticle& );
  void   setEntryAngle( const Particle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( const JetParticle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( double a )
  { 
     poleFaceAngle = a;
     tanPFAngle    = tan( poleFaceAngle );
  }
  double setAngle( double a )
  { 
     this->setEntryAngle(a);
     return poleFaceAngle;
  }
};

#endif // RBEND_H
