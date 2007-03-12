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
****** REVISION HISTORY:
******
****** Mar 2007    ostiguy@fnal.gov
****** - added support for reference counted smart pointers.
**************************************************************************
*************************************************************************/

#ifndef CF_SBEND_H
#define CF_SBEND_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;

class Particle;
class JetParticle;

class CF_sbend;

typedef boost::shared_ptr<CF_sbend>            CFSbendPtr;
typedef boost::shared_ptr<CF_sbend const> ConstCFSbendPtr;

class DLLEXPORT CF_sbend : public bmlnElmnt
{
 public:
  // Constructors
  CF_sbend();
  CF_sbend( double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            double const&,     // bend angle   [ radians ]
            int = 4 );  // ignored

  CF_sbend( const char*,// name
            double const&,     // length       [ meters ]
            double const&,     // field        [ tesla ]
            double const&,     // bend angle   [ radians ]
            int = 4 );  // ignored

  CF_sbend( double const&,     // length  [ meters ]
            double const&,     // field   [ tesla ]
                        // (assumed along the y-axis)
            double const&,     // geometric bend angle   [ radians ]
            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for sbends by MAD
            int = 4 );  // ignored

  CF_sbend( const char*,// name
            double const&,     // length  [ meters ]
            double const&,     // field   [ tesla ]
            double const&,     // geometric bend angle   [ radians ]
            double const&,     // upstream edge angle [radians]
            double const&,     // downstream edge angle [radians]
                        // signs of previous two parameters
                        // are as defined for sbends by MAD
            int = 4 );  // ignored

  CF_sbend( const CF_sbend& );
  CF_sbend* Clone() const 
    { return new CF_sbend( *this ); }


  ~CF_sbend();

  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ); 
  void accept( ConstBmlVisitor& v ) const; 
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v ) const;
  
  void peekAt( double& s, const Particle& );

  const char* Type() const;
  bool isMagnet() const;
  
  double OrbitLength( Particle const& );
  void Split( double const&, bmlnElmnt**, bmlnElmnt** ) const;
    // WARNING: After the Split function is used, the new elements 
    // must be commissioned with RefRegVisitor.


  // Note: entry and exit angles are not arguments
  // in the rbend constructors. A symmetric bend is assumed
  // by default. Otherwise, use one of the following.

  double setEntryAngle( Particle const& ); 
  double setExitAngle ( Particle const& ); 

  double getEntryAngle()   const { return _usAngle; }
  double getExitAngle()    const { return _dsAngle; }

  double setEntryAngle( double const& radians); 
  double setExitAngle(  double const& radians); 

  double getEntryEdgeAngle() const { return _usEdgeAngle; }
  double getExitEdgeAngle()  const { return _dsEdgeAngle; }

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

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
