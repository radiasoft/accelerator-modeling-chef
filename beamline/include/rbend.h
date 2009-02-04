/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rbend.h
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
******
****** Dec 2008 ostiguy@fnal
****** - restored basic support for MAD-style propagator 
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder setLength method
****** - added setStrength method
******   : not needed in earlier implementations because
******     rbend had no internal structure then.
****** - added member functions to nullify edge effects
******   : used by rbend::Split
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators 
****** - rbend is now implemented as composite element
****** Mar 2007           ostiguy@fnal.gov
****** - covariant return types
****** - support for reference counted elements
******     
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

  class    Propagator;
  class MADPropagator;

 public:

  class        rbend_core_access;
  friend class rbend_core_access;

  // length  [ meters ]
  // field   [ tesla ] (assumed along the y-axis)

  rbend();

  // Symmetric bend with parallel faces.
  rbend( std::string const& name, 
         double const& length,         // length [ meters ] 
         double const& field,          // field  [ tesla ] (assumed along the y-axis) 
         double const& bendangle );    // as specified in MAD;
                                       // : sets entry and exit angles by default
                                       // : for edge focusing calculations.

  // Other possibilities

  rbend( std::string const& name, 
         double const& length, 
         double const& field, 
         double const& bendangle, 
         double const& entry_angle );  // upstream entry angle
                                       // : assumed symmetric
  rbend( std::string const& name, 
         double const& length, 
         double const& field, 
         double const& bendangle,
         double const& usedge,         // upstream edge angle [radians]     
         double const& dsedge);        // downstream edge angle [radians]
                                       // : signs of previous two parameters
                                       // : are as defined for rbends by MAD
  rbend( std::string const& name, 
         double const& length,
         double const& field,
         double const& bendangle,
         double const& entry_angle,    // entry angle [radians] RELATIVE TO parallel faces
                                       // (assumes symmetric pssage unless reset)
         double const& usedge,
         double const& dsedge );

  rbend( rbend const& );
 ~rbend();

  rbend* Clone() const { return new rbend( *this ); }

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

 // Note: fiducial trajectory entry and exit angles are not arguments
 // in the rbend constructors. A symmetric bend is assumed
 // by default. Otherwise, use one of the following.

  double setEntryAngle( Particle const& ); 
  double  setExitAngle( Particle const& ); 
  double setEntryAngle( double const& ); 
  double  setExitAngle( double const& ); 

  double getEntryAngle()    const;
  double  getExitAngle()    const;

  void          setPoleFaceAngle( Particle const& p );
  double const& getPoleFaceAngle() const;

  double setEntryFaceAngle( double const& radians ); 
  double  setExitFaceAngle( double const& radians); 

  double getEntryFaceAngle() const;
  double  getExitFaceAngle() const; 

  double const& getBendAngle() const; // bend angle

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  const char* Type() const;

  bool isMagnet()     const;
  bool isThin()       const;
  bool isPassive()    const;
  bool isDriftSpace() const;

  double OrbitLength( Particle const& );
    // Computes arclength of orbit assuming a symmetric bend.
    // WARNING: This is not the true arclength under all conditions.

  std::pair<ElmPtr,ElmPtr> split( double const& pct) const;

 protected:

  void propagateReference( Particle& p, double initialBRho, bool scaling);

 private:

  double   angle_;
  double   usFaceAngle_;
  double   dsFaceAngle_;
  double   usAngle_;
  double   dsAngle_;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
  
};

class rbend::rbend_core_access {

 public:

  static double&                    get_usAngle( rbend& o)      { return o.usAngle_;     } 
  static double&                    get_dsAngle( rbend& o)      { return o.dsAngle_;     }
  static double&                    get_usFaceAngle( rbend& o ) { return o.usFaceAngle_; }
  static double&                    get_dsFaceAngle( rbend& o ) { return o.dsFaceAngle_; }

};

#endif // RBEND_H
