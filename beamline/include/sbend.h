/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sbend.h
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


#ifndef SBEND_H
#define SBEND_H

#include "bmlnElmnt.h"

class sbend : public bmlnElmnt
{
private:
  // bmlnElmnt::strength -> magnetic field [T]
  double _angle;            // [radians] bend angle
  double _usEdgeAngle, _dsEdgeAngle;
                            // [radians] as defined in MAD for sbends.
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
                            // quantity
  FNAL::Complex _propPhase, _propTerm;
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
    const char* Type() const { return "sbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

   class NoEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::NoEdge_Prop"; }

    NoEdge_Prop();
    ~NoEdge_Prop();
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
    const char* Type() const { return "sbend::Exact_Prop"; }

    Exact_Prop();
    ~Exact_Prop();
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
    const char* Type() const { return "sbend::InEdge_Prop"; }

    InEdge_Prop();
    ~InEdge_Prop();
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
    const char* Type() const { return "sbend::OutEdge_Prop"; }

    OutEdge_Prop();
    ~OutEdge_Prop();
    // REMOVE: char isApproximate();
    // REMOVE: void makeApproximate();
    // REMOVE: void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static OutEdge_Prop OutEdge;

  friend class MAD_Prop;
  friend class NoEdge_Prop;
  friend class Exact_Prop;
  friend class InEdge_Prop;
  friend class OutEdge_Prop;

  // REMOVE: friend class Null_Prop : public bmlnElmnt::PropFunc
  // REMOVE: {
  // REMOVE: public:
  // REMOVE:   int operator()( bmlnElmnt*, Particle&    );
  // REMOVE:   int operator()( bmlnElmnt*, JetParticle& );
  // REMOVE:   const char* Type() const { return "sbend::Null_Prop"; }
  // REMOVE: 
  // REMOVE:   Null_Prop();
  // REMOVE:   ~Null_Prop();
  // REMOVE:   // REMOVE: char isApproximate();
  // REMOVE:   // REMOVE: void makeApproximate();
  // REMOVE:   // REMOVE: void makeExact();
  // REMOVE:   void setPropagator( NoEdge_Prop* );
  // REMOVE: private:
  // REMOVE:   NoEdge_Prop* _myPropagator;
  // REMOVE: };
  // REMOVE: static Null_Prop AbortProp;

  // REMOVE: friend class Approx_Prop : public bmlnElmnt::PropFunc
  // REMOVE: {
  // REMOVE: public:
  // REMOVE:   int operator()( bmlnElmnt*, Particle&    );
  // REMOVE:   int operator()( bmlnElmnt*, JetParticle& );
  // REMOVE:   const char* Type() const { return "sbend::Fast_Prop"; }
  // REMOVE: private:
  // REMOVE:   double _fastArcsin( double x     ) const;
  // REMOVE:   Jet    _fastArcsin( const Jet& x ) const;
  // REMOVE: };
  // REMOVE: static Approx_Prop Approx;

  void P_Face ( Particle&,    const double& /* psi */  ) const;
  void J_Face ( JetParticle&, const double& /* psi */  ) const;
  

  // Constructors 
  sbend( double,     // (orbit) length [meters]
         double,     // magnetic field [tesla]
                     // (assumed along the y-axis)
         double,     // geometric bend angle [radians]
                     // sign( bend angle ) = sign( field )
         PropFunc*    = &sbend::Exact );

  sbend( const char*,// name
         double,     // (orbit) length  [meters]
         double,     // magnetic field [tesla]
         double,     // geometric bend angle   [radians]
         PropFunc*    = &sbend::Exact );

  sbend( double,     // (orbit) length  [meters]
         double,     // field   [tesla]
         double,     // geometric bend angle [radians]
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for sbends by MAD
         PropFunc*    = &sbend::Exact );

  sbend( const char*,// name
         double,     // (orbit) length  [meters]
         double,     // field   [tesla]
         double,     // geometric bend angle [radians]
         double,     // upstream edge angle [radians]
         double,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for sbends by MAD
         PropFunc*    = &sbend::Exact );

  sbend( const sbend& );
  ~sbend();


  // Public methods
  double setAngle(double a) { return (_angle = a); }
  double getAngle() const   { return _angle; }
  // aliassed
  double setBendAngle(double a) { return (_angle = a); }
  double getBendAngle() const   { return _angle; }
  

  // Note: entry and exit angles are not arguments
  // in the sbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.
  double setEntryAngle( const Particle& ); 
  double setExitAngle( const Particle& ); 
  double getEntryAngle() const { return _usAngle; }
  double getExitAngle()  const { return _dsAngle; }
  double setEntryAngle( double /* radians */ ); 
  double setExitAngle( double /* radians */ ); 
  double getEntryEdgeAngle() const { return _usEdgeAngle; }
  double getExitEdgeAngle()  const { return _dsEdgeAngle; }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  void makeAsinApproximate( int /* number of terms */);
  void makeAsinExact();
  bool isAsinExact();

  void releasePropFunc();
  void setupPropFunc();
  void eliminate();

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitSbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSbend( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new sbend( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
};

#endif // SBEND_H
