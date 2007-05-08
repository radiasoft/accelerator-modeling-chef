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
******
**************************************************************************
*************************************************************************/
#ifndef SOLENOID_H
#define SOLENOID_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class Particle;

class BmlVisitor;
class ConstBmlVisitor;
class Solenoid;

typedef boost::shared_ptr<Solenoid>        SolenoidPtr; 
typedef boost::shared_ptr<Solenoid const>  ConstSolenoidPtr; 


class DLLEXPORT Solenoid : public bmlnElmnt
{
public:
  // Constructors 
  Solenoid();
  Solenoid( double const&,  // (orbit) length [meters]
            double const&   // magnetic field [tesla]
                            // (assumed along the z-axis)
          );
  Solenoid( const  char*,   // name
            double const&,  // (orbit) length  [meters]
            double const&   // magnetic field [tesla]
          );
  Solenoid( const Solenoid& );

  Solenoid* Clone() const;

  ~Solenoid();   

  const char* Type() const;
  bool isMagnet()   const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void Split( double const&, ElmPtr& , ElmPtr& ) const;

  void localPropagate( Particle&      );
  void localPropagate( JetParticle&   );

private:

  bool inEdge_;
  bool outEdge_;

};


#endif // SOLENOID_H
