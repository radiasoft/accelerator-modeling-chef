/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      Jet.h
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


#ifndef JET_HXX
#define JET_HXX

#include "slist.h"
#include "dlist.h"
#include "Jet__environment.h"
#include "IntArray.h"
#include "VectorD.h"
#include "JL.h"    // Not strictly needed, but should not be
                   // necessary to invoke this from app'n.

struct JLterm;
struct JL;
struct JLC;
class  JetVector;
class  Mapping;
class  JetC;
class  LieOperator;

class Jet {
private:
  JL*  jl;
  dlist_iterator* constIterPtr;
  dlist_iterator* iterPtr;

  Jet EpsInverse() const;  // Calculates the inverse of
                           // nilpotent Jets.
  Jet concat() const;

  Jet TruncMult( const Jet&, const int& ) const;  // Used by division algorithm.

  // Friends
  // ??? REMOVE: friend class coord;   // ??? Is this necessary???
  // ??? REMOVE: friend class JetVector;
  // ??? REMOVE: friend class JetC;
  // ??? REMOVE: friend class LieOperator;  // ??? Necessary for LieOperator copy constructor
  // ??? REMOVE: friend class CLieOperator;

  // ??? REMOVE: friend void normalForm( const Mapping&,    /* Input:   map                    */
  // ??? REMOVE:                         int,               /* Input:   order                  */
  // ??? REMOVE:                         MatrixC*,          /* Output:  matrix of eigenvectors */
  // ??? REMOVE:                                            /*          of Jacobian of map     */
  // ??? REMOVE:                         CLieOperator*,     /* Output:  normal form            */
  // ??? REMOVE:                         CLieOperator* );   /* Output:  transformation         */
  // ??? REMOVE: 
  // ??? REMOVE: 

  // Utility functions; used by public analytic functions ...
  static Jet epsCos( const Jet& );
  static Jet epsExp( const Jet& );
  static Jet epsPow( const Jet&, const double& );
  static Jet epsSin( const Jet& );
  static Jet epsSqrt( const Jet& ); 


protected:
  // ??? REMOVE static dlist  coordPool;          
  static dlist coordPtrs;
  static slist newCoords;
  static slist newValues;
  static int   currentIndex;

  JL* operator->() const { return jl; }

public:
  static Jet__environment* lastEnv;  // G++ needs this here.

  // Constructors and destructors_____________________________________
  Jet( Jet__environment* = Jet::lastEnv );
  Jet( double, Jet__environment* = Jet::lastEnv );
  Jet( const Jet& );
  Jet( const JetC&  );
  Jet( const JetC&, /* const */ Jet__environment* );
  ~Jet();

  void Reconstruct();
  void Reconstruct( Jet__environment* pje );

  static Jet__environment* workEnv;  // This probably doesn't belong with Jet.
  static void BeginEnvironment( int = 1 /* maximum weight */ );
  static void Parameters();
  static Jet__environment* EndEnvironment( double* = 0 /* scale array    */ );
  static void EnlargeEnvironment( Jet__environment* );
  static Jet__environment* CreateEnvFrom( const JetC__environment* );

  void setEnvTo( const Jet& );              // Changes environment to
  void setEnvTo( const Jet__environment* ); // that of the argument.

  int intEnv() const;                       // Returns integer representation
                                            // of the environment pointer.
  Jet__environment* Env() const;            // ??? DANGER!!! These two break
  void Env( Jet__environment* pje ) const;  // ??? DANGER!!! protection!!


  JLC* newJLC() const;                      // Used by conversion routines.
  JLC* newJLC( /* const */ JetC__environment* ) const;

  // Obsolete setup routines -----------------------------------------
  // static void Setup( const int      = 6, /* Total number of variables */
  //                    const int      = 1, /* Maximum derivative weight */
  //                    const int      = 0, /* Dimension of phase space  */
  //                    const double*  = 0, /* Default reference point   */
  //                    const double*  = 0  /* Scaling array             */
  //                  );
  // 
  // static void setup( const int, const int );
  // static void setup( const int, const int, const double* );
  // static void setup( const int, const int, const int );
  // static void setup( const int, const int, const int, const double* );
  // ------------------------------------------------------------------

  // ??? REMOVE static void FixReference        ( const double* );
  // ??? REMOVE static void FixReference        ( const int* );
  // ??? REMOVE static void FixReference        ( const Jet& );
  // ??? REMOVE static void FixReferenceAtStart ( const LieOperator& );
  // ??? REMOVE static void FixReferenceAtEnd   ( const LieOperator& );

  // ??? REMOVE: static FILE  *scratchFile;


  // Public member functions__________________________________________
  JLterm* get();                // Pops the top term, which should be the 
                                // one of lowest weight.
  JLterm  firstTerm() const;    // Returns a JLterm equivalent to the top term,
                                // which should be the one of lowest weight.
  JLterm  lowTerm()   const;    // Returns a JLterm equivalent to the 
                                // non-zero term of lowest weight.
  void addTerm( JLterm* );      // Public only for diagnostic purposes.

  void    resetConstIterator();
  JLterm  stepConstIterator()  const;
  void    resetIterator();
  JLterm* stepIterator();

  // ??? REMOVE void fixReference();
  // ??? REMOVE void fixReference( const double* );
  // ??? REMOVE void fixReference( const Jet& );
  // ??? REMOVE void fixReferenceAtEnd( const LieOperator& );
  // ??? REMOVE void fixReferenceAtStart( const LieOperator& );

  char isNilpotent() const;

  void peekAt() const;
  void printCoeffs() const;
  void peekAt( int ) const;       // Prints to Jet::scratchFile
  void printCoeffs( int ) const;
  int  termCount() const;         // Returns number of monomial terms

  // ??? REMOVE: dlist image() const;
  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

  void getReference( double* ) const;
  int  getEnvNumVar() const;
  int  getEnvSpaceDim() const;
  int  getEnvMaxWeight() const;

  int  getWeight() const;
  int  getAccuWgt() const;

  void scaleBy( double );

  void setVariable( const double&, const int&, Jet__environment* );
               // Alters the environment
  void setVariable( const double&, const int& );
               // Alters its environment
  void setVariable( const int&, Jet__environment* );
               // Does not alter the environment.
  void setVariable( const int& );
               // Does not alter its environment.

  double standardPart() const;
  void clear();
  double weightedDerivative( const int* ) const;
  double derivative( const int* ) const;
  Jet filter(  const int&, const int& ) const;  
                                  // Returns only those JLterms whose weight 
          	                  // are between two specified values, inclusive.
  Jet filter( char (*)( const IntArray&, const double& ) ) const; 
                                   // Returns those JLterms for which the 
                                   // argument is satisfied.
  double operator() ( const Vector& ) const;
  double operator() ( const double* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the Jet variable.  Essentially acts as a 
				   // power series expansion.
  Jet  operator() ( const Jet* ) const;  
                                   // Self explanatory ...
  Jet  operator() ( /* const */ JetVector& ) const;  // Self explanatory ...
  // ??? REMOVE Jet  operator() ( const LieOperator& ) const; 
  // ??? REMOVE                                  // Self explanatory ...
  Jet  D( const int* ) const ;	      // Performs differentiation of a Jet variable.
  Jet  D( const IntArray& ) const ;   // Performs differentiation of a Jet variable.

  // Operators________________________________________________________
  friend ostream& operator<<(ostream&, const Jet&);
  friend istream& operator>>(istream&, Jet&);
  friend char operator==( const Jet&, const Jet& );
  friend char operator==( const Jet&, const double& );
  friend char operator==( const double&, const Jet& );

  Jet& operator=( const Jet& );
  Jet& operator=( const JetC& );
  Jet& operator=( const double& );
  Jet& DeepCopy( const Jet& );
  void operator+=( const double& );
  void operator-=( const double& );

  void operator+=( const Jet& );   // ??? Why are these void operators?

  friend Jet operator+( const Jet&, const Jet& );
  friend Jet operator+( const Jet&, const double& );
  friend Jet operator+( const double&, const Jet& );
  friend Jet operator-( const Jet&, const Jet& );
  friend Jet operator-( const Jet& );
  friend Jet operator-( const Jet&, const double& );
  friend Jet operator-( const double&, const Jet& );
  friend Jet operator*( const Jet&, const Jet& );
  friend Jet operator*( const Jet&, const double& );
  friend Jet operator*( const double&, const Jet& );  // ??? Change this!!
  friend Jet operator/( const Jet&, const Jet& );     // ??? Friendliness
  friend Jet operator/( const Jet&, const double& );  // ??? unnecessary
  friend Jet operator/( const double&, const Jet& ); 
  friend Jet operator^( const Jet&, const Jet& );   // Poisson bracket

  friend JetC operator*( const Complex&, const Jet& );
  friend JetC operator*( const Jet&, const Complex& );

  friend JLterm operator*( const JLterm&, const JLterm& );
  friend Jet operator*( const Jet&, const int& );
  friend Jet operator*( const int&, const Jet& );
  friend Jet operator/( const Jet&, const int& ); 

  // ??? REMOVE: friend LieOperator operator*( const LieOperator&, const LieOperator& );

  // Utility arithmetic functions ...
  void Negate();
  void Mult( const double& );


  // Inversion ...
  Jet Inverse() const;       // Returns the inverse jet
                             //  only when the number of
                             //  coordinates = 1.

  // Overloaded analytic functions__________________________________
  friend Jet acos( const Jet& );
  friend Jet asin( const Jet& );
  friend Jet atan( const Jet& );
  //  friend Jet atan2( const Jet&, const Jet& );
  friend Jet cos( const Jet& );
  friend Jet cosh( const Jet& );
  friend Jet exp( const Jet& );
  friend Jet log( const Jet& );
  friend Jet log10( const Jet& );
  friend Jet pow( const Jet&, const double& );
  friend Jet pow( const Jet&, int );
  friend Jet sin( const Jet& );
  friend Jet sinh( const Jet& );
  friend Jet sqrt( const Jet& );
  friend Jet tan( const Jet& );
  friend Jet tanh( const Jet& );
  friend Jet erf( const Jet& );
  friend Jet erfc( const Jet& );

  // ??? REMOVE friend Jet w( const Jet& );
  friend Jet fabs( const Jet& );

  friend Jet real( const JetC & );
  friend Jet imag( const JetC & );


  static slist environments;   // environments is made public for
                               // convenience, but no user program
                               // should play around directly
                               // with it.

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void operator-=( Jet&, const Jet& );   // ??? Why are these void operators?
void operator*=( Jet&, const Jet& );   // ??? And why not member functions?
void operator/=( Jet&, const Jet& );
void operator/=( Jet&, const double& );
char operator!=( const Jet&, const Jet& );
char operator!=( const Jet&, const double& );
char operator!=( const double&, const Jet& );

// *******************************************************************

class coord : public Jet {
private:
  int index;
public:
  coord( double = 0.0 );
  coord( const coord& );
  ~coord();

  void   operator=( const double& );
  coord& operator=( const coord& );
  coord& operator=( const Jet& );

  inline int Index() { return index; }

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};



LieOperator operator*( const LieOperator&, const LieOperator& );



inline Jet__environment* Jet::Env() const
{
  return jl->myEnv;
}

inline void Jet::Env( Jet__environment* pje ) const
{
  pje = jl->myEnv;
}


inline int Jet::intEnv() const
{  
  return int( jl->myEnv );
}


inline int Jet::termCount() const
{
  return jl->count;
}

inline int Jet::getEnvNumVar() const 
{
  return jl->myEnv->NumVar;
}

inline int Jet::getEnvSpaceDim() const 
{
  return jl->myEnv->SpaceDim;
}

inline int Jet::getEnvMaxWeight() const 
{
  return jl->myEnv->MaxWeight;
}

inline int Jet::getWeight() const 
{
  return jl->weight;
}

inline int Jet::getAccuWgt() const 
{
  return jl->accuWgt;
}

inline char Jet::isNilpotent() const 
{
 return jl->isNilpotent();
}

#endif // JET_HXX
