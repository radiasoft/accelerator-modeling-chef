/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      MLJ.h
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


#ifndef MLJ_H
#define MLJ_H


#include <math.h>
#ifdef __VISUAL_CPP__
#include <iostream>
#include <complex>
#else
#include <iostream>
#include <complex>
#endif

#include "complexAddon.h"

#include "Jet.h"

///////////////////////////////////////////////////////////////////////
struct MLJ {

  Jet **m;
  int r,c;			// number of rows and columns
  int rc;			// reference count

  // Private member functions_________________________________________
  void clear();
  void error(char * msgl, char * msg2 = " ")const ;

  // Constructors and destructors_____________________________________
  MLJ();
  MLJ(const MLJ&);
  MLJ(int rows ,int columns , const Jet& initval);
  MLJ(int rows, int columns, Jet* initval);
  ~MLJ();

  // Public member functions__________________________________________ 
 MLJ& operator=(const MLJ&);
 char operator==(const Jet&);
 MLJ& operator+=(const Jet&);
 MLJ& operator-=(const Jet&);
friend  char operator==(const MLJ&,const MLJ&);

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};


//////////////////////////////////////////////////////////////////////////

#endif
