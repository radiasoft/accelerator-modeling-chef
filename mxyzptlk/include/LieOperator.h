/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      LieOperator.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******  REVISION HISTORY 
******                  
******  May 2005  ostiguy@fnal.gov
******            - File converted to a stub to preserve src compatibility
******              with pre-template code base.  
******                                               
******                                                                 
******                                                                
******                                                                
**************************************************************************
*************************************************************************/


#ifndef LIEOPERATOR_H
#define LIEOPERATOR_H

#include <complex>
#include <mxyzptlk/JetVector.h>
#include <mxyzptlk/TLieOperator.h>

typedef TLieOperator<double>                LieOperator;
typedef TLieOperator<std::complex<double> > CLieOperator;

#endif // LIEOPERATOR_H
