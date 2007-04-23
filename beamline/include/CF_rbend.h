/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbend.h
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

#ifndef CF_RBEND_H
#define CF_RBEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>

class BmlVisitor;
class ConstBmlVisitor;

class Particle;
class JetParticle;

class CF_rbend;

typedef boost::shared_ptr<CF_rbend>            CFRbendPtr;
typedef boost::shared_ptr<CF_rbend const> ConstCFRbendPtr;

class DLLEXPORT CF_rbend : public bmlnElmnt
{
 public:
  // Constructors
  CF_rbend();
  CF_rbend( double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            int n = 1 );// number of blocks: 4n+1 bends + 2(4n) thin multipoles
 
  CF_rbend( const char*,// name
            double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            int = 1 );

  CF_rbend( double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            double const&,     // entry angle  [ radians ]
            int n = 1 );// number of blocks: 4n+1 bends + 2(4n) thin multipoles
 
  CF_rbend( const char*,// name
            double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            double const&,     // entry angle  [ radians ]
            int = 1 );

  CF_rbend( double const&,     // length  [ meters ]           // No entry angle assumed.
            double const&,     // field   [ tesla ]            // Must be registered.
                        // (assumed along the y-axis)   // See RefRegVisitor

            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const char*,// name
            double const&,     // length  [ meters ]
            double const&,     // field   [ tesla ]

            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( double const&,     // length  [ meters ]
            double const&,     // field   [ tesla ]
                        // (assumed along the y-axis)
            double const&,     // entry angle [radians] RELATIVE TO parallel faces
                        //   (assumes symmetric pssage unless reset)
            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const char*,// name
            double const&,     // length  [ meters ]
            double const&,     // field   [ tesla ]
            double const&,     // entry angle [radians] RELATIVE TO parallel faces
                        //   (assumes symmetric pssage unless reset)
            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const CF_rbend& );

  CF_rbend* Clone() const 
     { return new CF_rbend( *this ); }

  ~CF_rbend();

  CF_rbend& operator=( CF_rbend const& rhs);

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 

  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );
  
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
  double setEntryAngle( const Particle& ); 
  double setExitAngle( const Particle& ); 
  double getEntryAngle() const { return usAngle_; }
  double getExitAngle() const { return dsAngle_; }
  double setEntryAngle( double const& /* radians */ ); 
  double setExitAngle( double const& /* radians */ ); 
  double getEntryEdgeAngle() const { return  usEdgeAngle_; }
  double getExitEdgeAngle() const { return   dsEdgeAngle_; }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  // Aliases, for the sake of backwards compatability
  double PoleFaceAngle() const { return this->getEntryAngle(); }
  double getPoleFaceAngle() const { return this->getEntryAngle(); }
  double getTanPoleFaceAngle() const { return tan(this->getEntryAngle()); }


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

  bmlnElmnt** u_;           // Address of first internal bmlElmnt pointer
  bmlnElmnt** v_;           // Address of final internal bmlElmnt pointer


  double usEdgeAngle_, dsEdgeAngle_;
                            // [radians] as defined in MAD for rbends.
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
                            // sgn( usTan_ ) = - sgn( dsTan_ )

  void calcPropParams();
  void finishConstructor( int /* number of element blocks*/ );

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // CF_RBEND
