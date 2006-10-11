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
**************************************************************************
*************************************************************************/


#ifndef LAMBERTSON_H
#define LAMBERTSON_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class DLLEXPORT thinLamb : public bmlnElmnt
{
private:
  double    xSeptum;       // position of Lambertson septum in meters.
  beamline* ExtBeamline;   // pointer to the beamline of extracted particles.
  double    RefState[6];   // A particle in the "field" region
                           // with phase space state of RefState()
                           // will be on the reference orbit of the
                           // extraction line.  The specified coordinates are 
                           // SUBTRACTED from the particle coordinates.
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  
  thinLamb();
  thinLamb( char* n);      // name
  
  thinLamb( char*  n,      // name
            double x,      // x position of the septum.
            beamline* b,   // extracted beamline pointer.
            double* s );   // reference state for extraction beamline.
  
  thinLamb( double x,      // x position of the septum.
            beamline* b,   // extracted beamline pointer.
            double* s );   // reference state for extraction beamline.
  
  thinLamb( const thinLamb& );
  ~thinLamb();
  
  void setSeptum( double x); 
  void setBeamline( beamline* b); 
  void setRefState( const double* s);

  void getRefState( double* );
  
  void localPropagate( ParticleBunch& x );
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );
  
  void accept( BmlVisitor& v ) { v.visitThinLamb( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinLamb( this ); }

  void eliminate();
  
  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "thinLamb") != 0 ) return bmlnElmnt::isType(c); else return true; }

  thinLamb* Clone() const { return new thinLamb( *this ); }
};
 

#endif // LAMBERTSON_H
