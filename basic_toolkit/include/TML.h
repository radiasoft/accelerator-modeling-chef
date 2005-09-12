/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TML.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.
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

#ifndef TML_H
#define TML_H

#include <iostream>
#include <complex>
#include <math.h>

#include <complexAddon.h>

template<typename T> 
struct TML
{
  T** _m;
  int _r, _c;  // number of rows and columns
  int _rc;     // reference count

  void _clear();

  // Constructors and destructors_____________________________________
  TML();
  TML(const TML<T>&);
  TML(int rows ,int columns,        T  initval);
  TML(int rows, int columns,  const T* initval);
  ~TML();

  // Public member functions__________________________________________

 TML<T>& operator=(const TML<T>&);
 TML<T>& operator+=(const T&);
 TML<T>& operator-=(const T&);

 bool operator==(const TML<T>&);
 bool operator==(const T&);
};

#endif //TML_H
