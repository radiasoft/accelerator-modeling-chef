/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      JetCVector.h
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
****** Revision History:
****** 
****** May 2005 -  Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******             Stub file to preserve source compatibility with 
******             code developed with the non-templated version 
******             of mxyzptlk.  
******  
****** ******************************************************************
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
*******                                                                
**************************************************************************
*************************************************************************/

#ifndef JETCVECTOR_H
#define JETCVECTOR_H

#include "Jet.h"
#include "TJetVector.tcc"

typedef TJetVector<double,FNAL::Complex> JetVector;
typedef TJetVector<FNAL::Complex,double> JetCVector;

#endif // JETCVECTOR_H
