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
**************************************************************************
*************************************************************************/

#ifndef CF_RBEND_H
#define CF_RBEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>

class Particle;
class JetParticle;

class DLLEXPORT CF_rbend : public bmlnElmnt
{
 public:
  // Constructors
  CF_rbend();
  CF_rbend( double,     // length       [ meters ]
            double,     // field        [ tesla ]
            int n = 1 );// number of blocks: 4n+1 bends + 2(4n) thin multipoles
 
  CF_rbend( const char*,// name
            double,     // length       [ meters ]
            double,     // field        [ tesla ]
            int = 1 );

  CF_rbend( double,     // length       [ meters ]
            double,     // field        [ tesla ]
            double,     // entry angle  [ radians ]
            int n = 1 );// number of blocks: 4n+1 bends + 2(4n) thin multipoles
 
  CF_rbend( const char*,// name
            double,     // length       [ meters ]
            double,     // field        [ tesla ]
            double,     // entry angle  [ radians ]
            int = 1 );

  CF_rbend( double,     // length  [ meters ]           // No entry angle assumed.
            double,     // field   [ tesla ]            // Must be registered.
                        // (assumed along the y-axis)   // See RefRegVisitor

            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]

            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
                        // (assumed along the y-axis)
            double,     // entry angle [radians] RELATIVE TO parallel faces
                        //   (assumes symmetric pssage unless reset)
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // entry angle [radians] RELATIVE TO parallel faces
                        //   (assumes symmetric pssage unless reset)
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const CF_rbend& );

  CF_rbend* Clone() const 
     { return new CF_rbend( *this ); }

  ~CF_rbend();
  void eliminate();

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) { v.visitCF_rbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCF_rbend( this ); }
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );
  
  // REMOVE: void peekAt( double& s, Particle* = 0 );
  void peekAt( double& s, const Particle& ) const;

  const char* Type() const;
  bool isMagnet() const;
  
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.


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

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  // Aliases, for the sake of backwards compatability
  double PoleFaceAngle() const { return this->getEntryAngle(); }
  double getPoleFaceAngle() const { return this->getEntryAngle(); }
  double getTanPoleFaceAngle() const { return tan(this->getEntryAngle()); }


  double AdjustPosition( const Particle& );
  double AdjustPosition( const JetParticle& );


  int setQuadrupole ( double );  
  int setSextupole  ( double );  
  int setOctupole   ( double );  
  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B''l/2 )
  //       .setOctupole   ( B'''l/6 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  int setDipoleField ( double );  
  // Here the argument is the dipole field, 
  // NOT the integrated dipole field.
  void setStrength   ( double );
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
  bmlnElmnt** _u;           // Address of first internal bmlElmnt pointer
  bmlnElmnt** _v;           // Address of final internal bmlElmnt pointer


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

  void _calcPropParams();
  void _finishConstructor( int /* number of element blocks*/ );

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // CF_RBEND
