/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbend.h
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
****** Aug 2007           ostiguy@fnal.gov
****** - composite structure based on regular beamline
******
****** Dec 2007           ostiguy@fnal.gov
****** - new type-safe propagators
******
****** Apr 2008           michelotti@fnal.gov
****** - added placeholder setLength method
****** - changed interpretation of "ang" argument to
******   two constructors from "entry angle" to "bend angle,"
******   in order to conform with usage in other bend constructors.
******
**************************************************************************
*************************************************************************/

#ifndef CF_RBEND_H
#define CF_RBEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>
#include <list>
#include <utility>

class BmlVisitor;
class ConstBmlVisitor;

class Particle;
class JetParticle;

class CF_rbend;

typedef boost::shared_ptr<CF_rbend>            CFRbendPtr;
typedef boost::shared_ptr<CF_rbend const> ConstCFRbendPtr;

class DLLEXPORT CF_rbend : public bmlnElmnt
{

  class Propagator;

 public:

  typedef boost::shared_ptr<BasePropagator<CF_rbend> > PropagatorPtr; 
 
  //---------------------------------------------------------------------------------------------
  // length is expressed in [m]
  // field  is in [T], field pointing to the -y direction is positive 
  // All angles are espressed in radians
  // usAngle, dsAngle  upstream and downstream pole face angles, RELATIVELY to the parallel faces.
  // usFaceAngle, dsFaceAngle upstream and downstream "edge" angles MAD sign convention
  // If usAngle, dsAngle are not specified, usAngle=dsAngle=0.0
  // If only usAngle is specified, then usAngle = dsAngle is assumed
  // ---------------------------------------------------------------------------------------------
  
  CF_rbend();
  CF_rbend( char const* name, double const& length, double const& field);
  CF_rbend( char const* name, double const& length, double const& field, double const& bendangle );
  CF_rbend( char const* name, double const& length, double const& field, double const& usFaceAngle, double const& dsFaceAngle);
  CF_rbend( char const* name, double const& length, double const& field, double const& bendangle, 
                                                                         double const& usFaceAngle, double const& dsFaceAngle);

  CF_rbend(CF_rbend const& );

  CF_rbend* Clone() const 
     { return new CF_rbend( *this ); }

  ~CF_rbend();

  CF_rbend& operator=( CF_rbend const& rhs);

  void localPropagate(         Particle& );
  void localPropagate(      JetParticle& );
  void localPropagate(    ParticleBunch& );
  void localPropagate( JetParticleBunch& );


  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 
  
  void peekAt( double& s, const Particle& );

  const char* Type() const;
  bool isMagnet() const;
  
  double OrbitLength( const Particle& );
  void Split( double const&, ElmPtr&, ElmPtr& ) const;
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.

  // Note: entry and exit angles are not arguments
  // in the rbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.

  double setEntryAngle( Particle const& ); 
  double  setExitAngle( Particle const& ); 
  double setEntryAngle( double const& ); 
  double  setExitAngle( double const& ); 

  double getEntryAngle()    const;
  double  getExitAngle()    const;

  void setPoleFaceAngle( Particle const& p );

  double setEntryFaceAngle( double const& radians ); 
  double  setExitFaceAngle( double const& radians); 

  double getEntryFaceAngle() const;
  double  getExitFaceAngle() const; 

  double const& getBendAngle() const; // bend angle

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  double AdjustPosition( const Particle& );
  double AdjustPosition( const JetParticle& );

  int setQuadrupole ( double const& );  
  int setSextupole  ( double const& );  
  int setOctupole   ( double const& );  

  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B''l/2 )
  //       .setOctupole   ( B'''l/6 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  int setDipoleField ( double const& );  
  // Here the argument is the dipole field, 
  // NOT the integrated dipole field.

  void setStrength   ( double const& );
  // Specific implementation of virtual bmlnElmnt method.
  // Modifies all internal elements.
  void setLength     ( double const& );

  double getQuadrupole() const;
  double getSextupole()  const;
  double getOctupole()   const;

  // Returns integrated multipole strengths
  // i.e., .getQuadrupole() returns B'l
  //       .getSextupole()  returns B''l/2
  //       .getOctupole()   returns B'''l/6

  double getDipoleField() const;
  // Returns the dipole field,
  // NOT the integrated dipole field.

 private:

  double  angle_;           // total bend angle  [ radians ]
  double  usFaceAngle_;     // [radians] as defined in MAD for rbends.
  double  dsFaceAngle_; 
  double  usAngle_;         // [radians] entry (upstream) and exit (downstream) 
  double  dsAngle_;         // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. If no
                            // registration particle is used, default
                            // values depend only on edge angles (see
                            // below).

  std::list<std::pair<int, std::complex<double> > > multipoles_;
  
  PropagatorPtr  propagator_;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};

#endif // CF_RBEND
