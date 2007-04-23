/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      srot.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced srd file coupling due to visit() member. 
******   visit() takes advantage of (reference) dynamic type
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <beamline/srot.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class srot
// **************************************************

srot::srot() : bmlnElmnt() {
}

srot::srot( double const& s) : bmlnElmnt(0.0,s) {
}

srot::srot( const char* n ) : bmlnElmnt(n) {
}

srot::srot( const char* n, double const& s) : bmlnElmnt( n, 0.0,s) {
}

srot::srot( const srot& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
}

srot::~srot() {
}


const char* srot::Type() const 
{ 
  return "srot"; 
}


bool   srot::isMagnet() const
{
  return false; 
}


void srot::accept( BmlVisitor& v ) 
{
  v.visit(*this);
}


void srot::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}
