/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      bmlnElmnt.h
******  Version:   2.2
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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


#ifndef BMLNELMNT_H
#define BMLNELMNT_H

#include <string>
#include <iostream>
#include <exception>

#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "BmlVisitor.h"

class ParticleBunch;
class Aperture;
class Particle;

template<typename T1, typename T2> 
class TJet;
typedef TJet<double,FNAL::Complex> Jet;
// class Jet;

template<typename T1, typename T2> 
class TJetVector;
typedef TJetVector<double,FNAL::Complex> JetVector;
// class JetVector;

class JetParticle;
class InsertionList;
class sector;

// Forward declaration required
class bmlnElmnt;
class beamline;

#define BF_MAXCHAR 8
#define NOTKNOWN   -123456.789
#define OSTREAM_DOUBLE_PREC setprecision(20)

struct lattRing : public BarnacleData {
  struct {
    double hor;
    double ver;
  } tune;
  struct {
    double hor;
    double ver;
  } chromaticity;

  lattRing();
  ~lattRing() {}
  lattRing& operator=( const lattRing& );
} ;
 std::ostream& operator<<(std::ostream&, const lattRing&);

struct lattFunc : public BarnacleData {
  double arcLength;
  struct {
    double hor;
    double ver;
  } dispersion;
  struct {
    double hor;
    double ver;
  } dPrime;
  struct {
    double hor;
    double ver;
  } beta;
  struct {
    double hor;
    double ver;
  } alpha;
  struct {
    double hor;
    double ver;
  } psi;

  lattFunc();
  ~lattFunc() {}
  lattFunc& operator=( const lattFunc& );
} ;

std::ostream& operator<<(std::ostream&, const lattFunc&);
std::istream& operator>>(std::istream&, lattFunc&);

struct BMLN_posInfo {
    Vector inPoint;      // This struct defines the upstream and
    Vector inAxes  [3];  // downstream faces of the magnets slot.
    Vector outPoint;
    Vector outAxes [3];
    BMLN_posInfo( );
    BMLN_posInfo( const BMLN_posInfo& );
    BMLN_posInfo& operator=( const BMLN_posInfo& );   // ??? Does this struct
                                                      // ??? need a destructor?
    friend std::ostream& operator<<(std::ostream&, BMLN_posInfo&);

} ;


struct alignmentData {
  double       xOffset;
  double       yOffset;
  double       tilt;
  alignmentData();
  alignmentData(const alignmentData&);
  ~alignmentData();
  alignmentData& operator=(const alignmentData&);
  bool operator==(const alignmentData&) const;
};


class alignment {
private:
  double       xOffset;         // offset in meters
  double       yOffset;         // offset in meters
  double       tilt;            // roll in radians
                                // we could put in pitch someday
  double       cosTilt;         // cos(tilt)
  double       sinTilt;         // sin(tilt)
public:
  alignment();
  alignment( double, // xOffset
             double, // yOffset
             double  // roll (=tilt)
           );
  alignment(const alignment&);
  alignment(const alignmentData&);
  ~alignment();

  bool isNull() const;
  bool operator==( const alignment& ) const;

  alignment& operator=(const alignment&);
                                // these methods will overwrite an array
                                // BMLM_dynDim in length
                                // I don\'t think that the Particle or
                                // JetParticle should be modified directly
  void align(const Particle&, BMLN_posInfo, double*);
  void align(double*, BMLN_posInfo, double*);
  void misalign(const Particle&, BMLN_posInfo, double*);
  void align(JetParticle&, BMLN_posInfo, Jet*);
  void align(Jet*, BMLN_posInfo, Jet*);
  void misalign(JetParticle&, BMLN_posInfo, Jet*);
  void misalign( double* );
  void align( double* );
  void misalign( JetVector& );
  void align( JetVector& );

  void setAlignment(const alignmentData&);
  alignmentData getAlignment() const;

  double x_offset() { return xOffset; }
  double y_offset() { return yOffset; }
  double roll()     { return tilt;    }
  double cos_roll() { return cosTilt; }
  double sin_roll() { return sinTilt; }

  friend std::ostream& operator<<(std::ostream&, alignment&);
};


struct bmlnElmntData {
  char            type[80];
  bmlnElmnt*      address;
  char*           name;
  int             depth;
  double          length;
  double          strength;

    //
    // O.K.
    //
    //  double          aperture;

  Aperture*       pAperture;
  
  alignmentData   align;
  BMLN_posInfo    geometry;
  double          iToField;
  double	  shuntCurrent;
  char            more;

  bmlnElmntData();
  bmlnElmntData( bmlnElmntData& );
  virtual ~bmlnElmntData();
  virtual void eliminate();
  virtual void* clone();
};



class bmlnElmnt
{
public:

  // AsinFunctor is a utility class used by sbend and rbend.
  class AsinFunctor
  {
  public:
    AsinFunctor( bool = false /* i.e. approximate */, int = 1 );
    AsinFunctor( const AsinFunctor& );
    ~AsinFunctor() {}
    double   operator()( double x ) const;
    Jet      operator()( const Jet& x ) const;
    int      setNumTerms( int );
    int      getNumTerms();
    bool     isExact();
    bool     makeExact();
    bool     makeApproximate();
  private: 
    bool                    _exactMode;
    int                     _n;
    static   const double   _coeff[];
    static   const int      _size;
  };


  class PropFunc
  {
    public:
      virtual int operator()( bmlnElmnt*, Particle&    ) = 0;
      virtual int operator()( bmlnElmnt*, JetParticle& ) = 0;
      virtual const char* Type() const                   = 0;
  };

  typedef char (*CRITFUNC)( bmlnElmnt* );
  struct Discriminator
  {
    Discriminator();   // Needed by constructors of derived classes.
    Discriminator( const Discriminator& );  // Aborts program.
    virtual ~Discriminator() {}
    virtual bool operator()( const bmlnElmnt* ) const = 0;
    virtual Discriminator* Clone() const = 0;
    virtual void writeTo( std::ostream& ) const = 0;
  };

  static const short BF_OK;
  static const short BF_NULL_ARG;
  static const short BF_BAD_START;

  // Exceptions
  struct GenericException : public std::exception
  {
    GenericException( std::string, int, const char* = "", const char* = "" );
    // Miscellaneous errors
    // 1st argument: name of file in which exception is thrown
    // 2nd         : line from which exception is thrown
    // 3rd         : identifies function containing throw
    // 4th         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };

protected:
  char*        ident;      // Name identifier of the element.
  double       length;     // Length of object [ meters ]
  double       strength;   // Interpretation depends on object.
  int          depth;      // Depth of objects below this element.
  
  alignment*   align;
  double       iToField;   // Conversion factor for current through
                           // magnet in amperes to field or gradient etc.
  double       shuntCurrent; // Does this element have a shunt?
  BMLN_posInfo geometry;

  beamline*    p_bml;      // The element may be composite.
  bmlnElmnt*   p_bml_e;    // with one active part.

  PropFunc*    Propagator;

  virtual void geomToEnd   ( BMLN_posInfo& );
  virtual void geomToStart ( BMLN_posInfo& );
  virtual void eliminate();

  friend class beamline;
  // friend class circuit;
  friend beamline& operator-( beamline& );

  double _ctRef;  // (normalized) time required for
                  // a reference particle to cross
                  // the element. Established by a
                  // RefRegVisitor.


public:
  bmlnElmnt( const char*   n = "NONAME" /* name     */,
             PropFunc*       = 0 
           );		   
  bmlnElmnt( double  l       /* length   */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( double  l       /* length   */,
             double  s       /* strength */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             double        l /* length   */,
             PropFunc*       = 0
           );		   
  bmlnElmnt( const char*   n /* name     */,
             double        l /* length   */,
             double        s /* strength */,
             PropFunc*       = 0
           );
  bmlnElmnt( const bmlnElmnt&  );
  bmlnElmnt( bmlnElmntData& );

  virtual ~bmlnElmnt();

  // ----------------------------------------------
  Aperture*    pAperture;  // O.K.
  
  BarnacleList dataHook;   // Carries data as service to application program.
  // ??? REMOVE:  lattFunc     lattInfo;   // Information on maps and lattice functions.
  bmlnElmnt& operator=( const bmlnElmnt& );

  virtual void accept( BmlVisitor& ) = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  PropFunc* setPropFunction ( const PropFunc* a );  // return previous
  PropFunc* setPropFunction ( const PropFunc& a );  // Propagator
  PropFunc* getPropFunction() { return Propagator; }

  void propagate( Particle& );
  void propagate( JetParticle& );
  void propagate( ParticleBunch& );

  virtual void localPropagate( Particle& );
  virtual void localPropagate( JetParticle& );
  virtual void localPropagate( ParticleBunch& );

  virtual void enterLocalFrame( Particle&                ) const;
  virtual void enterLocalFrame( JetParticle&             ) const;
  // ??? To do (?): virtual void enterLocalFrame( double*                  ) const;
  // ??? To do (?): virtual void enterLocalFrame( JetVector&               ) const;
  // ??? To do (?): virtual void enterLocalFrame( const Particle&, double* ) const;
  // ??? To do (?): virtual void enterLocalFrame( const JetParticle&, Jet* ) const;

  virtual void leaveLocalFrame( Particle&                ) const;
  virtual void leaveLocalFrame( JetParticle&             ) const;
  // ??? To do (?): virtual void leaveLocalFrame( double*                  ) const;
  // ??? To do (?): virtual void leaveLocalFrame( JetVector&               ) const;
  // ??? To do (?): virtual void leaveLocalFrame( const Particle&, double* ) const;
  // ??? To do (?): virtual void leaveLocalFrame( const JetParticle&, Jet* ) const;

  // Editing functions
  virtual bmlnElmnt* Clone() const = 0;  
  bmlnElmnt* clone() const;
                                       // Each type of bmlnElmnt will
                                       // create a copy of itself and
                                       // return a pointer to the new
                                       // object.
  bmlnElmnt* Clone(const char* name);  // Clone it and change the name.
  bmlnElmnt* clone(const char* name);

  virtual void Split( double s, bmlnElmnt**, bmlnElmnt** );
                                   // Splits the element at percent orbitlength
                                   // s from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.

  virtual void peekAt( double& s, Particle* = 0 );
  virtual bool equivTo( const bmlnElmnt& ) const;
  virtual bool equivTo( const bmlnElmnt* ) const;

  short  writeTag ( char,          // character to be written
                    short = 0      // position in tag
                  );
  short  writeTag ( const char*,   // characters to be written
                    short,         // starting position in tag
                    short          // number of characters 
                  );
  short  writeTag ( const char* );

  short  writeTag ( const std::string&,
                    short,         // starting position in tag
                    short          // number of characters 
                  );
  short  writeTag ( const std::string& );

  short  readTag  ( char*,         // returned characters
                    short,         // starting position in tag
                    short          // number of characters
                  );
  short  readTag  ( char* );

  char   readTag  ( short          // position in tag
                  );    
  std::string readTag  ( 
              short,         // starting position in tag
              short          // number of characters 
                  );
  std::string readTag  ();

  short  getTagSize() 
    { return BF_MAXCHAR; }

  virtual void setLength     ( double );
  virtual void setStrength   ( double );
  virtual void setStrength   ( double, int );  // for JetQuad
  virtual void setCurrent    ( double );
  virtual void setAlignment  ( const alignmentData& );
          void set           ( const bmlnElmntData& );
          void setAperture   ( Aperture* );
          void Rename        ( const char* );
  void setShunt(double a);


  // REMOVE: virtual double getReferenceTime() const {return _ctRef;}
  virtual double getReferenceTime() const;
  virtual double setReferenceTime( const Particle& );  // returns _ctRef
  virtual double setReferenceTime( double );  // returns previous _ctRef


  // Query functions ...
  alignmentData  Alignment( void ) const;
  Aperture*      getAperture( void );	// returns a clone of the aperture class
  inline int     hasAperture( void ) const { return  ( pAperture ? 1 : 0 ); }
  inline double  Strength() 	const { return strength; }
  double  Length() const;
  virtual double Current() 	const { return strength/iToField; }
  inline char    IsYourName( const char* x ) { return strcmp( x, ident ); }
  char hasName( const char* x ) const
    { return ( 0 == strcmp( ident, x ) ); }
  virtual const char*  Name() const
    { return ident; }
  virtual const char*  Type() const = 0;
  virtual int    isType(char* s) { return strcmp(s, "bmlnElmnt") == 0; }


  void setFlavor( const std::string& x )
    { flavor = x; }
  void setFlavor( const char* x )
    { flavor = x; }
  std::string Flavor()
    { return flavor; }
  std::string getFlavor()
    { return flavor; }
  std::string FlavorOrType()
    { if( flavor.length() == 0 ) return std::string(this->Type());
      else                       return flavor;
    }
  std::string getFlavorOrType()
    { if( flavor.length() == 0 ) return std::string(this->Type());
      else                       return flavor;
    }


  virtual double OrbitLength( const Particle& ) { return length; }
                                   // Returns length of design orbit
                                   // segment through the element.
                                   // Will be different from "length"
                                   // for rbends.
  double& IToField() 		{ return iToField; }
  double  getShunt() const	{ return shuntCurrent; }

  #ifdef OBJECT_DEBUG
  static int objectCount;
  #endif

private:
  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */
  virtual std::ostream& writeTo(std::ostream& os) 	{ return os; }
  virtual std::istream& readFrom(std::istream& is)	{ return is; }
  friend std::ostream& operator<<(std::ostream&, bmlnElmnt&);
  friend bmlnElmnt* read_istream(std::istream&);

  std::string flavor;     // Allows for "flavors" of types of elements.
  char         _tag[ BF_MAXCHAR ];
};



class BmlPtrList : private dlist
{
  //  A list of pointers to instances of bmlnElmnt objects.
  //  The objects are not owned by the list; their memory
  //    allocation must be handled elsewhere.
  //  The objects may be changed by users of the list.

public:
  BmlPtrList();
  BmlPtrList( const BmlPtrList& );
  ~BmlPtrList();

  BmlPtrList& operator=( const BmlPtrList& );

  void append( bmlnElmnt& );
  void append( bmlnElmnt* );
  void insert( bmlnElmnt& );
  void insert( bmlnElmnt* );

  bmlnElmnt* get();   // Iterator; removes elements from list
  bmlnElmnt* operator()( const int& ) const;

  int size() const;
  void clear();   // Preserves the bmlnElmnts
};


class beamline : public bmlnElmnt, public dlist
{
public:
  enum LineMode { line, ring, unknown };

  class arrayRep
  {
  public:
    arrayRep( const beamline*, bool = false );  
    // Clones all elements if 2nd argument is true
    ~arrayRep();
    int size() { return _n; }
    bmlnElmnt* e( int i ) { return _element[i]; }
    // WARNING: For speed, this is made unsafe.
    // User is responsible for staying in bounds.

  private:
    int         _n;
    bmlnElmnt** _element;
    bool        _cloned;
  };

  struct Criterion
  {
    virtual bool operator()( const bmlnElmnt* );
    virtual bool operator()( const bmlnElmnt& );
  };
  // Returns true if element matches the derived 
  // criterion; false, if not.
  struct Aye : Criterion {
    bool operator()( const bmlnElmnt* ) { return true; }
    bool operator()( const bmlnElmnt& ) { return true; }
  };
  struct Nay : Criterion {
    bool operator()( const bmlnElmnt* ) { return false; }
    bool operator()( const bmlnElmnt& ) { return false; }
  };
  static Aye yes;
  static Nay no;

  struct Action
  {
    virtual int operator()( bmlnElmnt* );
    virtual int operator()( bmlnElmnt& );
  };
  // Derived classes should return 0 if the action is 
  // performed successfully on the element. All other
  // values indicate error.


private:
  double            nominalEnergy;    // In GeV
  int               numElem;          // Number of elements in the beamline
  char              twissDone;
  std::ostream& writeTo(std::ostream&);
  // ??? REMOVE: istream& readFrom(istream&);
  friend std::istream& operator>>( std::istream&, beamline& );
  LineMode          _mode;

public:

  // CONSTRUCTORS AND DESTRUCTOR

  beamline( const char* nm = "NONAME" );
  beamline( bmlnElmnt* );
  beamline( const beamline& );
  beamline( char*, bmlnElmnt* );
  beamline( char*, beamline* );
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline(); 	       	       	       // Notice that this destructor does not destroy
                                       // the beamline elements.  To do that, use
                                       // beamline::zap().
  void zap();                          // Destroys the beamline elements
                                       // without destroying the beamline
  void eliminate();                    // Combines zap and ~beamline
  void clear();                        // Returns state to empty beamline.



  // EDITING LATTICE

  void insert( bmlnElmnt& );
  void insert( bmlnElmnt* );
  void append( bmlnElmnt& );
  void append( bmlnElmnt* );

  void Split( double, bmlnElmnt**, bmlnElmnt** );

  void InsertElementAt( double s_0, double s, const bmlnElmnt& q );
  void InsertElementAt( double s_0, double s, const bmlnElmnt* q );

                                      // Will insert q into the beamline at
                                      // OrbitLength s, assuming the beamline
                                      // begins at OrbitLength s_0.  Normally
                                      // will be invoked by a user program using
                                      // s_0 = 0, but s_0 is included so the
                                      // module can be used recursively.

  void InsertElementsFromList( double& s_0, InsertionList&, slist& );

				      // Will insert elements from the list into
                                      // the beamline at locations specified in
                                      // the list.  Removed elements are stored
                                      // in the final argument, in case they
                                      // need to be eliminated by the user module.

  beamline& operator^( bmlnElmnt& );  // Alters the beamline
  beamline& operator^( beamline& );
  beamline& operator+( bmlnElmnt& );  // Does not alter the beamline
  beamline& operator+( beamline& );
  beamline& operator-( beamline& );
  friend beamline& operator-( beamline& );


  int startAt( const bmlnElmnt*,     // Resets the "beginning" of the
               int = 1 );            // beamline to the element given
                                     // by the argument. Should be used
                                     // only for rings. Returns non-zero
                                     // if error occurs (esp., no such
                                     // element. The optional integer
                                     // argument allows one to reset to
                                     // nth occurrence of the element.
  int startAt( const char*,          // Resets the "beginning" of the
               int = 1 );            // beamline to the element whose
                                     // name is given by the argument.


  sector* makeSector ( int, JetParticle& );
                                     // Returns a pointer to a new
                                     // sector equivalent to the entire
                                     // beamline.  The argument is the
                                     // degree of the map.
  sector* MakeSector ( const bmlnElmnt&,   // Returns a pointer to a new sector
		      const bmlnElmnt&,   // equivalent to everything between
		      int,                // the first two arguments( exclusive).
		      JetParticle& );
  sector* MakeSectorFromStart 
    ( const bmlnElmnt&,   
     int,                
     JetParticle& );
  sector* MakeSectorToEnd 
    ( const bmlnElmnt&,   
     int,                
     JetParticle& );
  void     sectorize ( int, JetParticle& );
                                     // Alters the object itself.
  beamline sectorize ( bmlnElmnt&,   // Alters the object: everything between
                       bmlnElmnt&,   // the bmlnElmnt arguments replaced by a map.
                       int,          // <-- This argument is the degree of the map.
                       JetParticle&, 
                       const char* = "NONAME" );
  char putAbove( bmlnElmnt& x, bmlnElmnt& y ); // Insert y above (before;  upstream of) x
  char putBelow( bmlnElmnt& x, bmlnElmnt& y ); // Insert y below (after, downstream of) x

  beamline remove( bmlnElmnt&, bmlnElmnt& );
  char     remove( void * );
  char     remove( const bmlnElmnt& );
  char     remove( const bmlnElmnt* );

  // GEOMETRY

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void resetGeometry();
  void setAlignment ( const alignmentData& );


  // PROPAGATE PARTICLES

  void localPropagate( ParticleBunch& );
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void accept( BmlVisitor& v )
  { v.visitBeamline( this ); }
  void accept( ConstBmlVisitor& v ) const
  { v.visitBeamline( this ); }

  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );


  // EDIT PROPERTIES

  inline beamline::LineMode getLineMode() const
  { return _mode; }
  inline void setLineMode( beamline::LineMode x )
  { _mode = x; }
  // _mode doesn't affect behavior at the beamline level
  //    but carries information for higher level code,
  //    like _dataHook.

  void setEnergy( double /* Nominal energy in GeV */ );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS

  // ??? These three twiss functions should be eliminated.
  // ??? They are hopelessly obsolete.
  // twiss no-longer virtual to eliminate
  // circular library dependencies - FO

  int twiss( JetParticle&, 
             double = 0.00001 /* dpp */,
             short int = 1 /* attachFlag */ );
                           // Computes lattice functions all the
                           // way around the ring.
                           // Attaches a lattRing Barnacle labelled 
                           // "Ring" to the beamline, and a lattFunc
                           // Barnacle labelled "Twiss" to every element
                           // in the line.

  int twiss( char, 
             JetParticle& );
                           // Only computes lattice functions at
                           // the beginning of the ring.
                           // Uses the same method as MAD.
  int twiss( lattFunc&,
             JetParticle&,
             short int = 1 /* attachFlag */ );
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES

  void   peekAt( double& s, Particle* = 0 );
  lattFunc whatIsLattice( int );     // After element n, 0 <= n.
  lattFunc whatIsLattice( char* n ); // n is name of element 
  int    howMany() const { return numElem; }
  int    countHowMany( CRITFUNC = 0, slist* = 0 ) const;
  int    countHowManyDeeply( CRITFUNC = 0, slist* = 0 ) const;
  int    howDeep();
  int    contains( const bmlnElmnt*) const;
  // Returns the number of times the argument appears.

  inline bmlnElmnt* firstElement()
  { return (bmlnElmnt*) ( ((dlist*) this)->firstInfoPtr() ); }
  inline bmlnElmnt* lastElement()
  { return (bmlnElmnt*) ( ((dlist*) this)->lastInfoPtr() ); }
  inline char twissIsDone()
  { return twissDone; }
  inline void setTwissIsDone()
  { twissDone = 1; }
  inline void unsetTwissIsDone()
  { twissDone = 0; }
  double Energy() const 
  { return nominalEnergy; }
  const char*  Type() const;
  virtual int isType(char* c) 
  { if ( strcmp(c, "beamline") != 0 ) return bmlnElmnt::isType(c); 
    else return 1; 
  }

  bmlnElmnt* Clone() const;

  double OrbitLength( const Particle& );
  lattRing whatIsRing();


  // CLONING AND STORING

  beamline& operator=( const beamline& );
  beamline* flatten() const;    // Produces a flattened version of itself.

  void   writeLattFunc( );
  void   writeLattFunc( FILE* );
  void   writeLattFunc( char* );

} ;


extern beamline& operator*( int, beamline& );
extern beamline& operator*( int, bmlnElmnt& );
extern beamline& operator*( beamline&, int );
extern beamline& operator*( bmlnElmnt&, int );
extern beamline& operator^( bmlnElmnt&, bmlnElmnt& );
extern beamline& operator-( beamline& );


struct beamlineData : public bmlnElmntData
{
  int             numElem;          // Number of elements in the beamline

  beamlineData();
  beamlineData( beamlineData& );
  ~beamlineData();
  void eliminate();
  void* clone();

};


#endif // BMLNELMNT_H
