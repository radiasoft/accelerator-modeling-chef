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
**************************************************************************
*************************************************************************/

#ifndef CASCADE_H
#define CASCADE_H
 
class IntArray;
class Cascade;

class Switch
{
  public:
    Switch();
    Switch( int, int, const IntArray& );
    Switch( const Switch& );
    void reconstruct( int, int, const IntArray& );
    void reconstruct( const Switch& );
    ~Switch();

    friend class Cascade;

  private:
    int      _w;
    int      _index;
    IntArray _xpt;
    // bool     _terminator;
    Switch** _arrow;

    void _clean();
    void _finishConstructor();
};


class Cascade
{
  public:
    Cascade( int w = 2, int n = 2, bool v = false );
    // Arguments: w = maximum weight of possible IntArrays
    //            n = number of components in the IntArrays
    //            v = "verbosity." If true, the constructor
    //                will write to std::cout as it progresses.
    // Restrictions: w >= 0
    //               n >= 1
    Cascade( const Cascade& );
    void reconstruct( int w = 2, int n = 2, bool v = false );
    void reconstruct( const Cascade& );
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

#endif // CASCADE_H
