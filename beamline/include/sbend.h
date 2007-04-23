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
******    REVISION HISTORY
****** Mar 2007            ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******                                                                
**************************************************************************
*************************************************************************/
#ifndef SBEND_H
#define SBEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class sbend;

typedef boost::shared_ptr<sbend>            SBendPtr;
typedef boost::shared_ptr<sbend const> ConstSBendPtr;


class DLLEXPORT sbend : public bmlnElmnt {

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
    virtual ~NoEdge_Prop();

  private:

    double _fastArcsin( double const& x     ) const;
    Jet    _fastArcsin( const Jet& x ) const;
  };
  static NoEdge_Prop NoEdge;

   class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::Exact_Prop"; }

    Exact_Prop();
    virtual ~Exact_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* myPropagator_;
  };
  static Exact_Prop Exact;

   class InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::InEdge_Prop"; }

    InEdge_Prop();
    virtual ~InEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* myPropagator_;
  };
  static InEdge_Prop InEdge;

   class OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::OutEdge_Prop"; }

    OutEdge_Prop();
    virtual ~OutEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* myPropagator_;
  };
  static OutEdge_Prop OutEdge;

  friend class MAD_Prop;
  friend class NoEdge_Prop;
  friend class Exact_Prop;
  friend class InEdge_Prop;
  friend class OutEdge_Prop;


  void P_Face ( Particle&,    double const& psi ) const;
  void J_Face ( JetParticle&, double const& psi ) const;
  

  // Constructors 
  sbend();
  sbend( double const&,     // (orbit) length [meters]
         double const&,     // magnetic field [tesla]
                     // (assumed along the y-axis)
         double const&,     // geometric bend angle [radians]
                     // sign( bend angle ) = sign( field )
         PropFunc*    = &sbend::Exact );

  sbend( const char*,// name
         double const&,     // (orbit) length  [meters]
         double const&,     // magnetic field [tesla]
         double const&,     // geometric bend angle   [radians]
         PropFunc*    = &sbend::Exact );

  sbend( double const&,     // (orbit) length  [meters]
         double const&,     // field   [tesla]
         double const&,     // geometric bend angle [radians]
         double const&,     // upstream edge angle [radians]
         double const&,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for sbends by MAD
         PropFunc*    = &sbend::Exact );

  sbend( const char*,// name
         double const&,     // (orbit) length  [meters]
         double const&,     // field   [tesla]
         double const&,     // geometric bend angle [radians]
         double const&,     // upstream edge angle [radians]
         double const&,     // downstream edge angle [radians]
                     // signs of previous two parameters
                     // are as defined for sbends by MAD
         PropFunc*    = &sbend::Exact );

  sbend( const sbend& );

  sbend* Clone() const { return new sbend( *this ); }

  virtual ~sbend();


  // Public methods
  double setAngle(double const& a) { return (angle_ = a); }
  double getAngle() const   { return angle_; }
  // aliased
  double setBendAngle(double const& a) { return (angle_ = a); }
  double getBendAngle() const   { return angle_; }
  

  // Note: entry and exit angles are not arguments
  // in the sbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.

  double setEntryAngle( const Particle& ); 
  double  setExitAngle( const Particle& ); 
  double getEntryAngle()              const { return usAngle_; }
  double getExitAngle()               const { return dsAngle_; }
  double setEntryAngle( double const& radians); 
  double setExitAngle( double const&  radians); 
  double getEntryEdgeAngle()          const { return usEdgeAngle_; }
  double getExitEdgeAngle()           const { return dsEdgeAngle_; }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  void makeAsinApproximate( int /* number of terms */);
  void makeAsinExact();
  bool isAsinExact();

  void releasePropFunc();
  void setupPropFunc();

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*propfunc_)( this, p ); }
  void localPropagate( JetParticle& p ) { (*propfunc_)( this, p ); }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

private:

  // bmlnElmnt::strength -> magnetic field [T]

  double angle_;            // [radians] bend angle
  double usEdgeAngle_, dsEdgeAngle_;
                            // [radians] as defined in MAD for sbends.
  double usAngle_, dsAngle_;// [radians] entry (upstream) and exit (downstream) 
                            // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. If no
                            // registration particle is used, default
                            // values depend only on edge angles (see
                            // below).
  double usTan_, dsTan_;    // tangents of the entry and exit angles:
                            // px/pz of a reference particle at the
                            // upstream and downstream edges of the magnet.
                            // For a (usual) symmetric bend,
                            // sgn( _usTan ) = - sgn( _dsTan )
  double dphi_;             // angle between in- and out-frames: a derived 
                            // quantity
  std::complex<double> propPhase_, propTerm_;
                            // Used to propagate through constant magnetic
                            // field using bend angle and edge angle data.

  void calcPropParams();

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);


};

#endif // SBEND_H
