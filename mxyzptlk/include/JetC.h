#ifndef JETC_HXX
#define JETC_HXX

#include "slist.h"
#include "dlist.h"
#include "JetC__environment.h"
#include "IntArray.h"
#include "JLC.h"    // Not strictly needed, but should not be
                    // necessary to invoke this from app'n.

struct JLCterm;
struct JLC;
class  JetCVector;
class  Jet;
class  Mapping;

class JetC {

private:
  JLC*  jl;

  JetC EpsInverse() const;  // Calculates the inverse of
                           // nilpotent JetCs.
  JetC concat() const;

  JetC TruncMult( const JetC&, const int& );  // Used by division algorithm.

  // Friends
  friend class coordC;   // ??? Is this necessary???
  friend class JetCVector;
  friend class Jet;
  friend class LieOperator;  // ??? Necessary for LieOperator copy constructor
  friend class CLieOperator;

  friend Jet real( const JetC & );
  friend Jet imag( const JetC & );

  friend void normalForm( const Mapping&,    /* Input:   map                    */
                          int,               /* Input:   order                  */
                          MatrixC*,          /* Output:  matrix of eigenvectors */
                                             /*          of Jacobian of map     */
                          CLieOperator*,     /* Output:  normal form            */
                          CLieOperator* );   /* Output:  transformation         */


  // ??? REMOVE static dlist  coordCPool;          
  static dlist  coordCPtrs;          

  static slist             newCoordCs;
  static slist             newValues;
  static int               currentIndex;

public:
  char        stacked;    // ??? Made public for convenience: no application
                          // ??? should touch this variable.
  static JetC__environment* lastEnv;

  // Constructors and destructors_____________________________________
  JetC( JetC__environment* = JetC::lastEnv );
  JetC( const Complex&, JetC__environment* = JetC::lastEnv );
  JetC( const JetC& );
  JetC( const Jet&  );
  JetC( const Jet&, /* const */ JetC__environment* );
  ~JetC();

  void Reconstruct();
  void Reconstruct( JetC__environment* pje );

  static JetC__environment* workEnv;  // This probably doesn't belong with JetC.
  static void BeginEnvironment( int = 1 /* maximum weight */ );
  static void Parameters();
  static JetC__environment* EndEnvironment( Complex* = 0 /* scale array    */ );
  static void EnlargeEnvironment( JetC__environment* );
  static JetC__environment* CreateEnvFrom( const Jet__environment* );

  JetC__environment* Env() const;            // ??? These two break
  void Env( JetC__environment* pje ) const;  // ??? protection!!

  // Obsolete setup routines -----------------------------------------
  // static void Setup( const int      = 6, /* Total number of variables */
  //                    const int      = 1, /* Maximum derivative weight */
  //                    const int      = 0, /* Dimension of phase space  */
  //                    const Complex*  = 0, /* Default reference point   */
  //                    const Complex*  = 0  /* Scaling array             */
  //                  );
  // 
  // static void setup( const int, const int );
  // static void setup( const int, const int, const Complex* );
  // static void setup( const int, const int, const int );
  // static void setup( const int, const int, const int, const Complex* );
  // ------------------------------------------------------------------

  // ??? REMOVE static void FixReference        ( const Complex* );
  // ??? REMOVE static void FixReference        ( const int* );
  // ??? REMOVE static void FixReference        ( const JetC& );
  // ??? REMOVE static void FixReferenceAtStart ( const LieOperator& );
  // ??? REMOVE static void FixReferenceAtEnd   ( const LieOperator& );

  static FILE  *scratchFile;

  // Public member functions__________________________________________
  JLC* operator->() const { return jl; }

  JLCterm* get();            // Pops the top term, which should be the 
                            // one of lowest weight.
  void addTerm( JLCterm* );  // Public only for diagnostic purposes.

  // ??? REMOVE void fixReference();
  // ??? REMOVE void fixReference( const Complex* );
  // ??? REMOVE void fixReference( const JetC& );
  // ??? REMOVE void fixReferenceAtEnd( const LieOperator& );
  // ??? REMOVE void fixReferenceAtStart( const LieOperator& );

  char IsNilpotent() const;

  void peekAt() const;
  void printCoeffs() const;
  void peekAt( int ) const;       // Prints to JetC::scratchFile
  void printCoeffs( int ) const;

  dlist image();
  void writeToFile( char*   /* Name of unopened file */ );
  void writeToFile( FILE* );

  void getReference( Complex* );
  void scaleBy( Complex );

  void setVariable( const Complex&, const int&, JetC__environment* );
               // Alters the environment
  void setVariable( const int&, JetC__environment* );
               // Does not alter the environment.

  Complex standardPart() const;
  void clear();
  Complex weightedDerivative( int* );
  Complex derivative( int* );
  JetC filter(  const int&, const int& ) const;  
                                  // Returns only those JLCterms whose weight 
          	                  // are between two specified values, inclusive.
  JetC filter( char (*)( const IntArray&, const Complex& ) ); 
                                   // Returns those JLCterms for which the 
                                   // argument is satisfied.
  Complex operator() ( Complex* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the JetC variable.  Essentially acts as a 
				   // power series expansion.
  JetC  operator() ( JetC* ) const;  // Self explanatory ...
  JetC  operator() ( /* const */ JetCVector& ) const;  // Self explanatory ...
  // ??? REMOVE JetC  operator() ( const LieOperator& ) const; 
  // ??? REMOVE                                  // Self explanatory ...
  JetC  D( int* )const ;		   // Performs differentiation of a JetC variable.
  JetC  D( IntArray& )const ;	   // Performs differentiation of a JetC variable.

  // Operators________________________________________________________
  friend ostream& operator<<(ostream&, JetC&);
  friend istream& operator>>(istream&, JetC&);
  friend char operator==( const JetC&, const JetC& );
  friend char operator==( const JetC&, const Complex& );
  friend char operator==( const Complex&, const JetC& );

  JetC& operator=( const JetC& );
  JetC& operator=( const Jet&  );
  JetC& operator=( const Complex& );
  JetC& DeepCopy( const JetC& );
  void operator+=( const Complex& );
  void operator-=( const Complex& );

  void operator+=( const JetC& );   // ??? Why are these void operators?

  friend JetC operator+( const JetC&, const JetC& );
  friend JetC operator+( const JetC&, const Jet&  );
  friend JetC operator+( const Jet&,  const JetC& );
  friend JetC operator+( const JetC&, const Complex& );
  friend JetC operator+( const Complex&, const JetC& );
  friend JetC operator+( const JetC&, const double& );
  friend JetC operator+( const double&, const JetC& );

  friend JetC operator-( const JetC&, const JetC& );
  friend JetC operator-( const Jet&,  const JetC& );
  friend JetC operator-( const JetC&, const Jet&  );
  friend JetC operator-( const JetC& );
  friend JetC operator-( const JetC&, const Complex& );
  friend JetC operator-( const Complex&, const JetC& );
  friend JetC operator-( const JetC&, const double& );
  friend JetC operator-( const double&, const JetC& );

  friend JetC operator*( const JetC&, const JetC& );
  friend JetC operator*( const JetC&, const Jet& );
  friend JetC operator*( const Jet&,  const JetC& );
  friend JetC operator*( const JetC&, const Complex& );
  friend JetC operator*( const Complex&, const JetC& );  // ??? Change this!!

  friend JetC operator/( const JetC&, const JetC& );     // ??? Friendliness
  friend JetC operator/( const Jet&,  const JetC& );     // ??? unnecessary
  friend JetC operator/( const JetC&, const Jet&  );     
  friend JetC operator/( const JetC&, const Complex& );  
  friend JetC operator/( const Complex&, const JetC& ); 
  friend JetC operator/( const JetC&, const double& );  
  friend JetC operator/( const double&, const JetC& ); 

  friend JetC operator^( const JetC&, const JetC& );   // Poisson bracket

  friend JLCterm operator*( JLCterm&, JLCterm& );
  friend JetC operator*( const Complex&, const Jet& );
  friend JetC operator*( const Jet&, const Complex& );
  friend JetC operator*( const JetC&, const int& );
  friend JetC operator*( const int&, const JetC& );
  friend JetC operator*( const JetC&, const double& );
  friend JetC operator*( const double&, const JetC& );
  friend JetC operator/( const JetC& x, const int& j );

  friend LieOperator operator*( const LieOperator&, const LieOperator& );


  // Utility arithmetic functions ...
  void Negate();
  void Mult( const Complex& );


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
void operator/=( JetC&, const Complex );
char operator!=( const JetC&, const JetC& );
char operator!=( const JetC&, const Complex& );
char operator!=( const Complex&, const JetC& );


JetC operator+( const Complex&, const Jet& );
JetC operator+( const Jet&, const Complex& );
JetC operator-( const Complex&, const Jet& );
JetC operator-( const Jet&, const Complex& );
JetC operator*( const Complex&, const Jet& );
JetC operator*( const Jet&, const Complex& );
JetC operator/( const Complex&, const Jet& );
JetC operator/( const Jet&, const Complex& );

// *******************************************************************

class coordC : public JetC {
private:
  int index;
public:
  coordC();
  coordC( Complex = 0.0 );
  coordC( const coordC& );
  ~coordC();

  void    operator=( const Complex& );
  coordC& operator=( const coordC& );
  coordC& operator=( const JetC& );

  inline int Index() { return index; }

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

#endif 
