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
class  JetVector;
class  Mapping;

class Jet {
private:
  JL*  jl;

  Jet EpsInverse() const;  // Calculates the inverse of
                           // nilpotent Jets.
  Jet concat() const;

  Jet TruncMult( const Jet&, const int& );  // Used by division algorithm.

  // Friends
  friend class coord;   // ??? Is this necessary???
  friend class JetVector;
  friend class JetC;
  friend class LieOperator;  // ??? Necessary for LieOperator copy constructor
  friend class CLieOperator;

  friend void normalForm( const Mapping&,    /* Input:   map                    */
                          int,               /* Input:   order                  */
                          MatrixC*,          /* Output:  matrix of eigenvectors */
                                             /*          of Jacobian of map     */
                          CLieOperator*,     /* Output:  normal form            */
                          CLieOperator* );   /* Output:  transformation         */


  friend Jet real( const JetC & );
  friend Jet imag( const JetC & );

  // ??? REMOVE static dlist  coordPool;          
  static dlist  coordPtrs;          

  static slist             newCoords;
  static slist             newValues;
  static int               currentIndex;

public:
  char        stacked;    // ??? Made public for convenience: no application
                          // ??? should touch this variable.
  static Jet__environment* lastEnv;  // G++ needs this here.

  // Constructors and destructors_____________________________________
  Jet( Jet__environment* = Jet::lastEnv );
  Jet( const double&, Jet__environment* = Jet::lastEnv );
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

  Jet__environment* Env() const;            // ??? These two break
  void Env( Jet__environment* pje ) const;  // ??? protection!!

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

  static FILE  *scratchFile;

  // Public member functions__________________________________________
  JL* operator->() const { return jl; }

  JLterm* get();            // Pops the top term, which should be the 
                            // one of lowest weight.
  void addTerm( JLterm* );  // Public only for diagnostic purposes.

  // ??? REMOVE void fixReference();
  // ??? REMOVE void fixReference( const double* );
  // ??? REMOVE void fixReference( const Jet& );
  // ??? REMOVE void fixReferenceAtEnd( const LieOperator& );
  // ??? REMOVE void fixReferenceAtStart( const LieOperator& );

  char IsNilpotent() const;

  void peekAt() const;
  void printCoeffs() const;
  void peekAt( int ) const;       // Prints to Jet::scratchFile
  void printCoeffs( int ) const;

  dlist image();
  void writeToFile( char*   /* Name of unopened file */ );
  void writeToFile( FILE* );

  void getReference( double* );
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
  double weightedDerivative( int* );
  double derivative( int* );
  Jet filter(  const int&, const int& ) const;  
                                  // Returns only those JLterms whose weight 
          	                  // are between two specified values, inclusive.
  Jet filter( char (*)( const IntArray&, const double& ) ); 
                                   // Returns those JLterms for which the 
                                   // argument is satisfied.
  double operator() ( const Vector& ) const;
  double operator() ( double* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the Jet variable.  Essentially acts as a 
				   // power series expansion.
  Jet  operator() ( Jet* ) const;  // Self explanatory ...
  Jet  operator() ( /* const */ JetVector& ) const;  // Self explanatory ...
  // ??? REMOVE Jet  operator() ( const LieOperator& ) const; 
  // ??? REMOVE                                  // Self explanatory ...
  Jet  D( int* )const ;		   // Performs differentiation of a Jet variable.
  Jet  D( IntArray& )const ;	   // Performs differentiation of a Jet variable.

  // Operators________________________________________________________
  friend ostream& operator<<(ostream&, Jet&);
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

  friend JLterm operator*( JLterm&, JLterm& );
  friend Jet operator*( const Jet&, const int& );
  friend Jet operator*( const int&, const Jet& );
  friend Jet operator/( const Jet&, const int& ); 

  friend LieOperator operator*( const LieOperator&, const LieOperator& );


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

#endif // Jet_HXX
