/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      marker.h
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


#ifndef MARKER_H
#define MARKER_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class marker : public bmlnElmnt
{
public:
  marker();                   // Data to be written to standard output
  marker( const char* );      // Name identifier.
  marker( const marker& );
  marker( bmlnElmntData& );
  ~marker();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMarker( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMarker( this ); }

  const char* Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "marker") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new marker( *this ); }
} ;

#endif // MARKER_H
