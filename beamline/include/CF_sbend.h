/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      CF_sbend.h
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


#ifndef CF_SBEND_H
#define CF_SBEND_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class Particle;
class JetParticle;

class CF_sbend : public bmlnElmnt
{
 public:
  // Constructors
  CF_sbend( double,     // length       [ meters ]
            double,     // field        [ tesla ]
            double,     // bend angle   [ radians ]
            int = 4 );  // ignored

  CF_sbend( const char*,// name
            double,     // length       [ meters ]
            double,     // field        [ tesla ]
            double,     // bend angle   [ radians ]
            int = 4 );  // ignored

  CF_sbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
                        // (assumed along the y-axis)
            double,     // geometric bend angle   [ radians ]
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for sbends by MAD
            int = 4 );  // ignored

  CF_sbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // geometric bend angle   [ radians ]
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for sbends by MAD
            int = 4 );  // ignored

  CF_sbend( const CF_sbend& );

  ~CF_sbend();
  void eliminate();


  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) { v.visitCF_sbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCF_sbend( this ); }
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );
  
  // REMOVE: void peekAt( double& s, Particle* = 0 );
  void peekAt( double& s, const Particle& ) const;

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "CF_sbend") != 0 )
    return bmlnElmnt::isType(c); else return 1; }
  bool isMagnet() const;
  
  bmlnElmnt* Clone() const 
    { return new CF_sbend( *this ); }
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






  // REMOVE: double AdjustPosition( const Particle& );
  // REMOVE: double AdjustPosition( const JetParticle& );


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

  double getBendAngle() const;
  double BendAngle()    const { return this->getBendAngle(); }
  double getAngle()     const { return this->getBendAngle(); }
  double Angle()        const { return this->getBendAngle(); }

 private:
  bmlnElmnt** _u;           // Address of first internal bmlElmnt pointer
  bmlnElmnt** _v;           // Address of final internal bmlElmnt pointer

  double _angle;            // total bend angle  [ radians ]
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
  void _finishConstructor();

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // CF_SBEND
