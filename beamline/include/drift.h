/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      drift.h
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


#ifndef DRIFT_H
#define DRIFT_H

#include "bmlnElmnt.h"

class drift : public bmlnElmnt
{
public:
  drift();
  drift( double );             // length of drift in meters
  drift( char* );              // name
  drift( char*,                // name
         double );             // length of drift in meters
  drift( bmlnElmntData& );
  drift( const drift& );
  ~drift();

  void accept( BmlVisitor& v ) { v.visitDrift( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitDrift( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "drift") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new drift( *this ); }
} ;

#endif // DRIFT_H
