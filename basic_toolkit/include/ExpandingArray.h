/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.2
******                                    
******  File:      ExpandingArray.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab    
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
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                       
******                                                                
**************************************************************************
*************************************************************************/

// 
// Utility class ExpandingArray
// 

#ifndef EXPANDINGARRAY_H
#define EXPANDINGARRAY_H

#include <basic_toolkit/globaldefs.h>

class DLLEXPORT ExpandingArray{

public:
  ExpandingArray();
  ~ExpandingArray();

  double& operator[]( int );
  int size();

private:
  static const int _blockSize;
  int _maxIndex;
  double* _x;
};

#endif // EXPANDINGARRAY_H
