/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      marker.cc
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
******
**************************************************************************
*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/marker.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class marker 
// **************************************************

marker::marker() : bmlnElmnt() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

marker::marker( const char* n ) 
: bmlnElmnt(n) 
{}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

marker::marker( marker const& x ) 
: bmlnElmnt( x ) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

marker&  marker::operator=( marker const& rhs) 
{
  if ( &rhs == this ) return *this;
  
  bmlnElmnt::operator=(rhs);

  return *this;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

marker::~marker() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* marker::Type() const 
{ 
  return "marker"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void marker::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void marker::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}
