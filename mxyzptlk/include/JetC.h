/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetC.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


#ifndef JETC_HXX
#define JETC_HXX

#include "slist.h"
#include "dlist.h"
#include "JetC__environment.h"
#include "IntArray.h"
#include "JLC.h"    // Not strictly needed, but should not be
                    // necessary to invoke this from app'n.

struct JLCterm;
struct JL;
struct JLC;
class  JetCVector;
class  Jet;
class  Mapping;
class  LieOperator;
class  CLieOperator;

class JetC {

private:
  JLC*  jl;
  dlist_iterator* constIterPtr;
  dlist_iterator* iterPtr;

  JetC EpsInverse() const;  // Calculates the inverse of
                            // nilpotent JetCs.
  JetC concat() const;

  JetC TruncMult( const JetC&, const int& ) const;  // Used by division algorithm.

  // Friends
  // ??? REMOVE: friend class coordC;   // ??? Is this necessary???
  // ??? REMOVE: friend class JetCVector;
  // ??? REMOVE: friend class Jet;
  // ??? REMOVE: friend class LieOperator;  // ??? Necessary for LieOperator copy constructor
  // ??? REMOVE: friend class CLieOperator;

  friend Jet real( const JetC & );
  friend Jet imag( const JetC & );

  // ??? REMOVE: friend void normalForm( const Mapping&,    /* Input:   map                    */
  // ??? REMOVE:                         int,               /* Input:   order                  */
  // ??? REMOVE:                         MatrixC*,          /* Output:  matrix of eigenvectors */
  // ??? REMOVE:                                            /*          of Jacobian of map     */
  // ??? REMOVE:                         CLieOperator*,     /* Output:  normal form            */
  // ??? REMOVE:                         CLieOperator* );   /* Output:  transformation         */
  // ??? REMOVE: 
  // ??? REMOVE: 
  // ??? REMOVE static dlist  coordCPool;          


  // Utility functions; used by public analytic functions ...
  static JetC epsCos( const JetC& );
  static JetC epsExp( const JetC& );
  static JetC epsPow( const JetC&, const double& );
  static JetC epsSin( const JetC& );
  static JetC epsSqrt( const JetC& ); 


protected:
  static dlist  coordCPtrs;          
  static slist  newCoordCs;
  static slist  newValues;
  static int    currentIndex;

  JLC* operator->() const { return jl; }

public:
  static JetC__environment* lastEnv;

  // Constructors and destructors_____________________________________
  JetC( JetC__environment* = JetC::lastEnv );
  JetC( const FNAL::Complex&, JetC__environment* = JetC::lastEnv );
  JetC( const JetC& );
  JetC( const Jet&  );
  JetC( const Jet&, /* const */ JetC__environment* );
  ~JetC();

  void Reconstruct();
  void Reconstruct( JetC__environment* pje );

  static JetC__environment* workEnv;  // This probably doesn't belong with JetC.
  static void BeginEnvironment( int = 1 /* maximum weight */ );
  static void Parameters();
  static JetC__environment* EndEnvironment( FNAL::Complex* = 0 /* scale array    */ );
  static void EnlargeEnvironment( JetC__environment* );
  static JetC__environment* CreateEnvFrom( const Jet__environment* );

  void setEnvTo( const JetC& );              // Changes environment to
  void setEnvTo( const JetC__environment* ); // that of the argument.

  int intEnv() const;                        // Returns integer representation
                                             // of the environment pointer.
  JetC__environment* Env() const;            // ??? DANGER!!! These two break
  void Env( JetC__environment* pje ) const;  // ??? DANGER!!! protection!!


  JL* newJL() const;                         // Used by conversion routines.
  JL* newJL( /* const */ Jet__environment* ) const;

  // Obsolete setup routines -----------------------------------------
  // static void Setup( const int      = 6, /* Total number of variables */
  //                    const int      = 1, /* Maximum derivative weight */
  //                    const int      = 0, /* Dimension of phase space  */
  //                    const FNAL::Complex*  = 0, /* Default reference point   */
  //                    const FNAL::Complex*  = 0  /* Scaling array             */
  //                  );
  // 
  // static void setup( const int, const int );
  // static void setup( const int, const int, const FNAL::Complex* );
  // static void setup( const int, const int, const int );
  // static void setup( const int, const int, const int, const FNAL::Complex* );
  // ------------------------------------------------------------------

  // ??? REMOVE static void FixReference        ( const FNAL::Complex* );
  // ??? REMOVE static void FixReference        ( const int* );
  // ??? REMOVE static void FixReference        ( const JetC& );
  // ??? REMOVE static void FixReferenceAtStart ( const LieOperator& );
  // ??? REMOVE static void FixReferenceAtEnd   ( const LieOperator& );

  // ??? REMOVE: static FILE  *scratchFile;

  // Public member functions__________________________________________
  JLCterm* get();               // Pops the top term, which should be the 
                                // one of lowest weight.
  JLCterm  firstTerm() const;   // Returns a JLterm equivalent to the top term,
                                // which should be the one of lowest weight.
  JLCterm  lowTerm()   const;   // Returns a JLCterm equivalent to the 
                                // non-zero term of lowest weight.
  void addTerm( JLCterm* );     // Public only for diagnostic purposes.

  void     resetConstIterator();
  JLCterm  stepConstIterator()  const;
  void     resetIterator();
  JLCterm* stepIterator();

  // ??? REMOVE void fixReference();
  // ??? REMOVE void fixReference( const FNAL::Complex* );
  // ??? REMOVE void fixReference( const JetC& );
  // ??? REMOVE void fixReferenceAtEnd( const LieOperator& );
  // ??? REMOVE void fixReferenceAtStart( const LieOperator& );

  char isNilpotent() const;

  void peekAt() const;
  void printCoeffs() const;
  void peekAt( int ) const;       // Prints to JetC::scratchFile
  void printCoeffs( int ) const;
  int  termCount() const;         // Returns number of monomial terms

  // ??? REMOVE: dlist image();
  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

  void getReference( FNAL::Complex* ) const;
  int  getEnvNumVar() const;
  int  getEnvSpaceDim() const;
  int  getEnvMaxWeight() const;

  int  getWeight() const;
  int  getAccuWgt() const;

  void scaleBy( FNAL::Complex );

  void setVariable( const FNAL::Complex&, const int&, JetC__environment* );
               // Alters the environment
  void setVariable( const int&, JetC__environment* );
               // Does not alter the environment.

  FNAL::Complex standardPart() const;
  void clear();
  FNAL::Complex weightedDerivative( const int* ) const;
  FNAL::Complex derivative( const int* ) const;
  JetC filter(  const int&, const int& ) const;  
                                  // Returns only those JLCterms whose weight 
          	                  // are between two specified values, inclusive.
  JetC filter( char (*)( const IntArray&, const FNAL::Complex& ) ) const; 
                                   // Returns those JLCterms for which the 
                                   // argument is satisfied.
  FNAL::Complex operator() ( const FNAL::Complex* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the JetC variable.  Essentially acts as a 
				   // power series expansion.
  JetC  operator() ( const JetC* ) const;  // Self explanatory ...
  JetC  operator() ( /* const */ JetCVector& ) const;  // Self explanatory ...
  // ??? REMOVE JetC  operator() ( const LieOperator& ) const; 
  // ??? REMOVE                                  // Self explanatory ...
  JetC  D( const int* )const ;		   // Performs differentiation of a JetC variable.
  JetC  D( const IntArray& )const ;	   // Performs differentiation of a JetC variable.

  // Operators________________________________________________________
  friend ostream& operator<<(ostream&, const JetC&);
  friend istream& operator>>(istream&, JetC&);
  friend char operator==( const JetC&, const JetC& );
  friend char operator==( const JetC&, const FNAL::Complex& );
  friend char operator==( const FNAL::Complex&, const JetC& );

  JetC& operator=( const JetC& );
  JetC& operator=( const Jet&  );
  JetC& operator=( const FNAL::Complex& );
  JetC& DeepCopy( const JetC& );
  void operator+=( const FNAL::Complex& );
  void operator-=( const FNAL::Complex& );

  void operator+=( const JetC& );   // ??? Why are these void operators?

  friend JetC operator+( const JetC&, const JetC& );
  friend JetC operator+( const JetC&, const Jet&  );
  friend JetC operator+( const Jet&,  const JetC& );
  friend JetC operator+( const JetC&, const FNAL::Complex& );
  friend JetC operator+( const FNAL::Complex&, const JetC& );
  friend JetC operator+( const JetC&, const double& );
  friend JetC operator+( const double&, const JetC& );

  friend JetC operator-( const JetC&, const JetC& );
  friend JetC operator-( const Jet&,  const JetC& );
  friend JetC operator-( const JetC&, const Jet&  );
  friend JetC operator-( const JetC& );
  friend JetC operator-( const JetC&, const FNAL::Complex& );
  friend JetC operator-( const FNAL::Complex&, const JetC& );
  friend JetC operator-( const JetC&, const double& );
  friend JetC operator-( const double&, const JetC& );

  friend JetC operator*( const JetC&, const JetC& );
  friend JetC operator*( const JetC&, const Jet& );
  friend JetC operator*( const Jet&,  const JetC& );
  friend JetC operator*( const JetC&, const FNAL::Complex& );
  friend JetC operator*( const FNAL::Complex&, const JetC& );  // ??? Change this!!

  friend JetC operator/( const JetC&, const JetC& );     // ??? Friendliness
  friend JetC operator/( const Jet&,  const JetC& );     // ??? unnecessary
  friend JetC operator/( const JetC&, const Jet&  );     
  friend JetC operator/( const JetC&, const FNAL::Complex& );  
  friend JetC operator/( const FNAL::Complex&, const JetC& ); 
  friend JetC operator/( const JetC&, const double& );  
  friend JetC operator/( const double&, const JetC& ); 

  friend JetC operator^( const JetC&, const JetC& );   // Poisson bracket

  friend JLCterm operator*( const JLCterm&, const JLCterm& );
  friend JetC operator*( const FNAL::Complex&, const Jet& );
  friend JetC operator*( const Jet&, const FNAL::Complex& );
  friend JetC operator*( const JetC&, const int& );
  friend JetC operator*( const int&, const JetC& );
  friend JetC operator*( const JetC&, const double& );
  friend JetC operator*( const double&, const JetC& );
  friend JetC operator/( const JetC& x, const int& j );

  // ??? REMOVE: friend LieOperator operator*( const LieOperator&, const LieOperator& );


  // Utility arithmetic functions ...
  void Negate();
  void Mult( const FNAL::Complex& );


  // Inversion ...
  JetC Inverse() const;       // Returns the inverse JetC
                             //  only when the number of
                             //  coordCinates = 1.

  // Overloaded analytic functions__________________________________
  friend JetC acos( const JetC& );
  friend JetC asin( const JetC& );
  friend JetC atan( const JetC& );
  //  friend JetC atan2( const JetC&, const JetC& );
  friend JetC cos( const JetC& );
  friend JetC cosh( const JetC& );
  friend JetC exp( const JetC& );
  friend JetC log( const JetC& );
  friend JetC log10( const JetC& );
  friend JetC pow( const JetC&, const double& );
  friend JetC pow( const JetC&, int );
  friend JetC sin( const JetC& );
  friend JetC sinh( const JetC& );
  friend JetC sqrt( const JetC& );
  friend JetC tan( const JetC& );
  friend JetC tanh( const JetC& );
  friend JetC erf( const JetC& );
  friend JetC erfc( const JetC& );
  friend JetC w( const JetC& );
  friend JetC fabs( const JetC& );


  static slist environments;   // environments is made public for
                               // convenience, but no user program
                               // should play around directly
                               // with it.

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void operator-=( JetC&, const JetC& );   // ??? Why are these void operators?
void operator*=( JetC&, const JetC& );   // ??? And why not member functions?
void operator/=( JetC&, const JetC& );
void operator/=( JetC&, const FNAL::Complex );
char operator!=( const JetC&, const JetC& );
char operator!=( const JetC&, const FNAL::Complex& );
char operator!=( const FNAL::Complex&, const JetC& );


JetC operator+( const FNAL::Complex&, const Jet& );
JetC operator+( const Jet&, const FNAL::Complex& );
JetC operator-( const FNAL::Complex&, const Jet& );
JetC operator-( const Jet&, const FNAL::Complex& );
JetC operator*( const FNAL::Complex&, const Jet& );
JetC operator*( const Jet&, const FNAL::Complex& );
JetC operator/( const FNAL::Complex&, const Jet& );
JetC operator/( const Jet&, const FNAL::Complex& );

// *******************************************************************

class coordC : public JetC {
private:
  int index;
public:
  coordC();
  coordC( FNAL::Complex = complex_0 );
  coordC( const coordC& );
  ~coordC();

  void    operator=( const FNAL::Complex& );
  coordC& operator=( const coordC& );
  coordC& operator=( const JetC& );

  inline int Index() { return index; }

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};


LieOperator operator*( const LieOperator&, const LieOperator& );

void normalForm( const Mapping&,    /* Input:   map                    */
                 int,               /* Input:   order                  */
                 MatrixC*,          /* Output:  matrix of eigenvectors */
                                    /*          of Jacobian of map     */
                 CLieOperator*,     /* Output:  normal form            */
                 CLieOperator* );   /* Output:  transformation         */




inline JetC__environment* JetC::Env() const
{
  return jl->myEnv;
}

inline void JetC::Env( JetC__environment* pje ) const
{
  pje = jl->myEnv;
}


inline int JetC::intEnv() const
{  
  return int( jl->myEnv );
}


inline int JetC::termCount() const
{
  return jl->count;
}


inline int JetC::getEnvNumVar() const 
{
  return jl->myEnv->NumVar;
}

inline int JetC::getEnvSpaceDim() const 
{
  return jl->myEnv->SpaceDim;
}

inline int JetC::getEnvMaxWeight() const 
{
  return jl->myEnv->MaxWeight;
}

inline int JetC::getWeight() const 
{
  return jl->weight;
}

inline int JetC::getAccuWgt() const 
{
  return jl->accuWgt;
}

inline char JetC::isNilpotent() const 
{
 return jl->isNilpotent();
}


#endif // JETC_HXX
