/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      CF_rbend.h
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


#ifndef CF_RBEND_H
#define CF_RBEND_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class Proton;
class JetProton;

class CF_rbend : public bmlnElmnt
{
 public:
  // Constructors
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

  CF_rbend( double,     // length  [ meters ]     // No entry angle assumed
            double,     // field   [ tesla ]      // Must use a registration proton
                        // (assumed along the y-axis)

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

  CF_rbend( double,     // length  [ meters ]     // No entry angle assumed
            double,     // field   [ tesla ]      // Must use a registration proton
                        // (assumed along the y-axis)
            double,     // entry angle [radians]
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // entry angle [radians]
            double,     // upstream edge angle [radians]
            double,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for rbends by MAD
            int = 1 );

  CF_rbend( const CF_rbend& );

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
  
  void peekAt( double& s, Particle* = 0 );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "CF_rbend") != 0 )
    return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const 
    { return new CF_rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.


  // Note: entry and exit angles are not arguments
  // in the rbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.
  double setEntryAngle( const Particle& ); 
  double setExitAngle( const Particle& ); 
  double getEntryAngle() { return _usAngle; }
  double getExitAngle()  { return _dsAngle; }
  double setEntryAngle( double /* radians */ ); 
  double setExitAngle( double /* radians */ ); 

  // Aliases, for the sake of backwards compatability
  double PoleFaceAngle()    { return this->getEntryAngle(); }
  double getPoleFaceAngle() { return this->getEntryAngle(); }


  double AdjustPosition( const Proton& );
  double AdjustPosition( const JetProton& );


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






 private:
  bmlnElmnt** _u;
  bmlnElmnt** _v;


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
