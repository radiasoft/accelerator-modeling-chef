/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      LieOperator.h
******  Version:   5.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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


#ifndef LIEOPERATOR_H
#define LIEOPERATOR_H


#include <Jet.h>
#include <JetVector.h>
#include <TLieOperator.h>

typedef TLieOperator<double,FNAL::Complex> LieOperator;
typedef TLieOperator<FNAL::Complex,double> CLieOperator;

#endif // LIEOPERATOR_H
