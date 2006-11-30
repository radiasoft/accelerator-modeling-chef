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
#include <ext/hash_map>    // This is g++ specific, but will be supported by the next c++ std. 
#include <boost/any.hpp>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Frame.h>
#include <basic_toolkit/Barnacle.h>

#define NOTKNOWN   -123456.789
#define OSTREAM_DOUBLE_PREC setprecision(20)



//------------------------------
// Forward declarations
//------------------------------

class bmlnElmnt;
class alignmentData;
class beamline;
class BmlVisitor;
class ConstBmlVisitor;
class ParticleBunch;
class Aperture;
class Particle;
class JetParticle;
class InsertionList;
class sector;
class alignment;

template <typename T>
class TVector;

template <typename T>
class TJet;

typedef TVector<double> VectorD;
typedef TJet<double>    Jet;

bmlnElmnt* read_istream(std::istream&);



class DLLEXPORT bmlnElmnt
{

  friend class beamline; 

public:
                      

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

  virtual bmlnElmnt* Clone() const = 0;  
  virtual ~bmlnElmnt();

  // ----------------------------------------------
  Aperture*    pAperture;  // O.K.
  
  BarnacleList dataHook;   // Carries data as service to application program.

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

  void              setTag(  std::string const& tag){ tag_   = tag; }           
  std::string       getTag()           const        { return tag_;  }  


  // Modifiers
  // ---------
  virtual void setLength     ( double );
  virtual void setStrength   ( double );
  virtual void setCurrent    ( double );
  virtual void setShunt(double a);
          void setAperture   ( Aperture* );
          void rename        ( const char* x );  



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

  virtual std::ostream&    writeTo(std::ostream      & os) {return os;}     
  virtual std::istream&   readFrom(std::istream      & is) {return is;}

  friend  std::ostream& operator<<(std::ostream&, bmlnElmnt&);

  friend bmlnElmnt* read_istream(std::istream&);

  std::string  tag_;
};



#endif // BMLNELMNT_H
