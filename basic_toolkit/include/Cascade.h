/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Cascade.h
******  Version:   1.0                    
******                                                                
******  Copyright (c) 2004 Universities Research Association, Inc.
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

    void _finishConstructor();
};


class Cascade
{
  public:
    Cascade( int w, int n, bool v = false );
    // Arguments: w = maximum weight of possible IntArrays
    //            n = number of components in the IntArrays
    //            v = "verbosity." If true, the constructor
    //                will write to std::cout as it progresses.
    // Restrictions: w >= 0
    //               n >= 1
    ~Cascade();

    int index( const IntArray& );
    IntArray exponents( const IntArray& );
    // This is just for diagnostic purposes.
    //   The returned value should be the same as
    //   the input argument.

  private:
    const int  _maxWeight;
    const int  _numVar;
    int        _numberOfMonomials;
    int        _numberOfSwitches;
    bool       _verbose;
    Switch*    _arrayOfSwitches;
    Switch**   _startPoint;
};
