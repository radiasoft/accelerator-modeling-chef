/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      MXBBML_bool.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
**************************************************************************
*************************************************************************/


#ifndef MXBBMLBOOL_H
#define MXBBMLBOOL_H

// The following code is for the Solaris native compiler.  It does not
// have any boolean basic data type.  Note that this does *not* make
// bool a basic type, but it does make some code more readable
// -- Elliott McCrory


// #if defined(__SUNPRO_CC)
#if defined(__SUNPRO_CC)
typedef enum { false = 0, true = 1 } bool;
#endif // __SUNPRO_CC

#endif // MXBBMLBOOL_H
