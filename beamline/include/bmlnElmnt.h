/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      bmlnElmnt.h
******  Version:   3.1
******
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
****** REVISION HISTORY:
****** -----------------
****** 
****** Oct 2006:   Jean-Francois Ostiguy  ostiguy@fnal.gov
******
****** - beamline: decoupled list container from public interface
******             use std:list<> instead of dlist
******                                                           
**************************************************************************
*************************************************************************/
#ifndef BMLNELMNT_H
#define BMLNELMNT_H

#include <string>
#include <iostream>
#include <exception>

#include <list>
#include <basic_toolkit/slist.h>
#include <basic_toolkit/Frame.h>
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/VectorD.h>
#include <beamline/BmlVisitor.h>

#include <ext/hash_map>    // This is g++ specific, but will be supported
                           // in the next c++ std. Most other compilers 
                           // support hashes.      
#include <boost/any.hpp>

#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>

bmlnElmnt* read_istream(std::istream&);

class ParticleBunch;
class Aperture;
class Particle;

class JetParticle;
class InsertionList;
class sector;

// Forward declaration required
class bmlnElmnt;
class beamline;

#define NOTKNOWN   -123456.789
#define OSTREAM_DOUBLE_PREC setprecision(20)

struct DLLEXPORT lattRing : public BarnacleData {
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

struct DLLEXPORT lattFunc : public BarnacleData {
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


struct DLLEXPORT alignmentData {
  double       xOffset;
  double       yOffset;
  double       tilt;
  alignmentData();
  alignmentData(const alignmentData&);
  ~alignmentData();
  alignmentData& operator=(const alignmentData&);
  bool operator==(const alignmentData&) const;
};


class DLLEXPORT alignment {
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
  void    align(Particle const&, double*);
  void    align(double*,         double*);
  void misalign(Particle const&, double*);
  void    align(JetParticle&,    Jet*);
  void    align(Jet*,            Jet*);
  void misalign(JetParticle&,    Jet*);
  void misalign(double* );
  void    align(double* );
  void misalign(JetVector& );
  void    align(JetVector& );

  void          setAlignment(alignmentData const&);
  alignmentData getAlignment() const;

  double x_offset() { return xOffset; }
  double y_offset() { return yOffset; }
  double roll()     { return tilt;    }
  double cos_roll() { return cosTilt; }
  double sin_roll() { return sinTilt; }

  friend std::ostream& operator<<(std::ostream&, alignment&);
};


struct DLLEXPORT bmlnElmntData {
  char            type[80];
  bmlnElmnt*      address;
  char*           name;
  double          length;
  double          strength;

    //
    // O.K.
    //
    //  double          aperture;

  Aperture*       pAperture;
  
  alignmentData   align;
  double          iToField;
  double          shuntCurrent;
  char            more;

  bmlnElmntData();
  bmlnElmntData( bmlnElmntData& );
  virtual ~bmlnElmntData();
  virtual void eliminate();
  virtual void* clone();
};



class DLLEXPORT bmlnElmnt
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
      virtual ~PropFunc() {};
  };

  class PinnedFrameSet
  {
    public:
      bool  _altered;
      Frame _upStream;
      Frame _downStream;
      // If the element never moves, 
      //   both _upStream and _downStream should be the
      //   identity frames. 
      // If it moves, then _upStream will be the
      //   element's original in-frame AS SEEN BY 
      //   its current in-frame; _downStream will be its 
      //   original out-frame AS SEEN BY its current
      //   out-frame.
      // !!! In fact, this is redundant information.              !!!
      // !!! Either _upStream or _downStream could be eliminated. !!!

      PinnedFrameSet();
      ~PinnedFrameSet() {}

      void  reset();
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

    // comparison operator for hash table.  

  struct eqstr
  {
     bool operator()(const char* s1, const char* s2) const
     {
       return strcmp(s1, s2) == 0;
     }
  };

protected:
  char*        ident;      // Name identifier of the element.
  double       length;     // Length of object [ meters ]
  double       strength;   // Interpretation depends on object.
  
  alignment*   align;
  double       iToField;   // Conversion factor for current through
                           // magnet in amperes to field or gradient etc.
  double       shuntCurrent; // Does this element have a shunt?

  beamline*    p_bml;      // The element may be composite.
  bmlnElmnt*   p_bml_e;    // with one active part.

  PropFunc*    Propagator;

  virtual void releasePropFunc();
  virtual void setupPropFunc();
  virtual void eliminate();

  PinnedFrameSet _pinnedFrames;

  friend class beamline;
  // friend class circuit;

  friend beamline& operator-( beamline& );

  double _ctRef;  // (normalized) time required for
                  // a reference particle to cross
                  // the element. Established by a
                  // RefRegVisitor.

  __gnu_cxx::hash_map<const char*, boost::any, __gnu_cxx::hash<const char*>, bmlnElmnt::eqstr> _attributes;


public:
  bmlnElmnt( char const*   name = "NONAME",                                  PropFunc* = 0);              
  bmlnElmnt(                               double  length,                   PropFunc* = 0);              
  bmlnElmnt(                               double  length, double  strength, PropFunc* = 0);              
  bmlnElmnt( char const*   name,           double  length,                   PropFunc* = 0);
  bmlnElmnt( char const*   name,           double  length, double strength,  PropFunc* = 0);

  bmlnElmnt( bmlnElmnt const&  );
  bmlnElmnt( bmlnElmntData& );

  virtual ~bmlnElmnt();

  // ----------------------------------------------
  Aperture*    pAperture;  // O.K.
  
  BarnacleList dataHook;   // Carries data as service to application program.
  // ??? REMOVE:  lattFunc     lattInfo;   // Information on maps and lattice functions.
  // REMOVE: bmlnElmnt& operator=( const bmlnElmnt& );

  boost::any& operator[](std::string const& s);   // a type-safe facility to attach attributes
  bool attributeExists(std::string const& s);     // of arbitary type -jfo   
  void attributeClear (std::string const& s);           
  void attributeClear ();                                

  virtual void accept( BmlVisitor& ) = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  virtual PropFunc* setPropFunction ( const PropFunc* a );  // return previous
  virtual PropFunc* setPropFunction ( const PropFunc& a );  // Propagator

  PropFunc* getPropFunction() { return Propagator; }

  void propagate( Particle& );
  void propagate( JetParticle& );
  void propagate( ParticleBunch& );

  virtual void localPropagate( Particle& );
  virtual void localPropagate( JetParticle& );
  virtual void localPropagate( ParticleBunch& );


  // Methods to set alignment without 
  // (a little overkill, but so what?)
  virtual bool        alignRelX( double  meters);
  virtual bool        alignRelY( double  meters);
  virtual bool        alignAbsX( double  meters);
  virtual bool        alignAbsY( double  meters);
  virtual bool      alignRelXmm( double  mm);
  virtual bool      alignRelYmm( double  mm);
  virtual bool      alignAbsXmm( double  mm);
  virtual bool      alignAbsYmm( double  mm);
  virtual bool     alignRelRoll( double radians);
  virtual bool     alignAbsRoll( double radians);
  virtual bool alignRelRollmrad( double mrad );
  virtual bool alignAbsRollmrad( double mrad );
  virtual bool     setAlignment( alignmentData const& );

  inline  bool hasMoved()
    { return ( _pinnedFrames._altered || (0 != align) ); }
  void realign();
    // Resets element to its original position.
  void markPins(); // WRITE
    // Marks the current positions of the element as "original"
  void loadPinnedCoordinates( const Particle&, Vector&, double = 1.0 );  // WRITE
    // Pinned coordinates of the particle are returned in the vector
    //   argument.  
    // 
    // The third argument must be within [0,1]; it is internally
    //   set to 1.0 otherwise.  It indicates the percentage of distance
    //   from the upstream to downstream faces of the element at which 
    //   the coordinates are to be calculated.
    // 
    // Default value: 1.0 -> return pinned coordinates at the downstream end.
  Frame getUpstreamPinnedFrame() const 
    { return _pinnedFrames._upStream; }
  Frame getDownstreamPinnedFrame() const 
    { return _pinnedFrames._downStream; }


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

  virtual void Split( double pct, bmlnElmnt**, bmlnElmnt** ) const;
                                   // Splits the element at percent orbitlength
                                   // pct from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.

  // REMOVE: virtual void peekAt( double& s, Particle* = 0 );

  virtual void peekAt( double& s, const Particle& ) const;
  virtual bool equivTo( const bmlnElmnt& ) const;
  virtual bool equivTo( const bmlnElmnt* ) const;
  virtual bool hasParallelFaces() const;
  virtual bool hasStandardFaces() const;
  virtual bool isSimple() const;
  virtual bool isMagnet() const;


  // Tagging methods
  static const short BF_OK;
  static const short BF_NULL_ARG;
  static const short BF_BAD_START;

  short       writeTag(  std::string const& );              // Replaces entire tag
  short       writeTag(  std::string const& , short );      // starting position in tag
                   
  short       writeTag(  char const* newtag);               // Replaces entire tag
  short       writeTag(  char const* newtag, short pos);    // starting position in tag
                    
  short       writeTag(  char );
  short       writeTag(  char, short pos);                  // pos = position in tag

  std::string readTag() const;                              // returns entire tag
  std::string readTag(  short startpos, short n  ) const;   // n = no of characters read
  short       readTag(  char* )                    const;
  short       readTag(  char* returned_chars, short startpos, short n) const; 
  char        readTag(  short pos )                 const;  // pos = position in tag

  short       getTagSize() const;


  // Modifiers
  // ---------
  virtual void setLength     ( double );
  virtual void setStrength   ( double );
  virtual void setCurrent    ( double );
  virtual void setShunt(double a);
          void set           ( const bmlnElmntData& );
          void setAperture   ( Aperture* );
          void Rename        ( const char* );
          void rename        ( const char* x ) { this->Rename(x); }  
          // an alias


  // REMOVE: virtual double getReferenceTime() const {return _ctRef;}

  virtual double getReferenceTime() const;
  virtual double setReferenceTime( const Particle& );  // returns _ctRef
  virtual double setReferenceTime( double );  // returns previous _ctRef


  // Query functions ...

  alignmentData  Alignment( void )        const;
  Aperture*      getAperture( void );                                           // returns a clone of the aperture class
  int            hasAperture( void )      const { return  ( pAperture ? 1 : 0 ); }
  double         Strength()               const { return strength; }
  double         Length()                 const;
  virtual double Current()                const  { return strength/iToField; }
  char           IsYourName(char const* x)       { return strcmp( x, ident ); }
  char           hasName( const char* x ) const  { return ( 0 == strcmp( ident, x ) ); }

  virtual const char*  Name()             const { return ident; }
  virtual const char*  Type()             const = 0;
  virtual bool isType( const char* c )          { return strcmp(c, "bmlnElmnt") == 0; }


  void setFlavor( const std::string& x ) { flavor = x; }
  void setFlavor( const char* x )        { flavor = x; }
  std::string Flavor()                   { return flavor; }
  std::string getFlavor()                { return flavor; }
  std::string FlavorOrType()             { if( flavor.length() == 0 ) return std::string(this->Type());
                                            else                      return flavor; 
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
  double& IToField()            { return iToField; }
  double  getShunt() const      { return shuntCurrent; }


private:

  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */
  virtual std::ostream& writeTo(std::ostream& os)       { return os; }
  virtual std::istream& readFrom(std::istream& is)      { return is; }
  friend std::ostream& operator<<(std::ostream&, bmlnElmnt&);
  friend bmlnElmnt* read_istream(std::istream&);

  std::string flavor;     // Allows for "flavors" of types of elements.
  std::string  _tag;
};


//------------------------------------------------------------------------------------------------------------

class DLLEXPORT BmlPtrList : private dlist
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

  void append( bmlnElmnt* );
  void insert( bmlnElmnt* );

  bmlnElmnt* get();   // Iterator; removes elements from list
  bmlnElmnt* operator()( const int& ) const;

  int size() const;
  void clear();   // Preserves the bmlnElmnts
};


//-----------------------------------------------------------------------------------------------------

class DLLEXPORT beamline : public bmlnElmnt
{

 friend class BmlVisitor;
 friend class ConstBmlVisitor;
 friend class BeamlineIterator;
 friend class ReverseBeamlineIterator;
 friend class DeepBeamlineIterator;
 friend class DeepReverseBeamlineIterator;

public:
  enum LineMode { line, ring, unknown };

  class arrayRep
  {
  public:
    arrayRep( const beamline*, bool = false );    // Clones all elements if 2nd argument is true
   ~arrayRep();
    int size() { return _n; }
    bmlnElmnt* e( int i ) { return _element[i]; } // WARNING: For speed, this is made unsafe.
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
    virtual ~Criterion() {};

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
    virtual ~Action() {}
  };
  // Derived classes should return 0 if the action is 
  // performed successfully on the element. All other
  // values indicate error.


private:

  // Data

  double                  nominalEnergy;    // In GeV
  int                     numElem;          // Number of elements in the beamline
  char                    twissDone;
  LineMode               _mode;
  std::list<bmlnElmnt*>  _theList; 

  // Methods
  void   _moveRel(   int axis, double u,     bmlnElmnt* thePtr,             int*   errorCodePtr, BmlPtrList* recycleBinPtr, std::string invoker );
  void _rotateRel(   int axis, double angle, bmlnElmnt* thePtr, double pct, int*   errorCodePtr, BmlPtrList* recycleBinPtr, std::string invoker );

  std::ostream& writeTo(std::ostream&);
  friend std::istream& operator>>( std::istream&, beamline& );


public:

  // CONSTRUCTORS AND DESTRUCTOR____________________________________________________________

  beamline( const char* nm = "NONAME" );
  beamline( bmlnElmnt* );
  beamline( beamline const& );
  beamline( char*, bmlnElmnt* );
  beamline( char*, beamline* );
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline();                         // Notice that this destructor does not destroy
                                       // the beamline elements.  To do that, use
                                       // beamline::zap().
  void zap();                          // Destroys the beamline elements
                                       // without destroying the beamline
  void eliminate();                    // Combines zap and ~beamline
  void clear();                        // Returns state to empty beamline.



  // EDITING LATTICE_____________________________________________________________________

  void insert( bmlnElmnt* );
  void append( bmlnElmnt* );

  void Split( double, bmlnElmnt**, bmlnElmnt** ) const;

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
  // InsertElementsFromList1 is a workaround for python bindings. See
  // python-bindings/src/py-bmlnelmnt.cpp
  double InsertElementsFromList1( double s_0, InsertionList& il, slist& sl)
                                      // a version of InsertElementsFromList
                                      // that is callable from Python.
    {
      double tmp(s_0);
      InsertElementsFromList(tmp, il, sl);
      return tmp;
    }

  int replace( const bmlnElmnt*, const bmlnElmnt* );
                                      // Will replace the first argument with
                                      // the second. Return values:
                                      // 0 everything went as planned
                                      // 1 first argument was not found
                                      // 2 at least one argument was null
                                      // NOTE: works only at the highest
                                      // level of the tree hierarchy; does
                                      // not descend into the tree.
                                      // NOTE ALSO: will replace only the
                                      // first object found whose address
                                      // matches that of the second.

  int deepReplace( const bmlnElmnt*, const bmlnElmnt* );
                                      // Like replace() but descends into
                                      // the hierarchy. Either argument
                                      // may be a pointer to a beamline.

  //beamline& operator^( bmlnElmnt& );  // Alters the beamline
  //beamline& operator+( bmlnElmnt& );  // Does not alter the beamline
  //beamline& operator+( beamline& );
  //beamline& operator-( beamline& );

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

  sector* MakeSector ( bmlnElmnt const&, bmlnElmnt const&, int,  JetParticle&);                       // Returns a pointer to a new sector
                                                                                                      // equivalent to everything between
                                                                                                      // the first two arguments( exclusive).
                 


  sector* MakeSectorFromStart (bmlnElmnt const&,  int,  JetParticle& );
  sector*     MakeSectorToEnd (bmlnElmnt const&,  int,  JetParticle& );


  void     sectorize ( int, JetParticle& );                                                                         // Alters the object itself.
  beamline sectorize ( bmlnElmnt* start,  bmlnElmnt* end, int degree,  JetParticle& p,   const char* = "NONAME");   // Alters the object: everything between
                                                                                                                    // the bmlnElmnt arguments replaced by a map.
                                                                                                                    // <-- This argument is the degree of the map.
                        
                

  void putAbove( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt* y ); // Insert y above (before;  upstream of) x
  void putBelow( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt* y ); // Insert y below (after, downstream of) x

  beamline remove( bmlnElmnt*, bmlnElmnt* );
  void     remove( bmlnElmnt* );


  // Change geometry of the line

  bool     setAlignment( alignmentData const& );
  BmlPtrList   moveRelX( bmlnElmnt*, double, int* = 0 );
  BmlPtrList   moveRelY( bmlnElmnt*, double, int* = 0 );
  BmlPtrList   moveRelZ( bmlnElmnt*, double, int* = 0 );
  BmlPtrList      pitch( bmlnElmnt*, double, double, int* );
  BmlPtrList        yaw( bmlnElmnt*, double, double, int* );
  BmlPtrList       roll( bmlnElmnt*, double, double, int* );


  // PROPAGATE PARTICLES


  void localPropagate( Particle& );          
  void localPropagate( ParticleBunch& );          
  void localPropagate( JetParticle& );          

  void enterLocalFrame( Particle&    )    const;   
  void enterLocalFrame( JetParticle&    ) const;   

  void leaveLocalFrame( Particle&    )    const;   
  void leaveLocalFrame( JetParticle&    ) const;   




  void accept( BmlVisitor& v )
  { v.visitBeamline( this ); }
  void accept( ConstBmlVisitor& v ) const
  { v.visitBeamline( this ); }

  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );

  void realignAllElements();  // WRITE
  void markAllPins();         // WRITE

  // EDIT PROPERTIES________________________________________________________________

  inline beamline::LineMode getLineMode() const
  { return _mode; }
  inline void setLineMode( beamline::LineMode x )
  { _mode = x; }

  //   _mode doesn't affect behavior at the beamline level
  //    but carries information for higher level code,
  //    like _dataHook.

  void setEnergy( double  nominalEnergyGeV );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS_________________________________________________________________________

  // twiss no-longer virtual to eliminate
  // circular library dependencies - FO

  int twiss( JetParticle&, double dpp = 0.00001, short int  attachFlag = 1 );
                           // Computes lattice functions all the
                           // way around the ring.
                           // Attaches a lattRing Barnacle labelled 
                           // "Ring" to the beamline, and a lattFunc
                           // Barnacle labelled "Twiss" to every element
                           // in the line.

  int twiss( char,         JetParticle& );
                           // Only computes lattice functions at
                           // the beginning of the ring.
                           // Uses the same method as MAD.

  int twiss( lattFunc&,    JetParticle&,  short int attachFlag = 1);
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES _________________________________________________________________________________

  // REMOVE: void   peekAt( double& s, Particle* = 0 );

  void                peekAt( double& s, const Particle& ) const;
  lattFunc     whatIsLattice( int );                                                // After element n, 0 <= n.
  lattFunc     whatIsLattice( char* n );                                            // n is name of element 
  int                howMany()                           const { return numElem; }  // WARNING: not reliable!
  int           countHowMany( CRITFUNC = 0, slist* = 0 ) const;
  int     countHowManyDeeply( CRITFUNC = 0, slist* = 0 ) const;
  int                  depth()                           const;                     // Returns -1 if beamline is empty.
                                                                                    // Returns  0 if beamline is flat
                                                                                    //            or all its subbeamlines are empty.
                                                                                    // Otherwise returns 1 + largest
                                                                                    // depth of all subbeamlines.

  int    contains( const bmlnElmnt*) const;     // Returns the number of times the argument appears.

  bool find( bmlnElmnt*& u, bmlnElmnt*& v, bmlnElmnt*& w ) const;

  // Upon entry: u and w should have null value but can, in fact
  //               be anything. 
  //               WARNING: they will be reset, so don't use addresses
  //               of valid elements.
  //             *v is the element to be searched for.
  // Upon exit:  u points to the element upstream of v
  //             w points to the element downstream of v
  //               Return value of u or w can be null (i.e. 0) if
  //               this beamline is not treated as a ring and *v is
  //               either the first or last element, respectively.
  //
  //             Value returned: true,  if *v is found
  //                             false, if *v is not found
  //               If false is returned, then return values of 
  //               u and w are unspecified.
  // 
  // Comments:   Only one instance, the first instance, of *v is found.
  //             This routine is meant to work on lines containing
  //               unique elements. For some cases, it may work with
  //               multiple instances of the same element.
  // 
  // Example:    beamline A; ...
  //             bmlnElmnt* u = 0; bmlnElmnt* w = 0;
  //             rbend B; ...
  //             if( A.find( u, &B, w ) ) { ... }
  // 



  inline bmlnElmnt* firstElement() const
  { return   _theList.front(); }

  inline bmlnElmnt* lastElement() const
  { return   _theList.back(); }

  inline char twissIsDone()
  { return twissDone; }

  inline void setTwissIsDone()
  { twissDone = 1; }

  inline void unsetTwissIsDone()
  { twissDone = 0; }

  double Energy() const 
  { return nominalEnergy; }

  const char*  Type() const;
  virtual bool isType( const char* c )
  { if ( strcmp(c, "beamline") != 0 ) return bmlnElmnt::isType(c); 
    else return true; 
  }
  bool isFlat();

  beamline* Clone() const;

  double OrbitLength( const Particle& );
  lattRing whatIsRing();


  // CLONING AND STORING_______________________________________________________________________

  // REMOVE: beamline& operator=( const beamline& );
  beamline* flatten() const;    // Produces a flattened version of itself.
                                // WARNING: the elements are not cloned.
                                //   Thus the flattened line contains the
                                //   same objects as its original.

  void   writeLattFunc( );
  void   writeLattFunc( FILE* );
  void   writeLattFunc( char* );

} ;




//------------------------------------------------------------------------------------------------------------

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
