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


#include "marker.h"

using namespace std;

// **************************************************
//   class marker 
// **************************************************

marker::marker() : bmlnElmnt() 
{
}

marker::marker( char* n ) : bmlnElmnt(n) 
{
}

marker::marker( const marker& x ) : bmlnElmnt( (bmlnElmnt&) x ) 
{
}

marker::marker( bmlnElmntData& x ) : bmlnElmnt( x ) 
{
}

marker::~marker() {
}



const char* marker::Type() const 
{ 
  return "marker"; 
}

