/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      mxyzptlk.h
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


#ifndef MXYZPTLK_H
#define MXYZPTLK_H

class JetTerm;
class Jet;
class JetCterm;
class JetC;
class LieOperator;
class CLieOperator;


#include <math.h>
#ifdef __VISUAL_CPP__
#include <iostream>
#include <complex>
#else
#include <stdlib.h>
#include <stdio.h>
#include <complex>
#include <iostream>
#endif
// ??? Should this line be added??? #include <iostream.h>
// ??? Should this line be added??? #include <fstream.h>
// ??? Should this line be added??? #include <iomanip.h>

#include "dlist.h"
#include "complexAddon.h"
#include "TMatrix.h"
#include "JL.h"
#include "Jet.h"
#include "LieOperator.h"
#include "JLC.h"
#include "JetC.h"
#include "CLieOperator.h"
#include "Mapping.h"
#include "MappingC.h"

#endif // MXYZPTLK_H
