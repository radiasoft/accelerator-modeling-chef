/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetUtilities.h
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef JETUTIL_H
#define JETUTIL_H

extern void normalForm( const Mapping&,      /* Input:   map                    */
                        int,                 /* Input:   order                  */
                        MatrixC*,            /* Output:  matrix of eigenvectors */
                                             /*          of Jacobian of map     */
                        CLieOperator*,       /* Output:  normal form            */
                        CLieOperator* );     /* Output:  transformation         */


extern int ConvertNtoTunes( MappingC&,       /* Output: tune map                 */
                /* const */ CLieOperator& ); /* Input:  normal form vector field */


#endif  // JETUTIL_H
