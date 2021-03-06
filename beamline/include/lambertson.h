/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      lambertson.h
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
****** Mar 2007    ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007    ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/
#ifndef LAMBERTSON_H
#define LAMBERTSON_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlnElmnt.h>
#include <vector>

class BmlVisitor;
class ConstBmlVisitor;

class    thinLamb;
typedef  boost::shared_ptr<thinLamb>            ThinLambPtr;
typedef  boost::shared_ptr<thinLamb const> ConstThinLambPtr;

class DLLEXPORT thinLamb : public BmlnElmnt {

  class Propagator;

public:

  // x position of the septum
  // extracted beamline pointer
  // s  reference state for extraction beamline

  thinLamb();
  thinLamb( std::string const&  name);      
  thinLamb( std::string const&  name, double const& septum_pos_x,  BmlPtr& b,  double* state );   
  
  thinLamb(                    double const& septum_pos_x,  BmlPtr& b,  double* state );   
  
  thinLamb( thinLamb const& );

  thinLamb* clone() const { return new thinLamb( *this ); }

 ~thinLamb();
  
  thinLamb& operator=( thinLamb const&);

  void setSeptum( double const& x); 
  void setBeamline( BmlPtr& b); 

  void                       setRefState( std::vector<double> const& );
  std::vector<double> const& getRefState() const;
  
  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  bool        isMagnet()     const;
  bool        isThin()       const;
  bool        isPassive()    const;
  bool        isDriftSpace() const;

  const char* Type()     const;

private:


  double              xSeptum_;       // position of Lambertson septum in meters.
  BmlPtr              ExtBeamline_;   // pointer to the beamline of extracted particles.
  std::vector<double> RefState_;      // A particle in the "field" region
                                      // with phase space state of RefState()
                                      // will be on the reference orbit of the
                                      // extraction line.  The specified coordinates are 
                                      // SUBTRACTED from the particle coordinates.
  
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

};
 

#endif // LAMBERTSON_H
