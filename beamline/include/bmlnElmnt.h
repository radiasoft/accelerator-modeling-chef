#ifndef BMLNELMNT_H
#define BMLNELMNT_H

#include "apstring.h"
#include "Particle.h"
#include "ParticleBunch.h"
#include "Aperture.h"   // O.K.
#include "BmlVisitor.h"

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
 ostream& operator<<(ostream&, const lattRing&);

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

ostream& operator<<(ostream&, const lattFunc&);
istream& operator>>(istream&, lattFunc&);

struct BMLN_posInfo {
    Vector inPoint;      // This struct defines the upstream and
    Vector inAxes  [3];  // downstream faces of the magnets slot.
    Vector outPoint;
    Vector outAxes [3];
    BMLN_posInfo( );
    BMLN_posInfo( const BMLN_posInfo& );
    BMLN_posInfo& operator=( const BMLN_posInfo& );   // ??? Does this struct
                                                      // ??? need a destructor?
    friend ostream& operator<<(ostream&, BMLN_posInfo&);

} ;


struct alignmentData {
  double       xOffset;
  double       yOffset;
  double       tilt;
  alignmentData();
  alignmentData(const alignmentData&);
  ~alignmentData();
  alignmentData& operator=(const alignmentData&);
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
  alignmentData getAlignment();

  double x_offset() { return xOffset; }
  double y_offset() { return yOffset; }
  double roll()     { return tilt;    }
  double cos_roll() { return cosTilt; }
  double sin_roll() { return sinTilt; }

  friend ostream& operator<<(ostream&, alignment&);
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
  class PropFunc
  {
    public:
      virtual int operator()( bmlnElmnt*, Particle&    ) = 0;
      virtual int operator()( bmlnElmnt*, JetParticle& ) = 0;
      virtual const char* Type() const                   = 0;
  };

  typedef char (*CRITFUNC)( bmlnElmnt* );

  static const short BF_OK;
  static const short BF_NULL_ARG;
  static const short BF_BAD_START;

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

  virtual void image( int,           // depth of image
                      slist*,        // list to append responses to
                      BMLN_posInfo*  // current geometry, updated
                    );
  virtual void geomToEnd   ( BMLN_posInfo& );
  virtual void geomToStart ( BMLN_posInfo& );
  virtual void eliminate();

  friend class beamline;
  // friend class circuit;
  friend beamline& operator-( beamline& );


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

  PropFunc* setPropFunction ( const PropFunc* a );  // return previous
  PropFunc* setPropFunction ( const PropFunc& a );  // Propagator
  PropFunc* getPropFunction() { return Propagator; }

  void propagate( Particle& x ) 
  {
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      enterLocalFrame ( x );
      localPropagate  ( x );
      leaveLocalFrame ( x );
    }
  }

  void propagate( JetParticle& x )
  {
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      enterLocalFrame ( x );
      localPropagate  ( x );
      leaveLocalFrame ( x );
    }
  }

  void propagate( ParticleBunch& x )
  {
    static Particle* p;
    if( align == 0 ) {
      localPropagate  ( x );
    }
    else {
      slist_iterator getNext( (slist&) x );
      while((  p = (Particle*) getNext()  )) enterLocalFrame( *p );
      localPropagate  ( x );
      getNext.Reset( (const slist&) x );
      while((  p = (Particle*) getNext()  )) leaveLocalFrame( *p );
    }
  }

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

  bmlnElmntData* image();
  void image( bmlnElmntData* );

  // Editing functions
  virtual bmlnElmnt* Clone() const = 0;  
                                   // Each type of bmlnElmnt will
                                   // create a copy of itself and
                                   // return a pointer to the new
                                   // object.
  bmlnElmnt* Clone(char* name);    // Clone it and change the name.

  virtual void Split( double s, bmlnElmnt**, bmlnElmnt** );
                                   // Splits the element at percent orbitlength
                                   // s from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.
  virtual void peekAt( double& s, Particle* = 0 );

  short  writeTag ( char,          // character to be written
                    short = 0      // position in tag
                  );
  short  writeTag ( const char*,   // characters to be written
                    short,         // starting position in tag
                    short          // number of characters 
                  );
  short  writeTag ( const char* );

  short  writeTag ( const String&,
                    short,         // starting position in tag
                    short          // number of characters 
                  );
  short  writeTag ( const String& );

  short  readTag  ( char*,         // returned characters
                    short,         // starting position in tag
                    short          // number of characters
                  );
  short  readTag  ( char* );

  char   readTag  ( short          // position in tag
                  );    
  String readTag  ( short,         // starting position in tag
                    short          // number of characters 
                  );
  String readTag  ();

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

  // Query functions ...
  alignmentData  Alignment( void );
  Aperture*      getAperture( void );	// returns a clone of the aperture class
  inline int     hasAperture( void ) const { return  ( pAperture ? 1 : 0 ); }
  inline double  Strength() 	const { return strength; }
  inline double  Length() 	const { return length; }
  virtual double Current() 	const { return strength/iToField; }
  inline char    IsYourName( const char* x ) { return strcmp( x, ident ); }
  char hasName( const char* x ) const
    { return ( 0 == strcmp( ident, x ) ); }
  virtual const char*  Name() const
    { return ident; }
  virtual const char*  Type() const = 0;
  virtual int    isType(char* s) { return strcmp(s, "bmlnElmnt") == 0; }


  void setFlavor( const apstring& x )
    { flavor = x; }
  void setFlavor( const char* x )
    { flavor = x; }
  apstring Flavor()
    { return flavor; }
  apstring getFlavor()
    { return flavor; }
  apstring FlavorOrType()
    { if( flavor.length() == 0 ) return apstring(this->Type());
      else                       return flavor;
    }
  apstring getFlavorOrType()
    { if( flavor.length() == 0 ) return apstring(this->Type());
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
  virtual ostream& writeTo(ostream& os) 	{ return os; }
  virtual istream& readFrom(istream& is)	{ return is; };
  friend ostream& operator<<(ostream&, bmlnElmnt&);
  friend bmlnElmnt* read_istream(istream&);

  apstring     flavor;     // Allows for "flavors" of types of elements.
  char         _tag[ BF_MAXCHAR ];
};



struct InsertionListElement {
  double     s;  // Position (design orbit length)
  bmlnElmnt* q;  // Element to be inserted.
  InsertionListElement( double     x = 0.0, /* s */
                        bmlnElmnt* p = 0    /* q */ 
                      ) { s = x; q = p; }
};


class InsertionList : private dlist
{
private:
  double smax;
  Proton prtn;
public:
  InsertionList( double /* momentum [GeV/c] */ = 1000.0 );
  InsertionList( const InsertionList& );
  ~InsertionList();

  void Append( const InsertionListElement& );
  void Append( const InsertionListElement* );
  void Insert( const InsertionListElement& );
  void Insert( const InsertionListElement* );
  void MergeUnique( InsertionList& );
  void MergeAll   ( InsertionList& );
  InsertionListElement* Get();   // Iterator; removes elements from list
  InsertionListElement* operator()( const int& ) const;
  InsertionList& operator=( const InsertionList& );
  int Size();
  void Clear();   // Preserves the InsertionListElements
  Proton GetParticle() { return prtn; }
  friend ostream& operator<<( ostream&, const InsertionList& );
};

inline 
InsertionListElement* 
InsertionList::Get()
{
  return (InsertionListElement*) dlist::get();
}

inline 
InsertionListElement* 
InsertionList::operator()( const int& n ) const
{
  return (InsertionListElement*) dlist::operator[]( n );
}


class beamlineImage;
class beamline : public bmlnElmnt, public dlist
{
private:
  double            nominalEnergy;    // In GeV
  int               numElem;          // Number of elements in the beamline
  char              twissDone;
  void image( int, slist*, BMLN_posInfo* );
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);
  friend istream& operator>>( istream&, beamline& );

public:


  // CONSTRUCTORS AND DESTRUCTOR

  beamline( const char* nm = "NONAME" );
  beamline( bmlnElmnt* );
  beamline( const beamline& );
  beamline( beamlineImage* );         // Creates a beamline from the
                                      //  list representation written
                                      //  by beamline::image.  In the
                                      //  process it empties the list.
  beamline( char*, bmlnElmnt* );
  beamline( char*, beamline* );
  beamline( char*, slist* );          // Creates a beamline from the
                                      //  list representation written
                                      //  by beamline::image.  ??? UNWRITTEN
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline(); 	       	       	       // Notice that this destructor does not destroy
                                       // the beamline elements.  To do that, use
                                       // beamline::zap().
  void zap();                          // Destroys the beamline elements
                                       // without destroying the beamline
  void eliminate();                    // Combines zap and ~beamline



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

  void accept( BmlVisitor& v ) { v.visitBeamline( this ); }

  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );


  // EDIT PROPERTIES

  void setEnergy( double /* Nominal energy in GeV */ );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS

  // ??? These three twiss functions should be eliminated.
  // ??? They are hopelessly obsolete.

  int twiss( JetParticle&, 
             double = 0.00001 /* dpp */,
             char   = 1       /* attachFlag */ );
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
             char  = 1        /* attachFlag */ );
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES

  void   peekAt( double& s, Particle* = 0 );
  beamlineImage  image( int );
  lattFunc whatIsLattice( int );     // After element n, 0 <= n.
  lattFunc whatIsLattice( char* n ); // n is name of element 
  int    howMany() const { return numElem; }
  int    countHowMany();
  int    countHowManyDeeply();
  int    howDeep();
  inline bmlnElmnt* firstElement()
    {
      return (bmlnElmnt*) ( ((dlist*) this)->firstInfoPtr() );
    }
  inline bmlnElmnt* lastElement()
    {
      return (bmlnElmnt*) ( ((dlist*) this)->lastInfoPtr() );
    }
  char twissIsDone()
    {
      return twissDone;
    }
  void setTwissIsDone()
    {
      twissDone = 1;
    }
  void unsetTwissIsDone()
    {
      twissDone = 0;
    }
  double Energy() const { return nominalEnergy; }
  const char*  Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "beamline") != 0 ) return bmlnElmnt::isType(c); else return 1; }

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


class beamlineImage {
private:
  slist bak;

  beamlineImage();
  friend class beamline;

public:
  slist img;

  // ??? REMOVE beamlineImage( FILE* );
  beamlineImage( beamlineImage& );
  ~beamlineImage();
  void reset();    // If the user has manipulated img, the reset()
                  //  method restores it to its original state.
  void zap();      // Destroys all XXXXData objects.
  beamlineImage& operator=( const beamlineImage& );
                  // WARNING: there may be stray XXXXData objects
                  //  hanging around after this if ::zap() is not
                  //  used first.
};

#endif // BMLNELMNT_H
