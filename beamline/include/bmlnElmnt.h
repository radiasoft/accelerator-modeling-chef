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
****** - beamline: improved implementation. beamline is no longer 
******             derived from a list container, but rather contains 
******             an instance of a list.  std:list<> is used rather 
******             than the old style (void*) dlist, which is not type safe and
******             could not hold smart pointers. 
******
****** Dec 2006:   ostiguy@fnal.gov  
******
****** - fixed (possibly long standing) memory corruption problems in bmlnElmnt destructor.  
******   for many element types.   
******   Introduced core_access empty class to manage Propagator access privileges.
****** - cleanup of constructor code
****** - eliminated raw c-style strings 
****** - Eliminated obsolete tagging functions 
****** - various public interface cleanups
****** 
****** Mar 2007:   ostiguy@fnal.gov
****** - support for reference counted elements    
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
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

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
class sector;
class alignment;

typedef boost::shared_ptr<bmlnElmnt> ElmPtr;
typedef boost::shared_ptr<beamline>  BmlPtr;

typedef boost::shared_ptr<bmlnElmnt const> ConstElmPtr;
typedef boost::shared_ptr<beamline const>  ConstBmlPtr;

template <typename T>
class TVector;

template <typename T>
class TJet;

typedef TVector<double> VectorD;
typedef TJet<double>    Jet;

class bmlnElmnt_core_access;

bmlnElmnt* read_istream(std::istream&);


class DLLEXPORT bmlnElmnt
{

  friend class beamline; 
  friend class bmlnElmnt_core_access; 

 public:

  class PropFunc
  {
    public:
      virtual int operator()( bmlnElmnt*, Particle&    ) = 0;
      virtual int operator()( bmlnElmnt*, JetParticle& ) = 0;
      virtual const char* Type() const                   = 0;
      virtual ~PropFunc() {};
  };


   //--------------------------------------------------------------
   // PinnedFrameSet 
   //--------------------------------------------------------------
   // If the element never moves, both _upStream and _downStream 
   // should be the identity frames. 
   // If it moves, then _upStream will be the element's original 
   // in-frame AS SEEN BY its current in-frame; 
   //  _downStream will be its original out-frame AS SEEN BY 
   // its current out-frame.
   // Note that in fact, this is redundant information.  
   // Either _upStream or _downStream could be eliminated. !!!

  class PinnedFrameSet
  {
    public:
      bool  _altered;
      Frame _upStream;
      Frame _downStream;
      PinnedFrameSet();
      ~PinnedFrameSet() {}

      void  reset();
  };

  typedef char (*CRITFUNC)( bmlnElmnt* );
  struct Discriminator
  {
    Discriminator();                             // Needed by constructors of derived classes.
      virtual      Discriminator* Clone() const = 0;
      virtual     ~Discriminator() {}

      virtual bool operator()( const bmlnElmnt* ) const = 0;
      virtual void writeTo( std::ostream& ) const = 0;

    //   private:

      Discriminator( Discriminator const& );     // forbidden ????

  };


  // comparison operator for hash table.  

  struct eqstr
  {
     bool operator()(const char* s1, const char* s2) const
     {
       return strcmp(s1, s2) == 0;
     }
  };

  typedef __gnu_cxx::hash_map<const char*, boost::any, __gnu_cxx::hash<const char*>, bmlnElmnt::eqstr> hash_map_t;


 public:

 struct GenericException : public std::exception {

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


public:
  
  bmlnElmnt( char const*   name = "NONAME",                                                PropFunc* = 0);              
  bmlnElmnt(                               double const&  length,                          PropFunc* = 0);              
  bmlnElmnt(                               double const&  length, double const&  strength, PropFunc* = 0);              
  bmlnElmnt( char const*   name,           double const&  length,                          PropFunc* = 0);
  bmlnElmnt( char const*   name,           double const&  length, double const& strength,  PropFunc* = 0);

  bmlnElmnt( bmlnElmnt const&  );

  virtual  bmlnElmnt* Clone() const = 0;  
  virtual ~bmlnElmnt();

  bmlnElmnt& operator=( bmlnElmnt const& a );  
 
  boost::any& operator[](std::string const& s);   // a type-safe facility to attach attributes of any type
  bool attributeExists(std::string const& s);     
  void attributeClear (std::string const& s);           
  void attributeClear ();                                

  virtual void accept( BmlVisitor& ) = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  virtual PropFunc* setPropFunction ( const PropFunc* a );  // return previous
  virtual PropFunc* setPropFunction ( const PropFunc& a );  // Propagator

  PropFunc* getPropFunction() { return propfunc_; }

  void propagate( Particle&      );
  void propagate( JetParticle&   );
  void propagate( ParticleBunch& );

  virtual void localPropagate( Particle&      );
  virtual void localPropagate( JetParticle&   );
  virtual void localPropagate( ParticleBunch& );


  // Methods to set alignment without 
  // (a little overkill, but so what?)

  virtual bool        alignRelX( double const&  meters );
  virtual bool        alignRelY( double const&  meters );
  virtual bool        alignAbsX( double const&  meters );
  virtual bool        alignAbsY( double const&  meters );
  virtual bool      alignRelXmm( double const&  mm     );
  virtual bool      alignRelYmm( double const&  mm     );
  virtual bool      alignAbsXmm( double const&  mm     );
  virtual bool      alignAbsYmm( double const&  mm     );
  virtual bool     alignRelRoll( double const& radians );
  virtual bool     alignAbsRoll( double const& radians );
  virtual bool alignRelRollmrad( double const& mrad    );
  virtual bool alignAbsRollmrad( double const& mrad    );
  virtual bool     setAlignment( alignmentData const& );

  inline  bool hasMoved()
    { return ( pinnedFrames_._altered || align_  ); }

  void realign();  // Resets element to its original position.

  void markPins(); // WRITE // Marks the current positions of the element as "original"

  void loadPinnedCoordinates( Particle const &, Vector&, double = 1.0 );  // WRITE

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
    { return pinnedFrames_._upStream; }

  Frame getDownstreamPinnedFrame() const 
    { return pinnedFrames_._downStream; }


  virtual void enterLocalFrame( Particle&                ) const;
  virtual void enterLocalFrame( JetParticle&             ) const;

  virtual void leaveLocalFrame( Particle&                ) const;
  virtual void leaveLocalFrame( JetParticle&             ) const;

  // Editing functions

  virtual void Split( double const& pct, ElmPtr&, ElmPtr& ) const;
                                   // Splits the element at percent orbitlength
                                   // pct from the in-face and returns
                                   // addresses to the two pieces
                                   // in the second and third arguments.
                                   // The user has responsibility for
                                   // eventually deleting these.

  virtual void  peekAt( double& s, Particle const& ) const;

  virtual bool equivTo( bmlnElmnt const& ) const;
  virtual bool equivTo( bmlnElmnt const* ) const;

  virtual bool hasParallelFaces() const;
  virtual bool hasStandardFaces() const;

  virtual bool isSimple() const;
  virtual bool isMagnet() const = 0;


  // ... Tagging methods

  void              setTag(  std::string const& tag){ tag_   = tag; }           
  std::string       getTag()           const        { return tag_;  }  


  //  ... Modifiers

  virtual void setLength     ( double const& );
  virtual void setStrength   ( double const& );
  virtual void setCurrent    ( double const& );
  virtual void setShunt(double const& a);
          void setAperture   ( Aperture* );
          void rename        ( std::string name);  




  virtual double getReferenceTime() const;
  virtual double setReferenceTime( Particle const& );         // returns ctRef_
  virtual double setReferenceTime( double const& );           // returns previous ctRef_


  // ... Query functions 

  alignmentData  Alignment( void )        const;
  Aperture*      getAperture( void );                                           // returns a clone of the aperture class
  int            hasAperture( void )      const { return  ( pAperture_ ? 1 : 0 ); }
  double         Strength()               const { return strength_; }
  double         Length()                 const;
  virtual double Current()                const  { return strength_/iToField_; }

  std::string          Name()             const { return ident_; }
  virtual const char*  Type()             const = 0;


  virtual double OrbitLength( Particle const& ) { return length_; }
                                   // Returns length of design orbit
                                   // segment through the element.
                                   // Will be different from "length"
                                   // for rbends.

  double& IToField()            { return iToField_; }
  double  getShunt() const      { return shuntCurrent_; }



protected:

  std::string                    ident_;         // Name identifier of the element.
  double                         length_;        // Length of object [ meters ]
  double                         strength_;      // Interpretation depends on object.
  
  alignment*                     align_;
  double                         iToField_;      // Conversion factor for current through
                                                 // magnet in amperes to field or gradient etc.
  double                         shuntCurrent_;  // Does this element have a shunt?

  PinnedFrameSet                 pinnedFrames_;

  mutable double                 ctRef_;         // (normalized) time required for
                                                 // a reference particle to cross
                                                 // the element. Established by a
                                                 // RefRegVisitor.

  hash_map_t                     attributes_;

  std::string                    tag_;

  Aperture*                      pAperture_;     // O.K.

  PropFunc*                      propfunc_;      

  BmlPtr                         p_bml_;         // The element may be composite.
  ElmPtr                         bml_e_;         // with one active part.


  virtual void releasePropFunc();
  virtual void setupPropFunc();


  BmlPtr       getBmlPtr()   { return p_bml_; }   // used by core_access 
  ElmPtr       getElmPtr()   { return bml_e_; }   // used by core_access

private:

  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */

  virtual std::ostream&    writeTo(std::ostream      & os) {return os;}     
  virtual std::istream&   readFrom(std::istream      & is) {return is;}

  friend  std::ostream& operator<<(std::ostream&, bmlnElmnt&);

  friend bmlnElmnt* read_istream(std::istream&);

public:

  BarnacleList dataHook;   // Carries data as service to application program.


};



//---------------------------------------------------------------------------------
//   bmlnElmnt_core_access class
//---------------------------------------------------------------------------------

 class  bmlnElmnt_core_access { 

 public:

   static BmlPtr      getBmlPtr(bmlnElmnt& elm )  { return elm.getBmlPtr(); } 
   static ElmPtr      getElmPtr(bmlnElmnt& elm)   { return elm.getElmPtr(); } 

}; 


#endif // BMLNELMNT_H
