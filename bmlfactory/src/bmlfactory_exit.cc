/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                 
******                                                
******  Version:   1.0
******                                    
******  File:      bmlfactory_exit.cc
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
******  Usage, modification, and redistribution are subject to terms    
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

// This is a single special purpose file 
// to enable a primitive C++ exception handling,
// using an exception class that inherits from std::exception,
// in the MAD parser C files.

#include "GenericException.h"

extern "C" {

void bmlfactory_exit()
{
  throw( GenericException( __FILE__, __LINE__, 
         "void bmlfactory_exit()", 
         "Exception thrown by a madparser file. Cannot recover." ) );
}

}
