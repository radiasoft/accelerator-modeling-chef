/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Solenoid.h
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
****** REVISION HISTORY:
*****                                                               
****** Apr 2007 ostiguy@fnal.gov
******
******  - support for reference counted elements
******  - eliminated unecessary casts
******  - use std::string for renaming
******  - changes to header file to reduce file coupling 
****** Dec 2007 ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/

#ifndef SOLENOID_H
#define SOLENOID_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlnElmnt.h>
#include <beamline/ParticleFwd.h>


class BmlVisitor;
class ConstBmlVisitor;
class Solenoid;

typedef boost::shared_ptr<Solenoid>        SolenoidPtr; 
typedef boost::shared_ptr<Solenoid const>  ConstSolenoidPtr; 


class DLLEXPORT Solenoid : public BmlnElmnt {

  class Propagator; 

public:

  Solenoid();
  Solenoid( const  char*,   // name
            double const&,  // (orbit) length  [meters]
            double const&   // magnetic field [tesla]
          );

  Solenoid( Solenoid const& );

  Solenoid* clone() const;

  ~Solenoid();   

  Solenoid& operator=( Solenoid const& );

  const char* Type()    const;

  bool isMagnet()       const;
  bool isThin()         const;
  bool isPassive()      const;
  bool isDriftSpace()   const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  std::pair<ElmPtr,ElmPtr> split( double const& pct) const;

  bool hasInEdge()   const;
  bool hasOutEdge()  const;

private:

  bool          inEdge_;
  bool          outEdge_;

};


#endif // SOLENOID_H
