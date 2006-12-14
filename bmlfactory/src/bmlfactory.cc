/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                 
******                                                
******  File:      bmlfactory.cc
******  Version:   2.0
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
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
******  Authors:    Dmitri Mokhov, Oleg Krivosheev                  
******              Jean-Francois Ostiguy 
******
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
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
**************************************************************************
**************************************************************************
* Revision History: 
*
*  Version 1.5: August 2004 
*
*  ostiguy@fnal.gov: 
*
*  -Numerous **severe** memory leaks eliminated. 
*  -MAD CALL and RETURN statements now properly interpreted
*  -improved error recovery
*  -Expressions associated with MAD type parameters are now properly deleted.  
*  -Added member function allow external access list of beamline identifiers.
*  -Added member function to allow external evaluation of a variable expression. 
*  -Added member function to allow parsing from in-memory buffer
*
*  December 2006 : ostiguy@fnal.gov
*
* - introduced bmlfactory abstract interface class
* - fixed memory leak in instantiation of reverse order beamlines 
* - new, cleaned up scheme to keep track of allocated memry for beamline and 
*   bmlElmnts; Eliminated some dead code that was part of the old scheme.
*     
*****************************************************************************/

#include <string>
#include <bmlfactory/bmlfactory.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlfactory::bmlfactory(std::string fname,              const char* stringbuffer)
: fname_(fname), BRHO_(0.0)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlfactory::bmlfactory( std::string fname, double brho, const char* stringbuffer)
: fname_(fname), BRHO_(brho)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
