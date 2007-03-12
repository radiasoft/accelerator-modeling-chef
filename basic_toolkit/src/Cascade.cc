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
****** Revision History
******
****** Sep 2006     Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******
****** - eliminated archaic "reconstruct" member functions. 
******   Use placement new instead.
****** 
****** Feb 2007     ostiguy@fnal.gov
******
****** - use std::list (instead of c-style slist)
****** - wrote paragraph to document the purpose of the class, the data layout
******   and the initialization process. Added detailed comments. 
****** - refactored and simplified main loop: use partial comparison operators 
******   for IntArray.    
******                                    
********************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>
#include <functional>
#include <boost/bind.hpp>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/Cascade.h> // RENAME Cascade.h


using FNAL::pcerr;
using FNAL::pcout;

using namespace std;

//------------------------------------------------------------------------------------
//
// The class Cascade implements a network of "Switches" allowing 
// the of mapping n-tuples of monomial exponents to offsets within a table of monomial
// coefficients. Assuming n variables, the cost of computing an offset is 
// roughly that of n additions (computing ptr offset) + n indirections. 
// 
// Within the monomial coefficient table, coefficients are layed out in the following order
// 
// (1) increasing weight ( sum of the exponents or total order ) 
// (2) coefficients of identical weight are ordered in inverse lexicographical 
//     order of exponent tuple.
//
//  Let w be the maximum weight w and n be the no of variables.
//
//
//  There are then C(w+n, n) = ( w+n )!/ n! w!  distinct monomials  
//
//
//  e.g. w=1, n=6  C(w,n) = (1+6)!/ 1!6!  = 7 monomials
//
//
//  Now, consider (for example) a 4-tuple (e0, e1, e2, e3 )
//  at order w=3
//
//  The Cascade structure is layed out as follows :
//
//  an array arrayOfSwitches with dimension set to Sum(i=1,w) C(w+i,w)  
//  e.g. for order w=3  
//
//  size = C(3+4,4)  + C(3+3,3)  + C (3+2,2)  + C( 3+1,1)    
//  size = 7!/4!3!)  + 6!/(3!3!) + 5!/(2!3!)  + 4!/(1!3!)    
//         Group 1     Group 2      Group3      Group4
//  
//  The groups and their elements are layed out sequentially in memory,
//  in the above order.  
//  
//  Each Switch has an arrow_ member which is an array of 
//  pointers to other Switch objects.
//
//  a second array, startPoint_ is also defined. In this example, each entry in 
//  startPoint would point to an entry in Group 4, defined by the value of e0.
//  The dimension of startPoint_ is therefore equal to the max weight + 1.
//     
//  within Group 4, each entry points into an entry in Group 3. This entry is
//  defined by e1 such that arrow_[e1]  points to switch (e0,e1)  
//
//  within Group 3,  each entry points into an entry in Group 2. This entry
//  is defined e2  such that  arrow_[e2] points to   switch (e0,e1,e2)  
// 
//  etc ...
//
//  To obtain the offset that corresponds to e0,e1,2  one needs only to follow
//  the links (hence the name "Cascade")
// 
//  startPoint[e0] ->arrow_[e1]->arrow_[e2]->arrow_[e3]
//
//  Initialization proceeds as follow: Group1 is collected
//  into a list of "target" switches. Since these switches are "terminal", 
//  their arrow_ member array is empty.
//  Then, new switches are added to Group 2: a new switch is created with 
//  n-1 exponents; the target switch list is then traversed sequentially and 
//  the first n-1 exponents are compared. for every match, the new switch arrow_ 
//  array corresponding to the exponent that was omitted is initialized to the
//  target. This is repeated until the target group is exhausted. 
//     
//  The target switches become group 2 and group 3 are the new switches. 
//  The process is repeated until the entire array is filled.
//
//-----------------------------------------------------------------------------------

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Switch::Switch()
: w_(2), index_(0), xpt_( IntArray(6) )// , terminator(false)
{
   finishConstructor();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Switch::Switch( int maxWeight, int indexValue, IntArray const& x )
: w_(maxWeight), index_(indexValue), xpt_(x) // , terminator_(false)
{
  if( w_ <= 0 ) {
    (*pcout) << "Can't do this." << endl;
    exit(1);
  }
   finishConstructor();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Switch::Switch( Switch const& x )
:  w_(x.w_), index_( x.index_ ), xpt_( x.xpt_ ) 
{
  arrow_ = new Switch* [w_ + 1 ];
  std::copy( &x.arrow_[0], &x.arrow_[w_+1], &arrow_[0] );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Switch::finishConstructor()
{
  arrow_ = new Switch* [w_ + 1 ];
  for( int i=0; i < w_+1; ++i) { arrow_[i] = 0; }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Switch::~Switch()
{
  if( arrow_ ) { delete [] arrow_;  arrow_ = 0; }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Cascade::Cascade( int weight, int numvar, bool verbosity ) 
:  maxWeight_(weight), 
   numVar_(numvar),
   numberOfMonomials_( bcfRec( weight + numvar, numvar ) ),
   numberOfSwitches_(0),
   verbose_( verbosity ),
   arrayOfSwitches_(),
   startPoint_(0)
{
   finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Cascade::Cascade( Cascade const& x )
:  maxWeight_(x.maxWeight_), 
   numVar_(x.numVar_),
   numberOfMonomials_( bcfRec( x.maxWeight_ + x.numVar_, x.numVar_ ) ),
   numberOfSwitches_(0),
   verbose_( x.verbose_ ),
   arrayOfSwitches_(),
   startPoint_(0)
{
   finishConstructor();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Cascade::finishConstructor()
{

  // Construct an array of switches

  numberOfSwitches_ = numberOfMonomials_;

  int m = numVar_;

  while( --m > 0 ) { 
     numberOfSwitches_ += bcfRec( maxWeight_ + m, m ); 
  }

  if( verbose_ ) {
    (*pcout) << "\nNumber of switches  = " <<  numberOfSwitches_ 
             << "\n          monomials = " <<  numberOfMonomials_
             << "\n          linkers   = " << (numberOfSwitches_ - numberOfMonomials_)
             << endl;
  }

  arrayOfSwitches_   = std::vector<Switch>(numberOfSwitches_);

  Switch* const upperBound = &arrayOfSwitches_.back() + 1; //  pointer to the top of array memory


  // Initialize the first Switch ( 0, 0, 0, .... 0)
 
  int counter = 0;

  std::list<Switch*>  setOfSwitches;

  IntArray e(numVar_);

  if( verbose_ ) { (*pcout) << "Making: " << e << endl; }

  Switch* swPtr = &arrayOfSwitches_[0];

  new (swPtr) Switch( maxWeight_, counter++, e );  // this sets the unique weight=0 exponent tuple.

  setOfSwitches.push_back(swPtr);

  ++swPtr;

  // -----------------------------------------------------------------------------------
  // the code below generates a list of switches with exponent tuples in the following order:
  // (1) increasing weight 
  // (2) exponents of the same weight are ordered in inverse lexicographical order.
  //------------------------------------------------------------------------------------

  // Initialize remaining Switches

  int f[numVar_];
  std::fill( &f[0],&f[0]+ numVar_, int() );

  for( int i=1; i <= maxWeight_; ++i) {

    while( nexcom( i, numVar_, f) ) { 
      e.Set(f);
      if( verbose_ ) { (*pcout) << "Making: " << e << endl; }
      if( swPtr < upperBound ) {
        new (swPtr) Switch(  maxWeight_, counter++, e );
      }
      else {
        exit(-7);
      }
      setOfSwitches.push_back(swPtr);
      ++swPtr;
    }
  }

  if( numberOfMonomials_ != counter )
  {
    (*pcout) << "*** ERROR *** "
         << "Index counter is out of synch: "
         << counter
         << " != "
         << numberOfMonomials_ << '.'
         << endl;
    numberOfMonomials_ = arrayOfSwitches_.size();
  }

  // ----------------------------------------------------------------------------------------------------------
  // At this point, arrayofSwitches contains a switch for every possible monomial, in the order specified above.
  // These switches are the "final" switches in the chain because they fully define all exponents.
  // 
  // We now proceed to define the "linking" or intermediate Switches.      
  //
  // setOfSwitches  = the complete set of switches defined so far
  // targetSwitches = the target switches for the current interation
  // newSwitches    = the "source" switches for the current iteration
  //     
  //-----------------------------------------------------------------------------------------------------------

  list<Switch*> targetSwitches = setOfSwitches; 
  list<Switch*> newSwitches;

  int targetDim = numVar_;
  m = 0;

  // --------------------------
  // ** main loop
  // -------------------------

  Switch* targetPtr = 0 ;
  Switch* newPtr    = 0 ;

  while( targetDim > 1 ) { 

    while(  !targetSwitches.empty() ) {
      
      // sort the target switches  ignoring the rightmost index. This garantees that all switches
      // that have identical indices from 0 to targetDim-1 inclusively appear in succession in the list. 

      targetSwitches.sort( Switch::PartialLessThan(0,targetDim-2) );
      
      for ( std::list<Switch*>::iterator it = targetSwitches.begin(); it != targetSwitches.end(); ++it )
 
      // get a target switch 
      targetPtr =  targetSwitches.front();
      targetSwitches.pop_front();
 
      // create a new switch pointer in arrayOfSwitches based on the new target (note placement new syntax !)
      // note that the new switch pointer has one less indices than the target.
  
      newPtr = new (swPtr++) Switch( maxWeight_, -1, IntArray( targetPtr->xpt_.begin(), targetPtr->xpt_.begin()+ targetDim-1 ) );
      newSwitches.push_back( newPtr );

      newPtr->arrow_[targetPtr->xpt_(targetDim-1)] =  targetPtr;   

      // Continue poping out target switches and set arrow_ pointer array entries in new switch until 
      // target switches no longer match( EqualUpTo )
 
      if ( targetSwitches.empty() )  break;
      targetPtr = targetSwitches.front(); 

      while(  IntArray::PartialEqual( newPtr->xpt_, targetPtr->xpt_, 0, targetDim-2 )  )
      {
         targetSwitches.pop_front(); 
         newPtr->arrow_[ targetPtr->xpt_(targetDim-1) ] =  targetPtr;   
         targetPtr =  targetSwitches.front();

      }
    } // while 

    // Get ready for the next go-around

    targetSwitches = newSwitches;

    for  ( std::list<Switch*>::iterator it = newSwitches.begin(); it != newSwitches.end(); ++it ) {
      setOfSwitches.push_back(*it);
    }

    newSwitches.clear();  

    --targetDim;
  }
  //-----------------------
  // ** main loop ends
  //-----------------------


  //----------------------------------
  // Initialize the startPoint_ array
  //----------------------------------

  startPoint_ = new Switch* [maxWeight_ + 1 ];
  
  std::list<Switch*>::const_iterator sli = setOfSwitches.end();

  std::advance( sli,  - (maxWeight_ + 1) );

  for( int i=0; i < maxWeight_+1; ++i, ++sli ) 
  { 
    startPoint_[i] = *sli; 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


Cascade::~Cascade() 
{
  if( startPoint_ )  { delete []  startPoint_; startPoint_  = 0; }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Cascade::testSwitchAllocation( std::list<Switch*> const& setOfSwitches ) const
{
 
  (*pcout) << "All the switches:" << endl;

  Switch const * swPtr;

  int i = 0;
  for( std::list<Switch*>::const_iterator sli = setOfSwitches.begin(); sli != setOfSwitches.end(); ++sli ) 
  {
     swPtr = *sli;
     (*pcout) << swPtr->xpt_ << " : " << ++i << endl;
  }

  int expectedLinks = 0;
  int m = numVar_;

  while( 0 < --m ) {
    expectedLinks += bcfRec(maxWeight_+m, m);
  }

  (*pcout) << "Total number of nodes = " 
           <<  numberOfSwitches_ << endl;
  (*pcout) << "           link nodes = " 
           << ( numberOfSwitches_ - numberOfMonomials_ ) 
           << "; expected "
           << expectedLinks
          << endl;
 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Cascade::index( int const * const e ) const
{

  Switch* swPtr = startPoint_[e[0]];

  for( int j = 1; j < numVar_; ++j ) {
    swPtr = swPtr->arrow_[e[j]];
  }

  return swPtr->index_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

IntArray Cascade::exponents( IntArray const& e ) const
{

  IntArray::const_iterator it = e.begin();
 
  Switch* swPtr = startPoint_[ *it ];

  for( int j=1;  j < numVar_;  ++j) {

     swPtr = swPtr->arrow_[ *(++it) ];
  }
 
  return swPtr->xpt_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Cascade::selfTest() const
{

  int f[numVar_];
  IntArray e(numVar_);

  std::fill( &f[0],&f[0]+ numVar_, int() );

  // Test the results
  if( verbose_ ) { (*pcout) << "\nTesting accuracy." << endl; }

  // Do the first monomial by hand

  for( int i=0; i< numVar_; ++i ) { f[i] = 0; }
  e.Set(f);

  if( verbose_ ) { (*pcout) << e << endl; }

  Switch const* swPtr = startPoint_[e(0)];

  if( verbose_ ) { (*pcout) << "  " << swPtr->xpt_ << endl; }

  for( int j = 1; j < numVar_; ++j ) {
    swPtr =  swPtr->arrow_[e(j)];
    if( verbose_ ) { (*pcout) << "  " << swPtr->xpt_ << endl; }
  }

  if( verbose_ ) { (*pcout) << e << ": -> " << swPtr->xpt_ << endl; }

  if( e != swPtr->xpt_ ) {
    (*pcout) << "Error occurred: " 
         << e << ": -> " << swPtr->xpt_ 
         << endl;
  }

  // ... and then all subsequent monomials

  for( int i=1; i <= maxWeight_; ++i) {

    while( nexcom( i, numVar_, f) ) {
      e.Set(f);

      if( verbose_ ) { (*pcout) << e << endl; }

      swPtr = startPoint_[e(0)];

      if( verbose_ ) { (*pcout) << "  " << swPtr->xpt_ << endl; }

      for( int j = 1; j < numVar_; j++ ) {

        swPtr = swPtr->arrow_[e(j)];
        if( verbose_ ) { (*pcout) << "  " << swPtr->xpt_ << endl; }
      }
      if( verbose_ ) { (*pcout) << e << ": -> " << swPtr->xpt_ << endl; }
    }
  }

  if( verbose_  ) { (*pcout) << "Finished testing accuracy." << endl; }


  (*pcerr) << "\nCascade::selfTest beginning test of all possible indices." 
            << std::endl;
  int ret = 0;


  int w = maxWeight_;


  for( int i=0; i < numVar_; ++i) { f[i] = 0; }

  e.Set(f);
  if( e != (this->exponents(e)) ) {
    (*pcerr) << "Type 1 error: " << e << " != " << (this->exponents(e)) << endl;
    ret = 1;
  }
  if( e != arrayOfSwitches_[ index(e) ].xpt_ ) {
    (*pcerr) << "Type 2 error: " << e << " != " 
              << "arrayOfSwitches_["
              << ( index(e))
              << "] = " 
              << ( arrayOfSwitches_[index(e)].xpt_)
              << endl;
    ret = 2;
  }


  for( int i = 1; i <= maxWeight_; ++i) {
    while( nexcom( i, numVar_, f) ) {
      e.Set(f);
      if( e != exponents(e) ) {
        (*pcerr) << "Type 1 error: " << e << " != " << exponents(e) << endl;
        ret = 1;
      }
      if( e != arrayOfSwitches_[ index(e)].xpt_ ) {
        (*pcerr) << "Type 2 error: " << e << " != " 
                  << "arrayOfSwitches_["
                  <<  index(e)
                  << "] = " 
                  << (arrayOfSwitches_[this->index(e)].xpt_)
                  << endl;
        ret = 2;
      }
    }
  }

  (*pcerr) << "\nCascade::selfTest finished test of all possible indices." 
            << std::endl;
  return ret;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
