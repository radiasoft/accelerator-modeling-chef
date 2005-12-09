/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rbend.h
******  Version:   2.4
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

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif




class rbend : public bmlnElmnt
{
private:
  // bmlnElmnt::strength -> magnetic field [T]

  double _usEdgeAngle, _dsEdgeAngle;
                            // [radians] as defined in MAD for rbends.
  double _usAngle, _dsAngle;// [radians] entry (upstream) and exit (downstream) 
                            // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. If no
                            // registration particle is used, default
                            // values depend only on edge angles (see
                            // below).
  double _usTan, _dsTan;    // tangents of the entry and exit angles:
                            // px/pz of a reference particle at the
                            // upstream and downstream edges of the magnet.
                            // For a (usual) symmetric bend,
                            // sgn( _usTan ) = - sgn( _dsTan )
  double _dphi;             // angle between in- and out-frames: a derived 
                            // quantity.
  std::complex<double> _propPhase, _propTerm;
                            // Used to propagate through constant magnetic
                            // field using bend angle and edge angle data.

  bmlnElmnt::AsinFunctor  _myArcsin;
  void _calcPropParams();

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

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
    virtual ~NoEdge_Prop();
    // REMOVE: char isApproximate();
    // REMOVE: void makeApproximate();
    // REMOVE: void makeExact();
  private:
    double _fastArcsin( double x     ) const;
    Jet    _fastArcsin( const Jet& x ) const;
    // REMOVE: char   _approx;
  };
  static NoEdge_Prop NoEdge;

   class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Exact_Prop"; }

    Exact_Prop();
    virtual ~Exact_Prop();
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
    virtual ~InEdge_Prop();
    // REMOVE: char isApproximate();
    // REMOVE: void makeApproximate();
    // REMOVE: void makeExact();
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
    virtual ~OutEdge_Prop();
    // REMOVE: char isApproximate();
    // REMOVE: void makeApproximate();
    // REMOVE: void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static OutEdge_Prop OutEdge;

   class Null_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Null_Prop"; }

    Null_Prop();
    virtual ~Null_Prop();
    // REMOVE: char isApproximate();
    // REMOVE: void makeApproximate();
    // REMOVE: void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Null_Prop AbortProp;

   class Real_Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_Exact_Prop"; }

    Real_Exact_Prop();
    virtual ~Real_Exact_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_Exact_Prop RealExact;

  class Real_InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_InEdge_Prop"; }

    Real_InEdge_Prop();
    virtual ~Real_InEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_InEdge_Prop RealInEdge;

  class Real_OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_OutEdge_Prop"; }

    Real_OutEdge_Prop();
    virtual ~Real_OutEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_OutEdge_Prop RealOutEdge;

  friend class MAD_Prop;
  friend class NoEdge_Prop;
  friend class Exact_Prop;
  friend class InEdge_Prop;
  friend class OutEdge_Prop;
  friend class Null_Prop;
  friend class Real_Exact_Prop;

  // Constructors 
  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
                     // (assumed along the y-axis)
         PropFunc*    = &rbend::RealExact );

  rbend( const char*,// name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::RealExact );

  rbend( double,     // length  [ meters ]      // Symmetric bend
         double,     // field   [ tesla ]       // with parallel faces.
         double,     // entry angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( const char*,// name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // entry angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( double,     // length  [ meters ]     // No entry angle assumed
         double,     // field   [ tesla ]      // Must use a registration proton
                     // (assumed along the y-axis)
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  rbend( const char*,// name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // entry angle [radians] RELATIVE TO parallel faces
                     //   (assumes symmetric pssage unless reset)
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  rbend( const char*,// name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // entry angle [radians] RELATIVE TO parallel faces
                     //   (assumes symmetric pssage unless reset)
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  // REMOVE: rbend( double,     // length  [ meters ]
  // REMOVE:        double,     // field   [ tesla ]
  // REMOVE:        double,     // entry angle [ radians ]
  // REMOVE:        double,     // upstream edge angle [radians]
  // REMOVE:        double,     // downstream edge angle [radians]
  // REMOVE:                    // signs of previous two parameters
  // REMOVE:                    // are as defined for rbends by MAD
  // REMOVE:        PropFunc*    = &rbend::Exact );
  // REMOVE: 
  // REMOVE: rbend( const char*,// name
  // REMOVE:        double,     // length  [ meters ]
  // REMOVE:        double,     // field   [ tesla ]
  // REMOVE:        double,     // entry angle [ radians ]
  // REMOVE:        double,     // upstream edge angle [radians]
  // REMOVE:        double,     // downstream edge angle [radians]
  // REMOVE:                    // signs of previous two parameters
  // REMOVE:                    // are as defined for rbends by MAD
  // REMOVE:        PropFunc*    = &rbend::Exact );

  rbend( const rbend& );
  ~rbend();

  void releasePropFunc();
  void setupPropFunc();
  void eliminate();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitRbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitRbend( this ); }

  // Note: entry and exit angles are not arguments
  // in the rbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.
  double setEntryAngle( const Particle& ); 
  double setExitAngle( const Particle& ); 
  double getEntryAngle() const { return _usAngle; }
  double getExitAngle() const { return _dsAngle; }
  double setEntryAngle( double /* radians */ ); 
  double setExitAngle( double /* radians */ ); 
  double getEntryEdgeAngle() const { return _usEdgeAngle; }
  double getExitEdgeAngle() const { return _dsEdgeAngle; }

  // Aliases, for the sake of backwards compatability
  double setPoleFaceAngle( const Particle& x );
  double setPoleFaceAngle( const JetParticle& x );
  double PoleFaceAngle() const { return this->getEntryAngle(); }
  double getPoleFaceAngle() const { return this->getEntryAngle(); }
  double getTanPoleFaceAngle() const { return tan(this->getEntryAngle()); }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  void makeAsinApproximate( int /* number of terms */);
  void makeAsinExact();
  bool isAsinExact();

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rbend( *this ); }
  double OrbitLength( const Particle& );
    // Computes arclength of orbit assuming a symmetric bend.
    // WARNING: This is not the true arclength under all conditions.
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.
};

#endif // RBEND_H
