/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      Jet__environment.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved                             
******  
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
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
******  Revision History:
******                                                               
******  May 2005 -  Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******              Stub file to preserve source compatibility with 
******              code developed with the non-templated version 
******              of mxyzptlk.  
******  
*******                                                                
**************************************************************************
*************************************************************************/

#ifndef JET__ENV_H
#define JET__ENV_H

#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/EnvPtr.h>

typedef TJetEnvironment<double>                Jet__environment;
typedef TJetEnvironment<std::complex<double> > JetC__environment;

typedef EnvPtr<double>                         Jet__environment_ptr;
typedef EnvPtr<std::complex<double> >          JetC__environment_ptr;


#endif
