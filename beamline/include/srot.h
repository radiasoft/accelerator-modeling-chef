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
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef SROT_H
#define SROT_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class srot : public bmlnElmnt
{
public:
  srot();
  srot( double /* strength = rotation angle in radians */ );
  srot( char * /* name */ );
  srot( char * /* name */, double /* strength */ );
  srot( const srot& );
  ~srot();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSrot( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSrot( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "srot") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new srot( *this ); }
};

#endif // SROT_H
