/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.cc
******  Version:   2.0
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
******  Revision History:
******
******  Feb  2005 - Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******  
****** - Efficiency improvements.
****** - new memory management
******  
******  Sept 2005   ostiguy@fnal.gov
******
****** - new code based on a single template parameter
******   instead of two. Mixed mode handled
******   using conversion operators.
****** - centralized environment management
******
******  Nov 2005     ostiguy@fnal.gov                                           
******
****** - reference counted environments based on boost::intrusive_ptr<>
****** - shared scratchpads:  implemented ScratchArea as a distinct 
******   private class. Only one instance of ScratchArea is created for 
******   for *all* environments sharing the same values of maxweight, 
******   and numvar.   

**************************************************************************
*************************************************************************/
#ifndef TJETENVIRONMENT_TCC
#define TJETENVIRONMENT_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TJetEnvironment.h>
#include <TJet.h> 
#include <GenericException.h>
#include <VectorD.h>   // Used by .approxEq
#include <TMapping.h>

#include <iomanip>
#include <iosetup.h>
#include <vector>
#include <deque>
#include <boost/scoped_array.hpp>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


#include <iostream>

// --------------------------------------------------------------------------------
//  Static variables
//--------------------------------------------------------------------------------- 

template<typename T> 
std::list<typename EnvPtr<T>::Type>                                 TJetEnvironment<T>::_environments;  

template<typename T> std::deque<Tcoord<T>*>                         TJetEnvironment<T>::_coordinates; 
template<typename T> std::deque<Tparam<T>*>                         TJetEnvironment<T>::_parameters;  

template<typename T> 
std::list<typename TJetEnvironment<T>::template ScratchArea<T>* >   TJetEnvironment<T>::_scratch_areas;  

template<typename T> int                                            TJetEnvironment<T>::_tmp_maxWeight = 0;

// -----------------------------------------------------------------------------------
//      External routines
// -----------------------------------------------------------------------------------

extern bool nexcom( int, int, int* );  
                                // Computes the next composition
                                //  of an integer into a number of parts.
                                //  Algorithm devised by Herbert Wilf.

extern "C" {
 int bcfRec( int, int );        // Recursive evaluation of binomial
                                //  coefficient.
 int nearestInteger( double );  // Returns the integer nearest to its
                                //  double argument.
}


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
   if(  !TJetEnvironment<T>::_coordinates.empty() ||  !TJetEnvironment<T>::_parameters.empty() ) {
     throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T>::BeginEnvironment( int w )",
           "Cannot open two environments simultaneously. Close first." ) );
  };

   TJetEnvironment<T>::_tmp_maxWeight     = maxweight;

}

// ---------------------------------------------------------------------------

template<typename T>
typename EnvPtr<T>::Type TJetEnvironment<T>::EndEnvironment(double* scale)
{

  int maxweight  =   TJetEnvironment<T>::_tmp_maxWeight;
  int numvar     =   TJetEnvironment<T>::_coordinates.size() +  
                     TJetEnvironment<T>::_parameters.size();

  int spacedim   =   TJetEnvironment<T>::_coordinates.size();

  if( ( spacedim%2 != 0) ) {
    (*pcerr) << "\n\n"
         << "*** WARNING ***                                 \n"
         << "*** WARNING *** TJet<T>::EndEnvironment()       \n"
         << "*** WARNING *** Phase space has odd dimension.  \n"
         << "*** WARNING *** I hope you know what you        \n"
         << "*** WARNING *** are doing, but I doubt it.      \n"
         << "*** WARNING ***                                 \n"
         << endl;
  }
  

  //--------------------------
  // Get the reference points 
  //--------------------------

  boost::scoped_array<T> refpoints( new T[numvar] ); 
 
  typename std::deque< Tcoord<T>* >::iterator itc;

  int index = 0;

  for ( itc  = TJetEnvironment<T>::_coordinates.begin();
        itc !=  TJetEnvironment<T>::_coordinates.end();  ++itc ) {

       refpoints[index++] = (*itc)->value();
  };


  typename std::deque< Tparam<T>* >::iterator itp;
  for ( itp  = TJetEnvironment<T>::_parameters.begin();
        itp !=  TJetEnvironment<T>::_parameters.end();  ++itp ) {

       refpoints[index++] = (*itp)->value();
  };


  //---------------------------
  // Create the new environment 
  //---------------------------


  typename EnvPtr<T>::Type pje(TJetEnvironment<T>::makeJetEnvironment(maxweight, numvar, spacedim, refpoints.get(), scale));   
  

  //---------------------------------------------------------------------
  // instantiate the jet variables held by the coordinates and parameters
  //----------------------------------------------------------------------.
 
  index = 0;

  Tcoord<T>* p = 0;
  while ( !TJetEnvironment<T>::_coordinates.empty() ) {
    
       p = *(TJetEnvironment<T>::_coordinates.begin()); 
       TJetEnvironment<T>::_coordinates.pop_front();
       
       p->instantiate(index++, pje);
  };

  Tparam<T>* q = 0;

  while(  !TJetEnvironment<T>::_parameters.empty() ) {

       q = *(TJetEnvironment<T>::_parameters.begin());  
       TJetEnvironment<T>::_parameters.pop_front();

       q->instantiate(index++, pje);

  };


  TJet<T>::setLastEnv( pje );  

  return pje;
}

#if 0
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
template<typename T>
void TJet<T>::EnlargeEnvironment( const TJetEnvironment<T>* pje )
{
  // Like TJet<T>::BeginEnvironment

  if( _workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T>::EnlargeEnvironment( TJetEnvironment<T>* )",
           "Close open environment before invoking this function." ) );
  }

  _workEnv             = new TJetEnvironment<T>( );
  _workEnv->_maxWeight = pje->_maxWeight;
  TJetEnvironment<T>::_currentIndex        = 0;  

  // Creating new coordinates ...
  Tcoord<T>* p_coord;

  for( int i=0; i<pje->_numVar; i++ ) {
    p_coord = new Tcoord<T>( pje->_refPoint[i] );
    // _currentIndex is automatically increased by coord::coord
  }

  // Like TJet<T>::Parameters()

  if( pje->_pbok ) {
    _workEnv->_pbok     = pje->_pbok;
    _workEnv->_spaceDim = pje->_spaceDim;
  }
}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif

template<typename T>
class heldPtrsAreEqual {
  
 private:
    
  TJetEnvironment<T>* _pje_raw; 

 public:

  heldPtrsAreEqual( TJetEnvironment<T>* pje_raw ): _pje_raw(pje_raw) {} 
  bool operator()(typename EnvPtr<T>::Type& pje) { 
               return (_pje_raw == pje.get() ); 
  }  

}; 

template<typename T>
void TJetEnvironment<T>::dispose() {

  TJetEnvironment* pje = 0;

  typename std::list<typename EnvPtr<T>::Type>::iterator iter;
  
  iter =  std::find_if(  TJetEnvironment<T>::_environments.begin(),  
                         TJetEnvironment<T>::_environments.end(), 
                         heldPtrsAreEqual<T>(this) );
 
  if (iter == TJetEnvironment<T>::_environments.end() ) {
    (*pcerr) << "Error, env not found !" << std::endl; 
     return;
  }
   
  pje = (*iter).get();             // save the raw pointer. 

  //                   ****** NOTE ******* 
  // remove() returns the container's new 'end()' (which just
  // happens to be the first element that was removed); this value
  // is then used in the container's 'erase()' call.

  // remove all instances of  *this from the env list ... 

  _environments.erase(remove(_environments.begin(), _environments.end(), *iter), _environments.end()) ;   
  
  
 //                        **** NOTE ****  
 // The erase operation above is safe because intrusive_ptr_release() 
 // decrements the reference count from 0 to -1. As a result, 
 // dispose() is **not** called. dispose() is called only 
 // when unless the reference reaches 0 as a result of a decrement 
 // operation..

  // it is now safe to delete the raw pointer ;-) ...  

  if (pje) delete pje;

}


template<typename T>
TJetEnvironment<T>::TJetEnvironment(int maxweight, int numvar, int spacedim, T* refpoint, double* scale) : 

  _numVar(numvar),                // number of variables
  _spaceDim(spacedim),            // phase space dimensions
  _dof(spacedim/2),               // degrees of freedom                             
  _refPoint(new T[numvar]),       // reference point (set to zero by default)
  _scale(new double[numvar]),     // scale (set to 1.0e-3 by default) should be a Vector
  _maxWeight(maxweight),          // maximum weight (polynomial order)
  _pbok(numvar>spacedim),         // THIS IS HERE FOR COMPATIBILITY WITH EARLIER VERSIONS
                                  // _pbok was used as a flag to detect the presence of parameters 
                                  // poisson bracket OK is true only when phase space dimension is even; 
                                  // Consider simply checking the space dimensions before taking a PB ? 

  _scratch( _buildScratchPads(maxweight, numvar) )
 {

    
  for (int i=0; i<_numVar; ++i) {   
       _refPoint[i]   = refpoint[i];
          _scale[i]   = scale[i];
  } 

}
 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
template<typename U>
TJetEnvironment<T>::ScratchArea<U>::ScratchArea(TJetEnvironment<U>* pje, int w, int n):
  _maxWeight(w),                                        // maxWeight and numVar are duplicated here because 
  _numVar(n),                                           // they are needed to reference an existing scratch area
  _maxTerms( bcfRec( w + n, n ) ),                      // no of monomials in a polynomial of order w in n variables
  _offset(w,n),
  _exponent(new int[n]),  
  _monomial(0),
  _TJLmonomial(new typename JLPtr<T>::Type[_maxTerms]), // for monomials used in multinomial evaluation.  
  _TJLmml( TJLterm<T>::array_allocate(_maxTerms) ),     // for collecting terms during multiplication.
  _allZeroes(n)
{


 
 for( int i = 0; i<_maxTerms;  i++) {
   _TJLmonomial[i] = typename JLPtr<T>::Type( TJL<T>::makeTJL( typename EnvPtr<T>::Type(pje), T()) ); 
                                                  // the anonymous constructor above is not suitable for an array.  
                                                   

 }
   
 IntArray powers(n);
 T startValue(0.0);

 _TJLmml[0].Reconstruct( powers, startValue );

 int i = 1;
 for( int wd = 1; wd <= w; wd++ ) {
   while( nexcom( wd, n, _exponent ) ) {
     powers.Set(_exponent);
     if( i < _maxTerms ) {
       _TJLmml[i].Reconstruct( powers, startValue ); 
     }
     else {
       throw( GenericException( __FILE__, __LINE__, 
              "TJetEnvironment::_buildScratchPads()",
              "Overran number of possible monomial terms." ) );
     }
     i++;
   } // end while
 } // end for
}

// =================================================================================================================

template<typename T>
template<typename U>
TJetEnvironment<T>::ScratchArea<U>::~ScratchArea()
{

  if( _monomial   )  { delete [] _monomial;    _monomial    = 0; }

  if( _TJLmonomial ) { delete [] _TJLmonomial; _TJLmonomial = 0; }

  if( _TJLmml     )  { TJLterm<T>::array_deallocate(_TJLmml);      
                       _TJLmml = 0;      
                     }
  if( _exponent   )  { delete [] _exponent;    _exponent = 0;    }


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetEnvironment<T>::ScratchArea<T>* 
TJetEnvironment<T>::_buildScratchPads(int maxweight, int numvar)
{


  // If a suitable scratchpad already exists, return it.
 
  typename std::list<ScratchArea<T>*>::iterator it;

  it = find_if( _scratch_areas.begin(),  _scratch_areas.end(), Exists<T>(maxweight, numvar) );  

  if (it !=  _scratch_areas.end() ) return (*it); 
 
  // Otherwise, create a new one, save it and return it.
  
  ScratchArea<T>* p 
     = new ScratchArea<T>(this, maxweight, numvar);
  
  _scratch_areas.push_back(p);

  return p;

}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>::~TJetEnvironment() 
{
  

  if( _refPoint   )  { delete [] _refPoint;    _refPoint = 0;    }
  if( _scale      )  { delete [] _scale;       _scale = 0;       } 

  // the scratch area is not deleted. To do this safely would also involve 
  // reference counting  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>& TJetEnvironment<T>::DeepCopy( const TJetEnvironment& x ) 
{

  _numVar    = x._numVar;               // number of variables
  _spaceDim  = x._spaceDim;             // phase space dimensions
  _dof       = x._dof;                  // degrees of freedom                             

  if ( _refPoint ) delete _refPoint; 

  _refPoint = new T [_numVar];

  for (int i=0; i<_numVar; ++i ) {
     _refPoint[i] = x._refPoint[i];                 
  }

  if (_scale) delete _scale; 

  _scale = new double[_numVar];  
  
  for (int i=0; i<_numVar; ++i ) {
     _scale[i] = x._scale[i];                 
  }

  _maxWeight = x._maxWeight; 

  _pbok     = x._pbok;                  

  _scratch = _buildScratchPads(_maxWeight, _numVar);

  return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
typename EnvPtr<T>::Type  TJetEnvironment<T>::makeInverseJetEnvironment( const TMapping<T>& map ){

 //---------------------------------------------------------------------------------------------------------------------
 // NOTE: in general, the inverse map does not exist unless map.Dim() == pEnv->_numVar !@!! (Implicit function theorem) 
 // -------------------------------------------------------------------------------------------------------------------- 

  T* refpoint = new T[ map.Dim() ];

  for (int i=0; i <map.Dim(); ++i) {
    refpoint[i] =  map(i).standardPart();
  }  
  
  typename EnvPtr<T>::Type mapenv( map.Env() );   

  typename EnvPtr<T>::Type pInvEnv( makeJetEnvironment(  mapenv->_maxWeight, mapenv->_numVar,  mapenv->_spaceDim, refpoint, mapenv->_scale));

  delete[] refpoint;

  return pInvEnv;
 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EnvPtr<double>::Type TJetEnvironment<double>::makeJetEnvironment(int maxweight, const Vector& v , double* scale) {
 
 boost::scoped_array<double> refpoints( new double[ v.Dim() ]);

 for (int i=0; i<v.Dim(); ++i) {
     refpoints[i] = v(i); 
 }


 return EnvPtr<double>::Type( TJetEnvironment<double>::makeJetEnvironment(maxweight, v.Dim(), v.Dim(), refpoints.get(), scale) );
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename EnvPtr<T>::Type  TJetEnvironment<T>::makeJetEnvironment(int maxweight, int nvar, int spacedim, T* refpoints, double* scale)
{

 boost::scoped_array<T>       tmp_refpoints( new T[nvar]); 
 boost::scoped_array<double>  tmp_scale(new double[nvar]);    
 
 for (int i=0; i<nvar; ++i) {
   if (refpoints)
     tmp_refpoints[i] = refpoints[i];
   else
     tmp_refpoints[i] = T();

  if (scale)
     tmp_scale[i] = scale[i];
   else
     tmp_scale[i] = 0.001;

 }

 typename std::list<typename EnvPtr<T>::Type>::iterator env_iter;

 //-----------------------------------------------------
 // if a match already  exists, return it 
 //-----------------------------------------------------

 typename EnvPtr<T>::Type pje;
 typename EnvPtr<T>::Type tmppje;

 bool refpoints_are_equivalent = false;
 bool scales_are_equivalent    = false;

 for( env_iter = _environments.begin(); 
      env_iter != _environments.end(); 
     ++env_iter )                        { 
 
    tmppje = *env_iter; 

    if (tmppje->maxWeight()    != maxweight )   continue;  
    if (tmppje->numVar()       != nvar )        continue;  
    if (tmppje->spaceDim()     != spacedim )    continue;  
    
    refpoints_are_equivalent = true;
    for (int i=0; i<nvar; ++i ) {
       refpoints_are_equivalent = refpoints_are_equivalent && (tmppje->refPoint()[i] == tmp_refpoints[i] );
    }
    if ( !refpoints_are_equivalent )   continue; 

#if 0
===============================================================
    scales_are_equivalent = true;
    for (int i=0; i<nvar; ++i ) {
      scales_are_equivalent = scales_are_equivalent && ( tmppje->scale()[i] == tmp_scale[i] );
    }
    if ( ! scales_are_equivalent )              continue; 
==================================================================
#endif
  
    // -----------------------------------------------------------
    // if we got here, a suitable environment already exists
    // -----------------------------------------------------------
     pje = tmppje; 
     break;
 }

 if  (pje.get()) { // pje is not null 
     return pje;
 } else  {

     // NOTE: the 2nd argument (default=true) in the smart pointer constructor invocation is set to false. This
     //       prevents the reference count to be incremented. The effect is that the reference count will go to 0 when 
     //       the only instance of the smart ptr is the one left in the _environment list. When the ref count reaches 
     //       0, the custom deleter (dispose()) removes the env from the list.   
  
     // NOTE: The swap function here is used here to prevent the ref count of newly created env ptr to go from 1 to zero 
     //       and be prematurely deleted. Normally this is not a problem, but here the pointer is created (execeptionally)
     //         with an initial ref count of 0.

     typename EnvPtr<T>::Type newpje( new TJetEnvironment<T>( maxweight, nvar, spacedim, tmp_refpoints.get(), tmp_scale.get()), false);
     pje.swap( newpje );

     TJetEnvironment<T>::_environments.push_back( pje );

     return pje;
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

EnvPtr<double>::Type  TJetEnvironment<double>::getApproxJetEnvironment(int maxweight, const Vector& refpoints)
{

  //   Determine if a Jet environment already exists
  //   whose reference point is sufficiently close
  //   to the closed orbit. If so, returns it, otherwise
  //   return a NULL environment

  int nvar = refpoints.Dim();
  
  EnvPtr<double>::Type pje; // null 
  Vector tolerance( nvar );

  for( int i=0;  i<nvar; ++i ) {
    // *** CHANGE ***
    // *** CHANGE *** The tolerance criterion should be user-determined.
    // *** CHANGE ***
    tolerance(i) = std::abs(0.001*refpoints(i));
  }

  std::list<EnvPtr<double>::Type>::iterator env_iter;

  for ( env_iter  = TJetEnvironment<double>::_environments.begin();  
        env_iter != TJetEnvironment<double>::_environments.end(); 
        ++env_iter)
  { 
    if( (*env_iter)->numVar() != nvar ) continue; 

    if( (*env_iter)->hasApproxReferencePoint( refpoints, tolerance ) ) {
      pje   = *env_iter; 
    }
  }

  return pje;  
    

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0
template<typename T>
TJetEnvironment<T>::TJetEnvironment( const TJetEnvironment&) 
{
  //-------------------------------------------------------------------------
  // Since the copy constructor is declared private, this should 
  // never be called. 
  //-------------------------------------------------------------------------

    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment::TJetEnvironment( const TJetEnvironment&)",
           "Calling the copy constructor is forbidden." ) );


}
#endif

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TJetEnvironment<T>::operator==( const TJetEnvironment& x ) const
{
  int i;
  if( x._numVar != _numVar ) return false;
  if( x._spaceDim != _spaceDim ) return false;
  if( x._maxWeight != _maxWeight ) return false;
  for( i = 0; i < _numVar; i++ ) {
    if( _refPoint[i] != x._refPoint[i] ) return false;
    if( _scale[i] != x._scale[i] ) return false;
  }
  return true;  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::approxEq( const TJetEnvironment& x, const Vector& tolerance ) const
{
  int i;
  if( x._numVar != _numVar ) return false;
  if( x._spaceDim != _spaceDim ) return false;
  if( x._maxWeight != _maxWeight ) return false;
  for( i = 0; i < _numVar; i++ ) {
    if( std::abs(_refPoint[i] - x._refPoint[i]) > std::abs(tolerance(i)) ) 
    { return false; }
    // Note: unlike operator=, there is no test for _scale here.
  }
  return true;  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::approxEq( const TJetEnvironment& x, const double* tolerance ) const
{
  Vector w( _numVar, tolerance );
  return this->approxEq( x, w );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasReferencePoint( const Vector& x ) const
{
  if( x.Dim() != _numVar ) { return false; }
  for( int i = 0; i < _numVar; i++ ) {
    if( _refPoint[i] != x(i) ) return false;
  }
  return true;  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasReferencePoint( const double* x ) const
{
  Vector w( _numVar, x );
  return this->hasReferencePoint( w );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const Vector& x, const Vector& tolerance ) const
{
  if( x.Dim() != _numVar || tolerance.Dim() != _numVar ) 
  { return false; }
  for( int i = 0; i < _numVar; i++ ) {
    if( std::abs(_refPoint[i] - x(i)) > std::abs(tolerance(i)) ) 
    { return false; }
  }
  return true;  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const double* x, const Vector& tolerance ) const
{
  Vector w( _numVar, x );
  return this->hasApproxReferencePoint( w, tolerance );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const Vector& x, const double* tolerance ) const
{
  Vector w( _numVar, tolerance );
  return this->hasApproxReferencePoint( x, w );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const double* x, const double* tolerance ) const
{
  Vector w( _numVar, x );
  Vector z( _numVar, tolerance );
  return this->hasApproxReferencePoint( w, z );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetEnvironment<T>::operator!=( const TJetEnvironment& x ) const
{
  return !operator==( x );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, const TJetEnvironment<T>& x )
{
  os << x._numVar   << endl;
  os << x._spaceDim << endl;
  for( int i = 0; i < x._numVar; ++i) { 
     os << x._refPoint[i] << endl;
  }
  for( int i = 0; i < x._numVar; ++i) {
      os << x._scale[i] << endl;
//    os << setprecision(30) << x._scale[i] << endl;
  }
  os << x._maxWeight << endl;
  os << x._pbok << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<typename T>
std::istream& streamIn( std::istream& is, boost::intrusive_ptr<TJetEnvironment<T> >& pje )
{

  int numvar    = 0;
  int spacedim  = 0;
  int maxweight = 0;
  bool pbok     = false;

  is >> numvar;
  is >> spacedim;

  if( numvar < spacedim ) {
    throw( GenericException( __FILE__, __LINE__, 
           "istream& streamIn( istream&, EnvPtr<T>& )",
           "Jet_environment dimensions are wrong." ) );
  }
  
  T*      refpoint = new T[numvar]; 
  double* scale    = new double[numvar]; 

  for( int i=0; i<numvar; ++i ) is >> refpoint[i];
  for( int i=0; i<numvar; ++i ) is >> scale[i];

  is >> maxweight;
  is >> pbok;


  pje =  TJetEnvironment<T>::makeJetEnvironment(maxweight, numvar, spacedim, refpoint, scale);

  
  // Initialize the coordinates
  // ??? HOW ???

  //cleanup 

  delete[] refpoint; 
  delete[] scale;

  return is;
}

#endif // TJETENVIRONMENT_TCC
