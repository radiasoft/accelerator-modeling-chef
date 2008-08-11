/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.tcc
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
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
******  REVISION HISTORY:
******
******  Feb  2005 - Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******  
****** - Efficiency improvements.
****** - new memory management
******  
******  Sept 2005   ostiguy@fnal.gov
******
****** - new code based on a single template parameter rather than 
******   two. Mixed mode handled using implicit conversion.
****** - centralized environment management
******
******  Nov 2005     ostiguy@fnal.gov                                           
******
****** - reference counted environments based on boost::intrusive_ptr<>
****** - shared scratchpads:  implemented ScratchArea as a distinct 
******   private class. Only one instance of ScratchArea is created for 
******   for *all* environments sharing the same values of maxweight, 
******   and numvar.   
******
******  May 2006   ostiguy@fnal.gov
****** 
****** - EnvPtr<> is now a true class rather than a typedef wrapper 
****** - template syntax declaration cleanup to better conform to standard. 
******   Code now compiles cleanly with g++ 4.X   
******
****** Mar 2007   ostiguy@fnal.gov
******
****** - Introduced new compact monomial indexing scheme based on monomial 
******   ordering to replace previous scheme explicitly based on monomial exponents.
****** - monomial multiplication now handled via a lookup-table.
****** - added STL compatible monomial term iterators   
****** - eliminated dependence on (obsolete) class Cascade     
******      
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/VectorD.h>   // Used by .approxEq
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>
#include <mxyzptlk/JLPtr.h>
#include <mxyzptlk/TMapping.h>
#include <mxyzptlk/EnvPtr.h>

#include <limits>
#include <iomanip>
#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>
#include <functional>
#include <boost/function.hpp>


using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// --------------------------------------------------------------------------------
// IMPORTANT NOTE: The effect of calling exit() or returning from main() is to call the destructors 
// for all static objects, ***in the reverse order of their construction *** (automatic objects are not destructed).
// This may seem unabiguous ... unfortunately, according to the current std, the order of construction is *unspecified* !
//
// The last-in-first-out process also incorporates functions registered with atexit(), such that a function 
// registered with atexit() after a static object is constructed, will be called before that static object is destructed.
// EnvPtr<>s are kept in a static (list) container. When the program terminates, the destructor for this container is
// called. All the contained objects are "destroyed" (dispose() is called only if refcount = 0). 
// Unfortunately, there may be static instances of  EnvPtr in existence with refcount=1 *after* the list is destroyed. 
// When their destructor is called, the dispose() function will be called. In normal operations, dispose() removes 
// the object from the list ... but at this point the list is gone ! 
// 
// The kludge below is meant to work around this sort of problem.  
// Note that the environments_ list destructor will never be called because the destructors are 
// **not** called for **dynamic** objects upon exit from the main program.    
// Perhaps a cleaner scheme based on atexit() should be explored. -JFO
 
// --------------------------------------------------------------------------------
//  Static variables
//--------------------------------------------------------------------------------- 

//template<typename T> 
//EnvPtr<T>                                     TJetEnvironment<T>::lastEnv_; // defaults to a null pointer

template<typename T> 
std::stack<EnvPtr<T> > TJetEnvironment<T>::envstack_;

template<typename T> 
std::list<EnvPtr<T> >&                        TJetEnvironment<T>::environments_ 
               = *( new std::list<EnvPtr<T> >() );

template<typename T> 
std::deque<Tcoord<T>*>&                       TJetEnvironment<T>::coordinates_
               = *(new std::deque<Tcoord<T>*>() );

template<typename T> 
std::deque<Tparam<T>*>&                        TJetEnvironment<T>::parameters_  
               = *( new std::deque<Tparam<T>*>() );


template<typename T> 
std::list<typename TJetEnvironment<T>::template ScratchArea<T>* >&  TJetEnvironment<T>::scratch_areas_ 
               = * (new std::list<typename TJetEnvironment<T>::template ScratchArea<T>* >() ); 
  
template<typename T> int                       TJetEnvironment<T>::tmp_maxWeight_ = 0;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||




// ================================================================
//      Implementation of TJetEnvironment

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//      Initialization routines.
//      Must be called first by any program.
//
//***********************************************************************



template<typename T>
void TJetEnvironment<T>::BeginEnvironment(int maxweight)
{
   if(  !TJetEnvironment<T>::coordinates_.empty() ||  !TJetEnvironment<T>::parameters_.empty() ) {
     throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T>::BeginEnvironment( int w )",
           "Cannot open two environments simultaneously. Close first." ) );
  };

   TJetEnvironment<T>::tmp_maxWeight_     = maxweight;

}

// ---------------------------------------------------------------------------

template<typename T>
EnvPtr<T> TJetEnvironment<T>::EndEnvironment()
{

  int maxweight  =   TJetEnvironment<T>::tmp_maxWeight_;
  int numvar     =   TJetEnvironment<T>::coordinates_.size() +  
                     TJetEnvironment<T>::parameters_.size();

  int spacedim   =   TJetEnvironment<T>::coordinates_.size();

  if ( TJetEnvironment<T>::coordinates_.empty()  &&  TJetEnvironment<T>::parameters_.empty())
  {

    (*pcerr) << "\n\n"
         << "*** ERROR ***                                  \n"
         << "*** ERROR ***  No coordinate or parameter      \n"
         << "*** ERROR ***  compatible with the environment \n"
         << "*** ERROR ***  has been found.                 \n"
         << "*** ERROR ***  Does the environment type match \n"
         << "*** ERROR ***  that of the coordinates and     \n"
         << "*** ERROR ***  parameters ?                    \n"
         << endl;

     throw GenericException(__FILE__, __LINE__, 
                            "EnvPtr<T> TJetEnvironment<T>::EndEnvironment()",
                            "No coordinate or parameter found." );
     
    
  }
  //--------------------------
  // Get the reference points 
  //--------------------------

  TVector<T> refpoints( numvar ); 
 
  typename std::deque< Tcoord<T>* >::iterator itc;

  int index = 0;

  for ( itc  = TJetEnvironment<T>::coordinates_.begin();
        itc !=  TJetEnvironment<T>::coordinates_.end();  ++itc ) {

       refpoints[index++] = (*itc)->value();
  };


  typename std::deque< Tparam<T>* >::iterator itp;
  for ( itp  = TJetEnvironment<T>::parameters_.begin();
        itp !=  TJetEnvironment<T>::parameters_.end();  ++itp ) {

       refpoints[index++] = (*itp)->value();
  };


  //---------------------------
  // Create the new environment 
  //---------------------------


   EnvPtr<T> pje(TJetEnvironment<T>::makeJetEnvironment(maxweight, numvar, spacedim, refpoints ) );   
  

  //---------------------------------------------------------------------
  // instantiate the jet variables held by the coordinates and parameters
  //----------------------------------------------------------------------.
 
  index = 0;

  Tcoord<T>* p = 0;
  while ( !TJetEnvironment<T>::coordinates_.empty() ) {
    
       p = *(TJetEnvironment<T>::coordinates_.begin()); 
       TJetEnvironment<T>::coordinates_.pop_front();
       
       p->instantiate(index++, pje);
  };

  Tparam<T>* q = 0;

  while(  !TJetEnvironment<T>::parameters_.empty() ) {

       q = *(TJetEnvironment<T>::parameters_.begin());  
       TJetEnvironment<T>::parameters_.pop_front();

       q->instantiate(index++, pje);

  };

  //---------------------------------------------------------------------
  // !TJetEnvironment<T>::coordinates_.empty()
  //----------------------------------------------------------------------.
   
  // TJetEnvironment<T>::setLastEnv( pje );  

  return pje;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace { 

  template <typename U>
  struct EnvCompare {
    EnvCompare( TJetEnvironment<U> const * env )
       : env_(env) {}

    bool operator()( EnvPtr<U> const& rhs ) const 
       { return (env_ == rhs.get()); } 
 
    TJetEnvironment<U> const * const env_;
  };
}

//--------------------------------------------------------------------------------------------------------

template<typename T>
void TJetEnvironment<T>::dispose() {
 
  static bool disposed = false; 

  if ( disposed ) { disposed = false; return; }

  typename std::list<EnvPtr<T> >::iterator iter;

  // restore the true reference count.

  this->add_ref();

  EnvPtr<T> dummy( this, false);   
  iter =  std::find_if(  TJetEnvironment<T>::environments_.begin(),  
                         TJetEnvironment<T>::environments_.end(), EnvCompare<T>( this ) );
 
  if (iter == TJetEnvironment<T>::environments_.end() ) {
    (*pcerr) << "**ERROR***  Environment not found !" << std::endl; 
     return;
  }
  
  //------------------------------------------------------------------
  //                   ****** NOTE ******* 
  // remove() returns the container's new 'end()' (which just
  // happens to be the first element that was removed); this value
  // is then used in the container's 'erase()' call.
  //------------------------------------------------------------------

  // remove EnvPtr(this) from the env list ... 
 
   disposed = true; // The erase operation below will result in another call to dispose() because
                     // the ref count will go from 1 to 0 after erasure. 
                     // Setting this static flag prevents double deletion. 

   environments_.erase(remove(environments_.begin(), environments_.end(), *iter), environments_.end()) ;   

   return;
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>::TJetEnvironment(int maxweight, int numvar, int spacedim, TVector<T> const& refpoint) : 
  ReferenceCounter<TJetEnvironment<T> >(),
  numVar_(numvar),                // number of variables
  spaceDim_(spacedim),            // phase space dimensions
  refPoint_(refpoint),            // reference point (set to zero by default)
  maxWeight_(maxweight)           // maximum weight (polynomial order)
{

  if ( refPoint_.Dim() != numvar )  { 
    refPoint_ =  TVector<T>(numvar); 
  }
  scratch_ = buildScratchPads(maxweight, numvar);
  return;
}
 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void   TJetEnvironment<T>::pushEnv( EnvPtr<T> const& env)
{
  envstack_.push(env);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetEnvironment<T>::popEnv()
{
  envstack_.pop();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
EnvPtr<T> TJetEnvironment<T>::topEnv()
{
  return envstack_.top();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
template<typename U>
TJetEnvironment<T>::ScratchArea<U>::ScratchArea(TJetEnvironment<U>* pje, int w, int n):
  maxWeight_(w),                                        // maxWeight and numVar are duplicated here because 
  numVar_(n),                                           // they are needed to reference an existing scratch area
  maxTerms_( bcfRec( w + n, n ) ),                      // no of monomials in a polynomial of order w in n variables
  monomial_( maxTerms_),
  TJLmonomial_(maxTerms_ ),
  TJLmml_( maxTerms_ )                                  // the actual scratchpad 
{

 //-----------------------------------------------------------------------------------------------------------
 // Create the scratchpad.  
 // NOTE: within the scratchpad, the terms are ordered as follows: 
 // 
 //  (1) by weight, lowest weight first
 //  (2) terms of the same weight are ordered in *reverse* lexicographic order  
 //      for example
 //  ... 
 // Index: ( 11, 0, 1 )  << weight = 12
 // Index: ( 10, 1, 1 )
 // Index: ( 9, 2, 1 ) 
 // Index: ( 8, 3, 1 ) 
 // Index: ( 7, 4, 1 ) 
 // Index: ( 6, 5, 1 ) 
 // Index: ( 5, 6, 1 ) 
 // Index: ( 4, 7, 1 ) 
 // Index: ( 3, 8, 1 ) 
 // Index: ( 2, 9, 1 ) 
 // Index: ( 1, 10, 1 )
 // Index: ( 0, 11, 1 )
 // Index: ( 10, 0, 2 )
 // Index: ( 9, 1, 2 ) 
 // ...
 //
 //  the weight_offsets_ table contains offsets for the start of each group of terms of identical weight 
 //  
 //-------------------------------------------------------------------------------------------------------------

 IntArray exponents(numVar_);
 U startValue(0.0);


 weight_offsets_.resize( maxWeight_+2 );
 index_table_.resize(maxTerms_);

 new ( &TJLmml_[0] ) TJLterm<U>( startValue, 0, 0 );

 weight_offsets_[0] = 0; 
    index_table_[0] = exponents;

 int i = 1;
 for( int wd = 1; wd <= maxWeight_; ++wd ) {

   weight_offsets_[wd] = i;

   while( nexcom( wd, n, exponents ) ) {

     if( i <  maxTerms_ ) {
       new ( &TJLmml_[i])TJLterm<U>(  startValue, i , exponents.Sum());
       index_table_[i]    = exponents;
     }
     else {
       throw( GenericException( __FILE__, __LINE__, 
              "TJetEnvironment::_buildScratchPads()",
              "Overran number of possible monomial terms." ) );
     }

     ++i;
   } // end while
 } // end for

  weight_offsets_[ maxWeight_+1] = i; // points to 1 location beyond the last element in the scratchpad.  


 //------------------------------------
 // build monomial multiplication table  
 //-------------------------------------
   
   multTable_.resize(maxTerms_);

   int maxidx = 0; 
   int wgt    = maxWeight_+1; 

   for (int i=0; i<maxTerms_; ++i) {

      if ( wgt  >  ( maxWeight_ - index_table_[i].Sum() ) ) {
          --wgt;  
          maxidx        =  bcfRec( wgt + numVar_, numVar_ ); 
      }

      multTable_[i].resize(maxidx);

      for (int j=0; j < maxidx; ++j ) {
                  
         exponents =  index_table_[i]  +  index_table_[j];
         if ( exponents.Sum()  <= maxWeight_ ) 
         { 
                multTable_[i][j] = offsetIndex(exponents);
         }

      }
   }

  //-----------------------------------------------------------------------------------------
  // NOTE:  the call to offsetIndex(exponents) involves a reverse lexicographic search 
  //        within a pre-sorted table. 
  //        This operation is somewhat slow, and dominates the time needed to fill the 
  //        multiplication table. The latter needs to be filled once, when the 
  //        environment is set up.      
  //-----------------------------------------------------------------------------------------
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename U>
void  TJetEnvironment<T>::ScratchArea<U>::debug() const
{  
      

   std::cout << " ------------------------------------------------------------------------------" << std::endl;
   std::cout << " monomial table " << std::endl;
   std::cout << " ------------------------------------------------------------------------------" << std::endl;

  for (int i=0; i < maxTerms_;  ++i ) {
    
    std::cout <<  "offset : " <<  TJLmml_[i].offset_       << "  " 
              <<  "index : "  <<  index_table_[i]          << "  " 
              <<  "weight : " <<  index_table_[i].Weight() << "  " 
              <<  "value : "  <<  TJLmml_[i].value_        << std::endl;
  }


   std::cout << " ------------------------------------------------------------------------------" << std::endl;
   std::cout << " weight offsets table " << std::endl;
   std::cout << " ------------------------------------------------------------------------------" << std::endl;

   for (int i=0; i < maxWeight_+1;  ++i ) {
 
   std::cout << "weight " << i << "  start: " << weight_offsets_[i] 
                               << "  end:   " << weight_offsets_[i+1] << std::endl;
   }

   std::cout << " ------------------------------------------------------------------------------" << std::endl;
   std::cout << " multiplication table " << std::endl;
   std::cout << " ------------------------------------------------------------------------------" << std::endl;

   for (int i=0; i < maxTerms_;  ++i ) {
      for (int j=0; j<multTable_[i].size(); ++j ) {

       std::cout <<  index_table_[i] <<  " *  " 
                 <<  index_table_[j] <<  " =  " 
                 <<  index_table_[ multTable_[i][j] ] << std::endl;

      }
   }

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename U>
 TJetEnvironment<T>::ScratchArea<U>::~ScratchArea()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace {

template <typename T>
class ReverseLexicographicTermCompare {

public:

 ReverseLexicographicTermCompare( typename TJetEnvironment<T>::template ScratchArea<T> const& scratch ): scratch_(scratch) {}


 bool operator()(TJLterm<T> const& lhs,  IntArray const& rhs)  const
 {
     return  ( scratch_.index_table_[lhs.offset_] <  rhs );
 }

private:

 typename TJetEnvironment<T>::template ScratchArea<T> const& scratch_;

};

}
//---------------------------------------------------------------------------------------------------------

template <typename T>
template <typename U>
int  TJetEnvironment<T>::ScratchArea<U>::offsetIndex(IntArray const& exp) const
{

 int weight = exp.Sum(); 

 TJLterm<T> const* itstart = &TJLmml_[0] + weight_offsets_[weight]; 
 TJLterm<T> const* itend   = &TJLmml_[0] + weight_offsets_[weight+1];

 // look for a match to exp in the scratchpad, within monomials of same weight. 

 return std::lower_bound( itstart, itend, exp, ReverseLexicographicTermCompare<T>(*this) )->offset_;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetEnvironment<T>::ScratchArea<T>* 
TJetEnvironment<T>::buildScratchPads(int maxweight, int numvar)
{


  // If a suitable scratchpad already exists, return it.
 
  typename std::list<ScratchArea<T>*>::iterator it;

  it = find_if(  scratch_areas_.begin(),  scratch_areas_.end(), Exists<T>(maxweight, numvar) );  

  if (it !=  scratch_areas_.end() ) return (*it); 
 
  // Otherwise, create a new one, save it and return it.
  
  ScratchArea<T>* p 
     = new ScratchArea<T>(this, maxweight, numvar);
  
  scratch_areas_.push_back(p);

  return p;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>::~TJetEnvironment() 
{
  // the scratch area is not deleted. To do this safely would also involve 
  // reference counting  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
EnvPtr<T> TJetEnvironment<T>::makeInverseJetEnvironment( TMapping<T> const& map )
{

 //---------------------------------------------------------------------------------------------------------------------
 // NOTE: in general, the inverse map does not exist unless map.Dim() == pEnv->_numVar !@!! (Implicit function theorem) 
 // -------------------------------------------------------------------------------------------------------------------- 

  TVector<T> refpoint( map.Dim() );

  for (int i=0; i <map.Dim(); ++i) {
    refpoint[i] =  map[i].standardPart();
  }  
  
  EnvPtr<T> mapenv( map.Env() );   

  EnvPtr<T> pInvEnv( makeJetEnvironment(  mapenv->maxWeight_, mapenv->numVar_,  mapenv->spaceDim_, refpoint ) );

  return pInvEnv;
 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
EnvPtr<T>  TJetEnvironment<T>::makeJetEnvironment(int maxweight, int nvar, int spacedim, TVector<T> const& refpoints )
{

 //-----------------------------------------------------
 // if a match already  exists, return it 
 //-----------------------------------------------------

 TJetEnvironment<T>* pje = new TJetEnvironment<T>( maxweight, nvar, spacedim, ( refpoints.Dim() != nvar ? TVector<T>(nvar) : refpoints ) );

 for(  typename std::list<EnvPtr<T> >::iterator env_iter = environments_.begin(); 
                                                env_iter !=  environments_.end(); 
                                                         ++env_iter ){ 
 
    if  (**env_iter == *pje ) { // exit if a suitable env already exists
      return *env_iter; 
    }
 
 }

  //--------------------------------------------------------------------------------------------------------------
  // NOTE: The reference count will go to 0 when the only instance of the smart ptr is the one left in 
  //       the environment list_. When that happens, the custom deleter (dispose()) removes the env from the list.   
  //---------------------------------------------------------------------------------------------------------------

  EnvPtr<T> newpje(pje);
  TJetEnvironment<T>::environments_.push_back( newpje );
  newpje->release(); // do *not* count the instance that is in the environments_ list !!!!
  return newpje;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>&  TJetEnvironment<T>::operator=( TJetEnvironment<T> const& env)
{ 

 if ( &env == this ) return *this;
 
  numVar_    = env.numVar_;               // number of variables
  spaceDim_  = env.spaceDim_;             // phase space dimensions

  refPoint_ = TVector<T>( env.numVar_);
  std::copy( env.refPoint_.begin(),  env.refPoint_.end(), refPoint_.begin() );

  maxWeight_ = env.maxWeight_; 

  scratch_ = buildScratchPads(env.maxWeight_, env.numVar_);

  return *this; 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
template<typename U>
bool TJetEnvironment<T>::operator==( TJetEnvironment<U> const& x ) const
{
  Vector tol( x.numVar(), 10.0*std::numeric_limits<double>::epsilon() );
  return is_equivalent( x, tol );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TJetEnvironment<T>::is_equivalent( TJetEnvironment const& x, TVector<double> const& tol) const
{
  if( x.numVar_    !=  numVar_ )    return false;
  if( x.spaceDim_  !=  spaceDim_ )  return false;
  if( x.maxWeight_ !=  maxWeight_ ) return false;
 
  TVector<double>  eps     =  ( refPoint_ - x.refPoint_).Abs();
  return std::equal (eps.begin(), eps.end(), tol.begin(), (std::less<double>()) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::operator!=( TJetEnvironment const& x ) const
{
  return !operator==( x );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, TJetEnvironment<T> const& x )
{
  os << x.numVar_   << endl;
  os << x.spaceDim_ << endl;
  for( int i=0; i < x.numVar_; ++i) { 
     os << x.refPoint_[i] << endl;
  }

  os << x.maxWeight_ << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::istream& streamIn( std::istream& is, EnvPtr<T>& pje )
{

  int numvar    = 0;
  int spacedim  = 0;
  int maxweight = 0;

  is >> numvar;
  is >> spacedim;

  if( numvar < spacedim ) {
    throw( GenericException( __FILE__, __LINE__, 
           "istream& streamIn( istream&, EnvPtr<T>& )",
           "Jet_environment dimensions are wrong." ) );
  }
  
  TVector<T> refpoint(numvar); 

  for( int i=0; i<numvar; ++i ) is >> refpoint[i];

  is >> maxweight;

  pje =  TJetEnvironment<T>::makeJetEnvironment(maxweight, numvar, spacedim, refpoint );

  
  // Initialize the coordinates

  return is;
}


