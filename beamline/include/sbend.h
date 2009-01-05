/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sbend.h
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
******
****** Dec 2008 ostiguy@fnal
****** - restored basic support for MAD-style propagators.
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no implicit assumption about internal structure   
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder setLength method
****** - added setStrength method
******   : not needed in earlier implementations because
******     sbend had no internal structure then.
****** - added member functions to nullify edge effects
******   : used by sbend::Split
****** Dec 2007            ostiguy@fnal.gov
****** - eliminated obsolete support for "fast" arcsin 
****** - new typesafe propagator scheme
****** - sbend now implemented as a composite element  
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

  class Propagator;
  class MADPropagator;

 public:
 
  class sbend_core_access;

  sbend();

  sbend( std::string const& name, double const& length, 
	                          double const& strength,
                                  double const& bendangle );

  sbend( std::string const& name,
         double const& length,
         double const& strength,    double const& bendangle,
         double const& usfaceangle, double const& dsfaceangle );

  sbend( sbend const& );

  sbend* Clone() const { return new sbend( *this ); }

 ~sbend();

  bool hasParallelFaces() const;
  bool hasStandardFaces() const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  char const* Type()  const;
  bool isDriftSpace() const;
  bool isMagnet()     const;
  bool isThin()       const;
  bool isPassive()    const;

  std::pair<ElmPtr,ElmPtr> split( double const& pct) const;

  double        setBendAngle(double const& a) { return (angle_ = a); }
  double const& getBendAngle() const          { return angle_; }
  
  //------------------------------------------------------------------
  // Note: A symmetric bend is assumed by default. 
  //       Fiducial trajectory entry and exit angles are assumed to 
  //       be zero. If desired, they can be set to using one of the 
  //       member functions below. 
  //------------------------------------------------------------------

  double setEntryAngle( Particle const& ); 
  double  setExitAngle( Particle const& ); 

  double setEntryAngle( double const& radians); 
  double  setExitAngle( double const& radians); 

  double getEntryAngle()  const;
  double getExitAngle()   const; 

  double getEntryFaceAngle()   const; 
  double  getExitFaceAngle()   const; 

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

class sbend::sbend_core_access {

 public:

  static double&                    get_angle( sbend& o )       { return o.angle_;       }
  static double&                    get_usAngle( sbend& o)      { return o.usAngle_;     } 
  static double&                    get_dsAngle( sbend& o)      { return o.dsAngle_;     }
  static double&                    get_usFaceAngle( sbend& o)  { return o.usFaceAngle_; }
  static double&                    get_dsFaceAngle( sbend& o)  { return o.dsFaceAngle_; }

};
   
#endif // SBEND_H
