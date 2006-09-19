/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      IntArray.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*******
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
******                                                                
****** Revision History:
******
******  Mar 2005  
******
******  Jean-Francois Ostiguy, ostiguy@fnal.gov
******
******  Efficiency improvements. 
******  - Data is no longer dynamically allocated, 
******    resulting in fixed size TJLterm<>.  
******  - exponents are represented by exponent_t type variables. 
******    exponent_t is defined by default as signed char (8 bits) 
******  - no explicit destructor 
******  - added iterator classes
******  - eliminated inefficient array style 
******    dereferencing 
*****
******  Sep 2006 ostiguy@fnal.gov 
******
****** - added MonomialOrderPredicate member. 
******   (useful for debugging)
******   
*************************************************************************/

#ifndef INTARRAY_H
#define INTARRAY_H

#include <iostream>
#include <exception>
#include <string>

#include <basic_toolkit/globaldefs.h>

// ***  Note:
// ***  Default max is set 8 variables (6 phase space vars + 2). 
// ***  Increase intarray_max_variables if more variables are needed.

const int intarray_max_variables = 8;

// ***  Note:
// *** The following typedef can be safely redefined without 
// *** breaking mxyzptlk. 
 
typedef signed char exponent_t;


// **************************************************************************


class IntArray;
class IntArrayIterator;
class IntArrayReverseIterator;

bool  MonomialOrderPredicate( IntArray const& a1, IntArray const& a2);

class DLLEXPORT IntArray {

  friend class IntArrayIterator;
  friend class IntArrayReverseIterator;

protected:

  int         _dim;                            // Number of components
  exponent_t _comp[intarray_max_variables];    
           
public:

  // Constructors and the destructor ...
  IntArray( int              dim = intarray_max_variables,
            int const* const components = 0 );

  IntArray( IntArray const& x): _dim(x._dim) { memcpy((void *)_comp, (const void *)x._comp, _dim * sizeof(exponent_t)); }

  // ~IntArray();  MUST NOT BE DEFINED !

  // Assignment ...

  void     Set              ( const int* );
  void     Set              ( int  );

  inline int     operator()       (  int i) const
     { return (int) _comp[i]; }    // return component

  inline exponent_t&  operator()       ( int  i)
     { return _comp[i]; }    // set    component

  //void     Reconstruct( int = 0, const int* = 0 );  
  //void     Reconstruct( const IntArray& );  

 // Just like constructor

  // Functions ...

  IntArray&     operator=      ( IntArray const& );

  bool          operator==     ( IntArray const& ) const;
  bool          operator!=     ( IntArray const& ) const;
  bool          operator<      ( IntArray const& ) const;
  bool          operator<=     ( IntArray const& ) const;
  bool          operator>      ( IntArray const& ) const; // compares weights only 
  bool          operator>=     ( IntArray const& ) const;

  bool          operator==     ( int ) const;
  bool          operator!=     ( int ) const;

  bool          IsNull         () const;


  friend bool   MonomialOrderPredicate( IntArray const& a1, IntArray const& a2);

  IntArray      operator+( const IntArray& ) const;

  // Queries ...
   int  Dim() const { return _dim; }
   int  Sum() const;

  // Utilities ..

  friend std::ostream& operator<<( std::ostream&, const IntArray& );
  friend std::istream& operator>>( std::istream&, IntArray& );

  // Exceptions ...
  struct GenericException : public std::exception
  {
    GenericException( const char*, const char* );
    // 1st argument: identifies function containing throw
    // 2nd         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };

};
 

inline int IntArray::Sum() const {
    
     exponent_t s = 0;
     const exponent_t* comp_ptr  = &_comp[0] - 1;  
     int count = _dim+1;
     while (--count) s += *(++comp_ptr);
     return s;
}


inline IntArray IntArray::operator+( IntArray const& y) const {
     
     IntArray ret( *this );
     const exponent_t* yPtr = &(y._comp[0])   - 1;  
     exponent_t* xPtr = &(ret._comp[0]) - 1; 

     const exponent_t* upper = yPtr + _dim;
     while( yPtr < upper ) { 
        *(++xPtr) += *(++yPtr);
     }
     return ret;
}

// -----------------------------------------------------------------
// Iterator classes with inlined member functions for efficiency. 
//------------------------------------------------------------------

class IntArrayIterator{

 public:

  IntArrayIterator( const IntArray& a): _origin(&a._comp[0] - 1), _current_index(_origin) { }
  int operator()()   { return *(++_current_index); }
  void reset()       { _current_index = _origin;   }

 protected:

   const exponent_t* _origin;
   const exponent_t* _current_index;
	   
};

class IntArrayReverseIterator{

 public:

  IntArrayReverseIterator( const IntArray& a): _origin(&a._comp[a._dim]), _current_index(_origin) { }
  int operator()() { return *(--_current_index); }
  void reset()     { _current_index = _origin;   }

 protected:

   const exponent_t* _origin;
   const exponent_t* _current_index;
	   
};


#endif // INTARRAY_H
