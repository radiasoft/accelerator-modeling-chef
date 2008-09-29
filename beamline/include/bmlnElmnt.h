/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      bmlnElmnt.h
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
****** - beamline: improved implementation. beamline is no longer 
******             derived from a list container, but rather contains 
******             an instance of a list.  std:list<> is used rather 
******             than the old style (void*) dlist, which is not type 
******             safe and could not hold smart pointers. 
****** Dec 2006:   ostiguy@fnal.gov  
****** - fixed (possibly long standing) memory corruption problems 
******   in bmlnElmnt destructor for many element types.   
******   Introduced elm_core_access empty class to grant Propagators 
******   access privileges to element private data.
****** - cleanup of constructor code
****** - eliminated raw c-style strings 
****** - Eliminated obsolete tagging functions 
****** - various public interface cleanups
****** Mar 2007:   ostiguy@fnal.gov
****** - support for reference counted elements    
****** July 2007   ostiguy@fnal.gov
****** - new, less memory hungry implementation for PinnedFrameSet
****** - eliminated nested functor base classes (e.g. CRITFUNC) 
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
****** - Propagate/localPropagate functions now const correct
****** - added physical apertures
******
**************************************************************************
*************************************************************************/
#ifndef BMLNELMNT_H
#define BMLNELMNT_H

#include <string>
#include <iostream>
#include <exception>

#include <list>
#include <map>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Frame.h>
#include <beamline/ElmPtr.h>

#include <beamline/ParticleFwd.h>

//------------------------------
// Forward declarations
//------------------------------

class bmlnElmnt;
class alignmentData;
class beamline;
class BmlVisitor;
class ConstBmlVisitor;
class Aperture;
class sector;
class alignment;
class BasePropagator;

typedef boost::shared_ptr<beamline>        BmlPtr;
typedef boost::shared_ptr<beamline const>  ConstBmlPtr;

template <typename T>
class TVector;

template <typename T>
class TMapping;

template <typename T>
class TJet;

template <typename T>
class TJetVector;

typedef TVector<double>                VectorD;
typedef TJet<double>                   Jet;
typedef TJet<std::complex<double> >    JetC;
typedef TJetVector<double>             JetVector;
typedef TMapping<double>               Mapping;


template <typename Particle_t>
class TBunch;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

bmlnElmnt* read_istream(std::istream&);


class DLLEXPORT bmlnElmnt {

  friend class beamline; 
  friend class core_access; 

 public:
 
   enum aperture_t { infinite, elliptical, rectangular};

   typedef boost::shared_ptr<BasePropagator> PropagatorPtr;   

   class core_access;

   //--------------------------------------------------------------
   // PinnedFrameSet 
   //--------------------------------------------------------------
   // If the element never moves, both upStream_ and downStream_  
   // should be the identity frames. 
   // If it moves, then upStream_ will be the element's original 
   // in-frame AS SEEN BY its current in-frame; 
   // downStream_ will be its original out-frame AS SEEN BY 
   // its current out-frame.
   // Note that in fact, this is redundant information.  
   // Either upStream_ or downStream_ could be eliminated. !!!

  class PinnedFrameSet
  {

    public:

      PinnedFrameSet();
      PinnedFrameSet( PinnedFrameSet const& );

     ~PinnedFrameSet();

      PinnedFrameSet& operator=( PinnedFrameSet const& rhs);

      Frame const&   upStream()   const;
      Frame const&   downStream() const;
    
      void   upStream( Frame const& );
      void downStream( Frame const& );
    
      bool  altered() const;
      void  reset();

   private:

      Frame*  upStream_;
      Frame*  downStream_;

  };

  // comparison operator for hash table.  

  struct eqstr
  {
     bool operator()(const char* s1, const char* s2) const
     {
       return strcmp(s1, s2) == 0;
     }
  };

public:
  
  bmlnElmnt( std::string const& name="",  double const&  length=0.0, double const& strength=0.0 );

  bmlnElmnt( bmlnElmnt const&  );

  virtual  bmlnElmnt* Clone() const = 0;  
  virtual ~bmlnElmnt();

  bmlnElmnt& operator=( bmlnElmnt const& a );  
 
  boost::any& operator[](std::string const& s);    // a type-safe facility to attach attributes of any type

  bool         queryAttribute(std::string const& s) const;     
  void        removeAttribute(std::string const& s);           
  void        removeAttribute(); 

  virtual void accept( BmlVisitor&      )       = 0;
  virtual void accept( ConstBmlVisitor& ) const = 0;

  bool         hasAperture() const;
  void         setAperture( aperture_t type, double hor, double ver);
  boost::tuple<aperture_t,double,double> aperture() const;

  bool         hasAlignment() const;
  void         setAlignment( Vector const& translation, Vector const& rotation);  
  boost::tuple<Vector,Vector> getAlignment() const;

  void          enableEdges(bool usedge, bool dsedge);
  bool      hasUpstreamEdge() const;
  bool    hasDownstreamEdge() const;

  // Methods to set alignment without 
  // (a little overkill, but so what?)

  bool        alignRelX( double const&  meters );
  bool        alignRelY( double const&  meters );
  bool        alignAbsX( double const&  meters );
  bool        alignAbsY( double const&  meters );
  bool      alignRelXmm( double const&  mm     );
  bool      alignRelYmm( double const&  mm     );
  bool      alignAbsXmm( double const&  mm     );
  bool      alignAbsYmm( double const&  mm     );
  bool     alignRelRoll( double const& radians );
  bool     alignAbsRoll( double const& radians );
  bool alignRelRollmrad( double const& mrad    );
  bool alignAbsRollmrad( double const& mrad    );
  bool     setAlignment( alignmentData const& );

  bool    hasMoved()  const  { return ( pinnedFrames_.altered() || align_  ); }
  void    realign();  // Resets element to its original position.
  void    markPins(); // WRITE // Marks the current positions of the element as "original"
  void    loadPinnedCoordinates( Particle const &, Vector&, double=1.0 ) const; 

    
  // Pinned coordinates of the particle are returned in the vector
  //   argument.  
  // 
  // The third argument must be within [0,1]; it is internally
  //   set to 1.0 otherwise.  It indicates the percentage of distance
  //   from the upstream to downstream faces of the element at which 
  //   the coordinates are to be calculated.
  // 
  // Default value: 1.0 -> return pinned coordinates at the downstream end.

  Frame const& getUpstreamPinnedFrame() const 
    { return pinnedFrames_.upStream(); }

  Frame const& getDownstreamPinnedFrame() const 
    { return pinnedFrames_.downStream(); }


  void propagate(         Particle& p ) const;
  void propagate(      JetParticle& p ) const;
  void propagate(    ParticleBunch& b ) const;
  void propagate( JetParticleBunch& b ) const;

  void localPropagate(         Particle& p ) const;
  void localPropagate(      JetParticle& p ) const;
  void localPropagate(    ParticleBunch& b ) const;
  void localPropagate( JetParticleBunch& b ) const;

  void enterLocalFrame( Particle&                )   const;
  void enterLocalFrame( JetParticle&             )   const;

  void leaveLocalFrame( Particle&                )   const;
  void leaveLocalFrame( JetParticle&             )   const;

  // Editing functions

  virtual std::pair<ElmPtr,ElmPtr>  split( double const& ) const;
                                   // Splits the element at percent orbitlength from the in-face.

  // ... Tagging methods

  void              setTag(  std::string const& tag){ tag_   = tag; }           
  std::string       getTag()           const        { return tag_;  }  


  //  ... Modifiers

  void  setLength   ( double const& );
  void  setStrength ( double const& );

  void  rename        ( std::string const& name );  


  virtual double getReferenceTime()                    const;     // returns ctRef_
  virtual void   setReferenceTime( double const& );               

  // ... Query functions 

  alignmentData       Alignment( void )        const;

  double              Strength()               const;
  virtual double      Length()                 const;

  virtual double OrbitLength( Particle const& ) const;  // Returns length of design orbit
                                                        // segment through the element.
                                                        // Will be different from "length"
                                                        // for rbends.

  virtual bool isSimple()         const;
  virtual bool hasParallelFaces() const;
  virtual bool hasStandardFaces() const;
  virtual bool     isBeamline()   const;                   
  virtual bool   isDriftSpace()   const = 0;
  virtual bool       isMagnet()   const = 0;
  virtual bool      isPassive()   const = 0;                   
  virtual bool         isThin()   const = 0;                   


  char const*                 Name()   const;
  virtual char const*         Type()   const = 0;


private:

  double   strengthScale()          const;
  void     setStrengthScale( double const&);

  void     init_internals(BmlPtr const& bml, ElmPtr const& elm); 

  double                             strength_;           // Interpretation depends on object.
  double                             pscale_;             // momentum scaling factor ( for a linac, pscale_ != 1.0 )     

protected:

  virtual void  propagateReference( Particle& p, double initialBhro, bool scaling );

  std::string                        ident_;              // Name identifier of the element.

  double                             length_;             // Length of object [ meters ]

  alignment*                         align_;

  PinnedFrameSet                     pinnedFrames_;

  mutable double                     ctRef_;              // (normalized) time required for
                                                         // a reference particle to cross
                                                         // the element. Normally, established by a RefRegVisitor.

  aperture_t                         aperture_;          // aperture type: elliptical, rectangular
  double                             aperturex_;         // hor aperture 1/2 width;  < 0 ==> ignored  
  double                             aperturey_;         // ver aperture 1/2 width;  < 0 ==> ignored  


  std::map<std::string, boost::any>  attributes_;

  std::string                        tag_;             // FIXME !!! OBSOLETE


  BmlPtr                             bml_;             // The element may be composite.
  ElmPtr                             elm_;             // with one active part.

    
  bool                               usedge_;
  bool                               dsedge_;

  PropagatorPtr                      propagator_;



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


  /* All the work is done in friend ostream& operator<<(),
     placeholder for if descendants want to do somthing. */

  virtual std::ostream&    writeTo(std::ostream      & os) {return os;}     
  virtual std::istream&   readFrom(std::istream      & is) {return is;}

  friend  std::ostream& operator<<(std::ostream&, bmlnElmnt&);

  friend bmlnElmnt* read_istream(std::istream&);

};

class bmlnElmnt::core_access {

 public:

  static ElmPtr const&  get_ElmPtr( bmlnElmnt const& o) { return o.elm_;   }
  static BmlPtr const&  get_BmlPtr( bmlnElmnt const& o) { return o.bml_;   }
  static double const&  get_ctRef ( bmlnElmnt const& o) { return o.ctRef_; }
  static ElmPtr&        get_ElmPtr( bmlnElmnt& o)       { return o.elm_;   }
  static BmlPtr&        get_BmlPtr( bmlnElmnt& o)       { return o.bml_;   }
  static double&        get_ctRef ( bmlnElmnt& o)       { return o.ctRef_; }

};
   

#endif // BMLNELMNT_H
