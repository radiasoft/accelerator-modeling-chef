// TODO
// Remove exit statements
// Create streamPtrs, like a Sage
// Final check: destructor
// Rename this file Cascade.cc

/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Cascade.cc
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "GenericException.h"
#include "slist.h"
#include "IntArray.h"
#include "Cascade.h" // RENAME Cascade.h

extern char nexcom( int, int, int* );  
extern "C" { int bcfRec( int, int ); }

using namespace std;

Switch::Switch()
: _w(2), _index(0), _xpt(6)// , _terminator(false)
{
  _finishConstructor();
}


Switch::Switch( int maxWeight, int indexValue, const IntArray& x )
: _w(maxWeight), 
  _index(indexValue), 
  _xpt(x) // , _terminator(false)
{
  if( _w <= 0 ) {
    cout << "Can't do this." << endl;
    exit(1);
  }
  _finishConstructor();
}


Switch::Switch( const Switch& x )
: _w(x._w), 
  _index(x._index),
  _xpt(x._xpt) // , _terminator(x._terminator)
{
  _finishConstructor();
}


void Switch::reconstruct( int maxWeight, int indexValue, const IntArray& x )
{
  this->~Switch();
  _w = maxWeight;
  _index = indexValue;
  _xpt.Reconstruct( x.Dim(), 0 );
  _xpt = x;
  // _terminator = false;
  _finishConstructor();
}


void Switch::reconstruct( const Switch& x )
{
  this->~Switch();
  _w = x._w;
  _index = x._index;
  _xpt.Reconstruct( x._xpt.Dim(), 0 );
  _xpt = x._xpt;
  // _terminator = x._terminator;
  _finishConstructor();
}


void Switch::_finishConstructor()
{
  _arrow = new Switch* [_w+1];
  for( int i = 0; i < _w+1; i++ ) { _arrow[i] = 0; }
}


Switch::~Switch()
{
  if( _arrow ) { delete [] _arrow; _arrow = 0; }
}


Cascade::Cascade( int weight, int numvar, bool verbosity ) 
: _maxWeight(weight), 
  _numVar(numvar),
  _numberOfMonomials( bcfRec( weight + numvar, numvar ) ),
  _numberOfSwitches(0),
  _verbose( verbosity ),
  _arrayOfSwitches((Switch*) 0),
  _startPoint((Switch**) 0)
{
  // Construct an array of switches
  int f[_numVar], i;
  for( i = 0; i < _numVar; i++ ) { f[i] = 0; }
  IntArray e(_numVar);
  e.Set(f);

  _numberOfSwitches = _numberOfMonomials;
  int m = _numVar;
  while( 0 < --m ) { 
    _numberOfSwitches += bcfRec( _maxWeight + m, m ); 
  }

  if( _verbose ) {
    cout << "\nNumber of switches  = " << _numberOfSwitches 
         << "\n          monomials = " << _numberOfMonomials
         << "\n          linkers   = " << (_numberOfSwitches - _numberOfMonomials)
         << endl;
  }

  _arrayOfSwitches = new Switch [_numberOfSwitches];
  Switch* upperBound = &(_arrayOfSwitches[_numberOfSwitches]);
  // for( i = 0; i < numberOfMonomials; i++ ) {
  //   setOfSwitches[i]._terminator = true;
  // }


  // Initialize first Switch
  int counter = 0;
  slist setOfSwitches;
  Switch* swPtr = 0;
  swPtr = _arrayOfSwitches;

  if( _verbose ) { cout << "Making: " << e << endl; }

  if( swPtr < upperBound ) {
    swPtr->reconstruct( _maxWeight, counter++, e );
  }
  else {
    exit(-7);
  }
  setOfSwitches.append(swPtr);
  swPtr++;
  // swPtr->_terminator = true;
  // swPtr->_arrow[0] = &swPtr;


  // Initialize remaining Switches
  for( i = 1; i <= _maxWeight; i++ ) {
    while( nexcom( i, _numVar, f) ) {
      e.Set(f);
      if( _verbose ) { cout << "Making: " << e << endl; }
      if( swPtr < upperBound ) {
        swPtr->reconstruct( _maxWeight, counter++, e );
      }
      else {
        exit(-7);
      }
      setOfSwitches.append(swPtr);
      swPtr++;
      // swPtr->_terminator = true;
      // swPtr->_arrow[0] = &swPtr;
    }
  }

  if( _numberOfMonomials 
      != ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch))  ) 
  {
    cout << "*** ERROR *** "
         << "A total of " 
         << ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch))
         << " monomials have been made; expected " 
         << _numberOfMonomials << '.'
         << endl;
    _numberOfMonomials = ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch));
  }

  if( _numberOfMonomials != counter )
  {
    cout << "*** ERROR *** "
         << "Index counter is out of synch: "
         << counter
         << " != "
         << _numberOfMonomials << '.'
         << endl;
    _numberOfMonomials = ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch));
  }

  slist targetSwitches, newSwitches;
  targetSwitches = setOfSwitches;
  int targetDim = _numVar;
  m = 0;

  while( 1 < targetDim ) {
    Switch* targetPtr;

    while((  targetPtr = (Switch*) targetSwitches.get()  )) {
      m = targetDim - 1;
      IntArray dummy(m);
      for( i = 0; i < m; i++ ) { dummy(i) = targetPtr->_xpt(i); }
      Switch* probePtr = 0;
      Switch* foundPtr = 0;
      slist_iterator sli( newSwitches );
      while((  (0 == foundPtr) && (0 != (probePtr = (Switch*) sli()))  )) {
        if( probePtr->_xpt == dummy ) {
          foundPtr = probePtr;
        }
      }

      if( 0 != foundPtr ) {  // Found
        int j = 0;
        bool foundNull = ( 0 == foundPtr->_arrow[j++] );
        if( foundNull ) {
          cout << "Whoops" << endl;
          return;
        }
        while( !foundNull && (j < _maxWeight + 1) ) {
          foundNull = ( 0 == foundPtr->_arrow[j++] );
          if( foundNull ) {
            j--;
            foundPtr->_arrow[j] = targetPtr;
            j++;
          }
        }
        if( !foundNull ) {
          cout << "Oh-oh" << endl;
          return;
        }
      }

      else {  // Not found
        if( swPtr < upperBound ) {
          swPtr->reconstruct( _maxWeight, -1, dummy );
          // if( swPtr->_terminator ) {
          //   swPtr->_arrow[0] = swPtr; ???
          // }
          // else {
          swPtr->_arrow[0] = targetPtr;
          // }
        }
        else {
          exit(-7);
        }
 
        newSwitches.append( swPtr );
        swPtr++;
      }
    }


    // Get ready for the next go-around
    targetSwitches = newSwitches;
    while((  targetPtr = (Switch*) newSwitches.get()  )) {
      setOfSwitches.append(targetPtr);
    }
    newSwitches.clear();  // Unnecessary, but what the heck ...
    targetDim--;
  }


  // Look at the results
  slist_iterator sli( setOfSwitches );
  if( _verbose ) { 
    cout << "All the switches:" << endl;
    i = 0;
    while((  swPtr = (Switch*) sli()  )) {
      cout << swPtr->_xpt << " : " << ++i << endl;
    }
    sli.Reset();
  }

  int expectedLinks = 0;
  m = _numVar;
  while( 0 < --m ) {
    expectedLinks += bcfRec(_maxWeight+m,m);
  }

  if( _verbose ) {
    cout << "Total number of nodes = " 
         << _numberOfSwitches << endl;
    cout << "           link nodes = " 
         << (_numberOfSwitches - _numberOfMonomials) 
         << "; expected "
         << expectedLinks
         << endl;
  }


  // Initialize the _startPoint array
  const int w = _maxWeight;  // Done just for convenience
  const int n = _numVar;

  _startPoint = new Switch* [w+1];
  i = 1;
  // REMOVE: sli.Reset(); // Unnecessary; already done.
  while( i <= (_numberOfSwitches - w - 1) ) { sli(); i++; }
  for( i = 0; i < w+1; i++ ) { 
    _startPoint[i] = (Switch*) sli(); 
    if( 0 == _startPoint[i] ) {
      cout << "Yike" << endl;
      exit(-5);
    }
  }
  if( 0 != sli() ) {
    cout << "Oh no" << endl;
    exit(-3);
  }
  sli.Reset();


  // Test the results
  if( _verbose ) { cout << "\nTesting accuracy." << endl; }

  // Do the first monomial by hand
  for( i = 0; i < n; i++ ) { f[i] = 0; }
  e.Set(f);
  if( _verbose ) { cout << e << endl; }
  swPtr = _startPoint[e(0)];
  if( _verbose ) { cout << "  " << swPtr->_xpt << endl; }
  for( int j = 1; j < n; j++ ) {
    swPtr = (Switch*) (swPtr->_arrow[e(j)]);
    if( _verbose ) { cout << "  " << swPtr->_xpt << endl; }
  }
  if( _verbose ) { cout << e << ": -> " << swPtr->_xpt << endl; }

  if( e != swPtr->_xpt ) {
    cout << "Error occurred: " 
         << e << ": -> " << swPtr->_xpt 
         << endl;
  }

  // ... and then all subsequent monomials
  for( i = 1; i <= w; i++ ) {
    while( nexcom( i, n, f) ) {
      e.Set(f);
      if( _verbose ) { cout << e << endl; }
      swPtr = _startPoint[e(0)];
      if( _verbose ) { cout << "  " << swPtr->_xpt << endl; }
      for( int j = 1; j < n; j++ ) {
        swPtr = (Switch*) (swPtr->_arrow[e(j)]);
        if( _verbose ) { cout << "  " << swPtr->_xpt << endl; }
      }
      if( _verbose ) { cout << e << ": -> " << swPtr->_xpt << endl; }
    }
  }

  if( _verbose ) { cout << "Finished testing accuracy." << endl; }
}


Cascade::~Cascade() 
{
  if( _arrayOfSwitches ) { delete [] _arrayOfSwitches; _arrayOfSwitches = 0; }
  if( _startPoint )      { delete [] _startPoint;      _startPoint      = 0; }
}


int Cascade::index( const IntArray& e )
{
  static int j;
  static Switch* swPtr;

  swPtr = _startPoint[e(0)];
  for( j = 1; j < _numVar; j++ ) {
    swPtr = (Switch*) (swPtr->_arrow[e(j)]);
  }

  return swPtr->_index;
}


IntArray Cascade::exponents( const IntArray& e )
{
  static int j;
  static Switch* swPtr;

  swPtr = _startPoint[e(0)];
  for( j = 1; j < _numVar; j++ ) {
    swPtr = (Switch*) (swPtr->_arrow[e(j)]);
  }

  return swPtr->_xpt;
}
