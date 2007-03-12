/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
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
**************************************************************************
*************************************************************************/
#ifndef LAMBERTSON_H
#define LAMBERTSON_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;

class    thinLamb;
typedef  boost::shared_ptr<thinLamb>            ThinLambPtr;
typedef  boost::shared_ptr<thinLamb const> ConstThinLambPtr;

class DLLEXPORT thinLamb : public bmlnElmnt {

public:
  
  // x position of the septum
  // extracted beamline pointer
  // s  reference state for extraction beamline

  thinLamb();
  thinLamb( char const* name);      
  thinLamb( char const*  name, double const& septum_pos_x,  BmlPtr& b,  double* state );   
  
  thinLamb(                    double const& septum_pos_x,  BmlPtr& b,  double* state );   
  
  thinLamb( thinLamb const& );

  thinLamb* Clone() const { return new thinLamb( *this ); }

  virtual ~thinLamb();
  
  void setSeptum( double const& x); 
  void setBeamline( BmlPtr& b); 
  void setRefState( const double* s);

  void getRefState( double* );
  
  void localPropagate( ParticleBunch& x );
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );
  
  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;

private:


  double    xSeptum_;       // position of Lambertson septum in meters.
  BmlPtr    ExtBeamline_;   // pointer to the beamline of extracted particles.
  double    RefState_[6];   // A particle in the "field" region
                            // with phase space state of RefState()
                            // will be on the reference orbit of the
                            // extraction line.  The specified coordinates are 
                            // SUBTRACTED from the particle coordinates.
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

};
 

#endif // LAMBERTSON_H
