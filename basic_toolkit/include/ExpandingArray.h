/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.2
******                                    
******  File:      ExpandingArray.h
******                                                                
******  Copyright (c) 2002 Universities Research Association, Inc.    
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

// 
// Utility class ExpandingArray
// 

#ifndef EXPANDINGARRAY_H
#define EXPANDINGARRAY_H

class ExpandingArray
{
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
