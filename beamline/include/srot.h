/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      srot.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
#ifndef SROT_H
#define SROT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class DLLEXPORT srot : public bmlnElmnt
{
public:
  srot();
  srot( double /* strength = rotation angle in radians */ );
  srot( const char * /* name */ );
  srot( const char * /* name */, double /* strength */ );
  srot( const srot& );
  ~srot();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSrot( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSrot( this ); }

  const char* Type() const;
  virtual bool isType( const char* c ) 
  { if ( strcmp(c, "srot") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new srot( *this ); }
};

#endif // SROT_H
