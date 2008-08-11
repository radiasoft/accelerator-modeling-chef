/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  File: NormalForm.h
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
******  August 2008  ostiguy@fnal.gov
******  - this header file was moved from JetUtils.h 
******  - changed function signatures to use references and 
******    standard containers.  
******                                                              
**************************************************************************
*************************************************************************/

#ifndef NORMALFORM_H
#define NORMALFORM_H

#include <complex>
#include <vector>
#include <basic_toolkit/MatrixFwd.h>
#include <mxyzptlk/Mapping.h>
#include <mxyzptlk/LieOperator.h>
 
void normalForm( Mapping const& map, int order, MatrixC& eigenv, std::vector<CLieOperator>& normalform, 
                                                                 std::vector<CLieOperator>& transformation );

int ConvertNtoTunes( MappingC& tunemap, CLieOperator const& ); 

#endif  // NORMALFORM_H 

