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
****** Mar 2007           ostiguy@fnal.gov
****** - covariant return types
****** - support for reference counted elements
**************************************************************************
*************************************************************************/
#ifndef RBEND_H
#define RBEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class rbend;

typedef boost::shared_ptr<rbend>       RBendPtr;
typedef boost::shared_ptr<rbend const> ConstRBendPtr;

class BmlVisitor;
class ConstBmlVisitor;

class DLLEXPORT rbend : public bmlnElmnt
{

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

  private:
    double _fastArcsin( double const& x     ) const;
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

  // length  [ meters ]
  // field   [ tesla ] (assumed along the y-axis)

  rbend();
  rbend( double const& length, double const& field, PropFunc* = &rbend::RealExact );

  rbend( char const* name, double const& length, double const& field, PropFunc* = &rbend::RealExact );

// Symmetric bend with parallel faces.

  rbend(                   double const& length, double const& field, double const& entry_angle, PropFunc* = &rbend::Exact );

  rbend( const char* name, double const& length, double const& field, double const& entry_angle, PropFunc* = &rbend::Exact );

  rbend(                   double const& length, double const& field, double const& upstream_edge_angle, double const& downstream_edge_angle, PropFunc* = &rbend::RealExact );

  rbend( const char* name, double const&, // length  [ meters ]
         double const&,     // field   [ tesla ]
         double const&,     // upstream edge angle [radians]
         double const&,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  rbend( double const&,     // length  [ meters ]
         double const&,     // field   [ tesla ]
         double const&,     // entry angle [radians] RELATIVE TO parallel faces
                     //   (assumes symmetric pssage unless reset)
         double const&,     // upstream edge angle [radians]
         double const&,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );

  rbend( const char*,// name
         double const&,     // length  [ meters ]
         double const&,     // field   [ tesla ]
         double const&,     // entry angle [radians] RELATIVE TO parallel faces
                     //   (assumes symmetric pssage unless reset)
         double const&,     // upstream edge angle [radians]
         double const&,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for rbends by MAD
         PropFunc*    = &rbend::RealExact );


  rbend( rbend const& );

  rbend* Clone() const { return new rbend( *this ); }

  virtual ~rbend();

  void releasePropFunc();
  void setupPropFunc();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p )   { (*propfunc_)( this, p ); }
  void localPropagate( JetParticle& p )   { (*propfunc_)( this, p ); }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  // Note: entry and exit angles are not arguments
  // in the rbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.

  double setEntryAngle( Particle const& ); 
  double setExitAngle(  Particle const& ); 
  double getEntryAngle()                  const { return usAngle_; }
  double getExitAngle()                   const { return dsAngle_; }
  double setEntryAngle( double const& radians ); 
  double setExitAngle(  double const& radians); 
  double getEntryEdgeAngle()              const { return usEdgeAngle_; }
  double getExitEdgeAngle()               const { return dsEdgeAngle_; }

  // Aliases, for the sake of backwards compatability

  double setPoleFaceAngle(    Particle const& x );
  double setPoleFaceAngle( JetParticle const& x );
  double PoleFaceAngle()                           const { return this->getEntryAngle(); }
  double getPoleFaceAngle()                        const { return this->getEntryAngle(); }
  double getTanPoleFaceAngle()                     const { return tan(this->getEntryAngle()); }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  void makeAsinApproximate( int nterms);
  void makeAsinExact();
  bool isAsinExact();

  const char* Type() const;
  bool isMagnet() const;

  double OrbitLength( Particle const& );
    // Computes arclength of orbit assuming a symmetric bend.
    // WARNING: This is not the true arclength under all conditions.

  void Split( double const&, ElmPtr&, ElmPtr& ) const;
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.

private:
  // bmlnElmnt::strength -> magnetic field [T]

  double usEdgeAngle_;      // [radians] as defined in MAD for rbends.
  double dsEdgeAngle_;
  double usAngle_;          // [radians] entry (upstream) and exit (downstream) 
  double dsAngle_;          // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. If no
                            // registration particle is used, default
                            // values depend only on edge angles (see
                            // below).
  double usTan_;            // tangents of the entry and exit angles:
  double dsTan_;            // px/pz of a reference particle at the
                            // upstream and downstream edges of the magnet.
                            // For a (usual) symmetric bend,
                            // sgn( _usTan ) = - sgn( _dsTan )
  double dphi_;             // angle between in- and out-frames: a derived 
                            // quantity.

  std::complex<double>   propPhase_;   // Used to propagate through constant magnetic
  std::complex<double>   propTerm_;    // field using bend angle and edge angle data.

  void calcPropParams();

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};

#endif // RBEND_H
