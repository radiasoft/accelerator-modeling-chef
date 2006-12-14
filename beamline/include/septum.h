/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      septum.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
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

#ifndef SEPTUM_H
#define SEPTUM_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>


class DLLEXPORT thinSeptum : public bmlnElmnt
{
private:
  double strengthPos;    // kick in strength in radians for x > xWire
  double strengthNeg;	 // kick in strength in radians for x < xWire
  double xWire;		 // position of wire septum in meters
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  
  thinSeptum();
  
  thinSeptum( char const*  name );
  
  thinSeptum( char const*  name,
	    double const& sPos,    // kick in strength in radians for x > xWire 
	    double const& sNeg,    // kick in strength in radians for x < xWire 
	    double const& x );     // position of wire septum in meters
   
  thinSeptum( double const& sPos,    
	    double const& sNeg,    
	    double const& x ); 
  
  thinSeptum( const thinSeptum& );

  thinSeptum* Clone() const { return new thinSeptum( *this ); }

  virtual ~thinSeptum();
  
  void setStrengths( double const& sPos, double const& sNeg); 
  void setWire( double const& x); 
  
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );

  void accept( BmlVisitor& v ) { v.visitThinSeptum( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinSeptum( this ); }

  const char* Type() const;


};
 
#endif // SEPTUM_H
