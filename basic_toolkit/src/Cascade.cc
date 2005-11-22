// TODO
// Remove exit statements
// Create streamPtrs, like a Sage
// Final check: destructor

/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Cascade.cc
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iosetup.h>
#include <GenericException.h>
#include <slist.h>
#include <IntArray.h>
#include <Cascade.h> // RENAME Cascade.h


extern char nexcom( int, int, int* );  
extern "C" { int bcfRec( int, int ); }

using FNAL::pcerr;
using FNAL::pcout;

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
    (*pcout) << "Can't do this." << endl;
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
  _clean();
  // REMOVE: this->~Switch();
  _w = maxWeight;
  _index = indexValue;
  _xpt.Reconstruct( x.Dim(), 0 );
  _xpt = x;
  // _terminator = false;
  _finishConstructor();
}


void Switch::reconstruct( const Switch& x )
{
  _clean();
  // REMOVE:   this->~Switch();
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
  _clean();
}


void Switch::_clean()
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
  _finishConstructor();
}


Cascade::Cascade( const Cascade& x )
: _maxWeight(x._maxWeight), 
  _numVar(x._numVar),
  _numberOfMonomials( bcfRec( x._maxWeight + x._numVar, x._numVar ) ),
  _numberOfSwitches(0),
  _verbose( x._verbose ),
  _arrayOfSwitches((Switch*) 0),
  _startPoint((Switch**) 0)
{
  _finishConstructor();
}


void Cascade::reconstruct( int weight, int numvar, bool verbosity )
{
  _clean();
  // REMOVE:   this->~Cascade();

  _maxWeight         = weight;
  _numVar            = numvar;
  _numberOfMonomials = bcfRec( weight + numvar, numvar );
  _numberOfSwitches  = 0;
  _verbose           = verbosity;

  _finishConstructor();
}


void Cascade::reconstruct( const Cascade& x )
{
  _clean();
  // REMOVE:   this->~Cascade();

  _maxWeight         = x._maxWeight;
  _numVar            = x._numVar;
  _numberOfMonomials = bcfRec( _maxWeight + _numVar, _numVar );
  _numberOfSwitches  = 0;
  _verbose           = x._verbose;

  _finishConstructor();
}


void Cascade::_finishConstructor()
{
  // Construct an array of switches
  int f[_numVar];
  for( int i = 0; i < _numVar; i++ ) { f[i] = 0; }
  IntArray e(_numVar);
  e.Set(f);

  _numberOfSwitches = _numberOfMonomials;
  int m = _numVar;
  while( 0 < --m ) { 
    _numberOfSwitches += bcfRec( _maxWeight + m, m ); 
  }

  if( _verbose ) {
    (*pcout) << "\nNumber of switches  = " << _numberOfSwitches 
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

  if( _verbose ) { (*pcout) << "Making: " << e << endl; }

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
  for( int i = 1; i <= _maxWeight; i++ ) {
    while( nexcom( i, _numVar, f) ) {
      e.Set(f);
      if( _verbose ) { (*pcout) << "Making: " << e << endl; }
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
    (*pcout) << "*** ERROR *** "
         << "A total of " 
         << ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch))
         << " monomials have been made; expected " 
         << _numberOfMonomials << '.'
         << endl;
    _numberOfMonomials = ((((int) swPtr) - ((int) _arrayOfSwitches))/sizeof(Switch));
  }

  if( _numberOfMonomials != counter )
  {
    (*pcout) << "*** ERROR *** "
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

      for( int i = 0; i < m; i++ ) { dummy(i) = targetPtr->_xpt(i); }

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
          (*pcout) << "Whoops" << endl;
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
          (*pcout) << "Oh-oh" << endl;
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
    (*pcout) << "All the switches:" << endl;
    int i = 0;
    while((  swPtr = (Switch*) sli()  )) {
      (*pcout) << swPtr->_xpt << " : " << ++i << endl;
    }
    sli.Reset();
  }

  int expectedLinks = 0;
  m = _numVar;
  while( 0 < --m ) {
    expectedLinks += bcfRec(_maxWeight+m,m);
  }

  if( _verbose ) {
    (*pcout) << "Total number of nodes = " 
         << _numberOfSwitches << endl;
    (*pcout) << "           link nodes = " 
         << (_numberOfSwitches - _numberOfMonomials) 
         << "; expected "
         << expectedLinks
         << endl;
  }


  // Initialize the _startPoint array
  const int w = _maxWeight;  // Done just for convenience
  const int n = _numVar;

  _startPoint = new Switch* [w+1];
  int i = 1;
  // REMOVE: sli.Reset(); // Unnecessary; already done.
  while( i <= (_numberOfSwitches - w - 1) ) { sli(); i++; }
  for( i = 0; i < w+1; i++ ) { 
    _startPoint[i] = (Switch*) sli(); 
    if( 0 == _startPoint[i] ) {
      (*pcout) << "Yike" << endl;
      exit(-5);
    }
  }
  if( 0 != sli() ) {
    (*pcout) << "Oh no" << endl;
    exit(-3);
  }
  sli.Reset();


  // Test the results
  if( _verbose ) { (*pcout) << "\nTesting accuracy." << endl; }

  // Do the first monomial by hand
  for( int i = 0; i < n; i++ ) { f[i] = 0; }
  e.Set(f);
  if( _verbose ) { (*pcout) << e << endl; }
  swPtr = _startPoint[e(0)];
  if( _verbose ) { (*pcout) << "  " << swPtr->_xpt << endl; }
  for( int j = 1; j < n; j++ ) {
    swPtr = (Switch*) (swPtr->_arrow[e(j)]);
    if( _verbose ) { (*pcout) << "  " << swPtr->_xpt << endl; }
  }
  if( _verbose ) { (*pcout) << e << ": -> " << swPtr->_xpt << endl; }

  if( e != swPtr->_xpt ) {
    (*pcout) << "Error occurred: " 
         << e << ": -> " << swPtr->_xpt 
         << endl;
  }

  // ... and then all subsequent monomials
  for( int i = 1; i <= w; i++ ) {
    while( nexcom( i, n, f) ) {
      e.Set(f);
      if( _verbose ) { (*pcout) << e << endl; }
      swPtr = _startPoint[e(0)];
      if( _verbose ) { (*pcout) << "  " << swPtr->_xpt << endl; }
      for( int j = 1; j < n; j++ ) {
        swPtr = (Switch*) (swPtr->_arrow[e(j)]);
        if( _verbose ) { (*pcout) << "  " << swPtr->_xpt << endl; }
      }
      if( _verbose ) { (*pcout) << e << ": -> " << swPtr->_xpt << endl; }
    }
  }

  if( _verbose ) { (*pcout) << "Finished testing accuracy." << endl; }
}


Cascade::~Cascade() 
{
  _clean();
}


void Cascade::_clean()
{
  if( _arrayOfSwitches ) { delete [] _arrayOfSwitches; _arrayOfSwitches = 0; }
  if( _startPoint )      { delete [] _startPoint;      _startPoint      = 0; }
}


int Cascade::index( const IntArray& e ) const
{

  IntArrayIterator getNext( e );

  // Switch* swPtr = _startPoint[e(0)];

  Switch* swPtr = _startPoint[ getNext() ];

  
  for( int j = 1; j < _numVar; ++j ) {

    // swPtr = (Switch*) (swPtr->_arrow[e(j)]); 

    swPtr = (Switch*) ( swPtr->_arrow[ getNext() ] ); 
  }

  return swPtr->_index;
}


int Cascade::index( const int e[] ) const
{

//  const int* idx = &e[0] - 1;
  
  Switch* swPtr = _startPoint[e[0]];
//  Switch* swPtr = _startPoint[ *(++idx) ];
  
  for( int j = 1; j < _numVar; j++ ) {
       swPtr = (Switch*) (swPtr->_arrow[e[j]]);
// swPtr = (Switch*) (swPtr->_arrow[*(++idx)]);
  }

  return swPtr->_index;
}


IntArray Cascade::exponents( const IntArray& e )
{

  IntArrayIterator getNext( e );

  //  Switch* swPtr = _startPoint[e(0)];

  Switch* swPtr = _startPoint[ getNext()];

  for( int j = 1; j < _numVar; j++ ) {
    //    swPtr = (Switch*) (swPtr->_arrow[e(j)]);
    swPtr = (Switch*) (swPtr->_arrow[ getNext() ] );
  }
  return swPtr->_xpt;
}


int Cascade::selfTest()
{
  (*pcerr) << "\nCascade::selfTest beginning test of all possible indices." 
            << std::endl;
  int ret = 0;
  int n = _numVar;
  int w = _maxWeight;
  int f[n];
  IntArray e(n);
  for( int i = 0; i < n; i++ ) { f[i] = 0; }

  e.Set(f);
  if( e != (this->exponents(e)) ) {
    (*pcerr) << "Type 1 error: " << e << " != " << (this->exponents(e)) << endl;
    ret = 1;
  }
  if( e != _arrayOfSwitches[this->index(e)]._xpt ) {
    (*pcerr) << "Type 2 error: " << e << " != " 
              << "_arrayOfSwitches["
              << (this->index(e))
              << "] = " 
              << (_arrayOfSwitches[this->index(e)]._xpt)
              << endl;
    ret = 2;
  }


  for( int i = 1; i <= w; i++ ) {
    while( nexcom( i, n, f) ) {
      e.Set(f);
      if( e != (this->exponents(e)) ) {
        (*pcerr) << "Type 1 error: " << e << " != " << (this->exponents(e)) << endl;
        ret = 1;
      }
      if( e != _arrayOfSwitches[this->index(e)]._xpt ) {
        (*pcerr) << "Type 2 error: " << e << " != " 
                  << "_arrayOfSwitches["
                  << (this->index(e))
                  << "] = " 
                  << (_arrayOfSwitches[this->index(e)]._xpt)
                  << endl;
        ret = 2;
      }
    }
  }

  (*pcerr) << "\nCascade::selfTest finished test of all possible indices." 
            << std::endl;
  return ret;
}
