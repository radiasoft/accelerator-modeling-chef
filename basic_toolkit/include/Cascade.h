/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Cascade.h
******  Version:   1.0                    
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
****** Revision History
******
****** Sep 2006     Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******
****** - eliminated archaic "reconstruct" member functions. 
******   Use placemnent new instead.                                     
******
**************************************************************************
*************************************************************************/

#ifndef CASCADE_H
#define CASCADE_H

#include <basic_toolkit/globaldefs.h>
 
class IntArray;
class Cascade;

class Switch
{
  public:
    Switch();
    Switch( int, int, const IntArray& );
    Switch( const Switch& );
    ~Switch();

    friend class Cascade;

  private:
    int      _w;
    int      _index;
    IntArray _xpt;
    Switch** _arrow;

    DLLLOCAL void _clean();
    DLLLOCAL void _finishConstructor();
};


class DLLEXPORT Cascade
{
  public:
    Cascade( int w = 2, int n = 2, bool v = false );
    // Arguments: w = maximum weight of possible IntArrays
    //            n = number of components in the IntArrays
    //            v = "verbosity." If true, the constructor
    //                will write to std::cout as it progresses.
    // Restrictions: w >= 0
    //               n >= 1
    Cascade( Cascade const& );
    ~Cascade();

    int index( const IntArray& ) const;
    // Warning: for purposes of speed, this assumes that 
    //   the argument has the correct dimension.

    int index( const int [] )    const;
    // Warning: this assumes that the argument is an array
    //   of the appropriate dimension. It is provided
    //   for backwards compatability of old code.

    IntArray exponents( const IntArray& );
    // This is just for diagnostic purposes.
    //   The returned value should be the same as
    //   the input argument.

    int selfTest();

  private:
    int        _maxWeight;
    int        _numVar;
    int        _numberOfMonomials;
    int        _numberOfSwitches;
    bool       _verbose;
    Switch*    _arrayOfSwitches;
    Switch**   _startPoint;

    void _clean();
    void _finishConstructor();
};


inline int Cascade::index( IntArray const& e ) const
{


  IntArrayIterator getNext( e );
  Switch* swPtr = _startPoint[ getNext() ];
    for( int j = 1; j < _numVar; ++j ) {
    swPtr = (Switch*) ( swPtr->_arrow[ getNext() ] ); 
  }
  return swPtr->_index;

  //----------------------------------------------
  // the code below works only with 1st order Jets
  //----------------------------------------------
#if  0
  IntArrayIterator getNext( e );
  for( int j = 0; j < _numVar; ++j ) {
    if (getNext() != 0 ) return j+1;  
  }
  return 0;
#endif
}


#endif // CASCADE_H
