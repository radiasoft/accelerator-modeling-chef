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


#ifndef DRIFT_H
#define DRIFT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class DLLEXPORT drift : public bmlnElmnt
{
public:
  drift();
  drift( double );             // length of drift in meters
  drift( const char* );        // name
  drift( const char*,          // name
         double );             // length of drift in meters
  drift( bmlnElmntData& );
  drift( const drift& );
  ~drift();

  void accept( BmlVisitor& v ) { v.visitDrift( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitDrift( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "drift") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new drift( *this ); }
} ;

#endif // DRIFT_H
