/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      JetUtilities.h
******                                                                
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
******                                                                
**************************************************************************
*************************************************************************/


#ifndef JETUTIL_H
#define JETUTIL_H

#include <complex>
#include <Mapping.h>
#include <LieOperator.h>
 
extern void normalForm( const Mapping&,                            /* Input:   map                    */
                        int,                                       /* Input:   order                  */
                        MatrixD*,                                  /* Output:  matrix of eigenvectors */
                                                                   /*          of Jacobian of map     */
                        LieOperator*,                              /* Output:  normal form            */
                        LieOperatorC*);                            /* Output:  transformation         */


extern int ConvertNtoTunes( MappingC&,        /* Output: tune map                 */
                /* const */ LieOperator& );   /* Input:  normal form vector field */


#endif  // JETUTIL_H
